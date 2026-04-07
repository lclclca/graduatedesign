#include "activity.h"
#include "globals.h"
#include "subprograms.h"
#include "gtypes.h"

#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <string.h>

/*
 * External references to port/blackboard/buffer IDs created in main.c
 */

/* Sampling ports */
extern SAMPLING_PORT_ID_TYPE pr1samplingout_id;

/* Queuing ports */
extern QUEUING_PORT_ID_TYPE  pr1queueingin_id;

/* Blackboards */
extern BLACKBOARD_ID_TYPE    bb_acc1_id;
extern BLACKBOARD_ID_TYPE    bb_acc2_id;
extern BLACKBOARD_ID_TYPE    bb_sem_id;
extern BLACKBOARD_ID_TYPE    bb_t3t4_id;
extern BLACKBOARD_ID_TYPE    bb_t4t3_id;

/* Buffers */
extern BUFFER_ID_TYPE        buf_sensor_id;
extern BUFFER_ID_TYPE        buf_t3tot4_id;
extern BUFFER_ID_TYPE        buf_t4tot3_id;

/* -------------------------------------------------------------------------
 * task11_job
 * Period : 25 ms  (25,000,000 ns)
 * Priority: 2
 * ------------------------------------------------------------------------- */
void *task11_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* TODO: implement task11 functional logic */

        /* Example: call a subprogram */
        commandboard_receiveinput_spg();

        /* Example: write to sampling port */
        {
            MESSAGE_SIZE_TYPE msg_size = 64;
            unsigned char     msg[64]  = {0};

            /* TODO: fill msg with actual data */
            WRITE_SAMPLING_MESSAGE(pr1samplingout_id,
                                   (MESSAGE_ADDR_TYPE)msg,
                                   msg_size,
                                   &ret);
            CHECK_CODE("WRITE_SAMPLING_MESSAGE(pr1samplingout)", ret);
        }

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task11)", ret);
    }

    return NULL;
}

/* -------------------------------------------------------------------------
 * task12_job
 * Period : 50 ms  (50,000,000 ns)
 * Priority: 3
 * ------------------------------------------------------------------------- */
void *task12_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* TODO: implement task12 functional logic */

        commandboard_printinfos_spg();

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task12)", ret);
    }

    return NULL;
}

/* -------------------------------------------------------------------------
 * task13_job
 * Period : 50 ms  (50,000,000 ns)
 * Priority: 4
 * ------------------------------------------------------------------------- */
void *task13_job(void *arg)
{
    RETURN_CODE_TYPE         ret;
    MESSAGE_ADDR_TYPE        msg_addr;
    MESSAGE_SIZE_TYPE        msg_len;
    QUEUING_PORT_STATUS_TYPE qstatus;

    while (1)
    {
        /* TODO: implement task13 functional logic */

        /* Example: receive from queuing port */
        {
            static unsigned char recv_buf[256];

            RECEIVE_QUEUING_MESSAGE(pr1queueingin_id,
                                    INFINITE_TIME_VALUE,
                                    (MESSAGE_ADDR_TYPE)recv_buf,
                                    &msg_len,
                                    &ret);
            CHECK_CODE("RECEIVE_QUEUING_MESSAGE(pr1queueingin)", ret);

            /* TODO: process received message in recv_buf */
        }

        calculate_spg();

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task13)", ret);
    }

    return NULL;
}

/* -------------------------------------------------------------------------
 * task14_job
 * Period : 50 ms  (50,000,000 ns)
 * Priority: 5
 * ------------------------------------------------------------------------- */
void *task14_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* TODO: implement task14 functional logic */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task14)", ret);
    }

    return NULL;
}