#include <stdio.h>
#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "globals.h"
#include "subprograms.h"

/* ------------------------------------------------------------------ */
/* Extern references to port / IPC object IDs created in main.c       */
/* ------------------------------------------------------------------ */
extern SAMPLING_PORT_ID_TYPE  pr2samplingin_id;
extern QUEUING_PORT_ID_TYPE   pr2queueingout_id;

extern BLACKBOARD_ID_TYPE     bb_acc3_id;
extern BLACKBOARD_ID_TYPE     bb_t2t3_id;
extern BLACKBOARD_ID_TYPE     bb_t3t2_id;

extern BUFFER_ID_TYPE         buf_order_id;
extern BUFFER_ID_TYPE         buf_t2tot3_id;
extern BUFFER_ID_TYPE         buf_t3tot2_id;

/* ================================================================== */
/* task21_job  –  period 50 ms, priority 2                            */
/* ================================================================== */
void *task21_job(void *arg)
{
    RETURN_CODE_TYPE        ret;
    MESSAGE_ADDR_TYPE       msg_buf;
    MESSAGE_SIZE_TYPE       msg_len;
    VALIDITY_TYPE           validity;

    (void)arg;

    while (1)
    {
        /* Read from sampling port pr2samplingin */
        ret = READ_SAMPLING_MESSAGE(pr2samplingin_id,
                                    (MESSAGE_ADDR_TYPE)msg_buf,
                                    &msg_len,
                                    &validity);
        CHECK_CODE("READ_SAMPLING_MESSAGE(pr2samplingin)", ret);

        /* Application logic placeholder */
        commandboard_receiveinput_spg();

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task21)", ret);
    }

    return NULL;
}

/* ================================================================== */
/* task22_job  –  period 50 ms, priority 3                            */
/* ================================================================== */
void *task22_job(void *arg)
{
    RETURN_CODE_TYPE    ret;
    /* Example: write a message to the queuing port */
    static char         payload[] = "task22_data";
    MESSAGE_SIZE_TYPE   payload_len = (MESSAGE_SIZE_TYPE)sizeof(payload);

    (void)arg;

    while (1)
    {
        /* Send via queuing port pr2queueingout */
        ret = SEND_QUEUING_MESSAGE(pr2queueingout_id,
                                   (MESSAGE_ADDR_TYPE)payload,
                                   payload_len,
                                   0,          /* timeout: no wait */
                                   &ret);
        CHECK_CODE("SEND_QUEUING_MESSAGE(pr2queueingout)", ret);

        /* Application logic placeholder */
        commandboard_printinfos_spg();

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task22)", ret);
    }

    return NULL;
}

/* ================================================================== */
/* task23_job  –  period 100 ms, priority 4                           */
/* ================================================================== */
void *task23_job(void *arg)
{
    RETURN_CODE_TYPE    ret;

    (void)arg;

    while (1)
    {
        /*
         * TODO: Add task23 application logic here.
         * Example IPC usage:
         *   READ_BLACKBOARD  / DISPLAY_BLACKBOARD  via bb_acc3_id …
         *   READ_BUFFER      / SEND_BUFFER         via buf_order_id …
         */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task23)", ret);
    }

    return NULL;
}