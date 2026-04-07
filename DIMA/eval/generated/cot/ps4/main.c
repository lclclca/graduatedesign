#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "deployment.h"
#include "globals.h"
#include "gtypes.h"

/* ------------------------------------------------------------------ */
/* Global port ID arrays (extern-referenced in activity.c)             */
/* ------------------------------------------------------------------ */
QUEUING_PORT_ID_TYPE  arinc_queuing_ports[NB_QUEUEINGS];

/* ------------------------------------------------------------------ */
/* Global process ID array                                             */
/* ------------------------------------------------------------------ */
PROCESS_ID_TYPE arinc_threads[NB_THREADS];

/* ================================================================== */
/* appMain — partition initialisation entry point                      */
/* ================================================================== */
void appMain(void)
{
    RETURN_CODE_TYPE       ret;
    PROCESS_ATTRIBUTE_TYPE tattr;

    /* ---- Create queuing ports ------------------------------------ */

    /* order_in: DESTINATION, max_nb=1 */
    CREATE_QUEUING_PORT(
        "order_in",
        sizeof(integer),
        1,
        DESTINATION,
        FIFO,
        &arinc_queuing_ports[ORDER_IN_PORT_IDX],
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT order_in", ret);

    /* queueingout: SOURCE, max_nb=4 */
    CREATE_QUEUING_PORT(
        "queueingout",
        sizeof(integer),
        4,
        SOURCE,
        FIFO,
        &arinc_queuing_ports[QUEUEINGOUT_PORT_IDX],
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT queueingout", ret);

    /* queueingin: DESTINATION, max_nb=4 */
    CREATE_QUEUING_PORT(
        "queueingin",
        sizeof(integer),
        4,
        DESTINATION,
        FIFO,
        &arinc_queuing_ports[QUEUEINGIN_PORT_IDX],
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT queueingin", ret);

    /* ---- Create and start task41 --------------------------------- */
    /* period: 25 ms = 25000000 ns, priority: 2 */
    strcpy(tattr.NAME, "task41");
    tattr.ENTRY_POINT   = task41_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 25000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 25000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK41_THREAD_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task41", ret);
    START(arinc_threads[TASK41_THREAD_IDX], &ret);
    CHECK_CODE("START task41", ret);

    /* ---- Create and start task42 --------------------------------- */
    /* period: 50 ms = 50000000 ns, priority: 3 */
    strcpy(tattr.NAME, "task42");
    tattr.ENTRY_POINT   = task42_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK42_THREAD_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task42", ret);
    START(arinc_threads[TASK42_THREAD_IDX], &ret);
    CHECK_CODE("START task42", ret);

    /* ---- Create and start task43 --------------------------------- */
    /* period: 50 ms = 50000000 ns, priority: 4 */
    strcpy(tattr.NAME, "task43");
    tattr.ENTRY_POINT   = task43_job;
    tattr.BASE_PRIORITY = 4;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK43_THREAD_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task43", ret);
    START(arinc_threads[TASK43_THREAD_IDX], &ret);
    CHECK_CODE("START task43", ret);

    /* ---- Create and start task44 --------------------------------- */
    /* period: 100 ms = 100000000 ns, priority: 5 */
    strcpy(tattr.NAME, "task44");
    tattr.ENTRY_POINT   = task44_job;
    tattr.BASE_PRIORITY = 5;
    tattr.PERIOD        = 100000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 100000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK44_THREAD_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task44", ret);
    START(arinc_threads[TASK44_THREAD_IDX], &ret);
    CHECK_CODE("START task44", ret);

    /* ---- Create and start task45 --------------------------------- */
    /* period: 200 ms = 200000000 ns, priority: 6 */
    strcpy(tattr.NAME, "task45");
    tattr.ENTRY_POINT   = task45_job;
    tattr.BASE_PRIORITY = 6;
    tattr.PERIOD        = 200000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 200000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK45_THREAD_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task45", ret);
    START(arinc_threads[TASK45_THREAD_IDX], &ret);
    CHECK_CODE("START task45", ret);

    /* ---- Switch partition to NORMAL mode ------------------------- */
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE NORMAL", ret);
}

/* ================================================================== */
/* Health Monitor Callbacks                                            */
/* ================================================================== */

void module_HM_callback(ERROR_STATUS_TYPE *error)
{
    /* Module-level health monitor callback for partition ps4 / module M3.
     * In a production system, log the error and take corrective action.
     * Here we perform a partition restart as a safe default.            */
    RETURN_CODE_TYPE ret;
    (void)error;
    SET_PARTITION_MODE(WARM_START, &ret);
}

void partition_HM_callback(ERROR_STATUS_TYPE *error)
{
    /* Partition-level health monitor callback.
     * Log error identifier and attempt recovery.                       */
    RETURN_CODE_TYPE ret;
    (void)error;
    SET_PARTITION_MODE(WARM_START, &ret);
}