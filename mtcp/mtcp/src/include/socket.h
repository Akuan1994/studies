#ifndef __SOCKET_H_
#define __SOCKET_H_

#include "mtcp_api.h"
#include "mtcp_epoll.h"

/*----------------------------------------------------------------------------*/
// socketѡ��
enum socket_opts
{
	MTCP_NONBLOCK		= 0x01,
	MTCP_ADDR_BIND		= 0x02, 
};
/*----------------------------------------------------------------------------*/
// socket����
struct socket_map
{
	int id;						// �൱��fd
	int socktype;				// socket����
	uint32_t opts;				// socketѡ��

	struct sockaddr_in saddr;	// socket�󶨵ı��ص�ַ

	union {
		struct tcp_stream *stream;
		struct tcp_listener *listener;
		struct mtcp_epoll *ep;
		struct pipe *pp;
	};		// ����socktype��ͬ�����ӵ���ͬ�Ĺ����������epoll�Ļ�����mtcp_eppoll����

	uint32_t epoll;			/* registered events */		// �û�ע���epoll�¼�
	uint32_t events;		/* available events */		// ��Ч���¼�
	mtcp_epoll_data_t ep_data;							// ��Ӧ��event_data

	TAILQ_ENTRY (socket_map) free_smap_link;

};
/*----------------------------------------------------------------------------*/
typedef struct socket_map * socket_map_t;
/*----------------------------------------------------------------------------*/
socket_map_t 
AllocateSocket(mctx_t mctx, int socktype, int need_lock);
/*----------------------------------------------------------------------------*/
void 
FreeSocket(mctx_t mctx, int sockid, int need_lock); 
/*----------------------------------------------------------------------------*/
socket_map_t 
GetSocket(mctx_t mctx, int sockid);
/*----------------------------------------------------------------------------*/
// tcp���Ӷ���
struct tcp_listener
{
	int sockid;					// �����׽��ֵ�fd
	socket_map_t socket;		// socket����

	int backlog;				// backlog��
	stream_queue_t acceptq;		// accept����
	
	pthread_mutex_t accept_lock;
	pthread_cond_t accept_cond;
};
/*----------------------------------------------------------------------------*/

#endif /* __SOCKET_H_ */
