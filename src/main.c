#include <stdio.h>
#include <stdlib.h>

#include "codif.h"
#include "find.h"
#include "list.h"
#include "pair.h"
#include "repla.h"
#include "utils.h"
#include "wc.h"


/**
 * Lee una string de un apuntador a FILE y asigna memoria dinámicamente
 * para leer todo su contenido. Comienza con tamaño size y aumenta en 16
 * en 16 la longitud.
 * 
 * @param fp: Apuntador a FILE.
 * @param size: Tamaño inicial esperado de la string.
 * @return Apuntador a string, debe ser liberada por el usuario.
 */
char *input_string(FILE* fp, int size){
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

int main(int argc, char **argv) {
    /*
    char *m;

    // printf("myutil> ");
    // m = input_string(stdin, 10);
    // printf("%s\n", m);

    free(m);
    */
    char *root;
    char *path;

    /* Inicialización de codif */
    /*
    root = argv[1];
    walk_dir_tree(root, codif, NULL, NULL, NULL);
    */

   /* Inicialización de find */
    root = argv[1];
    path = argv[2];
    walk_dir_tree(root, find, NULL, path, NULL);

    
    return 0;
}