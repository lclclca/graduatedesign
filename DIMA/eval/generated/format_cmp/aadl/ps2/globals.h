#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <os/pos/apex/apexLib.h>

/*
 * CHECK_CODE(msg, code)
 * Prints success if code == NO_ERROR, otherwise prints the actual error code.
 */
#define CHECK_CODE(msg, code)                                           \
    do {                                                                \
        if ((code) == NO_ERROR) {                                       \
            printf("[OK]    %s\n", (msg));                              \
        } else {                                                        \
            printf("[ERROR] %s failed with code: %d\n", (msg), (int)(code)); \
        }                                                               \
    } while (0)

#endif /* GLOBALS_H */