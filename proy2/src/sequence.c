/**
 * Implementación de secuencia como arreglo dinámico.
 * Soporta las operaciones de creación, inserción, encogimiento, mezcla
 * y eliminación.
 *
 * Autor: Christopher Gómez.
 * Fecha: 29-06-2022.
 */

#include <stdlib.h>
#include <stdio.h>

#include "sequence.h"

#define LONG_MAX 9223372036854775807

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
 * Extrae una secuencia de enteros de un archivo, el archivo solamente
 * contiene enteros separados por saltos de línea.
 * 
 * @param path String con la ruta al archivo.
 * @return Apuntador a la secuencia, su memoria debe ser liberada.
 *     NULL en caso de algún error.
 */
Sequence *Sequence_extract_from_file(char *path) {
    Sequence *seq;
    int64_t n;
    FILE *fp;

    seq = Sequence_new(32);
    if (!seq) return NULL;

    fp = fopen(path, "r");
    if (!fp) {
        free(seq);
        return NULL;
    }

    while (fscanf(fp, "%ld", &n) != EOF) {
        if (!Sequence_insert(seq, n)) {
            fprintf(stderr, "Error al insertar elemento %ld en la secuencia.\n", n);
            continue;
        }
    }
    
    if (!Sequence_shrink(seq)) {
        fprintf(stderr, "Error manipulando la secuencia.\n");
        free(seq);
        return NULL;
    }

    fclose(fp);
    return seq;
}

/**
 * Inserta un elemento en la secuencia.
 * 
 * @param n Capacidad inicial de la secuencia.
 * @return 1 si la operación fue exitosa.
 *     0 en caso contrario.
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
 * @return 1 si pudo encoger
 *     0 en caso contrario.
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
int Sequence_merge(Sequence **seq1, Sequence *seq2) {
    int64_t *arr1 = (*seq1)->arr, *arr2 = seq2->arr;
    int n = (*seq1)->size, m = seq2->size;
    int i, j;

    /* Crea la nueva secuencia */
    Sequence *merged_seq = Sequence_new(n+m);
    if (!merged_seq) return 0;

    /* Mezcla las secuencias, se usa el hecho de que ambas están ordenadas */
    for (i=0, j=0; i<n && j<m;) {
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
 * De un arreglo de n secuencias ordenadas, escribe en un archivo
 * todos sus elementos mezclados en orden ascendente.
 * 
 * @param seq_arr Arreglo de apuntadores a secuencias.
 * @param n 
 * @param path 
 * @return 1 si la operación fue exitosa
 *     0 en caso contrario.
 */
int Sequence_write_merged(Sequence **seq_arr, int n, char *path) {
    FILE *fp;
    int i, n_total = 0;
    int *index;

    /* Mantiene en un arreglo apuntadores al arreglo interno de
    cada secuencia */
    if (!(index = malloc(sizeof(int) * n))) return 0;
    for (i=0; i<n; i++) {
        n_total += seq_arr[i]->size;
        index[i] = 0;
    }

    /* Por cada iteración busca la menor cabeza y la escribe */
    if (!(fp = fopen(path, "w"))) return 0;
    for (i=0; i<n_total; i++) {
        int j;
        int64_t min = LONG_MAX, min_i;

        /* Halla la menor cabeza de arreglo */
        for (j=0; j<n; j++) {
            int k = index[j];

            /* Si no se ha llegado al final del j-ésimo arreglo */
            if (k < seq_arr[j]->size) {
                /* Si la cabeza es menor que el mínimo actual, se actualiza */
                if (seq_arr[j]->arr[k] < min) {
                    min = seq_arr[j]->arr[k];
                    min_i = j;
                }
            }
        }
        index[min_i]++;

        /* Escribe el menor hallado */
        fprintf(fp, i == 0 ? "%ld" : "\n%ld", min);
    }
    fclose(fp);

    free(index);
    return 1;
}

/**
 * Imprime los elementos de la secuencia.
 * 
 * @param seq Apuntador a la secuencia.
 */
void Sequence_print(Sequence *seq) {
    int i;
    printf("[");
    for (i=0; i<seq->used; i++) {
        printf(i == 0 ? "%ld" : ", %ld", seq->arr[i]);
    }
    printf("]\n");
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