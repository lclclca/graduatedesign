#ifndef ACTIVITY_H
#define ACTIVITY_H

/* ============================================================
 * activity.h
 * Partition P2 – Task (Process) Entry-Point Declarations
 * ARINC 653 / ACoreOS653
 *
 * Each AADL thread maps to one ARINC 653 process whose entry
 * function is declared here.
 * ============================================================ */

#include "apex/apextypes.h"
#include "gtypes.h"

/* ------------------------------------------------------------
 * Tsk21 – Period 50 ms, Priority 2
 * Calls: commandboard_receiveinput_spg
 *        commandboard_printinfos_spg
 * Reads:  sampling port (pr2samplingin / task21sampling)
 *         buffer        (g_buf_22_21 / task1_order_in)
 * Writes: blackboard    (g_bb_21_22  / task1_acc3_out)
 * ------------------------------------------------------------ */
void task21_entry(void);

/* ------------------------------------------------------------
 * Tsk22 – Period 50 ms, Priority 3
 * Reads:  blackboard (g_bb_21_22  / task2_acc3_in)
 *         buffer     (g_buf_23_22 / task2_buffer_receive)
 *         blackboard (g_bb_23_22  / task2_blackboard_read)
 * Writes: buffer     (g_buf_22_21 / task2_order_out)
 *         buffer     (g_buf_22_23 / task2_buffer_send)
 *         blackboard (g_bb_22_23  / task2_blackboard_write)
 * ------------------------------------------------------------ */
void task22_entry(void);

/* ------------------------------------------------------------
 * Tsk23 – Period 100 ms, Priority 4
 * Calls: commandboard_receiveinput_spg
 *        commandboard_printinfos_spg
 * Reads:  buffer     (g_buf_22_23 / task3_buffer_receive)
 *         blackboard (g_bb_22_23  / task3_blackboard_read)
 * Writes: buffer     (g_buf_23_22 / task3_buffer_send)
 *         blackboard (g_bb_23_22  / task3_blackboard_write)
 *         queuing port (g_queuing_port_out / task3Queuing)
 * ------------------------------------------------------------ */
void task23_entry(void);

#endif /* ACTIVITY_H */