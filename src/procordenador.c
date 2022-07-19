#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#include "procordenador.h"

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
 *     secuencia al mezclador, de tamaño m. Debe liberarse su memoria.
 */
void do_sorter_work(
    int n, int nm,
    int sorter_queue, int merger_queue,
    int from_reader,  int *to_merger
) {

    int i;
    for (;;) {
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

        /* Procede a leer los datos del archivo */
        printf("Ordenador %d ordenando %s\n", n, path);
        sleep(2);

        /* Al terminar, espera algún mezclador desoculpado */
        /* Si no pudo obtener mezclador, termina el proceso con
        estado de error */
        n_read = read(merger_queue, &m, sizeof(int));
        if (!n_read) exit(1);

        /* Encola el tamaño y la secuencia */
        int size = 5*i + 10;
        write(to_merger[m], &size, sizeof(int));
        for (i=0; i<size; i++) {
            write(to_merger[m], &i, sizeof(int));
        }
    }

    /* Al terminar su trabajo cierra los extremos de cada pipe y libera
    la memoria */
    for (i=0; i<nm; i++) close(to_merger[i]);
    free(to_merger);
    close(merger_queue);
}