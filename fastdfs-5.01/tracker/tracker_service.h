/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//tracker_service.h

#ifndef _TRACKER_SERVICE_H_
#define _TRACKER_SERVICE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fdfs_define.h"
#include "ioevent.h"
#include "fast_task_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

//typedef struct nio_thread_data struct nio_thread_data;

extern int g_tracker_thread_count;
extern struct nio_thread_data *g_thread_data;

/* tracker����ĳ�ʼ�� */
int tracker_service_init();


/* ����tracker_service��Դ */
int tracker_service_destroy();

/* ��ֹ���еĹ����̣߳���ÿ���̵߳�pipe[1]�ܵ�����С��0��socket������ */
int tracker_terminate_threads();

/* 
 * ѭ��accept�������̻�������һ��accept��
 * ���g_accept_threads>1����ͨ�����߳���ͬʱaccept 
 */
void tracker_accept_loop(int server_sock);

/* tracker������ */
int tracker_deal_task(struct fast_task_info *pTask);

#ifdef __cplusplus
}
#endif

#endif
