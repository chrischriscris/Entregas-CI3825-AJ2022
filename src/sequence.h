#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__

#include <aio.h>

/**
 * Estructura que implementa una secuencia de enteros
 * mediante un arreglo dinámico.
 * 
 * Soporta las operaciones de inserción, encogimiento,
 * mezcla ordenada y ordenamiento.
 * 
 * Campos:
 *     arr: Apuntador al arreglo interno.
 *     used: Cantidad de elementos del arreglo en uso.
 *     size: Tamaño total del arreglo.
 */
typedef struct {
  int64_t *arr;
  int used;
  int size;
} Sequence;

Sequence *Sequence_new(int n);
Sequence *Sequence_extract_from_file(char *path);
int Sequence_insert(Sequence *seq, int64_t el);
int Sequence_shrink(Sequence *seq);
void Sequence_sort(Sequence *seq);
int Sequence_merge(Sequence **seq1, Sequence *seq2);
int Sequence_write_merged(Sequence **seq_arr, int n, char *path);
void Sequence_print(Sequence *seq);
void Sequence_destroy(Sequence *seq);

#endif