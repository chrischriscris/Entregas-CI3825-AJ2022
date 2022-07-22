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
#include "procescritor.h"

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[]) {
    int ns, nm, i, pid;
    char *root, *outfile;
    int sorter_count, merger_count, *to_sorter;

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
        perror("Error al crear pipe\n");
        exit(1);
    };

    if (pipe(merger_queue)) {
        perror("Error al crear pipe\n");
        exit(1);
    };

    reader_sorter = initialize_pipes(ns);
    sorter_merger = initialize_pipes(nm);

    /* ================ ORDENADORES ================ */
    for (i=0; i<ns; i++) {
        if ((pid = fork()) == 0) {
            /* Proceso hijo de Lector */
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
            close(reader_sorter[i][WRITE_END]);

            /* Empieza a trabajar */
            do_sorter_work(
                i, nm,
                sorter_queue[WRITE_END], merger_queue[READ_END],
                reader_sorter[i][READ_END], to_merger
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
                fprintf(stderr, "No se pudo crear ningún ordenador, terminando programa\n");
                exit(1);
            }
        }
    }

    /* Crean las pipes a usar por los mezcladores */
    if (pipe(reader_writer) == -1) {
        perror("Error al crear pipe\n");
        exit(1);
    }

    merger_writer = initialize_pipes(nm);

    /* ================ MEZCLADORES ================ */
    for (i=0; i<nm; i++) {
        if ((pid = fork()) == 0) {
            /* Proceso hijo de Lector */
            int j;

            /* Cierra las pipes destinadas a los demás mezcladores */
            for (j=0; j<nm; j++) {
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
            exit(0);
        } else if (pid > 0) {
            merger_count++;
        }  else {
            perror("Error al crear mezclador\n");
            if (i == ns-1 && !merger_count) {
                fprintf(stderr, "No se pudo crear ningún ordenador, terminando programa\n");
                exit(1);
            }
        }
    }

    /* ================ ESCRITOR ================ */
    if ((pid = fork()) == 0) {
        /* Proceso hijo de Lector */
        int j, *from_merger;

        /* Cierra las pipes que no usará */
        close(sorter_queue[READ_END]);
        close(sorter_queue[WRITE_END]);
        close(merger_queue[READ_END]);
        close(merger_queue[WRITE_END]);
        close(reader_writer[WRITE_END]);
        for (j=0; j<ns; j++) {
            close(reader_sorter[j][READ_END]);
            close(reader_sorter[j][WRITE_END]);
            free(reader_sorter[j]);
        }
        free(sorter_merger);
        for (j=0; j<nm; j++) {
            close(sorter_merger[j][READ_END]);
            close(sorter_merger[j][WRITE_END]);
            free(sorter_merger[j]);
        }
        free(sorter_merger);

        /* Guarda en un arreglo los extremos de lectura de las
        pipes Mezclador - Escritor */
        /* Aborta el proceso si no puede obtener memoria */
        from_merger = safe_malloc(sizeof(int) * nm);
        for (j=0; j<nm; j++) {
            from_merger[j] = merger_writer[j][READ_END];
            close(merger_writer[j][WRITE_END]);
        }

        do_writer_work(nm, reader_writer[READ_END], from_merger, outfile);
        exit(0);
    } else if (pid < 0) {
        fprintf(stderr, "No se pudo crear el escritor, terminando programa\n");
        exit(1);
    }

    /* ============= LECTOR ============= */

    /* No usa la escritura de cola de ords */
    close(sorter_queue[WRITE_END]);
    close(reader_writer[READ_END]);

    /* Solo usa la escritura de Lector - Ordenador */
    to_sorter = safe_malloc(sizeof(int) * ns);
    for (i=0; i<ns; i++) {
        to_sorter[i] = reader_sorter[i][WRITE_END];
        close(reader_sorter[i][READ_END]);
    }

    /* No usa ninguna pipe Ordenador - Mezclador */
    for (i=0; i<nm; i++) {
        close(sorter_merger[i][READ_END]);
        close(sorter_merger[i][WRITE_END]);
        free(sorter_merger[i]);

        close(merger_writer[i][READ_END]);
        close(merger_writer[i][WRITE_END]);
        free(merger_writer[i]);
    }
    free(sorter_merger);
    free(merger_writer);

    /* Recorre el árbol de directorios enviándole a los ordenadores
    los archivos */
    walk_dir_tree_proc(root, sorter_queue[READ_END], to_sorter);

    /* Espera que vayan terminando los ordenadores */
    for (i=0; i<ns; i++) {
        int n;
        close(reader_sorter[i][WRITE_END]);
        free(reader_sorter[i]);

        /* Para bloquear hasta que no vea que los ordenadores
        terminaron y se volvieron a encolar */
        read(sorter_queue[READ_END], &n, sizeof(int));
    }
    close(sorter_queue[READ_END]);
    free(reader_sorter);

    /* Indica al escritor que está listo para el último,
    encolándole mezcladores apenas estén listos */
    for (i=0; i<nm; i++) {
        int n;
        read(merger_queue[READ_END], &n, sizeof(int));

        /* Se encola el mezclador al escritor */
        write(reader_writer[WRITE_END], &n, sizeof(int));
    }
    close(merger_queue[READ_END]);
    close(reader_writer[WRITE_END]);

    for (i=0; i<nm+ns+1; i++) {
        int status;
        pid = wait(&status);

        if (status) fprintf(stderr, "Error en el proceso %d\n", pid);
    }

    printf("Terminó el padre\n");
    return 0;
}