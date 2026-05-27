#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "deployment.h"
#include "globals.h"
#include "gtypes.h"

/* ------------------------------------------------------------------ */
/* appMain                                                             */
/* Partition entry point: create resources, tasks, then set NORMAL    */
/* ------------------------------------------------------------------ */
void appMain(void)
{
    RETURN_CODE_TYPE       ret;
    PROCESS_ATTRIBUTE_TYPE tattr;

    /* ----------------------------------------------------------------
     * 1. Create queuing ports
     * ---------------------------------------------------------------- */

    /* order_in – DESTINATION, max 4 messages */
    CREATE_QUEUING_PORT(
        "order_in",
        (MESSAGE_SIZE_TYPE)sizeof(integer),
        4,
        DESTINATION,
        FIFO,
        &ps3_queuing_port_ids[ORDER_IN_PORT_IDX],
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT order_in", ret);

    /* queueingin – DESTINATION, max 4 messages */
    CREATE_QUEUING_PORT(
        "queueingin",
        (MESSAGE_SIZE_TYPE)sizeof(integer),
        4,
        DESTINATION,
        FIFO,
        &ps3_queuing_port_ids[QUEUEINGIN_PORT_IDX],
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT queueingin", ret);

    /* ----------------------------------------------------------------
     * 2. Create and start task31
     *    Period: 25 ms = 25000000 ns | Priority: 2
     * ---------------------------------------------------------------- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task31");
    tattr.ENTRY_POINT   = task31_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 25000000ll;
    tattr.STACK_SIZE    = STACK_SIZE;
    tattr.TIME_CAPACITY = 25000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK31_THREAD_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task31", ret);
    START(arinc_threads[TASK31_THREAD_IDX], &ret);
    CHECK_CODE("START task31", ret);

    /* ----------------------------------------------------------------
     * 3. Create and start task32
     *    Period: 50 ms = 50000000 ns | Priority: 3
     * ---------------------------------------------------------------- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task32");
    tattr.ENTRY_POINT   = task32_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = STACK_SIZE;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK32_THREAD_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task32", ret);
    START(arinc_threads[TASK32_THREAD_IDX], &ret);
    CHECK_CODE("START task32", ret);

    /* ----------------------------------------------------------------
     * 4. Create and start task33
     *    Period: 50 ms = 50000000 ns | Priority: 4
     * ---------------------------------------------------------------- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task33");
    tattr.ENTRY_POINT   = task33_job;
    tattr.BASE_PRIORITY = 4;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = STACK_SIZE;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK33_THREAD_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task33", ret);
    START(arinc_threads[TASK33_THREAD_IDX], &ret);
    CHECK_CODE("START task33", ret);

    /* ----------------------------------------------------------------
     * 5. Switch partition to NORMAL mode
     * ---------------------------------------------------------------- */
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE NORMAL", ret);
}

/* ------------------------------------------------------------------ */
/* Health Monitor Callbacks                                            */
/* ------------------------------------------------------------------ */
void module_HM_callback(ERROR_STATUS_TYPE *error)
{
    (void)error;
    /* Module-level health monitor: no action defined for ps3 */
}

void partition_HM_callback(ERROR_STATUS_TYPE *error)
{
    (void)error;
    /* Partition-level health monitor: no action defined for ps3 */
}