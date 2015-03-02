/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//connection_pool.h
/* ���ӳ� */
#ifndef _CONNECTION_POOL_H
#define _CONNECTION_POOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "common_define.h"
#include "pthread_func.h"
#include "hash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	int sock;		/* socket������ */
	int port;		/* �˿ں� */
	char ip_addr[IP_ADDRESS_SIZE];	/* ip��ַ */
} ConnectionInfo;	/* ������Ϣ */

struct tagConnectionManager;

typedef struct tagConnectionNode {
	ConnectionInfo *conn;		/* ������Ϣ */
	struct tagConnectionManager *manager;		/* ���ӹ������ */
	struct tagConnectionNode *next;
	time_t atime;  /* last access time(���һ������ʱ��) */
} ConnectionNode;		/* ���Ӷ���ڵ� */

typedef struct tagConnectionManager {
	ConnectionNode *head;	/* ���Ӷ�������� */
	int total_count;  /* total connections(�����ڵ������) */
	int free_count;   /* free connections(��ǰ���нڵ������) */
	pthread_mutex_t lock;
} ConnectionManager;		/* ���ӹ������ */

typedef struct tagConnectionPool {
	HashArray hash_array;  /* key is ip:port, value is ConnectionManager */
	pthread_mutex_t lock;
	int connect_timeout;		/* ���ӳ�ʱʱ�� */
	int max_count_per_entry;  /* 0 means no limit��ÿ��hashͰ��������� */

	/*
	connections whose the idle time exceeds this time will be closed
	*/
	int max_idle_time;	/* ������ʱ�� */
} ConnectionPool;	/* ���ӳ� */


/* ��ʼ�����ӳ� */
int conn_pool_init(ConnectionPool *cp, int connect_timeout, \
	const int max_count_per_entry, const int max_idle_time);

/* �������ӳ���Դ */
void conn_pool_destroy(ConnectionPool *cp);

/* 
 * ��ȡһ��ָ��ip:port�����ӣ����ض�Ӧ��������Ϣ
 * ������ӳ��в����ڣ��½���Ӧ��ConnectionManager����������ָ��ip��port
 */
ConnectionInfo *conn_pool_get_connection(ConnectionPool *cp, 
	const ConnectionInfo *conn, int *err_no);

/* �ͷ�������Դ�����˽ڵ������нڵ������У�free_count��1 */
#define conn_pool_close_connection(cp, conn) \
	conn_pool_close_connection_ex(cp, conn, false)

/* 
 * �رպ�ָ��ip:port֮������� 
 * bForceΪtrue����ǿ�ƶϿ����ӣ�����free_count��1��������ĶϿ�socket����
 */
int conn_pool_close_connection_ex(ConnectionPool *cp, ConnectionInfo *conn, 
	const bool bForce);

/* �Ͽ��ʹ�ip:port������ */
void conn_pool_disconnect_server(ConnectionInfo *pConnection);

/* ����pConnection���洢��ip��port������ */
int conn_pool_connect_server(ConnectionInfo *pConnection, \
		const int connect_timeout);

/* ��ȡ���ӳ��е����нڵ��� */
int conn_pool_get_connection_count(ConnectionPool *cp);

#ifdef __cplusplus
}
#endif

#endif

