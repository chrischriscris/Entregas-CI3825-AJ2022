/**
 * Implementación de codif y funciones de ayuda para su funcionamiento.
 *
 * Autor: Christopher Gómez.
 * Fecha: 29-06-2022.
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

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
    int n_half_unread, half_size;
    char *buf1, *buf2;
    int fd = open(path, O_RDWR);
    if (fd == -1) return -1;

    /* Se obtiene el tamaño del archivo */
    half_size = lseek(fd, 0,  SEEK_END) / 2;
    n_half_unread = half_size;

    /* Asigna memoria para buffers dinámicos */
    buf1 = malloc(sizeof(char) * BUFSIZ);
    buf2 = malloc(sizeof(char) * BUFSIZ);
    if (!buf1 || !buf2) {
        close(fd);
        free(buf1);
        free(buf2);
        return -1;
    }

    while (n_half_unread) {
        int i, m = half_size - n_half_unread;
        int n_bytes = n_half_unread < BUFSIZ ? n_half_unread : BUFSIZ;

        /* Lee bloques de la izquierda y derecha */
        lseek(fd, m, SEEK_SET);
        read(fd, buf1, n_bytes);

        lseek(fd, -m - n_bytes , SEEK_END);
        read(fd, buf2, n_bytes);

        /* Intercambia los buffers en orden inverso */
        for (i = 0 ; i < n_bytes; i++) {
            int j = n_bytes - i - 1;
            char temp = buf2[j];
            buf2[j] = buf1[i];
            buf1[i] = temp;
        }

        /* Escribe nuevamente los bloques */
        lseek(fd, m, SEEK_SET);
        write(fd, buf1, n_bytes);

        lseek(fd, -m - n_bytes , SEEK_END);
        n_half_unread -= write(fd, buf2, n_bytes);
    }

    free(buf1);
    free(buf2);

    close(fd);
    return 0;
}