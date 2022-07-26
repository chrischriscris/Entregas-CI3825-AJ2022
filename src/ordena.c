#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "sequence.h"
#include "utils.h"

int walk_dir_tree_ordena(char *root, Sequence **seq);

int main(int argc, char *argv[]) {
    char *root, *outfile;
    Sequence *seq = Sequence_new(0);
    Sequence *seqs_arr[1];

    seqs_arr[0] = seq;

    /* Valida los argumentos de línea de comandos */
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <raiz> <archivo salida>\n", argv[0]);
        return 1;
    }

    /* Obtiene los argumentos */
    root = argv[1];
    outfile = argv[2];

    if (walk_dir_tree_ordena(root, &seq) == -1) {
        fprintf(stderr, "Ha habido un error recorriendo los directorios\n");
        return 1;
    }

    if (!Sequence_write_merged(seqs_arr, 1, outfile)) {
        fprintf(stderr, "Ha habido un error en el programa\n");
        return 1;
    }

    Sequence_destroy(seq);
    return 0;
}

int walk_dir_tree_ordena(char *root, Sequence **seq) {
    struct dirent *entry;

    /* Abre el directorio */
    DIR *dir = opendir(root);
    if (!dir) return -1;

    while ((entry = readdir(dir))) {
        char *d_name = entry->d_name;

        /* Verifica que no sean '.' o '..' para no caer em un ciclo
        infinito */
        if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0) {
            continue;
        } else {
            char *full_path = malloc(sizeof(char) *
                (strlen(root) + strlen(d_name) + 2));
            
            int is_dir;

            /* Construye la ruta completa */
            sprintf(full_path, "%s/%s", root, d_name);

            is_dir = is_directory(full_path);
            if (is_dir == -1) {
                free(full_path);
                continue;
            }

            if (is_dir) {
                /* Si es directorio, se explora recursivamente */
                if (walk_dir_tree_ordena(full_path, seq) == -1
                ) {
                    free(full_path);
                    continue;
                }
            } else {
                /* De lo contrario, se hace una operación sobre el archivo,
                de ser regular*/
                if (is_regular_file(full_path) && is_txt_file(full_path)) {
                    /* Extrae la secuencia del archivo */
                    Sequence *new_seq = Sequence_extract_from_file(full_path);
                    if (!new_seq) {
                        fprintf(stderr, "Error al extraer secuencia del archivo %s\n", full_path);
                        free(full_path);
                        continue;
                    }

                    /* Ordena la secuencia */
                    Sequence_sort(new_seq);

                    /* Mezcla con la secuencia recién ordenada */
                    if (!Sequence_merge(seq, new_seq))
                        fprintf(stderr, "Error al mezclar una secuencia\n");
                }
            }
            free(full_path);
        }
    }
    closedir(dir);

    return 0;
}