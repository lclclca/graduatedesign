#ifndef __DISP_GENERATED_GLOBALS_H_
#define __DISP_GENERATED_GLOBALS_H_

#include <stdio.h>
#include <os/pos/apex/apexLib.h>

#define CHECK_CODE(msg, code) \
    if ((code) == NO_ERROR) { \
        printf("%s : NO_ERROR\n", (msg)); \
    } else { \
        printf("%s : ERROR(%s)\n", (msg), codeToStr(code)); \
    }

#define CHECK_VALIDCODE(msg, validCode, ret) \
    if ((validCode) == VALID && (ret) == NO_ERROR) { \
        printf("%s : VALID/NO_ERROR\n", (msg)); \
    } else { \
        printf("%s : INVALID or ERROR(%s)\n", (msg), codeToStr(ret)); \
    }

static char *codeToStr(RETURN_CODE_TYPE retCode)
{
    switch (retCode) {
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