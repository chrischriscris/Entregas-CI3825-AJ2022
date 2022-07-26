#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include "sequence.h"
#include "utils.h"

/* Declara los mecanismos de comunicación (memoria compartida) */

/* Lector - Ordenador */
char *global_path;

/* Ordenador - Mezclador */
Sequence **global_seqs;
int free_mergers = 0;

/* QUITAR */
int nm;
char *outfile;

/* Flag indicando si ya se debe comenzar el último paso */
int last_step = 0;

/* Donde el mezclador pasa secuencias al escritor */
Sequence *writer_seq;

/* Inicializa los mutex y variables de condición a usar */
pthread_mutex_t path_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sorter_merger_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t merger_writer_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t path_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t merger_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t writer_available = PTHREAD_COND_INITIALIZER;

void *sorter_thread(void *arg);
void *merger_thread(void *arg);
void *writer_thread(void *arg);

int main(int argc, char *argv[]) {
    int i, ns, *merger_n, *res;
    char *root;
    pthread_t *tsorter_ids, *tmerger_ids, twriter_id;
    int sorter_count, merger_count;

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
    if (!ns || !nm) {
        fprintf(stderr,
            "El número de ordenadores y el número de mezcladores deben ser enteros positivos\n"
        );
        return 1;
    }

    tsorter_ids = malloc(sizeof(pthread_t) * ns);
    tmerger_ids = malloc(sizeof(pthread_t) * nm);
    merger_n = malloc(sizeof(int) * nm);
    global_seqs = malloc(sizeof(Sequence *) * nm);

    /* ================ ORDENADORES ================ */
    for (i=0; i<ns; i++) {
        if (pthread_create(&tsorter_ids[i], NULL, sorter_thread, NULL)) {
            fprintf(stderr, "Error creando ordenador %d\n", (int) i);
            
            /* Termina el proceso principal solo si no hay por lo menos
            un ordenador al terminar de crearlos todos */
            if (i == ns-1 && !sorter_count) exit(1);
        } else {
            sorter_count++;
        }
    }

    /* ================ MEZCLADORES ================ */
    for (i=0; i<nm; i++) {
        merger_n[i] = i;
        if (pthread_create(&tmerger_ids[i], NULL, merger_thread, merger_n + i)) {
            fprintf(stderr, "Error creando mezclador %d\n", (int) tmerger_ids[i]);

            /* Análogo a los ordenadores */
            if (i == nm-1 && !merger_count) exit(1);
        } else {
            merger_count++;
        }
    }

    /* ================ ESCRITOR ================ */
    if (pthread_create(&twriter_id, NULL, writer_thread, NULL)) {
        fprintf(stderr, "Error creando escritor\n");
        exit(1);
    }

    /* ================= Lector ================= */
    walk_dir_tree(
        root,
        0, NULL,
        &global_path, &path_mutex, &path_available, 0
    );

    /* -------- Región crítica -------- */
    pthread_mutex_lock(&path_mutex);

    /* Espera que se agarre el último archivo pasado */
    while (global_path) pthread_cond_wait(&path_available, &path_mutex);

    /* Avisa que no hay más archivos que pasar */
    global_path = (char *) -1;
    pthread_cond_broadcast(&path_available);

    pthread_mutex_unlock(&path_mutex);
    /* ------ Fin región crítica ------ */

    /* Espera que todos los ordenadores terminen */
    for (i=0; i<ns; i++) {
        if (pthread_join(tsorter_ids[i], (void **) &res))
            fprintf(stderr, "Ha ocurrido un esperando al Ordenador %d\n", i);

        if (res)
            fprintf(stderr, "Ha ocurrido un error en el Ordenador %d\n", i);
    }

    /* Cuando los ordenadores terminaron, ya se puede empezar a pasar las
    secuencias a los mezcladores. */
    /* No se usan mutex porque la variable, aunque global, es de solo lectura */
    last_step = 1;
    pthread_cond_broadcast(&merger_available);

    /* Espera a que todos los mezcladores terminen */
    for (i=0; i<nm; i++) {
        if (pthread_join(tmerger_ids[i], (void **) &res))
            fprintf(stderr, "Ha ocurrido un esperando al Mezclador %d\n", i);

        if (res)
            fprintf(stderr, "Ha ocurrido un error en el Mezclador %d\n", i);
    }

    if (pthread_join(twriter_id, (void **) &res))
        fprintf(stderr, "Ha ocurrido un esperando al Escritor\n");

    if (res)
        fprintf(stderr, "Ha ocurrido un error en el Escritor\n");

    return 0;
}

void *sorter_thread(void *arg) {
    for (;;) {
        Sequence *seq;
        char *path;
        int i;

        /* -------- Región crítica -------- */
        pthread_mutex_lock(&path_mutex);

        /* Se suspende hasta que el lector actualice la variable global */
        while (!global_path) pthread_cond_wait(&path_available, &path_mutex);

        if (global_path == (char *) -1) {
            pthread_mutex_unlock(&path_mutex);
            break;
        }

        /* Toma el path de la variable global, lo hace NULL y avisa */
        path = global_path;
        global_path = NULL;
        pthread_cond_broadcast(&path_available);

        pthread_mutex_unlock(&path_mutex);
        /* ------ Fin región crítica ------ */
    
        /* Procede a leer y ordenar los datos del archivo */
        seq = Sequence_extract_from_file(path);
        if (!seq) {
            fprintf(stderr, "Error al extraer secuencia del archivo %s\n", path);
            free(path);
            continue;
        }
        free(path);
        Sequence_sort(seq);

        /* Pasa la secuencia a un mezclador libre */

        /* -------- Región crítica -------- */
        pthread_mutex_lock(&sorter_merger_mutex);

        /* Se suspende hasta que haya un mezclador disponible */
        while (!free_mergers)
            pthread_cond_wait(&merger_available, &sorter_merger_mutex);

        /* Luego busca y pasa la secuencia al disponible, lo marca como desocupado
        y avisa */
        for (i=0; i<nm; i++) {
            if (!global_seqs[i]) {
                global_seqs[i] = seq;
                free_mergers--;
                pthread_cond_broadcast(&merger_available);
                break;
            }
        }

        pthread_mutex_unlock(&sorter_merger_mutex);
        /* ------ Fin región crítica ------ */
    }

    pthread_exit(0);
}

void *merger_thread(void *arg) {
    int n = *(int *) arg;
    Sequence *local_seq = Sequence_new(0);

    for (;;) {
        Sequence *arriving_seq;

        /* -------- Región crítica -------- */
        pthread_mutex_lock(&sorter_merger_mutex);

        /* Se anota como disponible y avisa */
        free_mergers++;
        global_seqs[n] = NULL;
        pthread_cond_broadcast(&merger_available);

        /* Mientras no haya trabajo y no sea el último paso */
        while (!global_seqs[n] && !last_step)
            pthread_cond_wait(&merger_available, &sorter_merger_mutex);

        /* Si no hay trabajo y es hora del último paso, sale del ciclo */
        if (!global_seqs[n] && last_step) {
            pthread_mutex_unlock(&sorter_merger_mutex);
            break;
        }

        /* Toma la secuencia y decrementa el número de mezcladores libres */
        arriving_seq = global_seqs[n];

        pthread_mutex_unlock(&sorter_merger_mutex);
        /* ------ Fin región crítica ------ */

        /* Mezcla la secuencia */
        if (!Sequence_merge(&local_seq, arriving_seq))
            fprintf(stderr, "Error al mezclar una secuencia\n");
    }

    /* -------- Región crítica -------- */
    pthread_mutex_lock(&merger_writer_mutex);

    /* Se suspende hasta que el escritor esté libre */
    while (writer_seq) pthread_cond_wait(&writer_available, &merger_writer_mutex);

    /* Pasa la secuencia al escritor y avisa */
    writer_seq = local_seq;
    pthread_cond_broadcast(&writer_available);

    pthread_mutex_unlock(&merger_writer_mutex);
    /* ------ Fin región crítica ------ */

    pthread_exit(0);
}

void *writer_thread(void *arg) {
    Sequence **seqs_arr = malloc(sizeof(Sequence) * nm);
    int i;

    for (i=0; i<nm; i++) {
        /* -------- Región crítica -------- */
        /* Toma la i-ésima secuencia de algún mezclador */
        pthread_mutex_lock(&merger_writer_mutex);

        while (!writer_seq) pthread_cond_wait(&writer_available, &merger_writer_mutex);

        /* Toma la secuencia y avisa */
        seqs_arr[i] = writer_seq;
        writer_seq = NULL;

        pthread_cond_signal(&writer_available);
        pthread_mutex_unlock(&merger_writer_mutex);
        /* ------ Fin región crítica ------ */
    }

    Sequence_write_merged(seqs_arr, nm, outfile);
    for (i=0; i<nm; i++) Sequence_destroy(seqs_arr[i]);
    free(seqs_arr);

    pthread_exit(0);
}