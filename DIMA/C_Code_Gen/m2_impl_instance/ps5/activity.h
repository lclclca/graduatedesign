#ifndef __PS5_GENERATED_ACTIVITY_H_
#define __PS5_GENERATED_ACTIVITY_H_

/* ==============================================================
 *  FILE:    activity.h
 *  ROLE:    任务入口函数声明（Task Entry Point Declarations）
 *
 *  IMA 架构层次：线程层（Thread Layer）
 *  IMA 分区：ps5 (P5)     模块：M2 (arinckernel.module2)
 *  对应 AADL 文件：DIMA_threads.aadl
 *
 *  本文件声明 activity.c 中实现的所有任务入口函数。
 *  ps5 分区只有分区间端口，无分区内通信（黑板/缓冲区）。
 *
 *  运行时调用关系：
 *    OS → appMain() → CREATE_PROCESS + START
 *    → [SET_PARTITION_MODE(NORMAL)]
 *    → RTOS 按各任务的 Period 和 Priority 周期调度执行 taskXX_job()
 * ==============================================================*/

/* Periodic task: task51 (Tsk51)
 * Period=50ms  Priority=1  Stack=8KB
 * Reads: order_in (queuing DESTINATION, receives from ps1/ps2)
 */
void* task51_job(void);

/* Periodic task: task52 (Tsk52)
 * Period=50ms  Priority=2  Stack=8KB
 * Reads:  order_in    (queuing DESTINATION, receives from ps1/ps2)
 * Writes: queueingout (queuing SOURCE, sends to ps4/M3)
 */
void* task52_job(void);

/* Periodic task: task53 (Tsk53)
 * Period=200ms  Priority=3  Stack=8KB
 * No inter- or intra-partition communication (computation-only task)
 */
void* task53_job(void);

#endif
