#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <os/pos/apex/apexLib.h>
#include "gtypes.h"

/*
 * 任务入口函数声明
 * 命名规则：tasks[].name + _job
 */
void *task21_job(void *arg);
void *task22_job(void *arg);
void *task23_job(void *arg);

#endif /* ACTIVITY_H */