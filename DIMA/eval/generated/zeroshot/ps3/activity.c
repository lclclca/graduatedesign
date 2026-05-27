#include <stdio.h>
#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "globals.h"
#include "gtypes.h"
#include "deployment.h"

/* ------------------------------------------------------------------ */
/* External port IDs declared in main.c                               */
/* ------------------------------------------------------------------ */
extern QUEUING_PORT_ID_TYPE order_in_id;
extern QUEUING_PORT_ID_TYPE queueingin_id;

/* ================================================================== */
/* task31 — period: 25 ms (25,000,000 ns), priority: 2               */
/* ================================================================== */
void *task31_job(void *arg)
{
    RETURN_CODE_TYPE       ret;
    MESSAGE_ADDR_TYPE      msg_addr;
    MESSAGE_SIZE_TYPE      msg_len;

    (void)arg;

    while (1)
    {
        /* --- Receive from queuing port: order_in --- */
        RECEIVE_QUEUING_MESSAGE(order_in_id,
                                (APEX_INTEGER)0,   /* no blocking timeout */
                                &msg_addr,
                                &msg_len,
                                &ret);
        if (ret == NO_ERROR) {
            printf("[PS3][task31] Received %ld bytes from order_in\n",
                   (long)msg_len);
        } else if (ret != NO_ACTION) {
            /* NO_ACTION means empty queue — not a real error in polling */
            CHECK_CODE("task31 RECEIVE_QUEUING_MESSAGE(order_in)", ret);
        }

        /* --- Receive from queuing port: queueingin --- */
        RECEIVE_QUEUING_MESSAGE(queueingin_id,
                                (APEX_INTEGER)0,
                                &msg_addr,
                                &msg_len,
                                &ret);
        if (ret == NO_ERROR) {
            printf("[PS3][task31] Received %ld bytes from queueingin\n",
                   (long)msg_len);
        } else if (ret != NO_ACTION) {
            CHECK_CODE("task31 RECEIVE_QUEUING_MESSAGE(queueingin)", ret);
        }

        PERIODIC_WAIT(&ret);
        CHECK_CODE("task31 PERIODIC_WAIT", ret);
    }

    return NULL;
}

/* ================================================================== */
/* task32 — period: 50 ms (50,000,000 ns), priority: 3               */
/* ================================================================== */
void *task32_job(void *arg)
{
    RETURN_CODE_TYPE  ret;

    (void)arg;

    while (1)
    {
        /* TODO: Implement task32 functional logic here */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("task32 PERIODIC_WAIT", ret);
    }

    return NULL;
}

/* ================================================================== */
/* task33 — period: 50 ms (50,000,000 ns), priority: 4               */
/* ================================================================== */
void *task33_job(void *arg)
{
    RETURN_CODE_TYPE  ret;

    (void)arg;

    while (1)
    {
        /* TODO: Implement task33 functional logic here */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("task33 PERIODIC_WAIT", ret);
    }

    return NULL;
}