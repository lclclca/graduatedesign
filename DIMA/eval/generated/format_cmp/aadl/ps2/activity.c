#include <stdio.h>
#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "globals.h"
#include "gtypes.h"
#include "subprograms.h"

/*
 * Extern references to port/blackboard/buffer IDs created in main.c
 */

/* Sampling port */
extern SAMPLING_PORT_ID_TYPE  sampling_in_id;       /* pr2samplingin  (DESTINATION) */

/* Queuing port */
extern QUEUING_PORT_ID_TYPE   queuing_out_id;        /* pr2queueingout (SOURCE)      */

/* Blackboards */
extern BLACKBOARD_ID_TYPE     bb_task1_acc3_id;      /* task21 -> task22             */
extern BLACKBOARD_ID_TYPE     bb_task2_task3_id;     /* task22 <-> task23            */

/* Buffers */
extern BUFFER_ID_TYPE         buf_task2_order_id;    /* task22 -> task21 (order)     */
extern BUFFER_ID_TYPE         buf_task2_task3_id;    /* task22 <-> task23 (data)     */

/* ------------------------------------------------------------------ */
/* Tsk21 — Period 50 ms, Priority 2                                    */
/* Connections:                                                        */
/*   IN  sampling  : pr2samplingin   (READ_SAMPLING_MESSAGE)           */
/*   OUT blackboard: task1_acc3_out  (DISPLAY_BLACKBOARD)              */
/*   IN  buffer    : task1_order_in  (RECEIVE_BUFFER)                  */
/* ------------------------------------------------------------------ */
void *task21_job(void *arg)
{
    RETURN_CODE_TYPE        ret;
    integer                 sampling_data   = 0;
    integer                 order_data      = 0;
    integer                 acc3_out_data   = 0;
    MESSAGE_SIZE_TYPE       msg_size        = 0;
    VALIDITY_TYPE           validity;
    APEX_BYTE               recv_buf[sizeof(integer)];
    APEX_BYTE               send_buf[sizeof(integer)];

    while (1)
    {
        /* --- Read sampling input from partition port --- */
        ret = READ_SAMPLING_MESSAGE(sampling_in_id,
                                    (MESSAGE_ADDR_TYPE)&sampling_data,
                                    &msg_size,
                                    &validity);
        CHECK_CODE("READ_SAMPLING_MESSAGE (task21)", ret);

        /* Call receive subprogram */
        commandboard_receiveinput_spg();

        /* Call print subprogram */
        commandboard_printinfos_spg();

        /* --- Receive order from task22 via buffer (non-blocking, timeout=0) --- */
        ret = RECEIVE_BUFFER(buf_task2_order_id,
                             0,                          /* timeout: 0 = no wait */
                             (MESSAGE_ADDR_TYPE)recv_buf,
                             &msg_size,
                             &ret);
        /* Not checking strictly — buffer may be empty */
        if (ret == NO_ERROR && msg_size == sizeof(integer))
        {
            memcpy(&order_data, recv_buf, sizeof(integer));
            printf("[task21] received order: %d\n", order_data);
        }

        /* --- Write acc3 output to blackboard for task22 --- */
        acc3_out_data = sampling_data; /* example: pass through */
        memcpy(send_buf, &acc3_out_data, sizeof(integer));
        ret = DISPLAY_BLACKBOARD(bb_task1_acc3_id,
                                 (MESSAGE_ADDR_TYPE)send_buf,
                                 sizeof(integer));
        CHECK_CODE("DISPLAY_BLACKBOARD bb_task1_acc3 (task21)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT (task21)", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* Tsk22 — Period 50 ms, Priority 3                                    */
/* Connections:                                                        */
/*   IN  blackboard: task2_acc3_in       (READ_BLACKBOARD)             */
/*   OUT buffer    : task2_order_out     (SEND_BUFFER -> task21)       */
/*   OUT buffer    : task2_buffer_send   (SEND_BUFFER -> task23)       */
/*   IN  buffer    : task2_buffer_receive(RECEIVE_BUFFER <- task23)    */
/*   IN  blackboard: task2_blackboard_read (READ_BLACKBOARD <- task23) */
/*   OUT blackboard: task2_blackboard_write(DISPLAY_BLACKBOARD->task23)*/
/* ------------------------------------------------------------------ */
void *task22_job(void *arg)
{
    RETURN_CODE_TYPE    ret;
    integer             acc3_data       = 0;
    integer             order_data      = 0;
    integer             buf_send_data   = 0;
    integer             buf_recv_data   = 0;
    integer             bb_read_data    = 0;
    integer             bb_write_data   = 0;
    MESSAGE_SIZE_TYPE   msg_size        = 0;
    APEX_BYTE           data_buf[sizeof(integer)];

    while (1)
    {
        /* --- Read acc3 from blackboard (task21 -> task22) --- */
        ret = READ_BLACKBOARD(bb_task1_acc3_id,
                              (MESSAGE_ADDR_TYPE)data_buf,
                              &msg_size);
        CHECK_CODE("READ_BLACKBOARD bb_task1_acc3 (task22)", ret);
        if (ret == NO_ERROR && msg_size == sizeof(integer))
        {
            memcpy(&acc3_data, data_buf, sizeof(integer));
        }

        /* --- Send order to task21 via buffer --- */
        order_data = acc3_data + 1; /* example processing */
        memcpy(data_buf, &order_data, sizeof(integer));
        ret = SEND_BUFFER(buf_task2_order_id,
                          (MESSAGE_ADDR_TYPE)data_buf,
                          sizeof(integer),
                          0);        /* timeout 0 */
        CHECK_CODE("SEND_BUFFER order (task22->task21)", ret);

        /* --- Send data to task23 via buffer --- */
        buf_send_data = acc3_data;
        memcpy(data_buf, &buf_send_data, sizeof(integer));
        ret = SEND_BUFFER(buf_task2_task3_id,
                          (MESSAGE_ADDR_TYPE)data_buf,
                          sizeof(integer),
                          0);
        CHECK_CODE("SEND_BUFFER data (task22->task23)", ret);

        /* --- Receive data from task23 via buffer --- */
        ret = RECEIVE_BUFFER(buf_task2_task3_id,
                             0,
                             (MESSAGE_ADDR_TYPE)data_buf,
                             &msg_size,
                             &ret);
        if (ret == NO_ERROR && msg_size == sizeof(integer))
        {
            memcpy(&buf_recv_data, data_buf, sizeof(integer));
            printf("[task22] received from task23 buffer: %d\n", buf_recv_data);
        }

        /* --- Read blackboard from task23 --- */
        ret = READ_BLACKBOARD(bb_task2_task3_id,
                              (MESSAGE_ADDR_TYPE)data_buf,
                              &msg_size);
        CHECK_CODE("READ_BLACKBOARD bb_task2_task3 (task22)", ret);
        if (ret == NO_ERROR && msg_size == sizeof(integer))
        {
            memcpy(&bb_read_data, data_buf, sizeof(integer));
        }

        /* --- Write blackboard to task23 --- */
        bb_write_data = acc3_data * 2; /* example */
        memcpy(data_buf, &bb_write_data, sizeof(integer));
        ret = DISPLAY_BLACKBOARD(bb_task2_task3_id,
                                 (MESSAGE_ADDR_TYPE)data_buf,
                                 sizeof(integer));
        CHECK_CODE("DISPLAY_BLACKBOARD bb_task2_task3 (task22)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT (task22)", ret);
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* Tsk23 — Period 100 ms, Priority 4                                   */
/* Connections:                                                        */
/*   OUT buffer    : task3_buffer_send    (SEND_BUFFER -> task22)      */
/*   IN  buffer    : task3_buffer_receive (RECEIVE_BUFFER <- task22)   */
/*   IN  blackboard: task3_blackboard_read (READ_BLACKBOARD <- task22) */
/*   OUT blackboard: task3_blackboard_write(DISPLAY_BLACKBOARD->task22)*/
/*   OUT queuing   : task3Queuing          (SEND_QUEUING_MESSAGE)      */
/* ------------------------------------------------------------------ */
void *task23_job(void *arg)
{
    RETURN_CODE_TYPE    ret;
    integer             buf_recv_data   = 0;
    integer             buf_send_data   = 0;
    integer             bb_read_data    = 0;
    integer             bb_write_data   = 0;
    integer             queuing_data    = 0;
    MESSAGE_SIZE_TYPE   msg_size        = 0;
    APEX_BYTE           data_buf[sizeof(integer)];

    while (1)
    {
        /* Call receive subprogram */
        commandboard_receiveinput_spg();

        /* --- Receive data from task22 via buffer --- */
        ret = RECEIVE_BUFFER(buf_task2_task3_id,
                             0,
                             (MESSAGE_ADDR_TYPE)data_buf,
                             &msg_size,
                             &ret);
        if (ret == NO_ERROR && msg_size == sizeof(integer))
        {
            memcpy(&buf_recv_data, data_buf, sizeof(integer));
            printf("[task23] received from task22 buffer: %d\n", buf_recv_data);
        }

        /* --- Send data back to task22 via buffer --- */
        buf_send_data = buf_recv_data + 10; /* example processing */
        memcpy(data_buf, &buf_send_data, sizeof(integer));
        ret = SEND_BUFFER(buf_task2_task3_id,
                          (MESSAGE_ADDR_TYPE)data_buf,
                          sizeof(integer),
                          0);
        CHECK_CODE("SEND_BUFFER data (task23->task22)", ret);

        /* --- Read blackboard from task22 --- */
        ret = READ_BLACKBOARD(bb_task2_task3_id,
                              (MESSAGE_ADDR_TYPE)data_buf,
                              &msg_size);
        CHECK_CODE("READ_BLACKBOARD bb_task2_task3 (task23)", ret);
        if (ret == NO_ERROR && msg_size == sizeof(integer))
        {
            memcpy(&bb_read_data, data_buf, sizeof(integer));
        }

        /* --- Write blackboard to task22 --- */
        bb_write_data = buf_recv_data; /* example */
        memcpy(data_buf, &bb_write_data, sizeof(integer));
        ret = DISPLAY_BLACKBOARD(bb_task2_task3_id,
                                 (MESSAGE_ADDR_TYPE)data_buf,
                                 sizeof(integer));
        CHECK_CODE("DISPLAY_BLACKBOARD bb_task2_task3 (task23)", ret);

        /* --- Send queuing message to partition output port --- */
        queuing_data = buf_recv_data;
        memcpy(data_buf, &queuing_data, sizeof(integer));
        ret = SEND_QUEUING_MESSAGE(queuing_out_id,
                                   (MESSAGE_ADDR_TYPE)data_buf,
                                   sizeof(integer),
                                   5);    /* timeout: 5 ns as per spec */
        CHECK_CODE("SEND_QUEUING_MESSAGE (task23)", ret);

        /* Call print subprogram */
        commandboard_printinfos_spg();

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT (task23)", ret);
    }

    return NULL;
}