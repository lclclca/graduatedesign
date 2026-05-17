/*============================================================
 * subprograms.c
 * 子程序实现 —— ps2 分区
 *============================================================*/
#include <string.h>
#include "subprograms.h"
#include "globals.h"
#include "deployment.h"

/*------------------------------------------------------------
 * commandboard_receiveinput_spg
 * 从采样端口 pr2samplingin 读取最新消息。
 * 返回 G_TRUE 表示读到有效（VALID）数据，否则 G_FALSE。
 *------------------------------------------------------------*/
BOOL_TYPE commandboard_receiveinput_spg(SAMPLING_IN_MSG_TYPE *out_msg)
{
    RETURN_CODE_TYPE    ret;
    MESSAGE_SIZE_TYPE   received_len = 0;
    VALIDITY_TYPE       validity     = INVALID;

    if (out_msg == NULL) {
        return G_FALSE;
    }

    memset(out_msg, 0, sizeof(SAMPLING_IN_MSG_TYPE));

    READ_SAMPLING_MESSAGE(
        g_pr2samplingin_id,
        (MESSAGE_ADDR_TYPE)out_msg,
        &received_len,
        &validity,
        &ret);

    /* NOT_AVAILABLE 表示端口尚无数据，属正常情况，不视为错误 */
    if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
        CHECK_CODE("READ_SAMPLING_MESSAGE pr2samplingin", ret);
    }

    return ((ret == NO_ERROR) && (validity == VALID)) ? G_TRUE : G_FALSE;
}

/*------------------------------------------------------------
 * commandboard_printinfos_spg
 * 打印当前关键状态（实际项目中可对接日志驱动）。
 *------------------------------------------------------------*/
void commandboard_printinfos_spg(const SAMPLING_IN_MSG_TYPE  *in_msg,
                                 const BB_ACC3_TYPE          *acc3,
                                 const BB_T2T3_TYPE          *t2t3,
                                 const BB_T3T2_TYPE          *t3t2)
{
    /* 在 ACoreOS653 目标上，使用分区级日志宏或串口驱动输出。
       此处以标准 C 注释占位，避免引入平台相关头文件依赖。 */
    (void)in_msg;
    (void)acc3;
    (void)t2t3;
    (void)t3t2;

    /*
     * 示例输出（伪代码，按项目日志 API 替换）：
     *
     * LOG("[ps2] sensor_id=%d raw=%d ts=%u",
     *     in_msg->sensor_id, in_msg->raw_value, in_msg->timestamp_ms);
     * LOG("[ps2] acc3: value=%d cnt=%u",
     *     acc3->acc_value, acc3->update_count);
     * LOG("[ps2] t2t3: data=%d valid=%d | t3t2: data=%d valid=%d",
     *     t2t3->data, t2t3->valid, t3t2->data, t3t2->valid);
     */
}