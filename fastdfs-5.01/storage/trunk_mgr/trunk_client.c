/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//trunk_client.c

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "fdfs_define.h"
#include "logger.h"
#include "fdfs_global.h"
#include "sockopt.h"
#include "shared_func.h"
#include "tracker_proto.h"
#include "storage_global.h"
#include "trunk_client.h"

/* 
 * storage向trunk_server发送为小文件分配空间的报文
 * 发送报文:group_name + file_size(4字节) + store_path_index(1字节)
 * 返回报文:FDFSTrunkInfoBuff对象
 */
static int trunk_client_trunk_do_alloc_space(ConnectionInfo *pTrunkServer, \
		const int file_size, FDFSTrunkFullInfo *pTrunkInfo)
{
	TrackerHeader *pHeader;
	char *p;
	int result;
	char out_buff[sizeof(TrackerHeader) + FDFS_GROUP_NAME_MAX_LEN + 5];
	FDFSTrunkInfoBuff trunkBuff;
	int64_t in_bytes;

	pHeader = (TrackerHeader *)out_buff;
	memset(out_buff, 0, sizeof(out_buff));
	p = out_buff + sizeof(TrackerHeader);
	/* 写入group_name */
	snprintf(p, sizeof(out_buff) - sizeof(TrackerHeader), \
		"%s", g_group_name);
	p += FDFS_GROUP_NAME_MAX_LEN;
	/* 写入file_size */
	int2buff(file_size, p);
	p += 4;
	/* 写入要存储的store_path_index */
	*p++ = pTrunkInfo->path.store_path_index;
	long2buff(FDFS_GROUP_NAME_MAX_LEN + 5, pHeader->pkg_len);
	pHeader->cmd = STORAGE_PROTO_CMD_TRUNK_ALLOC_SPACE;

	if ((result=tcpsenddata_nb(pTrunkServer->sock, out_buff, \
			sizeof(out_buff), g_fdfs_network_timeout)) != 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"send data to storage server %s:%d fail, " \
			"errno: %d, error info: %s", __LINE__, \
			pTrunkServer->ip_addr, pTrunkServer->port, \
			result, STRERROR(result));

		return result;
	}

	p = (char *)&trunkBuff;
	if ((result=fdfs_recv_response(pTrunkServer, \
		&p, sizeof(FDFSTrunkInfoBuff), &in_bytes)) != 0)
	{
		return result;
	}

	/* 返回报文为一个FDFSTrunkInfoBuff对象，说明分配空间成功 */
	if (in_bytes != sizeof(FDFSTrunkInfoBuff))
	{
		logError("file: "__FILE__", line: %d, " \
			"storage server %s:%d, recv body length: %d invalid, " \
			"expect body length: %d", __LINE__, \
			pTrunkServer->ip_addr, pTrunkServer->port, \
			(int)in_bytes, (int)sizeof(FDFSTrunkInfoBuff));
		return EINVAL;
	}

	pTrunkInfo->path.store_path_index = trunkBuff.store_path_index;
	pTrunkInfo->path.sub_path_high = trunkBuff.sub_path_high;
	pTrunkInfo->path.sub_path_low = trunkBuff.sub_path_low;
	pTrunkInfo->file.id = buff2int(trunkBuff.id);
	pTrunkInfo->file.offset = buff2int(trunkBuff.offset);
	pTrunkInfo->file.size = buff2int(trunkBuff.size);
	pTrunkInfo->status = FDFS_TRUNK_STATUS_HOLD;

	return 0;
}

/* 
 * 如果自己是trunk_server，直接为小文件分配空间
 * 否则向trunk_server发送报文要求分配空间 
 */
int trunk_client_trunk_alloc_space(const int file_size, \
		FDFSTrunkFullInfo *pTrunkInfo)
{
	int result;
	ConnectionInfo trunk_server;
	ConnectionInfo *pTrunkServer;

	/* 如果自己就是trunk server */
	if (g_if_trunker_self)
	{
		/* 直接分配内存 */
		return trunk_alloc_space(file_size, pTrunkInfo);
	}

	if (*(g_trunk_server.ip_addr) == '\0')
	{
		logError("file: "__FILE__", line: %d, " \
			"no trunk server", __LINE__);
		return EAGAIN;
	}

	/* 建立和trunk_server的连接 */
	memcpy(&trunk_server, &g_trunk_server, sizeof(ConnectionInfo));
	if ((pTrunkServer=tracker_connect_server(&trunk_server, &result)) == NULL)
	{
		logError("file: "__FILE__", line: %d, " \
			"can't alloc trunk space because connect to trunk " \
			"server %s:%d fail, errno: %d", __LINE__, \
			trunk_server.ip_addr, trunk_server.port, result);
		return result;
	}

	/* 
	 * storage向trunk_server发送为小文件分配空间的报文
	 * 发送报文:group_name + file_size(4字节) + store_path_index(1字节)
	 * 返回报文:FDFSTrunkInfoBuff对象
	 */
	result = trunk_client_trunk_do_alloc_space(pTrunkServer, \
			file_size, pTrunkInfo);

	tracker_disconnect_server_ex(pTrunkServer, result != 0);
	return result;
}

/* 
 * storage向trunk_server发送确认小文件的状态的报文
 * 发送报文:group_name + FDFSTrunkInfoBuff对象
 * 返回报文:无
 */
#define trunk_client_trunk_do_alloc_confirm(pTrunkServer, pTrunkInfo, status) \
	trunk_client_trunk_confirm_or_free(pTrunkServer, pTrunkInfo, \
		STORAGE_PROTO_CMD_TRUNK_ALLOC_CONFIRM, status)

/* 
 * storage向trunk_server发送释放指向小文件空间的报文
 * 发送报文:group_name + FDFSTrunkInfoBuff对象
 * 返回报文:无
 */
#define trunk_client_trunk_do_free_space(pTrunkServer, pTrunkInfo) \
	trunk_client_trunk_confirm_or_free(pTrunkServer, pTrunkInfo, \
		STORAGE_PROTO_CMD_TRUNK_FREE_SPACE, 0)

/* 
 * storage向trunk_server发送确认小文件的状态或者释放空间的报文
 * 发送报文:group_name + FDFSTrunkInfoBuff对象
 * 返回报文:无
 */
static int trunk_client_trunk_confirm_or_free(ConnectionInfo *pTrunkServer,\
		const FDFSTrunkFullInfo *pTrunkInfo, const int cmd, \
		const int status)
{
	TrackerHeader *pHeader;
	FDFSTrunkInfoBuff *pTrunkBuff;
	int64_t in_bytes;
	int result;
	char out_buff[sizeof(TrackerHeader) \
		+ STORAGE_TRUNK_ALLOC_CONFIRM_REQ_BODY_LEN];

	pHeader = (TrackerHeader *)out_buff;
	pTrunkBuff = (FDFSTrunkInfoBuff *)(out_buff + sizeof(TrackerHeader) \
			 + FDFS_GROUP_NAME_MAX_LEN);
	memset(out_buff, 0, sizeof(out_buff));
	/* 写入group_name */
	snprintf(out_buff + sizeof(TrackerHeader), sizeof(out_buff) - \
		sizeof(TrackerHeader),  "%s", g_group_name);
	long2buff(STORAGE_TRUNK_ALLOC_CONFIRM_REQ_BODY_LEN, pHeader->pkg_len);
	pHeader->cmd = cmd;
	pHeader->status = status;

	/* 写入小文件的FDFSTrunkInfoBuff信息 */
	pTrunkBuff->store_path_index = pTrunkInfo->path.store_path_index;
	pTrunkBuff->sub_path_high = pTrunkInfo->path.sub_path_high;
	pTrunkBuff->sub_path_low = pTrunkInfo->path.sub_path_low;
	int2buff(pTrunkInfo->file.id, pTrunkBuff->id);
	int2buff(pTrunkInfo->file.offset, pTrunkBuff->offset);
	int2buff(pTrunkInfo->file.size, pTrunkBuff->size);

	if ((result=tcpsenddata_nb(pTrunkServer->sock, out_buff, \
			sizeof(out_buff), g_fdfs_network_timeout)) != 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"send data to storage server %s:%d fail, " \
			"errno: %d, error info: %s", __LINE__, \
			pTrunkServer->ip_addr, pTrunkServer->port, \
			result, STRERROR(result));

		return result;
	}

	if ((result=fdfs_recv_header(pTrunkServer, &in_bytes)) != 0)
	{
		return result;
	}

	if (in_bytes != 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"storage server %s:%d response data " \
			"length: "INT64_PRINTF_FORMAT" is invalid, " \
			"should == 0", __LINE__, pTrunkServer->ip_addr, \
			pTrunkServer->port, in_bytes);
		return EINVAL;
	}

	return 0;
}

/* storage向trunk_server确认小文件的状态是否已分配空间 */
int trunk_client_trunk_alloc_confirm(const FDFSTrunkFullInfo *pTrunkInfo, \
		const int status)
{
	int result;
	ConnectionInfo trunk_server;
	ConnectionInfo *pTrunkServer;

	/* 如果自己就是trunk_server，直接确认其状态 */
	if (g_if_trunker_self)
	{
		return trunk_alloc_confirm(pTrunkInfo, status);
	}

	if (*(g_trunk_server.ip_addr) == '\0')
	{
		return EAGAIN;
	}

	memcpy(&trunk_server, &g_trunk_server, sizeof(ConnectionInfo));
	if ((pTrunkServer=tracker_connect_server(&trunk_server, &result)) == NULL)
	{
		logError("file: "__FILE__", line: %d, " \
			"trunk alloc confirm fail because connect to trunk " \
			"server %s:%d fail, errno: %d", __LINE__, \
			trunk_server.ip_addr, trunk_server.port, result);
		return result;
	}

	/* 
	 * storage向trunk_server发送确认小文件的状态的报文
	 * 发送报文:group_name + FDFSTrunkInfoBuff对象
	 * 返回报文:无
	 */
	result = trunk_client_trunk_do_alloc_confirm(pTrunkServer, \
			pTrunkInfo, status);

	tracker_disconnect_server_ex(pTrunkServer, result != 0);
	return result;
}

/* storage向trunk_server请求释放小文件占用的空间 */
int trunk_client_trunk_free_space(const FDFSTrunkFullInfo *pTrunkInfo)
{
	int result;
	ConnectionInfo trunk_server;
	ConnectionInfo *pTrunkServer;

	if (g_if_trunker_self)
	{
		/* 直接释放小文件pTrunkInfo占用的内存空间，供以后其他文件复用*/
		return trunk_free_space(pTrunkInfo, true);
	}

	if (*(g_trunk_server.ip_addr) == '\0')
	{
		return EAGAIN;
	}

	memcpy(&trunk_server, &g_trunk_server, sizeof(ConnectionInfo));
	if ((pTrunkServer=tracker_connect_server(&trunk_server, &result)) == NULL)
	{
		logError("file: "__FILE__", line: %d, " \
			"free trunk space fail because connect to trunk " \
			"server %s:%d fail, errno: %d", __LINE__, \
			trunk_server.ip_addr, trunk_server.port, result);
		return result;
	}

	/* 
	 * storage向trunk_server发送释放指向小文件空间的报文
	 * 发送报文:group_name + FDFSTrunkInfoBuff对象
	 * 返回报文:无
	 */
	result = trunk_client_trunk_do_free_space(pTrunkServer, pTrunkInfo);
	tracker_disconnect_server_ex(pTrunkServer, result != 0);
	return result;
}

