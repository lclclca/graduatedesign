#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

extern SAMPLING_PORT_ID_TYPE mon_status_in_id;

void* taskM1_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    VALIDITY_TYPE validity;
    integer val = 0;
    while (1)
    {
        READ_SAMPLING_MESSAGE(mon_status_in_id,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &validity, &ret);
        CHECK_VALIDCODE("READ_SAMPLING_MESSAGE(status_in)", validity, ret);
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskM1)", ret);
    }
}