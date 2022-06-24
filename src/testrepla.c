#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pair.h"
#include "list.h"
#include "repla.h"

int main(int argc, char **argv) {
    Node *word_list;
    int i;

    if (argc < 3) {
        printf("Uso: %s <archivo pares> {lista de nombres de archivos}.\n", argv[0]);
        exit(1);
    }

    /* Extrae la lista de palabras e itera por todos los archivos
    proporcionados en el argv, con la función de reemplazar */
    word_list = extract_words_from_file(argv[1]);
    for (i = 2; i < argc; i++) {
        replace_words(argv[i], word_list);
        if (i + 1!= argc)
            printf("\n--\n");
    }
    List_destroy(word_list);

    return 0;
}