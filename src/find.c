/**
 * Implementación de find, ifind y cfind y funciones de ayuda para su
 * funcionamiento.
 *
 * Autor: Christopher Gómez.
 * Fecha: 29-06-2022.
 */
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "find.h"
#include "utils.h"

int find_substr(char *str1, void *str2);
int find_substr_case(char *str1, void *str2);
int find_substr_inside(char *str, void *word);

/**
 * Implementación de la función de interpretador find.
 *
 * Imprime los nombres de los archivos que contienen a str como
 * subcadena. Es case sensitive.
 * 
 * @param root: Directorio raíz del árbol de archivos.
 * @param str: String con la subcadena a buscar. Puede ser NULL.
 * @return 0 si la operación fue exitosa.
 *     -1 si ocurrió un error.
 */
int find(char *root, char *str) {
    return walk_dir_tree(root, find_substr, find_substr, str, str);
}

/**
 * Implementación de la función de interpretador ifind.
 *
 * Imprime los nombres de los archivos que contienen a str como
 * subcadena. No es case sensitive.
 * 
 * @param root: Directorio raíz del árbol de archivos.
 * @param str: String con la subcadena a buscar. Puede ser NULL.
 * @return 0 si la operación fue exitosa.
 *     -1 si ocurrió un error.
 */
int ifind(char *root, char *str) {
    return walk_dir_tree(root, find_substr_case, find_substr_case, str, str);
}

/**
 * Implementación de la función de interpretador cfind.
 *
 * Imprime los nombres de los archivos que contienen a str como
 * subcadena. Y adicionalmente su contenido tenga la cadena word.
 * Es case sensitive
 * 
 * @param root: Directorio raíz del árbol de archivos.
 * @param str: String con la subcadena a buscar.
 * @param word: String con la subcadena a buscar dentro de los archivos.
 * @return 0 si la operación fue exitosa.
 *     -1 si ocurrió un error.
 */
int cfind(char *root, char *str, char *word) {
    char **args = malloc(sizeof(char *) * 2);
    int res;

    args[0] = str;
    args[1] = word;

    res = walk_dir_tree(root, find_substr_inside, NULL, args, NULL);

    free(args);
    return res;
}

/**
 * Compara dos strings e imprime la primera si la segunda
 * es NULL o si la segunda es una subcadena de la primera.
 * 
 * @param str1: Primera string a comparar.
 * @param str2: Segunda cadena a comparar.
 * @return 1 si contiene a str.
 *     0 en caso contrario.
 */
int find_substr(char *str1, void *str2) {
    char *str_cmp = str2;
    if (!str2 || strstr(str1, str_cmp) != NULL) {
        printf("%s\n", str1);
        return 1;
    }
    return 0;
}

/**
 * Compara dos strings e imprime la primera si la segunda
 * es NULL o si la segunda es una subcadena de la primera,
 * sin sensibilidad a mayúsculas/minúsculas.
 * 
 * @param str1: Primera string a comparar.
 * @param str2: Segunda cadena a comparar.
 * @return 1 si contiene a str.
 *     0 en caso contrario.
 */
int find_substr_case(char *str1, void *str2) {
    char *str_cmp = str2;
    if (!str2 || strcasestr(str1, str_cmp) != NULL) {
        printf("%s\n", str1);
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
 *     -1 en caso de algún eror.
 */
int find_substr_inside(char *path, void *args_void) {
    int fd, nread;
    char buf[BUFSIZ];
    char **args = args_void;

    /* Obtiene las palabras del argumento */
    char *str = args[0];
    char *word = args[1];
    char *cur_word = word;

    /* Si el nombre del archivo no contiene str, retorna */
    if (!strstr(path, str)) return 0;

    fd = open(path, O_RDONLY);
    if (fd == -1) return -1;

    /* Por cada bloque leído */
    while ((nread = read(fd, buf, BUFSIZ)) > 0) {
        int i = 0;
        char *cur_char = buf;

        /* Por cada caracter del bloque */
        for (; i < nread; i++, cur_char++) {
            /* Por cada letra de la palabra a buscar */
            for (; *cur_char == *cur_word && i < nread; cur_word++) {
                cur_char++;
                i++;
            }

            /* Si la palabra coincide, imprime y retorna */
            if (*cur_word == '\0') {
                printf("%s\n", path);
                close(fd);
                return 1;
            }

            /* Si se llegó al final del bloque, extrae el siguiente */
            if (i == nread) break;

            /* Si no, se reinicia la palabra */
            if (cur_word != word) {
                cur_char--;
                i--;
            }
            cur_word = word;
        }
    }

    close(fd);

    if (nread == -1) return -1;
    return 0;
}