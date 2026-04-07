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
SAMPLING_PORT_ID_TYPE pc_data_in_id;
QUEUING_PORT_ID_TYPE  pc_cmd_out_id;

/* ---- Intra-partition resources ---- */
BLACKBOARD_ID_TYPE pc_bb_data_link_id;
BUFFER_ID_TYPE     pc_buf_msg_queue_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    strcpy(tattr.NAME, "taskC1");
    tattr.ENTRY_POINT   = taskC1_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 25000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskC1)", ret);
    START(arinc_threads[0], &ret);
    CHECK_CODE("START(taskC1)", ret);

    strcpy(tattr.NAME, "taskC2");
    tattr.ENTRY_POINT   = taskC2_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[1]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskC2)", ret);
    START(arinc_threads[1], &ret);
    CHECK_CODE("START(taskC2)", ret);

    strcpy(tattr.NAME, "taskC3");
    tattr.ENTRY_POINT   = taskC3_job;
    tattr.BASE_PRIORITY = 4;
    tattr.PERIOD        = 100000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[2]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskC3)", ret);
    START(arinc_threads[2], &ret);
    CHECK_CODE("START(taskC3)", ret);

    CREATE_SAMPLING_PORT("data_in", sizeof(integer), DESTINATION, 100ll,
                         &(pc_data_in_id), &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT(data_in)", ret);

    CREATE_QUEUING_PORT("cmd_out", sizeof(integer), 4, SOURCE, FIFO,
                        &(pc_cmd_out_id), &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(cmd_out)", ret);

    CREATE_BLACKBOARD("bb_data_link", sizeof(integer),
                      &(pc_bb_data_link_id), &ret);
    CHECK_CODE("CREATE_BLACKBOARD(bb_data_link)", ret);

    CREATE_BUFFER("buf_msg_queue", sizeof(integer), 4, FIFO,
                  &(pc_buf_msg_queue_id), &ret);
    CHECK_CODE("CREATE_BUFFER(buf_msg_queue)", ret);

    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}

void module_HM_callback(ERROR_STATUS_TYPE *error_status) {
    printf("[HM MODULE pc] ErrorID=%d  FailedProcess=%d  State=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID,
           (int)error_status->SYSTEM_STATE);
}

void partition_HM_callback(ERROR_STATUS_TYPE *error_status) {
    printf("[HM PARTITION pc] ErrorID=%d  FailedProcess=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID);
}