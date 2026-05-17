#ifndef ACTIVITY_H
#define ACTIVITY_H

/* ---------------------------------------------------------------
 * Task entry-point declarations for ps2
 * --------------------------------------------------------------- */

/**
 * task21_proc – periodic task, 50 ms, priority 2
 *   Reads the sampling input port and publishes data on bb_acc3.
 */
void task21_proc(void);

/**
 * task22_proc – periodic task, 50 ms, priority 3
 *   Reads bb_t3t2 / buf_t3tot2, processes, writes bb_t2t3 /
 *   buf_t2tot3, and sends a queuing message on pr2queueingout.
 */
void task22_proc(void);

/**
 * task23_proc – periodic task, 100 ms, priority 4
 *   Reads bb_t2t3 / buf_t2tot3, produces a command on buf_order,
 *   and exchanges data with task22 via bb_t3t2 / buf_t3tot2.
 */
void task23_proc(void);

#endif /* ACTIVITY_H */