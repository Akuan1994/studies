/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//tracker_types.h

#ifndef _TRACKER_TYPES_H_
#define _TRACKER_TYPES_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "fdfs_define.h"
#include "connection_pool.h"

#define FDFS_ONE_MB	(1024 * 1024)		/* 1MB��Ӧ 1024*1024�ֽ� */

#define FDFS_GROUP_NAME_MAX_LEN		16		/* Group Name����󳤶� */
#define FDFS_MAX_SERVERS_EACH_GROUP	32		/* ÿ��group��storage������ */
#define FDFS_MAX_GROUPS		       512			/* ���group�� */
#define FDFS_MAX_TRACKERS		16		/* tracker server������� */

#define FDFS_MAX_META_NAME_LEN		 64
#define FDFS_MAX_META_VALUE_LEN		256

#define FDFS_FILE_PREFIX_MAX_LEN	16
#define FDFS_LOGIC_FILE_PATH_LEN	10
#define FDFS_TRUE_FILE_PATH_LEN		 6
#define FDFS_FILENAME_BASE64_LENGTH     27
#define FDFS_TRUNK_FILE_INFO_LEN  16
#define FDFS_MAX_SERVER_ID        ((1 << 24) - 1)	/* storage server id�����ֵ */

#define FDFS_ID_TYPE_SERVER_ID    1		/* id��ʽΪserver id */
#define FDFS_ID_TYPE_IP_ADDRESS   2	/* id��ʽΪip��ַ */

#define FDFS_NORMAL_LOGIC_FILENAME_LENGTH (FDFS_LOGIC_FILE_PATH_LEN + \
		FDFS_FILENAME_BASE64_LENGTH + FDFS_FILE_EXT_NAME_MAX_LEN + 1)

#define FDFS_TRUNK_FILENAME_LENGTH (FDFS_TRUE_FILE_PATH_LEN + \
		FDFS_FILENAME_BASE64_LENGTH + FDFS_TRUNK_FILE_INFO_LEN + \
		1 + FDFS_FILE_EXT_NAME_MAX_LEN)
#define FDFS_TRUNK_LOGIC_FILENAME_LENGTH  (FDFS_TRUNK_FILENAME_LENGTH + \
		(FDFS_LOGIC_FILE_PATH_LEN - FDFS_TRUE_FILE_PATH_LEN))

#define FDFS_VERSION_SIZE		6

/* storager����״̬ */
//status order is important!
#define FDFS_STORAGE_STATUS_INIT	  0
#define FDFS_STORAGE_STATUS_WAIT_SYNC	  1
#define FDFS_STORAGE_STATUS_SYNCING	  2
#define FDFS_STORAGE_STATUS_IP_CHANGED    3
#define FDFS_STORAGE_STATUS_DELETED	  4
#define FDFS_STORAGE_STATUS_OFFLINE	  5
#define FDFS_STORAGE_STATUS_ONLINE	  6
#define FDFS_STORAGE_STATUS_ACTIVE	  7
#define FDFS_STORAGE_STATUS_RECOVERY	  9
#define FDFS_STORAGE_STATUS_NONE	 99

/* ѡ���ĸ������ϴ��ļ��ķ�ʽ */
//which group to upload file
#define FDFS_STORE_LOOKUP_ROUND_ROBIN	0  //round robin(ѭ��)
#define FDFS_STORE_LOOKUP_SPEC_GROUP	1  //specify group(�ض���)
#define FDFS_STORE_LOOKUP_LOAD_BALANCE	2  //load balance(����ѡ��)

/* �ϴ��ļ�ʱͬһ������ѡ����һ��storage�ķ�ʽ */
//which server to upload file
#define FDFS_STORE_SERVER_ROUND_ROBIN	0  //round robin(ѭ��)
#define FDFS_STORE_SERVER_FIRST_BY_IP	1  //the first server order by ip(����ip������׸�)
#define FDFS_STORE_SERVER_FIRST_BY_PRI	2  //the first server order by priority(�������ȼ�������׸�)

/* �����ļ�ʱѡ����һ��storage�ķ�ʽ */
//which server to download file
#define FDFS_DOWNLOAD_SERVER_ROUND_ROBIN	0  //round robin(ѭ��)
#define FDFS_DOWNLOAD_SERVER_SOURCE_FIRST	1  //the source server(�����ϴ�ʱ��storage server)

/* �ϴ��ļ�ʱѡ����һ��Ŀ¼�ķ�ʽ */
//which path to upload file
#define FDFS_STORE_PATH_ROUND_ROBIN	0  //round robin(ѭ��)
#define FDFS_STORE_PATH_LOAD_BALANCE	2  //load balance(����ѡ��)

/* 
 * the mode of the files distributed to the data path
 * �ļ���dataĿ¼�·�ɢ�洢���� 
 */
#define FDFS_FILE_DIST_PATH_ROUND_ROBIN	0  //round robin
#define FDFS_FILE_DIST_PATH_RANDOM	1  //random(�����ļ�����hash)

//http check alive type
#define FDFS_HTTP_CHECK_ALIVE_TYPE_TCP  0  //tcp
#define FDFS_HTTP_CHECK_ALIVE_TYPE_HTTP 1  //http

#define FDFS_DOWNLOAD_TYPE_TCP	0  //tcp
#define FDFS_DOWNLOAD_TYPE_HTTP	1  //http

/* Ĭ����ת�洢��ʽ�Ļ�ÿ��Ŀ¼�洢�����ļ���ת����һ��Ŀ¼ */
#define FDFS_FILE_DIST_DEFAULT_ROTATE_COUNT   100

#define FDFS_DOMAIN_NAME_MAX_SIZE	128

#define FDFS_STORAGE_STORE_PATH_PREFIX_CHAR  'M'		/* �ļ�����store_path��ǰ׺�ַ� */
#define FDFS_STORAGE_DATA_DIR_FORMAT         "%02X"
#define FDFS_STORAGE_META_FILE_EXT           "-m"

#define FDFS_APPENDER_FILE_SIZE  INFINITE_FILE_SIZE
#define FDFS_TRUNK_FILE_MARK_SIZE  (512 * 1024LL * 1024 * 1024 * 1024 * 1024LL)

#define FDFS_CHANGE_FLAG_TRACKER_LEADER	1  //tracker leader changed
#define FDFS_CHANGE_FLAG_TRUNK_SERVER	2  //trunk server changed
#define FDFS_CHANGE_FLAG_GROUP_SERVER	4  //group server changed

/* ����file_size�е�ĳһλ�ж��Ƿ���appender_file */
#define IS_APPENDER_FILE(file_size)   ((file_size & FDFS_APPENDER_FILE_SIZE)!=0)
/* ����file_size�е�ĳһλ�ж��Ƿ���trunk_file */
#define IS_TRUNK_FILE(file_size)     ((file_size&FDFS_TRUNK_FILE_MARK_SIZE)!=0)

#define IS_SLAVE_FILE(filename_len, file_size) \
	((filename_len > FDFS_TRUNK_LOGIC_FILENAME_LENGTH) || \
	(filename_len > FDFS_NORMAL_LOGIC_FILENAME_LENGTH && \
	 !IS_TRUNK_FILE(file_size)))

/* ��ȡtrunk_file����ʵ�ļ���С��ֻȡ��32λ��ǰ����Ϊ0 */
#define FDFS_TRUNK_FILE_TRUE_SIZE(file_size) \
	(file_size & 0xFFFFFFFF)

#define FDFS_STORAGE_ID_MAX_SIZE	16		/* storage server id��󳤶� */

/* ÿ̨storage server�����洢�ռ��С���ֱ�־ */
#define TRACKER_STORAGE_RESERVED_SPACE_FLAG_MB		0	/* �������� */
#define TRACKER_STORAGE_RESERVED_SPACE_FLAG_RATIO	1	/* �ٷֱ� */

typedef struct
{
	char status;			/* ״̬ */
	char port[4];			/* ͨ�Ŷ˿� */
	char id[FDFS_STORAGE_ID_MAX_SIZE];	/* id */
	char ip_addr[IP_ADDRESS_SIZE];		/* ip��ַ */
} FDFSStorageBrief;	/* storage server�ļ�Ҫ��Ϣ�ṹ */

typedef struct
{
	char group_name[FDFS_GROUP_NAME_MAX_LEN + 8];  //for 8 bytes alignment
	int64_t total_mb;  //total disk storage in MB
	int64_t free_mb;  //free disk storage in MB
	int64_t trunk_free_mb;  //trunk free disk storage in MB
	int count;        //server count
	int storage_port; //storage server port
	int storage_http_port; //storage server http port
	int active_count; //active server count
	int current_write_server; //current server index to upload file
	int store_path_count;  //store base path count of each storage server
	int subdir_count_per_path;
	int current_trunk_file_id;  //current trunk file id
} FDFSGroupStat;

typedef struct
{
	/* following count stat by source server,
           not including synced count
	*/
	int64_t total_upload_count;
	int64_t success_upload_count;
	int64_t total_append_count;
	int64_t success_append_count;
	int64_t total_modify_count;
	int64_t success_modify_count;
	int64_t total_truncate_count;
	int64_t success_truncate_count;
	int64_t total_set_meta_count;
	int64_t success_set_meta_count;
	int64_t total_delete_count;
	int64_t success_delete_count;
	int64_t total_download_count;
	int64_t success_download_count;
	int64_t total_get_meta_count;
	int64_t success_get_meta_count;
	int64_t total_create_link_count;
	int64_t success_create_link_count;
	int64_t total_delete_link_count;
	int64_t success_delete_link_count;
	int64_t total_upload_bytes;
	int64_t success_upload_bytes;
	int64_t total_append_bytes;
	int64_t success_append_bytes;
	int64_t total_modify_bytes;
	int64_t success_modify_bytes;
	int64_t total_download_bytes;
	int64_t success_download_bytes;
	int64_t total_sync_in_bytes;
	int64_t success_sync_in_bytes;
	int64_t total_sync_out_bytes;
	int64_t success_sync_out_bytes;
	int64_t total_file_open_count;
	int64_t success_file_open_count;
	int64_t total_file_read_count;
	int64_t success_file_read_count;
	int64_t total_file_write_count;
	int64_t success_file_write_count;

	/* last update timestamp as source server, 
           current server' timestamp
	*/
	time_t last_source_update;

	/* last update timestamp as dest server, 
           current server' timestamp
	*/
	time_t last_sync_update;

	/* 
	 * last syned timestamp, 
	 * source server's timestamp
	 * ���һ��ȫ��ͬ����ɵ�ͬ��ʱ���
	 */
	time_t last_synced_timestamp;

	/* last heart beat time (���һ���յ���������ʱ��) */
	time_t last_heart_beat_time;
} FDFSStorageStat;		/* storage server״̬��Ϣ */

/* struct for network transfering */
typedef struct
{
	char sz_total_upload_count[8];
	char sz_success_upload_count[8];
	char sz_total_append_count[8];
	char sz_success_append_count[8];
	char sz_total_modify_count[8];
	char sz_success_modify_count[8];
	char sz_total_truncate_count[8];
	char sz_success_truncate_count[8];
	char sz_total_set_meta_count[8];
	char sz_success_set_meta_count[8];
	char sz_total_delete_count[8];
	char sz_success_delete_count[8];
	char sz_total_download_count[8];
	char sz_success_download_count[8];
	char sz_total_get_meta_count[8];
	char sz_success_get_meta_count[8];
	char sz_total_create_link_count[8];
	char sz_success_create_link_count[8];
	char sz_total_delete_link_count[8];
	char sz_success_delete_link_count[8];
	char sz_total_upload_bytes[8];
	char sz_success_upload_bytes[8];
	char sz_total_append_bytes[8];
	char sz_success_append_bytes[8];
	char sz_total_modify_bytes[8];
	char sz_success_modify_bytes[8];
	char sz_total_download_bytes[8];
	char sz_success_download_bytes[8];
	char sz_total_sync_in_bytes[8];
	char sz_success_sync_in_bytes[8];
	char sz_total_sync_out_bytes[8];
	char sz_success_sync_out_bytes[8];
	char sz_total_file_open_count[8];
	char sz_success_file_open_count[8];
	char sz_total_file_read_count[8];
	char sz_success_file_read_count[8];
	char sz_total_file_write_count[8];
	char sz_success_file_write_count[8];
	char sz_last_source_update[8];
	char sz_last_sync_update[8];
	char sz_last_synced_timestamp[8];
	char sz_last_heart_beat_time[8];
} FDFSStorageStatBuff;	/* storage server״̬��Ϣ�ַ��� */

typedef struct StructFDFSStorageDetail
{
	char status;
	char padding;  //just for padding
	char id[FDFS_STORAGE_ID_MAX_SIZE];
	char ip_addr[IP_ADDRESS_SIZE];
	char version[FDFS_VERSION_SIZE];
	char domain_name[FDFS_DOMAIN_NAME_MAX_SIZE];

	struct StructFDFSStorageDetail *psync_src_server;	/* ����̨storageͬ�����ļ� */
	int64_t *path_total_mbs; /* total disk storage in MB(ÿ��·���·ֱ�ͳ�Ƶ��ܿռ��С) */
	int64_t *path_free_mbs;  /* free disk storage in MB(ÿ��·���·ֱ�ͳ�ƵĿ��пռ��С) */

	int64_t total_mb;  /* total disk storage in MB(�ۼ��ܿռ��С) */
	int64_t free_mb;  /* free disk storage in MB(�ۼƿ��пռ��С) */
	int64_t changelog_offset;  /* changelog file offset(changelog�ļ��е�ƫ����) */

	time_t sync_until_timestamp;		/* ͬ��ʱ��� */
	time_t join_time;  //storage join timestamp (create timestamp)
	time_t up_time;    //startup timestamp

	int store_path_count;  /* store base path count of each storage server(base path�ĸ���) */
	int subdir_count_per_path;	/* ����Ŀ¼�ĸ��� */
	int upload_priority; /* storage upload priority(�ϴ����ȼ�) */

	int storage_port;   //storage server port
	int storage_http_port; //storage http server port

	int current_write_path; /* current write path index(��ǰʹ�õ�·������) */

	int chg_count;    /* current server changed counter(storage server��Ϣ�ı����) */
	int trunk_chg_count;   /* trunk server changed count(trunk server��Ϣ�ı����) */
	FDFSStorageStat stat;

#ifdef WITH_HTTPD
	int http_check_last_errno;
	int http_check_last_status;
	int http_check_fail_count;
	char http_check_error_info[256];
#endif
} FDFSStorageDetail;	/* storage server��Ϣ */

typedef struct
{
	char group_name[FDFS_GROUP_NAME_MAX_LEN + 8];   //for 8 bytes alignment
	int64_t total_mb;  //total disk storage in MB
	int64_t free_mb;  //free disk storage in MB
	int64_t trunk_free_mb;  //trunk free disk storage in MB
	int alloc_size;  //alloc storage count(�ѷ���ռ��storage serverԪ�ظ���)
	int count;    //total server count(ʵ�ʵ�storage serverԪ�ظ���)
	int active_count; //active server count(��Ծ��storage serverԪ�ظ���)
	int storage_port;  //storage server port(storage server������tracker serverͨѶ�Ķ˿�)
	int storage_http_port; //storage http server port
	int current_trunk_file_id;  /* current trunk file id report by storage(storage�㱨�ĵ�ǰtrunk_file_id) */
	FDFSStorageDetail **all_servers;   //all storage servers(�������storage server��Ϣ)
	FDFSStorageDetail **sorted_servers;  /* storages order by ip addr(��Ÿ���ip���������storage server��Ϣ) */
	FDFSStorageDetail **active_servers;  /* storages order by ip addr(��Ÿ���ip����Ļ�Ծ��storage server��Ϣ) */
	FDFSStorageDetail *pStoreServer;  /* for upload priority mode(upload_priorityֵ��С�����ȼ��ߵ�stroage) */
	FDFSStorageDetail *pTrunkServer;  /* point to the trunk server(ָ��trunk_server) */
	char last_trunk_server_id[FDFS_STORAGE_ID_MAX_SIZE];		/* ���һ�ε�trunk server id */

#ifdef WITH_HTTPD
	FDFSStorageDetail **http_servers;  //storages order by ip addr
	int http_server_count; //http server count
	int current_http_server; //current http server index
#endif

	int current_read_server;   /* current read storage server index(��ǰgroup����storage������) */
	int current_write_server;  /* current write storage server index(��ǰgroup�ϴ�storage������) */

	int store_path_count;  /* store base path count of each storage server(ÿ��storage�Ĵ洢·������) */

	/* 
	 * subdir_count * subdir_count directories will be auto created
	 * under each store_path (disk) of the storage servers
	 * ����Ŀ¼�ĸ���
	 */
	int subdir_count_per_path;

	/* 
	 * row for src storage, col for dest storage
	 * ��Դstorage server��Ŀ��storage server�����һ��ͬ��ʱ���
	 * �д���Դstorage server���д���Ŀ��storage server
	 */
	int **last_sync_timestamps;

	int chg_count;   /* current group changed count(��group�ı任����) */
	int trunk_chg_count;   /* trunk server changed count(��group��trunk_server�ĸı����) */
	time_t last_source_update;  //last source update timestamp
	time_t last_sync_update;    //last synced update timestamp
} FDFSGroupInfo;	/* ����group��Ϣ */

typedef struct
{
	int alloc_size;   //alloc group count
	int count;  /* ��������ʹ�õ�group����Ϣ�����Ԫ�ظ��� */
	FDFSGroupInfo **groups;	/* ��������ʹ�õ�group����Ϣ���� */
	FDFSGroupInfo **sorted_groups; //groups order by group_name
	FDFSGroupInfo *pStoreGroup;  /* the group to store uploaded files(ָ��ĳ��group�ϴ��ļ�) */
	int current_write_group;  /* current group index to upload file(��ǰ�����ϴ��ļ���group��sorted_groups�е�����) */
	byte store_lookup;  //store to which group, from conf file(ѡ����һ�����ϴ����ϴ��ļ��ķ�ʽ)
	byte store_server;  //store to which storage server, from conf file(�ϴ��ļ�ʱͬһ������ѡ���ĸ�storage�ķ�ʽ)
	byte download_server; //download from which storage server, from conf file(�����ļ�ʱѡ���ĸ�storage�ķ�ʽ)
	byte store_path;  //store to which path, from conf file(�ϴ��ļ�ʱѡ����һ��Ŀ¼�ķ�ʽ)
	char store_group[FDFS_GROUP_NAME_MAX_LEN + 8];  //for 8 bytes aliginment(���ָ���ض��������ϴ��ļ�����Ϊ����)
} FDFSGroups;	/* ����group��Ϣ */

typedef struct
{
	FDFSGroupInfo *pGroup;		/* ��ǰ���ӵ�storage����group��Ϣ */
	FDFSStorageDetail *pStorage;	/* ��ǰ���ӵ�storage server */
	union {
		int tracker_leader;  //for notify storage servers
		int trunk_server;    //for notify other tracker servers
	} chg_count;
} TrackerClientInfo;	/* ��ǰ���ӵ������Ϣ */

typedef struct
{
	char name[FDFS_MAX_META_NAME_LEN + 1];  //key
	char value[FDFS_MAX_META_VALUE_LEN + 1]; //value
} FDFSMetaData;	/* Ԫ���ݵĽṹ��Ϣ */

typedef struct
{
	int storage_port;
	int storage_http_port;
	int store_path_count;
	int subdir_count_per_path;
	int upload_priority;
	int join_time; //storage join timestamp (create timestamp)
	int up_time;   //storage service started timestamp
        char version[FDFS_VERSION_SIZE];   //storage version
	char group_name[FDFS_GROUP_NAME_MAX_LEN + 1];	/* storage����group��name */
        char domain_name[FDFS_DOMAIN_NAME_MAX_SIZE];
        char init_flag;
	signed char status;
	int tracker_count;		/* ���õ�tracker server������ */
	ConnectionInfo tracker_servers[FDFS_MAX_TRACKERS];	/* storage�����õ�����tracker��������Ϣ */
} FDFSStorageJoinBody;	/* �¼����storage����Ϣ�ṹ */

typedef struct
{
	int server_count;		/* tracker server������ */
	int server_index;  //server index for roundrobin
	int leader_index;  /* leader server index(tracker leader����λ������) */
	ConnectionInfo *servers;	/* ����tracker server��������Ϣ */
} TrackerServerGroup;		/* ����tracker_server����Ϣ */

typedef struct
{
	char *buffer;  //the buffer pointer
	char *current; /* pointer to current position(��ǰ��ȡ����λ��) */
	int length;    /* the content length(��δ��ȡ���ݵĳ���) */
	int version;   //for binlog pre-read, compare with binlog_write_version
} BinLogBuffer;

typedef struct
{
	char id[FDFS_STORAGE_ID_MAX_SIZE];
	char group_name[FDFS_GROUP_NAME_MAX_LEN + 8];  //for 8 bytes alignment
	char ip_addr[IP_ADDRESS_SIZE];
} FDFSStorageIdInfo;	/* storage server������Ϣ�ṹ */

typedef struct
{
	char id[FDFS_STORAGE_ID_MAX_SIZE];
	char group_name[FDFS_GROUP_NAME_MAX_LEN + 1];
	char sync_src_id[FDFS_STORAGE_ID_MAX_SIZE];
} FDFSStorageSync;	/* storageͬ����Ϣ�ṹ */

typedef struct {
	char flag;	/* ��־ 1:ָ����С(MB) 2:�ٷֱ� */
	union {
		int mb;
		double ratio;
	} rs;
} FDFSStorageReservedSpace;		/* �����������洢�ռ��С ��Ϣ*/

typedef struct {
	int count;   /* store path count(�洢Ŀ¼����) */
	char **paths; /* file store paths(�洢Ŀ¼·������) */
} FDFSStorePaths;	/* storage���ݴ洢Ŀ¼�ṹ */

#endif

