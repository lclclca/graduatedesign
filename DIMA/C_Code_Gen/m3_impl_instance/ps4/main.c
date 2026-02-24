#include <os/pos/apex/apexLib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

PROCESS_ID_TYPE arinc_threads[ACoreOS653_CONFIG_NB_THREADS];
QUEUING_PORT_ID_TYPE ps4_order_in_id;
QUEUING_PORT_ID_TYPE ps4_queueingout_id;
QUEUING_PORT_ID_TYPE ps4_queueingin_id;

void appMain(void) 
{

  PROCESS_ATTRIBUTE_TYPE tattr;
  RETURN_CODE_TYPE ret = NO_ERROR;

  strcpy (tattr.NAME, "task41");
  tattr.ENTRY_POINT = task41_job;
  tattr.BASE_PRIORITY = 2;
  tattr.PERIOD = 25000000ll;
  tattr.STACK_SIZE = 8192;
  tattr.TIME_CAPACITY = 50000000ll;
  tattr.DEADLINE = SOFT;
  CREATE_PROCESS (&(tattr), &(arinc_threads[0]), &(ret));
  CHECK_CODE("CREATE_PROCESS(arinc_threads[0])", ret);
  START (arinc_threads[0], &(ret));
  CHECK_CODE("START(arinc_threads[0])", ret);


  CREATE_QUEUING_PORT ("order_in",1, sizeof (integer), DESTINATION, FIFO, &(ps4_order_in_id), &(ret));
  CHECK_CODE("CREATE_QUEUING_PORT(order_in)", ret);
  CREATE_QUEUING_PORT ("queueingout",4, sizeof (integer), SOURCE, FIFO, &(ps4_queueingout_id), &(ret));
  CHECK_CODE("CREATE_QUEUING_PORT(queueingout)", ret);
  strcpy (tattr.NAME, "task42");
  tattr.ENTRY_POINT = task42_job;
  tattr.BASE_PRIORITY = 3;
  tattr.PERIOD = 50000000ll;
  tattr.STACK_SIZE = 8192;
  tattr.TIME_CAPACITY = 50000000ll;
  tattr.DEADLINE = SOFT;
  CREATE_PROCESS (&(tattr), &(arinc_threads[1]), &(ret));
  CHECK_CODE("CREATE_PROCESS(arinc_threads[1])", ret);
  START (arinc_threads[1], &(ret));
  CHECK_CODE("START(arinc_threads[1])", ret);


  CREATE_QUEUING_PORT ("queueingin",4, sizeof (integer), DESTINATION, FIFO, &(ps4_queueingin_id), &(ret));
  CHECK_CODE("CREATE_QUEUING_PORT(queueingin)", ret);
  strcpy (tattr.NAME, "task43");
  tattr.ENTRY_POINT = task43_job;
  tattr.BASE_PRIORITY = 4;
  tattr.PERIOD = 50000000ll;
  tattr.STACK_SIZE = 8192;
  tattr.TIME_CAPACITY = 50000000ll;
  tattr.DEADLINE = SOFT;
  CREATE_PROCESS (&(tattr), &(arinc_threads[2]), &(ret));
  CHECK_CODE("CREATE_PROCESS(arinc_threads[2])", ret);
  START (arinc_threads[2], &(ret));
  CHECK_CODE("START(arinc_threads[2])", ret);


  strcpy (tattr.NAME, "task44");
  tattr.ENTRY_POINT = task44_job;
  tattr.BASE_PRIORITY = 5;
  tattr.PERIOD = 100000000ll;
  tattr.STACK_SIZE = 8192;
  tattr.TIME_CAPACITY = 50000000ll;
  tattr.DEADLINE = SOFT;
  CREATE_PROCESS (&(tattr), &(arinc_threads[3]), &(ret));
  CHECK_CODE("CREATE_PROCESS(arinc_threads[3])", ret);
  START (arinc_threads[3], &(ret));
  CHECK_CODE("START(arinc_threads[3])", ret);


  strcpy (tattr.NAME, "task45");
  tattr.ENTRY_POINT = task45_job;
  tattr.BASE_PRIORITY = 6;
  tattr.PERIOD = 200000000ll;
  tattr.STACK_SIZE = 8192;
  tattr.TIME_CAPACITY = 50000000ll;
  tattr.DEADLINE = SOFT;
  CREATE_PROCESS (&(tattr), &(arinc_threads[4]), &(ret));
  CHECK_CODE("CREATE_PROCESS(arinc_threads[4])", ret);
  START (arinc_threads[4], &(ret));
  CHECK_CODE("START(arinc_threads[4])", ret);


  SET_PARTITION_MODE (NORMAL, &(ret));
  CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
  /*  Now, we created all resources of the process. Consequently,this thread */
  /*  will not be used any more and it will be keptin a dormant state. By */
  /*  doing that, we also allow one morethread in this partition*/
  return;
}