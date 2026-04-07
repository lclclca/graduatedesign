#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

extern QUEUING_PORT_ID_TYPE pa_ctrl_in_id;

void* taskA1_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(pa_ctrl_in_id, 5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(ctrl_in)", ret);
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskA1)", ret);
    }
}