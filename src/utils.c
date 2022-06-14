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
    cur_char = fgetc(fp);

    /* Por cada letra del archivo a leer */
    while (cur_char != EOF) {
        Node *cur_node;

        /* Por cada palabra en la lista */
        for (cur_node = head; cur_node != NULL; cur_node = cur_node->next) {
            char *cur_word = cur_node->data->first;
            char cmp_char;
            int i = 0;

            /* Por cada letra en la palabra */
            for (cmp_char = cur_word[i]; cmp_char != '\0';
                cmp_char = cur_word[i]
            ) {
                /* Si no coincide con la letra actual, pasa a la
                siguiente palabra */
                if (cur_char != cmp_char) {
                    fseek(fp, -i-1, SEEK_CUR);
                    cur_char = fgetc(fp);
                    break;
                }

                /* Si coincide, pasa a la siguiente palabra */
                cur_char = fgetc(fp);
                i++;
            }

            /* Si la palabra coincide, reemplaza la palabra */
            if (cmp_char == '\0') {
                printf("%s", cur_node->data->second);
                break;
            }
        }

        cur_char = fgetc(fp);
        /* Si no se coincidió con ninguna palabra, imprime el caracter */
        printf("%c", cur_char);
    }
}