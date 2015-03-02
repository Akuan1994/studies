
#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "common_define.h"

typedef struct tagAVLTreeNode {
	void *data;					/* ���� */
	struct tagAVLTreeNode *left;	/* ������ */
	struct tagAVLTreeNode *right;	/* ������ */
	byte balance;				/* ƽ�����ӣ���������ȼ�ȥ��������ȵ�ֵ */
} AVLTreeNode;	/* ƽ��������ڵ� */

typedef int (*DataOpFunc) (void *data, void *args);

typedef struct tagAVLTreeInfo {
	AVLTreeNode *root;			/* ���ڵ� */
	FreeDataFunc free_data_func;	/* �ͷ�ĳ���ڵ�ĺ���ָ�� */
	CompareFunc compare_func;	/* �Ƚ�Ԫ�ش�С�ĺ���ָ�� */
} AVLTreeInfo;		/* ƽ���������Ϣ */

#ifdef __cplusplus
extern "C" {
#endif

/* ��ʼ��AVL���� */
int avl_tree_init(AVLTreeInfo *tree, FreeDataFunc free_data_func, \
	CompareFunc compare_func);
/* �ݹ�����ƽ������� */
void avl_tree_destroy(AVLTreeInfo *tree);

/* ��ƽ��������в���һ������ */
int avl_tree_insert(AVLTreeInfo *tree, void *data);
int avl_tree_replace(AVLTreeInfo *tree, void *data);
/* ��AVL����ɾ��һ���ڵ� */
int avl_tree_delete(AVLTreeInfo *tree, void *data);
/* ��AVL���в���ĳ���ڵ� */
void *avl_tree_find(AVLTreeInfo *tree, void *target_data);
/* �ݹ���Һ�target_dataһ����ֵ�����û�У��ͷ��ص�һ���Դ�һЩ��ֵ */
void *avl_tree_find_ge(AVLTreeInfo *tree, void *target_data);
/* �������avl����ÿһ���ڵ㣬ִ��data_op_func�ص����� */
int avl_tree_walk(AVLTreeInfo *tree, DataOpFunc data_op_func, void *args);
/* ����AVL���Ľڵ���� */
int avl_tree_count(AVLTreeInfo *tree);
/*����AVL������� */
int avl_tree_depth(AVLTreeInfo *tree);
//void avl_tree_print(AVLTreeInfo *tree);

#ifdef __cplusplus
}
#endif

#endif
