#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

/* ============================================================
 * deployment.h
 * Partition P2 – Deployment & Initialisation Declarations
 * ARINC 653 / ACoreOS653
 * ============================================================ */

#include "apex/apextypes.h"

/* ------------------------------------------------------------
 * Create and configure all ARINC 653 resources required by
 * Partition P2 before SET_PARTITION_MODE(NORMAL) is called.
 *
 * Sequence:
 *   1. Inter-partition ports  (sampling in, queuing out)
 *   2. Intra-partition blackboards
 *   3. Intra-partition buffers
 *   4. Processes (Tsk21, Tsk22, Tsk23)
 * ------------------------------------------------------------ */
void deployment_init(void);

/* ------------------------------------------------------------
 * Module-level Health Monitor callback.
 * Required by ARINC 653 Part 1 §3.7 and DAL-A guidelines.
 * Registered with CREATE_ERROR_HANDLER at module level.
 * ------------------------------------------------------------ */
void module_HM_callback(ERROR_STATUS_TYPE *error_status);

/* ------------------------------------------------------------
 * Partition-level Health Monitor callback.
 * Registered with CREATE_ERROR_HANDLER at partition level.
 * ------------------------------------------------------------ */
void partition_HM_callback(ERROR_STATUS_TYPE *error_status);

#endif /* DEPLOYMENT_H */