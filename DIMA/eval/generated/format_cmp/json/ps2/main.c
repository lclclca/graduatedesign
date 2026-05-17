/*============================================================
 * main.c
 * 分区主入口 —— ps2 分区
 *
 * 执行顺序（ARINC 653 冷启动标准流程）：
 *   1. 注册 HM 回调
 *   2. 创建所有 IPC 对象（deployment_init）
 *   3. 创建所有任务
 *   4. 切换到 NORMAL 模式
 *============================================================*/
#include <string.h>
#include "globals.h"
#include "deployment.h"
#include "activity.h"
#include "gtypes.h"

/*------------------------------------------------------------
 * 前向声明：HM 回调
 *------------------------------------------------------------*/
static void module_HM_callback(ERROR_STATUS_TYPE *error);
static void partition_HM_callback(ERROR_STATUS_TYPE *error);

/*------------------------------------------------------------
 * module_HM_callback
 * 模块级健康监控回调。
 * 由 ARINC 653 HM 在检测到模块级错误时调用。
 *------------------------------------------------------------*/
static void module_HM_callback(ERROR_STATUS_TYPE *error)
{
    if (error == NULL) {
        return;
    }

    /*
     * 策略：
     *   - 对于可恢复错误（APPLICATION_ERROR）→ 忽略并继续
     *   - 对于不可恢复错误               → 重置模块
     * 此处保守实现：任何模块级错误均请求模块重启。
     */
    switch (error->ERROR_CODE) {
        case APPLICATION_ERROR:
            /* 应用层错误可选择恢复，视具体策略而定 */
            break;

        case DEADLINE_MISSED_ERROR:
        case HARDWARE_FAULT_ERROR:
        case POWER_FAIL_ERROR:
        default:
            /* 不可恢复：请求模块重置（平台相关，此处以 SET_MODULE_MODE 示意） */
            {
                RETURN_CODE_TYPE ret;
                SET_MODULE_MODE(IDLE, &ret);
                /* 若 SET_MODULE_MODE 不支持，平台将调用安全关机序列 */
                (void)ret;
            }
            break;
    }
}

/*------------------------------------------------------------
 * partition_HM_callback
 * 分区级健康监控回调。
 * 由 ARINC 653 HM 在检测到分区级错误时调用。
 *------------------------------------------------------------*/
static void partition_HM_callback(ERROR_STATUS_TYPE *error)
{
    if (error == NULL) {
        return;
    }

    switch (error->ERROR_CODE) {
        case APPLICATION_ERROR:
            /*
             * 应用层错误：尝试恢复——重启出错进程。
             * STOP_SELF / START 仅在进程级错误时有意义；
             * 若 FAILED_PROCESS_ID 有效则重启对应进程。
             */
            if (error->FAILED_PROCESS_ID != 0) {
                RETURN_CODE_TYPE ret;
                STOP(error->FAILED_PROCESS_ID, &ret);
                (void)ret;
                START(error->FAILED_PROCESS_ID, &ret);
                (void)ret;
            }
            break;

        case DEADLINE_MISSED_ERROR:
            /*
             * 截止期错误：停止该进程，允许其他进程继续运行。
             */
            if (error->FAILED_PROCESS_ID != 0) {
                RETURN_CODE_TYPE ret;
                STOP(error->FAILED_PROCESS_ID, &ret);
                (void)ret;
            }
            break;

        case STACK_OVERFLOW_ERROR:
        case HARDWARE_FAULT_ERROR:
        default:
            /*
             * 不可恢复错误：重启整个分区。
             */
            {
                RETURN_CODE_TYPE ret;
                SET_PARTITION_MODE(WARM_START, &ret);
                (void)ret;
            }
            break;
    }
}

/*------------------------------------------------------------
 * 内部辅助：创建三个周期性任务
 *------------------------------------------------------------*/
static void create_tasks(void)
{
    PROCESS_ATTRIBUTE_TYPE attr;
    RETURN_CODE_TYPE        ret;

    /*-- task21: 50 ms, 优先级 2 --*/
    memset(&attr, 0, sizeof(attr));
    strncpy(attr.NAME, "task21", MAX_NAME_LENGTH - 1);
    attr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task21_entry;
    attr.STACK_SIZE    = (STACK_SIZE_TYPE)TASK_STACK_SIZE;
    attr.BASE_PRIORITY = (PRIORITY_TYPE)TASK21_PRIORITY;
    attr.PERIOD        = TASK21_PERIOD_NS;
    attr.TIME_CAPACITY = TASK_TIME_CAPACITY_NS;
    attr.DEADLINE      = HARD;

    CREATE_PROCESS(&attr, &g_task21_id, &ret);
    CHECK_CODE("CREATE_PROCESS task21", ret);

    START(g_task21_id, &ret);
    CHECK_CODE("START task21", ret);

    /*-- task22: 50 ms, 优先级 3 --*/
    memset(&attr, 0, sizeof(attr));
    strncpy(attr.NAME, "task22", MAX_NAME_LENGTH - 1);
    attr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task22_entry;
    attr.STACK_SIZE    = (STACK_SIZE_TYPE)TASK_STACK_SIZE;
    attr.BASE_PRIORITY = (PRIORITY_TYPE)TASK22_PRIORITY;
    attr.PERIOD        = TASK22_PERIOD_NS;
    attr.TIME_CAPACITY = TASK_TIME_CAPACITY_NS;
    attr.DEADLINE      = HARD;

    CREATE_PROCESS(&attr, &g_task22_id, &ret);
    CHECK_CODE("CREATE_PROCESS task22", ret);

    START(g_task22_id, &ret);
    CHECK_CODE("START task22", ret);

    /*-- task23: 100 ms, 优先级 4 --*/
    memset(&attr, 0, sizeof(attr));
    strncpy(attr.NAME, "task23", MAX_NAME_LENGTH - 1);
    attr.ENTRY_POINT   = (SYSTEM_ADDRESS_TYPE)task23_entry;
    attr.STACK_SIZE    = (STACK_SIZE_TYPE)TASK_STACK_SIZE;
    attr.BASE_PRIORITY = (PRIORITY_TYPE)TASK23_PRIORITY;
    attr.PERIOD        = TASK23_PERIOD_NS;
    attr.TIME_CAPACITY = TASK_TIME_CAPACITY_NS;
    attr.DEADLINE      = HARD;

    CREATE_PROCESS(&attr, &g_task23_id, &ret);
    CHECK_CODE("CREATE_PROCESS task23", ret);

    START(g_task23_id, &ret);
    CHECK_CODE("START task23", ret);
}

/*------------------------------------------------------------
 * main —— 分区初始化入口（ARINC 653 冷/暖启动均调用此函数）
 *------------------------------------------------------------*/
int main(void)
{
    RETURN_CODE_TYPE       ret;
    PARTITION_STATUS_TYPE  status;

    /*-- 查询启动模式 --*/
    GET_PARTITION_STATUS(&status, &ret);
    CHECK_CODE("GET_PARTITION_STATUS", ret);

    if (status.OPERATING_MODE == COLD_START ||
        status.OPERATING_MODE == WARM_START) {

        /*-- 1. 注册 HM 回调 --*/
        CREATE_ERROR_HANDLER(
            (SYSTEM_ADDRESS_TYPE)partition_HM_callback,
            (STACK_SIZE_TYPE)TASK_STACK_SIZE,
            &ret);
        CHECK_CODE("CREATE_ERROR_HANDLER (partition)", ret);

        /* 模块 HM 回调由模块级初始化注册；
         * ACoreOS653 通过配置表绑定，此处显式注册供参考。 */
        (void)module_HM_callback;   /* 抑制未使用警告 */

        /*-- 2. 创建 IPC 对象 --*/
        deployment_init();

        /*-- 3. 创建并启动任务 --*/
        create_tasks();

        /*-- 4. 进入 NORMAL 模式 --*/
        SET_PARTITION_MODE(NORMAL, &ret);
        CHECK_CODE("SET_PARTITION_MODE NORMAL", ret);
    }

    /* 若到达此处，表明模式切换异常——永远不应执行 */
    for (;;) { ; }

    return 0;
}