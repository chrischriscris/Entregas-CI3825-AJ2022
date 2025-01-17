/**
 * Implementación de par.
 *
 * Autor: Christopher Gómez.
 * Fecha: 04-06-2022.
 */
#include <stdlib.h>
#include <stdio.h>

#include "pair.h"

/**
 * Crea una par de cadenas de caracteres.
 *
 * @param first: Primer elemento del par.
 * @param second: Primer elemento del par.
 * @return Un apuntador a Pair si la creación fue exitosa.
 *     NULL en caso contrario.
 */
Pair *Pair_new(char *first, char *second) {
    Pair *p = malloc(sizeof(Pair));
    if (!p)
        return NULL;

    p->first = first;
    p->second = second;

    return p;
}

/**
 * Imprime una representación en string del par de cadenas de caracteres.
 *
 * Para propósitos de depuración.
 *
 * @param pair: Apuntador a estructura Pair.
 */
void Pair_print(Pair *pair) {
    printf("('%s', '%s')", pair->first, pair->second);
}