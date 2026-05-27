#ifndef __IMA2C_GENERATED_ACTIVITY_H_
#define __IMA2C_GENERATED_ACTIVITY_H_

/* ==============================================================
 *  FILE:    activity.h
 *  ROLE:    任务入口函数声明（Task Entry Point Declarations）
 *
 *  IMA 架构层次：线程层（Thread Layer）
 *  IMA 分区：ps2 (P2)     模块：M1 (arinckernel.module1)
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
 *    Dispatch_Protocol => Periodic → PERIODIC_WAIT() 在循环末尾调用
 *    Period            => Xms      → tattr.PERIOD = X * 1000000ll (ns)
 *    Priority          => N        → tattr.BASE_PRIORITY = N
 * ==============================================================*/

/* Periodic task: task21 (Tsk21)
 * Period=50ms  Priority=2  Stack=8KB
 * Reads:  pr2samplingin (sampling DESTINATION), buf_order (from task22)
 * Writes: bb_acc3 (to task22)
 */
void* task21_job(void);

/* Periodic task: task22 (Tsk22)
 * Period=50ms  Priority=3  Stack=8KB
 * Reads:  bb_acc3 (from task21), buf_t3tot2, bb_t3t2 (from task23)
 * Writes: buf_order (to task21), buf_t2tot3, bb_t2t3 (to task23)
 */
void* task22_job(void);

/* Periodic task: task23 (Tsk23)
 * Period=100ms  Priority=4  Stack=8KB
 * Reads:  buf_t2tot3, bb_t2t3 (from task22)
 * Writes: buf_t3tot2, bb_t3t2 (to task22), pr2queueingout (queuing SOURCE)
 */
void* task23_job(void);

#endif
