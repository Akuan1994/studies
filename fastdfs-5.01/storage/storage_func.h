/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//storage_func.h

#ifndef _STORAGE_FUNC_H_
#define _STORAGE_FUNC_H_

#include "tracker_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef char * (*get_filename_func)(const void *pArg, \
			char *full_filename);

/* ��fd��ָ���ļ���գ�֮��buff�е�����д��fdָ�����ļ� */
int storage_write_to_fd(int fd, get_filename_func filename_func, \
		const void *pArg, const char *buff, const int len);

/* ��ʼ������ع��������������ļ��������洢Ŀ¼���ָ��������ݵ� */
int storage_func_init(const char *filename, \
		char *bind_addr, const int addr_size);

/* ����ȫ����Ϣ�洢����ؿռ估�ر���Դռ�� */
int storage_func_destroy();

/* ��storage�ĵ�ǰ״̬��Ϣд����� */
int storage_write_to_stat_file();

/* storage��ͬ����Ϣд����̱��棬��storage����ʱʹ�� */
int storage_write_to_sync_ini_file();

/* ���ָ����Storage Server�Ƿ�������� */
bool storage_server_is_myself(const FDFSStorageBrief *pStorageBrief);

/* ���ָ����id�Ƿ���������id */
bool storage_id_is_myself(const char *storage_id);

/* ����ָ��Ŀ¼����������û� */
#define STORAGE_CHOWN(path, current_uid, current_gid) \
	if (!(g_run_by_gid == current_gid && g_run_by_uid == current_uid)) \
	{ \
		if (chown(path, g_run_by_uid, g_run_by_gid) != 0) \
		{ \
			logError("file: "__FILE__", line: %d, " \
				"chown \"%s\" fail, " \
				"errno: %d, error info: %s", \
				__LINE__, path, \
				errno, STRERROR(errno)); \
			return errno != 0 ? errno : EPERM; \
		} \
	}


/* �޸��ļ�������group��user */
#define STORAGE_FCHOWN(fd, path, current_uid, current_gid) \
	if (!(g_run_by_gid == current_gid && g_run_by_uid == current_uid)) \
	{ \
		if (fchown(fd, g_run_by_uid, g_run_by_gid) != 0) \
		{ \
			logError("file: "__FILE__", line: %d, " \
				"chown \"%s\" fail, " \
				"errno: %d, error info: %s", \
				__LINE__, path, \
				errno, STRERROR(errno)); \
			return errno != 0 ? errno : EPERM; \
		} \
	}


/*
int write_serialized(int fd, const char *buff, size_t count, const bool bSync);
int fsync_serialized(int fd);
int recv_file_serialized(int sock, const char *filename, \
		const int64_t file_bytes);
*/

#ifdef __cplusplus
}
#endif

#endif
