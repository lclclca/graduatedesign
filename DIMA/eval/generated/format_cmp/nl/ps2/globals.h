#ifndef GLOBALS_H
#define GLOBALS_H

#include <apex/apex.h>
#include <apex/apexBlackboard.h>
#include <apex/apexBuffer.h>
#include <apex/apexProcess.h>
#include <apex/apexSampling.h>
#include <apex/apexQueuing.h>

/* ---------------------------------------------------------------
 * Process identifiers
 * --------------------------------------------------------------- */
extern PROCESS_ID_TYPE  g_task21_id;
extern PROCESS_ID_TYPE  g_task22_id;
extern PROCESS_ID_TYPE  g_task23_id;

/* ---------------------------------------------------------------
 * Inter-partition communication port identifiers
 * --------------------------------------------------------------- */
extern SAMPLING_PORT_ID_TYPE  g_pr2samplingin_id;
extern QUEUING_PORT_ID_TYPE   g_pr2queueingout_id;

/* ---------------------------------------------------------------
 * Intra-partition IPC resource identifiers
 * --------------------------------------------------------------- */
/* Blackboards */
extern BLACKBOARD_ID_TYPE  g_bb_acc3_id;
extern BLACKBOARD_ID_TYPE  g_bb_t2t3_id;
extern BLACKBOARD_ID_TYPE  g_bb_t3t2_id;

/* Buffers */
extern BUFFER_ID_TYPE  g_buf_order_id;
extern BUFFER_ID_TYPE  g_buf_t2tot3_id;
extern BUFFER_ID_TYPE  g_buf_t3tot2_id;

#endif /* GLOBALS_H */