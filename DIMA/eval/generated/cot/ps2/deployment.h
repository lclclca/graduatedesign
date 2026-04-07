#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

/* 部署常量：ps2 分区，模块 M1 */

#define NB_THREADS      3
#define NB_SAMPLINGS    1
#define NB_QUEUEINGS    1
#define NB_BLACKBOARDS  3
#define NB_BUFFERS      3
#define STACKS_SIZE     24576

/* 任务栈大小：总栈平均分配给3个任务 */
#define TASK_STACK_SIZE (STACKS_SIZE / NB_THREADS)   /* = 8192 */

/* 刷新周期（纳秒）：采样端口使用最小任务周期 50ms */
#define PR2SAMPLINGIN_REFRESH_PERIOD   50000000ll

/* 队列端口超时（0 = 非阻塞） */
#define QUEUING_TIMEOUT   0ll

/* 缓冲区/黑板超时 */
#define BB_TIMEOUT        0ll
#define BUF_TIMEOUT       0ll

#endif /* DEPLOYMENT_H */