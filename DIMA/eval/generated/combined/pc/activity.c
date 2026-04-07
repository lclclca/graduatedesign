#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

extern SAMPLING_PORT_ID_TYPE pc_data_in_id;
extern BLACKBOARD_ID_TYPE    pc_bb_data_link_id;

void* taskC1_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    VALIDITY_TYPE validity;
    integer val = 0;
    while (1)
    {
        READ_SAMPLING_MESSAGE(pc_data_in_id,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &validity, &ret);
        CHECK_VALIDCODE("READ_SAMPLING_MESSAGE(data_in)", validity, ret);

        DISPLAY_BLACKBOARD(pc_bb_data_link_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_data_link)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskC1)", ret);
    }
}

extern BLACKBOARD_ID_TYPE pc_bb_data_link_id;
extern BUFFER_ID_TYPE     pc_buf_msg_queue_id;

void* taskC2_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        READ_BLACKBOARD(pc_bb_data_link_id, 0ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("READ_BLACKBOARD(bb_data_link)", ret);

        WRITE_BUFFER(pc_buf_msg_queue_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), 5000000ll, &ret);
        CHECK_CODE("WRITE_BUFFER(buf_msg_queue)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskC2)", ret);
    }
}

extern BUFFER_ID_TYPE        pc_buf_msg_queue_id;
extern QUEUING_PORT_ID_TYPE  pc_cmd_out_id;

void* taskC3_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        READ_BUFFER(pc_buf_msg_queue_id, 5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("READ_BUFFER(buf_msg_queue)", ret);

        SEND_QUEUING_MESSAGE(pc_cmd_out_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), 5000000ll, &ret);
        CHECK_CODE("SEND_QUEUING_MESSAGE(cmd_out)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskC3)", ret);
    }
}