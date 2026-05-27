#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <os/pos/apex/apexLib.h>

/*
 * CHECK_CODE(msg, code):
 *   If code == NO_ERROR, print success message.
 *   Otherwise, print the actual error code.
 */
#define CHECK_CODE(msg, code) \
    do { \
        if ((code) == NO_ERROR) { \
            printf("[OK]    %s succeeded\n", (msg)); \
        } else { \
            printf("[ERROR] %s failed with code: %d\n", (msg), (int)(code)); \
        } \
    } while (0)

#endif /* GLOBALS_H */