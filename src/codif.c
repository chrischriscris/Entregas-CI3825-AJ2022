/**
 * Implementación de codif y funciones de ayuda para su funcionamiento.
 *
 * Autor: Christopher Gómez.
 * Fecha: 29-06-2022.
 */
#include <stdio.h>
#include "codif.h"
#include "utils.h"

int reverse_file_content(char *path, void *_);

/**
 * Implementación de la función de interpretador codif.
 * 
 * Por cada archivo regular, invierte su contenido. No imprime nada.
 * 
 * @param root: Directorio raíz del árbol de archivos.
 * @return 0 si la operación fue exitosa.
 *     -1 si ocurrió un error.
 */
int codif(char *root) {
    return walk_dir_tree(root, reverse_file_content, NULL, NULL, NULL);
}

/**
 * Revierte el orden de los caracteres de un archivo.
 * 
 * @param path: String con la ruta del archivo a revertir.
 * @return 0 en caso de exito.
 *    -1 en caso de error.
 */
int reverse_file_content(char *path, void *_) {
    int lpos = 0;
    int rpos;
    FILE *fp = fopen(path, "r+");
    if (!fp) return -1;

    /* Se obtiene de esta forma el tamaño del archivo */
    fseek(fp, -1,  SEEK_END);
    rpos = ftell(fp);

    rewind(fp);
    while (lpos < rpos) {
        char c1, c2;

        /* Guarda los caracteres de ambos extremos */
        c1  = fgetc(fp);
        fseek(fp, rpos, SEEK_SET);
        c2 = fgetc(fp);
        
        /* Se reemplaza el caracter de la derecha por el de la izquierda */
        fseek(fp, rpos--, SEEK_SET);
        fputc(c1, fp);

        /* Se reemplaza el caracter de la izquierda por el de la derecha */
        fseek(fp, lpos, SEEK_SET);
        fputc(c2, fp);
        lpos++;
    }
    fclose(fp);

    return 0;
}