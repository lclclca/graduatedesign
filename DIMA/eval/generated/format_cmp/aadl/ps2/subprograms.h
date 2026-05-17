#ifndef SUBPROGRAMS_H
#define SUBPROGRAMS_H

/* ============================================================
 * subprograms.h
 * Partition P2 – Subprogram Declarations
 * Maps to AADL subprograms::commandboard_receiveinput_spg
 *                            commandboard_printinfos_spg
 * Called by Tsk21 and Tsk23
 * ============================================================ */

#include "apex/apextypes.h"
#include "gtypes.h"

/* ------------------------------------------------------------
 * commandboard_receiveinput_spg
 *
 * Reads one sample from the inbound sampling port
 * (pr2samplingin → task21sampling) and returns the value.
 * Called by: Tsk21, Tsk23 (indirectly via queuing path)
 * ------------------------------------------------------------ */
void commandboard_receiveinput_spg(APP_INTEGER_TYPE *data_out,
                                   RETURN_CODE_TYPE *rc);

/* ------------------------------------------------------------
 * commandboard_printinfos_spg
 *
 * Logs / prints partition diagnostic information.
 * In a DAL-A context this writes to a health-monitor trace
 * buffer; here it is a stub with a defined calling convention.
 * Called by: Tsk21, Tsk23
 * ------------------------------------------------------------ */
void commandboard_printinfos_spg(const APP_INTEGER_TYPE *data_in,
                                 RETURN_CODE_TYPE       *rc);

#endif /* SUBPROGRAMS_H */