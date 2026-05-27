#include <stdio.h>
#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "deployment.h"
#include "globals.h"
#include "gtypes.h"
#include "activity.h"

/* ------------------------------------------------------------------ */
/* Global IDs for ports, blackboards and buffers                       */
/* (extern-referenced in activity.c)                                   */
/* ------------------------------------------------------------------ */

/* Sampling port */
SAMPLING_PORT_ID_TYPE   sampling_in_id;        /* pr2samplingin  (DESTINATION) */

/* Queuing port */
QUEUING_PORT_ID_TYPE    queuing_out_id;         /* pr2queueingout (SOURCE)      */

/* Blackboards */
BLACKBOARD_ID_TYPE      bb_task1_acc3_id;       /* task21 -> task22             */
BLACKBOARD_ID_TYPE      bb_task2_task3_id;      /* task22 <-> task23            */

/* Buffers */
BUFFER_ID_TYPE          buf_task2_order_id;     /* task22 -> task21 (order)     */
BUFFER_ID_TYPE          buf_task2_task3_id;     /* task22 <-> task23 (data)     */

/* ------------------------------------------------------------------ */
/* appMain — partition entry point                                      */
/* ------------------------------------------------------------------ */
void appMain(void)
{
    RETURN_CODE_TYPE    ret;
    PROCESS_ID_TYPE     proc_id;
    PROCESS_ATTRIBUTE_TYPE tattr;

    printf("[P2] appMain started\n");

    /* ==============================================================
     * 1. CREATE SAMPLING PORT (DESTINATION)
     *    pr2samplingin — refresh period 100 ns
     * ============================================================== */
    ret = CREATE_SAMPLING_PORT(
            "pr2samplingin",            /* port name                  */
            sizeof(integer),            /* max message size (bytes)   */
            DESTINATION,                /* direction                  */
            100,                        /* refresh period (ns)        */
            &sampling_in_id);
    CHECK_CODE("CREATE_SAMPLING_PORT pr2samplingin", ret);

    /* ==============================================================
     * 2. CREATE QUEUING PORT (SOURCE)
     *    pr2queueingout — queue size 30, timeout 5 ns, FIFO
     * ============================================================== */
    ret = CREATE_QUEUING_PORT(
            "pr2queueingout",           /* port name                  */
            sizeof(integer),            /* max message size (bytes)   */
            30,                         /* max nb messages            */
            SOURCE,                     /* direction                  */
            FIFO,                       /* queuing discipline         */
            &queuing_out_id);
    CHECK_CODE("CREATE_QUEUING_PORT pr2queueingout", ret);

    /* ==============================================================
     * 3. CREATE BLACKBOARDS
     * ============================================================== */

    /* task1_acc3: task21 writes, task22 reads */
    ret = CREATE_BLACKBOARD(
            "bb_task1_acc3",
            sizeof(integer),
            &bb_task1_acc3_id);
    CHECK_CODE("CREATE_BLACKBOARD bb_task1_acc3", ret);

    /* task2_task3: task22 <-> task23 (bidirectional; one shared BB) */
    ret = CREATE_BLACKBOARD(
            "bb_task2_task3",
            sizeof(integer),
            &bb_task2_task3_id);
    CHECK_CODE("CREATE_BLACKBOARD bb_task2_task3", ret);

    /* ==============================================================
     * 4. CREATE BUFFERS
     * ============================================================== */

    /* buf_task2_order: task22 -> task21 (order feedback) */
    ret = CREATE_BUFFER(
            "buf_task2_order",
            sizeof(integer),            /* max message size           */
            10,                         /* max nb messages            */
            FIFO,                       /* queuing discipline         */
            &buf_task2_order_id);
    CHECK_CODE("CREATE_BUFFER buf_task2_order", ret);

    /* buf_task2_task3: task22 <-> task23 (data exchange) */
    ret = CREATE_BUFFER(
            "buf_task2_task3",
            sizeof(integer),
            10,
            FIFO,
            &buf_task2_task3_id);
    CHECK_CODE("CREATE_BUFFER buf_task2_task3", ret);

    /* ==============================================================
     * 5. CREATE PROCESSES (threads)
     * ============================================================== */

    /* --- task21: Period 50 ms = 50,000,000 ns, Priority 2 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task21");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task21_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE;
    tattr.BASE_PRIORITY     = 2;
    tattr.PERIOD            = 50000000;     /* 50 ms in ns */
    tattr.TIME_CAPACITY     = INFINITE_TIME_VALUE;
    tattr.DEADLINE          = SOFT;

    ret = CREATE_PROCESS(&tattr, &proc_id);
    CHECK_CODE("CREATE_PROCESS task21", ret);

    ret = START(proc_id, &ret);
    CHECK_CODE("START task21", ret);

    /* --- task22: Period 50 ms = 50,000,000 ns, Priority 3 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task22");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task22_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE;
    tattr.BASE_PRIORITY     = 3;
    tattr.PERIOD            = 50000000;     /* 50 ms in ns */
    tattr.TIME_CAPACITY     = INFINITE_TIME_VALUE;
    tattr.DEADLINE          = SOFT;

    ret = CREATE_PROCESS(&tattr, &proc_id);
    CHECK_CODE("CREATE_PROCESS task22", ret);

    ret = START(proc_id, &ret);
    CHECK_CODE("START task22", ret);

    /* --- task23: Period 100 ms = 100,000,000 ns, Priority 4 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task23");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task23_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE;
    tattr.BASE_PRIORITY     = 4;
    tattr.PERIOD            = 100000000;    /* 100 ms in ns */
    tattr.TIME_CAPACITY     = INFINITE_TIME_VALUE;
    tattr.DEADLINE          = SOFT;

    ret = CREATE_PROCESS(&tattr, &proc_id);
    CHECK_CODE("CREATE_PROCESS task23", ret);

    ret = START(proc_id, &ret);
    CHECK_CODE("START task23", ret);

    /* ==============================================================
     * 6. SET PARTITION MODE TO NORMAL
     * ============================================================== */
    ret = SET_PARTITION_MODE(NORMAL);
    CHECK_CODE("SET_PARTITION_MODE NORMAL", ret);

    /* Should not be reached after SET_PARTITION_MODE(NORMAL) */
    printf("[P2] appMain: unexpected return from SET_PARTITION_MODE\n");
}

/* ------------------------------------------------------------------ */
/* Health Monitor Callbacks                                            */
/* ------------------------------------------------------------------ */

void module_HM_callback(ERROR_STATUS_TYPE *error)
{
    printf("[P2][MODULE_HM] error_code=%d, failed_process=%d\n",
           (int)error->ERROR_CODE,
           (int)error->FAILED_PROCESS_ID);
    /* TODO: implement module-level health monitoring recovery logic */
}

void partition_HM_callback(ERROR_STATUS_TYPE *error)
{
    printf("[P2][PARTITION_HM] error_code=%d, failed_process=%d\n",
           (int)error->ERROR_CODE,
           (int)error->FAILED_PROCESS_ID);
    /* TODO: implement partition-level health monitoring recovery logic */
}