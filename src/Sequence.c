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
void Sequence_new(size_t n) {
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
        seq->size *= 3;
        seq->size /= 2;

        seq->arr = realloc(seq->arr, seq->size * sizeof(int64_t));
        if (!seq->arr) return 0;
    }

    seq->arr[seq->used++] = el;
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