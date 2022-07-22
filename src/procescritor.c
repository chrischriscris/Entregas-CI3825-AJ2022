#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#include "procescritor.h"
#include "Sequence.h"

/**
 * Hace el trabajo del escritor.
 * 
 * @param nm Número de mezcladores.
 * @param from_reader Extremo de lectura de pipe para obtener un mezclador
 *     listo para pasar su secuencia.
 * @param from_merger Arreglo de extremos de lectura de pipes para leer la
 *     secuencia del mezclador, de tamaño nm. Debe liberarse su memoria.
 * @param path Ruta a archivo de salida
 */
void do_writer_work(int nm, int from_reader, int *from_merger, char *path) {
    int i;
    Sequence **seqs_arr = malloc(sizeof(Sequence)*nm);

    for (i=0; i<nm; i++) {
        int m, j, seq_size, n_read;

        /* Lee el mezclador asignado */
        n_read = read(from_reader, &m, sizeof(int));
        if (n_read == -1) continue;
        printf("Mezclador %d recibe mezclador %d\n", i, m);
        
        /* Lee el tamaño de la secuencia */
        read(from_merger[m], &seq_size, sizeof(int));

        /* Lee la secuencia */
        seqs_arr[i] = Sequence_new(seq_size);
        if (!seqs_arr[i]) continue;

        for (j=0; j<seq_size; j++) {
            if (read(from_merger[m], &seqs_arr[j]->arr + j, sizeof(int64_t)) != sizeof(int64_t))
                continue;
        }
    }

    Sequence_write_merged(seqs_arr, nm, path);
    for (i=0; i<nm; i++) {
        Sequence_destroy(seqs_arr[i]);
        close(from_merger[i]);
    }
    free(seqs_arr);

    close(from_reader);
}