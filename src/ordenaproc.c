#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "procordenador.h"
#include "procmezclador.h"

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[]) {
    int ns, nm, i, pid;
    char *root, *outfile;
    int sorter_count, merger_count;

    /* Mecanismos de comunicación */

    /* Ordenadores - Lector */
    int sorter_queue[2]; 
    int **reader_sorter;
    
    /* Mezcladores - Ordenadores */
    int merger_queue[2];
    int **sorter_merger;

    /* Mezcladores - Escritor */
    int **merger_writer;

    /* Lector - Escritor */
    int reader_writer[2];

    /* Mezcladores disponibles */
    /* Valida los argumentos de línea de comandos */
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <num Ordenadores> <num Mezcladores> <raiz> <archivo salida>\n", argv[0]);
        return 1;
    }

    /* Obtiene los argumentos */
    ns = atoi(argv[1]);
    nm = atoi(argv[2]);
    root = argv[3];
    outfile = argv[4];

    /* Crea los pipes a usar por los ordenadores */
    if (pipe(sorter_queue) == -1) {
        perror("Error al crear pipe");
        exit(1);
    };

    if (pipe(merger_queue)) {
        perror("Error al crear pipe");
        exit(1);
    };

    reader_sorter = initialize_pipes(ns);
    sorter_merger = initialize_pipes(nm);

    /* ================ ORDENADORES ================ */
    for (i=0; i<ns; i++) {
        if ((pid = fork()) == 0) {
            /* Proceso hijo de Lector */
            int *from_reader = *(reader_sorter + i);
            int j, *to_merger;

            /* Cierra las pipes destinadas a los demás ordenadores */
            for (j=0; j<ns; j++) {
                if (i==j) continue;
                close(reader_sorter[j][READ_END]);
                close(reader_sorter[j][WRITE_END]);
                free(reader_sorter[j]);
            }

            /* Guarda en un arreglo los extremos de escritura de las
            pipes Ordenador - Mezclador */
            /* Aborta el proceso si no puede obtener memoria */
            to_merger = safe_malloc(sizeof(int) * nm);
            for (j=0; j<nm; j++) {
                to_merger[j] = sorter_merger[j][WRITE_END];
                close(sorter_merger[j][READ_END]);
            }

            /* Cierra los extremos que no voy a usar de las demás pipes */
            close(sorter_queue[READ_END]);
            close(merger_queue[WRITE_END]);
            close(from_reader[WRITE_END]);

            /* Empieza a trabajar */
            do_sorter_work(
                i, nm,
                sorter_queue[WRITE_END], merger_queue[READ_END],
                from_reader[READ_END], to_merger
            );

            free(reader_sorter);
            exit(0);
        } else if (pid > 0) {
            sorter_count++;
        } else {
            perror("Error al crear lector");
            /* Termina el proceso principal solo si no hay por lo menos
            un lector al terminar de crearlos todos */
            if (i == ns-1 && !sorter_count) {
                fprintf("No se pudo crear ningún lector, terminando programa\n");
                exit(1);
            }
        }
    }
    
    /* Crean las pipes a usar por los mezcladores */
    if (pipe(reader_writer) == -1) {
        perror("Error al crear pipe");
        exit(1);
    }

    merger_writer = initialize_pipes(nm);

    /* ================ MEZCLADORES ================ */
    for (i=0; i<nm; i++) {
        if ((pid = fork()) == 0) {
            /* Proceso hijo de Lector */
            int *from_sorter = *(sorter_merger + i);
            int j, *to_writer;

            /* Cierra las pipes destinadas a los demás mezcladores */
            for (j=0; j<ns; j++) {
                if (i==j) continue;
                close(sorter_merger[j][READ_END]);
                close(sorter_merger[j][WRITE_END]);
                free(sorter_merger[j]);

                close(merger_writer[j][READ_END]);
                close(merger_writer[j][WRITE_END]);
                free(merger_writer[j]);
            }

            /* Cierra los extremos que no voy a usar de las demás pipes */
            close(sorter_queue[READ_END]);
            close(sorter_queue[WRITE_END]);
            close(merger_queue[READ_END]);
            close(sorter_merger[i][WRITE_END]);
            close(merger_writer[i][READ_END]);
            for (j=0; j<ns; j++) {
                close(reader_sorter[j][READ_END]);
                close(reader_sorter[j][WRITE_END]);
                free(reader_sorter[j]);
            }
            free(reader_sorter);

            /* Empieza a trabajar */
            do_merger_work(
                i,
                merger_queue[WRITE_END],
                sorter_merger[i][READ_END],
                merger_writer[i][WRITE_END]
            );

            free(sorter_merger);
            free(merger_writer);
            printf("Ordenador terminado\n");
            exit(0);
        } else if (pid > 0) {
            merger_count++;
        }  else {
            perror("Error al crear mezclador");
            if (i == ns-1 && !merger_count) {
                fprintf("No se pudo crear ningún escritor, terminando programa\n");
                exit(1);
            }
        }
    }

    /* ================ ESCRITOR ================ */
    if ((pid = fork()) == 0) {
            /* Proceso hijo de Lector */
            int *from_sorter = *(sorter_merger + i);
            int j, *to_writer;

            /* Cierra las pipes que no usará */
            close(sorter_queue[READ_END]);
            close(sorter_queue[WRITE_END]);
            close(merger_queue[READ_END]);
            close(merger_queue[WRITE_END]);
            for (j=0; j<ns; j++) {
                close(reader_sorter[j][READ_END]);
                close(reader_sorter[j][WRITE_END]);
                free(reader_sorter[j]);
            }
            free(reader_sorter);

            /* Cierra los extremos de las pipes que no usará */

        int j, n = 0;

        /* Cierra los pipes de lector-ordenador */
        close(sorter_queue[READ_END]);
        close(sorter_queue[WRITE_END]);
        for (j=0; j<ns; j++) {
            close(reader_sorter[j][READ_END]);
            close(reader_sorter[j][WRITE_END]);
        }
        close(merger_queue[READ_END]);
        close(merger_queue[WRITE_END]);
        for (j=0; j<nm; j++) {
            close(sorter_merger[j][READ_END]);
            close(sorter_merger[j][WRITE_END]);
        }

        /* Cierra los pipes de mezclador-escritor */
        close(reader_writer[WRITE_END]);
        for (j=0; j<nm; j++) {
            close(merger_writer[j][WRITE_END]);
        }

        while (1) {
            if (n == nm) break;

            /* Lee el mezclador asignado */
            int m;
            read(reader_writer[READ_END], &m, sizeof(int));
            printf("Al escritor %d le dieron mezclador %d\n", n, m);
            
            /* Lee el tamaño de la secuencia */
            int size;
            read(merger_writer[m][READ_END], &size, sizeof(int));

            /* Lee la secuencia */
            int *secuencia = malloc(size * sizeof(int));
            for (j=0; j<size; j++) {
                read(merger_writer[m][READ_END], &secuencia[j], sizeof(int));
            }

            /* Imprime la secuencia */
            printf("Escritor escribiendo secuencia de %d elementos\n", size);
            /* for (j=0; j<size; j++) {
                printf("%d  ", secuencia[j]);
            }
            */

            n++;
        }
        /* Escribe ordenado salida */
        printf("A punto de morir\n");
        exit(0);
    }

    /* ============= LECTOR ============= */
    /* No usa la escritura de cola de ords */
    
    close(sorter_queue[WRITE_END]);
    close(reader_writer[READ_END]);

    for (i=0; i<nm; i++) {
        close(sorter_merger[i][READ_END]);
        close(sorter_merger[i][WRITE_END]);
    }

    /* No usa la lectura de nadie */
    for (i=0; i<ns; i++)
        close(reader_sorter[i][READ_END]);

    /* Pasarle a los ordenadores los archivos */
    /* DENTRO DE TRAVERSE_DIR */
    for (i=0; i<10; i++) {
        char filename[100];
        int n, m;
        sprintf(filename, "file%d.txt", i);

        read(sorter_queue[READ_END], &n, sizeof(int));
        m = strlen(filename);
        write(reader_sorter[n][WRITE_END], &m, sizeof(int));
        write(reader_sorter[n][WRITE_END], filename, m+1);
    }

    /* Cierra los extremos de los pipes */
    for (i=0; i<ns; i++)
        close(reader_sorter[i][WRITE_END]);

    for (i=0; i<ns; i++) {
        int n;
        read(sorter_queue[READ_END], &n, sizeof(int));
        close(reader_sorter[n][READ_END]);
    }
    close(sorter_queue[READ_END]);

    /* Cierra los extremos de los pipes */
    for (i=0; i<nm; i++)
        close(sorter_merger[i][WRITE_END]);

    for (i=0; i<nm; i++) {
        int n;
        read(merger_queue[READ_END], &n, sizeof(int));

        /* Cierra pipe */
        close(sorter_merger[n][READ_END]);

        /* Se encola el mezclador al escritor */
        write(reader_writer[WRITE_END], &n, sizeof(int));
    }

    close(merger_queue[READ_END]);
    close(reader_writer[WRITE_END]);

    for (i=0; i<nm+ns+1;i++) wait(NULL);
    printf("Terminó el padre\n");
    return 0;
}