#include "activity.h"
#include "globals.h"
#include "subprograms.h"

#include <stdio.h>
#include <string.h>
#include <os/pos/apex/apexLib.h>

/* -------------------------------------------------------
 * Extern references to IPC port and shared resource IDs
 * declared as globals in main.c
 * ------------------------------------------------------- */

/* Sampling ports */
extern SAMPLING_PORT_ID_TYPE  pr2samplingin_id;

/* Queuing ports */
extern QUEUING_PORT_ID_TYPE   pr2queueingout_id;

/* Blackboards */
extern BLACKBOARD_ID_TYPE     bb_acc3_id;
extern BLACKBOARD_ID_TYPE     bb_t2t3_id;
extern BLACKBOARD_ID_TYPE     bb_t3t2_id;

/* Buffers */
extern BUFFER_ID_TYPE         buf_order_id;
extern BUFFER_ID_TYPE         buf_t2tot3_id;
extern BUFFER_ID_TYPE         buf_t3tot2_id;

/* -------------------------------------------------------
 * task21_job
 *   Period   : 50 ms  (50,000,000 ns)
 *   Priority : 2
 * ------------------------------------------------------- */
void *task21_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* ---- User logic for task21 ---- */

        /* Example: receive input via the auxiliary subprogram */
        commandboard_receiveinput_spg();

        /* Example: read from sampling input port pr2samplingin */
        {
            MESSAGE_ADDR_TYPE  msg_buf[256];
            MESSAGE_SIZE_TYPE  msg_len  = 0;
            VALIDITY_TYPE      validity = INVALID;

            READ_SAMPLING_MESSAGE(pr2samplingin_id,
                                  (MESSAGE_ADDR_TYPE)msg_buf,
                                  &msg_len,
                                  &validity,
                                  &ret);
            CHECK_CODE("READ_SAMPLING_MESSAGE(pr2samplingin)", ret);
        }

        /* TODO: add task21-specific processing here */

        /* ---- End of periodic work — suspend until next period ---- */
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task21)", ret);
    }

    return NULL;
}

/* -------------------------------------------------------
 * task22_job
 *   Period   : 50 ms  (50,000,000 ns)
 *   Priority : 3
 * ------------------------------------------------------- */
void *task22_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* ---- User logic for task22 ---- */

        /* Example: send a message via queuing output port pr2queueingout */
        {
            static unsigned char out_msg[64] = "task22_data";
            MESSAGE_SIZE_TYPE    out_len      = sizeof("task22_data");

            SEND_QUEUING_MESSAGE(pr2queueingout_id,
                                 (MESSAGE_ADDR_TYPE)out_msg,
                                 out_len,
                                 0,          /* timeout: no wait */
                                 &ret);
            CHECK_CODE("SEND_QUEUING_MESSAGE(pr2queueingout)", ret);
        }

        /* TODO: add task22-specific processing here */

        /* ---- End of periodic work — suspend until next period ---- */
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task22)", ret);
    }

    return NULL;
}

/* -------------------------------------------------------
 * task23_job
 *   Period   : 100 ms (100,000,000 ns)
 *   Priority : 4
 * ------------------------------------------------------- */
void *task23_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* ---- User logic for task23 ---- */

        /* Example: print partition infos via auxiliary subprogram */
        commandboard_printinfos_spg();

        /* TODO: add task23-specific processing here */

        /* ---- End of periodic work — suspend until next period ---- */
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task23)", ret);
    }

    return NULL;
}