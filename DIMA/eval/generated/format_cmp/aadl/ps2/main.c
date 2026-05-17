/* ============================================================
 * main.c
 * Partition P2 – Entry Point
 * ARINC 653 / ACoreOS653
 *
 * This file contains the partition main() function, which is
 * invoked by the ACoreOS653 runtime for every partition start
 * (COLD_START and WARM_START).  It:
 *   1. Determines the start condition.
 *   2. Registers the partition Health Monitor callback.
 *   3. Calls deployment_init() to create ARINC 653 resources.
 *   4. Switches the partition to NORMAL operating mode.
 *
 * DAL-A reference: ARINC 653 Part 1, §3.2 (Partition Modes)
 * ============================================================ */

#include "apex/apex.h"
#include "gtypes.h"
#include "deployment.h"

int main(void)
{
    RETURN_CODE_TYPE        rc;
    PARTITION_STATUS_TYPE   status;

    /* ----------------------------------------------------------
     * Step 1 – Query current partition start condition
     * ---------------------------------------------------------- */
    GET_PARTITION_STATUS(&status, &rc);
    CHECK_CODE("main GET_PARTITION_STATUS", rc);

    /* ----------------------------------------------------------
     * Step 2 – Register partition-level HM callback
     *
     * CREATE_ERROR_HANDLER registers the partition_HM_callback
     * function and allocates a dedicated error-handler stack.
     * The module_HM_callback is registered at module level by
     * the ACoreOS653 system integrator via the XML configuration;
     * we expose it here so the linker can resolve the symbol.
     * ---------------------------------------------------------- */
    CREATE_ERROR_HANDLER((SYSTEM_ADDRESS_TYPE)partition_HM_callback,
                         (STACK_SIZE_TYPE)4096,
                         &rc);
    CHECK_CODE("main CREATE_ERROR_HANDLER partition_HM_callback", rc);

    /* ----------------------------------------------------------
     * Step 3 – Initialise resources
     *
     * Both COLD_START and WARM_START pass through deployment_init
     * because ARINC 653 §3.2.2 requires all resources to be
     * re-created on every start.  In a WARM_START the application
     * may additionally restore persistent state (omitted here for
     * clarity, but the framework hook is the same).
     * ---------------------------------------------------------- */
    if (status.OPERATING_MODE == IDLE) {
        /* First cold start – full initialisation              */
        deployment_init();
    } else if (status.OPERATING_MODE == COLD_START) {
        /* Cold restart triggered by HM                        */
        deployment_init();
    } else {
        /* WARM_START – re-create resources, restore state     */
        deployment_init();
        /* Application-specific warm-start restore hook would
         * go here if the design required persistent recovery.  */
    }

    /* ----------------------------------------------------------
     * Step 4 – Switch to NORMAL operating mode
     *
     * After this call the scheduler begins dispatching the three
     * periodic tasks.  main() does not return.
     * ---------------------------------------------------------- */
    SET_PARTITION_MODE(NORMAL, &rc);
    CHECK_CODE("main SET_PARTITION_MODE NORMAL", rc);

    /* Should never reach this point */
    while (1) { /* deliberate infinite loop */ }

    return 0;
}