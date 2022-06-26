#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

int find(char *path, void *str) {
    char *str_cmp = !str ? path : (char *) str;
    if (strstr(path, str_cmp) != NULL) {
        printf("%s\n", path);
        return 1;
    }
    return 0;
}

int ifind(char *path, void *str) {
    char *str_cmp = !str ? path : (char *) str;
    if (strcasestr(path, str_cmp) != NULL) {
        printf("%s\n", path);
        return 1;
    }
    return 0;
}

int cfind(char *path, void *str_word) {
    FILE *fp;
    int cur_char;

    /* Obtiene las palabras del argumento */
    char *str = ((char **) str_word)[0];
    char *word = ((char **) str_word)[1];
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
            return 1;
        }

        /* Si se llegó al final del archivo, termina el ciclo */
        if (cur_char == EOF) break;
        cur_word = word;
    }
    fclose(fp);

    return 0;
}