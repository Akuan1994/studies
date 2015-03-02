/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "logger.h"
#include "sockopt.h"
#include "shared_func.h"
#include "storage_global.h"

volatile bool g_continue_flag = true;				/* �����Ƿ�����ı�־ */
FDFSStorePathInfo *g_path_space_list = NULL;		/* ���д洢·������Ϣ */
int g_subdir_count_per_path = DEFAULT_DATA_DIR_COUNT_PER_PATH;		/* ÿ���洢·���µ�Ŀ¼���� */

int g_server_port = FDFS_STORAGE_SERVER_DEF_PORT;		/* storage server�˿ںţ��������ļ���ȡ */

/*  
 * storage server��web server������ͨ������Ե��������web server
 * ����URL�оͿ���ͨ��������ʽ������storage server�ϵ��ļ��� 
 */
char g_http_domain[FDFS_DOMAIN_NAME_MAX_SIZE] = {0};
int g_http_port = 80;			/* http�˿� */
int g_last_server_port = 0;	/* ֮ǰ������ͨ�Ŷ˿� */
int g_last_http_port = 0;		/* ֮ǰʹ�õ�http�˿� */
int g_max_connections = DEFAULT_MAX_CONNECTONS;
int g_accept_threads = 1;
int g_work_threads = DEFAULT_WORK_THREADS;		/* �����߳��� */
int g_buff_size = STORAGE_DEFAULT_BUFF_SIZE;		/* ���ͺͽ������ݵ���󳤶� */

bool g_disk_rw_direct = false;
bool g_disk_rw_separated = true;			/* ����IO��д�Ƿ���� */
int g_disk_reader_threads = DEFAULT_DISK_READER_THREADS;	/* �����洢·���Ķ��߳��� */
int g_disk_writer_threads = DEFAULT_DISK_WRITER_THREADS;		/* �����洢·����д�߳��� */
int g_extra_open_file_flags = 0;		/* ���ļ��ĸ��ӱ�־λ */

/* �ļ���dataĿ¼�·�ɢ�洢���� */
int g_file_distribute_path_mode = FDFS_FILE_DIST_PATH_ROUND_ROBIN;

/* 
 * ������Ĳ���g_file_distribute_path_mode����Ϊ0��������ŷ�ʽ��ʱ����������Ч��
 * ��һ��Ŀ¼�µ��ļ���ŵ��ļ����ﵽ������ֵʱ
 * �����ϴ����ļ��洢����һ��Ŀ¼��
 */
int g_file_distribute_rotate_count = FDFS_FILE_DIST_DEFAULT_ROTATE_COUNT;

/* 
 * ��д����ļ�ʱ
 * ÿд��N���ֽڵ���һ��ϵͳ����fsync������ǿ��ͬ����Ӳ��
 * 0��ʾ�Ӳ�����fsync  
 */
int g_fsync_after_written_bytes = -1;

struct timeval g_network_tv = {DEFAULT_NETWORK_TIMEOUT, 0};

int g_dist_path_index_high = 0; /* current write to high path(��ǰʹ�õĶ���Ŀ¼index) */
int g_dist_path_index_low = 0;  /* current write to low path(��ǰʹ�õ�����Ŀ¼index) */
int g_dist_write_file_count = 0;  /* current write file count(��ǰĿ¼д���ļ��Ĵ���) */

int g_storage_count = 0;
FDFSStorageServer g_storage_servers[FDFS_MAX_SERVERS_EACH_GROUP];	  /* ͬgroup�е�����storage�ļ��� */
FDFSStorageServer *g_sorted_storages[FDFS_MAX_SERVERS_EACH_GROUP];	  /* ͬgroup�������ź����storage�ļ��� */

int g_tracker_reporter_count = 0;		/* tracker reporter�߳��� */
int g_heart_beat_interval  = STORAGE_BEAT_DEF_INTERVAL;		/* �����������ļ��ʱ�� */
int g_stat_report_interval = STORAGE_REPORT_DEF_INTERVAL;		/* �㱨״̬�ļ��ʱ�� */

/* ��ȡbinlog����Ҫͬ�����ļ��������ȡ�������ߵ�ʱ�� */
int g_sync_wait_usec = STORAGE_DEF_SYNC_WAIT_MSEC;
/* unit: milliseconds(ͬ����һ���ļ���ļ��ʱ��) */
int g_sync_interval = 0;
TimeInfo g_sync_start_time = {0, 0};		/* ÿ�쿪ʼͬ���ļ���ʱ�� */
TimeInfo g_sync_end_time = {23, 59};		/* ��ȡÿ�����ͬ���ļ���ʱ�� */
bool g_sync_part_time = false;				/* �Ƿ�һ���еĲ���ʱ�β�ͬ���ļ� */
/* ��־���ڴ���ˢ�µ��ļ��еļ��ʱ�� */
int g_sync_log_buff_interval = SYNC_LOG_BUFF_DEF_INTERVAL;
/* ͬ��binglog�����²�����־����Ӳ�̵�ʱ��������λΪ�� */
int g_sync_binlog_buff_interval = SYNC_BINLOG_BUFF_DEF_INTERVAL;
/* ͬ������ٸ��ļ��󣬰�storage��mark�ļ�ͬ�������� */
int g_write_mark_file_freq = FDFS_DEFAULT_SYNC_MARK_FILE_FREQ;
/* ��storage��stat�ļ�ͬ�������̵�ʱ��������λΪ��*/
int g_sync_stat_file_interval = DEFAULT_SYNC_STAT_FILE_INTERVAL;

FDFSStorageStat g_storage_stat;
int g_stat_change_count = 1;
int g_sync_change_count = 0;

int g_storage_join_time = 0;		/* storage�����ʱ�� */
int g_sync_until_timestamp = 0;	/* ���һ��ͬ��ʱ��� */
bool g_sync_old_done = false;		/* ֮ǰ��ͬ���Ƿ���� */
char g_sync_src_id[FDFS_STORAGE_ID_MAX_SIZE] = {0};		/* ͬ����Դstorage server */

char g_group_name[FDFS_GROUP_NAME_MAX_LEN + 1] = {0};		/* storage����group */
char g_my_server_id_str[FDFS_STORAGE_ID_MAX_SIZE] = {0}; /* my server id string(�����id) */
char g_tracker_client_ip[IP_ADDRESS_SIZE] = {0}; /* storage ip as tracker client(��tracker�������ӵ������ip) */
char g_last_storage_ip[IP_ADDRESS_SIZE] = {0};	 /* the last storage ip address(֮ǰ������ip��ַ) */

/* access_log��ȫ����־���� */
LogContext g_access_log_context = {LOG_INFO, STDERR_FILENO, NULL};

in_addr_t g_server_id_in_filename = 0;
bool g_use_access_log = false;    /* if log to access log(�Ƿ��ļ�������¼��access log ) */
bool g_rotate_access_log = false; /* if rotate the access log every day(�Ƿ�ÿ����תaccess log) */
/* ����error log��־�Ƿ�ÿ����ת */
bool g_rotate_error_log = false;
bool g_use_storage_id = false;    //identify storage by ID instead of IP address
byte g_id_type_in_filename = FDFS_ID_TYPE_IP_ADDRESS; //id type of the storage server in the filename
bool g_store_slave_file_use_link = false; //if store slave file use symbol link

/* 
 * �Ƿ����ϴ��ļ��Ѿ�����
 * ����Ѿ����ڣ��򲻴����ļ����ݣ�����һ�����������Խ�ʡ���̿ռ�
 * ���Ӧ��Ҫ���FastDHT ʹ�ã����Դ�ǰҪ�Ȱ�װFastDHT 
 */
bool g_check_file_duplicate = false;	/* �Ƿ����ļ��ظ� */
byte g_file_signature_method = STORAGE_FILE_SIGNATURE_METHOD_HASH;
char g_key_namespace[FDHT_MAX_NAMESPACE_LEN+1] = {0};
int g_namespace_len = 0;
int g_allow_ip_count = 0;
in_addr_t *g_allow_ip_addrs = NULL;

TimeInfo g_access_log_rotate_time = {0, 0}; /* rotate access log time base(access logÿ����תʱ���) */
TimeInfo g_error_log_rotate_time  = {0, 0}; /* rotate error log time base(error logÿ����תʱ���) */

gid_t g_run_by_gid;					/* ��ǰ�ػ���������gid */
uid_t g_run_by_uid;					/* ��ǰ�ػ���������uid */

char g_run_by_group[32] = {0};		/* �����û��� */
char g_run_by_user[32] = {0};			/* �����û� */

char g_bind_addr[IP_ADDRESS_SIZE] = {0};	/* storage server�󶨵�ip��ַ */
bool g_client_bind_addr = true;		/* storage��Ϊclientʱ�Ƿ�ʹ�ð󶨵�ip��ַ */
/* �ò������Ƶ�storage server ip�ı�ʱ��Ⱥ�Ƿ��Զ����� */
bool g_storage_ip_changed_auto_adjust = false;
bool g_thread_kill_done = false;
bool g_file_sync_skip_invalid_record = false;	/* ͬ��ʱ�Ƿ�������Ч�ļ�¼ */

/* �߳�ջ��С */
int g_thread_stack_size = 512 * 1024;

/* 
 * ��storage server��ΪԴ���������ϴ��ļ������ȼ�������Ϊ����
 * ֵԽС�����ȼ�Խ�� 
 */
int g_upload_priority = DEFAULT_UPLOAD_PRIORITY;
time_t g_up_time = 0;		/* ��������ʱ�� */

#ifdef WITH_HTTPD
FDFSHTTPParams g_http_params;
int g_http_trunk_size = 64 * 1024;
#endif

#if defined(DEBUG_FLAG) && defined(OS_LINUX)
char g_exe_name[256] = {0};
#endif

struct storage_nio_thread_data *g_nio_thread_data = NULL;	/* ÿ�������̵߳��߳����� */
struct storage_dio_thread_data *g_dio_thread_data = NULL;	/* dio�̱߳������飬ÿһ���洢·����Ӧһ�� */

/* �Ƚ�����FDFSStorageServer���󣬸���server.id�Ƚ� */
int storage_cmp_by_server_id(const void *p1, const void *p2)
{
	return strcmp((*((FDFSStorageServer **)p1))->server.id,
		(*((FDFSStorageServer **)p2))->server.id);
}

