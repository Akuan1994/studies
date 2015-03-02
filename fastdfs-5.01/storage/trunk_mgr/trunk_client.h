/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//trunk_client.h

#ifndef _TRUNK_CLIENT_H_
#define _TRUNK_CLIENT_H_

#include "common_define.h"
#include "tracker_types.h"
#include "trunk_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * ����Լ���trunk_server��ֱ��ΪС�ļ�����ռ�
 * ������trunk_server���ͱ���Ҫ�����ռ� 
 */
int trunk_client_trunk_alloc_space(const int file_size, \
		FDFSTrunkFullInfo *pTrunkInfo);

/* storage��trunk_serverȷ��С�ļ���״̬�Ƿ��ѷ���ռ� */
int trunk_client_trunk_alloc_confirm(const FDFSTrunkFullInfo *pTrunkInfo, \
		const int status);

/* storage��trunk_server�����ͷ�С�ļ�ռ�õĿռ� */
int trunk_client_trunk_free_space(const FDFSTrunkFullInfo *pTrunkInfo);

#ifdef __cplusplus
}
#endif

#endif

