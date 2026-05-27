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
SAMPLING_PORT_ID_TYPE pb_sensor_out_id;
QUEUING_PORT_ID_TYPE  pb_cmd_in_id;

/* ---- Intra-partition communication ---- */
BLACKBOARD_ID_TYPE    pb_bb_data_link_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    strcpy(tattr.NAME, "taskB1");
    tattr.ENTRY_POINT   = taskB1_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 25000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskB1)", ret);
    START(arinc_threads[0], &ret);
    CHECK_CODE("START(taskB1)", ret);

    strcpy(tattr.NAME, "taskB2");
    tattr.ENTRY_POINT   = taskB2_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[1]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskB2)", ret);
    START(arinc_threads[1], &ret);
    CHECK_CODE("START(taskB2)", ret);

    CREATE_SAMPLING_PORT(
        "sensor_out",
        sizeof(integer),
        SOURCE,
        100ll,
        &(pb_sensor_out_id), &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT(sensor_out)", ret);

    CREATE_QUEUING_PORT(
        "cmd_in",
        sizeof(integer),
        4,
        DESTINATION,
        FIFO,
        &(pb_cmd_in_id), &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(cmd_in)", ret);

    CREATE_BLACKBOARD(
        "bb_data_link",
        sizeof(integer),
        &(pb_bb_data_link_id), &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_data_link)", ret);

    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}

void module_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM MODULE pb] ErrorID=%d  FailedProcess=%d  State=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID,
           (int)error_status->SYSTEM_STATE);
}

void partition_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM PARTITION pb] ErrorID=%d  FailedProcess=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID);
}