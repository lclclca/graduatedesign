/* ============================================================
 * activity.c
 * Partition P2 – Task (Process) Entry-Point Implementations
 * ARINC 653 / ACoreOS653
 *
 * Implements the three periodic tasks defined in the AADL model:
 *   Tsk21 (50 ms, priority 2)
 *   Tsk22 (50 ms, priority 3)
 *   Tsk23 (100 ms, priority 4)
 * ============================================================ */

#include "activity.h"
#include "globals.h"
#include "subprograms.h"
#include "gtypes.h"
#include "apex/apex.h"

/* ============================================================
 * task21_entry
 *
 * Periodic task – 50 ms
 * Data-flow (from AADL connections):
 *   RECEIVE  sampling port  g_sampling_port_in  (task21sampling)
 *   READ     buffer         g_buf_22_21         (task1_order_in)
 *   WRITE    blackboard     g_bb_21_22          (task1_acc3_out)
 * Subprogram calls:
 *   commandboard_receiveinput_spg
 *   commandboard_printinfos_spg
 * ============================================================ */
void task21_entry(void)
{
    RETURN_CODE_TYPE    rc;
    APP_INTEGER_TYPE    sampling_data  = 0;
    APP_INTEGER_TYPE    order_data     = 0;
    APP_INTEGER_TYPE    acc3_out       = 0;
    MESSAGE_SIZE_TYPE   recv_len       = 0;

    while (1) {
        /* ---- 1. Receive from inbound sampling port ---------- */
        commandboard_receiveinput_spg(&sampling_data, &rc);
        CHECK_CODE("Tsk21 receiveinput_spg", rc);

        /* ---- 2. Read order feedback buffer from task22 ------ */
        RECEIVE_BUFFER(g_buf_22_21,
                       ARINC_ZERO_TIMEOUT,
                       (MESSAGE_ADDR_TYPE)&order_data,
                       &recv_len,
                       &rc);
        /* TIMED_OUT is acceptable: no new order this cycle     */
        if (rc != NO_ERROR && rc != TIMED_OUT) {
            CHECK_CODE("Tsk21 RECEIVE_BUFFER buf_22_21", rc);
        }

        /* ---- 3. Compute accelerometer accumulator (stub) ---- */
        acc3_out = sampling_data + order_data;   /* application logic placeholder */

        /* ---- 4. Write result to blackboard for task22 ------- */
        DISPLAY_BLACKBOARD(g_bb_21_22,
                           (MESSAGE_ADDR_TYPE)&acc3_out,
                           (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
                           &rc);
        CHECK_CODE("Tsk21 DISPLAY_BLACKBOARD bb_21_22", rc);

        /* ---- 5. Print diagnostic info ----------------------- */
        commandboard_printinfos_spg(&acc3_out, &rc);
        CHECK_CODE("Tsk21 printinfos_spg", rc);

        /* ---- 6. Wait for next period ------------------------ */
        PERIODIC_WAIT(&rc);
        CHECK_CODE("Tsk21 PERIODIC_WAIT", rc);
    }
}

/* ============================================================
 * task22_entry
 *
 * Periodic task – 50 ms
 * Data-flow (from AADL connections):
 *   READ  blackboard  g_bb_21_22  (task2_acc3_in)
 *   READ  buffer      g_buf_23_22 (task2_buffer_receive)
 *   READ  blackboard  g_bb_23_22  (task2_blackboard_read)
 *   WRITE buffer      g_buf_22_21 (task2_order_out)
 *   WRITE buffer      g_buf_22_23 (task2_buffer_send)
 *   WRITE blackboard  g_bb_22_23  (task2_blackboard_write)
 * ============================================================ */
void task22_entry(void)
{
    RETURN_CODE_TYPE    rc;
    APP_INTEGER_TYPE    acc3_in        = 0;
    APP_INTEGER_TYPE    buf_from_23    = 0;
    APP_INTEGER_TYPE    bb_from_23     = 0;
    APP_INTEGER_TYPE    order_out      = 0;
    APP_INTEGER_TYPE    buf_to_23      = 0;
    APP_INTEGER_TYPE    bb_to_23       = 0;
    MESSAGE_SIZE_TYPE   msg_len        = 0;

    while (1) {
        /* ---- 1. Read blackboard from task21 (acc3_in) ------- */
        READ_BLACKBOARD(g_bb_21_22,
                        (MESSAGE_ADDR_TYPE)&acc3_in,
                        &msg_len,
                        &rc);
        CHECK_CODE("Tsk22 READ_BLACKBOARD bb_21_22", rc);

        /* ---- 2. Receive buffer from task23 ------------------ */
        RECEIVE_BUFFER(g_buf_23_22,
                       ARINC_ZERO_TIMEOUT,
                       (MESSAGE_ADDR_TYPE)&buf_from_23,
                       &msg_len,
                       &rc);
        if (rc != NO_ERROR && rc != TIMED_OUT) {
            CHECK_CODE("Tsk22 RECEIVE_BUFFER buf_23_22", rc);
        }

        /* ---- 3. Read blackboard from task23 ----------------- */
        READ_BLACKBOARD(g_bb_23_22,
                        (MESSAGE_ADDR_TYPE)&bb_from_23,
                        &msg_len,
                        &rc);
        CHECK_CODE("Tsk22 READ_BLACKBOARD bb_23_22", rc);

        /* ---- 4. Application logic (stub) -------------------- */
        order_out = acc3_in + buf_from_23;
        buf_to_23 = acc3_in - bb_from_23;
        bb_to_23  = acc3_in ^ buf_from_23;   /* synthetic aggregate */

        /* ---- 5. Write order feedback buffer to task21 ------- */
        SEND_BUFFER(g_buf_22_21,
                    (MESSAGE_ADDR_TYPE)&order_out,
                    (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
                    ARINC_ZERO_TIMEOUT,
                    &rc);
        CHECK_CODE("Tsk22 SEND_BUFFER buf_22_21", rc);

        /* ---- 6. Write buffer to task23 ---------------------- */
        SEND_BUFFER(g_buf_22_23,
                    (MESSAGE_ADDR_TYPE)&buf_to_23,
                    (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
                    ARINC_ZERO_TIMEOUT,
                    &rc);
        CHECK_CODE("Tsk22 SEND_BUFFER buf_22_23", rc);

        /* ---- 7. Write blackboard to task23 ------------------ */
        DISPLAY_BLACKBOARD(g_bb_22_23,
                           (MESSAGE_ADDR_TYPE)&bb_to_23,
                           (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
                           &rc);
        CHECK_CODE("Tsk22 DISPLAY_BLACKBOARD bb_22_23", rc);

        /* ---- 8. Wait for next period ------------------------ */
        PERIODIC_WAIT(&rc);
        CHECK_CODE("Tsk22 PERIODIC_WAIT", rc);
    }
}

/* ============================================================
 * task23_entry
 *
 * Periodic task – 100 ms
 * Data-flow (from AADL connections):
 *   READ  buffer      g_buf_22_23      (task3_buffer_receive)
 *   READ  blackboard  g_bb_22_23       (task3_blackboard_read)
 *   WRITE buffer      g_buf_23_22      (task3_buffer_send)
 *   WRITE blackboard  g_bb_23_22       (task3_blackboard_write)
 *   SEND  queuing port g_queuing_port_out (task3Queuing)
 * Subprogram calls:
 *   commandboard_receiveinput_spg
 *   commandboard_printinfos_spg
 * ============================================================ */
void task23_entry(void)
{
    RETURN_CODE_TYPE    rc;
    APP_INTEGER_TYPE    buf_from_22    = 0;
    APP_INTEGER_TYPE    bb_from_22     = 0;
    APP_INTEGER_TYPE    buf_to_22      = 0;
    APP_INTEGER_TYPE    bb_to_22       = 0;
    APP_INTEGER_TYPE    queuing_out    = 0;
    APP_INTEGER_TYPE    sampling_data  = 0;
    MESSAGE_SIZE_TYPE   msg_len        = 0;

    while (1) {
        /* ---- 1. Receive supplementary sampling input -------- */
        commandboard_receiveinput_spg(&sampling_data, &rc);
        CHECK_CODE("Tsk23 receiveinput_spg", rc);

        /* ---- 2. Receive buffer from task22 ------------------ */
        RECEIVE_BUFFER(g_buf_22_23,
                       ARINC_ZERO_TIMEOUT,
                       (MESSAGE_ADDR_TYPE)&buf_from_22,
                       &msg_len,
                       &rc);
        if (rc != NO_ERROR && rc != TIMED_OUT) {
            CHECK_CODE("Tsk23 RECEIVE_BUFFER buf_22_23", rc);
        }

        /* ---- 3. Read blackboard from task22 ----------------- */
        READ_BLACKBOARD(g_bb_22_23,
                        (MESSAGE_ADDR_TYPE)&bb_from_22,
                        &msg_len,
                        &rc);
        CHECK_CODE("Tsk23 READ_BLACKBOARD bb_22_23", rc);

        /* ---- 4. Application logic (stub) -------------------- */
        buf_to_22   = buf_from_22 + sampling_data;
        bb_to_22    = bb_from_22  + sampling_data;
        queuing_out = buf_to_22   + bb_to_22;

        /* ---- 5. Write buffer to task22 ---------------------- */
        SEND_BUFFER(g_buf_23_22,
                    (MESSAGE_ADDR_TYPE)&buf_to_22,
                    (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
                    ARINC_ZERO_TIMEOUT,
                    &rc);
        CHECK_CODE("Tsk23 SEND_BUFFER buf_23_22", rc);

        /* ---- 6. Write blackboard to task22 ------------------ */
        DISPLAY_BLACKBOARD(g_bb_23_22,
                           (MESSAGE_ADDR_TYPE)&bb_to_22,
                           (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
                           &rc);
        CHECK_CODE("Tsk23 DISPLAY_BLACKBOARD bb_23_22", rc);

        /* ---- 7. Send result to inter-partition queuing port - */
        SEND_QUEUING_MESSAGE(g_queuing_port_out,
                             (MESSAGE_ADDR_TYPE)&queuing_out,
                             (MESSAGE_SIZE_TYPE)APP_DATA_SIZE,
                             (SYSTEM_TIME_TYPE)QUEUING_PORT_OUT_TIMEOUT,
                             &rc);
        CHECK_CODE("Tsk23 SEND_QUEUING_MESSAGE queuing_port_out", rc);

        /* ---- 8. Print diagnostic info ----------------------- */
        commandboard_printinfos_spg(&queuing_out, &rc);
        CHECK_CODE("Tsk23 printinfos_spg", rc);

        /* ---- 9. Wait for next period ------------------------ */
        PERIODIC_WAIT(&rc);
        CHECK_CODE("Tsk23 PERIODIC_WAIT", rc);
    }
}