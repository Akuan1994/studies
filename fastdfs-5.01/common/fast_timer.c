#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "logger.h"
#include "fast_timer.h"

/* ��ʼ��ʱ���ֽṹ */
int fast_timer_init(FastTimer *timer, const int slot_count,
    const int64_t current_time)
{
  int bytes;
  if (slot_count <= 0 || current_time <= 0) {
    return EINVAL;
  }

  /* ����ʱ���ֲ��� */
  timer->slot_count = slot_count;
  /* ����ʱ����base_time */
  timer->base_time = current_time; //base time for slot 0
  timer->current_time = current_time;
  bytes = sizeof(FastTimerSlot) * slot_count;
  /* ΪFastTimerSlot����ռ� */
  timer->slots = (FastTimerSlot *)malloc(bytes);
  if (timer->slots == NULL) {
     return errno != 0 ? errno : ENOMEM;
  }
  memset(timer->slots, 0, bytes);
  return 0;
}

/* ����ʱ���������Դ */
void fast_timer_destroy(FastTimer *timer)
{
  if (timer->slots != NULL) {
    free(timer->slots);
    timer->slots = NULL;
  }
}

/* ����ָ��ʱ���base_time�������ĸ�slot�� */
#define TIMER_GET_SLOT_INDEX(timer, expires) \
  (((expires) - timer->base_time) % timer->slot_count)

/* ����ָ��ʱ���base_time��������slot��ָ�� */
#define TIMER_GET_SLOT_POINTER(timer, expires) \
  (timer->slots + TIMER_GET_SLOT_INDEX(timer, expires))

/* ��ָ���ڵ���뵽ʱ������ */
int fast_timer_add(FastTimer *timer, FastTimerEntry *entry)
{
  FastTimerSlot *slot;

  /* ����Ҫ��ӵĽڵ�Ĺ���ʱ���ȡ����slotָ�� */
  slot = TIMER_GET_SLOT_POINTER(timer, entry->expires >
     timer->current_time ? entry->expires : timer->current_time);
  /* �������ڵ���ӵ�����slot��head֮�� */
  entry->next = slot->head.next;
  if (slot->head.next != NULL) {
    slot->head.next->prev = entry;
  }
  entry->prev = &slot->head;
  slot->head.next = entry;
  return 0;
}

/* �޸�ָ���ڵ�Ĺ���ʱ�� */
int fast_timer_modify(FastTimer *timer, FastTimerEntry *entry,
    const int64_t new_expires)
{
  if (new_expires == entry->expires) {
    return 0;
  }

  /* ����µĹ���ʱ��С�ھɵ�ʱ�䣬ɾ������ڵ㣬�Ե�ǰʱ������һ���ڵ� */
  if (new_expires < entry->expires) {
    fast_timer_remove(timer, entry);
    entry->expires = new_expires;
    return fast_timer_add(timer, entry);
  }

  /* ����µĹ���ʱ�������slot��ԭ������ͬһ������Ҫrehash */
  entry->rehash = TIMER_GET_SLOT_INDEX(timer, new_expires) !=
      TIMER_GET_SLOT_INDEX(timer, entry->expires);
  entry->expires = new_expires;  //lazy move
  return 0;
}

/* ɾ��ʱ���ֵ�ָ���ڵ� */
int fast_timer_remove(FastTimer *timer, FastTimerEntry *entry)
{
  /* ��˫��������ɾ��ָ���ڵ� */
  if (entry->prev == NULL) {
     return ENOENT;   //already removed
  }

  if (entry->next != NULL) {
     entry->next->prev = entry->prev;
     entry->prev->next = entry->next;
     entry->next = NULL;
  }
  else {
     entry->prev->next = NULL;
  }

  entry->prev = NULL;
  return 0;
}

/* ����ָ��ʱ���base_time��������slot��ָ�룬����timer->current_time��1 */
FastTimerSlot *fast_timer_slot_get(FastTimer *timer, const int64_t current_time)
{
  /* �����ǰʱ��С��timerʱ��ĵ�ǰʱ�䣬����NULL */
  if (timer->current_time >= current_time) {
    return NULL;
  }

  /* ���ص�ǰʱ������slot������timer�ĵ�ǰʱ���1 */
  return TIMER_GET_SLOT_POINTER(timer, timer->current_time++);
}

/*
 * ��ȡ���й���ʱ����timer->current_time��ָ��current_time֮��Ľڵ�
 * ���еĹ��ڽڵ����ջ�����head��ָ���˫��������
 */
int fast_timer_timeouts_get(FastTimer *timer, const int64_t current_time,
   FastTimerEntry *head)
{
  FastTimerSlot *slot;
  FastTimerEntry *entry;
  FastTimerEntry *first;
  FastTimerEntry *last;
  FastTimerEntry *tail;
  int count;	/* ��¼���ڽڵ��� */
  head->prev = NULL;
  head->next = NULL;
  if (timer->current_time >= current_time) {
    return 0;
  }

  first = NULL;
  last = NULL;
  tail = head;
  count = 0;
  /* 
   * ��timer->current_time��ʼֱ��current_time��ÿ��ѭ��+1�������������й��ڽڵ�
   * ���ڽڵ���ӵ�head��ָ���������
   */
  while (timer->current_time < current_time) 
  {
    /* ��ȡʱ���ֵ�ǰʱ������slot */
    slot = TIMER_GET_SLOT_POINTER(timer, timer->current_time++);
    /* entryָ��slot�ĵ�һ����Ч�ڵ� */
    entry = slot->head.next;
    while (entry != NULL) 
    {
      /* û�дﵽ����ʱ��Ľڵ� */
      if (entry->expires >= current_time)
      {
      	  /* ��ʱfirstָ��ǰ���һ���Ѿ����ڵĽڵ� */
         if (first != NULL) 
	  {
	     /* ��ʱ���ֵ�slot���������޳����ڵĽڵ㣬���뵽head��ָ��������� */
            first->prev->next = entry;
            entry->prev = first->prev;

            tail->next = first;
            first->prev = tail;
            tail = last;	/* ��ʱ��lastָ�������Ĺ��ڽڵ��е����һ�� */
            first = NULL;
         }
	  /* ����˽ڵ�û�й��ڲ�����Ҫrehash�����·���slot����ɾ������� */
         if (entry->rehash) 
	  {
           last = entry;
           entry = entry->next;

           last->rehash = false;
           fast_timer_remove(timer, last);
           fast_timer_add(timer, last);
           continue;
         }
      }
      /* �������ʱ��Ľڵ� */
      else 
      {
         count++;
	  /* ������������ڽڵ��еĵ�һ����firstָ��ýڵ� */
         if (first == NULL) {
            first = entry;
         }
      }
	  
      last = entry;
      entry = entry->next;
    }

    /* ������һ���ڵ��ǹ��ڽڵ� */
    if (first != NULL) 
    {
       first->prev->next = NULL;

       tail->next = first;
       first->prev = tail;
       tail = last;
       first = NULL;
    }
  }

  /* ����й��ڽڵ㣬tail->nextָ��NULL */
  if (count > 0) 
  {
     tail->next = NULL;
  }

  return count;
}

