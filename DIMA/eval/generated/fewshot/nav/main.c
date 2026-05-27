#include <os/pos/apex/apexLib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

PROCESS_ID_TYPE arinc_threads[ACoreOS653_CONFIG_NB_THREADS];

/* ---- Inter-partition ports ---- */
SAMPLING_PORT_ID_TYPE nav_pos_out_id;
SAMPLING_PORT_ID_TYPE nav_status_out_id;
QUEUING_PORT_ID_TYPE  nav_cmd_out_id;

/* ---- Intra-partition communication ---- */
BLACKBOARD_ID_TYPE    nav_bb_bb1_id;
BLACKBOARD_ID_TYPE    nav_bb_bb2_id;
BUFFER_ID_TYPE        nav_buf_buf1_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    strcpy(tattr.NAME, "taskN1");
    tattr.ENTRY_POINT   = taskN1_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 25000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskN1)", ret);
    START(arinc_threads[0], &ret);
    CHECK_CODE("START(taskN1)", ret);

    strcpy(tattr.NAME, "taskN2");
    tattr.ENTRY_POINT   = taskN2_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[1]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskN2)", ret);
    START(arinc_threads[1], &ret);
    CHECK_CODE("START(taskN2)", ret);

    strcpy(tattr.NAME, "taskN3");
    tattr.ENTRY_POINT   = taskN3_job;
    tattr.BASE_PRIORITY = 4;
    tattr.PERIOD        = 100000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 100000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[2]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskN3)", ret);
    START(arinc_threads[2], &ret);
    CHECK_CODE("START(taskN3)", ret);

    strcpy(tattr.NAME, "taskN4");
    tattr.ENTRY_POINT   = taskN4_job;
    tattr.BASE_PRIORITY = 5;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[3]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskN4)", ret);
    START(arinc_threads[3], &ret);
    CHECK_CODE("START(taskN4)", ret);

    CREATE_SAMPLING_PORT(
        "pos_out",
        sizeof(integer),
        SOURCE,
        100000000ll,
        &(nav_pos_out_id), &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT(pos_out)", ret);

    CREATE_SAMPLING_PORT(
        "status_out",
        sizeof(integer),
        SOURCE,
        100000000ll,
        &(nav_status_out_id), &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT(status_out)", ret);

    CREATE_QUEUING_PORT(
        "cmd_out",
        sizeof(integer),
        4,
        SOURCE,
        FIFO,
        &(nav_cmd_out_id), &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(cmd_out)", ret);

    CREATE_BLACKBOARD(
        "bb_bb1",
        sizeof(integer),
        &(nav_bb_bb1_id), &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_bb1)", ret);

    CREATE_BLACKBOARD(
        "bb_bb2",
        sizeof(integer),
        &(nav_bb_bb2_id), &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_bb2)", ret);

    CREATE_BUFFER(
        "buf_buf1",
        sizeof(integer),
        4,
        FIFO,
        &(nav_buf_buf1_id), &ret);
    CHECK_CODE("CREATE_BUFFER(buf_buf1)", ret);

    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}

void module_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM MODULE nav] ErrorID=%d  FailedProcess=%d  State=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID,
           (int)error_status->SYSTEM_STATE);
}

void partition_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM PARTITION nav] ErrorID=%d  FailedProcess=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID);
}