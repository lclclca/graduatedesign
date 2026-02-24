#include \<os/pos/apex/apexLib.h>
#include \<stdlib.h>
#include \<stdio.h>
#include \<string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

PROCESS_ID_TYPE arinc_threads[ACoreOS653_CONFIG_NB_THREADS];
<threads:{thread|
<thread.queueInfo:{queue|QUEUING_PORT_ID_TYPE <queue.id>;};separator="\n">
<thread.sampleInfo:{sample|SAMPLING_PORT_ID_TYPE <sample.id>;}; separator="\n">
<thread.bufferInfo:{buffer|<if(buffer.direction)>extern BUFFER_ID_TYPE <buffer.id>;<endif>}; separator="\n">
<thread.blackboardInfo:{blackboard|<if(blackboard.direction)>extern BLACKBOARD_ID_TYPE <blackboard.id>;<endif>}; separator="\n">
<thread.eventInfo:{event|<if(event.direction)>extern EVENT_ID_TYPE <event.id>;<endif>}; separator="\n">
<thread.semaphoreInfo:{semaphore|<if(semaphore.direction)>extern SEMAPHORE_ID_TYPE <semaphore.id>;<endif>}; separator="\n">
}>
void appMain(void) 
{

  PROCESS_ATTRIBUTE_TYPE tattr;
  RETURN_CODE_TYPE ret = NO_ERROR;
  
 <threads:{thread|
 <thread.queueInfo:{queue|CREATE_QUEUING_PORT ("<queue.portName>",<queue.queueSize>, sizeof (<queue.dataType>), <queue.port_direction>, <queue.queuingDiscipline>, &(<queue.id>), &(ret));<\n>CHECK_CODE("CREATE_QUEUING_PORT(<queue.portName>)", ret);}; separator="\n">
 <thread.sampleInfo:{sample|CREATE_SAMPLING_PORT ("<sample.portName>", 30, <sample.port_direction>, <sample.refresh>, &(<sample.id>), &(ret));<\n>CHECK_CODE("CREATE_SAMPLING_PORT(<sample.portName>)", ret);}; separator="\n">
 <thread.bufferInfo:{buffer|<if(buffer.direction)>CREATE_BUFFER ("<buffer.portName>", sizeof (<buffer.dataType>), <buffer.maxNbMessage>, <buffer.queuingDiscipline>, &(<buffer.id>), &(ret));<\n>CHECK_CODE("CREATE_BUFFER(<buffer.portName>)", ret);<endif>}; separator="\n">
 <thread.blackboardInfo:{blackboard|<if(blackboard.direction)>CREATE_BLACKBOARD ("<blackboard.portName>", <blackboard.maxNbMessage>, &(<blackboard.id>), &(ret));<\n>CHECK_CODE("CREATE_BLACKBOARD(<blackboard.portName>)", ret);<endif>}; separator="\n">
 <thread.eventInfo:{event|<if(event.direction)>CREATE_EVENT ("<event.portName>", &(<event.id>), &(ret));<\n>CHECK_CODE("CREATE_EVENT(<event.portName>)", ret);<endif>}; separator="\n">
 <thread.semaphoreInfo:{semaphore|<if(semaphore.direction)>CREATE_SEMAPHORE ("<semaphore.portName>", <semaphore.initCounter>, <semaphore.maxCounter>, <semaphore.queuingDiscipline>, &(<semaphore.id>), &(ret));<\n>CHECK_CODE("CREATE_SEMAPHORE(<semaphore.portName>)", ret);<endif>}; separator="\n">
 strcpy (tattr.NAME, "<thread.threadName>");
 tattr.ENTRY_POINT = <thread.entryPoint>;
 tattr.BASE_PRIORITY = <thread.basePriority>;
 tattr.PERIOD = <thread.period>ll;
 tattr.STACK_SIZE = <thread.stackSize>;
 tattr.TIME_CAPACITY = <thread.timeCapacity>ll;
 tattr.DEADLINE = <thread.deadlineType>;
 CREATE_PROCESS (&(tattr), &(arinc_threads[<i0>]), &(ret));
 CHECK_CODE("CREATE_PROCESS(arinc_threads[<i0>])", ret);
 START (arinc_threads[<i0>], &(ret));
 CHECK_CODE("START(arinc_threads[<i0>])", ret);
 <\n>}>
  SET_PARTITION_MODE (NORMAL, &(ret));
  CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
  /*  Now, we created all resources of the process. Consequently,this thread */
  /*  will not be used any more and it will be keptin a dormant state. By */
  /*  doing that, we also allow one morethread in this partition*/
  return;
}