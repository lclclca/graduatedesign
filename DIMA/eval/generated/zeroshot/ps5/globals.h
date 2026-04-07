#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <os/pos/apex/apexLib.h>

/*
 * CHECK_CODE - 检查 APEX API 返回码
 *   msg  : API 名称字符串，用于打印
 *   code : RETURN_CODE_TYPE 类型的返回值
 * 若返回 NO_ERROR 则打印成功信息，否则打印真实错误码。
 */
#define CHECK_CODE(msg, code)                                          \
    do {                                                               \
        if ((code) == NO_ERROR) {                                      \
            printf("[PS5] %s: SUCCESS\n", (msg));                      \
        } else {                                                       \
            printf("[PS5] %s: ERROR code = %d\n", (msg), (int)(code));\
        }                                                              \
    } while (0)

#endif /* GLOBALS_H */