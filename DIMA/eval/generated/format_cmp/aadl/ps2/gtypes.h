#ifndef GTYPES_H
#define GTYPES_H

/* ============================================================
 * gtypes.h
 * Partition P2 – Global Type Definitions
 * ARINC 653 / ACoreOS653
 * ============================================================ */

#include "apex/apextypes.h"

/* ------------------------------------------------------------
 * Basic application data type
 * Mapped from AADL Base_Types::Integer
 * ------------------------------------------------------------ */
typedef APEX_INTEGER    APP_INTEGER_TYPE;

/* ------------------------------------------------------------
 * Return-code check macro
 * Halts partition on any APEX error (DAL-A requirement)
 * ------------------------------------------------------------ */
#define CHECK_CODE(msg, code)                                       \
    do {                                                            \
        if ((code) != NO_ERROR) {                                   \
            /* In DAL-A code a real implementation would invoke  */ \
            /* the HM system; here we spin to preserve state.    */ \
            while (1) { /* deliberate infinite loop */ }            \
        }                                                           \
    } while (0)

/* ------------------------------------------------------------
 * Partition-wide sizing constants
 * ------------------------------------------------------------ */
#define APP_DATA_SIZE       sizeof(APP_INTEGER_TYPE)

/* Sampling port (pr2samplingin) */
#define SAMPLING_PORT_IN_NAME       "pr2samplingin"
#define SAMPLING_PORT_IN_PERIOD_NS  ((SYSTEM_TIME_TYPE)100)        /* 100 ns */

/* Queuing port (pr2queueingout) */
#define QUEUING_PORT_OUT_NAME       "pr2queueingout"
#define QUEUING_PORT_OUT_DEPTH      30
#define QUEUING_PORT_OUT_TIMEOUT    ((APEX_INTEGER)5)              /* 5 ns  */

/* Intra-partition communication names */
#define BLACKBOARD_21_22_NAME   "BB_task21_to_task22"
#define BLACKBOARD_22_23_NAME   "BB_task22_to_task23"
#define BLACKBOARD_23_22_NAME   "BB_task23_to_task22"

#define BUFFER_22_21_NAME       "BUF_task22_to_task21"
#define BUFFER_22_23_NAME       "BUF_task22_to_task23"
#define BUFFER_23_22_NAME       "BUF_task23_to_task22"

#define BUFFER_MAX_MSG          10   /* intra-partition buffer depth */

/* Task names */
#define TASK21_NAME     "Tsk21"
#define TASK22_NAME     "Tsk22"
#define TASK23_NAME     "Tsk23"

/* Task periods (nanoseconds) */
#define TASK21_PERIOD   ((SYSTEM_TIME_TYPE)50000000)    /* 50 ms  */
#define TASK22_PERIOD   ((SYSTEM_TIME_TYPE)50000000)    /* 50 ms  */
#define TASK23_PERIOD   ((SYSTEM_TIME_TYPE)100000000)   /* 100 ms */

/* Task stack sizes (bytes) */
#define TASK21_STACK    8192
#define TASK22_STACK    8192
#define TASK23_STACK    8192

/* Task priorities */
#define TASK21_PRIORITY 2
#define TASK22_PRIORITY 3
#define TASK23_PRIORITY 4

#endif /* GTYPES_H */