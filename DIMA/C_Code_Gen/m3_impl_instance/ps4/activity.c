#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

#define SZ 1024
/*  Periodic task : task41  */
void* task41_job (void)
{

  RETURN_CODE_TYPE ret = NO_ERROR;
  while (1)
  {
    /* Get the IN ports values */
     
    /* Send the OUT ports values */
     

    PERIODIC_WAIT (&(ret));
    CHECK_CODE("PERIODIC_WAIT (&(ret))", ret);
  }  
}
/*  Periodic task : task42  */
extern QUEUING_PORT_ID_TYPE ps4_order_in_id;
extern QUEUING_PORT_ID_TYPE ps4_queueingout_id;
void* task42_job (void)
{

  RETURN_CODE_TYPE ret = NO_ERROR;
  char ps4_order_in_dvalue[SZ];
  char ps4_queueingout_dvalue[SZ];
  memset(ps4_order_in_dvalue, 0, 1024);
  memset(ps4_queueingout_dvalue, 0, 1024);
  MESSAGE_SIZE_TYPE ps4_order_in_length;

  while (1)
  {
    /* Get the IN ports values */

    GET_QUEUING_PORT_ID("order_in", &(ps4_order_in_id), &(ret));
    CHECK_CODE("GET_QUEUING_PORT_ID(order_in)", ret);

    RECEIVE_QUEUING_MESSAGE(ps4_order_in_id, -1ll, ps4_order_in_dvalue, &(ps4_order_in_length), &(ret));
    CHECK_CODE("RECEIVE_QUEUING_MESSAGE(ps4_order_in_id)", ret);

     
    /* Send the OUT ports values */
    GET_QUEUING_PORT_ID("queueingout", &(ps4_queueingout_id), &(ret));
    CHECK_CODE("GET_QUEUING_PORT_ID(queueingout)", ret);

    SEND_QUEUING_MESSAGE(ps4_queueingout_id, ps4_queueingout_dvalue, sizeof(integer), 5000000ll, &(ret));
    CHECK_CODE("SEND_QUEUING_MESSAGE(ps4_queueingout_id)", ret); 

    PERIODIC_WAIT (&(ret));
    CHECK_CODE("PERIODIC_WAIT (&(ret))", ret);
  }  
}
/*  Periodic task : task43  */
extern QUEUING_PORT_ID_TYPE ps4_queueingin_id;
void* task43_job (void)
{

  RETURN_CODE_TYPE ret = NO_ERROR;
  char ps4_queueingin_dvalue[SZ];
  memset(ps4_queueingin_dvalue, 0, 1024);
  MESSAGE_SIZE_TYPE ps4_queueingin_length;
  while (1)
  {
    /* Get the IN ports values */

    GET_QUEUING_PORT_ID("queueingin", &(ps4_queueingin_id), &(ret));
    CHECK_CODE("GET_QUEUING_PORT_ID(queueingin)", ret);

    RECEIVE_QUEUING_MESSAGE(ps4_queueingin_id, 5000000ll, ps4_queueingin_dvalue, &(ps4_queueingin_length), &(ret));
    CHECK_CODE("RECEIVE_QUEUING_MESSAGE(ps4_queueingin_id)", ret);
     
    /* Send the OUT ports values */
     

    PERIODIC_WAIT (&(ret));
    CHECK_CODE("PERIODIC_WAIT (&(ret))", ret);
  }  
}
/*  Periodic task : task44  */
void* task44_job (void)
{

  RETURN_CODE_TYPE ret = NO_ERROR;
  while (1)
  {
    /* Get the IN ports values */
     
    /* Send the OUT ports values */
     

    PERIODIC_WAIT (&(ret));
    CHECK_CODE("PERIODIC_WAIT (&(ret))", ret);
  }  
}
/*  Periodic task : task45  */
void* task45_job (void)
{

  RETURN_CODE_TYPE ret = NO_ERROR;
  while (1)
  {
    /* Get the IN ports values */
     
    /* Send the OUT ports values */
     

    PERIODIC_WAIT (&(ret));
    CHECK_CODE("PERIODIC_WAIT (&(ret))", ret);
  }  
}
