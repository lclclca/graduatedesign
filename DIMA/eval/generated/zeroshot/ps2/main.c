#include <stdio.h>
#include <string.h>

#include <os/pos/apex/apexLib.h>

#include "deployment.h"
#include "globals.h"
#include "gtypes.h"
#include "activity.h"

/* ================================================================
 * Global IPC object identifiers
 * These are declared here and referenced as extern in activity.c
 * ================================================================ */

/* Sampling ports */
SAMPLING_PORT_ID_TYPE  pr2samplingin_id;

/* Queuing ports */
QUEUING_PORT_ID_TYPE   pr2queueingout_id;

/* Blackboards */
BLACKBOARD_ID_TYPE     bb_acc3_id;
BLACKBOARD_ID_TYPE     bb_t2t3_id;
BLACKBOARD_ID_TYPE     bb_t3t2_id;

/* Buffers */
BUFFER_ID_TYPE         buf_order_id;
BUFFER_ID_TYPE         buf_t2tot3_id;
BUFFER_ID_TYPE         buf_t3tot2_id;

/* ================================================================
 * Health-Monitor callbacks (mandatory ARINC 653 symbols)
 * ================================================================ */

void module_HM_callback(ERROR_STATUS_TYPE *error)
{
    printf("[HM] module_HM_callback: error level=%d, code=%d\n",
           (int)error->ERROR_CODE,
           (int)error->FAILED_PROCESS_ID);
    /* TODO: implement module-level health monitoring recovery */
}

void partition_HM_callback(ERROR_STATUS_TYPE *error)
{
    printf("[HM] partition_HM_callback: error level=%d, code=%d\n",
           (int)error->ERROR_CODE,
           (int)error->FAILED_PROCESS_ID);
    /* TODO: implement partition-level health monitoring recovery */
}

/* ================================================================
 * appMain — partition initialisation entry point
 * ================================================================ */
void appMain(void)
{
    RETURN_CODE_TYPE    ret;
    PROCESS_ID_TYPE     proc_id;
    PROCESS_ATTRIBUTE_TYPE tattr;

    printf("[INIT] Partition ps2 (module M1) starting...\n");

    /* --------------------------------------------------------
     * 1. Create Sampling Ports
     * -------------------------------------------------------- */

    /* pr2samplingin — DESTINATION (READ) */
    CREATE_SAMPLING_PORT(
        "pr2samplingin",            /* port name must match XML config */
        256,                        /* max message size (bytes)        */
        DESTINATION,                /* direction                       */
        1000000000LL,               /* refresh period: 1 s (ns)        */
        &pr2samplingin_id,
        &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT(pr2samplingin)", ret);

    /* --------------------------------------------------------
     * 2. Create Queuing Ports
     * -------------------------------------------------------- */

    /* pr2queueingout — SOURCE (SEND) */
    CREATE_QUEUING_PORT(
        "pr2queueingout",           /* port name must match XML config */
        256,                        /* max message size (bytes)        */
        30,                         /* max nb messages                 */
        SOURCE,                     /* direction                       */
        FIFO,                       /* queuing discipline              */
        &pr2queueingout_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(pr2queueingout)", ret);

    /* --------------------------------------------------------
     * 3. Create Blackboards
     * -------------------------------------------------------- */

    CREATE_BLACKBOARD(
        "bb_acc3",
        256,                        /* max message size (bytes) */
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

    /* --------------------------------------------------------
     * 4. Create Buffers
     * -------------------------------------------------------- */

    CREATE_BUFFER(
        "buf_order",
        256,                        /* max message size (bytes) */
        10,                         /* max nb messages          */
        FIFO,
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

    /* --------------------------------------------------------
     * 5. Create Processes (ARINC 653 periodic threads)
     * -------------------------------------------------------- */

    /* --- task21 : period 50 ms, priority 2 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task21");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task21_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE /
                              ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY     = 2;
    tattr.PERIOD            = 50LL * 1000000LL;   /* 50 ms in ns  */
    tattr.TIME_CAPACITY     = 50LL * 1000000LL;   /* WCET budget  */
    tattr.DEADLINE          = HARD;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task21)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task21)", ret);

    /* --- task22 : period 50 ms, priority 3 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task22");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task22_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE /
                              ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY     = 3;
    tattr.PERIOD            = 50LL * 1000000LL;
    tattr.TIME_CAPACITY     = 50LL * 1000000LL;
    tattr.DEADLINE          = HARD;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task22)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task22)", ret);

    /* --- task23 : period 100 ms, priority 4 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task23");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task23_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE /
                              ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY     = 4;
    tattr.PERIOD            = 100LL * 1000000LL;  /* 100 ms in ns */
    tattr.TIME_CAPACITY     = 100LL * 1000000LL;
    tattr.DEADLINE          = HARD;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task23)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task23)", ret);

    /* --------------------------------------------------------
     * 6. Switch partition to NORMAL mode — releases all processes
     * -------------------------------------------------------- */
    printf("[INIT] Setting partition mode to NORMAL...\n");
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);

    /* Should not be reached after SET_PARTITION_MODE(NORMAL) */
    printf("[WARN] appMain returned unexpectedly.\n");
}