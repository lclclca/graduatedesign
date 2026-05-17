/* ============================================================
 * subprograms.c
 * Partition P2 – Subprogram Implementations
 * ARINC 653 / ACoreOS653
 * ============================================================ */

#include "subprograms.h"
#include "globals.h"
#include "apex/apex.h"

/* ============================================================
 * commandboard_receiveinput_spg
 *
 * Reads the most recent message from the inbound sampling port.
 * The port was created with ARINC653::Sampling_Refresh_Period
 * of 100 ns; validity is checked via the VALIDITY output.
 * ============================================================ */
void commandboard_receiveinput_spg(APP_INTEGER_TYPE *data_out,
                                   RETURN_CODE_TYPE *rc)
{
    MESSAGE_SIZE_TYPE   msg_len  = 0;
    VALIDITY_TYPE       validity = INVALID;

    READ_SAMPLING_MESSAGE(g_sampling_port_in,
                          (MESSAGE_ADDR_TYPE)data_out,
                          &msg_len,
                          &validity,
                          rc);
    /* rc is propagated to caller; CHECK_CODE applied there */

    if (validity == INVALID) {
        /* Data too stale or port never written – use safe default */
        *data_out = (APP_INTEGER_TYPE)0;
    }
}

/* ============================================================
 * commandboard_printinfos_spg
 *
 * Writes diagnostic data to the partition trace buffer.
 * In production this would call a certified logging service;
 * here we demonstrate the ARINC 653 calling convention and
 * leave the I/O implementation as a platform hook.
 * ============================================================ */
void commandboard_printinfos_spg(const APP_INTEGER_TYPE *data_in,
                                 RETURN_CODE_TYPE       *rc)
{
    /* Platform-specific trace hook – body intentionally minimal
     * to keep this unit certifiable.  Replace with the BSP call
     * provided by the ACoreOS653 board support package.         */
    (void)data_in;   /* suppress unused-parameter warning */
    *rc = NO_ERROR;
}