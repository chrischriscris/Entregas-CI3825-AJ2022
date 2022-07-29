#ifndef __SHAREDVARS_H__
#define __SHAREDVARS_H__

#include <pthread.h>

#include "sequence.h"

/* Declaración de variables que se usarán en varios hilos */

/* Lector - Ordenador */
extern char *global_path;

/* Ordenador - Mezclador */
extern Sequence **global_seqs;
extern int free_mergers;

/* QUITAR */
extern int nm;
extern char *outfile;

/* Flag indicando si ya se debe comenzar el último paso */
extern int last_step;

/* Donde el mezclador pasa secuencias al escritor */
extern Sequence *writer_seq;

/* Mutex y variables de condición */
extern pthread_mutex_t path_mutex;
extern pthread_mutex_t sorter_merger_mutex;
extern pthread_mutex_t merger_writer_mutex;

extern pthread_cond_t path_available;
extern pthread_cond_t merger_available;
extern pthread_cond_t writer_available;

#endif