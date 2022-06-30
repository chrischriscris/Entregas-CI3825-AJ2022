/**
 * Implementación de codif y funciones de ayuda para su funcionamiento.
 *
 * Autor: Christopher Gómez.
 * Fecha: 29-06-2022.
 */
#include <stdio.h>
#include <stdlib.h>
#include "codif.h"
#include "utils.h"

int reverse_file_content(char *path, void *_);
void reverse_array(char *arr, int n);

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
    int n_half_unread, half_size, parity;
    char *buf1, *buf2;
    FILE *fp = fopen(path, "r+");
    if (!fp) return -1;

    /* Se obtiene el tamaño del archivo */
    fseek(fp, 0,  SEEK_END);
    parity = ftell(fp) % 2;
    half_size = ftell(fp) / 2;
    n_half_unread = half_size;

    /* Asigna memoria para buffers dinámicos */
    buf1 = malloc(sizeof(char) * BUFSIZ);
    buf2 = malloc(sizeof(char) * BUFSIZ);
    if (!buf1 || !buf2) {
        fclose(fp);
        free(buf1);
        free(buf2);
        return -1;
    }

    while (n_half_unread >= BUFSIZ) {
        int m = half_size - n_half_unread;

        /* Lee bloques de la izquierda y derecha */
        fseek(fp, m, SEEK_SET);
        fread(buf1, 1, BUFSIZ, fp);

        fseek(fp, -m - BUFSIZ , SEEK_END);
        fread(buf2, 1, BUFSIZ, fp);

        /* Revierte los bloques en memoria principal */
        reverse_array(buf1, BUFSIZ);
        reverse_array(buf2, BUFSIZ);

        /* Escribe el bloque de la derecha a la izquierda y viceversa*/
        fseek(fp, m, SEEK_SET);
        fwrite(buf2, 1, BUFSIZ, fp);

        fseek(fp, -m - BUFSIZ , SEEK_END);
        n_half_unread -= fwrite(buf1, 1, BUFSIZ, fp);
    }

    /* Revierte y escribe los últimos bloques de tamaño menor al buffer */
    fseek(fp, half_size - n_half_unread, SEEK_SET);
    fread(buf1, 1, n_half_unread, fp);

    /* Se mueve 1 si el tamaño del archivo era impar */
    fseek(fp, parity, SEEK_CUR);
    fread(buf2, 1, n_half_unread, fp);

    reverse_array(buf1, n_half_unread);
    reverse_array(buf2, n_half_unread);

    fseek(fp, half_size - n_half_unread, SEEK_SET);
    fwrite(buf2, 1, n_half_unread, fp);

    fseek(fp, parity, SEEK_CUR);
    n_half_unread -= fwrite(buf1, 1, n_half_unread, fp);

    free(buf1);
    free(buf2);

    fclose(fp);
    return 0;
}

void reverse_array(char *arr, int n) {
    int i;
    for (i = 0; i < n / 2; i++) {
        int temp = arr[i];
        arr[i] = arr[n-i-1];
        arr[n-i-1] = temp;
    }
}