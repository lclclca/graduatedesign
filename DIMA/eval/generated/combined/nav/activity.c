#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

/* ---- Sampling ports ---- */
extern SAMPLING_PORT_ID_TYPE nav_pos_out_id;
extern SAMPLING_PORT_ID_TYPE nav_status_out_id;

/* ---- Queuing ports ---- */
extern QUEUING_PORT_ID_TYPE nav_cmd_out_id;

/* ---- Blackboards ---- */
extern BLACKBOARD_ID_TYPE nav_bb_bb1_id;
extern BLACKBOARD_ID_TYPE nav_bb_bb2_id;

/* ---- Buffers ---- */
extern BUFFER_ID_TYPE nav_buf_buf1_id;

/*
 * taskN1 (period=25ms, priority=2)
 * Writes navigation position data to the pos_out sampling port.
 */
void* taskN1_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    integer val = 0;
    while (1)
    {
        val++;
        WRITE_SAMPLING_MESSAGE(nav_pos_out_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);
        CHECK_CODE("WRITE_SAMPLING_MESSAGE(pos_out)", ret);
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskN1)", ret);
    }
}

/*
 * taskN2 (period=50ms, priority=3)
 * Writes status data to the status_out sampling port and
 * sends a command message via the cmd_out queuing port.
 */
void* taskN2_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    integer val = 0;
    while (1)
    {
        val++;
        WRITE_SAMPLING_MESSAGE(nav_status_out_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);
        CHECK_CODE("WRITE_SAMPLING_MESSAGE(status_out)", ret);

        SEND_QUEUING_MESSAGE(nav_cmd_out_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), 0ll, &ret);
        CHECK_CODE("SEND_QUEUING_MESSAGE(cmd_out)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskN2)", ret);
    }
}

/*
 * taskN3 (period=100ms, priority=4)
 * Writes data to blackboard bb_bb1 and reads from blackboard bb_bb2.
 */
void* taskN3_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        val++;
        DISPLAY_BLACKBOARD(nav_bb_bb1_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_bb1)", ret);

        READ_BLACKBOARD(nav_bb_bb2_id, 0ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("READ_BLACKBOARD(bb_bb2)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskN3)", ret);
    }
}

/*
 * taskN4 (period=50ms, priority=5)
 * Writes data to buffer buf_buf1 and reads from blackboard bb_bb1.
 */
void* taskN4_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        val++;
        WRITE_BUFFER(nav_buf_buf1_id,
            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), 5000000ll, &ret);
        CHECK_CODE("WRITE_BUFFER(buf_buf1)", ret);

        READ_BLACKBOARD(nav_bb_bb1_id, 0ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("READ_BLACKBOARD(bb_bb1)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(taskN4)", ret);
    }
}