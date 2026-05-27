#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

#include <os/pos/apex/apexLib.h>

/* Number of threads (tasks) */
#define NB_THREADS      3

/* Number of queuing ports */
#define NB_QUEUEINGS    2

/* Number of sampling ports */
#define NB_SAMPLINGS    0

/* Number of blackboards */
#define NB_BLACKBOARDS  0

/* Number of buffers */
#define NB_BUFFERS      0

/* Stack size per task (total / nb_threads) */
#define STACKS_SIZE     24576

/* Queuing port indices */
#define ORDER_IN_IDX        0
#define QUEUEINGOUT_IDX     1

/* Thread indices */
#define TASK51_IDX  0
#define TASK52_IDX  1
#define TASK53_IDX  2

#endif /* DEPLOYMENT_H */