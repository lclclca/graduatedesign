#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "deployment.h"
#include "globals.h"
#include "gtypes.h"
#include "subprograms.h"

/* Port IDs declared in main.c */
extern QUEUING_PORT_ID_TYPE arinc_queuing_ports[NB_QUEUEINGS];

/* ------------------------------------------------------------------ */
/* task41 — period 25 ms, priority 2
 * Reads from order_in, writes to queueingout
 * ------------------------------------------------------------------ */
void *task41_job(void *arg)
{
    RETURN_CODE_TYPE    ret;
    MESSAGE_SIZE_TYPE   len;
    integer             val;

    while (1)
    {
        /* Read from order_in (DESTINATION queuing port) */
        RECEIVE_QUEUING_MESSAGE(
            arinc_queuing_ports[ORDER_IN_PORT_IDX],
            0,                                      /* timeout: 0 = non-blocking */
            (MESSAGE_ADDR_TYPE)&val,
            &len,
            &ret);
        CHECK_CODE("task41 RECEIVE_QUEUING_MESSAGE order_in", ret);

        /* Process: copy received value as output */
        ps4_queueingout_val = val;

        /* Write to queueingout (SOURCE queuing port) */
        SEND_QUEUING_MESSAGE(
            arinc_queuing_ports[QUEUEINGOUT_PORT_IDX],
            (MESSAGE_ADDR_TYPE)&ps4_queueingout_val,
            sizeof(integer),
            0,                                      /* timeout: 0 = non-blocking */
            &ret);
        CHECK_CODE("task41 SEND_QUEUING_MESSAGE queueingout", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("task41 PERIODIC_WAIT", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task42 — period 50 ms, priority 3
 * Reads from queueingin, writes to queueingout
 * ------------------------------------------------------------------ */
void *task42_job(void *arg)
{
    RETURN_CODE_TYPE    ret;
    MESSAGE_SIZE_TYPE   len;
    integer             val;

    while (1)
    {
        /* Read from queueingin (DESTINATION queuing port) */
        RECEIVE_QUEUING_MESSAGE(
            arinc_queuing_ports[QUEUEINGIN_PORT_IDX],
            0,
            (MESSAGE_ADDR_TYPE)&val,
            &len,
            &ret);
        CHECK_CODE("task42 RECEIVE_QUEUING_MESSAGE queueingin", ret);

        /* Process: forward value to output */
        ps4_queueingout_val = val;

        /* Write to queueingout (SOURCE queuing port) */
        SEND_QUEUING_MESSAGE(
            arinc_queuing_ports[QUEUEINGOUT_PORT_IDX],
            (MESSAGE_ADDR_TYPE)&ps4_queueingout_val,
            sizeof(integer),
            0,
            &ret);
        CHECK_CODE("task42 SEND_QUEUING_MESSAGE queueingout", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("task42 PERIODIC_WAIT", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task43 — period 50 ms, priority 4
 * Reads from queueingin only
 * ------------------------------------------------------------------ */
void *task43_job(void *arg)
{
    RETURN_CODE_TYPE    ret;
    MESSAGE_SIZE_TYPE   len;
    integer             val;

    while (1)
    {
        /* Read from queueingin (DESTINATION queuing port) */
        RECEIVE_QUEUING_MESSAGE(
            arinc_queuing_ports[QUEUEINGIN_PORT_IDX],
            0,
            (MESSAGE_ADDR_TYPE)&val,
            &len,
            &ret);
        CHECK_CODE("task43 RECEIVE_QUEUING_MESSAGE queueingin", ret);

        /* Process: store received value */
        ps4_queueingin_val = val;

        PERIODIC_WAIT(&ret);
        CHECK_CODE("task43 PERIODIC_WAIT", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task44 — period 100 ms, priority 5
 * Writes to queueingout only
 * ------------------------------------------------------------------ */
void *task44_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* Write current output value to queueingout */
        SEND_QUEUING_MESSAGE(
            arinc_queuing_ports[QUEUEINGOUT_PORT_IDX],
            (MESSAGE_ADDR_TYPE)&ps4_queueingout_val,
            sizeof(integer),
            0,
            &ret);
        CHECK_CODE("task44 SEND_QUEUING_MESSAGE queueingout", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("task44 PERIODIC_WAIT", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* task45 — period 200 ms, priority 6
 * Reads from order_in only
 * ------------------------------------------------------------------ */
void *task45_job(void *arg)
{
    RETURN_CODE_TYPE    ret;
    MESSAGE_SIZE_TYPE   len;
    integer             val;

    while (1)
    {
        /* Read from order_in (DESTINATION queuing port) */
        RECEIVE_QUEUING_MESSAGE(
            arinc_queuing_ports[ORDER_IN_PORT_IDX],
            0,
            (MESSAGE_ADDR_TYPE)&val,
            &len,
            &ret);
        CHECK_CODE("task45 RECEIVE_QUEUING_MESSAGE order_in", ret);

        /* Process: store received order value */
        ps4_order_in_val = val;

        PERIODIC_WAIT(&ret);
        CHECK_CODE("task45 PERIODIC_WAIT", ret);
    }

    return NULL;
}