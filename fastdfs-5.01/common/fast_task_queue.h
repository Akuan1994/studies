/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//fast_task_queue.h

/* ������� */

#ifndef _FAST_TASK_QUEUE_H
#define _FAST_TASK_QUEUE_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "common_define.h"
#include "ioevent.h"
#include "fast_timer.h"

struct fast_task_info;

/* ��������Ļص����� */
typedef int (*TaskFinishCallBack) (struct fast_task_info *pTask);

typedef void (*TaskCleanUpCallBack) (struct fast_task_info *pTask);

/* IO�¼��Ļص����� */
typedef void (*IOEventCallback) (int sock, short event, void *arg);

typedef struct ioevent_entry
{
	int fd;						/* ��Ӧ�ļ������� */
	FastTimerEntry timer;			/* ʱ������ĳ��slot�е�һ���ڵ㣬data��ϢҲ��������� */
	IOEventCallback callback;		/* IO�¼�������Ļص����� */
} IOEventEntry;	/* IO�¼��ڵ� */

struct nio_thread_data		/* �����߳����� */
{
	struct ioevent_puller ev_puller;		/* IOEvent���ȶ��� */
	struct fast_timer timer;			/* ʱ���ֶ��� */
        int pipe_fds[2];
	struct fast_task_info *deleted_list;
};

struct fast_task_info	/* ������Ϣ�ṹ��ÿһ�����Ӷ�Ӧһ�� */
{
	IOEventEntry event;				/* IO�¼��ڵ� */
	char client_ip[IP_ADDRESS_SIZE];		/* �����еĿͻ���ip��ַ */
	void *arg;  //extra argument pointer		/* ����������� */
	char *data; //buffer for write or recv
	int size;   //alloc size					/* data�������ռ�Ĵ�С */
	int length; //data length					/* ������ĳ��� */
	int offset; //current offset				/* ��ȡsocketʱ���Ѷ������ݵ�ƫ���� */
	int req_count; //request count
	TaskFinishCallBack finish_callback;		/* ��������Ļص����� */
	struct nio_thread_data *thread_data;	/* �߳�����ָ�룬����epoll�ȵķ�װ���Լ�ʱ���ֶ��� */
	struct fast_task_info *next;	
};

struct fast_task_queue		/* ������� */
{
	struct fast_task_info *head;	/* �������ͷָ�� */
	struct fast_task_info *tail;		/* �������βָ�� */
	pthread_mutex_t lock;			/* ������ */
	int max_connections;			/* ���������(������󳤶�) */
	int min_buff_size;			/* data�ֶεĳ��� */
	int max_buff_size;
	int arg_size;					/* ��������Ĵ�С */
	bool malloc_whole_block;		/* �Ƿ�Ϊdata������ͬһ��blockһ������ڴ� */
};

#ifdef __cplusplus
extern "C" {
#endif

/* ���ӳ�(�ڴ�ض���)�ĳ�ʼ�� */
int free_queue_init(const int max_connections, const int min_buff_size, \
		const int max_buff_size, const int arg_size);

/* �����ڴ�ض��� */
void free_queue_destroy();

/* ���ڴ�ض���ͷ�����ڵ� */
int free_queue_push(struct fast_task_info *pTask);

/* pop�ڴ�ض���ͷ�ڵ� */
struct fast_task_info *free_queue_pop();

/* ͳ���ڴ�ض����п��ýڵ��� */
int free_queue_count();


/* ��ʼ��������� */
int task_queue_init(struct fast_task_queue *pQueue);

/* ������ڵ�pTask���뵽��������� */
int task_queue_push(struct fast_task_queue *pQueue, \
		struct fast_task_info *pTask);

/* pop�������ͷ�ڵ� */
struct fast_task_info *task_queue_pop(struct fast_task_queue *pQueue);

/* ����������нڵ��� */
int task_queue_count(struct fast_task_queue *pQueue);

#ifdef __cplusplus
}
#endif

#endif

