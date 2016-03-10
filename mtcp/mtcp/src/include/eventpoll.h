#ifndef __EVENTPOLL_H_
#define __EVENTPOLL_H_

#include "mtcp_api.h"
#include "mtcp_epoll.h"

/*----------------------------------------------------------------------------*/
// mtcp-epoll״̬��Ϣ
struct mtcp_epoll_stat
{
	uint64_t calls;
	uint64_t waits;				// wait����
	uint64_t wakes;				// epoll_wait�����Ѵ���

	uint64_t issued;
	uint64_t registered;		// ��ע���¼���
	uint64_t invalidated;		// �����������Ч�¼���
	uint64_t handled;			// �¼���������
};
/*----------------------------------------------------------------------------*/
// �¼���������socket_id
struct mtcp_epoll_event_int
{
	struct mtcp_epoll_event ev;
	int sockid;
};
/*----------------------------------------------------------------------------*/
// �¼����е�����
enum event_queue_type
{
	USR_EVENT_QUEUE = 0, 
	USR_SHADOW_EVENT_QUEUE = 1, 
	MTCP_EVENT_QUEUE = 2
};
/*----------------------------------------------------------------------------*/
// �¼�����
struct event_queue
{
	struct mtcp_epoll_event_int *events;		// �����¼�
	int start;			// starting index		// ��ʼevent�����
	int end;			// ending index			// ���һ��event�����
	
	int size;			// max size				// ��󳤶�
	int num_events;		// number of events		// ��ǰ�¼���
};
/*----------------------------------------------------------------------------*/
// mtcp��epoll�ܶ���
struct mtcp_epoll
{
	struct event_queue *usr_queue;			// ��mtcp�����ƹ������Ѿ������˵��¼�
	struct event_queue *usr_shadow_queue;	// �û����õ���Ҫ�������¼�
	struct event_queue *mtcp_queue;			// ��Ŵ����˵��¼�������������յ����ݰ�

	uint8_t waiting;						// ����epoll_wait״̬�ı�־
	struct mtcp_epoll_stat stat;			// ״̬��Ϣ
	
	pthread_cond_t epoll_cond;				// ����������epoll_wait����pthread_cond_waitʵ�������������¼�����ʱ����pthread_cond_signal����
	pthread_mutex_t epoll_lock;				// �߳���
};
/*----------------------------------------------------------------------------*/

int 
CloseEpollSocket(mctx_t mctx, int epid);

#endif /* __EVENTPOLL_H_ */
