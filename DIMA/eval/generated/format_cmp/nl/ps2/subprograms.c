#include "subprograms.h"
#include "globals.h"
#include "deployment.h"

#include <string.h>   /* memcpy */

/* ---------------------------------------------------------------
 * commandboard_receiveinput_spg
 *   Reads one T_INT_DATA value from the pr2samplingin sampling
 *   port.  Validity of the message is checked via VALID_MESSAGE.
 * --------------------------------------------------------------- */
RETURN_CODE_TYPE commandboard_receiveinput_spg(T_INT_DATA *out_value)
{
    MESSAGE_SIZE_TYPE   received_len = 0;
    VALIDITY_TYPE       validity     = INVALID_MESSAGE;
    RETURN_CODE_TYPE    ret          = NO_ERROR;
    T_INT_DATA          tmp          = 0;

    if (out_value == NULL) {
        return INVALID_PARAM;
    }

    READ_SAMPLING_MESSAGE(
        g_pr2samplingin_id,
        (MESSAGE_ADDR_TYPE)&tmp,
        &received_len,
        &validity,
        &ret
    );

    if (ret != NO_ERROR) {
        return ret;
    }

    if (validity != VALID_MESSAGE) {
        /* Port not yet written by sender – treat as no-data */
        return NOT_AVAILABLE;
    }

    if (received_len != (MESSAGE_SIZE_TYPE)sizeof(T_INT_DATA)) {
        return INVALID_CONFIG;
    }

    *out_value = tmp;
    return NO_ERROR;
}

/* ---------------------------------------------------------------
 * commandboard_printinfos_spg
 *   Lightweight diagnostic logger.  On a target with no stdout
 *   this would write to a memory-mapped trace buffer or invoke
 *   the ARINC 653 REPORT_APPLICATION_MESSAGE service.
 * --------------------------------------------------------------- */
void commandboard_printinfos_spg(const char *label, T_INT_DATA value)
{
    /*
     * In a certified environment replace this body with the
     * platform's approved trace / logging API.
     * REPORT_APPLICATION_MESSAGE is the ARINC 653 APEX call for
     * application-level diagnostic messages; shown conceptually:
     *
     *   char buf[64];
     *   RETURN_CODE_TYPE rc;
     *   snprintf(buf, sizeof(buf), "%s : %d", label, (int)value);
     *   REPORT_APPLICATION_MESSAGE((MESSAGE_ADDR_TYPE)buf,
     *                              (MESSAGE_SIZE_TYPE)strlen(buf),
     *                              &rc);
     */
    (void)label;
    (void)value;
}