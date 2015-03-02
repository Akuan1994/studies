/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//tracker_client_thread.h

#ifndef _TRACKER_CLIENT_THREAD_H_
#define _TRACKER_CLIENT_THREAD_H_

#include "tracker_types.h"
#include "storage_sync.h"

#ifdef __cplusplus
extern "C" {
#endif

/* report�����Դ��ʼ������ */
int tracker_report_init();

 /* ����ͬtracker�����߳���Դ */
int tracker_report_destroy();

/* ����ͬ����tracker server�������߳� */
int tracker_report_thread_start();

/* ֹͣ���е�ͬtracker�������߳� */
int kill_tracker_report_threads();

/* ��tracker���ͱ��ģ��������ָ��group */
int tracker_report_join(ConnectionInfo *pTrackerServer, \
		const int tracker_index, const bool sync_old_done);

/*  
 * ��tracker���ͱ��Ļ㱨ָ��storage��״̬��Ϣ
 * ������:group_name��֮����һ��FDFSStorageBrief�ṹ�ı���
 * ���ر���:��
 */
int tracker_report_storage_status(ConnectionInfo *pTrackerServer, \
		FDFSStorageBrief *briefServer);

/* 
 * ��ȡpReader->storage_id��Ӧ��Դstorage��ͬ��ʱ��� 
 * ���ͱ���:group_name+storage_id 
 * ���ر���:ָ����storage��TrackerStorageSyncReqBody(����Դstorage��ͬ��ʱ���) 
 */
int tracker_sync_src_req(ConnectionInfo *pTrackerServer, \
		StorageBinLogReader *pReader);

/* ���¶��storage_server��״̬��Ϣ */
int tracker_sync_diff_servers(ConnectionInfo *pTrackerServer, \
		FDFSStorageBrief *briefServers, const int server_count);

/* 
 * ��ָ��tracker���ͱ��Ļ�ȡchangelog�ļ������� 
 * �������ر���
 * ���ر���:changelog�ļ��е�һ�Σ���pTask->storage��ƫ���������� 
 * ������ͬ���storage�ı����¼���޸Ļ���������ص�����sync��trunk��mark_file�ļ�
 */
int tracker_deal_changelog_response(ConnectionInfo *pTrackerServer);

#ifdef __cplusplus
}
#endif

#endif
