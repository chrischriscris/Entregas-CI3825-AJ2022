#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "wc.h"

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