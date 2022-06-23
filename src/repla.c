/**
 * Entry-point de programa de sustituir.
 *
 * Programa que toma una lista de pares de strings <str1, str2> y reemplaza,
 * en un conjunto de archivos de texto, todas las ocurrencias de str1 por str2.
 * Imprime el resultado por salida estándar, separando cada archivo del
 * conjunto de archivos con el delimitador '--'.
 *
 * Uso:
 * > sustituir <ruta a archivo de pares> {lista de nombres de archivos}
 * 
 * El archivo de pares consiste en texto plano, donde cada línea contiene dos
 * cadenas de caracteres separadas por ':'. Ejemplo:
 *
 * -------------------------------
 * Hola:hola                     |
 * sustituir:cambiar             |
 * hola como estas:que tal       |
 * string:cadena de caracteres   |
 * -------------------------------
 * 
 * Autor: Christopher Gómez.
 * Fecha: 04-06-2022.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "pair.h"
#include "list.h"
#include "repla.h"

#define PAIRMATCH "%[^:\n]:%[^\n]\n"

/**
 * Lee un archivo con el formato indicado y extrae pares de cadenas de
 * caracteres en una lista ordenada en orden descendiente del tamaño de
 * la cadena.
 * 
 * @param path: Cadena de caracteres con la ruta del archivo a leer.
 * @return Apuntador a la cabeza de una lista enlazada de pares de palabras.
 *     NULL en caso de error.
 */
Node *extract_words_from_file(char *path) {
    FILE *fp;
    Node *l;
    char p1[50], p2[50];

    fp = fopen(path, "r");
    if (!fp) return NULL;

    l = List_new();
    if (!l) return NULL;

    /* Lee hasta encontrar ':' y escribe en p1,
    luego lee hasta encontrar un salto de línea. */
    while (fscanf(fp, PAIRMATCH, p1, p2) != EOF) {
        int p1_length = strlen(p1);
        char *word1 = malloc(p1_length + 1);
        char *word2 = malloc(strlen(p2) + 1);
        Pair *p;

        if (!word1 || !word2) return NULL;
        
        /* Aquí se guarda el par y se enlaza a la lista. */
        p = Pair_new(strcpy(word1, p1), strcpy(word2, p2));
        if (!p) return NULL;

        if (List_push(&l, p, p1_length) == -1) return NULL;
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
 * Destruye la lista enlazada de pares de cadenas al terminar.
 * 
 * @param path: Cadena de caracteres con la ruta del archivo a leer.
 * @param head: Apuntador a la cabeza de una lista enlazada de pares de palabras.
 * @return 0 si la operación fue exitosa.
 *     -1 en caso de error.
 */
int replace_words(char *path, Node *head) {
    FILE *fp;
    struct stat st;
    FILE *tmp_fp;
    char *tmp_filename;
    int cur_char;

    /* Abre el archivo original y guarda sus permisos */
    fp = fopen(path, "r");
    if (!fp) return -1;
    stat(path, &st);

    /* Crea y abre archivo temporal para hacer la sustitución */
    tmp_filename = "tmp.txt";
    tmp_fp = fopen(tmp_filename, "w");
    if (!tmp_fp) return -1;

    /* Por cada letra del archivo a leer */
    while ((cur_char = fgetc(fp)) != EOF) {
        Node *cur_node = head->data ? head : NULL;
        long int cur_pos = ftell(fp) - 1;

        /* Por cada palabra en la lista */
        for (; cur_node; cur_node = cur_node->next) {
            char *cur_word = cur_node->data->first;

            /* Por cada letra en la palabra */
            for (;cur_char == *cur_word; cur_word++)
                cur_char = fgetc(fp);

            /* Si la palabra coincide, la reemplaza */
            if (*cur_word == '\0') {
                fprintf(tmp_fp, "%s", cur_node->data->second);
                fseek(fp, -1, SEEK_CUR);
                break;
            }

            /* Si no, pasa a la siguiente de la lista */
            fseek(fp, cur_pos, SEEK_SET);
            cur_char = fgetc(fp);
        }

        /* Si se llegó al final de la lista, se imprime el caracter */
        if (!cur_node) fprintf(tmp_fp, "%c", cur_char);

        /* Si se llegó al final del archivo, termina el ciclo */
        if (cur_char == EOF) break;
    }

    fclose(fp);
    fclose(tmp_fp);
    
    /* Borra el archivo original y renombra el archivo temporal, dándole
    los mismos permisos que el original */
    if (rename(tmp_filename, path) == -1) {
        remove(tmp_filename);
        return -1; 
    }
    chmod(path, st.st_mode);

    return 0;
}