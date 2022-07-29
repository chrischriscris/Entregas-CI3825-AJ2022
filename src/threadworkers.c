#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include "threadworkers.h"
#include "sharedvars.h"
#include "sequence.h"
#include "utils.h"

void *sorter_thread(void *arg) {
    for (;;) {
        Sequence *seq;
        char *path;
        int i;

        /* -------- Región crítica -------- */
        pthread_mutex_lock(&path_mutex);

        /* Se suspende hasta que el lector actualice la variable global */
        while (!global_path) pthread_cond_wait(&path_available, &path_mutex);

        if (global_path == (char *) -1) {
            pthread_mutex_unlock(&path_mutex);
            break;
        }

        /* Toma el path de la variable global, lo hace NULL y avisa */
        path = global_path;
        global_path = NULL;
        pthread_cond_broadcast(&path_available);

        pthread_mutex_unlock(&path_mutex);
        /* ------ Fin región crítica ------ */
    
        /* Procede a leer y ordenar los datos del archivo */
        seq = Sequence_extract_from_file(path);
        if (!seq) {
            fprintf(stderr, "Error al extraer secuencia del archivo %s\n", path);
            free(path);
            continue;
        }
        free(path);
        Sequence_sort(seq);

        /* Pasa la secuencia a un mezclador libre */

        /* -------- Región crítica -------- */
        pthread_mutex_lock(&sorter_merger_mutex);

        /* Se suspende hasta que haya un mezclador disponible */
        while (!free_mergers)
            pthread_cond_wait(&merger_available, &sorter_merger_mutex);

        /* Luego busca y pasa la secuencia al disponible, lo marca como desocupado
        y avisa */
        for (i=0; i<nm; i++) {
            if (!global_seqs[i]) {
                global_seqs[i] = seq;
                free_mergers--;
                pthread_cond_broadcast(&merger_available);
                break;
            }
        }

        pthread_mutex_unlock(&sorter_merger_mutex);
        /* ------ Fin región crítica ------ */
    }

    pthread_exit(0);
}

void *merger_thread(void *arg) {
    int n = *(int *) arg;
    Sequence *local_seq = Sequence_new(0);

    for (;;) {
        Sequence *arriving_seq;

        /* -------- Región crítica -------- */
        pthread_mutex_lock(&sorter_merger_mutex);

        /* Se anota como disponible y avisa */
        free_mergers++;
        global_seqs[n] = NULL;
        pthread_cond_broadcast(&merger_available);

        /* Mientras no haya trabajo y no sea el último paso */
        while (!global_seqs[n] && !last_step)
            pthread_cond_wait(&merger_available, &sorter_merger_mutex);

        /* Si no hay trabajo y es hora del último paso, sale del ciclo */
        if (!global_seqs[n] && last_step) {
            pthread_mutex_unlock(&sorter_merger_mutex);
            break;
        }

        /* Toma la secuencia y decrementa el número de mezcladores libres */
        arriving_seq = global_seqs[n];

        pthread_mutex_unlock(&sorter_merger_mutex);
        /* ------ Fin región crítica ------ */

        /* Mezcla la secuencia */
        if (!Sequence_merge(&local_seq, arriving_seq))
            fprintf(stderr, "Error al mezclar una secuencia\n");
    }

    /* -------- Región crítica -------- */
    pthread_mutex_lock(&merger_writer_mutex);

    /* Se suspende hasta que el escritor esté libre */
    while (writer_seq) pthread_cond_wait(&writer_available, &merger_writer_mutex);

    /* Pasa la secuencia al escritor y avisa */
    writer_seq = local_seq;
    pthread_cond_broadcast(&writer_available);

    pthread_mutex_unlock(&merger_writer_mutex);
    /* ------ Fin región crítica ------ */

    pthread_exit(0);
}

void *writer_thread(void *arg) {
    Sequence **seqs_arr = malloc(sizeof(Sequence) * nm);
    int i;

    for (i=0; i<nm; i++) {
        /* -------- Región crítica -------- */
        /* Toma la i-ésima secuencia de algún mezclador */
        pthread_mutex_lock(&merger_writer_mutex);

        while (!writer_seq) pthread_cond_wait(&writer_available, &merger_writer_mutex);

        /* Toma la secuencia y avisa */
        seqs_arr[i] = writer_seq;
        writer_seq = NULL;

        pthread_cond_signal(&writer_available);
        pthread_mutex_unlock(&merger_writer_mutex);
        /* ------ Fin región crítica ------ */
    }

    Sequence_write_merged(seqs_arr, nm, outfile);
    for (i=0; i<nm; i++) Sequence_destroy(seqs_arr[i]);
    free(seqs_arr);

    pthread_exit(0);
}