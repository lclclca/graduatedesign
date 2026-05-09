#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

extern SAMPLING_PORT_ID_TYPE nav_pos_out_id;
extern SAMPLING_PORT_ID_TYPE nav_status_out_id;
extern QUEUING_PORT_ID_TYPE  nav_cmd_out_id;
extern BLACKBOARD_ID_TYPE    nav_bb_bb1_id;
extern BLACKBOARD_ID_TYPE    nav_bb_bb2_id;
extern BUFFER_ID_TYPE        nav_buf_buf1_id;

void* taskN1_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    integer val = 0;

    while (1)
    {
        WRITE_SAMPLING_MESSAGE(nav_pos_out_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            &ret);
        CHECK_CODE("WRITE_SAMPLING_MESSAGE(pos_out)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskN1)", ret);
    }
}

void* taskN2_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    integer val = 0;

    while (1)
    {
        WRITE_SAMPLING_MESSAGE(nav_status_out_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            &ret);
        CHECK_CODE("WRITE_SAMPLING_MESSAGE(status_out)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskN2)", ret);
    }
}

void* taskN3_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    integer val = 0;

    while (1)
    {
        SEND_QUEUING_MESSAGE(nav_cmd_out_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            0ll,
            &ret);
        CHECK_CODE("SEND_QUEUING_MESSAGE(cmd_out)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskN3)", ret);
    }
}

void* taskN4_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    integer val = 0;
    VALIDITY_TYPE validity = INVALID;

    while (1)
    {
        READ_BLACKBOARD(nav_bb_bb1_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            &validity,
            &ret);
        CHECK_VALIDCODE("READ_BLACKBOARD(bb_bb1)", validity, ret);

        READ_BLACKBOARD(nav_bb_bb2_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            &validity,
            &ret);
        CHECK_VALIDCODE("READ_BLACKBOARD(bb_bb2)", validity, ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskN4)", ret);
    }
}