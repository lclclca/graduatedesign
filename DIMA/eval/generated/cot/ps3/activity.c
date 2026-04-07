#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "deployment.h"
#include "globals.h"
#include "gtypes.h"

/* ------------------------------------------------------------------ */
/* task31_job                                                          */
/* Period: 25 ms | Priority: 2                                        */
/* Reads from queuing port: order_in                                  */
/* ------------------------------------------------------------------ */
void *task31_job(void *arg)
{
    RETURN_CODE_TYPE      ret;
    MESSAGE_SIZE_TYPE     len;
    integer               val;

    while (1)
    {
        /* Receive from order_in queuing port (non-blocking, timeout=0) */
        RECEIVE_QUEUING_MESSAGE(
            ps3_queuing_port_ids[ORDER_IN_PORT_IDX],
            0,
            (MESSAGE_ADDR_TYPE)&val,
            &len,
            &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE order_in", ret);

        if (ret == NO_ERROR)
        {
            ps3_order_in_val = val;
        }

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task31", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task32_job                                                          */
/* Period: 50 ms | Priority: 3                                        */
/* Reads from queuing port: queueingin                                */
/* ------------------------------------------------------------------ */
void *task32_job(void *arg)
{
    RETURN_CODE_TYPE      ret;
    MESSAGE_SIZE_TYPE     len;
    integer               val;

    while (1)
    {
        /* Receive from queueingin queuing port (non-blocking, timeout=0) */
        RECEIVE_QUEUING_MESSAGE(
            ps3_queuing_port_ids[QUEUEINGIN_PORT_IDX],
            0,
            (MESSAGE_ADDR_TYPE)&val,
            &len,
            &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE queueingin", ret);

        if (ret == NO_ERROR)
        {
            ps3_queueingin_val = val;
        }

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task32", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task33_job                                                          */
/* Period: 50 ms | Priority: 4                                        */
/* No port operations assigned                                        */
/* ------------------------------------------------------------------ */
void *task33_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* No communication resources assigned to task33 */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task33", ret);
    }

    return NULL;
}