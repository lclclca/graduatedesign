#ifndef ACTIVITY_H
#define ACTIVITY_H

/*
 * Task job function declarations for partition ps2.
 * Each function corresponds to a periodic ARINC 653 process.
 */

void *task21_job(void *arg);
void *task22_job(void *arg);
void *task23_job(void *arg);

#endif /* ACTIVITY_H */