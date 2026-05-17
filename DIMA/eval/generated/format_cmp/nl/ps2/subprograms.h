#ifndef SUBPROGRAMS_H
#define SUBPROGRAMS_H

#include "gtypes.h"

/* ---------------------------------------------------------------
 * Auxiliary subprogram declarations for ps2
 * --------------------------------------------------------------- */

/**
 * commandboard_receiveinput_spg
 *   Reads one integer value from the sampling input port
 *   (pr2samplingin) and returns it via *out_value.
 *   Returns NO_ERROR on success, or the APEX error code on failure.
 */
RETURN_CODE_TYPE commandboard_receiveinput_spg(T_INT_DATA *out_value);

/**
 * commandboard_printinfos_spg
 *   Prints / logs a diagnostic line composed of a label string
 *   and an associated integer value.  In a flight-certified build
 *   this would call the certified logging service; here it uses
 *   the ARINC 653 Health-Monitor reporting path conceptually.
 */
void commandboard_printinfos_spg(const char *label, T_INT_DATA value);

#endif /* SUBPROGRAMS_H */