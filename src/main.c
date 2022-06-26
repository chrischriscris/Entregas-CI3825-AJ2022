#include <stdio.h>
#include <stdlib.h>

#include "wc.h"
#include "find.h"
#include "list.h"
#include "pair.h"
#include "codif.h"
#include "repla.h"
#include "utils.h"


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

    /* Inicialización de codif */
    /*
    root = argv[1];
    walk_dir_tree(root, codif, NULL, NULL, NULL);
    */

   /* Inicialización de find */
   /*
    root = argv[1];
    path = argv[2];
    walk_dir_tree(root, find, NULL, path, NULL);
    */

    /* Inicialización de ifind */
    /*
    root = argv[1];
    path = argv[2];
    walk_dir_tree(root, ifind, NULL, path, NULL);
    */

    /* Inicialización de cfind */
    /*
    root = argv[1];
    args = malloc(sizeof(char *) * 2);
    args[0] = argv[2];
    args[1] = argv[3];
    walk_dir_tree(root, cfind, NULL, args, NULL);
    free(args);
    */

    /* Inicialización de repla */
    /*
    root = argv[1];
    path = argv[2];
    list = extract_words_from_file(path);
    if (!list) {
        printf("Error al leer el archivo.\n");
        return 1;
    }
    walk_dir_tree(root, repla, NULL, list, NULL);
    */

    /* Inicialización de wc */
    /*
    root = argv[1];
    wc(root, &c, &l);
    */

    return 0;
}