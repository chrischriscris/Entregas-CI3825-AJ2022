/**
 * Implementación de los procesos ordenador, mezclador y escritor de ordenaproc.c.
 *
 * Autor: Christopher Gómez.
 * Fecha: 29-06-2022.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "procworkers.h"
#include "sequence.h"

/**
 * Proceso ordenador de ordenaproc.c.
 * 
 * Se encola como ordenador desocupado para recibir una archivo txt del lector,
 * ordenar la secuencia contenida en él y pasarla a un mezclador desocupado.
 * 
 * @param n Número de ordenador.
 * @param nm Número de mezcladores.
 * @param sorter_queue Extremo de escritura de pipe para encolarse como
 *     proceso desocupado.
 * @param merger_queue Extremo de lectura de pipe para recibir un mezclador
 *     desocupado.
 * @param from_reader Extremo de lectura de pipe para obtener nombres de
 *     archivos del lector.
 * @param to_merger Arreglo de extremos de lecturas de pipes para pasar la
 *     secuencia al mezclador, de tamaño nm. Debe liberarse su memoria.
 */
void sorter_process(
    int n, int nm,
    int sorter_queue, int merger_queue,
    int from_reader,  int *to_merger
) {
    int i;
    for (;;) {
        Sequence *seq;
        int path_len, n_read, m;
        char *path;

        /* Se encola como disponible */
        if (write(sorter_queue, &n, sizeof(int)) == -1) continue;

        /* Espera a que le manden un archivo, si no lee nada significa
        que cerraron los extremos y sale del ciclo */
        n_read = read(from_reader, &path_len, sizeof(int));
        if (n_read == -1) continue;
        if (!n_read) break;

        /* Guarda el path */
        path = malloc(path_len);

        /* Pasa al siguiente archivo si no puede guardar el filename */
        if (!path) continue;
        if (read(from_reader, path, path_len) != path_len) {
            free(path);
            continue;
        }

        /* Procede a leer y ordenar los datos del archivo */
        seq = Sequence_extract_from_file(path);
        if (!seq) {
            fprintf(stderr, "Error al extraer secuencia del archivo %s\n", path);
            free(path);
            continue;
        }
        free(path);
        Sequence_sort(seq);

        /* Al terminar, espera algún mezclador desocupado */
        if (!read(merger_queue, &m, sizeof(int))) {
            Sequence_destroy(seq);
            continue;
        }

        /* Encola el tamaño de la secuencia y luego la secuencia */
        if (write(to_merger[m], &seq->size, sizeof(int)) == -1) {
            Sequence_destroy(seq);
            continue;
        }

        for (i=0; i<seq->size; i++) {
            if (write(to_merger[m], seq->arr + i, sizeof(int64_t)) != sizeof(int64_t))
                continue;
        }

        Sequence_destroy(seq);
    }

    /* Al terminar su trabajo cierra los extremos de cada pipe y libera
    la memoria */
    for (i=0; i<nm; i++) close(to_merger[i]);
    free(to_merger);
    close(from_reader);
    close(sorter_queue);
    close(merger_queue);
}

/**
 * Proceso mezclador de ordenaproc.c, mantiene una secuencia local.
 * 
 * Se encola como mezclador desocupado para recibir una secuencia ya ordenada
 * del ordenador y mezclarla de manera ordenada con su secuencia local.
 * 
 * Cuando el lector indica, pasa su secuencia al escritor.
 * 
 * @param n Número de mezclador.
 * @param merger_queue Extremo de escritura de pipe para encolarse como
 *     proceso desocupado.
 * @param from_sorter Extremo de lectura de pipe para obtener la secuencias
 *     del ordenador.
 * @param to_writer Extremo de lectura de pipe para pasar la secuencia al
 *     escritor.
 */
void merger_process(int n, int merger_queue, int from_sorter,  int to_writer) {
    int i, size;
    int64_t *arr;
    Sequence *local_seq = Sequence_new(0);
    if (!local_seq) exit(1);

    for (;;) {
        Sequence *arriving_seq;

        int n_read;
        int arriving_seq_size;

        /* Se encola como disponible */
        if (write(merger_queue, &n, sizeof(int)) == -1) continue;

        /* Lee tamaño de la secuencia */
        n_read = read(from_sorter, &arriving_seq_size, sizeof(int));
        if (n_read == -1) continue;
        if (n_read == 0) break;

        arriving_seq = Sequence_new(arriving_seq_size);
        if (!arriving_seq) continue;

        /* Lee los números de la secuencia */
        for (i=0; i<arriving_seq_size; i++) {
            int64_t m;
            read(from_sorter, &m, sizeof(int64_t));
            Sequence_insert(arriving_seq, m);
        }
        
        /* Mezcla la secuencia con la local */
        if (!Sequence_merge(&local_seq, arriving_seq))
            fprintf(stderr, "Error al mezclar una secuencia\n");
    }
    close(from_sorter);
    close(merger_queue);

    /* Pasa la secuencia al escritor */
    size = local_seq->size;
    if (write(to_writer, &size, sizeof(int)) == -1) {
        Sequence_destroy(local_seq);
        exit(1);
    };

    arr = local_seq->arr;
    for (i=0; i<size; i++) {
        if (write(to_writer, &arr[i], sizeof(int64_t)) != sizeof(int64_t)) {
            continue;
        }
    }

    Sequence_destroy(local_seq);
    close(to_writer);
}

/**
 * Proceso escritor de ordenaproc.c.
 * 
 * Cuando el lector le avisa, recibe las secuencias de los mezcladores y
 * al final escribe en un archivo los elementos de todas las secuencias de
 * forma ordenada.
 * 
 * @param nm Número de mezcladores.
 * @param from_reader Extremo de lectura de pipe para obtener un mezclador
 *     listo para pasar su secuencia.
 * @param from_merger Arreglo de extremos de lectura de pipes para leer la
 *     secuencia del mezclador, de tamaño nm. Debe liberarse su memoria.
 * @param path Ruta a archivo de salida
 */
void writer_process(int nm, int from_reader, int *from_merger, char *path) {
    int i;
    Sequence **seqs_arr = malloc(sizeof(Sequence)*nm);

    for (i=0; i<nm; i++) {
        int m, j, seq_size, n_read;

        /* Lee el mezclador asignado */
        n_read = read(from_reader, &m, sizeof(int));
        if (n_read == -1) continue;

        /* Lee el tamaño de la secuencia */
        read(from_merger[m], &seq_size, sizeof(int));

        /* Lee la secuencia */
        seqs_arr[i] = Sequence_new(seq_size);
        if (!seqs_arr[i]) continue;

        for (j=0; j<seq_size; j++) {
            int64_t el;
            if (read(from_merger[m], &el, sizeof(int64_t)) != sizeof(int64_t))
                continue;
            Sequence_insert(seqs_arr[i], el);
        }
    }

    Sequence_write_merged(seqs_arr, nm, path);
    for (i=0; i<nm; i++) {
        Sequence_destroy(seqs_arr[i]);
        close(from_merger[i]);
    }
    free(seqs_arr);

    free(from_merger);
    close(from_reader);
}