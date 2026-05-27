#include "globals.h"

/* 采样端口 ID */
SAMPLING_PORT_ID_TYPE  ps2_pr2samplingin_id;

/* 队列端口 ID */
QUEUING_PORT_ID_TYPE   ps2_pr2queueingout_id;

/* 黑板 ID */
BLACKBOARD_ID_TYPE     ps2_bb_acc3_id;
BLACKBOARD_ID_TYPE     ps2_bb_t2t3_id;
BLACKBOARD_ID_TYPE     ps2_bb_t3t2_id;

/* 缓冲区 ID */
BUFFER_ID_TYPE         ps2_buf_order_id;
BUFFER_ID_TYPE         ps2_buf_t2tot3_id;
BUFFER_ID_TYPE         ps2_buf_t3tot2_id;

/* 任务句柄数组 */
PROCESS_ID_TYPE        arinc_threads[3];