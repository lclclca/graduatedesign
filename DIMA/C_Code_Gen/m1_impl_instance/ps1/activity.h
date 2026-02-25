#ifndef __IMA2C_GENERATED_ACTIVITY_H_
#define __IMA2C_GENERATED_ACTIVITY_H_

/* ==============================================================
 *  FILE:    activity.h
 *  ROLE:    任务入口函数声明（Task Entry Point Declarations）
 *
 *  IMA 架构层次：线程层（Thread Layer）
 *  IMA 分区：ps1 (P1)     模块：M1 (arinckernel.module1)
 *  对应 AADL 文件：DIMA_threads.aadl
 *    每个 thread 构造对应一个周期性任务，由 ARINC 653 调度执行。
 *
 *  本文件声明 activity.c 中实现的所有任务入口函数。
 *  这些函数在 main.c 的 appMain() 中通过如下方式注册到 OS：
 *    PROCESS_ATTRIBUTE_TYPE.ENTRY_POINT = taskXX_job;
 *    CREATE_PROCESS(&tattr, &thread_id, &ret);
 *    START(thread_id, &ret);
 *
 *  运行时调用关系：
 *    OS → appMain() → CREATE_PROCESS + START
 *    → [SET_PARTITION_MODE(NORMAL)]
 *    → RTOS 按各任务的 Period 和 Priority 周期调度执行 taskXX_job()
 *
 *  AADL thread 属性 → C 任务参数映射：
 *    Dispatch_Protocol    => Periodic → PERIODIC_WAIT() 在循环末尾调用
 *    Period               => Xms      → tattr.PERIOD = X * 1000000ll (ns)
 *    Priority             => N        → tattr.BASE_PRIORITY = N
 *    Stack_Size           => 8192 B   → tattr.STACK_SIZE = 8192
 *    Deadline             => Soft     → tattr.DEADLINE = SOFT
 * ==============================================================*/

/* Periodic task: task11 (Tsk11)
 * Period=25ms  Priority=2  Stack=8KB
 * Writes: pr1samplingout (sampling SOURCE), bb_acc1, bb_acc2
 * Reads:  buf_sensor (from task12)
 */
void* task11_job(void);

/* Periodic task: task12 (Tsk12)
 * Period=50ms  Priority=3  Stack=8KB
 * Reads:  bb_acc1, bb_acc2 (from task11)
 * Writes: buf_sensor (to task11), bb_sem (to task13, protected access)
 */
void* task12_job(void);

/* Periodic task: task13 (Tsk13)
 * Period=50ms  Priority=4  Stack=8KB
 * Reads:  bb_sem (from task12, protected), buf_t4tot3, bb_t4t3 (from task14)
 * Writes: buf_t3tot4, bb_t3t4 (to task14)
 */
void* task13_job(void);

/* Periodic task: task14 (Tsk14)
 * Period=50ms  Priority=5  Stack=8KB
 * Reads:  pr1queueingin (queuing DESTINATION), buf_t3tot4, bb_t3t4 (from task13)
 * Writes: buf_t4tot3, bb_t4t3 (to task13)
 */
void* task14_job(void);

#endif
