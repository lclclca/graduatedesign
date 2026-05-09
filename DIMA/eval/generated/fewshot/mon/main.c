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
SAMPLING_PORT_ID_TYPE mon_status_in_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    strcpy(tattr.NAME, "taskM1");
    tattr.ENTRY_POINT   = taskM1_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 100000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 100000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &ret);
    CHECK_CODE("CREATE_PROCESS(taskM1)", ret);
    START(arinc_threads[0], &ret);
    CHECK_CODE("START(taskM1)", ret);

    CREATE_SAMPLING_PORT(
        "status_in",
        sizeof(integer),
        DESTINATION,
        100000000ll,
        &(mon_status_in_id), &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT(status_in)", ret);

    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}

void module_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM MODULE mon] ErrorID=%d  FailedProcess=%d  State=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID,
           (int)error_status->SYSTEM_STATE);
}

void partition_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM PARTITION mon] ErrorID=%d  FailedProcess=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID);
}