/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//storage_dio.h

#ifndef _STORAGE_DIO_H
#define _STORAGE_DIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "tracker_types.h"
#include "fast_task_queue.h"

struct storage_dio_context		/* ÿһ����д�̶߳�Ӧ���̱߳��� */
{
	struct fast_task_queue queue;		/* ���߳�Ҫ������������ */
	pthread_mutex_t lock;
	pthread_cond_t cond;
};

struct storage_dio_thread_data		/* ÿ���洢·����Ӧ��dio�̱߳��� */
{
	/* for mixed read / write */
	struct storage_dio_context *contexts;	/* ���ж�д�̵߳������Ϣ */
	int count;  /* context count(��д�߳�������contextsԪ�ظ���) */

	/* for separated read / write */
	struct storage_dio_context *reader;	/* ���ж��߳���Ϣ */
	struct storage_dio_context *writer;	/* ����д�߳���Ϣ */
};

#ifdef __cplusplus
extern "C" {
#endif

extern int g_dio_thread_count;

 /* ��ʼ������IO�����Դ��ÿһ���洢·����������ض�д�߳� */
int storage_dio_init();

 /* 
 * �����ж�д�̷߳���֪ͨ��������������������
 * ֮ǰg_continue_flag��Ϊfalse���߳�ȫ���˳� 
 */
void storage_dio_terminate();

/* ��ȡָ���洢·���Ķ���д�̵߳�����index */
int storage_dio_get_thread_index(struct fast_task_info *pTask, \
		const int store_path_index, const char file_op);

/* ����������뵽��������в�֪ͨ��Ӧ�Ķ�д�߳̽��д��� */
int storage_dio_queue_push(struct fast_task_info *pTask);

/* ��ָ�����ļ�����д��pTask->data */
int dio_read_file(struct fast_task_info *pTask);

/* ��pTask->data �е�����д��ָ���ļ� */
int dio_write_file(struct fast_task_info *pTask);

/* ��ȡָ���ļ���pFileContext->offset��֮���������� */
int dio_truncate_file(struct fast_task_info *pTask);

/* ɾ��ָ������ͨ�ļ� */
int dio_delete_normal_file(struct fast_task_info *pTask);

/* ɾ��ָ����trunk_file */
int dio_delete_trunk_file(struct fast_task_info *pTask);

/* ���ѽ��յ���buff���ݶ�����֮��������� */
int dio_discard_file(struct fast_task_info *pTask);

/* ��ȡ�ļ���ɺ�������� */
void dio_read_finish_clean_up(struct fast_task_info *pTask);

/* д���ļ���ɺ�������������û��ȫ��д��ɹ���ɾ�����ļ� */
void dio_write_finish_clean_up(struct fast_task_info *pTask);

/* ������ݵ��ļ�ĩβ��ɺ����������û��ȫд��Ļ���ԭ */
void dio_append_finish_clean_up(struct fast_task_info *pTask);

/* д��trunk_file�ļ���ɺ�������������û��ȫ��д��ɹ�����ԭ */
void dio_trunk_write_finish_clean_up(struct fast_task_info *pTask);

/* �޸��ļ���������������û��ȫ����ɣ���ԭ */
void dio_modify_finish_clean_up(struct fast_task_info *pTask);

/* ��ȡ�ļ���ɺ�������� */
#define dio_truncate_finish_clean_up  dio_read_finish_clean_up

/* ���ָ����fd�еĶ�дλ�ô��Ƿ���trunk_file */
int dio_check_trunk_file_ex(int fd, const char *filename, const int64_t offset);

/* �ϴ��ļ�ǰ���trunk_file�Ƿ���ȷ�����û�У�����trunk_file */
int dio_check_trunk_file_when_upload(struct fast_task_info *pTask);

/* ͬ����ʱ����trunk_file�Ƿ���ȷ */
int dio_check_trunk_file_when_sync(struct fast_task_info *pTask);

/* ��trunk_header��Ϣд���ļ� */
int dio_write_chunk_header(struct fast_task_info *pTask);

#ifdef __cplusplus
}
#endif

#endif

