#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

#include <os/pos/apex/apexLib.h>

/* Number of threads (tasks) */
#define NB_THREADS      5

/* Number of queuing ports */
#define NB_QUEUEINGS    3

/* Number of sampling ports */
#define NB_SAMPLINGS    0

/* Number of blackboards */
#define NB_BLACKBOARDS  0

/* Number of buffers */
#define NB_BUFFERS      0

/* Stack size for all tasks combined */
#define STACKS_SIZE     40960

/* Thread index definitions */
#define TASK41_THREAD_IDX  0
#define TASK42_THREAD_IDX  1
#define TASK43_THREAD_IDX  2
#define TASK44_THREAD_IDX  3
#define TASK45_THREAD_IDX  4

/* Queuing port index definitions */
#define ORDER_IN_PORT_IDX     0
#define QUEUEINGOUT_PORT_IDX  1
#define QUEUEINGIN_PORT_IDX   2

#endif /* DEPLOYMENT_H */