/**
 * Utiles de la libreria.
 *
 * Autor: Christopher Gómez.
 * Fecha: 13-06-2022.
 */
#define PAIRMATCH "%[^:]:%[^\n]\n"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "list.h"
#include "pair.h"

void verify_malloc(void *ptr) {
    if (!ptr) {
        printf("Hubo en error asignando memoria.\n");
        exit(1);
    }
}

FILE *open_file(char *path, char *mode) {
    FILE *fp = fopen(path, mode);
    if (!fp) {
        printf("Hubo en error abriendo el archivo %s.\n", path);
        exit(1);
    }
    return fp;
}

/**
 * Lee un archivo con el formato indicado y extrae pares de cadenas
 * de caracteres en una lista enlazada.
 
 * @param path: Cadena de caracteres con la ruta del archivo a leer.
 * @return Apuntador a lista enlazada de pares de palabras.
 */
Node *extract_words_from_file(char *path) {
    FILE *fp;
    char p1[50], p2[50];
    Node *l;

    fp = open_file(path, "r");

    l = List_new();
    verify_malloc(l);

    /* Lee hasta encontrar ':' y escribe en p1,
    luego lee hasta encontrar un salto de línea. */
    while (fscanf(fp, PAIRMATCH, p1, p2) != EOF) {
        int p1_length = strlen(p1);
        char *word1 = malloc(p1_length + 1);
        char *word2 = malloc(strlen(p2) + 1);
        Pair *p;

        verify_malloc(word1);
        verify_malloc(word2);
        
        /* Aquí se guarda el par y se enlaza a la lista. */
        p = Pair_new(strcpy(word1, p1), strcpy(word2, p2));
        List_push(&l, p, p1_length);
    }
    fclose(fp);

    return l;
}

void replace_words(char *path, Node *head) {
    FILE *fp;
    char cur_char;

    fp = open_file(path, "r");

    /* Por cada letra del archivo a leer */
    while ((cur_char = fgetc(fp)) != EOF) {
        Node *cur_node;

        /* Por cada palabra en la lista */
        for (cur_node = head; cur_node != NULL; cur_node = cur_node->next) {
            char *word_from = cur_node->data->first;
            char cmp_char;

            /* Por cada letra en la palabra */
            for (cmp_char = 0; cmp_char < strlen(cur_node->data->word1); cmp_char++) {
                /* Si la letra del archivo es igual a la letra de la palabra */
                if (cur_char == cur_node->data->word1[cmp_char]) {
                    /* Imprime la palabra */
                    printf("%s", cur_node->data->word2);
                    /* Salta a la siguiente palabra */
                    break;
                }
            }
            char *word1 = p->word1;
            char *word2 = p->word2;
        if (cur_char == EOF)
            break;

        printf("%c", cur_char);
    }
}