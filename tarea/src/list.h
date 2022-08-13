#ifndef __LIST_H__
#define __LIST_H__

#include <aio.h>

#include "pair.h"

/**
 * Estructura de nodo para lista circular doblemente enlazada.
 */
typedef struct Node {
    struct Node *prev, *next; /** Elementos anterior y siguiente */
    Pair *data; /** Dato del nodo, de tipo Pair */
    int length; /** Longitud de la primera palabra del par */
} Node;

Node *List_new();
u_int8_t List_push(Node **head, Pair *data, int length);
void List_print(Node *head);

#endif