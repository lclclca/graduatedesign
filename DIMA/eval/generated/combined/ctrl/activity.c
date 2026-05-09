#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

extern QUEUING_PORT_ID_TYPE ctrl_nav_in_id;
extern BUFFER_ID_TYPE       ctrl_buf_ctrl_id;

void* taskT1_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(ctrl_nav_in_id, 5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(nav_in)", ret);

        WRITE_BUFFER(ctrl_buf_ctrl_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), 5000000ll, &ret);
        CHECK_CODE("WRITE_BUFFER(buf_ctrl)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskT1)", ret);
    }
}

extern BUFFER_ID_TYPE       ctrl_buf_ctrl_id;
extern QUEUING_PORT_ID_TYPE ctrl_ctrl_out_id;

void* taskT2_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        READ_BUFFER(ctrl_buf_ctrl_id, 5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("READ_BUFFER(buf_ctrl)", ret);

        SEND_QUEUING_MESSAGE(ctrl_ctrl_out_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), 5000000ll, &ret);
        CHECK_CODE("SEND_QUEUING_MESSAGE(ctrl_out)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskT2)", ret);
    }
}

extern QUEUING_PORT_ID_TYPE ctrl_nav_in_id;
extern QUEUING_PORT_ID_TYPE ctrl_ctrl_out_id;

void* taskT3_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(ctrl_nav_in_id, 5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(nav_in)", ret);

        SEND_QUEUING_MESSAGE(ctrl_ctrl_out_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), 5000000ll, &ret);
        CHECK_CODE("SEND_QUEUING_MESSAGE(ctrl_out)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskT3)", ret);
    }
}