#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

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
    int fd; 
    
    /* Si n es 0 no se hace nada */
    if (n == 0) return 0;

    fd = open(path, O_RDWR);
    if (fd == -1) return -1;

    /* Se obtiene el tamaño del archivo */
    size = lseek(fd, 0,  SEEK_END);
    n_unread = size;

    /* Asigna memoria para buffers dinámicos */
    buf1 = malloc(sizeof(char) * BUFSIZ);
    buf2 = malloc(sizeof(char) * abs(n));
    if (!buf1 || !buf2) {
        close(fd);
        free(buf1);
        free(buf2);
        return -1;
    }

    /* Dependiendo del signo de n, se rota a la izq o der */
    if (n < 0) {
        /* Se guardan los n caracteres al comienzo */
        lseek(fd, 0, SEEK_SET);
        n_unread -= read(fd, buf2, -n);

        /* Va rotando bloques n posiciones desde el bloque
        más a la izquierda al más a la derecha */
        while (n_unread) {
            int n_bytes = n_unread < BUFSIZ ? n_unread : BUFSIZ;

            /* Toma un bloque */
            lseek(fd, -n_unread, SEEK_END);
            read(fd, buf1, n_bytes);

            /* Lo coloca adelante n posiciones */
            lseek(fd, n - n_bytes, SEEK_CUR);
            n_unread -= write(fd, buf1, n_bytes);
        }

        /* Escribe los n caracteres del comienzo al final */
        lseek(fd, n, SEEK_END);
        write(fd, buf2, -n);
    } else if (n > 0) {
        /* Se guardan los n caracteres al final */
        lseek(fd, size - n, SEEK_SET);
        n_unread -= read(fd, buf2, n);

        /* Va rotando bloques n posiciones desde el bloque
        más a la derecha al más a la izquierda */
        while (n_unread) {
            int n_bytes = n_unread < BUFSIZ ? n_unread : BUFSIZ;

            /* Toma un bloque */
            lseek(fd, n_unread - n_bytes, SEEK_SET);
            read(fd, buf1, n_bytes);

            /* Lo coloca atrás n posiciones */
            lseek(fd, n - n_bytes, SEEK_CUR);
            n_unread -= write(fd, buf1, n_bytes);
        }

        /* Escribe los n caracteres finales al comienzo */
        lseek(fd, 0, SEEK_SET);
        write(fd, buf2, n);
    }
    free(buf1);
    free(buf2);

    close(fd);
    return 0;
}