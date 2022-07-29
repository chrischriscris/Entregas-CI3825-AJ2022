#ifndef __PROCORDENADOR_H__
#define __PROCORDENADOR_H__

void sorter_process(
    int n, int nm,
    int sorter_queue, int merger_queue,
    int from_reader,  int *to_merger
);

#endif