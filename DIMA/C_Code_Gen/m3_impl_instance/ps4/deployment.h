#ifndef __IMA2C_GENERATED_DEPLOYMENT_H_
#define __IMA2C_GENERATED_DEPLOYMENT_H_

/* ==============================================================
 *  FILE:    deployment.h
 *  ROLE:    编译期部署配置（Compile-time Deployment Configuration）
 *
 *  IMA 架构层次：模块级 → 分区级硬件绑定
 *  IMA 分区：ps4 (P4)     模块：M3 (arinckernel.module3)
 *  对应 AADL 文件：
 *    - DIMA_Processor.aadl  ：处理器定义 + 分区时间窗口调度
 *    - DIMA_models.aadl     ：软件绑定到硬件（actual_processor_binding）
 *    - module.xml           ：操作系统读取的 ARINC 653 模块配置文件
 *
 *  本文件通过 #define 宏告诉 ACoreOS653 运行时本分区需要的资源量：
 *    NB_THREADS      ：分区内任务（线程）数量
 *    NB_SAMPLINGS    ：采样端口数量（对应 AADL out data port，周期刷新）
 *    NB_QUEUEINGS    ：队列端口数量（对应 AADL in/out event data port，FIFO）
 *    NB_BLACKBOARDS  ：黑板数量（分区内 data access，最新值覆盖语义）
 *    NB_BUFFERS      ：缓冲区数量（分区内 data access，FIFO 队列语义）
 *    NEEDS_xxx       ：编译时按需裁剪 ARINC 653 API，未用到的不链接
 *    CONFIG_STACKS_SIZE：所有任务栈空间之和（5 任务 × 8192 字节）
 *
 *  本分区资源概览：
 *    任务数       = 5   (task41~45)
 *    采样端口数   = 0   队列端口数 = 3  (order_in, queueingout, queueingin)
 *    黑板数       = 0   缓冲区数   = 0
 *    总栈空间     = 40960 字节
 * ==============================================================*/

#define IMA2C_RUNTIME_ACoreOS653 1
#define ACoreOS653_GENERATED_CODE 1

/* 任务数量 */
#define ACoreOS653_CONFIG_NB_THREADS 5          /* task41, task42, task43, task44, task45 */

/* 分区间通信端口数量 */
#define ACoreOS653_CONFIG_NB_SAMPLINGS 0        /* 无采样端口 */
#define ACoreOS653_CONFIG_NB_QUEUEINGS 3        /* order_in (DEST), queueingout (SRC), queueingin (DEST) */

/* 分区内通信资源数量 */
#define ACoreOS653_CONFIG_NB_BLACKBOARDS 0      /* 无黑板 */
#define ACoreOS653_CONFIG_NB_BUFFERS 0          /* 无缓冲区 */

/* 按需启用的 ARINC 653 服务（未使用的置 0 可减少链接体积）*/
#define ACoreOS653_NEEDS_ARINC653_PARTITION  1
#define ACoreOS653_NEEDS_ARINC653_PROCESS    1
#define ACoreOS653_NEEDS_ARINC653_QUEUEING   1
#define ACoreOS653_NEEDS_ARINC653_SAMPLING   0  /* ps4 无采样端口 */
#define ACoreOS653_NEEDS_ARINC653_BLACKBOARD 0  /* ps4 无黑板 */
#define ACoreOS653_NEEDS_ARINC653_BUFFER     0  /* ps4 无缓冲区 */
#define ACoreOS653_NEEDS_ARINC653_SEMAPHORE  0  /* ps4 无信号量 */
#define ACoreOS653_NEEDS_ARINC653_EVENT      0  /* ps4 无事件 */
#define ACoreOS653_NEEDS_MIDDLEWARE          1
#define ACoreOS653_NEEDS_ARINC653_TIME       1

/* 总栈空间：5 任务 × 8192 字节 = 40960 字节 */
#define ACoreOS653_CONFIG_STACKS_SIZE 40960

#endif
