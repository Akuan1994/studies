
#ifndef PROCESS_CTRL_H
#define PROCESS_CTRL_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ���������ļ����ڴ��в��һ�ȡ�����ļ��е�"base_path"�ֶ�ֵ */
int get_base_path_from_conf_file(const char *filename, char *base_path,
	const int path_size);

/* ���ļ��л�ȡpidֵ */
int get_pid_from_file(const char *pidFilename, pid_t *pid);

/* ���ػ����̵Ľ��̺�д�뵽pid�ļ��� */
int write_to_pid_file(const char *pidFilename);

/* ɾ��pid_file */
int delete_pid_file(const char *pidFilename);

/* ֹͣpid�ļ�����дָ������*/
int process_stop(const char *pidFilename);

/* 
 * ����pid�ļ�����дָ������
 * ��ʵ���ǹرգ���Ϊmain�����к�������bool stop��ֵ���ж��Ƿ��ٴ�start
 */
int process_restart(const char *pidFilename);

int process_exist(const char *pidFilename);

/* ��������������start | restart | stop */
int process_action(const char *pidFilename, const char *action, bool *stop);

#ifdef __cplusplus
}
#endif

#endif

