#include <apex/apex.h>
#include <apex/apexBlackboard.h>
#include <apex/apexBuffer.h>
#include <apex/apexProcess.h>
#include <apex/apexSampling.h>
#include <apex/apexQueuing.h>
#include <apex/apexError.h>
#include <apex/apexPartition.h>

#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "activity.h"
#include "subprograms.h"

/* ================================================================
 * Health Monitor Callbacks
 * ================================================================ */

/**
 * module_HM_callback
 *   Invoked by the OS when a module-level health-monitor event is
 *   raised.  The partition must decide whether to stop, reset, or
 *   ignore the fault.  For a hard fault we stop the partition.
 */
void module_HM_callback(HM_ERROR_ID_TYPE   error_id,
                         HM_ERROR_STATUS_TYPE *error_status)
{
    RETURN_CODE_TYPE ret = NO_ERROR;

    (void)error_id;
    (void)error_status;

    /*
     * Policy: stop this partition on any module-level HM event.
     * In a production system the policy table drives this decision.
     */
    SET_PARTITION_MODE(IDLE, &ret);
    /* If SET_PARTITION_MODE fails there is nothing more we can do. */
    (void)ret;
}

/**
 * partition_HM_callback
 *   Invoked by the OS when a partition-level health-monitor event
 *   is raised (e.g. process exception, stack overflow, deadline
 *   miss).  We log the event and restart the offending process or
 *   the whole partition depending on severity.
 */
void partition_HM_callback(HM_ERROR_ID_TYPE   error_id,
                            HM_ERROR_STATUS_TYPE *error_status)
{
    RETURN_CODE_TYPE ret = NO_ERROR;

    commandboard_printinfos_spg("partition HM error_id",
                                (T_INT_DATA)error_id);
    (void)error_status;

    /*
     * Policy: for process-level faults attempt a warm restart of
     * the partition; for all others go IDLE.
     */
    if (error_id == PROCESS_ERROR) {
        SET_PARTITION_MODE(WARM_START, &ret);
    } else {
        SET_PARTITION_MODE(IDLE, &ret);
    }
    (void)ret;
}

/* ================================================================
 * Initialisation helpers
 * ================================================================ */

static void init_ports(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;

    /* --- Sampling INPUT port: pr2samplingin --- */
    CREATE_SAMPLING_PORT(
        (SAMPLING_PORT_NAME_TYPE)PR2SAMPLINGIN_NAME,
        (MESSAGE_SIZE_TYPE)PR2SAMPLINGIN_MSG_SIZE,
        DESTINATION,
        (SYSTEM_TIME_TYPE)PR2SAMPLINGIN_REFRESH,
        &g_pr2samplingin_id,
        &ret
    );
    CHECK_CODE("CREATE_SAMPLING_PORT pr2samplingin", ret);

    /* --- Queuing OUTPUT port: pr2queueingout --- */
    CREATE_QUEUING_PORT(
        (QUEUING_PORT_NAME_TYPE)PR2QUEUEINGOUT_NAME,
        (MESSAGE_SIZE_TYPE)PR2QUEUEINGOUT_MSG_SIZE,
        (MESSAGE_RANGE_TYPE)PR2QUEUEINGOUT_DEPTH,
        SOURCE,
        FIFO,
        &g_pr2queueingout_id,
        &ret
    );
    CHECK_CODE("CREATE_QUEUING_PORT pr2queueingout", ret);
}

static void init_blackboards(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)BB_ACC3_NAME,
        (MESSAGE_SIZE_TYPE)BB_MSG_SIZE,
        &g_bb_acc3_id,
        &ret
    );
    CHECK_CODE("CREATE_BLACKBOARD bb_acc3", ret);

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)BB_T2T3_NAME,
        (MESSAGE_SIZE_TYPE)BB_MSG_SIZE,
        &g_bb_t2t3_id,
        &ret
    );
    CHECK_CODE("CREATE_BLACKBOARD bb_t2t3", ret);

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)BB_T3T2_NAME,
        (MESSAGE_SIZE_TYPE)BB_MSG_SIZE,
        &g_bb_t3t2_id,
        &ret
    );
    CHECK_CODE("CREATE_BLACKBOARD bb_t3t2", ret);
}

static void init_buffers(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;

    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)BUF_ORDER_NAME,
        (MESSAGE_SIZE_TYPE)BUF_MSG_SIZE,
        (MESSAGE_RANGE_TYPE)BUF_DEPTH,
        FIFO,
        &g_buf_order_id,
        &ret
    );
    CHECK_CODE("CREATE_BUFFER buf_order", ret);

    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)BUF_T2TOT3_NAME,
        (MESSAGE_SIZE_TYPE)BUF_MSG_SIZE,
        (MESSAGE_RANGE_TYPE)BUF_DEPTH,
        FIFO,
        &g_buf_t2tot3_id,
        &ret
    );
    CHECK_CODE("CREATE_BUFFER buf_t2tot3", ret);

    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)BUF_T3TOT2_NAME,
        (MESSAGE_SIZE_TYPE)BUF_MSG_SIZE,
        (MESSAGE_RANGE_TYPE)BUF_DEPTH,
        FIFO,
        &g_buf_t3tot2_id,
        &ret
    );
    CHECK_CODE("CREATE_BUFFER buf_t3tot2", ret);
}

static void init_processes(void)
{
    RETURN_CODE_TYPE   ret  = NO_ERROR;
    PROCESS_ATTRIBUTE_TYPE attr;

    /* ---- task21 ---- */
    attr.PERIOD        = (SYSTEM_TIME_TYPE)TASK21_PERIOD;
    attr.TIME_CAPACITY = (SYSTEM_TIME_TYPE)TASK21_PERIOD;
    attr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task21_proc;
    attr.STACK_SIZE    = (STACK_SIZE_TYPE)STACKS_SIZE;
    attr.BASE_PRIORITY = (PRIORITY_TYPE)TASK21_PRIORITY;
    attr.DEADLINE      = SOFT;

    CREATE_PROCESS(&attr, &g_task21_id, &ret);
    CHECK_CODE("CREATE_PROCESS task21", ret);

    /* ---- task22 ---- */
    attr.PERIOD        = (SYSTEM_TIME_TYPE)TASK22_PERIOD;
    attr.TIME_CAPACITY = (SYSTEM_TIME_TYPE)TASK22_PERIOD;
    attr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task22_proc;
    attr.STACK_SIZE    = (STACK_SIZE_TYPE)STACKS_SIZE;
    attr.BASE_PRIORITY = (PRIORITY_TYPE)TASK22_PRIORITY;
    attr.DEADLINE      = SOFT;

    CREATE_PROCESS(&attr, &g_task22_id, &ret);
    CHECK_CODE("CREATE_PROCESS task22", ret);

    /* ---- task23 ---- */
    attr.PERIOD        = (SYSTEM_TIME_TYPE)TASK23_PERIOD;
    attr.TIME_CAPACITY = (SYSTEM_TIME_TYPE)TASK23_PERIOD;
    attr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task23_proc;
    attr.STACK_SIZE    = (STACK_SIZE_TYPE)STACKS_SIZE;
    attr.BASE_PRIORITY = (PRIORITY_TYPE)TASK23_PRIORITY;
    attr.DEADLINE      = SOFT;

    CREATE_PROCESS(&attr, &g_task23_id, &ret);
    CHECK_CODE("CREATE_PROCESS task23", ret);
}

static void start_processes(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;

    START(g_task21_id, &ret);
    CHECK_CODE("START task21", ret);

    START(g_task22_id, &ret);
    CHECK_CODE("START task22", ret);

    START(g_task23_id, &ret);
    CHECK_CODE("START task23", ret);
}

/* ================================================================
 * Partition entry point
 * ================================================================ */
int main(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;

    /* ---- 1. Create inter-partition ports ---- */
    init_ports();

    /* ---- 2. Create intra-partition IPC resources ---- */
    init_blackboards();
    init_buffers();

    /* ---- 3. Create and start periodic processes ---- */
    init_processes();
    start_processes();

    /* ---- 4. Transition to NORMAL operating mode ---- */
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE NORMAL", ret);

    /*
     * Execution never reaches here in NORMAL mode because
     * SET_PARTITION_MODE is a non-returning call when mode == NORMAL.
     */
    return 0;
}