#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#include "procmezclador.h"

/**
 * Hace el trabajo del mezclador.
 * 
 * @param n Número de mezclador.
 * @param merger_queue Extremo de escritura de pipe para encolarse como
 *     proceso desocupado.
 * @param from_sorter Extremo de lectura de pipe para obtener la secuencias
 *     del ordenador.
 * @param to_writer Extremo de lectura de pipe para pasar la secuencia al
 *     escritor.
 */
void do_merger_work(int n, int merger_queue, int from_sorter,  int to_writer) {
    int i;
    int local_seq_size;
    for (;;) {
        int arriving_seq_size, n_read;
        /* Se encola como disponible */
        write(merger_queue, &n, sizeof(int));

        /* Lee tamaño de la secuencia */
        n_read = read(from_sorter, &arriving_seq_size, sizeof(int));
        if (n_read == 0) break;

        printf("Mezclador %d mezclando %d enteros\n", n, arriving_seq_size);
        /* Lee los números de la secuencia */
        for (i=0; i<arriving_seq_size; i++) {
            int m;
            read(from_sorter, &m, sizeof(int));
            printf("%d  ", m);
        }
        printf("\n");
        sleep(2);
    }
    close(merger_queue);

    /* Pasa la secuencia al escritor */
    write(to_writer, &local_seq_size, sizeof(int));
    for (i=0; i<local_seq_size; i++) write(to_writer, &i, sizeof(int));
}