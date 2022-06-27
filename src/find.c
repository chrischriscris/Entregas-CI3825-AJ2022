#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

/**
 * Imprime la ruta si contiene a str.
 * 
 * @param path: String con una ruta.
 * @param str: String con una subcadena a buscar.
 * @return 1 si contiene a str.
 *     0 en caso contrario.
 */
int find(char *path, void *str) {
    char *str_cmp = !str ? path : str;
    if (strstr(path, str_cmp) != NULL) {
        printf("%s\n", path);
        return 1;
    }
    return 0;
}

/**
 * Imprime la ruta si contiene a str, sin sesibilidad a
 * mayúsculas/minúsculas.
 * 
 * @param path: String con una ruta.
 * @param str: String con una subcadena a buscar.
 * @return 1 si contiene a str.
 *     0 en caso contrario.
 */
int ifind(char *path, void *str) {
    char *str_cmp = !str ? path : str;
    if (strcasestr(path, str_cmp) != NULL) {
        printf("%s\n", path);
        return 1;
    }
    return 0;
}


/**
 * Imprime la ruta de un archivo regular si esta contiene a str
 * y además el archivo contiene a word dentro.
 * 
 * @param path: String con una ruta.
 * @param args: Arreglo {str, word}.
 * @return 1 si la ruta contiene str y tiene a word en su contenido.
 *     0 en caso contrario.
 *     -1 em casp de algún eror.
 */
int cfind(char *path, void *args) {
    FILE *fp;
    char cur_char;

    /* Obtiene las palabras del argumento */
    char *str = ((char **) args)[0];
    char *word = ((char **) args)[1];
    char *cur_word = word;

    /* Si el nombre del archivo no contiene str, retorna */
    if (!strstr(path, str)) return 0;

    fp = fopen(path, "r");
    if (!fp) return -1;

    /* Por cada letra del archivo a leer */
    while ((cur_char = fgetc(fp)) != EOF) {
        /* Por cada letra en la palabra */
        for (;cur_char == *cur_word; cur_word++)
            cur_char = fgetc(fp);

        /* Si la palabra coincide, imprime y retorna */
        if (*cur_word == '\0') {
            printf("%s\n", path);
            fclose(fp);
            return 1;
        }

        /* Si se llegó al final del archivo, termina el ciclo */
        if (cur_char == EOF) break;
        cur_word = word;
    }
    fclose(fp);

    return 0;
}