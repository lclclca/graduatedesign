#include <stdio.h>
#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "deployment.h"
#include "globals.h"
#include "activity.h"

/* ================================================================== */
/* Global IDs for ports, blackboards and buffers                      */
/* ================================================================== */

/* Sampling ports */
SAMPLING_PORT_ID_TYPE   pr2samplingin_id;

/* Queuing ports */
QUEUING_PORT_ID_TYPE    pr2queueingout_id;

/* Blackboards */
BLACKBOARD_ID_TYPE      bb_acc3_id;
BLACKBOARD_ID_TYPE      bb_t2t3_id;
BLACKBOARD_ID_TYPE      bb_t3t2_id;

/* Buffers */
BUFFER_ID_TYPE          buf_order_id;
BUFFER_ID_TYPE          buf_t2tot3_id;
BUFFER_ID_TYPE          buf_t3tot2_id;

/* ================================================================== */
/* Health-Monitor callbacks                                            */
/* ================================================================== */

void module_HM_callback(HM_ERROR_STATUS_TYPE *error_status)
{
    printf("[HM] module_HM_callback: error_id=%d, failed_process_id=%d\n",
           (int)error_status->ERROR_CODE,
           (int)error_status->FAILED_PROCESS_ID);
    /* TODO: add module-level recovery actions */
}

void partition_HM_callback(HM_ERROR_STATUS_TYPE *error_status)
{
    printf("[HM] partition_HM_callback: error_id=%d, failed_process_id=%d\n",
           (int)error_status->ERROR_CODE,
           (int)error_status->FAILED_PROCESS_ID);
    /* TODO: add partition-level recovery actions */
}

/* ================================================================== */
/* appMain – partition entry point                                     */
/* ================================================================== */

void appMain(void)
{
    RETURN_CODE_TYPE        ret;
    PROCESS_ID_TYPE         pid;
    PROCESS_ATTRIBUTE_TYPE  tattr;

    /* -------------------------------------------------------------- */
    /* 1. Create sampling port(s)                                      */
    /* -------------------------------------------------------------- */
    ret = CREATE_SAMPLING_PORT(
            "pr2samplingin",            /* port name (must match XML config)  */
            256,                        /* max message size (bytes) – adjust   */
            DESTINATION,               /* direction                           */
            50 * 1000000LL,            /* refresh period: 50 ms in ns        */
            &pr2samplingin_id);
    CHECK_CODE("CREATE_SAMPLING_PORT(pr2samplingin)", ret);

    /* -------------------------------------------------------------- */
    /* 2. Create queuing port(s)                                       */
    /* -------------------------------------------------------------- */
    ret = CREATE_QUEUING_PORT(
            "pr2queueingout",           /* port name                          */
            256,                        /* max message size (bytes) – adjust  */
            30,                         /* max nb messages                    */
            SOURCE,                     /* direction                          */
            FIFO,                       /* queuing discipline                 */
            &pr2queueingout_id);
    CHECK_CODE("CREATE_QUEUING_PORT(pr2queueingout)", ret);

    /* -------------------------------------------------------------- */
    /* 3. Create blackboards                                           */
    /* -------------------------------------------------------------- */
    ret = CREATE_BLACKBOARD("bb_acc3", 256, &bb_acc3_id);
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc3)", ret);

    ret = CREATE_BLACKBOARD("bb_t2t3", 256, &bb_t2t3_id);
    CHECK_CODE("CREATE_BLACKBOARD(bb_t2t3)", ret);

    ret = CREATE_BLACKBOARD("bb_t3t2", 256, &bb_t3t2_id);
    CHECK_CODE("CREATE_BLACKBOARD(bb_t3t2)", ret);

    /* -------------------------------------------------------------- */
    /* 4. Create buffers                                               */
    /* -------------------------------------------------------------- */
    ret = CREATE_BUFFER("buf_order",   256, 16, FIFO, &buf_order_id);
    CHECK_CODE("CREATE_BUFFER(buf_order)", ret);

    ret = CREATE_BUFFER("buf_t2tot3", 256, 16, FIFO, &buf_t2tot3_id);
    CHECK_CODE("CREATE_BUFFER(buf_t2tot3)", ret);

    ret = CREATE_BUFFER("buf_t3tot2", 256, 16, FIFO, &buf_t3tot2_id);
    CHECK_CODE("CREATE_BUFFER(buf_t3tot2)", ret);

    /* -------------------------------------------------------------- */
    /* 5. Create processes (tasks)                                     */
    /* -------------------------------------------------------------- */

    /* --- task21 : period 50 ms, priority 2 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task21");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task21_job;
    tattr.STACK_SIZE    = ACoreOS653_NEEDS_STACKS_SIZE /
                          ACoreOS653_CONFIG_NB_THREADS;   /* divide evenly */
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 50 * 1000000LL;                /* 50 ms in ns   */
    tattr.TIME_CAPACITY = INFINITE_TIME_VALUE;
    tattr.DEADLINE      = SOFT;

    ret = CREATE_PROCESS(&tattr, &pid);
    CHECK_CODE("CREATE_PROCESS(task21)", ret);

    ret = START(pid, &ret);
    CHECK_CODE("START(task21)", ret);

    /* --- task22 : period 50 ms, priority 3 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task22");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task22_job;
    tattr.STACK_SIZE    = ACoreOS653_NEEDS_STACKS_SIZE /
                          ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50 * 1000000LL;
    tattr.TIME_CAPACITY = INFINITE_TIME_VALUE;
    tattr.DEADLINE      = SOFT;

    ret = CREATE_PROCESS(&tattr, &pid);
    CHECK_CODE("CREATE_PROCESS(task22)", ret);

    ret = START(pid, &ret);
    CHECK_CODE("START(task22)", ret);

    /* --- task23 : period 100 ms, priority 4 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task23");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task23_job;
    tattr.STACK_SIZE    = ACoreOS653_NEEDS_STACKS_SIZE /
                          ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY = 4;
    tattr.PERIOD        = 100 * 1000000LL;               /* 100 ms in ns  */
    tattr.TIME_CAPACITY = INFINITE_TIME_VALUE;
    tattr.DEADLINE      = SOFT;

    ret = CREATE_PROCESS(&tattr, &pid);
    CHECK_CODE("CREATE_PROCESS(task23)", ret);

    ret = START(pid, &ret);
    CHECK_CODE("START(task23)", ret);

    /* -------------------------------------------------------------- */
    /* 6. Switch partition to NORMAL mode                              */
    /* -------------------------------------------------------------- */
    ret = SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);

    /* appMain must not return in ARINC 653 */
    while (1) { ; }
}