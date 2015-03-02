#include "sched_thread.h"
#include "logger.h"
#include "ioevent_loop.h"

/* ��������Ϊcount����׼���õ�IO�¼� */
static void deal_ioevents(IOEventPoller *ioevent, const int count)
{
	int i;
	int event;
	IOEventEntry *pEntry;
	for (i=0; i<count; i++)
	{
		/* ȡ��i���Ѿ������ļ����������¼� */
		event = IOEVENT_GET_EVENTS(ioevent, i);

		/* ȡIO�¼���Ӧ������ */
		pEntry = (IOEventEntry *)IOEVENT_GET_DATA(ioevent, i);

		/* ����֮ǰע��Ļص���������˴�IO�¼� */
		pEntry->callback(pEntry->fd, event, pEntry->timer.data);
	}
}

/* ����headָ��������й����¼���ɵ�˫������ */
static void deal_timeouts(FastTimerEntry *head)
{
	FastTimerEntry *entry;
	FastTimerEntry *curent;
	IOEventEntry *pEventEntry;

	entry = head->next;
	while (entry != NULL)
	{
		curent = entry;
		entry = entry->next;

		/* ����ÿһ���ڵ㣬���ûص�������������¼� */
		pEventEntry = (IOEventEntry *)curent->data;
		if (pEventEntry != NULL)
		{
			pEventEntry->callback(pEventEntry->fd, IOEVENT_TIMEOUT,
						curent->data);
		}
	}
}

/* 
 * ѭ���ȴ�IO�¼����������ûص��������д��� 
 * pThreadData���߳��Լ�������
 * recv_notify_callback��ÿ���̵߳Ĺܵ�pipe_fds[0]��READ�¼��Ļص�����
 * clean_up_callback����Ҫɾ����IO�¼��Ļص�����
 * continue_flag ָ��һ��volatile ��bool���������Ϊfalse����ֹͣѭ��
 */
int ioevent_loop(struct nio_thread_data *pThreadData,
	IOEventCallback recv_notify_callback, TaskCleanUpCallBack
	clean_up_callback, volatile bool *continue_flag)
{
	int result;
	IOEventEntry ev_notify;
	FastTimerEntry head;
	struct fast_task_info *pTask;
	time_t last_check_time;
	int count;

	memset(&ev_notify, 0, sizeof(ev_notify));

	/* ÿ�������̶߳�Ӧ�Ĺܵ��Ķ��ļ������� */
	ev_notify.fd = pThreadData->pipe_fds[0];
	/* ��pipe[0]�ɶ�ʱ������recv_notify_callback���д������½������ӵ�socket������ */
	ev_notify.callback = recv_notify_callback;

	/* ���̶߳�Ӧ�Ĺܵ��Ķ��ļ����������뵽���������� */
	if (ioevent_attach(&pThreadData->ev_puller,
		pThreadData->pipe_fds[0], IOEVENT_READ,
		&ev_notify) != 0)
	{
		result = errno != 0 ? errno : ENOMEM;
		logCrit("file: "__FILE__", line: %d, " \
			"ioevent_attach fail, " \
			"errno: %d, error info: %s", \
			__LINE__, result, STRERROR(result));
		return result;
	}

	/* ��ʼ�����һ�μ��ʱ�� */
	last_check_time = g_current_time;
	while (*continue_flag)
	{
		pThreadData->deleted_list = NULL;
		/* ����ֱ����IO�¼�����������ʱʱ����ev_puller��timeout�������� */
		count = ioevent_poll(&pThreadData->ev_puller);
		/* countֵΪ��׼���õ��ļ��������������������0��ʾ��ʱ��<0��ʾ���� */
		if (count > 0)
		{
			/* ����IO�¼� */
			deal_ioevents(&pThreadData->ev_puller, count);
		}
		else if (count < 0)
		{
			result = errno != 0 ? errno : EINVAL;
			if (result != EINTR)
			{
				logError("file: "__FILE__", line: %d, " \
					"ioevent_poll fail, " \
					"errno: %d, error info: %s", \
					__LINE__, result, STRERROR(result));
				return result;
			}
		}

		/* �������Ҫɾ����IO�¼� */
		if (pThreadData->deleted_list != NULL)
		{
			count = 0;
			while (pThreadData->deleted_list != NULL)
			{
				pTask = pThreadData->deleted_list;
				pThreadData->deleted_list = pTask->next;

				/* ����clean_up_callback�ص���������Ҫɾ����IO�¼� */
				clean_up_callback(pTask);
				count++;
			}
			logInfo("cleanup task count: %d", count);
		}

		if (g_current_time - last_check_time > 0)
		{
			/* �������һ�μ��ʱ�� */
			last_check_time = g_current_time;
			/*
			 * ��ȡ���й���ʱ����timer->current_time��g_current_time�Ľڵ�
			 * ���еĹ��ڽڵ����ջ�����head��ָ���˫��������
			 */
			count = fast_timer_timeouts_get(
				&pThreadData->timer, g_current_time, &head);

			if (count > 0)
			{
				/* ����headָ��������й����¼���ɵ�˫������ */
				deal_timeouts(&head);
			}
		}
	}

	return 0;
}

/* 
 * ��sock���������뵽����������
 * �ȴ�eventָ���¼�������ִ��callback�ص�����
 * timeoutʱ�����ڣ�����pThread�߳������е�timer��ʱ�ֶ����������Ƿ���ڵ��ж�
 */
int ioevent_set(struct fast_task_info *pTask, struct nio_thread_data *pThread,
	int sock, short event, IOEventCallback callback, const int timeout)
{
	int result;

	/* �߳����� */
	pTask->thread_data = pThread;
	/* IO�¼�������socket������ */
	pTask->event.fd = sock;
	/* IO�¼�������Ļص����� */
	pTask->event.callback = callback;

	/* ����socket���������뵽�������ϣ��ȴ��¼����� */
	if (ioevent_attach(&pThread->ev_puller,
		sock, event, pTask) < 0)
	{
		result = errno != 0 ? errno : ENOENT;
		logError("file: "__FILE__", line: %d, " \
			"ioevent_attach fail, " \
			"errno: %d, error info: %s", \
			__LINE__, result, STRERROR(result));
		return result;
	}

	/* ����ʱ���ֵĽڵ��д�ŵ��������� */
	pTask->event.timer.data = pTask;
	/* ���õ�ǰ�¼��Ĺ���ʱ�� */
	pTask->event.timer.expires = g_current_time + timeout;

	/* �����¼����뵽ʱ������ */
	result = fast_timer_add(&pThread->timer, &pTask->event.timer);
	if (result != 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"fast_timer_add fail, " \
			"errno: %d, error info: %s", \
			__LINE__, result, STRERROR(result));
		return result;
	}

	return 0;
}

