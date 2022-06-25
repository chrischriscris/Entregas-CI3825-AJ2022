#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "wc.h"
#include "utils.h"

/**
 * Recorre un arbol de directorios en DFS, contando las líneas y
 * caracteres ...
 * 
 * Se llama con chars = 0 y lines = 0
 * 
 * @param path: String con la ruta del directorio raiz a recorrer.
 * @return 0 en caso de exito.
 *     -1 en caso de error.
 */
int wc(char *path, int *chars, int *lines) {
    DIR *dir;
    struct dirent *entry;

    /* Abre el directorio */
    dir = opendir(path);
    if (!dir) return -1;

    *chars = 0;
    *lines = 0;

    while ((entry = readdir(dir))) {
        char *d_name = entry->d_name;
        
        /* Verifica que no sean '.' o '..' para no caer em un ciclo
        infinito */
        if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0) {
            continue;
        } else {
            char *full_path = malloc(strlen(path) + strlen(d_name) + 2);
            int is_dir;

            /* Construye la ruta completa */
            sprintf(full_path, "%s/%s", path, d_name);

            is_dir = is_directory(full_path);
            if (is_dir == -1) return -1;

            if (is_dir) {
                int n, m;

                /* Si es directorio, se explora recursivamente */
                if (wc(full_path, &n, &m) == -1) {
                    fprintf(stderr, "Hubo un error abriendo navegando por \
                        el directorio %s.", full_path);
                    free(full_path);
                    continue;
                }
                *chars += n;
                *lines += m;
            } else {
                /* De lo contrario, se hace una operación sobre el archivo,
                de ser regular*/
                if (is_regular_file(full_path)) {
                    int n, m;

                    if (count_chars_and_lines(full_path, &n, &m) == -1) {
                        fprintf(stderr, "Hubo un error abriendo navegando por \
                            el directorio %s.", full_path);
                        free(full_path);
                        continue;
                    }
                    *chars += n;
                    *lines += m;
                }
            }
            free(full_path);
        }
    }
    printf("El directorio %s: %d líneas y %d caracteres\n", path, *lines, *chars);

    return 0;
}


/**
 * Cuenta el número de caracteres y líneas de un archivo.
 * 
 * @param path: String con la ruta del archivo.
 * @param chars (out): Número de caracteres.
 * @param lines (out): Número de líneas.
 * @return: 0 si la operación fue exitosa.
 *     -1 en caso contrario.
 */
int count_chars_and_lines(char *path, int *chars, int *lines) {
    FILE *fp;
    int c;

    fp = fopen(path, "r");
    if (!fp) return -1;

    *chars = 0;
    *lines = 0;
    
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            (*lines)++;
        }
        (*chars)++;
    }

    fclose(fp);
    return 0;
}