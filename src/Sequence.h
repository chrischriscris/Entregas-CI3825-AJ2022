#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__

#include <aio.h>

/**
 * Estructura que implementa una secuencia de enteros
 * mediante un arreglo dinámico.
 * 
 * Soporta las operaciones de:
 */
typedef struct {
  int64_t *arr; /* Apuntador a arreglo interno */
  size_t used;    /* Cantidad de elementos en uso */
  size_t size;    /* Tamaño total del arreglo */
} Sequence;

Sequence *Sequence_new(size_t n);
int Sequence_insert(Sequence *seq, int64_t el);
int Sequence_shrink(Sequence *seq);
void Sequence_sort(Sequence *seq);
void Sequence_destroy(Sequence *seq);

#endif