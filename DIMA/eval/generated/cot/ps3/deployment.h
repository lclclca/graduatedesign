#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

#include <os/pos/apex/apexLib.h>

/* Number of threads */
#define NB_THREADS      3

/* Number of queuing ports */
#define NB_QUEUEINGS    2

/* Stack size per thread */
#define STACK_SIZE      8192

/* Thread index definitions */
#define TASK31_THREAD_IDX  0
#define TASK32_THREAD_IDX  1
#define TASK33_THREAD_IDX  2

/* Queuing port index definitions */
#define ORDER_IN_PORT_IDX      0
#define QUEUEINGIN_PORT_IDX    1

extern PROCESS_ID_TYPE arinc_threads[NB_THREADS];
extern QUEUING_PORT_ID_TYPE ps3_queuing_port_ids[NB_QUEUEINGS];

#endif /* DEPLOYMENT_H */