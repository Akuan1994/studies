#ifndef __IOEVENT_H__
#define __IOEVENT_H__

#include <stdint.h>
#include <poll.h>
#include <sys/time.h>

#define IOEVENT_TIMEOUT  0x8000

#if IOEVENT_USE_EPOLL
#include <sys/epoll.h>
/* epoll������ʽ��ET��LT���֣�ET�Ǹ��ٷ�ʽ�������ظ�֪ͨ��Ĭ��ΪLT */
#define IOEVENT_EDGE_TRIGGER EPOLLET

#define IOEVENT_READ  EPOLLIN
#define IOEVENT_WRITE EPOLLOUT
#define IOEVENT_ERROR (EPOLLERR | EPOLLPRI | EPOLLHUP)

#elif IOEVENT_USE_KQUEUE
#include <sys/event.h>
#define IOEVENT_EDGE_TRIGGER EV_CLEAR

#define KPOLLIN    0x001
#define KPOLLPRI   0x002
#define KPOLLOUT   0x004
#define KPOLLERR   0x010
#define KPOLLHUP   0x020
#define IOEVENT_READ  KPOLLIN
#define IOEVENT_WRITE KPOLLOUT
#define IOEVENT_ERROR (KPOLLHUP | KPOLLPRI | KPOLLHUP)

#ifdef __cplusplus
extern "C" {
#endif

int kqueue_ev_convert(int16_t event, uint16_t flags);

#ifdef __cplusplus
}
#endif

#elif IOEVENT_USE_PORT
#include <port.h>
#define IOEVENT_EDGE_TRIGGER 0

#define IOEVENT_READ  POLLIN
#define IOEVENT_WRITE POLLOUT
#define IOEVENT_ERROR (POLLERR | POLLPRI | POLLHUP)
#endif

typedef struct ioevent_puller {
    int size;  //max events (fd)
    int extra_events;
    int poll_fd;	/* ����epoll�����ռ��һ���ļ������� */

#if IOEVENT_USE_EPOLL
    struct epoll_event *events;	/* events���� */
    int timeout;	/* epoll_wait�ĳ�ʱʱ�� */
#elif IOEVENT_USE_KQUEUE
    struct kevent *events;
    struct timespec timeout;
#elif IOEVENT_USE_PORT
    port_event_t *events;
    timespec_t timeout;
#endif
} IOEventPoller;	/* ioevent���Ƚṹ�� */

/* ��ȡioevent�е�events��������ʹ��epollʱ��EPOLLIN����EPOLLOUT�� */
#if IOEVENT_USE_EPOLL
  #define IOEVENT_GET_EVENTS(ioevent, index) \
      ioevent->events[index].events
#elif IOEVENT_USE_KQUEUE
  #define IOEVENT_GET_EVENTS(ioevent, index)  kqueue_ev_convert( \
      ioevent->events[index].filter, ioevent->events[index].flags)
#elif IOEVENT_USE_PORT
  #define IOEVENT_GET_EVENTS(ioevent, index) \
      ioevent->events[index].portev_events
#else
#error port me
#endif

/* ��ȡioevent�е�data */
#if IOEVENT_USE_EPOLL
  #define IOEVENT_GET_DATA(ioevent, index)  \
      ioevent->events[index].data.ptr
#elif IOEVENT_USE_KQUEUE
  #define IOEVENT_GET_DATA(ioevent, index)  \
      ioevent->events[index].udata
#elif IOEVENT_USE_PORT
  #define IOEVENT_GET_DATA(ioevent, index)  \
      ioevent->events[index].portev_user
#else
#error port me
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ��ʼ��ioevent������events��������ʱʱ��� */
int ioevent_init(IOEventPoller *ioevent, const int size,
    const int timeout, const int extra_events);

/* ����ioevent�����Դ */
void ioevent_destroy(IOEventPoller *ioevent);

/* ��events���������event */
int ioevent_attach(IOEventPoller *ioevent, const int fd, const int e,
    void *data);

/* �޸���ע���fd�ļ����¼� */
int ioevent_modify(IOEventPoller *ioevent, const int fd, const int e,
    void *data);

/* ��events������ɾ��ָ��event */
int ioevent_detach(IOEventPoller *ioevent, const int fd);

/* ����epoll_wait�ȣ��ȴ�ָ���ļ��ɶ����д */
int ioevent_poll(IOEventPoller *ioevent);

#ifdef __cplusplus
}
#endif

#endif

