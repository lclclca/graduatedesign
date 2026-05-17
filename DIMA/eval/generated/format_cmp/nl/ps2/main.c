#include <stdio.h>
#include <string.h>

#include <os/pos/apex/apexLib.h>

#include "deployment.h"
#include "globals.h"
#include "activity.h"

/* =======================================================
 * Global IPC port IDs
 * ======================================================= */

/* Sampling ports */
SAMPLING_PORT_ID_TYPE  pr2samplingin_id;

/* Queuing ports */
QUEUING_PORT_ID_TYPE   pr2queueingout_id;

/* =======================================================
 * Global shared-resource IDs
 * ======================================================= */

/* Blackboards */
BLACKBOARD_ID_TYPE     bb_acc3_id;
BLACKBOARD_ID_TYPE     bb_t2t3_id;
BLACKBOARD_ID_TYPE     bb_t3t2_id;

/* Buffers */
BUFFER_ID_TYPE         buf_order_id;
BUFFER_ID_TYPE         buf_t2tot3_id;
BUFFER_ID_TYPE         buf_t3tot2_id;

/* =======================================================
 * Health-Monitor callbacks (mandatory stubs)
 * ======================================================= */

void module_HM_callback(ERROR_STATUS_TYPE *error)
{
    /* TODO: handle module-level health-monitor errors */
    printf("[HM] module_HM_callback: error_code=%d\n",
           (int)error->ERROR_CODE);
}

void partition_HM_callback(ERROR_STATUS_TYPE *error)
{
    /* TODO: handle partition-level health-monitor errors */
    printf("[HM] partition_HM_callback: error_code=%d\n",
           (int)error->ERROR_CODE);
}

/* =======================================================
 * appMain — partition initialisation entry point
 * ======================================================= */
void appMain(void)
{
    RETURN_CODE_TYPE      ret;
    PROCESS_ID_TYPE       proc_id;
    PROCESS_ATTRIBUTE_TYPE tattr;

    printf("=== ps2 partition initialisation (M1) ===\n");

    /* ---------------------------------------------------
     * 1. Create sampling input port: pr2samplingin
     *    Direction : DESTINATION (READ_SAMPLING_MESSAGE)
     * --------------------------------------------------- */
    CREATE_SAMPLING_PORT(
        "pr2samplingin",          /* port name (must match XML config)  */
        256,                      /* max message size (bytes)           */
        DESTINATION,              /* direction                          */
        50000000LL,               /* refresh period (ns) — 50 ms       */
        &pr2samplingin_id,
        &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT(pr2samplingin)", ret);

    /* ---------------------------------------------------
     * 2. Create queuing output port: pr2queueingout
     *    Direction : SOURCE (SEND_QUEUING_MESSAGE)
     *    Queue depth: 30
     * --------------------------------------------------- */
    CREATE_QUEUING_PORT(
        "pr2queueingout",         /* port name                         */
        256,                      /* max message size (bytes)          */
        30,                       /* queue depth                       */
        SOURCE,                   /* direction                         */
        FIFO,                     /* queuing discipline                */
        &pr2queueingout_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(pr2queueingout)", ret);

    /* ---------------------------------------------------
     * 3. Create blackboards
     * --------------------------------------------------- */
    CREATE_BLACKBOARD(
        "bb_acc3",
        256,                      /* max message size (bytes)          */
        &bb_acc3_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc3)", ret);

    CREATE_BLACKBOARD(
        "bb_t2t3",
        256,
        &bb_t2t3_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_t2t3)", ret);

    CREATE_BLACKBOARD(
        "bb_t3t2",
        256,
        &bb_t3t2_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_t3t2)", ret);

    /* ---------------------------------------------------
     * 4. Create buffers
     * --------------------------------------------------- */
    CREATE_BUFFER(
        "buf_order",
        256,                      /* max message size (bytes)          */
        10,                       /* max number of messages            */
        FIFO,                     /* queuing discipline                */
        &buf_order_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER(buf_order)", ret);

    CREATE_BUFFER(
        "buf_t2tot3",
        256,
        10,
        FIFO,
        &buf_t2tot3_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER(buf_t2tot3)", ret);

    CREATE_BUFFER(
        "buf_t3tot2",
        256,
        10,
        FIFO,
        &buf_t3tot2_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER(buf_t3tot2)", ret);

    /* ---------------------------------------------------
     * 5. Create processes (tasks)
     * --------------------------------------------------- */

    /* --- task21: period=50 ms, priority=2 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task21");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task21_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE /
                              ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY     = 2;
    tattr.PERIOD            = 50000000LL;    /* 50 ms in ns  */
    tattr.TIME_CAPACITY     = 50000000LL;    /* WCET budget  */
    tattr.DEADLINE          = SOFT;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task21)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task21)", ret);

    /* --- task22: period=50 ms, priority=3 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task22");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task22_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE /
                              ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY     = 3;
    tattr.PERIOD            = 50000000LL;    /* 50 ms in ns  */
    tattr.TIME_CAPACITY     = 50000000LL;
    tattr.DEADLINE          = SOFT;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task22)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task22)", ret);

    /* --- task23: period=100 ms, priority=4 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task23");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task23_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE /
                              ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY     = 4;
    tattr.PERIOD            = 100000000LL;   /* 100 ms in ns */
    tattr.TIME_CAPACITY     = 100000000LL;
    tattr.DEADLINE          = SOFT;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task23)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task23)", ret);

    /* ---------------------------------------------------
     * 6. Switch partition to NORMAL mode — starts scheduling
     * --------------------------------------------------- */
    printf("=== Setting partition mode to NORMAL ===\n");
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);

    /*
     * Execution should not reach here after SET_PARTITION_MODE(NORMAL).
     * If it does, loop indefinitely.
     */
    while (1) { ; }
}