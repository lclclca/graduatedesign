#ifndef GLOBALS_H
#define GLOBALS_H

#include <os/pos/apex/apexLib.h>
#include "gtypes.h"

/* 分区名前缀：ps2_ */

/* 采样端口 ID */
extern SAMPLING_PORT_ID_TYPE  ps2_pr2samplingin_id;

/* 队列端口 ID */
extern QUEUING_PORT_ID_TYPE   ps2_pr2queueingout_id;

/* 黑板 ID */
extern BLACKBOARD_ID_TYPE     ps2_bb_acc3_id;
extern BLACKBOARD_ID_TYPE     ps2_bb_t2t3_id;
extern BLACKBOARD_ID_TYPE     ps2_bb_t3t2_id;

/* 缓冲区 ID */
extern BUFFER_ID_TYPE         ps2_buf_order_id;
extern BUFFER_ID_TYPE         ps2_buf_t2tot3_id;
extern BUFFER_ID_TYPE         ps2_buf_t3tot2_id;

/* 任务句柄数组：0=task21, 1=task22, 2=task23 */
extern PROCESS_ID_TYPE        arinc_threads[3];

#endif /* GLOBALS_H */