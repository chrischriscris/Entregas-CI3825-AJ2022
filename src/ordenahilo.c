/**
 * Entry-point del ordenahilo. Se ejecuta indicando el número de ordenadores,
 * el número de mezcladores, una ruta a un directorio raíz y el nombre de un
 * archivo donde se escribirán los resultados.
 * 
 * Ordena todos los archivos .txt que se encuentren en el directorio raíz y sus
 * sub-directorios, de forma recursiva. Cada ordenamiento se ejecuta de forma
 * concurrente/paralela en un hilo ordenador distinto. Luego cada ordenador pasa su
 * secuencia ordenada a un mezclador, que mezcla la secuencia con una local,
 * y finalmente cada mezclador pasa sus secuencias a un escritor, que las
 * escribe de forma ordenada en un archivo de salida.
 * 
* Se ejecuta de la forma:
 * 
 * ordenahilo <num Ordenadores> <num Mezcladores> <raiz> <archivo salida>
 * 
 * Autor: Christopher Gómez.
 * Fecha: 30-07-2022.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include "threadworkers.h"
#include "sharedvars.h"
#include "sequence.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    int i, ns, nm, *merger_n, *res;
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

    /* Se inicializa el arreglo con valores distintos de nulo, cada mezclador
    se marca como desocupado haciendo global_seqs[i] = NULL */
    global_seqs = malloc(sizeof(Sequence *) * nm);
    for (i = 0; i<nm; i++) global_seqs[i] = (Sequence *) 1;

    tsorter_ids = malloc(sizeof(pthread_t) * ns);
    tmerger_ids = malloc(sizeof(pthread_t) * nm);
    merger_n = malloc(sizeof(int) * nm);

    /* ================ ORDENADORES ================ */
    for (i=0; i<ns; i++) {
        if (pthread_create(&tsorter_ids[i], NULL, sorter_thread, &nm)) {
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
    if (pthread_create(&twriter_id, NULL, writer_thread, &nm)) {
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

    /* Espera que todos los ordenadores terminen y libera sus recursos */
    for (i=0; i<ns; i++) {
        if (pthread_join(tsorter_ids[i], (void **) &res))
            fprintf(stderr, "Ha ocurrido un esperando al Ordenador %d\n", i);

        if (res)
            fprintf(stderr, "Ha ocurrido un error en el Ordenador %d\n", i);
    }
    free(tsorter_ids);
    pthread_mutex_destroy(&path_mutex);
    pthread_cond_destroy(&path_available);

    /* Cuando los ordenadores terminaron, ya se puede empezar a pasar las
    secuencias a los mezcladores. */
    /* No se usan mutex porque la variable, aunque global, es de solo lectura */
    last_step = 1;
    pthread_cond_broadcast(&merger_available);

    /* Espera a que todos los mezcladores terminen y libera sus recursos */
    for (i=0; i<nm; i++) {
        if (pthread_join(tmerger_ids[i], (void **) &res))
            fprintf(stderr, "Ha ocurrido un esperando al Mezclador %d\n", i);

        if (res)
            fprintf(stderr, "Ha ocurrido un error en el Mezclador %d\n", i);
    }
    free(global_seqs);
    free(tmerger_ids);
    free(merger_n);
    pthread_mutex_destroy(&sorter_merger_mutex);
    pthread_cond_destroy(&merger_available);

    /* Espera que el escritor termine y libera sus recursos */
    if (pthread_join(twriter_id, (void **) &res))
        fprintf(stderr, "Ha ocurrido un esperando al Escritor\n");

    if (res)
        fprintf(stderr, "Ha ocurrido un error en el Escritor\n");
    pthread_mutex_destroy(&merger_writer_mutex);
    pthread_cond_destroy(&writer_available);

    return 0;
}