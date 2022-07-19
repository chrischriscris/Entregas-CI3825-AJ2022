#ifndef __PROCORDENADOR_H__
#define __PROCORDENADOR_H__

void do_sorter_work(
    int n, int nm,
    int sorter_queue, int merger_queue,
    int from_reader,  int *to_merger
);

#endif