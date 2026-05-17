#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

/* ---------------------------------------------------------------
 * Partition-level deployment constants for ps2 (module M1)
 * --------------------------------------------------------------- */

#define PARTITION_NAME          "ps2"

/* Task / thread configuration */
#define NB_THREADS              3

/* Port configuration */
#define NB_SAMPLING_PORTS       1
#define NB_QUEUING_PORTS        1

/* IPC resource configuration */
#define NB_BLACKBOARDS          3
#define NB_BUFFERS              3

/* Stack size (bytes) assigned to each task */
#define STACKS_SIZE             24576

/* ---------------------------------------------------------------
 * Task periods (nanoseconds, ARINC 653 time base)
 * --------------------------------------------------------------- */
#define TASK21_PERIOD           (50  * 1000000LL)   /* 50  ms */
#define TASK22_PERIOD           (50  * 1000000LL)   /* 50  ms */
#define TASK23_PERIOD           (100 * 1000000LL)   /* 100 ms */

/* Task priorities (higher value = higher urgency in ACoreOS653) */
#define TASK21_PRIORITY         2
#define TASK22_PRIORITY         3
#define TASK23_PRIORITY         4

/* ---------------------------------------------------------------
 * Port names (must match system integrator XML configuration)
 * --------------------------------------------------------------- */
#define PR2SAMPLINGIN_NAME      "pr2samplingin"
#define PR2QUEUEINGOUT_NAME     "pr2queueingout"

/* Queuing port parameters */
#define PR2QUEUEINGOUT_DEPTH    30
#define PR2QUEUEINGOUT_MSG_SIZE ((APEX_INTEGER)sizeof(int))
#define PR2SAMPLINGIN_MSG_SIZE  ((APEX_INTEGER)sizeof(int))

/* Sampling port refresh period (must be >= sender's period) */
#define PR2SAMPLINGIN_REFRESH   (100 * 1000000LL)   /* 100 ms */

/* ---------------------------------------------------------------
 * IPC resource names
 * --------------------------------------------------------------- */
/* Blackboards */
#define BB_ACC3_NAME            "bb_acc3"
#define BB_T2T3_NAME            "bb_t2t3"
#define BB_T3T2_NAME            "bb_t3t2"
#define BB_MSG_SIZE             ((APEX_INTEGER)sizeof(int))

/* Buffers */
#define BUF_ORDER_NAME          "buf_order"
#define BUF_T2TOT3_NAME         "buf_t2tot3"
#define BUF_T3TOT2_NAME         "buf_t3tot2"
#define BUF_MSG_SIZE            ((APEX_INTEGER)sizeof(int))
#define BUF_DEPTH               10

#endif /* DEPLOYMENT_H */