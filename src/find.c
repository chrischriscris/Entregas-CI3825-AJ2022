#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#include "find.h"
#include "utils.h"

int find_substr(char *str1, void *str2);
int find_substr_case(char *str1, void *str2);
int find_substr_inside(char *str, void *word);

int find(char *root, char *str) {
    return walk_dir_tree(root, find_substr, find_substr, str, str);
}

int ifind(char *root, char *str) {
    return walk_dir_tree(root, find_substr_case, find_substr_case, str, str);
}

int cfind(char *root, char *str, char *word) {
    char **args = malloc(sizeof(char *) * 2);
    int res;

    args[0] = str;
    args[1] = word;

    res = walk_dir_tree(root, find_substr, NULL, args, NULL);

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
 *     -1 em casp de algún eror.
 */
int find_substr_inside(char *path, void *args) {
    FILE *fp;
    char cur_char;
    char **args_cast = args;

    /* Obtiene las palabras del argumento */
    char *str = args_cast[0];
    char *word = args_cast[1];
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