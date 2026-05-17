#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

/* Number of threads/processes in this partition */
#define ACoreOS653_CONFIG_NB_THREADS        3

/* Sampling ports: pr2samplingin (DESTINATION) */
#define ACoreOS653_CONFIG_NB_SAMPLINGS      1

/* Queuing ports: pr2queueingout (SOURCE) */
#define ACoreOS653_CONFIG_NB_QUEUEINGS      1

/* Blackboards: task1_acc3 (task21->task22), task2_blackboard (task22<->task23) */
#define ACoreOS653_CONFIG_NB_BLACKBOARDS    2

/* Buffers: task2to1 (task22->task21), task2to3 (task22<->task23) */
#define ACoreOS653_CONFIG_NB_BUFFERS        2

/* Stack size per thread: 8 KB = 8192 bytes */
#define ACoreOS653_NEEDS_STACKS_SIZE        8192

#endif /* DEPLOYMENT_H */