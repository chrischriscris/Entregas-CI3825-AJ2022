#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "utils.h"

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
 * Recorre un arbol de directorios en DFS.
 * 
 * @param root: String con la ruta del directorio raiz a recorrer.
 * @param fn1: Funcion a llamar por cada archivo regular en el árbol,
 *     de directorios, que retorna 0 si fue exitosa y -1 en caso de
 *     error. Si es NULL no se llama.
 * @param fn2: Análoga a la anterior, se llama por cada archivo del árbol.
 * @param arg1: Apuntador al argumento que recibirá fn1.
 * @param arg2: Apuntador al argumento que recibirá fn2.
 * @return 0 en caso de exito.
 *     -1 en caso de error.
 */
int walk_dir_tree(char *root, int (*fn1)(char *, void *),
    int (*fn2)(char *, void *), void *arg1, void *arg2) {
    DIR *dir;
    struct dirent *entry;

    /* Abre el directorio */
    dir = opendir(root);
    if (!dir) return -1;

    while ((entry = readdir(dir))) {
        char *d_name = entry->d_name;
        
        /* Verifica que no sean '.' o '..' para no caer em un ciclo
        infinito */
        if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0) {
            continue;
        } else {
            char *full_path = malloc(strlen(root) + strlen(d_name) + 2);
            int is_dir;

            /* Construye la ruta completa */
            sprintf(full_path, "%s/%s", root, d_name);

            is_dir = is_directory(full_path);
            if (is_dir == -1) return -1;

            if (is_dir) {
                /* Si es directorio, se explora recursivamente */
                if (walk_dir_tree(full_path, fn1, fn2, arg1, arg2) == -1) {
                    free(full_path);
                    return -1;
                }
            } else {
                /* De lo contrario, se hace una operación sobre el archivo,
                de ser regular*/
                if (is_regular_file(full_path)) {
                    /* Si fn1 no es nula, se llama */
                    if (fn1) {
                        if (fn1(full_path, arg1) == -1) {
                            fprintf(stderr, "Hubo un error abriendo el archivo \
                                %s.", full_path);
                            free(full_path);
                            continue;
                        }
                    }
                }
            }
            /* Análogamente, si fn2 no es nula, se llama */
            if (fn2) {
                if (fn2(full_path, arg2) == -1) {
                    fprintf(stderr, "Hubo un error abriendo navegando por \
                        el directorio %s.", full_path);
                    free(full_path);
                    continue;
                }
            }
            free(full_path);
        }
    }

    return 0;
}