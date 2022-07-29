#ifndef __THREADWORKERS_H__
#define __THREADWORKERS_H__

void *sorter_thread(void *arg);
void *merger_thread(void *arg);
void *writer_thread(void *arg);

#endif