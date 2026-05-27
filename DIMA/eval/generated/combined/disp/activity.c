#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

extern SAMPLING_PORT_ID_TYPE disp_pos_in_id;
extern BLACKBOARD_ID_TYPE    disp_bb_disp_id;

void* taskD1_job(void)
{
    RETURN_CODE_TYPE    ret = NO_ERROR;
    MESSAGE_SIZE_TYPE   msg_len;
    VALIDITY_TYPE       validity;
    integer             val = 0;

    while (1)
    {
        READ_SAMPLING_MESSAGE(disp_pos_in_id,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &validity, &ret);
        CHECK_VALIDCODE("READ_SAMPLING_MESSAGE(pos_in)", validity, ret);

        READ_BLACKBOARD(disp_bb_disp_id, 0ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("READ_BLACKBOARD(bb_disp)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskD1)", ret);
    }
}

extern QUEUING_PORT_ID_TYPE  disp_ctrl_in_id;

void* taskD2_job(void)
{
    RETURN_CODE_TYPE    ret = NO_ERROR;
    MESSAGE_SIZE_TYPE   msg_len;
    integer             val = 0;

    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(disp_ctrl_in_id, 5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(ctrl_in)", ret);

        DISPLAY_BLACKBOARD(disp_bb_disp_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_disp)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskD2)", ret);
    }
}