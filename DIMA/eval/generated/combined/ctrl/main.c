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
QUEUING_PORT_ID_TYPE ctrl_nav_in_id;
QUEUING_PORT_ID_TYPE ctrl_ctrl_out_id;

/* ---- Intra-partition buffers ---- */
BUFFER_ID_TYPE ctrl_buf_ctrl_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    strcpy(tattr.NAME, "taskT1");
    tattr.ENTRY_POINT   = taskT1_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 25000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskT1)", ret);
    START(arinc_threads[0], &ret);
    CHECK_CODE("START(taskT1)", ret);

    strcpy(tattr.NAME, "taskT2");
    tattr.ENTRY_POINT   = taskT2_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[1]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskT2)", ret);
    START(arinc_threads[1], &ret);
    CHECK_CODE("START(taskT2)", ret);

    strcpy(tattr.NAME, "taskT3");
    tattr.ENTRY_POINT   = taskT3_job;
    tattr.BASE_PRIORITY = 4;
    tattr.PERIOD        = 100000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 100000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[2]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskT3)", ret);
    START(arinc_threads[2], &ret);
    CHECK_CODE("START(taskT3)", ret);

    CREATE_QUEUING_PORT("nav_in", sizeof(integer), 4, DESTINATION, FIFO,
                        &(ctrl_nav_in_id), &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(nav_in)", ret);

    CREATE_QUEUING_PORT("ctrl_out", sizeof(integer), 4, SOURCE, FIFO,
                        &(ctrl_ctrl_out_id), &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(ctrl_out)", ret);

    CREATE_BUFFER("buf_ctrl", sizeof(integer), 4, FIFO,
                  &(ctrl_buf_ctrl_id), &ret);
    CHECK_CODE("CREATE_BUFFER(buf_ctrl)", ret);

    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}

void module_HM_callback(ERROR_STATUS_TYPE *error_status) {
    printf("[HM MODULE ctrl] ErrorID=%d  FailedProcess=%d  State=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID,
           (int)error_status->SYSTEM_STATE);
}

void partition_HM_callback(ERROR_STATUS_TYPE *error_status) {
    printf("[HM PARTITION ctrl] ErrorID=%d  FailedProcess=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID);
}