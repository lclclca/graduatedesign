#ifndef __PS3_GENERATED_ACTIVITY_H_
#define __PS3_GENERATED_ACTIVITY_H_

/* ==============================================================
 *  FILE:    activity.h
 *  ROLE:    任务入口函数声明（Task Entry Point Declarations）
 *
 *  IMA 架构层次：线程层（Thread Layer）
 *  IMA 分区：ps3 (P3)     模块：M2 (arinckernel.module2)
 *  对应 AADL 文件：DIMA_threads.aadl
 *
 *  本文件声明 activity.c 中实现的所有任务入口函数。
 *  ps3 分区只有分区间接收端口，无分区内通信（黑板/缓冲区）。
 *
 *  运行时调用关系：
 *    OS → appMain() → CREATE_PROCESS + START
 *    → [SET_PARTITION_MODE(NORMAL)]
 *    → RTOS 按各任务的 Period 和 Priority 周期调度执行 taskXX_job()
 * ==============================================================*/

/* Periodic task: task31 (Tsk31)
 * Period=25ms  Priority=2  Stack=8KB
 * Reads: order_in (queuing DESTINATION, receives from ps1/ps2)
 */
void* task31_job(void);

/* Periodic task: task32 (Tsk32)
 * Period=50ms  Priority=3  Stack=8KB
 * Reads: order_in (queuing DESTINATION, receives from ps1/ps2)
 */
void* task32_job(void);

/* Periodic task: task33 (Tsk33)
 * Period=50ms  Priority=4  Stack=8KB
 * Reads: queueingin (queuing DESTINATION, receives from ps4/M3)
 */
void* task33_job(void);

#endif
