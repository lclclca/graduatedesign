/* ============================================================
 * globals.c
 * Partition P2 – Global Handle Definitions
 * ARINC 653 / ACoreOS653
 * ============================================================ */

#include "globals.h"

/* ------------------------------------------------------------
 * Inter-partition port handles
 * ------------------------------------------------------------ */
SAMPLING_PORT_ID_TYPE   g_sampling_port_in  = 0;
QUEUING_PORT_ID_TYPE    g_queuing_port_out  = 0;

/* ------------------------------------------------------------
 * Intra-partition blackboard handles
 * ------------------------------------------------------------ */
BLACKBOARD_ID_TYPE  g_bb_21_22 = 0;
BLACKBOARD_ID_TYPE  g_bb_22_23 = 0;
BLACKBOARD_ID_TYPE  g_bb_23_22 = 0;

/* ------------------------------------------------------------
 * Intra-partition buffer handles
 * ------------------------------------------------------------ */
BUFFER_ID_TYPE  g_buf_22_21 = 0;
BUFFER_ID_TYPE  g_buf_22_23 = 0;
BUFFER_ID_TYPE  g_buf_23_22 = 0;

/* ------------------------------------------------------------
 * Task process handles
 * ------------------------------------------------------------ */
PROCESS_ID_TYPE g_task21_id = 0;
PROCESS_ID_TYPE g_task22_id = 0;
PROCESS_ID_TYPE g_task23_id = 0;