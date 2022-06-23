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
#include <time.h>

void count_chars_and_lines(char *path, int *chars, int *lines) {
    FILE *fp;
    int c;

    fp = fopen(path, "r");
    if (!fp) return;

    *chars = 0;
    *lines = 0;
    
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            (*lines)++;
        }
        (*chars)++;
    }

    fclose(fp);
}