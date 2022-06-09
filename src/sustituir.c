/**
 * Enry-point de programa de sustituir.
 *
 *
 * Write down usage instructions here.
 *
 * Autor: Christopher Gómez.
 * Fecha: 04-06-2022.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pair.h"
#include "list.h"

#define PAIRMATCH "%[^:]:%[^\n]\n"

/**
 * Lee un archivo con el formato indicado y extrae pares de cadenas
 * de caracteres en una lista enlazada.
 
 * @param path: Cadena de caracteres con la ruta del archivo a leer.
 * @return Apuntador a lista enlazada de pares de palabras.
 */
List *extract_words_from_file(char *path) {
    FILE *fp;
    char p1[50];
    char p2[50];
    List *l;

    fp = fopen(path, "r");
    if (!fp) {
        printf("Error abriendo el archivo.\n");
        exit(1);
    }

    l = List_new();
    if (!l) {
        printf("Hubo en error asignando memoria.\n");
        exit(1);
    }

    /* Lee hasta encontrar ':' y escribe en p1,
    luego lee hasta encontrar un salto de línea. */
    while (fscanf(fp, PAIRMATCH, p1, p2) != EOF) {
        char *word1 = malloc(sizeof(char)*strlen(p1) + 1);
        char *word2 = malloc(sizeof(char)*strlen(p2) + 1);
        Pair *p;
        
        /* Aquí se guarda el par y se enlaza a la lista. */
        p = Pair_new(strcpy(word1, p1), strcpy(word2, p2));
        List_push(l, p);
    }
    fclose(fp);

    return l;
}

int main(int argc, char **argv) {
    List *word_list;

    if (argc < 3) {
        printf("Uso: sustituir <archivo pares> {lista de nombres de archivos}.\n");
        exit(1);
    }

    word_list = extract_words_from_file(argv[1]);

    List_print(word_list);
    printf("\n");

    return 0;
}