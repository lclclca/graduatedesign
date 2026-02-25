#ifndef __IMA2C_GENERATED_ACTIVITY_H_
#define __IMA2C_GENERATED_ACTIVITY_H_

/* ==============================================================
 *  FILE:    activity.h
 *  ROLE:    任务入口函数声明（Task Entry Point Declarations）
 *
 *  IMA 架构层次：线程层（Thread Layer）
 *  IMA 分区：ps4 (P4)     模块：M3 (arinckernel.module3)
 *  对应 AADL 文件：DIMA_threads.aadl
 *
 *  本文件声明 activity.c 中实现的所有任务入口函数。
 *  ps4 分区只有分区间端口，无分区内通信（黑板/缓冲区）。
 *
 *  运行时调用关系：
 *    OS → appMain() → CREATE_PROCESS + START
 *    → [SET_PARTITION_MODE(NORMAL)]
 *    → RTOS 按各任务的 Period 和 Priority 周期调度执行 taskXX_job()
 * ==============================================================*/

/* Periodic task: task41 (Tsk41)
 * Period=25ms  Priority=2  Stack=8KB
 * No inter- or intra-partition communication (computation-only task)
 */
void* task41_job(void);

/* Periodic task: task42 (Tsk42)
 * Period=50ms  Priority=3  Stack=8KB
 * Reads:  order_in    (queuing DESTINATION, receives from ps1/M1)
 * Writes: queueingout (queuing SOURCE, forwards data)
 */
void* task42_job(void);

/* Periodic task: task43 (Tsk43)
 * Period=50ms  Priority=4  Stack=8KB
 * Reads: queueingin (queuing DESTINATION, receives feedback from ps5/M2)
 */
void* task43_job(void);

/* Periodic task: task44 (Tsk44)
 * Period=100ms  Priority=5  Stack=8KB
 * No inter- or intra-partition communication (computation-only task)
 */
void* task44_job(void);

/* Periodic task: task45 (Tsk45)
 * Period=200ms  Priority=6  Stack=8KB
 * No inter- or intra-partition communication (computation-only task)
 */
void* task45_job(void);

#endif
