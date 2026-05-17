#ifndef GLOBALS_H
#define GLOBALS_H

/* ============================================================
 * globals.h
 * Partition P2 – Global Handle Declarations
 * ARINC 653 / ACoreOS653
 * ============================================================ */

#include "apex/apextypes.h"
#include "gtypes.h"

/* ------------------------------------------------------------
 * Inter-partition port handles
 * ------------------------------------------------------------ */
extern SAMPLING_PORT_ID_TYPE    g_sampling_port_in;   /* pr2samplingin  */
extern QUEUING_PORT_ID_TYPE     g_queuing_port_out;   /* pr2queueingout */

/* ------------------------------------------------------------
 * Intra-partition blackboard handles
 * task21 -> task22
 * task22 -> task23
 * task23 -> task22
 * ------------------------------------------------------------ */
extern BLACKBOARD_ID_TYPE   g_bb_21_22;
extern BLACKBOARD_ID_TYPE   g_bb_22_23;
extern BLACKBOARD_ID_TYPE   g_bb_23_22;

/* ------------------------------------------------------------
 * Intra-partition buffer handles
 * task22 -> task21  (order feedback)
 * task22 -> task23
 * task23 -> task22
 * ------------------------------------------------------------ */
extern BUFFER_ID_TYPE   g_buf_22_21;
extern BUFFER_ID_TYPE   g_buf_22_23;
extern BUFFER_ID_TYPE   g_buf_23_22;

/* ------------------------------------------------------------
 * Task process handles
 * ------------------------------------------------------------ */
extern PROCESS_ID_TYPE  g_task21_id;
extern PROCESS_ID_TYPE  g_task22_id;
extern PROCESS_ID_TYPE  g_task23_id;

#endif /* GLOBALS_H */