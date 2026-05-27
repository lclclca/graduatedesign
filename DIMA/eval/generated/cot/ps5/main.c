#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "deployment.h"
#include "globals.h"
#include "gtypes.h"

/* ------------------------------------------------------------------ */
/* Global port ID arrays                                               */
/* ------------------------------------------------------------------ */
QUEUING_PORT_ID_TYPE ps5_queuing_port_ids[NB_QUEUEINGS];

/* ------------------------------------------------------------------ */
/* Global thread ID array                                              */
/* ------------------------------------------------------------------ */
PROCESS_ID_TYPE arinc_threads[NB_THREADS];

/* ------------------------------------------------------------------ */
/* appMain — partition initialisation entry point                      */
/* ------------------------------------------------------------------ */
void appMain(void)
{
    RETURN_CODE_TYPE       ret;
    PROCESS_ATTRIBUTE_TYPE tattr;

    /* ---- Create queuing port: order_in (DESTINATION) ---- */
    CREATE_QUEUING_PORT(
        "order_in",
        sizeof(integer),
        4,
        DESTINATION,
        FIFO,
        &ps5_queuing_port_ids[ORDER_IN_IDX],
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT order_in", ret);

    /* ---- Create queuing port: queueingout (SOURCE) ---- */
    CREATE_QUEUING_PORT(
        "queueingout",
        sizeof(integer),
        4,
        SOURCE,
        FIFO,
        &ps5_queuing_port_ids[QUEUEINGOUT_IDX],
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT queueingout", ret);

    /* ---- Create task51 ---- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME,      "task51");
    tattr.ENTRY_POINT   = task51_job;
    tattr.BASE_PRIORITY = 1;
    tattr.PERIOD        = 50000000ll;   /* 50 ms */
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK51_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task51", ret);
    START(arinc_threads[TASK51_IDX], &ret);
    CHECK_CODE("START task51", ret);

    /* ---- Create task52 ---- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME,      "task52");
    tattr.ENTRY_POINT   = task52_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 50000000ll;   /* 50 ms */
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK52_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task52", ret);
    START(arinc_threads[TASK52_IDX], &ret);
    CHECK_CODE("START task52", ret);

    /* ---- Create task53 ---- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME,      "task53");
    tattr.ENTRY_POINT   = task53_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 200000000ll;  /* 200 ms */
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 200000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[TASK53_IDX], &ret);
    CHECK_CODE("CREATE_PROCESS task53", ret);
    START(arinc_threads[TASK53_IDX], &ret);
    CHECK_CODE("START task53", ret);

    /* ---- Switch partition to NORMAL mode ---- */
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE NORMAL", ret);
}

/* ------------------------------------------------------------------ */
/* Health-monitor callbacks                                            */
/* ------------------------------------------------------------------ */
void module_HM_callback(ERROR_STATUS_TYPE *error)
{
    (void)error;
    /* Module-level health monitoring: no action defined for ps5 */
}

void partition_HM_callback(ERROR_STATUS_TYPE *error)
{
    (void)error;
    /* Partition-level health monitoring: no action defined for ps5 */
}