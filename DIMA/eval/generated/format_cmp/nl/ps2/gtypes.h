#ifndef GTYPES_H
#define GTYPES_H

#include <apex/apex.h>
#include <apex/apexBlackboard.h>
#include <apex/apexBuffer.h>
#include <apex/apexProcess.h>
#include <apex/apexSampling.h>
#include <apex/apexQueuing.h>
#include <apex/apexError.h>
#include <apex/apexTime.h>
#include <apex/apexPartition.h>

/* ---------------------------------------------------------------
 * Basic integer type exchanged through ports and IPC resources
 * --------------------------------------------------------------- */
typedef APEX_INTEGER  T_INT_DATA;

/* Convenience macro – checks RETURN_CODE and halts on error */
#define CHECK_CODE(msg, code)                                        \
    do {                                                             \
        if ((code) != NO_ERROR) {                                    \
            /* In a real system: log + raise HM event */            \
            while (1) { /* halt */ }                                 \
        }                                                            \
    } while (0)

#endif /* GTYPES_H */