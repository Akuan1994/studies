/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fdfs_client.h"
#include "logger.h"

static void usage(char *argv[])
{
	printf("Usage: %s <config_file> <local_filename> " \
		"[storage_ip:port] [store_path_index]\n", argv[0]);
}

int main(int argc, char *argv[])
{
	char *conf_filename;		//�����ļ���
	char *local_filename;		//Ҫ�ϴ����ļ���
	char group_name[FDFS_GROUP_NAME_MAX_LEN + 1];	//Group Name
	ConnectionInfo *pTrackerServer;
	int result;
	int store_path_index;
	ConnectionInfo storageServer;
	char file_id[128];
	
	if (argc < 3)	//�����������3������ʾ������Ϣ
	{
		usage(argv);
		return 1;
	}

	log_init();
	g_log_context.log_level = LOG_ERR;
	ignore_signal_pipe();

	conf_filename = argv[1];
	if ((result=fdfs_client_init(conf_filename)) != 0)	//���ݲ����������ļ����пͻ��˳�ʼ��
	{
		return result;
	}

	pTrackerServer = tracker_get_connection();	//��ȡ��TrackerServer������
	if (pTrackerServer == NULL)		//���û���ҵ�TrackerServer����رտͻ���
	{
		fdfs_client_destroy();
		return errno != 0 ? errno : ECONNREFUSED;
	}

	local_filename = argv[2];	//�Ӳ����л�ȡ�����ϴ��ļ���
	*group_name = '\0';
	if (argc >= 4)		//������ĸ����������ϣ��û�ָ����Ҫ���ӵ�Storage Server������������ѯ���õ�Storage Server
	{
		const char *pPort;
		const char *pIpAndPort;

		pIpAndPort = argv[3];	//���ĸ�����Ӧ������Ҫָ����Storage Server��IP��ַ���˿ں�
		pPort = strchr(pIpAndPort, ':');
		if (pPort == NULL)	//������ĸ�������û��":"�ţ��˿ںţ����رտͻ��ˣ���ʾ������Ϣ
		{
			fdfs_client_destroy();
			fprintf(stderr, "invalid storage ip address and " \
				"port: %s\n", pIpAndPort);
			usage(argv);
			return 1;
		}

		storageServer.sock = -1;
		snprintf(storageServer.ip_addr, sizeof(storageServer.ip_addr), 	//����IP��ַ����ȡ������Ķ˿ں�
			 "%.*s", (int)(pPort - pIpAndPort), pIpAndPort);
		storageServer.port = atoi(pPort + 1);	//ȥ����ð�ţ���ȡ�˿ں�
		if (argc >= 5)
		{
			store_path_index = atoi(argv[4]);
		}
		else
		{
			store_path_index = -1;
		}
	}
	//��ȡTracker���Ӻ����Tracker��ѯ��ǰ�ĸ�Storage����
	else if ((result=tracker_query_storage_store(pTrackerServer,
	          &storageServer, group_name, &store_path_index)) != 0)
	{
		fdfs_client_destroy();
		fprintf(stderr, "tracker_query_storage fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
		return result;
	}
	
	result = storage_upload_by_filename1(pTrackerServer, \
			&storageServer, store_path_index, \
			local_filename, NULL, \
			NULL, 0, group_name, file_id);
	if (result == 0)	//�ļ��ϴ��ɹ�����ʾ�ļ�ID�������������ļ���
	{
		printf("%s\n", file_id);
	}
	else
	{
		fprintf(stderr, "upload file fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
	}

	tracker_disconnect_server_ex(pTrackerServer, true);		//�رպ�Tracker Server������
	fdfs_client_destroy();

	return result;
}

