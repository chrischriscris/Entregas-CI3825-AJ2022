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

#include "pair.h"
#include "list.h"
#include "utils.h"

int main(int argc, char **argv) {
    Node *word_list;
    int i;

    if (argc < 3) {
        printf("Uso: sustituir <archivo pares> {lista de nombres de archivos}.\n");
        exit(1);
    }

    /* Extrae la lista de palabras e itera por todos los archivos
    proporcionados en el argv, con la función de reemplazar */
    word_list = extract_words_from_file(argv[1]);
    for (i = 2; i < argc; i++) {
        replace_words(argv[i], word_list);
        if (i + 1!= argc)
            printf("\n--\n");
    }

    return 0;
}