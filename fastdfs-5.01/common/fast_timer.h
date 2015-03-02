#ifndef __FAST_TIMER_H__
#define __FAST_TIMER_H__

#include <stdint.h>
#include "common_define.h"

/* ʱ�����㷨��غ��� */

typedef struct fast_timer_entry {
  int64_t expires;					/* ����ʱ�� */
  void *data;						/* �ڵ������� */
  struct fast_timer_entry *prev;		/* ǰ���ڵ� */
  struct fast_timer_entry *next;		/* ��̽ڵ� */
  bool rehash;
} FastTimerEntry;		/* ���е�һ���ڵ㣬˫������ */

typedef struct fast_timer_slot {
  struct fast_timer_entry head;	/* �������е�ͷ�ڵ㣬����˫������洢 */
} FastTimerSlot;	/* ʱ�����㷨�е�һ���� */

typedef struct fast_timer {
  int slot_count;    /* time wheel slot count(ʱ�����еĲ���) */
  int64_t base_time; /* base time for slot 0(slot 0 �е�ʱ��) */
  int64_t current_time;	/* ��ǰʱ�� */
  FastTimerSlot *slots;		/* slot���� */
} FastTimer;	/* ʱ���ֽṹ�� */

#ifdef __cplusplus
extern "C" {
#endif

/* ��ʼ��ʱ���ֽṹ */
int fast_timer_init(FastTimer *timer, const int slot_count,
    const int64_t current_time);

/* ����ʱ���������Դ */
void fast_timer_destroy(FastTimer *timer);

/* ��ָ���ڵ���뵽ʱ������ */
int fast_timer_add(FastTimer *timer, FastTimerEntry *entry);

/* ɾ��ʱ���ֵ�ָ���ڵ� */
int fast_timer_remove(FastTimer *timer, FastTimerEntry *entry);

/* �޸�ָ���ڵ�Ĺ���ʱ�� */
int fast_timer_modify(FastTimer *timer, FastTimerEntry *entry,
    const int64_t new_expires);

/* ����ָ��ʱ���base_time��������slot��ָ�룬����timer->current_time��1 */
FastTimerSlot *fast_timer_slot_get(FastTimer *timer, const int64_t current_time);

/*
 * ��ȡ���й���ʱ����timer->current_time��ָ��current_time֮��Ľڵ�
 * ���еĹ��ڽڵ����ջ�����head��ָ���˫��������
 */
int fast_timer_timeouts_get(FastTimer *timer, const int64_t current_time,
   FastTimerEntry *head);

#ifdef __cplusplus
}
#endif

#endif

