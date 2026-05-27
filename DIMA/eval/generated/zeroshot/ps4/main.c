#include <string.h>
#include <stdio.h>

#include <os/pos/apex/apexLib.h>

#include "deployment.h"
#include "globals.h"
#include "gtypes.h"
#include "activity.h"

/* ------------------------------------------------------------------ */
/* Global queuing port IDs (extern-referenced from activity.c)         */
/* ------------------------------------------------------------------ */
QUEUING_PORT_ID_TYPE order_in_id;
QUEUING_PORT_ID_TYPE queueingout_id;
QUEUING_PORT_ID_TYPE queueingin_id;

/* ------------------------------------------------------------------ */
/* Health-Monitor callbacks (mandatory APEX hooks)                     */
/* ------------------------------------------------------------------ */

void module_HM_callback(ERROR_STATUS_TYPE *error)
{
    printf("[PS4] module_HM_callback: error_code=%d, failed_id=%lld\n",
           (int)error->ERROR_CODE,
           (long long)error->FAILED_PROCESS_ID);
    /* TODO: insert module-level HM recovery actions */
}

void partition_HM_callback(ERROR_STATUS_TYPE *error)
{
    printf("[PS4] partition_HM_callback: error_code=%d, failed_id=%lld\n",
           (int)error->ERROR_CODE,
           (long long)error->FAILED_PROCESS_ID);
    /* TODO: insert partition-level HM recovery actions */
}

/* ------------------------------------------------------------------ */
/* appMain – partition entry point                                      */
/* ------------------------------------------------------------------ */
void appMain(void)
{
    RETURN_CODE_TYPE      ret;
    PROCESS_ID_TYPE       pid;
    PROCESS_ATTRIBUTE_TYPE tattr;

    /* ============================================================== */
    /* 1. Create queuing ports                                         */
    /* ============================================================== */

    /*
     * order_in  – DESTINATION, max messages = 1
     * Message size: define a reasonable upper bound (256 bytes).
     * Discipline: FIFO.
     */
    CREATE_QUEUING_PORT(
        "order_in",                 /* port name (must match XML config) */
        256,                        /* max message size (bytes)           */
        1,                          /* max nb messages                    */
        DESTINATION,                /* direction                          */
        FIFO,                       /* queuing discipline                 */
        &order_in_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT order_in", ret);

    /*
     * queueingout – SOURCE, max messages = 4
     */
    CREATE_QUEUING_PORT(
        "queueingout",
        256,
        4,
        SOURCE,
        FIFO,
        &queueingout_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT queueingout", ret);

    /*
     * queueingin  – DESTINATION, max messages = 4
     */
    CREATE_QUEUING_PORT(
        "queueingin",
        256,
        4,
        DESTINATION,
        FIFO,
        &queueingin_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT queueingin", ret);

    /* ============================================================== */
    /* 2. No sampling ports, blackboards, or buffers for ps4           */
    /* ============================================================== */

    /* ============================================================== */
    /* 3. Create processes (tasks)                                     */
    /* Period unit: nanoseconds  (ms * 1 000 000)                     */
    /* Stack size: shared pool ACoreOS653_NEEDS_STACKS_SIZE / NB       */
    /* ============================================================== */

    /* --- task41: period=25 ms, priority=2 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task41");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task41_job;
    tattr.PERIOD        = 25 * 1000000LL;   /* 25 ms in ns  */
    tattr.TIME_CAPACITY = 25 * 1000000LL;
    tattr.STACK_SIZE    = ACoreOS653_NEEDS_STACKS_SIZE /
                          ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY = 2;
    tattr.DEADLINE      = SOFT;

    CREATE_PROCESS(&tattr, &pid, &ret);
    CHECK_CODE("CREATE_PROCESS task41", ret);

    START(pid, &ret);
    CHECK_CODE("START task41", ret);

    /* --- task42: period=50 ms, priority=3 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task42");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task42_job;
    tattr.PERIOD        = 50 * 1000000LL;
    tattr.TIME_CAPACITY = 50 * 1000000LL;
    tattr.STACK_SIZE    = ACoreOS653_NEEDS_STACKS_SIZE /
                          ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY = 3;
    tattr.DEADLINE      = SOFT;

    CREATE_PROCESS(&tattr, &pid, &ret);
    CHECK_CODE("CREATE_PROCESS task42", ret);

    START(pid, &ret);
    CHECK_CODE("START task42", ret);

    /* --- task43: period=50 ms, priority=4 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task43");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task43_job;
    tattr.PERIOD        = 50 * 1000000LL;
    tattr.TIME_CAPACITY = 50 * 1000000LL;
    tattr.STACK_SIZE    = ACoreOS653_NEEDS_STACKS_SIZE /
                          ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY = 4;
    tattr.DEADLINE      = SOFT;

    CREATE_PROCESS(&tattr, &pid, &ret);
    CHECK_CODE("CREATE_PROCESS task43", ret);

    START(pid, &ret);
    CHECK_CODE("START task43", ret);

    /* --- task44: period=100 ms, priority=5 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task44");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task44_job;
    tattr.PERIOD        = 100 * 1000000LL;
    tattr.TIME_CAPACITY = 100 * 1000000LL;
    tattr.STACK_SIZE    = ACoreOS653_NEEDS_STACKS_SIZE /
                          ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY = 5;
    tattr.DEADLINE      = SOFT;

    CREATE_PROCESS(&tattr, &pid, &ret);
    CHECK_CODE("CREATE_PROCESS task44", ret);

    START(pid, &ret);
    CHECK_CODE("START task44", ret);

    /* --- task45: period=200 ms, priority=6 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task45");
    tattr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task45_job;
    tattr.PERIOD        = 200 * 1000000LL;
    tattr.TIME_CAPACITY = 200 * 1000000LL;
    tattr.STACK_SIZE    = ACoreOS653_NEEDS_STACKS_SIZE /
                          ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY = 6;
    tattr.DEADLINE      = SOFT;

    CREATE_PROCESS(&tattr, &pid, &ret);
    CHECK_CODE("CREATE_PROCESS task45", ret);

    START(pid, &ret);
    CHECK_CODE("START task45", ret);

    /* ============================================================== */
    /* 4. Transition partition to NORMAL operating mode                */
    /* ============================================================== */
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE NORMAL", ret);

    /*
     * Execution does not return past SET_PARTITION_MODE(NORMAL).
     * The APEX kernel takes over scheduling from this point.
     */
}