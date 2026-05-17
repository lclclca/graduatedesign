/*============================================================
 * deployment.h
 * 部署常量 —— ps2 分区
 * 派生自分区规格 JSON
 *============================================================*/
#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

#include <apex/apex.h>

/*------------------------------------------------------------
 * 分区标识
 *------------------------------------------------------------*/
#define PARTITION_NAME          "ps2"
#define MODULE_NAME             "M1"

/*------------------------------------------------------------
 * 资源数量
 *------------------------------------------------------------*/
#define NB_THREADS              3
#define NB_SAMPLING_PORTS       1
#define NB_QUEUING_PORTS        1
#define NB_BLACKBOARDS          3
#define NB_BUFFERS              3

/*------------------------------------------------------------
 * 栈尺寸（字节，总量 24576，均分给三个任务）
 *------------------------------------------------------------*/
#define TOTAL_STACKS_SIZE       24576U
#define TASK_STACK_SIZE         (TOTAL_STACKS_SIZE / NB_THREADS)   /* 8192 */

/*------------------------------------------------------------
 * 任务周期与优先级（单位 ns，ARINC 653 时间单位）
 *------------------------------------------------------------*/
#define TASK21_PERIOD_NS        ((SYSTEM_TIME_TYPE)50  * 1000000LL)
#define TASK22_PERIOD_NS        ((SYSTEM_TIME_TYPE)50  * 1000000LL)
#define TASK23_PERIOD_NS        ((SYSTEM_TIME_TYPE)100 * 1000000LL)

#define TASK21_PRIORITY         2
#define TASK22_PRIORITY         3
#define TASK23_PRIORITY         4

#define TASK_TIME_CAPACITY_NS   ((SYSTEM_TIME_TYPE)10  * 1000000LL)  /* 10 ms WCET */

/*------------------------------------------------------------
 * 端口名称
 *------------------------------------------------------------*/
#define SAMPLING_IN_PORT_NAME   "pr2samplingin"
#define QUEUING_OUT_PORT_NAME   "pr2queueingout"

/*------------------------------------------------------------
 * 队列端口参数
 *------------------------------------------------------------*/
#define QUEUING_OUT_MAX_NB_MSG  30
#define QUEUING_OUT_MAX_MSG_SZ  ((MESSAGE_SIZE_TYPE)sizeof(QUEUING_OUT_MSG_TYPE))
#define SAMPLING_IN_MAX_MSG_SZ  ((MESSAGE_SIZE_TYPE)sizeof(SAMPLING_IN_MSG_TYPE))
#define SAMPLING_IN_REFRESH_NS  ((SYSTEM_TIME_TYPE)100 * 1000000LL) /* 100 ms */

/*------------------------------------------------------------
 * 黑板 / 缓冲区名称
 *------------------------------------------------------------*/
#define BB_ACC3_NAME            "bb_acc3"
#define BB_T2T3_NAME            "bb_t2t3"
#define BB_T3T2_NAME            "bb_t3t2"

#define BUF_ORDER_NAME          "buf_order"
#define BUF_T2TOT3_NAME         "buf_t2tot3"
#define BUF_T3TOT2_NAME         "buf_t3tot2"

/*------------------------------------------------------------
 * 缓冲区队列深度
 *------------------------------------------------------------*/
#define BUF_ORDER_NB_MSG        10
#define BUF_T2TOT3_NB_MSG       8
#define BUF_T3TOT2_NB_MSG       8

/*------------------------------------------------------------
 * 超时（INFINITE_TIME_VALUE 或具体纳秒）
 *------------------------------------------------------------*/
#define SEND_TIMEOUT_NS         ((SYSTEM_TIME_TYPE)0)   /* 非阻塞 */
#define RECV_TIMEOUT_NS         ((SYSTEM_TIME_TYPE)0)   /* 非阻塞 */

#include "gtypes.h"   /* 引入消息结构体供尺寸计算 */

#endif /* DEPLOYMENT_H */