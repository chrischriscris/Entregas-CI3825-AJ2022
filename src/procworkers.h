#ifndef __PROCWORKERS_H__
#define __PROCWORKERS_H__

void sorter_process(
    int n, int nm,
    int sorter_queue, int merger_queue,
    int from_reader,  int *to_merger
);
void merger_process(int n, int merger_queue, int from_sorter, int to_writer);
void writer_process(int nm, int from_reader, int *from_merger, char *path);

#endif