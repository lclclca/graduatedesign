#include "globals.h"

/* ── 采样端口 ── */
SAMPLING_PORT_ID_TYPE ps1_pr1samplingout_id;

/* ── 队列端口 ── */
QUEUING_PORT_ID_TYPE  ps1_pr1queueingin_id;

/* ── 黑板 ── */
BLACKBOARD_ID_TYPE    ps1_bb_acc1_id;
BLACKBOARD_ID_TYPE    ps1_bb_acc2_id;
BLACKBOARD_ID_TYPE    ps1_bb_sem_id;
BLACKBOARD_ID_TYPE    ps1_bb_t3t4_id;
BLACKBOARD_ID_TYPE    ps1_bb_t4t3_id;

/* ── 缓冲区 ── */
BUFFER_ID_TYPE        ps1_buf_sensor_id;
BUFFER_ID_TYPE        ps1_buf_t3tot4_id;
BUFFER_ID_TYPE        ps1_buf_t4tot3_id;

/* ── 进程句柄 ── */
PROCESS_ID_TYPE       arinc_threads[4];