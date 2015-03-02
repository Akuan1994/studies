/**
* Copyright (C) 2012 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//trunk_free_block_checker.h

#ifndef _TRUNK_FREE_BLOCK_CHECKER_H_
#define _TRUNK_FREE_BLOCK_CHECKER_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "common_define.h"
#include "fdfs_global.h"
#include "tracker_types.h"
#include "trunk_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	FDFSTrunkPathInfo path;  //trunk file path
	int id;                  //trunk file id
} FDFSTrunkFileIdentifier;		/* trunk_file��ʶ����Ϣ */

typedef struct {
	int alloc;  /* alloc block count(�ܹ����������) */
	int count;  /* block count(һ��trunk_file��С�ļ�������) */
	FDFSTrunkFullInfo **blocks;   /* sort by FDFSTrunkFullInfo.file.offset(����ƫ���������С�ļ���Ӧ�Ķ����s�ڴ�ռ�) */
} FDFSBlockArray;		/* һ��trunk_file�е��ļ��ռ����� */

typedef struct {
	FDFSTrunkFileIdentifier trunk_file_id;	/* trunk_file��path��id */
	FDFSBlockArray block_array;			/* С�ļ���Ϣ���� */
} FDFSTrunksById;		/* ����id�洢��trunk_file��Ϣ */

/* ��ʼ��tree_info_by_id���avl����һ���ڵ����һ�����trunk_file�ļ� */
int trunk_free_block_checker_init();

/* ����tree_info_by_id���avl�� */
void trunk_free_block_checker_destroy();

/* ����tree_info_by_id������Ľڵ���(��trunk_file������) */
int trunk_free_block_tree_node_count();

/* ����tree_info_by_id������С�ļ������� */
int trunk_free_block_total_count();

/* �鿴pTrunkInfo���С�ļ��Ƿ��Ѿ����ڣ������ڷ���0 */
int trunk_free_block_check_duplicate(FDFSTrunkFullInfo *pTrunkInfo);

/* ��pTrunkInfo���С�ļ�����Ϣ���뵽tree_info_by_id��avl���� */
int trunk_free_block_insert(FDFSTrunkFullInfo *pTrunkInfo);

/* ��tree_info_by_id���ж�Ӧtrunk_file_id�Ľڵ��������ɾ��pTrunkInfo���С�ļ�����Ϣ */
int trunk_free_block_delete(FDFSTrunkFullInfo *pTrunkInfo);

/* ��tree_info_by_id������е���Ϣdump��filenameָ�����ļ��� */
int trunk_free_block_tree_print(const char *filename);

#ifdef __cplusplus
}
#endif

#endif

