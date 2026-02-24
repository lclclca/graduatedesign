#include <stdio.h>
#include "send.h"

#define fmax(x,y)(x > y ? x : y)


void maximumGap(int* nums, int numsSize) {
    if (numsSize < 2) {
        return 0;
    }
    int exp = 1;
    int buf[numsSize];
    memset(buf, 0, sizeof(buf));
    int maxVal = -32768;
    int j;
    for (j = 0; j < numsSize; ++j) {
        maxVal = fmax(maxVal, nums[j]);
    }

    while (maxVal >= exp) {
        int cnt[10];
        memset(cnt, 0, sizeof(cnt));
        int i;
        for (i = 0; i < numsSize; i++) {
            int digit = (nums[i] / exp) % 10;
            cnt[digit]++;
        }
        for (i = 1; i < 10; i++) {
            cnt[i] += cnt[i - 1];
        }
        for (i = numsSize - 1; i >= 0; i--) {
            int digit = (nums[i] / exp) % 10;
            buf[cnt[digit] - 1] = nums[i];
            cnt[digit]--;
        }
        memcpy(nums, buf, sizeof(int) * numsSize);
        exp *= 10;
    }

    int ret = 0;

    for (j = 1; j < numsSize; j++) {
        ret = fmax(ret, nums[j] - nums[j-1]);
    }
}

void user_send(void)
{
#ifndef __DEOS__
  
#else
     myprint_str_int ("Accelerometer 3 outputs data ",pt1++);
#endif

#ifndef __DEOS__
    printf("user_send algorithm starts running \n");
#else
     myprint_str_int ("user_send algorithm failure\n");
#endif

#ifndef __DEOS__
      printf ("  |-> Accelerometer 3 sends data Over Range:->100\n");
      printf ("\n"); 
#else
     myprint_str_int ("  |-> Accelerometer 3 sends data Over Range:->90\n");
#endif


#ifndef __DEOS__
    printf ("user_send algorithm is initialising...\n");
#else
     myprint_str ("acc is initialising... ");
#endif

#ifndef __DEOS__
    printf ("user_send algorithm initialised!\n");
#else
     myprint_str ("user_send algorithm initialised! ");
#endif
 
}


