/*============================================================
 * globals.h
 * 全局 ARINC 653 对象句柄声明 —— ps2 分区
 *============================================================*/
#ifndef GLOBALS_H
#define GLOBALS_H

#include <apex/apex.h>
#include "gtypes.h"

/*------------------------------------------------------------
 * 任务句柄（3个任务）
 *------------------------------------------------------------*/
extern PROCESS_ID_TYPE  g_task21_id;
extern PROCESS_ID_TYPE  g_task22_id;
extern PROCESS_ID_TYPE  g_task23_id;

/*------------------------------------------------------------
 * 采样端口句柄（1个 DESTINATION 端口）
 *------------------------------------------------------------*/
extern SAMPLING_PORT_ID_TYPE  g_pr2samplingin_id;

/*------------------------------------------------------------
 * 队列端口句柄（1个 SOURCE 端口）
 *------------------------------------------------------------*/
extern QUEUING_PORT_ID_TYPE   g_pr2queueingout_id;

/*------------------------------------------------------------
 * 黑板句柄（3块）
 *------------------------------------------------------------*/
extern BLACKBOARD_ID_TYPE  g_bb_acc3_id;
extern BLACKBOARD_ID_TYPE  g_bb_t2t3_id;
extern BLACKBOARD_ID_TYPE  g_bb_t3t2_id;

/*------------------------------------------------------------
 * 缓冲区句柄（3块）
 *------------------------------------------------------------*/
extern BUFFER_ID_TYPE  g_buf_order_id;
extern BUFFER_ID_TYPE  g_buf_t2tot3_id;
extern BUFFER_ID_TYPE  g_buf_t3tot2_id;

#endif /* GLOBALS_H */