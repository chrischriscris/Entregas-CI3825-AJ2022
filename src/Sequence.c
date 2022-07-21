#include <stdlib.h>

#include "Sequence.h"

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
Sequence *Sequence_new(size_t n) {
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
 * Libera la memoria asignada a la secuencia pasada como argumento.
 * 
 * @param seq Apuntador a la secuencia.
 */
void Sequence_destroy(Sequence *seq) {
    free(seq->arr);
    free(seq);
}