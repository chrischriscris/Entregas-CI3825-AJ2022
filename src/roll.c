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

    /* Se obtiene el tamaño del archivo */
    fseek(fp, 0,  SEEK_END);
    size = ftell(fp);
    n_unread = size;

    /* Asigna memoria para buffers dinámicos */
    buf1 = malloc(sizeof(char) * BUFSIZ);
    buf2 = malloc(sizeof(char) * n);
    if (!buf1 || !buf2) {
        fclose(fp);
        free(buf1);
        free(buf2);
        return -1;
    }

    /* Dependiendo del signo de n, se rota a la izq o der */
    if (n < 0) {

    } else if (n > 0) {
        /* Se guardan los n caracteres al final */
        fseek(fp, size - n, SEEK_SET);
        n_unread -= fread(buf2, 1, n, fp);

        /* Va rodando bloques hacia atrás */
        while (n_unread >= BUFSIZ) {
            fseek(fp, n_unread - BUFSIZ, SEEK_SET);
            fread(buf1, 1, BUFSIZ, fp);
            fseek(fp, n - BUFSIZ, SEEK_CUR);
            n_unread -= fwrite(buf1, 1, BUFSIZ, fp);
        }

        /* Escribe el bloque de tamaño menor al buffer */
        rewind(fp);
        fread(buf1, 1, n_unread, fp);
        fseek(fp, n, SEEK_SET);
        n_unread -= fwrite(buf1, 1, n_unread, fp);

        /* Escribe los n caracteres finales al comienzo */
        rewind(fp);
        fwrite(buf2, 1, n, fp);
    }

    printf("n_unread = %d\n", n_unread);
    free(buf1);
    free(buf2);

    fclose(fp);
    return 0;
}