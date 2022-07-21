#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#include "procordenador.h"
#include "Sequence.h"
#include "utils.h"

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
        int path_size, n_read, m;
        char *path;

        /* Se encola como disponible */
        write(sorter_queue, &n, sizeof(int));

        /* Espera a que le manden un archivo, si no lee nada significa
        que cerraron los extremos y sale del ciclo */
        n_read = read(from_reader, &path_size, sizeof(int));
        if (!n_read) break;

        /* Guarda el filename */
        path = malloc(path_size+1);

        /* Pasa al siguiente archivo si no puede guardar el filename */
        if (!path) continue;
        n_read = read(from_reader, path, path_size+1);
        if (n_read != path_size+1) {
            free(path);
            continue;
        }

        /* Procede a leer y ordenar los datos del archivo */
        printf("Ordenador %d ordenando %s\n", n, path);
        seq = extract_sequence_from_file(path);
        if (!seq) {
            fprintf(stderr, "Error al leer archivo %s\n", path);
            free(path);
            continue;
        }
        free(path);
        Sequence_sort(seq);

        /* Al terminar, espera algún mezclador desoculpado */
        /* Si no pudo obtener mezclador, termina el proceso con
        estado de error */
        n_read = read(merger_queue, &m, sizeof(int));
        if (!n_read) {
            Sequence_destroy(seq);
            exit(1);
        }

        /* Encola el tamaño y la secuencia y luego la secuencia */
        write(to_merger[m], &seq->size, sizeof(size_t));
        for (i=0; i<seq->size; i++) {
            write(to_merger[m], seq->arr + i, sizeof(int64_t));
        }
        Sequence_destroy(seq);
    }

    /* Al terminar su trabajo cierra los extremos de cada pipe y libera
    la memoria */
    for (i=0; i<nm; i++) close(to_merger[i]);
    free(to_merger);
    close(merger_queue);
}