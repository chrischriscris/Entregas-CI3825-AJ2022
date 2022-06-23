/**
 * Utiles para el programa sustituir.
 *
 * Autor: Christopher Gómez.
 * Fecha: 13-06-2022.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pair.h"
#include "list.h"
#include "utils.h"

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