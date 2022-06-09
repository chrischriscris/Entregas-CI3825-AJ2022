/**
 * Implementación de lista circular doblemente enlazada.
 *
 * Autor: Christopher Gómez.
 * Fecha: 04-06-2022.
 */
#include <stdlib.h>
#include <stdio.h>

#include "list.h"
#include "pair.h"

/**
 * Estructura de nodo para lista circular doblemente enlazada.
 */
typedef struct Node {
    struct Node *prev, *next; /** Elementos anterior y siguiente */
    Pair *data; /** Dato del nodo, de tipo Pair */
} Node;

/**
 * Estructura mínima de lista.
 */
struct List {
    Node *head; /* Apuntador a la cabeza de la lista */
};

/**
 * Crea una lista circular doblemente enlazada sin elementos.
 *
 * @return Un apuntador a List si la creación fue exitosa.
 *     NULL en caso contrario.
 */
List *List_new() {
    /* Asigna memoria dinámicamente para crear la lista */
    Node *head;
    List *list;

    head = malloc(sizeof(Node));
    if (!head)
        return NULL;

    list =  malloc(sizeof(List));
    if (!list)
        return NULL;

    /* La cabeza de la lista apunta a sí misma */
    head->next = head;
    head->prev = head;
    head->data = NULL; /* */

    /* Se guarda el apuntador a la cabeza en la lista */
    list->head = head;

    return list;
}

/**
 * Añade un nodo al comienzo de la lista.
 *
 * @param list: Apuntador a estructura lista.
 * @param data: dato a almacenar en el nodo, de tipo Pair.
 * @return 1 si la operación fue exitosa.
 *     0 en caso contrario.
 */
u_int8_t List_push(List *list, Pair *data) {
    Node *head = list->head;

    if (!head->data) {
        /* Si la lista es nueva, coloca el dato en el nodo */
        head->data = data;
    } else {
        /* De otra forma, crea una nueva entrada para la lista
        dinámicamente */
        Node *new_node = malloc(sizeof(Node));
        if (!new_node)
            return 0;

        new_node->data = data;
        new_node->prev = head->prev;
        new_node->next = head;
        head->prev->next = new_node;
        head->prev = new_node;
    }

    return 1;
}

/**
 * Imprime todos los elementos de la lista.
 *
 * Para propósitos de depuración.
 *
 * @param list: Apuntador a estructura List.
 */
void List_print(List *list) {
    Node *head = list->head;
    Node *cur = head;

    printf("[");
    do {
        Pair *p = cur->data;

        Pair_print(p);
        
        cur = cur->next;
        if (cur != head)
            printf(", ");
    } while (cur != head);
    printf("]");
}