#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "procmezclador.h"
#include "sequence.h"

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
            fprintf(stderr, "Error al mezclar claeruna secuencia\n");
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