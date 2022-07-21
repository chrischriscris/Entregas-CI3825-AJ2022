#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#include "procmezclador.h"
#include "Sequence.h"
#include "utils.h"

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
    Sequence *local_seq = Sequence_new(0);

    for (;;) {
        Sequence *arriving_seq;

        int n_read;
        size_t arriving_seq_size;
        /* Se encola como disponible */
        write(merger_queue, &n, sizeof(int));

        /* Lee tamaño de la secuencia */
        n_read = read(from_sorter, &arriving_seq_size, sizeof(size_t));
        if (n_read == 0) break;

        printf("Mezclador %d mezclando %ld enteros\n", n, arriving_seq_size);
        arriving_seq = Sequence_new(arriving_seq_size);
        if (!arriving_seq) continue;

        /* Lee los números de la secuencia */
        for (i=0; i<arriving_seq_size; i++) {
            int64_t m;
            read(from_sorter, &m, sizeof(int64_t));
            Sequence_insert(arriving_seq, m);
            printf("%ld  ", m);
        }
        printf("\n");

        /* Combinar con la propia */
        sleep(2);
    }
    close(merger_queue);

    /* Pasa la secuencia al escritor */
    write(to_writer, &local_seq->size, sizeof(int));
    for (i=0; i<local_seq->size; i++) write(to_writer, local_seq->arr + i, sizeof(int64_t));
}