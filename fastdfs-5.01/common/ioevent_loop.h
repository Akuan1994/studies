#ifndef _IOEVENT_LOOP_H
#define _IOEVENT_LOOP_H

#include "fast_task_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * ѭ���ȴ�IO�¼����������ûص��������д��� 
 * pThreadData���߳��Լ�������
 * recv_notify_callback��ÿ���̵߳Ĺܵ�pipe_fds[0]��READ�¼��Ļص�����
 * clean_up_callback����Ҫɾ����IO�¼��Ļص�����
 * continue_flag ָ��һ��volatile ��bool���������Ϊfalse����ֹͣѭ��
 */
int ioevent_loop(struct nio_thread_data *pThreadData,
	IOEventCallback recv_notify_callback, TaskCleanUpCallBack
	clean_up_callback, volatile bool *continue_flag);

/* 
 * ��sock���������뵽����������
 * �ȴ�eventָ���¼�������ִ��callback�ص�����
 * timeoutʱ�����ڣ�����pThread�߳������е�timer��ʱ�ֶ����������Ƿ���ڵ��ж�
 */
int ioevent_set(struct fast_task_info *pTask, struct nio_thread_data *pThread,
	int sock, short event, IOEventCallback callback, const int timeout);

#ifdef __cplusplus
}
#endif

#endif

