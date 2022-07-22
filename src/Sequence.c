#include <stdlib.h>
#include <stdio.h>

#include "Sequence.h"

#define INFINITE 9223372036854775807

/**
 * Asigna memoria e inicializa una secuencia con una capacidad
 * inicial.
 * 
 * La memoria asignada debe ser liberada por el usuario.
 * 
 * @param n Capacidad inicial de la secuencia.
 * @return Apuntador a la secuencia.
 *     NULL en caso de algún error.
 */
Sequence *Sequence_new(int n) {
    Sequence *seq = malloc(sizeof(Sequence));
    if (!seq) return NULL;

    seq->arr = malloc(sizeof(int64_t) * n);
    seq->size = n;
    seq->used = 0;

    return seq;
}

/**
 * Inserta un elemento en la secuencia.
 * 
 * @param n Capacidad inicial de la secuencia.
 * @return 1 si pudo insertar, 0 en caso contrario.
 */
int Sequence_insert(Sequence *seq, int64_t el) {
    if (seq->used == seq->size) {
        /* Factor de crecimiento 1.5x */
        seq->size = (seq->size * 3) / 2 + 8;

        seq->arr = realloc(seq->arr, seq->size * sizeof(int64_t));
        if (!seq->arr) return 0;
    }

    seq->arr[seq->used++] = el;
    return 1;
}

/**
 * Encoge la secuencia al tamaño exacto del número de elementos
 * que contiene.
 * 
 * @param seq Apuntado a la secuencia.
 * @return 1 si pudo encoger, 0 en caso contrario.
 */
int Sequence_shrink(Sequence *seq) {
    if (seq->used == 0) {
        seq->size = 0;
        seq->arr = NULL;
        return 1;
    }

    seq->size = seq->used;
    seq->arr = realloc(seq->arr, seq->used * sizeof(int64_t));
    if (!seq->arr) return 0;

    return 1;
}

/**
 * Ordena la secuencia usando el algoritmo de ordenamiento por selección.
 * 
 * @param seq Apuntador a la secuencia a ordenar.
 */
void Sequence_sort(Sequence *seq) {
    int64_t temp, *arr = seq->arr;
    int n = seq->size;
    int i, j;

    /* Se ordena la secuencia con Selection Sort */
    for (i=0; i<n-1; i++) {
        for (j=i+1; j<n; j++) {
            if (arr[j] < arr[i]) {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

/**
 * @
 * 
 * @param seq1 
 * @param seq2 
 * @return Sequence* 
 */
Sequence *Sequence_merge(Sequence *seq1, Sequence *seq2) {
    int i, j, k;
    int n = seq1->size, m = seq2->size;
    int k = n + m;
    int64_t *secuencia1 = seq1->arr, *secuencia2 = seq2->arr;

    /* Crea la nueva secuencia */
    Sequence *sequence = create_sequence(k);
    if (sequence == NULL) return NULL;

    /* Mezcla las secuencias */
    for (i = 0, j = 0, k = 0; i < k; i++) {

        /* Si ambas secuencias no están vacías, revisa cuál de los dos tiene el menor */
        if (j < n && k < m) {
            if (secuencia1[j] < secuencia2[k]) {
                sequence->arr[i] = secuencia1[j];
                j++;
            } else {
                sequence->arr[i] = secuencia2[k];
                k++;
            }
        } else if (j < n) {
            /* Si la secuencia 2 ya está vacía, se copia el resto de la 1 */
            sequence->arr[i] = secuencia1[j];
            j++;
        } else {
            /* Si la secuencia 1 ya está vacía, se copia el resto de la 2 */
            sequence->arr[i] = secuencia2[k];
            k++;
        }
    }

    return sequence;
}

/**
 * 
 * 
 * @param seq_arr 
 * @param n 
 * @param path 
 * @return int 
 */
int Sequence_write_merged(Sequence **seq_arr, int n, char *path) {
    FILE *fp;
    int i, *index, index_total = 0, first = 1;

    /* Crea un arreglo para mantener los índices de cada secuencia */
    if (!(index = malloc(sizeof(int) * n))) return -1;
    for (i=0; i<n; i++) index[i] = 0;
    
    /* Busca el total de números a escribir */
    for (i = 0; i < n; i++) index_total += seq_arr[i]->size;
    
    /* Escribe en la salida, viendo el menor de los enteros de las secuencias */
    if (!(fp = fopen(path, "w"))) return -1;
    for (;;) {
        int i, min_index = 0;
        int64_t min = INFINITE;

        /* Verifica si todas las secuencias ya fueron escritas */
        if (!index_total) break;

        /* Busca el menor entero entre las secuencias */
        for (i = 0; i < n; i++) {
            if (index[i] < seq_arr[i]->size && seq_arr[i]->arr[index[i]] < min) {
                min = seq_arr[i]->arr[index[i]];
                min_index = i;
            }
        }

        /* Escribe el menor de los enteros de las secuencias */
        if (first) {
            first = 0;
            fprintf(fp, "%ld", min);
        } else {
            fprintf(fp, "\n%ld", min);
        }

        index[min_index]++;
        index_total--;
    }

    free(index);
    fclose(fp);
    return 0;
}

/**
 * Libera la memoria asignada a la secuencia pasada como argumento.
 * 
 * @param seq Apuntador a la secuencia.
 */
void Sequence_destroy(Sequence *seq) {
    free(seq->arr);
    free(seq);
}