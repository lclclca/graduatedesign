#ifndef ACTIVITY_H
#define ACTIVITY_H

/*
 * Task job function declarations for partition ps1.
 * Each function is the ENTRY_POINT for its corresponding ARINC 653 process.
 */

void *task11_job(void *arg);
void *task12_job(void *arg);
void *task13_job(void *arg);
void *task14_job(void *arg);

#endif /* ACTIVITY_H */