/**
 * Utiles para el programa sustituir.
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

/**
 *
 */
void verify_malloc(void *ptr) {
    if (!ptr) {
        printf("Hubo en error asignando memoria.\n");
        exit(1);
    }
}

/**
 *
 */
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
        Node *cur_node = head;
        long int cur_pos = ftell(fp) - 1;

        /* Por cada palabra en la lista */
        for (; cur_node; cur_node = cur_node->next) {
            char *cur_word = cur_node->data->first;

            /* Por cada letra en la palabra */
            for (;cur_char == *cur_word; cur_word++)
                cur_char = fgetc(fp);

            /* Si la palabra coincide, la reemplaza */
            if (*cur_word == '\0') {
                printf("%s", cur_node->data->second);
                fseek(fp, -1, SEEK_CUR);
                break;
            }
            
            /* Si no, pasa a la siguiente de la lista */
            fseek(fp, cur_pos, SEEK_SET);
            cur_char = fgetc(fp);
        }

        /* Si se llegó al final de la lista, se imprime el caracter */
        if (!cur_node)
            printf("%c", cur_char);

        /* Si se llegó al final del archivo, termina el ciclo */
        if (cur_char == EOF)
            break;
    }
}