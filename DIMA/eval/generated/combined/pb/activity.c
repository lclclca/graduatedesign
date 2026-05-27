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
extern BLACKBOARD_ID_TYPE    pb_bb_data_link_id;

void* taskB1_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        /* Write sensor data to the SOURCE sampling port */
        WRITE_SAMPLING_MESSAGE(pb_sensor_out_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);
        CHECK_CODE("WRITE_SAMPLING_MESSAGE(sensor_out)", ret);

        /* Read the latest shared data from the blackboard */
        READ_BLACKBOARD(pb_bb_data_link_id, 0ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("READ_BLACKBOARD(bb_data_link)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskB1)", ret);
    }
}

extern QUEUING_PORT_ID_TYPE  pb_cmd_in_id;

void* taskB2_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        /* Receive command from the DESTINATION queuing port */
        RECEIVE_QUEUING_MESSAGE(pb_cmd_in_id, 5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(cmd_in)", ret);

        /* Publish the received command to the blackboard */
        DISPLAY_BLACKBOARD(pb_bb_data_link_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_data_link)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskB2)", ret);
    }
}