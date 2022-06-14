/**
 * Entry-point de programa de sustituir.
 *
 *
 * Write down usage instructions here.
 *
 * Autor: Christopher Gómez.
 * Fecha: 04-06-2022.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pair.h"
#include "list.h"
#include "utils.h"

int main(int argc, char **argv) {
    Node *word_list;

    if (argc < 3) {
        printf("Uso: sustituir <archivo pares> {lista de nombres de archivos}.\n");
        exit(1);
    }

    word_list = extract_words_from_file(argv[1]);

    List_print(word_list);

    replace_words(argv[2], word_list);

    return 0;
}