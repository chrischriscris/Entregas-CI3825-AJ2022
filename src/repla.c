/**
 * 
 * Autor: Christopher Gómez.
 * Fecha: 04-06-2022.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "pair.h"
#include "list.h"
#include "repla.h"

#define PAIRMATCH "%[^:\n]:%[^\n]\n"

char *rand_filename();
Node *extract_words_from_file(char *path);
int replace_words(char *path, void *l);

int repla(char *root, char *path) {
    int res;

    Node *list = extract_words_from_file(path);
    if (!list) return -1;

    res = walk_dir_tree(root, repla, NULL, list, NULL);

    List_destroy(list);
    return res;
}

/**
 * Genera una string con un nombre de archivo generado aleatoriamente con
 * el formato tmp_<rand_num>.tmp. La memoria asignada para la string debe
 * ser liberada por el usuario.
 *
 * @return String con el nombre generado.
 */
char *rand_filename() {
    char *filename = malloc(16);
    int r;

    /* Genera un número aleatorio del 0 al 9999999 */
    srand(clock());
    r = rand() % 10000000;

    /* Crea la string del nombre tmp_repla_<rand_num>.tmp */
    sprintf(filename, "tmp_%.7d.tmp", r);
    return filename;
}

/**
 * Lee un archivo con el formato indicado y extrae pares de cadenas de
 * caracteres en una lista ordenada en orden descendiente del tamaño de
 * la cadena. La lista de pares debe ser liberada por el usuario.
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

        if (!word1 || !word2) {
            fclose(fp);
            return NULL;
        }
        
        /* Aquí se guarda el par y se enlaza a la lista. */
        p = Pair_new(strcpy(word1, p1), strcpy(word2, p2));
        if (!p) {
            free(word1);
            free(word2);
            fclose(fp);
            return NULL;
        }

        if (List_push(&l, p, p1_length) == -1) {
            List_destroy(l);
            fclose(fp);
            return NULL;
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
 * Destruye la lista enlazada de pares de cadenas al terminar.
 * 
 * @param path: Cadena de caracteres con la ruta del archivo a leer.
 * @param head: Apuntador a la cabeza de una lista enlazada de pares de palabras.
 * @return 0 si la operación fue exitosa.
 *     -1 en caso de error.
 */
int replace_words(char *path, void *l) {
    FILE *fp;
    struct stat st;
    char *tmp_filename;
    FILE *tmp_fp;
    char cur_char;
    Node *head = l;

    /* Abre el archivo original y guarda sus permisos */
    fp = fopen(path, "r");
    if (!fp) return -1;
    stat(path, &st);

    /* Crea y abre archivo temporal para hacer la sustitución */
    tmp_filename = rand_filename();
    tmp_fp = fopen(tmp_filename, "w");
    if (!tmp_fp) {
        fclose(fp);
        return -1;
    }

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
        free(tmp_filename);
        return -1; 
    }

    free(tmp_filename);
    chmod(path, st.st_mode);

    return 0;
}