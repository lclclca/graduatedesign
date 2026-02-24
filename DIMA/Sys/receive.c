#include <stdio.h>
#include "receive.h"

#define N 10000
int visited[N];
char str[N];
int len, k_rec;
int highest;

void dfs(int node) {
	int x;
    for (x = 0; x < k_rec; ++x) {
        int nei = node * 10 + x;
        if (!visited[nei]) {
            visited[nei] = 1;
            dfs(nei % highest);
            str[len++] = x + '0';
        }
    }
}

char *crackSafe(int n, int k) {
    memset(visited, 0, sizeof(visited));
    memset(str, 0, sizeof(str));
    k_rec = k, len = 0;
    visited[0] = true;
    highest = pow(10, n - 1);
    dfs(0);
    int i;
    for (i = 0; i < n; i++) {
        str[len++] = '0';
    }
    return str;
}

void user_receive (void)
{

#ifndef __DEOS__
    printf ("acc2 is initialising...\n");
#else
     myprint_str ("acc2 is initialising...\n");
#endif
    
#ifndef __DEOS__
    printf ("acc2 initialised!\n");
#else
     myprint_str ("acc2 initialised!\n");
#endif

   printf("---user_receive algorithm running successfully ---\n");
   
}
