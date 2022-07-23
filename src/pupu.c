#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_ORDS 3
#define NUM_MEZC 4
#define NUM_FILES 15

char *filename_pointer;
int **seq_pointer;
int a = 0;

sem_t sem_ords;
sem_t sem_mezc_count;

void *ordenador(void *arg);
void *mezclador(void *arg);

int main(int argc, char *argv[]) {
    pthread_t ords_ids[NUM_ORDS];
    pthread_t mezc_ids[NUM_MEZC];
    pthread_t escr_id;

    int aux, i;

    pthread_attr_t attr;
    void *res;

    /* ======= Creación de cosas ======= */

    sem_init(&sem_ords, 0, 1);
    for (i=0; i<NUM_ORDS; i++) {
        if (pthread_create(&ords_ids[i], NULL, ordenador, NULL)) {
            printf("Error creando hilo %d\n", (int) ords_ids[i]);
            exit(1);
        }
    }

    sem_init(&sem_mezc_count, 0, NUM_MEZC);
    for (i=0; i<NUM_MEZC; i++) {
        int n = i;
        if (pthread_create(&mezc_ids[i], NULL, mezclador, &n)) {
            printf("Error creando hilo %d\n", (int) mezc_ids[i]);
            exit(1);
        }
    }

    /* ======= Lector ======= */
    for (int i=0; i<NUM_FILES; i++) {
        char *filename = malloc(100);
        int n, m;
        sprintf(filename, "file%d.txt", i);

        filename_pointer = filename;

        /* Espera que un ordenador agarre el filename */
        while (filename_pointer);
        sleep(1);
    }

    filename_pointer = (char *) -1;

    /* ======= Espera a todos los demás ======= */

    for (i=0; i<NUM_ORDS; i++) {
        if (pthread_join(ords_ids[i], &res))
            printf("Error haciendo join\n");

        printf("Joined with thread %ld; returned value was %s\n", ords_ids[i], (char *) res);
    }

    printf("Finished\n");
}

void *ordenador(void *arg) {
    int *seq = malloc(sizeof(int) * 10);
    int i;

    printf("Ordenador creado\n");
    for (;;) {
        sem_wait(&sem_ords); {
            char *received_path;
            int val;

            sem_getvalue(&sem_ords, &val);
            printf("Entré al wait, semaforo en: %d\n", val);
            
            while (!filename_pointer);
            if (filename_pointer == (char *) -1) {
                sem_post(&sem_ords);
                break;
            }

            received_path = filename_pointer;
            filename_pointer = NULL;

            printf("Ordenador agarró %s\n", received_path);
        } sem_post(&sem_ords);

        /* Extrae secuencia y la ordena */
        sleep(3);
        for (i=0; i<10; i++) seq[i] = i * a;
        a++;

        /* Pasa la secuencia */
        sem_wait(&sem_mezc_count);
        for (i=0; i<10; i++)
            if (!seq_pointer[i]) {
                seq_pointer[i] = seq;
                break;
            }
    }
    
    printf("Ordenador terminado\n");
    /* Ordena y pasa ... */
    sleep(4);

    return "Hola\n";
}

void *mezclador(void *arg) {
    int i, n = *(int *) arg;

    printf("Mezclador creado\n");
    for (;;) {
        int *received_seq;

        while (!seq_pointer[n]);
        received_seq = seq_pointer[n];
        seq_pointer[n] = NULL;

        sem_post(&sem_mezc_count);

        /* Imprime la secuencia */
        printf("Mezclador agarró:");
        for (i=0; i<10; i++) printf(" %d", received_seq[i]);
        printf("\n");
        
        /* Mezcla la secuencia */
        sleep(3);
    }
    
    printf("Mezclador terminado\n");
    /* Ordena y pasa ... */
    sleep(4);

    return "Hola\n";
}