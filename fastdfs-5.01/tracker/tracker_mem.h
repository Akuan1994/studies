/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//tracker_mem.h

#ifndef _TRACKER_MEM_H_
#define _TRACKER_MEM_H_

#include <time.h>
#include <pthread.h>
#include "tracker_types.h"

#define TRACKER_SYS_FILE_COUNT  4
#define STORAGE_GROUPS_LIST_FILENAME_OLD   "storage_groups.dat"				/* groups��Ϣ�ļ�old */
#define STORAGE_GROUPS_LIST_FILENAME_NEW   "storage_groups_new.dat"		/* groups��Ϣ�ļ�new */
#define STORAGE_SERVERS_LIST_FILENAME_OLD  "storage_servers.dat"
#define STORAGE_SERVERS_LIST_FILENAME_NEW  "storage_servers_new.dat"		/* storage server��Ϣ�ļ� */
#define STORAGE_SERVERS_CHANGELOG_FILENAME "storage_changelog.dat"			/* changelog�ļ��� */
#define STORAGE_SYNC_TIMESTAMP_FILENAME	   "storage_sync_timestamp.dat"		/* ͬ��ʱ��������ļ� */
#define TRUNK_SERVER_CHANGELOG_FILENAME    "trunk_server_change.log"			/* trunk_server�����Ϣ�ļ� */
#define STORAGE_DATA_FIELD_SEPERATOR	   ','		/* �ָ��� */

typedef struct {
	ConnectionInfo *pTrackerServer;
	int running_time;     /* running seconds, more means higher weight(����ʱ��) */
	int restart_interval; /* restart interval, less mean higher weight(�������������ʱ��) */
	bool if_leader;       /* if leader(�Ƿ����Ϊleader) */
} TrackerRunningStatus;	/* tracker server����״̬ */

#ifdef __cplusplus
extern "C" {
#endif

extern TrackerServerGroup g_tracker_servers;  //save all tracker servers from storage server
extern ConnectionInfo *g_last_tracker_servers;  //for delay free
extern int g_next_leader_index;			   //next leader index
extern int g_tracker_leader_chg_count;		   //for notify storage servers
extern int g_trunk_server_chg_count;		   //for notify other trackers

extern int64_t g_changelog_fsize; //storage server change log file size
extern char *g_tracker_sys_filenames[TRACKER_SYS_FILE_COUNT];

int tracker_mem_init();
int tracker_mem_destroy();

int tracker_mem_init_pthread_lock(pthread_mutex_t *pthread_lock);
int tracker_mem_pthread_lock();
int tracker_mem_pthread_unlock();

/* Ϊ��дtracker����ļ����� */
int tracker_mem_file_lock();
/* Ϊ��дtracker����ļ����� */
int tracker_mem_file_unlock();

/* ����group_name��ȡgroup��Ϣ����������ڣ�����NULL */
#define tracker_mem_get_group(group_name) \
	tracker_mem_get_group_ex((&g_groups), group_name)

/* ����group_name��ȡgroup��Ϣ����������ڣ�����NULL */
FDFSGroupInfo *tracker_mem_get_group_ex(FDFSGroups *pGroups, \
		const char *group_name);

/* ����id��pGroup���ҵ���Ӧ��storage_server */
FDFSStorageDetail *tracker_mem_get_storage(FDFSGroupInfo *pGroup, \
				const char *id);

/* ����ip��ַ����ȡstorage���� */
FDFSStorageDetail *tracker_mem_get_storage_by_ip(FDFSGroupInfo *pGroup, \
				const char *ip_addr);

/* ����pGroup�е�pStroageIdΪtrunk_server,�����ָ������ѡȡtrunk_binlog_size����storage */
const FDFSStorageDetail *tracker_mem_set_trunk_server( \
	FDFSGroupInfo *pGroup, const char *pStroageId, int *result);

/* ��pGroup��ɾ��ָ��storage_id��storage */
int tracker_mem_delete_storage(FDFSGroupInfo *pGroup, const char *id);

/* ��pGroup���޸�ָ��storage_id��ip */
int tracker_mem_storage_ip_changed(FDFSGroupInfo *pGroup, \
		const char *old_storage_ip, const char *new_storage_ip);

/* 
 * ���¼����storage_server���ڵ�group�Լ�storage��Ϣ���뵽�ڴ��ȫ�ֱ�����
 * ��д���ļ� 
 */
int tracker_mem_add_group_and_storage(TrackerClientInfo *pClientInfo, \
		const char *ip_addr, FDFSStorageJoinBody *pJoinBody, \
		const bool bNeedSleep);

/* ��ָ����storage��Ϊoffline״̬ */
int tracker_mem_offline_store_server(FDFSGroupInfo *pGroup, \
			FDFSStorageDetail *pStorage);

/* ���storage server���û�У��ͼ��뵽��Ӧgroup�� */
int tracker_mem_active_store_server(FDFSGroupInfo *pGroup, \
			FDFSStorageDetail *pTargetServer);

int tracker_mem_sync_storages(FDFSGroupInfo *pGroup, \
                FDFSStorageBrief *briefServers, const int server_count);

/* ��storage��״̬��Ϣд���ļ��б��� */
int tracker_save_storages();

/* ������group��ͬ��ʱ�����Ϣд���ļ��� */
int tracker_save_sync_timestamps();

/* ��ϵͳ�����ļ����ڴ��ж�Ӧ����Ϣд���ļ��� */
int tracker_save_sys_files();

int tracker_get_group_file_count(FDFSGroupInfo *pGroup);
int tracker_get_group_success_upload_count(FDFSGroupInfo *pGroup);

/* �ҵ�ͬgroup�е�һ̨storage�������Լ� */
FDFSStorageDetail *tracker_get_group_sync_src_server(FDFSGroupInfo *pGroup, \
			FDFSStorageDetail *pDestServer);

/* ��ָ��group�л�ȡһ�����ϴ��ļ���storage */
FDFSStorageDetail *tracker_get_writable_storage(FDFSGroupInfo *pStoreGroup);

#ifdef WITH_HTTPD
#define FDFS_DOWNLOAD_TYPE_PARAM  	const int download_type, 
#define FDFS_DOWNLOAD_TYPE_CALL		FDFS_DOWNLOAD_TYPE_TCP, 
#else
#define FDFS_DOWNLOAD_TYPE_PARAM 
#define FDFS_DOWNLOAD_TYPE_CALL 
#endif

/* ����file_name��ȡӦ��ȥ��̨storage server���� */
int tracker_mem_get_storage_by_filename(const byte cmd,FDFS_DOWNLOAD_TYPE_PARAM\
	const char *group_name, const char *filename, const int filename_len, \
	FDFSGroupInfo **ppGroup, FDFSStorageDetail **ppStoreServers, \
	int *server_count);

/* ���storage��״̬�Ƿ����� */
int tracker_mem_check_alive(void *arg);

/* ����pStorage��pGroup->all_servers�е�λ�� */
int tracker_mem_get_storage_index(FDFSGroupInfo *pGroup, \
		FDFSStorageDetail *pStorage);

/* ���Ϳ�ʼ��ȡ�����ļ��ı��� */
#define tracker_get_sys_files_start(pTrackerServer) \
		 fdfs_deal_no_body_cmd(pTrackerServer, \
		TRACKER_PROTO_CMD_TRACKER_GET_SYS_FILES_START)

/* ���ͽ�����ȡ�����ļ��ı��� */
#define tracker_get_sys_files_end(pTrackerServer) \
		 fdfs_deal_no_body_cmd(pTrackerServer, \
		TRACKER_PROTO_CMD_TRACKER_GET_SYS_FILES_END)

void tracker_calc_running_times(TrackerRunningStatus *pStatus);

/* ���ͱ��Ļ�ȡpTrackerServer��״̬��Ϣ */
int tracker_mem_get_status(ConnectionInfo *pTrackerServer, \
		TrackerRunningStatus *pStatus);

/* ������groups��ص���Ϣд��groups_new�ļ� */
int tracker_save_groups();

/* ��������group��trunk_server */
void tracker_mem_find_trunk_servers();

#ifdef __cplusplus
}
#endif

#endif
