#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "utils.h"
#include "Sequence.h"

void *safe_malloc(size_t n) {
    /* Asigna memoria */
    void *p = malloc(n);

    /* Aborta si hubo algún error */
    if (!p) {
        fprintf(stderr, "Ha habido un error al reservar memoria.\n");
        exit(1);
    }

    return p;
}

/**
 * Inicializa n pipes en un arreglo y retorna un apuntador al mismo.
 * 
 * La memoria debe ser liberada por el usuario (la de cada subarreglo y
 * la del arreglo entero).
 * Aborta el proceso en caso de error.
 * 
 * @param n: número de pipes a inicializar.
 * @return int** Apuntador a arreglo nx2 de pipes inicializados.
 */
int **initialize_pipes(int n) {
    int i;
    
    /* Asigna memoria para n punteros a entero */
    int **pipe_arr = safe_malloc(sizeof(int *) * n);

    /* Inicializa n pipes */
    for (i=0 ; i<n ; i++) {
        pipe_arr[i] = safe_malloc(sizeof(int) * 2);
        
        if (pipe(pipe_arr[i]) == -1) {
            perror("Error al crear pipe");
            exit(1);
        }
    }

    return pipe_arr;
}

/**
 * Extrae una secuencia de enteros de un archivo, el archivo solamente
 * contiene enteros separados por saltos de línea.
 * 
 * @param path String con la ruta al archivo.
 * @return Apuntador a la secuencia, su memoria debe ser liberada.
 *     NULL en caso de algún error.
 */
Sequence *extract_sequence_from_file(char *path) {
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