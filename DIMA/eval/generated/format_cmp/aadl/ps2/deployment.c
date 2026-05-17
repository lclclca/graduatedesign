/* ============================================================
 * deployment.c
 * Partition P2 – Deployment & Initialisation
 * ARINC 653 / ACoreOS653
 *
 * Called once from main() during the COLD_START / WARM_START
 * initialisation phase, before SET_PARTITION_MODE(NORMAL).
 * ============================================================ */

#include "deployment.h"
#include "globals.h"
#include "activity.h"
#include "gtypes.h"
#include "apex/apex.h"

/* ============================================================
 * module_HM_callback
 *
 * Invoked by the ARINC 653 kernel when a module-level error
 * occurs (e.g. hardware fault detected by the hypervisor).
 * DAL-A requirement: must be registered and must not return
 * without taking a recovery action.
 * ============================================================ */
void module_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    RETURN_CODE_TYPE rc;

    /* Log the error code for maintenance recording ----------- */
    /* (Platform-specific trace call omitted for portability)   */
    (void)error_status;

    /* Default module-level recovery: reset the module -------- */
    /* In a real system this would be a certified system call.   */
    STOP_SELF();   /* Prevents callback from returning silently  */
    (void)rc;
}

/* ============================================================
 * partition_HM_callback
 *
 * Invoked by the kernel when a partition-level error occurs
 * (e.g. process deadline miss, stack overflow, memory fault).
 * ============================================================ */
void partition_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    RETURN_CODE_TYPE    rc;
    PARTITION_STATUS_TYPE part_status;

    /* Inspect partition status for diagnostics --------------- */
    GET_PARTITION_STATUS(&part_status, &rc);
    /* rc intentionally unchecked here: we are already in an
     * error handler and must not recurse into CHECK_CODE.      */

    switch (error_status->ERROR_CODE) {
        case DEADLINE_MISSED:
        case APPLICATION_ERROR:
        case NUMERIC_ERROR:
        case ILLEGAL_REQUEST:
            /* Restart the offending process if identifiable ---- */
            if (error_status->FAILED_PROCESS_ID != 0) {
                START(error_status->FAILED_PROCESS_ID, &rc);
            }
            break;

        case STACK_OVERFLOW:
        case MEMORY_VIOLATION:
        case HARDWARE_FAULT:
        default:
            /* Catastrophic – reset partition              ------- */
            SET_PARTITION_MODE(COLD_START, &rc);
            break;
    }
}

/* ============================================================
 * deployment_init
 *
 * Creates all partition resources in COLD_START / WARM_START.
 * ============================================================ */
void deployment_init(void)
{
    RETURN_CODE_TYPE    rc;
    PROCESS_ATTRIBUTE_TYPE  attr;

    /* ----------------------------------------------------------
     * 1. Inter-partition ports
     * ---------------------------------------------------------- */

    /* Sampling input port: pr2samplingin (source = external)    */
    CREATE_SAMPLING_PORT(
        (SAMPLING_PORT_NAME_TYPE)SAMPLING_PORT_IN_NAME,
        (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
        DESTINATION,
        (SYSTEM_TIME_TYPE)SAMPLING_PORT_IN_PERIOD_NS,
        &g_sampling_port_in,
        &rc);
    CHECK_CODE("CREATE_SAMPLING_PORT pr2samplingin", rc);

    /* Queuing output port: pr2queueingout (sink = external)     */
    CREATE_QUEUING_PORT(
        (QUEUING_PORT_NAME_TYPE)QUEUING_PORT_OUT_NAME,
        (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
        (MESSAGE_RANGE_TYPE)QUEUING_PORT_OUT_DEPTH,
        SOURCE,
        FIFO,
        &g_queuing_port_out,
        &rc);
    CHECK_CODE("CREATE_QUEUING_PORT pr2queueingout", rc);

    /* ----------------------------------------------------------
     * 2. Intra-partition blackboards
     *
     * Each blackboard has exactly one writer and one reader,
     * matching the AADL port connections.
     * ---------------------------------------------------------- */

    /* BB: task21 writes → task22 reads  (task1_acc3_out / task2_acc3_in) */
    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)BLACKBOARD_21_22_NAME,
        (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
        &g_bb_21_22,
        &rc);
    CHECK_CODE("CREATE_BLACKBOARD bb_21_22", rc);

    /* BB: task22 writes → task23 reads  (task2_blackboard_write / task3_blackboard_read) */
    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)BLACKBOARD_22_23_NAME,
        (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
        &g_bb_22_23,
        &rc);
    CHECK_CODE("CREATE_BLACKBOARD bb_22_23", rc);

    /* BB: task23 writes → task22 reads  (task3_blackboard_write / task2_blackboard_read) */
    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)BLACKBOARD_23_22_NAME,
        (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
        &g_bb_23_22,
        &rc);
    CHECK_CODE("CREATE_BLACKBOARD bb_23_22", rc);

    /* ----------------------------------------------------------
     * 3. Intra-partition buffers
     *
     * Bidirectional pairs, each direction a separate buffer,
     * matching the AADL event-data port connections.
     * ---------------------------------------------------------- */

    /* BUF: task22 sends → task21 receives  (task2_order_out / task1_order_in) */
    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)BUFFER_22_21_NAME,
        (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
        (MESSAGE_RANGE_TYPE)BUFFER_MAX_MSG,
        FIFO,
        &g_buf_22_21,
        &rc);
    CHECK_CODE("CREATE_BUFFER buf_22_21", rc);

    /* BUF: task22 sends → task23 receives  (task2_buffer_send / task3_buffer_receive) */
    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)BUFFER_22_23_NAME,
        (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
        (MESSAGE_RANGE_TYPE)BUFFER_MAX_MSG,
        FIFO,
        &g_buf_22_23,
        &rc);
    CHECK_CODE("CREATE_BUFFER buf_22_23", rc);

    /* BUF: task23 sends → task22 receives  (task3_buffer_send / task2_buffer_receive) */
    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)BUFFER_23_22_NAME,
        (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
        (MESSAGE_RANGE_TYPE)BUFFER_MAX_MSG,
        FIFO,
        &g_buf_23_22,
        &rc);
    CHECK_CODE("CREATE_BUFFER buf_23_22", rc);

    /* ----------------------------------------------------------
     * 4. Processes  (AADL threads → ARINC 653 processes)
     * ---------------------------------------------------------- */

    /* ---- Tsk21: Period 50 ms, Priority 2 ------------------- */
    attr.NAME            = TASK21_NAME;
    attr.ENTRY_POINT     = (SYSTEM_ADDRESS_TYPE)task21_entry;
    attr.STACK_SIZE      = (STACK_SIZE_TYPE)TASK21_STACK;
    attr.BASE_PRIORITY   = (PRIORITY_TYPE)TASK21_PRIORITY;
    attr.PERIOD          = (SYSTEM_TIME_TYPE)TASK21_PERIOD;
    attr.TIME_CAPACITY   = (SYSTEM_TIME_TYPE)TASK21_PERIOD; /* WCET ≤ period */
    attr.DEADLINE        = HARD;

    CREATE_PROCESS(&attr, &g_task21_id, &rc);
    CHECK_CODE("CREATE_PROCESS Tsk21", rc);

    /* ---- Tsk22: Period 50 ms, Priority 3 ------------------- */
    attr.NAME            = TASK22_NAME;
    attr.ENTRY_POINT     = (SYSTEM_ADDRESS_TYPE)task22_entry;
    attr.STACK_SIZE      = (STACK_SIZE_TYPE)TASK22_STACK;
    attr.BASE_PRIORITY   = (PRIORITY_TYPE)TASK22_PRIORITY;
    attr.PERIOD          = (SYSTEM_TIME_TYPE)TASK22_PERIOD;
    attr.TIME_CAPACITY   = (SYSTEM_TIME_TYPE)TASK22_PERIOD;
    attr.DEADLINE        = HARD;

    CREATE_PROCESS(&attr, &g_task22_id, &rc);
    CHECK_CODE("CREATE_PROCESS Tsk22", rc);

    /* ---- Tsk23: Period 100 ms, Priority 4 ------------------ */
    attr.NAME            = TASK23_NAME;
    attr.ENTRY_POINT     = (SYSTEM_ADDRESS_TYPE)task23_entry;
    attr.STACK_SIZE      = (STACK_SIZE_TYPE)TASK23_STACK;
    attr.BASE_PRIORITY   = (PRIORITY_TYPE)TASK23_PRIORITY;
    attr.PERIOD          = (SYSTEM_TIME_TYPE)TASK23_PERIOD;
    attr.TIME_CAPACITY   = (SYSTEM_TIME_TYPE)TASK23_PERIOD;
    attr.DEADLINE        = HARD;

    CREATE_PROCESS(&attr, &g_task23_id, &rc);
    CHECK_CODE("CREATE_PROCESS Tsk23", rc);

    /* ----------------------------------------------------------
     * 5. Start all processes
     *    (Processes enter WAITING state; kernel dispatches them
     *     according to their period once NORMAL mode begins.)
     * ---------------------------------------------------------- */
    START(g_task21_id, &rc);
    CHECK_CODE("START Tsk21", rc);

    START(g_task22_id, &rc);
    CHECK_CODE("START Tsk22", rc);

    START(g_task23_id, &rc);
    CHECK_CODE("START Tsk23", rc);
}