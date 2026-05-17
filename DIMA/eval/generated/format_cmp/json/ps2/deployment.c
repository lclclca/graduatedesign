/*============================================================
 * deployment.c
 * 部署初始化：创建所有 ARINC 653 IPC 对象 —— ps2 分区
 *============================================================*/
#include <string.h>
#include "deployment.h"
#include "globals.h"

/*------------------------------------------------------------
 * 内部辅助：创建采样端口
 *------------------------------------------------------------*/
static void create_sampling_ports(void)
{
    RETURN_CODE_TYPE ret;

    CREATE_SAMPLING_PORT(
        (SAMPLING_PORT_NAME_TYPE)SAMPLING_IN_PORT_NAME,
        SAMPLING_IN_MAX_MSG_SZ,
        DESTINATION,
        SAMPLING_IN_REFRESH_NS,
        &g_pr2samplingin_id,
        &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT pr2samplingin", ret);
}

/*------------------------------------------------------------
 * 内部辅助：创建队列端口
 *------------------------------------------------------------*/
static void create_queuing_ports(void)
{
    RETURN_CODE_TYPE ret;

    CREATE_QUEUING_PORT(
        (QUEUING_PORT_NAME_TYPE)QUEUING_OUT_PORT_NAME,
        QUEUING_OUT_MAX_MSG_SZ,
        QUEUING_OUT_MAX_NB_MSG,
        SOURCE,
        FIFO,
        &g_pr2queueingout_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT pr2queueingout", ret);
}

/*------------------------------------------------------------
 * 内部辅助：创建黑板
 *------------------------------------------------------------*/
static void create_blackboards(void)
{
    RETURN_CODE_TYPE ret;

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)BB_ACC3_NAME,
        (MESSAGE_SIZE_TYPE)sizeof(BB_ACC3_TYPE),
        &g_bb_acc3_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_acc3", ret);

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)BB_T2T3_NAME,
        (MESSAGE_SIZE_TYPE)sizeof(BB_T2T3_TYPE),
        &g_bb_t2t3_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_t2t3", ret);

    CREATE_BLACKBOARD(
        (BLACKBOARD_NAME_TYPE)BB_T3T2_NAME,
        (MESSAGE_SIZE_TYPE)sizeof(BB_T3T2_TYPE),
        &g_bb_t3t2_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_t3t2", ret);
}

/*------------------------------------------------------------
 * 内部辅助：创建缓冲区
 *------------------------------------------------------------*/
static void create_buffers(void)
{
    RETURN_CODE_TYPE ret;

    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)BUF_ORDER_NAME,
        (MESSAGE_SIZE_TYPE)sizeof(BUF_ORDER_TYPE),
        (MESSAGE_RANGE_TYPE)BUF_ORDER_NB_MSG,
        FIFO,
        &g_buf_order_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER buf_order", ret);

    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)BUF_T2TOT3_NAME,
        (MESSAGE_SIZE_TYPE)sizeof(BUF_T2TOT3_TYPE),
        (MESSAGE_RANGE_TYPE)BUF_T2TOT3_NB_MSG,
        FIFO,
        &g_buf_t2tot3_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER buf_t2tot3", ret);

    CREATE_BUFFER(
        (BUFFER_NAME_TYPE)BUF_T3TOT2_NAME,
        (MESSAGE_SIZE_TYPE)sizeof(BUF_T3TOT2_TYPE),
        (MESSAGE_RANGE_TYPE)BUF_T3TOT2_NB_MSG,
        FIFO,
        &g_buf_t3tot2_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER buf_t3tot2", ret);
}

/*------------------------------------------------------------
 * 公开接口：一次性初始化所有 IPC 对象
 *------------------------------------------------------------*/
void deployment_init(void)
{
    create_sampling_ports();
    create_queuing_ports();
    create_blackboards();
    create_buffers();
}