#include <stdio.h>
#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "deployment.h"
#include "globals.h"
#include "gtypes.h"
#include "activity.h"

/* ------------------------------------------------------------------ */
/* Global port IDs (extern-referenced in activity.c)                  */
/* ------------------------------------------------------------------ */
QUEUING_PORT_ID_TYPE order_in_id   = 0;
QUEUING_PORT_ID_TYPE queueingin_id = 0;

/* ================================================================== */
/* Health-Monitor callbacks                                           */
/* ================================================================== */

void module_HM_callback(HM_ERROR_STATUS_TYPE *hm_status)
{
    printf("[PS3][module_HM_callback] error_id=%d\n",
           (int)hm_status->ERROR_CODE);
    /* TODO: Implement module-level health monitoring response */
}

void partition_HM_callback(HM_ERROR_STATUS_TYPE *hm_status)
{
    printf("[PS3][partition_HM_callback] error_id=%d\n",
           (int)hm_status->ERROR_CODE);
    /* TODO: Implement partition-level health monitoring response */
}

/* ================================================================== */
/* appMain — partition entry point                                    */
/* ================================================================== */
void appMain(void)
{
    RETURN_CODE_TYPE     ret;
    PROCESS_ID_TYPE      proc_id;
    PROCESS_ATTRIBUTE_TYPE tattr;

    printf("[PS3] appMain: partition ps3 starting (module M2)\n");

    /* ============================================================== */
    /* 1. Create Queuing Ports                                        */
    /* ============================================================== */

    /* order_in — DESTINATION, max messages: 4 */
    CREATE_QUEUING_PORT(
        "order_in",                        /* port name (must match XML config) */
        (MESSAGE_SIZE_TYPE)256,            /* max message size in bytes          */
        (MESSAGE_RANGE_TYPE)4,             /* max number of messages             */
        DESTINATION,                       /* port direction                     */
        FIFO,                              /* queuing discipline                 */
        &order_in_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(order_in)", ret);

    /* queueingin — DESTINATION, max messages: 4 */
    CREATE_QUEUING_PORT(
        "queueingin",
        (MESSAGE_SIZE_TYPE)256,
        (MESSAGE_RANGE_TYPE)4,
        DESTINATION,
        FIFO,
        &queueingin_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(queueingin)", ret);

    /* ============================================================== */
    /* 2. No Sampling Ports, Blackboards, or Buffers for this        */
    /*    partition (NB_SAMPLINGS=0, NB_BLACKBOARDS=0, NB_BUFFERS=0) */
    /* ============================================================== */

    /* ============================================================== */
    /* 3. Create Processes (Tasks)                                    */
    /* ============================================================== */

    /* --- task31: period=25ms, priority=2 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task31");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task31_job;
    tattr.STACK_SIZE    = (STACK_SIZE_TYPE)(ACoreOS653_NEEDS_STACKS_SIZE /
                                            ACoreOS653_CONFIG_NB_THREADS);
    tattr.BASE_PRIORITY = (PRIORITY_TYPE)2;
    tattr.PERIOD        = (SYSTEM_TIME_TYPE)(25 * 1000000LL);   /* 25 ms → ns */
    tattr.TIME_CAPACITY = (SYSTEM_TIME_TYPE)INFINITE_TIME_VALUE;
    tattr.DEADLINE      = SOFT;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task31)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task31)", ret);

    /* --- task32: period=50ms, priority=3 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task32");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task32_job;
    tattr.STACK_SIZE    = (STACK_SIZE_TYPE)(ACoreOS653_NEEDS_STACKS_SIZE /
                                            ACoreOS653_CONFIG_NB_THREADS);
    tattr.BASE_PRIORITY = (PRIORITY_TYPE)3;
    tattr.PERIOD        = (SYSTEM_TIME_TYPE)(50 * 1000000LL);   /* 50 ms → ns */
    tattr.TIME_CAPACITY = (SYSTEM_TIME_TYPE)INFINITE_TIME_VALUE;
    tattr.DEADLINE      = SOFT;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task32)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task32)", ret);

    /* --- task33: period=50ms, priority=4 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task33");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task33_job;
    tattr.STACK_SIZE    = (STACK_SIZE_TYPE)(ACoreOS653_NEEDS_STACKS_SIZE /
                                            ACoreOS653_CONFIG_NB_THREADS);
    tattr.BASE_PRIORITY = (PRIORITY_TYPE)4;
    tattr.PERIOD        = (SYSTEM_TIME_TYPE)(50 * 1000000LL);   /* 50 ms → ns */
    tattr.TIME_CAPACITY = (SYSTEM_TIME_TYPE)INFINITE_TIME_VALUE;
    tattr.DEADLINE      = SOFT;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task33)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task33)", ret);

    /* ============================================================== */
    /* 4. Transition partition to NORMAL operating mode              */
    /* ============================================================== */
    printf("[PS3] appMain: setting partition mode to NORMAL\n");
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);

    /*
     * Execution should not reach here after SET_PARTITION_MODE(NORMAL).
     * If it does, it indicates a fatal configuration error.
     */
    printf("[PS3] appMain: FATAL — returned from SET_PARTITION_MODE\n");
}