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
 * Verifica que un puntero no sea nulo. Si es nulo, termina el programa.
 * 
 * @param ptr Puntero a verificar.
 */
void verify_pointer(void *ptr) {
    if (!ptr) {
        printf("Hubo en error asignando memoria.\n");
        exit(1);
    }
}

/**
 * Abre un archivo y verifica que no haya habido errores.
 * 
 * @param path Ruta del archivo a abrir.
 * @return Puntero al archivo abierto.
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
 * Lee un archivo con el formato indicado y extrae pares de cadenas de
 * caracteres en una lista ordenada en orden descendiente del tamaño de
 * la cadena.
 * 
 * @param path: Cadena de caracteres con la ruta del archivo a leer.
 * @return Apuntador a la cabeza de una lista enlazada de pares de palabras.
 */
Node *extract_words_from_file(char *path) {
    FILE *fp;
    char p1[50], p2[50];
    Node *l;

    fp = open_file(path, "r");

    l = List_new();
    verify_pointer(l);

    /* Lee hasta encontrar ':' y escribe en p1,
    luego lee hasta encontrar un salto de línea. */
    while (fscanf(fp, PAIRMATCH, p1, p2) != EOF) {
        int p1_length = strlen(p1);
        char *word1 = malloc(p1_length + 1);
        char *word2 = malloc(strlen(p2) + 1);
        Pair *p;

        verify_pointer(word1);
        verify_pointer(word2);
        
        /* Aquí se guarda el par y se enlaza a la lista. */
        p = Pair_new(strcpy(word1, p1), strcpy(word2, p2));
        verify_pointer(p);

        if (!List_push(&l, p, p1_length)) {
            printf("Hubo en error agregando el par %s:%s a la lista.\n",
                word1, word2);
            exit(1);
        }
    }
    fclose(fp);

    return l;
}

/**
 * Lee un archivo y reemplaza en él todas las ocurrencias de las cadenas de
 * caracteres indicadas por otras, contenidas en una lista con pares de cadenas.
 * 
 * El análisis del archivo se hace de izquierda a derecha, por lo que se
 * reemplazan primero las ocurrencias más a la izquierda, tomando como prioridad
 * a las cadena en su orden de aparición en la lista.
 * 
 * Escribe en salida estándar el resultado.
 * 
 * @param path: Cadena de caracteres con la ruta del archivo a leer.
 * @return Apuntador a la cabeza de una lista enlazada de pares de palabras.
 */
void replace_words(char *path, Node *head) {
    FILE *fp;
    char cur_char;

    fp = open_file(path, "r");

    /* Por cada letra del archivo a leer */
    while ((cur_char = fgetc(fp)) != EOF) {
        Node *cur_node = head;
        long int cur_pos = ftell(fp) - 1;

        if (!cur_node->data) cur_node = NULL;

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
        if (!cur_node) printf("%c", cur_char);

        /* Si se llegó al final del archivo, termina el ciclo */
        if (cur_char == EOF) break;
    }
}