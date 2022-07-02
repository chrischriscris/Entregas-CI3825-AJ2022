#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

int rotate_file_content(char *path, void *m);

int roll(char *root, int n) {
    return walk_dir_tree(root, rotate_file_content, NULL, &n, NULL);
}

/**
 * Rota los caracteres de un archivo.
 * 
 * @param path: String con la ruta del archivo a revertir.
 * @return 0 en caso de exito.
 *    -1 en caso de error.
 */
int rotate_file_content(char *path, void *m) {
    int n = *(int *) m, n_unread, size;
    char *buf1, *buf2;
    FILE *fp = fopen(path, "r+");
    if (!fp) return -1;

    if (n == 0) return 0;

    /* Se obtiene el tamaño del archivo */
    fseek(fp, 0,  SEEK_END);
    size = ftell(fp);
    n_unread = size;

    /* Asigna memoria para buffers dinámicos */
    buf1 = malloc(sizeof(char) * BUFSIZ);
    buf2 = malloc(sizeof(char) * abs(n));
    if (!buf1 || !buf2) {
        fclose(fp);
        free(buf1);
        free(buf2);
        return -1;
    }

    /* Dependiendo del signo de n, se rota a la izq o der */
    if (n < 0) {
        n = -n;

        /* Se guardan los n caracteres al comienzo */
        rewind(fp);
        n_unread -= fread(buf2, 1, n, fp);

        /* Va rotando bloques n posiciones desde el bloque
        más a la izquierda al más a la derecha */
        while (n_unread) {
            int n_bytes = n_unread < BUFSIZ ? n_unread : BUFSIZ;

            /* Toma un bloque */
            fseek(fp, -n_unread, SEEK_END);
            fread(buf1, 1, n_bytes, fp);

            /* Lo coloca adelante n posiciones */
            fseek(fp, -n - n_bytes, SEEK_CUR);
            n_unread -= fwrite(buf1, 1, n_bytes, fp);
        }

        /* Escribe los n caracteres del comienzo al final */
        fseek(fp, -n, SEEK_END);
        fwrite(buf2, 1, n, fp);
    } else if (n > 0) {
        /* Se guardan los n caracteres al final */
        fseek(fp, size - n, SEEK_SET);
        n_unread -= fread(buf2, 1, n, fp);

        /* Va rotando bloques n posiciones desde el bloque
        más a la derecha al más a la izquierda */
        while (n_unread) {
            int n_bytes = n_unread < BUFSIZ ? n_unread : BUFSIZ;

            /* Toma un bloque */
            fseek(fp, n_unread - n_bytes, SEEK_SET);
            fread(buf1, 1, n_bytes, fp);

            /* Lo coloca atrás n posiciones */
            fseek(fp, n - n_bytes, SEEK_CUR);
            n_unread -= fwrite(buf1, 1, n_bytes, fp);
        }

        /* Escribe los n caracteres finales al comienzo */
        rewind(fp);
        fwrite(buf2, 1, n, fp);
    }
    free(buf1);
    free(buf2);

    fclose(fp);
    return 0;
}