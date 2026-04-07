#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <os/pos/apex/apexLib.h>

/*
 * CHECK_CODE - Check APEX return code and print result.
 * @msg  : string identifying the API call
 * @code : RETURN_CODE_TYPE value returned by the APEX call
 */
#define CHECK_CODE(msg, code)                                        \
    do {                                                             \
        if ((code) == NO_ERROR) {                                    \
            printf("[PS3] %s: SUCCESS\n", (msg));                    \
        } else {                                                     \
            printf("[PS3] %s: ERROR code = %d\n", (msg), (int)(code)); \
        }                                                            \
    } while (0)

#endif /* GLOBALS_H */