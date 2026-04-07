#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "deployment.h"
#include "globals.h"
#include "gtypes.h"

/* Port IDs declared in main.c */
extern QUEUING_PORT_ID_TYPE ps5_queuing_port_ids[NB_QUEUEINGS];

/* ------------------------------------------------------------------ */
/* task51 — period 50 ms, priority 1                                   */
/* Reads from queuing port: order_in                                   */
/* ------------------------------------------------------------------ */
void *task51_job(void *arg)
{
    RETURN_CODE_TYPE    ret;
    MESSAGE_SIZE_TYPE   len;
    integer             val;

    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(
            ps5_queuing_port_ids[ORDER_IN_IDX],
            0,                                  /* timeout = 0: non-blocking */
            (MESSAGE_ADDR_TYPE)&val,
            &len,
            &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE order_in", ret);

        if (ret == NO_ERROR)
        {
            ps5_order_in_val = val;
        }

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task51", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task52 — period 50 ms, priority 2                                   */
/* Writes to queuing port: queueingout                                 */
/* ------------------------------------------------------------------ */
void *task52_job(void *arg)
{
    RETURN_CODE_TYPE  ret;
    integer           val;

    while (1)
    {
        val = ps5_queueingout_val;

        SEND_QUEUING_MESSAGE(
            ps5_queuing_port_ids[QUEUEINGOUT_IDX],
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            0,                  /* timeout = 0: non-blocking */
            &ret);
        CHECK_CODE("SEND_QUEUING_MESSAGE queueingout", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task52", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task53 — period 200 ms, priority 3                                  */
/* No port communication; performs internal processing                 */
/* ------------------------------------------------------------------ */
void *task53_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* Internal processing placeholder */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task53", ret);
    }

    return NULL;
}