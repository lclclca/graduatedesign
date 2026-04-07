#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "globals.h"
#include "gtypes.h"

/*
 * Extern references to queuing port IDs created in appMain (main.c).
 * Sampling port array is empty for ps4 (NB_SAMPLINGS == 0).
 */
extern QUEUING_PORT_ID_TYPE order_in_id;
extern QUEUING_PORT_ID_TYPE queueingout_id;
extern QUEUING_PORT_ID_TYPE queueingin_id;

/* ------------------------------------------------------------------ */
/* task41 – period 25 ms, priority 2                                   */
/* ------------------------------------------------------------------ */
void *task41_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /*
         * TODO: implement task41 functional logic here.
         *
         * Example – receive from order_in (DESTINATION, max_nb=1):
         *
         *   MESSAGE_SIZE_TYPE len;
         *   char buf[MAX_MSG_SIZE];
         *   RECEIVE_QUEUING_MESSAGE(order_in_id,
         *                          0,          // IMMEDIATE
         *                          (MESSAGE_ADDR_TYPE)buf,
         *                          &len, &ret);
         *   CHECK_CODE("RECEIVE_QUEUING_MESSAGE order_in", ret);
         */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task41", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task42 – period 50 ms, priority 3                                   */
/* ------------------------------------------------------------------ */
void *task42_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /*
         * TODO: implement task42 functional logic here.
         *
         * Example – receive from queueingin (DESTINATION, max_nb=4):
         *
         *   MESSAGE_SIZE_TYPE len;
         *   char buf[MAX_MSG_SIZE];
         *   RECEIVE_QUEUING_MESSAGE(queueingin_id,
         *                          0,
         *                          (MESSAGE_ADDR_TYPE)buf,
         *                          &len, &ret);
         *   CHECK_CODE("RECEIVE_QUEUING_MESSAGE queueingin", ret);
         */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task42", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task43 – period 50 ms, priority 4                                   */
/* ------------------------------------------------------------------ */
void *task43_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /*
         * TODO: implement task43 functional logic here.
         *
         * Example – send to queueingout (SOURCE, max_nb=4):
         *
         *   char buf[] = "data";
         *   SEND_QUEUING_MESSAGE(queueingout_id,
         *                       (MESSAGE_ADDR_TYPE)buf,
         *                       sizeof(buf),
         *                       0,          // IMMEDIATE
         *                       &ret);
         *   CHECK_CODE("SEND_QUEUING_MESSAGE queueingout", ret);
         */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task43", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task44 – period 100 ms, priority 5                                  */
/* ------------------------------------------------------------------ */
void *task44_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* TODO: implement task44 functional logic here. */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task44", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task45 – period 200 ms, priority 6                                  */
/* ------------------------------------------------------------------ */
void *task45_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* TODO: implement task45 functional logic here. */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task45", ret);
    }

    return NULL;
}