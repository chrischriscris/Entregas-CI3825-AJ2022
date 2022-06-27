#include <stdio.h>
#include "codif.h"
#include "utils.h"

int reverse_file_content(char *path, void *_);

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
    FILE *fp1 = fopen(path, "r+");
    if (!fp1) return -1;

    /* Se obtiene de esta forma el tamaño del archivo */
    fseek(fp1, -1,  SEEK_END);
    rpos = ftell(fp1);

    rewind(fp1);
    while (lpos < rpos) {
        char c1, c2;

        /* Guarda los caracteres de ambos extremos */
        c1  = fgetc(fp1);
        fseek(fp1, rpos, SEEK_SET);
        c2 = fgetc(fp1);
        
        /* Se reemplaza el caracter de la derecha por el de la izquierda */
        fseek(fp1, rpos--, SEEK_SET);
        fputc(c1, fp1);

        /* Se reemplaza el caracter de la izquierda por el de la derecha */
        fseek(fp1, lpos, SEEK_SET);
        fputc(c2, fp1);
        lpos++;
    }
    fclose(fp1);

    return 0;
}