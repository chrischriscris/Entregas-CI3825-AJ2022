#include <stdio.h>
#include <stdlib.h>

#include "wc.h"
#include "find.h"
#include "list.h"
#include "codif.h"
#include "repla.h"

char *input_string(FILE* fp, int size);

int main(int argc, char **argv) {
    /* Inicialización del interpretador */
    /*
    char *m;

    printf("myutil> ");
    m = input_string(stdin, 10);
    printf("%s\n", m);

    free(m);
    */

    int c, l;
    char *root;
    char *path;
    char **args;
    Node *list;

    /* codif */ 
    /*
    root = argv[1];
    codif(root);
    */

    /* find */
    /*
    root = argv[1];
    path = argv[2];
    find(root, path);
    */

    /* ifind */
    /*
    root = argv[1];
    path = argv[2];
    ifind(root, path);
    */

    /* cfind */
    /*
    root = argv[1];
    str1 = argv[2];
    str2 = argv[3];
    cfind(root, str1, str2);
    */

    /* repla */
    /*
    root = argv[1];
    path = argv[2];
    repla(root, path);
    */

    /* wc */
    /*
    root = argv[1];
    wc(root, &c, &l);
    */

    return 0;
}

/**
 * Lee una string de un apuntador a FILE y asigna memoria dinámicamente
 * para leer todo su contenido. Comienza con tamaño size y aumenta en 16
 * en 16 la longitud.
 * 
 * @param fp: Apuntador a FILE.
 * @param size: Tamaño inicial esperado de la string.
 * @return Apuntador a string, debe ser liberada por el usuario.
 */
char *input_string(FILE* fp, int size) {
    char *str;
    int c;
    int len = 0;

    /* Se asigna size cantidad de memoria */
    str = malloc(size);
    if (!str)
        return NULL;
    
    while((c=fgetc(fp)) != EOF && c != '\n'){
        str[len++] = c;

        if (len == size + 1) {
            size += 16;
            str = realloc(str, size);
            if (!str)
                return str;
        }
    }

    str[len++] = '\0';
    return realloc(str, len);
}