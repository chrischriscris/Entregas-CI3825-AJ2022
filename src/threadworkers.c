#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "threadworkers.h"
#include "sharedvars.h"
#include "sequence.h"

/**
 * Hilo ordenador de ordenahilo.c,
 * 
 * Toma un archivo dado por el lector para ordenar la secuencia contenida en él,
 * ordenarla y pasarla a un mezclador desocupado.
 * 
 * @param arg Apuntador al número de ordenadores.
 */
void *sorter_thread(void *arg) {
    int nm = *(int *) arg;

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

/**
 * Hilo mezclador de ordenahilo.c, mantiene una secuencia local.
 * 
 * Se marca como mezclador desocupado para recibir una secuencia ya ordenada
 * del ordenador y mezclarla de manera ordenada con su secuencia local.
 * 
 * Cuando el lector indica, pasa su secuencia al escritor.
 * 
 * @param n Apuntador al número de mezclador del hilo.
 */
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

        /* Toma la secuencia */
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

/**
 * Hilo escritor de ordenahilo.c, mantiene un arreglo de secuencias.
 * 
 * Cuando el lector le avisa, recibe las secuencias de los mezcladores y
 * al final escribe en un archivo los elementos de todas las secuencias de
 * forma ordenada.
 * 
 * @param nm Apuntador al número de mezcladores.
 */
void *writer_thread(void *arg) {
    int nm = *(int *) arg;
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