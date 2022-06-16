/**
 * Implementación de lista circular, donde cada nodo contiene un
 * par de cadenas de caracteres y la longitud de la cadena de la
 * primera coordenada .
 * 
 * Soporta las operaciones de creación e inserción ordenada.
 *
 * Autor: Christopher Gómez.
 * Fecha: 04-06-2022.
 */
#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "pair.h"

/**
 * Crea una lista doblemente enlazada sin elementos.
 *
 * @return Un apuntador a la cabeza de la lista si la creación
 *     fue exitosa.
 *     NULL en caso contrario.
 */
Node *List_new() {
    /* Asigna memoria dinámicamente para crear la lista */
    Node *head = malloc(sizeof(Node));
    if (!head)
        return NULL;

    /* La cabeza de la lista apunta a sí misma */
    head->next = NULL;
    head->prev = NULL;
    head->data = NULL;

    return head;
}

/**
 * Añade un nodo en orden en la lista.
 *
 * @param head: Apuntador a la direccion de la cabeza de la lista.
 * @param data: Dato a almacenar en el nodo, de tipo Pair.
 * @param length: Longitud de la primera palabra del par.
 * @return 1 si la operación fue exitosa.
 *     0 en caso contrario.
 */
u_int8_t List_push(Node **list, Pair *data, int length) {
    Node *head = *list;

    if (!head->data) {
        /* Si la lista es nueva, coloca el dato en el nodo */
        head->data = data;
        head->length = length;
    } else {
        /* De otra forma, crea una nueva entrada para la lista
        dinámicamente */
        Node *new_node = malloc(sizeof(Node));
        if (!new_node)
            return 0;

        /* Coloca el dato en el nodo */
        new_node->data = data;
        new_node->length = length;

        if (length >= head->length) {
            /* Si el campo length es >= que el de la cabeza */
            new_node->next = head;
            new_node->prev = NULL;
            head->prev = new_node;

            /* Ahora el nuevo nodo es la cabeza de la lista */
            *list = new_node;
        } else {
            Node *cur = head;

            /* Busca la posición en la que debe insertarse el nuevo nodo:
            antes del primer nodo que contenga un campo length menor o
            igual, o al final de la lista si no lo hay */
            while (cur->next != NULL && cur->next->length > length)
                cur = cur->next;

            /* Se agrega el nodo entre cur y cur->next */
            new_node->next = cur->next;
            new_node->prev = cur;
            if (cur->next)
                cur->next->prev = new_node;
            cur->next = new_node;
        }
    }

    return 1;
}

/**
 * Imprime todos los elementos de la lista.
 *
 * Para propósitos de depuración.
 *
 * @param list: Apuntador a la cabeza de la lista
 */
void List_print(Node *head) {
    Node *cur = head;

    printf("[");
    while (cur != NULL && cur->data != NULL) {
        Pair *p = cur->data;
        Pair_print(p);
        if (cur->next)
            printf(", ");
        cur = cur->next;
    }
    printf("]");
}