#include <stdio.h>
#include "calculate.h"

int acc1_data[20]={0,1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1,0,0};
int acc2_data[20]={9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9,0};
int acc3_data[20]={9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9,0};
int acc4_data[20]={9,1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1,0,0};
int acc5_data[20]={9,8,7,6,5,4,3,2,1,0,1,2,3,40,5,60,7,80,9,0};
int acc6_data[20]={9,8,7,60,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9,0};

int acc1_data_temp=0;
int acc2_data_temp=0;
int acc3_data_temp=0;
int acc4_data_temp=0;
int acc5_data_temp=0;
int acc6_data_temp=0;

int t1=0;
int t2=0;
int t3=0;
int t4=0;
int t5=0;
int t6=0;
int t7=0;
int local_health_status[6]={0};

int acc1_error_msg_code=0;
int acc2_error_msg_code=0;
int acc3_error_msg_code=0;
int acc4_error_msg_code=0;
int acc5_error_msg_code=0;
int acc6_error_msg_code=0;

int acc1_error_action_code=0;
int acc2_error_action_code=0;
int acc3_error_action_code=0;
int acc4_error_action_code=0;
int acc5_error_action_code=0;
int acc6_error_action_code=0;

int acc1_data_store[10]={0};
int acc2_data_store[10]={0};
int acc3_data_store[10]={0};
int acc4_data_store[10]={0};
int acc5_data_store[10]={0};
int acc6_data_store[10]={0};

int acc1_validated_data=0;
int acc2_validated_data=0;
int acc3_validated_data=0;
int acc4_validated_data=0;
int acc5_validated_data=0;
int acc6_validated_data=0;

int acc1_x=0;
int acc2_x=0;
int acc3_x=0;
int acc4_x=0;
int acc5_x=0;
int acc6_x=0;

int x5=0;
int x6=0;

int Null_Point=0;
int pt1=1;
int pt2=7;
void calculate (int event_in,int* p)
{
#ifndef __DEOS__

 int event = 13;
#else
 myprint_str_int ("Accelerometer 1 outputs data ",pt1++);
#endif
   
switch (event) {
   case 13:
      local_health_status[0]=2; 
#ifndef __DEOS__
      printf ("acc1 receive reset command %d\n", event_in);
      printf ("\n");
#else
     myprint_str_int ("acc1 receive reset command ", event_in);
#endif
      break;
   default: ;
}

    acc1_data_temp=acc1_data[t1]; 
    *p = acc1_data_temp;
    t1++;
    if (t1>17) {
     t1=0;
    }
    if (acc1_data_temp>9) {
#ifndef __DEOS__

#else
     myprint_str_int ("  |-> Accelerometer 1 sends data Over Range:-> ", acc1_data_temp);
#endif
    }
  switch (local_health_status[0]) {

   case 1:

    break;
   case 2:
#ifndef __DEOS__
    printf ("acc1 is initialising...\n");
#else
     myprint_str ("acc1 is initialising...");
#endif
    t1=0;
    local_health_status[0]=0;
#ifndef __DEOS__
    printf ("acc1 initialised!\n");
#else
     myprint_str ("acc1 initialised!");
#endif
    break;
   default: ;
   }
   
}