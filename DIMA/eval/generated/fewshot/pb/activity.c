#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

extern SAMPLING_PORT_ID_TYPE pb_sensor_out_id;
extern QUEUING_PORT_ID_TYPE  pb_cmd_in_id;
extern BLACKBOARD_ID_TYPE    pb_bb_data_link_id;

void* taskB1_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    integer val = 0;

    while (1)
    {
        WRITE_SAMPLING_MESSAGE(pb_sensor_out_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            &ret);
        CHECK_CODE("WRITE_SAMPLING_MESSAGE(sensor_out)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskB1)", ret);
    }
}

void* taskB2_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;

    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(pb_cmd_in_id,
            5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(cmd_in)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskB2)", ret);
    }
}