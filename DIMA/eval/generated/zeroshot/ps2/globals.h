#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <os/pos/apex/apexLib.h>

/*
 * CHECK_CODE - Check APEX return code and print result.
 * @msg  : string describing the API call
 * @code : RETURN_CODE_TYPE returned by the APEX call
 */
#define CHECK_CODE(msg, code) \
    do { \
        if ((code) == NO_ERROR) { \
            printf("[OK]  %s succeeded.\n", (msg)); \
        } else { \
            printf("[ERR] %s failed, return code = %d\n", (msg), (int)(code)); \
        } \
    } while (0)

#endif /* GLOBALS_H */