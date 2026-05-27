#ifndef GLOBALS_H
#define GLOBALS_H

#include <os/pos/apex/apexLib.h>
#include "gtypes.h"

/* ── 全局变量前缀：ps1_ ── */

/* 采样端口 ID */
extern SAMPLING_PORT_ID_TYPE ps1_pr1samplingout_id;

/* 队列端口 ID */
extern QUEUING_PORT_ID_TYPE  ps1_pr1queueingin_id;

/* 黑板 ID */
extern BLACKBOARD_ID_TYPE    ps1_bb_acc1_id;
extern BLACKBOARD_ID_TYPE    ps1_bb_acc2_id;
extern BLACKBOARD_ID_TYPE    ps1_bb_sem_id;
extern BLACKBOARD_ID_TYPE    ps1_bb_t3t4_id;
extern BLACKBOARD_ID_TYPE    ps1_bb_t4t3_id;

/* 缓冲区 ID */
extern BUFFER_ID_TYPE        ps1_buf_sensor_id;
extern BUFFER_ID_TYPE        ps1_buf_t3tot4_id;
extern BUFFER_ID_TYPE        ps1_buf_t4tot3_id;

/* ARINC 653 进程句柄数组（4个任务） */
extern PROCESS_ID_TYPE       arinc_threads[4];

#endif /* GLOBALS_H */