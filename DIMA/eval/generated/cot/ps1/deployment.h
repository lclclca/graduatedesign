#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

/* ── 部署常量（来自规格 deployment 字段） ── */
#define NB_THREADS      4
#define NB_SAMPLINGS    1
#define NB_QUEUEINGS    1
#define NB_BLACKBOARDS  5
#define NB_BUFFERS      3
#define STACKS_SIZE     32768

/* 统一任务栈大小（STACKS_SIZE / NB_THREADS） */
#define TASK_STACK_SIZE 8192

#endif /* DEPLOYMENT_H */