/**
 * Implementación de wc y funciones de ayuda para su funcionamiento.
 *
 * Autor: Christopher Gómez.
 * Fecha: 29-06-2022.
 */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "wc.h"
#include "utils.h"

int count_chars_and_lines(char *path, int *chars, int *lines);

/**
 * Implementación de la función de interpretador wc.
 * 
 * Por directorio dentro del árbol, imprime el total de líneas y caracteres
 * de los archivos regulares que contiene en forma recursiva, incluyendo
 * la raíz del árbol.
 * 
 * @param root: String con la ruta del directorio raiz a recorrer.
 * @param chars (out): Entero con el número de caracteres de root.
 * @param lines (out): Entero con el número de líneas de root.
 * @return 0 en caso de exito.
 *     -1 en caso de error.
 */
int wc(char *root, int *chars, int *lines) {
    struct dirent *entry;

    /* Abre el directorio */
    DIR *dir = opendir(root);
    if (!dir) return -1;

    *chars = 0;
    *lines = 0;

    while ((entry = readdir(dir))) {
        char *d_name = entry->d_name;
        
        /* Verifica que no sean '.' o '..' para no caer en un ciclo
        infinito */
        if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0) {
            continue;
        } else {
            char *full_path = malloc(sizeof(char) *
                (strlen(root) + strlen(d_name) + 2));
            int is_dir;
            int n, m;

            /* Construye la ruta completa */
            sprintf(full_path, "%s/%s", root, d_name);

            is_dir = is_directory(full_path);
            if (is_dir == -1) {
                free(full_path);
                continue;
            }

            if (is_dir) {
                /* Si es directorio, se explora recursivamente */
                if (wc(full_path, &n, &m) == -1) {
                    fprintf(stderr, "Hubo un error abriendo navegando por \
                        el directorio %s.", full_path);
                    free(full_path);
                    continue;
                }
            } else {
                /* De lo contrario, se hace una operación sobre el archivo,
                de ser regular*/
                if (is_regular_file(full_path)) {
                    if (count_chars_and_lines(full_path, &n, &m) == -1) {
                        fprintf(stderr, "Hubo un error abriendo navegando por \
                            el directorio %s.", full_path);
                        free(full_path);
                        continue;
                    }
                }
            }
            *chars += n;
            *lines += m;

            free(full_path);
        }
    }
    printf("El directorio %s: %d líneas y %d caracteres\n", root, *lines, *chars);
    closedir(dir);

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
    int fd;
    int nread;
    char buf[BUFSIZ];

    fd = open(path, O_RDONLY);
    if (fd == -1) return -1;

    *chars = 0;
    *lines = 0;

    /* Carga en el buffer bloques enteros y cuenta el número de /n en ellos,
    llevando también cuenta del número de caracteres leídos */
    while ((nread = read(fd, buf, BUFSIZ)) > 0) {
        char *c = buf;
        int i = 0;
        *chars += nread;

        for (;i < nread; i++) {
            if (*c++ == '\n') {
                (*lines)++;
            }
        }
    }

    close(fd);
    if (nread == -1) return -1;
    return 0;
}