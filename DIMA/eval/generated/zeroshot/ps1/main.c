#include "activity.h"
#include "deployment.h"
#include "globals.h"
#include "gtypes.h"

#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <string.h>

/* =========================================================================
 * Global IDs for all IPC resources
 * ========================================================================= */

/* Sampling ports */
SAMPLING_PORT_ID_TYPE pr1samplingout_id;

/* Queuing ports */
QUEUING_PORT_ID_TYPE  pr1queueingin_id;

/* Blackboards */
BLACKBOARD_ID_TYPE    bb_acc1_id;
BLACKBOARD_ID_TYPE    bb_acc2_id;
BLACKBOARD_ID_TYPE    bb_sem_id;
BLACKBOARD_ID_TYPE    bb_t3t4_id;
BLACKBOARD_ID_TYPE    bb_t4t3_id;

/* Buffers */
BUFFER_ID_TYPE        buf_sensor_id;
BUFFER_ID_TYPE        buf_t3tot4_id;
BUFFER_ID_TYPE        buf_t4tot3_id;

/* =========================================================================
 * Health-Monitor Callbacks
 * ========================================================================= */

/*
 * module_HM_callback
 * Invoked by the ACoreOS653 health monitor at module level.
 * @error_id : identifies the module-level error detected by the HM.
 */
void module_HM_callback(ERROR_ID_TYPE error_id)
{
    printf("[MODULE HM] Module health-monitor callback invoked, error_id=%d\n",
           (int)error_id);
    /* TODO: implement module-level error handling strategy */
}

/*
 * partition_HM_callback
 * Invoked by the ACoreOS653 health monitor at partition level.
 * @error_id   : identifies the partition-level error.
 * @error_value: optional additional error value.
 */
void partition_HM_callback(ERROR_ID_TYPE    error_id,
                            ERROR_VALUE_TYPE error_value)
{
    printf("[PARTITION HM] Partition health-monitor callback invoked, "
           "error_id=%d error_value=%lld\n",
           (int)error_id, (long long)error_value);
    /* TODO: implement partition-level error handling strategy */
}

/* =========================================================================
 * appMain — Partition initialisation entry point
 * ========================================================================= */
void appMain(void)
{
    RETURN_CODE_TYPE  ret;
    PROCESS_ID_TYPE   proc_id;
    PROCESS_ATTRIBUTE_TYPE tattr;

    printf("[INIT] Partition ps1 (module M1) initialisation start\n");

    /* -----------------------------------------------------------------
     * 1. Create Sampling Ports
     * ----------------------------------------------------------------- */

    /* pr1samplingout — SOURCE (write) */
    CREATE_SAMPLING_PORT(
        (SAMPLING_PORT_NAME_TYPE)"pr1samplingout",   /* port name            */
        (MESSAGE_SIZE_TYPE)256,                       /* max message size     */
        SOURCE,                                       /* direction            */
        (SYSTEM_TIME_TYPE)25000000,                   /* refresh period (ns)  */
        &pr1samplingout_id,
        &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT(pr1samplingout)", ret);

    /* -----------------------------------------------------------------
     * 2. Create Queuing Ports
     * ----------------------------------------------------------------- */

    /* pr1queueingin — DESTINATION (receive), max 30 messages */
    CREATE_QUEUING_PORT(
        (QUEUING_PORT_NAME_TYPE)"pr1queueingin",      /* port name            */
        (MESSAGE_SIZE_TYPE)256,                        /* max message size     */
        (MESSAGE_RANGE_TYPE)30,                        /* max nb messages      */
        DESTINATION,                                   /* direction            */
        FIFO,                                          /* queuing discipline   */
        &pr1queueingin_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(pr1queueingin)", ret);

    /* -----------------------------------------------------------------
     * 3. Create Blackboards
     * ----------------------------------------------------------------- */

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)"bb_acc1",
        (MESSAGE_SIZE_TYPE)256,
        &bb_acc1_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc1)", ret);

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)"bb_acc2",
        (MESSAGE_SIZE_TYPE)256,
        &bb_acc2_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc2)", ret);

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)"bb_sem",
        (MESSAGE_SIZE_TYPE)256,
        &bb_sem_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_sem)", ret);

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)"bb_t3t4",
        (MESSAGE_SIZE_TYPE)256,
        &bb_t3t4_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_t3t4)", ret);

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)"bb_t4t3",
        (MESSAGE_SIZE_TYPE)256,
        &bb_t4t3_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_t4t3)", ret);

    /* -----------------------------------------------------------------
     * 4. Create Buffers
     * ----------------------------------------------------------------- */

    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)"buf_sensor",
        (MESSAGE_SIZE_TYPE)256,
        (MESSAGE_RANGE_TYPE)16,
        FIFO,
        &buf_sensor_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER(buf_sensor)", ret);

    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)"buf_t3tot4",
        (MESSAGE_SIZE_TYPE)256,
        (MESSAGE_RANGE_TYPE)16,
        FIFO,
        &buf_t3tot4_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER(buf_t3tot4)", ret);

    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)"buf_t4tot3",
        (MESSAGE_SIZE_TYPE)256,
        (MESSAGE_RANGE_TYPE)16,
        FIFO,
        &buf_t4tot3_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER(buf_t4tot3)", ret);

    /* -----------------------------------------------------------------
     * 5. Create Processes (Tasks)
     * ----------------------------------------------------------------- */

    /* --- task11 : period 25 ms, priority 2 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task11");
    tattr.ENTRY_POINT        = (SYSTEM_ADDRESS_TYPE)task11_job;
    tattr.STACK_SIZE         = ACoreOS653_NEEDS_STACKS_SIZE;
    tattr.BASE_PRIORITY      = (PRIORITY_TYPE)2;
    tattr.PERIOD             = (SYSTEM_TIME_TYPE)25000000;   /* 25 ms in ns  */
    tattr.TIME_CAPACITY      = (SYSTEM_TIME_TYPE)25000000;
    tattr.DEADLINE           = HARD;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task11)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task11)", ret);

    /* --- task12 : period 50 ms, priority 3 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task12");
    tattr.ENTRY_POINT        = (SYSTEM_ADDRESS_TYPE)task12_job;
    tattr.STACK_SIZE         = ACoreOS653_NEEDS_STACKS_SIZE;
    tattr.BASE_PRIORITY      = (PRIORITY_TYPE)3;
    tattr.PERIOD             = (SYSTEM_TIME_TYPE)50000000;   /* 50 ms in ns  */
    tattr.TIME_CAPACITY      = (SYSTEM_TIME_TYPE)50000000;
    tattr.DEADLINE           = HARD;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task12)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task12)", ret);

    /* --- task13 : period 50 ms, priority 4 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task13");
    tattr.ENTRY_POINT        = (SYSTEM_ADDRESS_TYPE)task13_job;
    tattr.STACK_SIZE         = ACoreOS653_NEEDS_STACKS_SIZE;
    tattr.BASE_PRIORITY      = (PRIORITY_TYPE)4;
    tattr.PERIOD             = (SYSTEM_TIME_TYPE)50000000;
    tattr.TIME_CAPACITY      = (SYSTEM_TIME_TYPE)50000000;
    tattr.DEADLINE           = HARD;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task13)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task13)", ret);

    /* --- task14 : period 50 ms, priority 5 --- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task14");
    tattr.ENTRY_POINT        = (SYSTEM_ADDRESS_TYPE)task14_job;
    tattr.STACK_SIZE         = ACoreOS653_NEEDS_STACKS_SIZE;
    tattr.BASE_PRIORITY      = (PRIORITY_TYPE)5;
    tattr.PERIOD             = (SYSTEM_TIME_TYPE)50000000;
    tattr.TIME_CAPACITY      = (SYSTEM_TIME_TYPE)50000000;
    tattr.DEADLINE           = HARD;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task14)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task14)", ret);

    /* -----------------------------------------------------------------
     * 6. Switch partition to NORMAL operating mode
     * ----------------------------------------------------------------- */
    printf("[INIT] All resources created. Setting partition mode to NORMAL.\n");

    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);

    /* appMain returns after SET_PARTITION_MODE — the kernel takes over. */
}