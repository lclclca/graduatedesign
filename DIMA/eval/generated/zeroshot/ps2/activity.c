#include "activity.h"
#include "globals.h"
#include "subprograms.h"
#include "gtypes.h"

#include <stdio.h>
#include <string.h>
#include <os/pos/apex/apexLib.h>

/* ---------------------------------------------------------------
 * External references to IPC object IDs created in main.c
 * --------------------------------------------------------------- */

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

/* ---------------------------------------------------------------
 * task21_job  —  period 50 ms, priority 2
 * --------------------------------------------------------------- */
void *task21_job(void *arg)
{
    RETURN_CODE_TYPE        ret;
    MESSAGE_ADDR_TYPE       msg_buf;
    MESSAGE_SIZE_TYPE       msg_len;
    VALIDITY_TYPE           validity;

    /* Example static receive buffer — adapt size to real message type */
    static unsigned char    rx_buf[256];

    msg_buf = (MESSAGE_ADDR_TYPE)rx_buf;

    while (1)
    {
        /* Read from sampling port pr2samplingin */
        READ_SAMPLING_MESSAGE(pr2samplingin_id,
                              msg_buf,
                              &msg_len,
                              &validity,
                              &ret);
        CHECK_CODE("READ_SAMPLING_MESSAGE(pr2samplingin)", ret);

        /* TODO: process received sampling message */

        /* Interact with blackboard bb_acc3 (read example) */
        /* READ_BLACKBOARD(bb_acc3_id, msg_buf, &msg_len, &ret); */
        /* CHECK_CODE("READ_BLACKBOARD(bb_acc3)", ret); */

        /* Interact with buffer buf_order (receive example) */
        /* RECEIVE_BUFFER(buf_order_id, INFINITE_TIME_VALUE,
                          msg_buf, &msg_len, &ret); */
        /* CHECK_CODE("RECEIVE_BUFFER(buf_order)", ret); */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task21)", ret);
    }

    return NULL;
}

/* ---------------------------------------------------------------
 * task22_job  —  period 50 ms, priority 3
 * --------------------------------------------------------------- */
void *task22_job(void *arg)
{
    RETURN_CODE_TYPE        ret;
    MESSAGE_ADDR_TYPE       msg_buf;
    MESSAGE_SIZE_TYPE       msg_len;

    static unsigned char    tx_buf[256];

    msg_buf  = (MESSAGE_ADDR_TYPE)tx_buf;
    msg_len  = sizeof(tx_buf);

    while (1)
    {
        /* TODO: prepare outgoing queuing message payload in tx_buf */
        commandboard_receiveinput_spg();

        /* Send queuing message on pr2queueingout */
        SEND_QUEUING_MESSAGE(pr2queueingout_id,
                             msg_buf,
                             msg_len,
                             0,          /* timeout: no wait */
                             &ret);
        CHECK_CODE("SEND_QUEUING_MESSAGE(pr2queueingout)", ret);

        /* Interact with blackboard bb_t2t3 (display example) */
        /* DISPLAY_BLACKBOARD(bb_t2t3_id, msg_buf, msg_len, &ret); */
        /* CHECK_CODE("DISPLAY_BLACKBOARD(bb_t2t3)", ret); */

        /* Interact with buffer buf_t2tot3 (send example) */
        /* SEND_BUFFER(buf_t2tot3_id, msg_buf, msg_len, 0, &ret); */
        /* CHECK_CODE("SEND_BUFFER(buf_t2tot3)", ret); */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task22)", ret);
    }

    return NULL;
}

/* ---------------------------------------------------------------
 * task23_job  —  period 100 ms, priority 4
 * --------------------------------------------------------------- */
void *task23_job(void *arg)
{
    RETURN_CODE_TYPE        ret;
    MESSAGE_ADDR_TYPE       msg_buf;
    MESSAGE_SIZE_TYPE       msg_len;

    static unsigned char    info_buf[256];

    msg_buf = (MESSAGE_ADDR_TYPE)info_buf;
    msg_len = sizeof(info_buf);

    while (1)
    {
        /* TODO: implement task23 functional behaviour */
        commandboard_printinfos_spg();

        /* Interact with blackboard bb_t3t2 (display example) */
        /* DISPLAY_BLACKBOARD(bb_t3t2_id, msg_buf, msg_len, &ret); */
        /* CHECK_CODE("DISPLAY_BLACKBOARD(bb_t3t2)", ret); */

        /* Interact with buffer buf_t3tot2 (send example) */
        /* SEND_BUFFER(buf_t3tot2_id, msg_buf, msg_len, 0, &ret); */
        /* CHECK_CODE("SEND_BUFFER(buf_t3tot2)", ret); */

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task23)", ret);
    }

    return NULL;
}