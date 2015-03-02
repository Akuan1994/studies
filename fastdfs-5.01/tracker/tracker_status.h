/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//tracker_status.h

#ifndef _TRACKER_STATUS_H_
#define _TRACKER_STATUS_H_

#include <time.h>

typedef struct {
	time_t up_time;			/* tracker��������ʱ�� */
	time_t last_check_time;	/* tracker״̬���ʱ�� */
} TrackerStatus;	/* tracker������״̬ */

#ifdef __cplusplus
extern "C" {
#endif

/* ��tracker������״̬��Ϣд���ļ� */
int tracker_write_status_to_file(void *args);

/* ���ļ��л�ȡtracker server��״̬ */
int tracker_load_status_from_file(TrackerStatus *pStatus);

#ifdef __cplusplus
}
#endif

#endif
