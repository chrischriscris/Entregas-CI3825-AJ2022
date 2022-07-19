#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#include "procescritor.h"

/**
 * Hace el trabajo del escritor.
 * 
 * @param nm Número de mezcladores.
 * @param from_reader Extremo de lectura de pipe para obtener un mezclador
 *     listo para pasar su secuencia.
 * @param from_merger Arreglo de extremos de lectura de pipes para leer la
 *     secuencia del mezclador, de tamaño nm. Debe liberarse su memoria.
 */
void do_writer_work(int nm, int from_reader, int *from_merger) {
    int i, *arriving_seq, **seqs_arr;

    for (i=0; i<nm; i++) {
        int j, m, arriving_seq_size;

        /* Lee el mezclador asignado */
        read(from_reader, &m, sizeof(int));
        printf("Mezclador %d recibe mezclador %d\n", i, m);
        
        /* Lee el tamaño de la secuencia */
        read(from_merger[m], &arriving_seq_size, sizeof(int));

        /* Lee la secuencia */
        arriving_seq = malloc(arriving_seq_size * sizeof(int));
        for (j=0; j<arriving_seq_size; j++) {
            read(from_merger[m], &arriving_seq[j], sizeof(int));
        }

        /* Imprime la secuencia */
        printf("Escritor escribiendo secuencia de %d elementos\n", arriving_seq_size);
        sleep(2);

        /*
        for (j=0; j<size; j++) {
            printf("%d  ", secuencia[j]);
        }
        */
    }
    /* Escribe ordenado salida */
}