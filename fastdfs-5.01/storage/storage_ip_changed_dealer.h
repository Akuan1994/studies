/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//storage_ip_changed_dealer.h

#ifndef _STORAGE_IP_CHANGED_DEALER_H_
#define _STORAGE_IP_CHANGED_DEALER_H_

#include "tracker_types.h"
#include "tracker_client_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ��ȡ��tracker�������ӵ������ip��ַ */
int storage_get_my_tracker_client_ip();

/* 
 * ������tracker���ͱ��Ļ�ȡchangelog�ļ������� ��ֱ����һ̨�ɹ�����
 * ���ͱ���:�ջ���group_name+storage_id 
 * ���ر���:changelog�ļ��е�һ�Σ���pTask->storage��ƫ���������� 
 * ������ͬ���storage�ı����¼���޸Ļ���������ص�����sync��trunk��mark_file�ļ�
 */
int storage_changelog_req();

/* 
 * ���ip�ı����Ҫ��Ⱥ�Զ���������tracker���ͱ���֪ͨ
 * ����ȡchangelog�ļ����� �б����storage���Ӧ��mark_file�ļ�
 */
int storage_check_ip_changed();

#ifdef __cplusplus
}
#endif

#endif

