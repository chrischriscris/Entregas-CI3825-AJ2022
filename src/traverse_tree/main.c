#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Verifica si una ruta dada hace referencia a un directorio.
 * 
 * @param path: String con la ruta a verificar.
 * @return 1 si es directorio
 *     - 0 en caso contrario
 *     - 1 en caso de error.
 */
int is_directory(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return -1;

    return st.st_mode & __S_IFDIR;
}

void list_files(char *dirname) {
    DIR *dir;
    struct dirent *entry;
    char *full_path;

    printf("Listing files in %s\n\n", dirname);

    /* Abre el directorio */
    dir = opendir(dirname);
    if (!dir) return;

    /* Lee recursivamente los archivos */
    while (entry = readdir(dir)) {
        struct stat st;
        char *d_name = entry->d_name;
        int include;
        
        /* Booleano para no incluir '.' ni '..', que lleva a ciclo
        infinito */
        include = strcmp(d_name, ".") != 0 && strcmp(d_name, "..") != 0;

        stat(d_name, &st);
        if ((st.st_mode & __S_IFDIR) && include) {
            printf("%s\n", d_name);

            full_path = malloc(strlen(dirname) + strlen(d_name) + 2);
            sprintf(full_path, "%s/%s", dirname, d_name);

            printf("%s\n", full_path);

            list_files(full_path);
            free(full_path);
        }
    
        entry = readdir(dir);
    }
}

int main(int argc, char **argv) {
    list_files(argv[1]);
    return 0;
}