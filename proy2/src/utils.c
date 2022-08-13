/**
 * Implementación de funciones de utilidad para las implementaciones de
 * ordenahilo y ordenaproc.
 *
 * Autor: Christopher Gómez.
 * Fecha: 29-06-2022.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>

#include "utils.h"
#include "sequence.h"

/**
 * 
 * Aborta el proceso en caso de error.
 * 
 * @param n: número de pipes a inicializar.
 * @return int** Apuntador a arreglo nx2 de pipes inicializados.
 */
void *safe_malloc(size_t n) {
    /* Asigna memoria */
    void *p = malloc(n);

    /* Aborta si hubo algún error */
    if (!p) {
        fprintf(stderr, "Ha habido un error al reservar memoria.\n");
        exit(1);
    }

    return p;
}

/**
 * Inicializa n pipes en un arreglo y retorna un apuntador al mismo.
 * 
 * La memoria debe ser liberada por el usuario (la de cada subarreglo y
 * la del arreglo entero).
 * Aborta el proceso en caso de error.
 * 
 * @param n: número de pipes a inicializar.
 * @return int** Apuntador a arreglo nx2 de pipes inicializados.
 */
int **initialize_pipes(int n) {
    int i;
    
    /* Asigna memoria para n punteros a entero */
    int **pipe_arr = safe_malloc(sizeof(int *) * n);

    /* Inicializa n pipes */
    for (i=0 ; i<n ; i++) {
        pipe_arr[i] = safe_malloc(sizeof(int) * 2);
        
        if (pipe(pipe_arr[i]) == -1) {
            perror("Error al crear pipe");
            exit(1);
        }
    }

    return pipe_arr;
}

/**
 * Recorre un arbol de directorios en DFS.
 * 
 * @param root: String con la ruta del directorio raiz a recorrer.
 * @param sorter_queue: Pipe para leer de la cola
 * @param to_sorter: Arreglo de pipes 
 * @return 0 en caso de exito.
 *     -1 en caso de error.
 */
int walk_dir_tree(
    char *root,
    int sorter_queue, int *to_sorter,
    char **global_path, pthread_mutex_t *mutex, pthread_cond_t *cond,
    int is_process
) {
    struct dirent *entry;

    /* Abre el directorio */
    DIR *dir = opendir(root);
    if (!dir) return -1;

    while ((entry = readdir(dir))) {
        char *d_name = entry->d_name;

        /* Verifica que no sean '.' o '..' para no caer em un ciclo
        infinito */
        if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0) {
            continue;
        } else {
            char *full_path = malloc(sizeof(char) *
                (strlen(root) + strlen(d_name) + 2));
            
            int is_dir;

            /* Construye la ruta completa */
            sprintf(full_path, "%s/%s", root, d_name);

            is_dir = is_directory(full_path);
            if (is_dir == -1) {
                free(full_path);
                continue;
            }

            if (is_dir) {
                /* Si es directorio, se explora recursivamente */
                if (walk_dir_tree(full_path,
                        sorter_queue, to_sorter,
                        global_path, mutex, cond,
                        is_process) == -1
                ) {
                    free(full_path);
                    continue;
                }
            } else {
                /* De lo contrario, se hace una operación sobre el archivo,
                de ser regular*/
                if (is_regular_file(full_path) && is_txt_file(full_path)) {
                    if (is_process) {
                        int n, path_len;
                        if (read(sorter_queue, &n, sizeof(int)) == -1) {
                            free(full_path);
                            continue;
                        }

                        path_len = strlen(full_path) + 1;
                        if (write(to_sorter[n], &path_len, sizeof(int)) == -1) {
                            free(full_path);
                            continue;
                        }

                        if (write(to_sorter[n], full_path, path_len) != path_len) {
                            free(full_path);
                            continue;
                        }
                    } else {
                        /* -------- Región crítica -------- */
                        pthread_mutex_lock(mutex);

                        /* Modifica la variable global y levanta a un solo
                        hilo que esté esperando */
                        *global_path = full_path;
                        pthread_cond_signal(cond);

                        /* Se suspende hasta que alguien tome el path de la
                        variable global */
                        while (*global_path) pthread_cond_wait(cond, mutex);

                        pthread_mutex_unlock(mutex);
                        /* ------ Fin región crítica ------ */
                    }
                }
            }
            /* Si son hilos, el hilo que  */
            if (is_process) free(full_path);
        }
    }
    closedir(dir);

    return 0;
}

/**
 * Verifica si una ruta es un directorio.
 *
 * @param path: String con la ruta a verificar.
 * @return 1 si es directorio.
 *     0 en caso contrario.
 *     -1 en caso de error.
 */
int is_directory(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return -1;

    return S_ISDIR(st.st_mode);
}

/**
 * Verifica si un archivo es regular.
 *
 * @param path: String con la ruta del archivo a verificar.
 * @return 1 si es un archivo regular.
 *     0 en caso contrario.
 *     -1 en caso de error.
 */
int is_regular_file(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return -1;

    return S_ISREG(st.st_mode);
}

/**
 * Verifica si un archivo tiene la extensión .txt.
 *
 * @param path: String con la ruta del archivo a verificar.
 * @return 1 si el archivo tiene la extensión.
 *     0 en caso contrario.
 *     -1 en caso de error.
 */
int is_txt_file(char *path) {
    char *dot = strrchr(path, '.');
    return dot && !strcmp(dot, ".txt");
}