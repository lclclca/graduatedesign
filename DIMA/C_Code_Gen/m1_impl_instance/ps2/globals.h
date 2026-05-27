#ifndef __IMA2C_GENERATED_GLOBALS_H_
#define __IMA2C_GENERATED_GLOBALS_H_

/* ==============================================================
 *  FILE:    globals.h
 *  ROLE:    运行时工具宏 + 全局辅助函数（Runtime Utility Macros）
 *
 *  IMA 架构层次：分区运行时支持层（Partition Runtime Support）
 *  IMA 分区：ps2 (P2)     模块：M1
 *  对应 AADL 文件：无直接对应（为 ARINC 653 APEX API 调用提供错误检查支持）
 *
 *  本文件提供两类工具：
 *
 *  1. CHECK_CODE(msg, code)
 *     封装 ARINC 653 APEX 服务返回值检查。
 *     activity.c 和 main.c 中每次调用 APEX 服务（CREATE_xxx、
 *     READ_xxx、WRITE_xxx、SEND_xxx、RECEIVE_xxx 等）之后，
 *     都要调用此宏以打印调用是否成功。
 *       NO_ERROR    → 打印成功信息
 *       其他错误码  → 打印具体错误名称，便于调试
 *
 *  2. CHECK_VALIDCODE(msg, validCode, ret)
 *     专用于采样端口读取（READ_SAMPLING_MESSAGE）的检查。
 *     ARINC 653 采样端口在读取时额外返回 VALIDITY_TYPE：
 *       VALID   → 消息在刷新周期（Refresh_Period）内已更新，数据有效
 *       INVALID → 消息超期未更新，数据可能过时
 *     此宏同时检查 VALIDITY 和 RETURN_CODE。
 *
 *  3. codeToStr(retCode)
 *     将 RETURN_CODE_TYPE 枚举值转换为可读字符串，
 *     供以上两个宏在打印错误时调用。
 * ==============================================================*/

#include <stdio.h>
#include <os/pos/apex/apexLib.h>

/* 检查 APEX 服务返回码：成功打印 NO_ERROR，失败打印具体错误名称 */
#define CHECK_CODE(msg, code) \
    if ((code) == NO_ERROR) { \
        printf("%s : NO_ERROR\n", (msg)); \
    } else { \
        printf("%s : ERROR(%s)\n", (msg), codeToStr(code)); \
    }

/* 检查采样端口读取的有效性（VALIDITY）和返回码（RETURN_CODE） */
#define CHECK_VALIDCODE(msg, validCode, ret) \
    if ((validCode) == VALID && (ret) == NO_ERROR) { \
        printf("%s : VALID/NO_ERROR\n", (msg)); \
    } else { \
        printf("%s : INVALID or ERROR(%s)\n", (msg), codeToStr(ret)); \
    }

/* 将 RETURN_CODE_TYPE 枚举转为可读字符串 */
static char *codeToStr(RETURN_CODE_TYPE retCode)
{
    switch (retCode)
    {
        case NO_ERROR:       return "NO_ERROR";
        case NO_ACTION:      return "NO_ACTION";
        case NOT_AVAILABLE:  return "NOT_AVAILABLE";
        case INVALID_PARAM:  return "INVALID_PARAM";
        case INVALID_CONFIG: return "INVALID_CONFIG";
        case INVALID_MODE:   return "INVALID_MODE";
        case TIMED_OUT:      return "TIMED_OUT";
        default:             break;
    }
    return "UNKNOWN";
}

#endif
