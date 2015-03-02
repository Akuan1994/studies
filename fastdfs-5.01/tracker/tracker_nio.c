/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "shared_func.h"
#include "sched_thread.h"
#include "fdfs_global.h"
#include "logger.h"
#include "sockopt.h"
#include "fast_task_queue.h"
#include "tracker_types.h"
#include "tracker_proto.h"
#include "tracker_mem.h"
#include "tracker_global.h"
#include "tracker_service.h"
#include "ioevent_loop.h"
#include "tracker_nio.h"

static void client_sock_read(int sock, short event, void *arg);
static void client_sock_write(int sock, short event, void *arg);

/* ���������������� */
void task_finish_clean_up(struct fast_task_info *pTask)
{
	TrackerClientInfo *pClientInfo;

	pClientInfo = (TrackerClientInfo *)pTask->arg;

	/* �����ע��ص�������������Ӧ�ص����� */
	if (pTask->finish_callback != NULL)
	{
		pTask->finish_callback(pTask);
		pTask->finish_callback = NULL;
	}

	if (pClientInfo->pGroup != NULL)
	{
		if (pClientInfo->pStorage != NULL)
		{
			/* ��ָ����storage��Ϊoffline״̬ */
			tracker_mem_offline_store_server(pClientInfo->pGroup, \
						pClientInfo->pStorage);
		}
	}

	/* ���������IO�¼�������ɾ�� */
	ioevent_detach(&pTask->thread_data->ev_puller, pTask->event.fd);
	/* �ر�socket */
	close(pTask->event.fd);

	pTask->event.fd = -1;

	/* ��������˳�ʱʱ�䣬��ʱ������ɾ�����¼� */
	if (pTask->event.timer.expires > 0)
	{
		fast_timer_remove(&pTask->thread_data->timer,
			&pTask->event.timer);
		pTask->event.timer.expires = 0;
	}

	/* ��ʼ��pTask->arg���� */
	memset(pTask->arg, 0, sizeof(TrackerClientInfo));
	/* ��������ʹ�õ�pTask������뵽�ڴ�صĶ����� */
	free_queue_push(pTask);
}

/* 
 * ÿ���̵߳Ĺܵ�pipe_fds[0]��READ�¼��Ļص����� 
 * ÿ�ζ�ȡһ��int���������½������ӵ�socket��������֮����뵽IO�¼�����
 * ��ͨ���ַ�����Ӧ�����߳��еȴ��ɶ��¼����������client_sock_read�������д���
 */
void recv_notify_read(int sock, short event, void *arg)
{
	int bytes;
	int incomesock;
	struct nio_thread_data *pThreadData;
	struct fast_task_info *pTask;
	char szClientIp[IP_ADDRESS_SIZE];		/* ip��ַ���ַ������� */
	in_addr_t client_addr;		/* ip��ַ */

	while (1)
	{
		/* ��ȡ�½������ӵ�socket������ */
		if ((bytes=read(sock, &incomesock, sizeof(incomesock))) < 0)
		{
			/* ��VxWorks��Windows�ϣ�EAGAIN�����ֽ���EWOULDBLOCK����ʾû�����ݿɶ� */
			if (!(errno == EAGAIN || errno == EWOULDBLOCK))
			{
				logError("file: "__FILE__", line: %d, " \
					"call read failed, " \
					"errno: %d, error info: %s", \
					__LINE__, errno, STRERROR(errno));
			}

			break;
		}
		/* read����0��ʾ�Ѿ����꣬�޿ɶ����� */
		else if (bytes == 0)
		{
			break;
		}

		/* �����socket������С��0�����ԣ�ֱ�ӷ��� */
		if (incomesock < 0)
		{
			return;
		}

		/* ����socket��������ȡip��ַ */
		client_addr = getPeerIpaddr(incomesock, \
				szClientIp, IP_ADDRESS_SIZE);
		if (g_allow_ip_count >= 0)
		{
			/* 
			 * ��g_allow_ip_addrs�����в��Ҵ�ip��ַ
			 * ���û���ҵ������������ӣ��ر�socket������ 
			 */
			if (bsearch(&client_addr, g_allow_ip_addrs, \
					g_allow_ip_count, sizeof(in_addr_t), \
					cmp_by_ip_addr_t) == NULL)
			{
				logError("file: "__FILE__", line: %d, " \
					"ip addr %s is not allowed to access", \
					__LINE__, szClientIp);

				close(incomesock);
				continue;
			}
		}

		/* ����socketΪ������ģʽ */
		if (tcpsetnonblockopt(incomesock) != 0)
		{
			close(incomesock);
			continue;
		}

		/* ���ڴ�صĶ����л�ȡһ��fast_task_info����ռ� */
		pTask = free_queue_pop();
		if (pTask == NULL)
		{
			logError("file: "__FILE__", line: %d, " \
				"malloc task buff failed, you should " \
				"increase the parameter: max_connections", \
				__LINE__);
			close(incomesock);
			continue;
		}

		strcpy(pTask->client_ip, szClientIp);

		/* ����socket��������ͨ���ַ������������߳���ȥ���� */
		pThreadData = g_thread_data + incomesock % g_work_threads;
		/* 
		 * ��incomesock���������뵽���������У��ȴ�����READ�¼� 
		 * incomesock���ڿɶ�״̬�󣬻����client_sock_read�ص��������д���
		 */
		if (ioevent_set(pTask, pThreadData, incomesock, IOEVENT_READ,
			client_sock_read, g_fdfs_network_timeout) != 0)
		{
			/* ���������������� */
			task_finish_clean_up(pTask);
			continue;
		}
	}
}

/* ��IO�¼��Ĵ���������Ϊsocket��д */
static int set_send_event(struct fast_task_info *pTask)
{
	int result;

	if (pTask->event.callback == client_sock_write)
	{
		return 0;
	}

	pTask->event.callback = client_sock_write;
	if (ioevent_modify(&pTask->thread_data->ev_puller,
		pTask->event.fd, IOEVENT_WRITE, pTask) != 0)
	{
		result = errno != 0 ? errno : ENOENT;
		task_finish_clean_up(pTask);

		logError("file: "__FILE__", line: %d, "\
			"ioevent_modify fail, " \
			"errno: %d, error info: %s", \
			__LINE__, result, STRERROR(result));
		return result;
	}
	return 0;
}

/* �����ͱ��ĵ��¼����뵽IO�¼������� */
int send_add_event(struct fast_task_info *pTask)
{
	pTask->offset = 0;

	/* direct send */
	/* ����ֱ����pTask->event.fd���ͱ��� */
	client_sock_write(pTask->event.fd, IOEVENT_WRITE, pTask);
	return 0;
}

/* ��ȡ�ӿͻ��˷��͹�������Ϣ */
static void client_sock_read(int sock, short event, void *arg)
{
	int bytes;
	int recv_bytes;
	struct fast_task_info *pTask;

	pTask = (struct fast_task_info *)arg;

	/* �������Ҫ�����¼��ĳ�ʱʱ�� */
	if (event & IOEVENT_TIMEOUT)
	{
		if (pTask->offset == 0 && pTask->req_count > 0)
		{
			/* �����¼���ʱʱ�� */
			pTask->event.timer.expires = g_current_time +
				g_fdfs_network_timeout;
			/* �����¼�����ʱ������ */
			fast_timer_add(&pTask->thread_data->timer,
				&pTask->event.timer);
		}
		else
		{
			logError("file: "__FILE__", line: %d, " \
				"client ip: %s, recv timeout, " \
				"recv offset: %d, expect length: %d", \
				__LINE__, pTask->client_ip, \
				pTask->offset, pTask->length);

			task_finish_clean_up(pTask);
		}

		return;
	}

	/* ����ǳ����¼� */
	if (event & IOEVENT_ERROR)
	{
		logError("file: "__FILE__", line: %d, " \
			"client ip: %s, recv error event: %d, "
			"close connection", __LINE__, pTask->client_ip, event);

		task_finish_clean_up(pTask);
		return;
	}

	while (1)
	{
		/* ���³�ʱʱ�� */
		fast_timer_modify(&pTask->thread_data->timer,
			&pTask->event.timer, g_current_time +
			g_fdfs_network_timeout);

		/* ��ȡ����ͷ */
		if (pTask->length == 0) //recv header
		{
			/* ֮ǰ�п����Ѿ���ȡ�˲������ݣ��ٶ�ȡ��ʱ���ȥƫ���� */
			recv_bytes = sizeof(TrackerHeader) - pTask->offset;
		}
		/* ��ȡ������ */
		else
		{
			recv_bytes = pTask->length - pTask->offset;
		}

		/* ����ȡ���ı���ͷ�ͱ��������pTask->data�� */
		bytes = recv(sock, pTask->data + pTask->offset, recv_bytes, 0);
		/* û�ж�ȡ������ */
		if (bytes < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
			}
			else
			{
				logError("file: "__FILE__", line: %d, " \
					"client ip: %s, recv failed, " \
					"errno: %d, error info: %s", \
					__LINE__, pTask->client_ip, \
					errno, STRERROR(errno));

				task_finish_clean_up(pTask);
			}

			return;
		}
		/* ���������ж� */
		else if (bytes == 0)
		{
			logDebug("file: "__FILE__", line: %d, " \
				"client ip: %s, recv failed, " \
				"connection disconnected.", \
				__LINE__, pTask->client_ip);

			task_finish_clean_up(pTask);
			return;
		}

		/* ��������ͷ */
		if (pTask->length == 0) //header
		{
			/* ����Ѷ�ȡ�����ݼ����¶��������ݳ��Ȳ��㣬ֱ�ӷ��� */
			if (pTask->offset + bytes < sizeof(TrackerHeader))
			{
				pTask->offset += bytes;
				return;
			}

			/* ��ȡ�����峤�� */
			/* �����������д�����ַ�����ԭΪ64λ���� */
			pTask->length = buff2long(((TrackerHeader *) \
						pTask->data)->pkg_len);
			/* ���Ƚ������� */
			if (pTask->length < 0)
			{
				logError("file: "__FILE__", line: %d, " \
					"client ip: %s, pkg length: %d < 0", \
					__LINE__, pTask->client_ip, \
					pTask->length);

				task_finish_clean_up(pTask);
				return;
			}

			/* �ܳ���Ϊ����ͷ���ȼ��ϱ����峤�� */
			pTask->length += sizeof(TrackerHeader);
			/* ����ܳ��ȳ�����������ȣ����������� */
			if (pTask->length > TRACKER_MAX_PACKAGE_SIZE)
			{
				logError("file: "__FILE__", line: %d, " \
					"client ip: %s, pkg length: %d > " \
					"max pkg size: %d", __LINE__, \
					pTask->client_ip, pTask->length, \
					TRACKER_MAX_PACKAGE_SIZE);

				task_finish_clean_up(pTask);
				return;
			}
		}

		/* ����ƫ���� */
		pTask->offset += bytes;
		if (pTask->offset >= pTask->length) //recv done
		{
			/* ���������� */
			pTask->req_count++;
			/* �����tracker������ */
			tracker_deal_task(pTask);
			return;
		}
	}

	return;
}

/* ��sock���ͱ��� */
static void client_sock_write(int sock, short event, void *arg)
{
	int bytes;
	int result;
	struct fast_task_info *pTask;

	pTask = (struct fast_task_info *)arg;
	/* ����¼���ʱ��ֱ�ӽ����������� */
	if (event & IOEVENT_TIMEOUT)
	{
		logError("file: "__FILE__", line: %d, " \
			"send timeout", __LINE__);

		task_finish_clean_up(pTask);

		return;
	}

	/* ��������ʧ�� */
	if (event & IOEVENT_ERROR)
	{
		logError("file: "__FILE__", line: %d, " \
			"client ip: %s, recv error event: %d, "
			"close connection", __LINE__, pTask->client_ip, event);

		task_finish_clean_up(pTask);
		return;
	}

	while (1)
	{
		/* �޸�ָ���ڵ�Ĺ���ʱ�� */
		fast_timer_modify(&pTask->thread_data->timer,
			&pTask->event.timer, g_current_time +
			g_fdfs_network_timeout);

		bytes = send(sock, pTask->data + pTask->offset, \
				pTask->length - pTask->offset,  0);
		//printf("%08X sended %d bytes\n", (int)pTask, bytes);
		/* ���û�з��ͳɹ� */
		if (bytes < 0)
		{	
			/* ��������IO�¼������У��ȴ����·��� */
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				/* ��IO�¼��Ĵ���������Ϊsocket��д */
				set_send_event(pTask);
			}
			else
			{
				logError("file: "__FILE__", line: %d, " \
					"client ip: %s, recv failed, " \
					"errno: %d, error info: %s", \
					__LINE__, pTask->client_ip, \
					errno, STRERROR(errno));

				task_finish_clean_up(pTask);
			}

			return;
		}
		/* ���ӶϿ� */
		else if (bytes == 0)
		{
			logWarning("file: "__FILE__", line: %d, " \
				"send failed, connection disconnected.", \
				__LINE__);

			task_finish_clean_up(pTask);
			return;
		}

		/* �����˲����ֽڣ��޸�ƫ�������´μ�����ƫ��������ʼ���� */
		pTask->offset += bytes;
		/* ���������� */
		if (pTask->offset >= pTask->length)
		{
			if (pTask->length == sizeof(TrackerHeader) && \
				((TrackerHeader *)pTask->data)->status == EINVAL)
			{
				logDebug("file: "__FILE__", line: %d, "\
					"close conn: #%d, client ip: %s", \
					__LINE__, pTask->event.fd,
					pTask->client_ip);
				task_finish_clean_up(pTask);
				return;
			}

			pTask->offset = 0;
			pTask->length  = 0;

			/* ��IO�¼��Ĵ���������Ϊsocket�ɶ� */
			pTask->event.callback = client_sock_read;
			if (ioevent_modify(&pTask->thread_data->ev_puller,
				pTask->event.fd, IOEVENT_READ, pTask) != 0)
			{
				result = errno != 0 ? errno : ENOENT;
				task_finish_clean_up(pTask);

				logError("file: "__FILE__", line: %d, "\
					"ioevent_modify fail, " \
					"errno: %d, error info: %s", \
					__LINE__, result, STRERROR(result));
				return;
			}

			return;
		}
	}
}

