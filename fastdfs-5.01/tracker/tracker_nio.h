/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//tracker_nio.h

#ifndef _TRACKER_NIO_H
#define _TRACKER_NIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fast_task_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * ÿ���̵߳Ĺܵ�pipe_fds[0]��READ�¼��Ļص����� 
 * ÿ�ζ�ȡһ��int���������½������ӵ�socket��������֮����뵽IO�¼�����
 * ��ͨ���ַ�����Ӧ�����߳��еȴ��ɶ��¼����������client_sock_read�������д���
 */
void recv_notify_read(int sock, short event, void *arg);

/* �����ͱ��ĵ��¼����뵽IO�¼������� */
int send_add_event(struct fast_task_info *pTask);

/* ���������������� */
void task_finish_clean_up(struct fast_task_info *pTask);

#ifdef __cplusplus
}
#endif

#endif

