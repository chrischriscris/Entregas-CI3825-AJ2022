#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#include "procordenador.h"

#define READ_END 0
#define WRITE_END 1
#define NUM_ORDS 9
#define NUM_MEZC 3
#define NUM_FILES 31

int main(int argc, char *argv[]) {
    int ord_lec[2];           /* Ordenadores disponibles */
    int lec_ord[NUM_ORDS][2]; /* Pipes de lector a ordenador */

    int mezc_ord[2];            /* Mezcladores disponibles */
    int ord_mezc[NUM_MEZC][2];  /* Pipes de ordenador a mezclador */
    
    int mezc_esc[NUM_MEZC][2];  /* Pipes de mezclador a escritor */
    int lec_esc[2];             /* Pipes de lector a escritor */

    int pid, i;

    for (i=0 ; i<NUM_ORDS ; i++) pipe(lec_ord[i]);
    for (i=0 ; i<NUM_MEZC ; i++) pipe(ord_mezc[i]);
    pipe(ord_lec);
    pipe(mezc_ord);

    /* ================ ORDENADORES ================ */
    for (i=0; i<NUM_ORDS; i++) {
        if ((pid = fork()) == 0) {
            int j;
            /* Recibe string de archivo */
            close(lec_ord[i][WRITE_END]);

            /* Se encola */
            close(ord_lec[READ_END]);
            close(mezc_ord[WRITE_END]);

            /* Cierra extremos de otras pipes */
            for (j=0; j<NUM_ORDS; j++) {
                if (j!=i) {
                    close(lec_ord[j][READ_END]);
                    close(lec_ord[j][WRITE_END]);
                }
            }

            /* Cierra extremo de lectura */
            for (j=0; j<NUM_MEZC; j++) 
                close(ord_mezc[j][READ_END]);

            while (1) {
                int n, r, m;
                char *filename;
                write(ord_lec[WRITE_END], &i, sizeof(int));

                /* Lee el tamaño del filename */
                r = read(lec_ord[i][READ_END], &n, sizeof(int));
                if (r == 0) break;
                
                /* Guarda el filename */
                filename = malloc(n+1);
                read(lec_ord[i][READ_END], filename, n+1);

                /* Lectura de datos */
                /* printf("Ordenador %d ordenando %s\n", i, filename); */

                /* Toma un mezclador disponible */
                read(mezc_ord[READ_END], &m, sizeof(int));

                /* Encola el tamaño y la secuencia */
                int size = 5*i + 10;
                write(ord_mezc[m][WRITE_END], &size, sizeof(int));
                for (j=0; j<size; j++) {
                    write(ord_mezc[m][WRITE_END], &j, sizeof(int));
                }
            }

            for (j=0; j<NUM_MEZC; j++) {
                close(ord_mezc[j][WRITE_END]);
            }
            close(mezc_ord[READ_END]);

            exit(0);
        } else if (pid > 0) {
            
        } else {
            perror("Error: fork");
            exit(1);
        }
    }

    for (i=0 ; i<NUM_MEZC ; i++) pipe(mezc_esc[i]);
    pipe(lec_esc);

    /* ================ MEZCLADORES ================ */
    for (i=0; i<NUM_MEZC; i++) {
        if ((pid = fork()) == 0) {
            int j;
            /* Cierra los pipes de lector-ordenador */
            close(ord_lec[READ_END]);
            close(ord_lec[WRITE_END]);

            for (j=0; j<NUM_ORDS; j++) {
                close(lec_ord[j][READ_END]);
                close(lec_ord[j][WRITE_END]);
            }

            close(ord_mezc[i][WRITE_END]);
            close(mezc_ord[READ_END]);

            for (j=0; j<NUM_MEZC; j++) {
                if (j!=i) {
                    close(ord_mezc[j][READ_END]);
                    close(ord_mezc[j][WRITE_END]);
                }
            }

            while (1) {
                int n, r;
                /* Encolar */
                write(mezc_ord[WRITE_END], &i, sizeof(int));

                /* Lee tamaño de la secuencia */
                r = read(ord_mezc[i][READ_END], &n, sizeof(int));
                if (r == 0) break;

                /* printf("Mezclador %d mezclando %d\n", i, n); */
                /* Lee los números de la secuencia */
                for (j=0; j<n;j++) {
                    int m;
                    read(ord_mezc[i][READ_END], &m, sizeof(int));
                    /* printf("%d  ", m); */
                }

                /* Mezcla */
            }
            close(mezc_ord[WRITE_END]);

            int size = 5*i + 10;
            write(mezc_esc[i][WRITE_END], &size, sizeof(int));
            for (j=0; j<size; j++) {
                write(mezc_esc[i][WRITE_END], &j, sizeof(int));
            }

            printf("Mezclador %d terminado\n", i);
            exit(0);
        } 
    }

    /* ================ ESCRITOR ================ */
    if ((pid = fork()) == 0) {
        int j, n = 0;

        /* Cierra los pipes de lector-ordenador */
        close(ord_lec[READ_END]);
        close(ord_lec[WRITE_END]);
        for (j=0; j<NUM_ORDS; j++) {
            close(lec_ord[j][READ_END]);
            close(lec_ord[j][WRITE_END]);
        }
        close(mezc_ord[READ_END]);
        close(mezc_ord[WRITE_END]);
        for (j=0; j<NUM_MEZC; j++) {
            close(ord_mezc[j][READ_END]);
            close(ord_mezc[j][WRITE_END]);
        }

        /* Cierra los pipes de mezclador-escritor */
        close(lec_esc[WRITE_END]);
        for (j=0; j<NUM_MEZC; j++) {
            close(mezc_esc[j][WRITE_END]);
        }

        while (1) {
            if (n == NUM_MEZC) break;

            /* Lee el mezclador asignado */
            int m;
            read(lec_esc[READ_END], &m, sizeof(int));
            printf("Al escritor %d le dieron mezclador %d\n", n, m);
            
            /* Lee el tamaño de la secuencia */
            int size;
            read(mezc_esc[m][READ_END], &size, sizeof(int));

            /* Lee la secuencia */
            int *secuencia = malloc(size * sizeof(int));
            for (j=0; j<size; j++) {
                read(mezc_esc[m][READ_END], &secuencia[j], sizeof(int));
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
    close(ord_lec[WRITE_END]);
    close(lec_esc[READ_END]);

    for (i=0; i<NUM_MEZC; i++) {
        close(ord_mezc[i][READ_END]);
        close(ord_mezc[i][WRITE_END]);
    }

    /* No usa la lectura de nadie */
    for (i=0; i<NUM_ORDS; i++)
        close(lec_ord[i][READ_END]);

    /* Pasarle a los ordenadores los archivos */
    /* DENTRO DE TRAVERSE_DIR */
    for (i=0; i<NUM_FILES; i++) {
        char filename[100];
        int n, m;
        sprintf(filename, "file%d.txt", i);

        read(ord_lec[READ_END], &n, sizeof(int));
        m = strlen(filename);
        write(lec_ord[n][WRITE_END], &m, sizeof(int));
        write(lec_ord[n][WRITE_END], filename, m+1);
    }

    /* Cierra los extremos de los pipes */
    for (i=0; i<NUM_ORDS; i++)
        close(lec_ord[i][WRITE_END]);

    for (i=0; i<NUM_ORDS; i++) {
        int n;
        read(ord_lec[READ_END], &n, sizeof(int));
        close(lec_ord[n][READ_END]);
    }
    close(ord_lec[READ_END]);

    /* Cierra los extremos de los pipes */
    for (i=0; i<NUM_MEZC; i++)
        close(ord_mezc[i][WRITE_END]);

    for (i=0; i<NUM_MEZC; i++) {
        int n;
        read(mezc_ord[READ_END], &n, sizeof(int));

        /* Cierra pipe */
        close(ord_mezc[n][READ_END]);

        /* Se encola el mezclador al escritor */
        write(lec_esc[WRITE_END], &n, sizeof(int));
    }

    close(mezc_ord[READ_END]);
    close(lec_esc[WRITE_END]);

    for (i=0; i<NUM_MEZC+NUM_ORDS+1;i++) wait(NULL);
    printf("Terminó el padre\n");
    return 0;
}