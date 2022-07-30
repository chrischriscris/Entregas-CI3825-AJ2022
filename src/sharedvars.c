#include <pthread.h>

#include "sharedvars.h"
#include "sequence.h"

/* Definición de variables que se usarán en varios hilos */
/* (Declaración en sharedvars.h) */

/* Lector - Ordenador */
char *global_path;

/* Ordenador - Mezclador */
Sequence **global_seqs;
int free_mergers = 0;

/* Flag indicando si ya se debe comenzar el último paso */
int last_step = 0;

/* Donde el mezclador pasa secuencias al escritor */
Sequence *writer_seq;
char *outfile;

/* Inicializa los mutex y variables de condición a usar */
pthread_mutex_t path_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sorter_merger_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t merger_writer_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t path_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t merger_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t writer_available = PTHREAD_COND_INITIALIZER;