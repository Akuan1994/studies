#ifndef HASH_H
#define    HASH_H

/* hash����ָ������ */
typedef uint32_t (*hash_func)(const void *key, size_t length);
hash_func hash;

enum hashfunc_type {
    JENKINS_HASH=0, MURMUR3_HASH
};	/* hash���������� */

/* hash��ʼ����ָ��ʹ�õ�hash���� */
int hash_init(enum hashfunc_type type);

#endif    /* HASH_H */

