/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//trunk_sync.h

#ifndef _TRUNK_SYNC_H_
#define _TRUNK_SYNC_H_

#include "tracker_types.h"
#include "storage_func.h"
#include "trunk_mem.h"

#define TRUNK_OP_TYPE_ADD_SPACE		'A'
#define TRUNK_OP_TYPE_DEL_SPACE		'D'

#define TRUNK_BINLOG_BUFFER_SIZE	(64 * 1024)	/* ��ȡtrunk_binlogʱ�Ļ����� */
#define TRUNK_BINLOG_LINE_SIZE		128			/* trunk_binlogÿһ�еĳ��� */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	char storage_id[FDFS_STORAGE_ID_MAX_SIZE];
	BinLogBuffer binlog_buff;
	int mark_fd;
	int binlog_fd;
	int64_t binlog_offset;
	int64_t last_binlog_offset;  //for write to mark file
} TrunkBinLogReader;		/* trunk binlog�Ķ�ȡ��Ϣ */

typedef struct
{
	time_t timestamp;		/* ����ʱ��� */
	char op_type;			/* �������� */
	FDFSTrunkFullInfo trunk;
} TrunkBinLogRecord;		/* trunk binlog�е�һ��������¼ */

extern int g_trunk_sync_thread_count;

/* trunkͬ�������Դ�ĳ�ʼ������������trunk_binlog_file�� */
int trunk_sync_init();

/* trunkͬ�������Դ�����ٹ������ر�trunk_binlog_file */
int trunk_sync_destroy();

/* ��buff�е�����д��trunk_binlog_file */
int trunk_binlog_write_buffer(const char *buff, const int length);

/* ��������¼д��trunk_binlog_file */
int trunk_binlog_write(const int timestamp, const char op_type, \
		const FDFSTrunkFullInfo *pTrunk);

/* ��trunk_binlog_file��� */
int trunk_binlog_truncate();

/* ��trunk_binlog_file�ж�ȡһ�м�¼��������TrunkBinLogRecord������ */
int trunk_binlog_read(TrunkBinLogReader *pReader, \
		      TrunkBinLogRecord *pRecord, int *record_length);

/* Ϊÿһ��ͬ���storage����һ��trunkͬ���߳� */
int trunk_sync_thread_start_all();

/* ����trunk��ͬ���߳� */
int trunk_sync_thread_start(const FDFSStorageBrief *pStorage);

/* ֹͣ�������������е�trunkͬ���߳� */
int kill_trunk_sync_threads();

/* ���������е�����ǿ��ˢ�µ������� */
int trunk_binlog_sync_func(void *args);

/* ��ȡtrunk_binlog_file���ļ��� */
char *get_trunk_binlog_filename(char *full_filename);

/* ����trunk_reader��Ϣ��ȡtrunk��mark_file */
char *trunk_mark_filename_by_reader(const void *pArg, char *full_filename);

/* ɾ������ͬ�������storage��Ӧ��mark_file�ļ�(����ʱ����Ϊ��׺) */
int trunk_unlink_all_mark_files();

/* ��ָ��storage_id��Ӧ��mark_file�����������ϵ�ǰʱ����Ϊ��׺ */
int trunk_unlink_mark_file(const char *storage_id);

/* �����µ�ip��port������trunk��mark_file */
int trunk_rename_mark_file(const char *old_ip_addr, const int old_port, \
		const char *new_ip_addr, const int new_port);

/* ��trunk binlog file����λ��ָ��ƫ������λ�� */
int trunk_open_readable_binlog(TrunkBinLogReader *pReader, \
		get_filename_func filename_func, const void *pArg);

/* 
 * ��ʼ��ָ��storage��Ӧ��TrunkBinLogReader���� 
 * pStorageΪNULL���ȡ�����reader����
 */
int trunk_reader_init(FDFSStorageBrief *pStorage, TrunkBinLogReader *pReader);

/* ����trunk_reader�����Դ */
void trunk_reader_destroy(TrunkBinLogReader *pReader);

/* ��ʼѹ��trunk_binlog_file��׼����������trunk_binlog_file����һ��rollback�ļ��Թ��ָ� */
int trunk_binlog_compress_apply();

/* ѹ����������storage_trunk.dat��trunk_binlog_file�е����ݺϲ� */
int trunk_binlog_compress_commit();

/* �ع��������ָ�trunk_binlog_file�ļ�����������ļ�������Ϣ���ϲ� */
int trunk_binlog_compress_rollback();

#ifdef __cplusplus
}
#endif

#endif
