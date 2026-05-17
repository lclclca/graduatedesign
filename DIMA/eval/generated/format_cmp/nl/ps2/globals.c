#include "globals.h"

/* ---------------------------------------------------------------
 * Process identifiers
 * --------------------------------------------------------------- */
PROCESS_ID_TYPE  g_task21_id       = 0;
PROCESS_ID_TYPE  g_task22_id       = 0;
PROCESS_ID_TYPE  g_task23_id       = 0;

/* ---------------------------------------------------------------
 * Inter-partition communication port identifiers
 * --------------------------------------------------------------- */
SAMPLING_PORT_ID_TYPE  g_pr2samplingin_id  = 0;
QUEUING_PORT_ID_TYPE   g_pr2queueingout_id = 0;

/* ---------------------------------------------------------------
 * Intra-partition IPC resource identifiers
 * --------------------------------------------------------------- */
BLACKBOARD_ID_TYPE  g_bb_acc3_id   = 0;
BLACKBOARD_ID_TYPE  g_bb_t2t3_id   = 0;
BLACKBOARD_ID_TYPE  g_bb_t3t2_id   = 0;

BUFFER_ID_TYPE  g_buf_order_id     = 0;
BUFFER_ID_TYPE  g_buf_t2tot3_id    = 0;
BUFFER_ID_TYPE  g_buf_t3tot2_id    = 0;