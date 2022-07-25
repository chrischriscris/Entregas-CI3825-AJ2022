#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "procordenador.h"
#include "sequence.h"

/**
 * Hace el trabajo del ordenador.
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
void do_sorter_work(
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