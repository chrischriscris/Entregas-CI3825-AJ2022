#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "walk_dir_tree.h"

/**
 * Verifica si una ruta dada hace referencia a un directorio distinto
 * de . y ..
 *
 * @param path: String con la ruta a verificar.
 * @return 1 si es directorio.
 *     0 en caso contrario.
 *     -1 en caso de error.
 */
int is_directory(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return -1;

    return (st.st_mode & __S_IFDIR) != 0;
}

/**
 * Recorre un arbol de directorios en DFS.
 * 
 * @param dirpath: String con la ruta del directorio a recorrer.
 * @param callback: Funcion a llamar por cada elemento encontrado.
 * @param data: Datos a pasar a la funcion callback.
 * @return 0 en caso de exito.
 *     -1 en caso de error.
 */
int walk_dir_tree(char *dirpath) {
    DIR *dir;
    struct dirent *entry;

    /* Abre el directorio */
    dir = opendir(dirpath);
    if (!dir) return -1;

    while ((entry = readdir(dir))) {
        char *d_name = entry->d_name;
        
        /* Verifica que no sean '.' o '..' para no caer em un ciclo
        infinito */
        if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0) {
            continue;
        } else {
            char *full_path = malloc(strlen(dirpath) + strlen(d_name) + 2);
            int is_dir;
            
            /* Construye la ruta completa */
            sprintf(full_path, "%s/%s", dirpath, d_name);

            is_dir = is_directory(full_path);
            if (is_dir == -1) return -1;
            if (is_dir) {
                /* Si es directorio, se explora recursivamente */
                if (walk_dir_tree(full_path) == -1)
                    return -1;
            } else {
                /* De lo contrario, se hace una operación sobre el archivo */
                printf("%s\n", full_path);
            }
            free(full_path);
        }
    }

    return 0;
}