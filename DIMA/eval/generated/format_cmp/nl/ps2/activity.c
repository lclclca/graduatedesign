#include "activity.h"
#include "globals.h"
#include "deployment.h"
#include "subprograms.h"
#include "gtypes.h"

#include <string.h>   /* memcpy */

/* ================================================================
 * task21_proc
 *   Period : 50 ms   Priority : 2
 *   Role   : Acquires data from pr2samplingin and publishes it
 *             on bb_acc3 so that task22 and task23 can consume it.
 * ================================================================ */
void task21_proc(void)
{
    RETURN_CODE_TYPE  ret      = NO_ERROR;
    T_INT_DATA        rx_data  = 0;

    for (;;) {
        /* --- 1. Receive inter-partition sampling message --- */
        ret = commandboard_receiveinput_spg(&rx_data);
        if (ret == NO_ERROR) {
            /* --- 2. Publish on blackboard bb_acc3 --- */
            DISPLAY_BLACKBOARD(
                g_bb_acc3_id,
                (MESSAGE_ADDR_TYPE)&rx_data,
                (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA),
                &ret
            );
            CHECK_CODE("task21: DISPLAY_BLACKBOARD bb_acc3", ret);

            commandboard_printinfos_spg("task21 rx", rx_data);
        }
        /* else: no valid data yet – wait for next activation */

        /* --- 3. Suspend until next period --- */
        PERIODIC_WAIT(&ret);
        CHECK_CODE("task21: PERIODIC_WAIT", ret);
    }
}

/* ================================================================
 * task22_proc
 *   Period : 50 ms   Priority : 3
 *   Role   : Reads bb_t3t2 / buf_t3tot2 (data from task23),
 *             processes it, writes bb_t2t3 / buf_t2tot3, and
 *             forwards a result to the queuing output port.
 * ================================================================ */
void task22_proc(void)
{
    RETURN_CODE_TYPE  ret           = NO_ERROR;
    T_INT_DATA        from_t3       = 0;
    T_INT_DATA        to_t3         = 0;
    T_INT_DATA        out_msg       = 0;
    MESSAGE_SIZE_TYPE received_len  = 0;
    VALIDITY_TYPE     validity      = INVALID_MESSAGE;

    for (;;) {
        /* --- 1. Read bb_t3t2 (data published by task23) --- */
        READ_BLACKBOARD(
            g_bb_t3t2_id,
            (MESSAGE_ADDR_TYPE)&from_t3,
            &received_len,
            &validity,
            &ret
        );
        /* Accept only when data is valid; otherwise keep previous */
        if (ret == NO_ERROR && validity == VALID_MESSAGE &&
            received_len == (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA)) {

            commandboard_printinfos_spg("task22 bb_t3t2 rx", from_t3);
        }

        /* --- 2. Read buf_t3tot2 (buffered data from task23) --- */
        RECEIVE_BUFFER(
            g_buf_t3tot2_id,
            (APEX_LONG_INTEGER)0,           /* NO_WAIT */
            (MESSAGE_ADDR_TYPE)&from_t3,
            &received_len,
            &ret
        );
        if (ret == NO_ERROR &&
            received_len == (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA)) {

            commandboard_printinfos_spg("task22 buf_t3tot2 rx", from_t3);
        }

        /* --- 3. Compute result (application logic placeholder) --- */
        to_t3  = from_t3 + 1;   /* example transform */
        out_msg = to_t3;

        /* --- 4. Publish to bb_t2t3 --- */
        DISPLAY_BLACKBOARD(
            g_bb_t2t3_id,
            (MESSAGE_ADDR_TYPE)&to_t3,
            (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA),
            &ret
        );
        CHECK_CODE("task22: DISPLAY_BLACKBOARD bb_t2t3", ret);

        /* --- 5. Send to buf_t2tot3 --- */
        SEND_BUFFER(
            g_buf_t2tot3_id,
            (MESSAGE_ADDR_TYPE)&to_t3,
            (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA),
            (APEX_LONG_INTEGER)0,           /* NO_WAIT */
            &ret
        );
        /* Buffer may be full; log but do not halt */
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("task22: SEND_BUFFER buf_t2tot3", ret);
        }

        /* --- 6. Send queuing output message --- */
        SEND_QUEUING_MESSAGE(
            g_pr2queueingout_id,
            (MESSAGE_ADDR_TYPE)&out_msg,
            (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA),
            (APEX_LONG_INTEGER)0,           /* NO_WAIT */
            &ret
        );
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("task22: SEND_QUEUING_MESSAGE", ret);
        }

        commandboard_printinfos_spg("task22 out", out_msg);

        /* --- 7. Suspend until next period --- */
        PERIODIC_WAIT(&ret);
        CHECK_CODE("task22: PERIODIC_WAIT", ret);
    }
}

/* ================================================================
 * task23_proc
 *   Period : 100 ms   Priority : 4
 *   Role   : Reads bb_t2t3 / buf_t2tot3, derives a command stored
 *             on buf_order, and sends feedback to task22 via
 *             bb_t3t2 / buf_t3tot2.
 * ================================================================ */
void task23_proc(void)
{
    RETURN_CODE_TYPE  ret           = NO_ERROR;
    T_INT_DATA        from_t2       = 0;
    T_INT_DATA        to_t2         = 0;
    T_INT_DATA        order         = 0;
    MESSAGE_SIZE_TYPE received_len  = 0;
    VALIDITY_TYPE     validity      = INVALID_MESSAGE;

    for (;;) {
        /* --- 1. Read bb_t2t3 (data published by task22) --- */
        READ_BLACKBOARD(
            g_bb_t2t3_id,
            (MESSAGE_ADDR_TYPE)&from_t2,
            &received_len,
            &validity,
            &ret
        );
        if (ret == NO_ERROR && validity == VALID_MESSAGE &&
            received_len == (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA)) {

            commandboard_printinfos_spg("task23 bb_t2t3 rx", from_t2);
        }

        /* --- 2. Read buf_t2tot3 (buffered data from task22) --- */
        RECEIVE_BUFFER(
            g_buf_t2tot3_id,
            (APEX_LONG_INTEGER)0,           /* NO_WAIT */
            (MESSAGE_ADDR_TYPE)&from_t2,
            &received_len,
            &ret
        );
        if (ret == NO_ERROR &&
            received_len == (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA)) {

            commandboard_printinfos_spg("task23 buf_t2tot3 rx", from_t2);
        }

        /* --- 3. Compute order (application logic placeholder) --- */
        order = from_t2 * 2;    /* example decision */
        to_t2 = from_t2 - 1;   /* feedback to task22 */

        /* --- 4. Store order in buf_order --- */
        SEND_BUFFER(
            g_buf_order_id,
            (MESSAGE_ADDR_TYPE)&order,
            (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA),
            (APEX_LONG_INTEGER)0,           /* NO_WAIT */
            &ret
        );
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("task23: SEND_BUFFER buf_order", ret);
        }
        commandboard_printinfos_spg("task23 order", order);

        /* --- 5. Publish feedback to bb_t3t2 --- */
        DISPLAY_BLACKBOARD(
            g_bb_t3t2_id,
            (MESSAGE_ADDR_TYPE)&to_t2,
            (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA),
            &ret
        );
        CHECK_CODE("task23: DISPLAY_BLACKBOARD bb_t3t2", ret);

        /* --- 6. Send feedback to buf_t3tot2 --- */
        SEND_BUFFER(
            g_buf_t3tot2_id,
            (MESSAGE_ADDR_TYPE)&to_t2,
            (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA),
            (APEX_LONG_INTEGER)0,           /* NO_WAIT */
            &ret
        );
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("task23: SEND_BUFFER buf_t3tot2", ret);
        }

        /* --- 7. Suspend until next period --- */
        PERIODIC_WAIT(&ret);
        CHECK_CODE("task23: PERIODIC_WAIT", ret);
    }
}