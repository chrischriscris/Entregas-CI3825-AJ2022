#include <stdlib.h>
#include <stdio.h>

#include "Sequence.h"

#define INFINITY 9223372036854775807

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
    if (!seq->used) {
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
 * Mezcla de forma ordenada dos secuencias, liberando la memoria
 * de la segunda.
 * 
 * @param seq1 Referencia al apuntador a la primera secuencia (out).
 * @param seq2 Apuntador a la segunda secuencia.
 * @return 1 si la operación fue exitosa.
 *     0 en caso contrario.
 */
Sequence *Sequence_merge(Sequence **seq1, Sequence *seq2) {
    int *arr1 = (*seq1)->arr, *arr2 = seq2->arr;
    int     n = (*seq1)->size,    m = seq2->size;
    int i, j;

    /* Crea la nueva secuencia */
    Sequence *merged_seq = Sequence_new(n+m);
    if (!merged_seq) return 0;

    /* Mezcla las secuencias, se usa el hecho de que ambas están ordenadas */
    for (i=0, j=0; i<n && j<m; i++) {
        if (arr1[i] < arr2[j]) Sequence_insert(merged_seq, arr1[i++]);
        else Sequence_insert(merged_seq, arr2[j++]);
    }

    /* Se copia el resto de la secuencia restante */
    while (i<n) Sequence_insert(merged_seq, arr1[i++]);
    while (j<m) Sequence_insert(merged_seq, arr2[j++]);

    /* Libera la memoria de las secuencias anteriores y modifica el
    apuntador para que apunte a la secuencia de retorno */
    Sequence_destroy(*seq1);
    Sequence_destroy(seq2);
    *seq1 = merged_seq;

    return 1;
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
        int64_t min = INFINITY;

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