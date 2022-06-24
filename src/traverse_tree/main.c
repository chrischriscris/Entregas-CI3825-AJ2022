#include <stdlib.h> 
#include <stdio.h>  /* printf */
#include <dirent.h> /* opendir, readdir, closedir */
#include <string.h> /* strcmp */
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Verifica si una ruta dada hace referencia a un directorio distinto
 *  de . y ..
 *
 * @param path: String con la ruta a verificar.
 * @return 1 si es directorio.
 *     0 en caso contrario.
 *     -1 en caso de error.
 */
int is_directory(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return -1;

    return st.st_mode & __S_IFDIR;
}

/**
 * Recorre un arbol de directorios en DFS.
 * 
 * @param dirpath: String con la ruta del directorio a recorrer.
 * @param callback: Funcion a llamar por cada elemento encontrado.
 * @param data: Datos a pasar a la funcion callback.
 * @return 0 en caso de exito
 */
int traverse_dir_tree(char *dirpath) {
    DIR *dir;
    struct dirent *entry;
    char *full_path;

    /* Abre el directorio */
    dir = opendir(dirpath);
    if (!dir) return;

    /* Lee recursivamente los archivos */
    while ((entry = readdir(dir))) {
        struct stat st;
        char *dirname = entry->d_name;
        int include;
        
        /* Booleano para no incluir '.' ni '..', que lleva a ciclo
        infinito (se accede al mismo directorio o al padre) */
        include = strcmp(dirname, ".") != 0 && strcmp(dirname, "..") != 0;

        stat(dirname, &st);
        if ((st.st_mode & S_IFDIR) && include) {
            printf("%s\n", dirname);

            full_path = malloc(strlen(dirpath) + strlen(dirname) + 2);
            sprintf(full_path, "%s/%s", dirpath, dirname);

            printf("%s\n", full_path);

            traverse_dir_tree(full_path);
            free(full_path);
        }
    
        entry = readdir(dir);
    }
}

int main(int argc, char **argv) {
    traverse_dir_tree(argv[1]);
    return 0;
}