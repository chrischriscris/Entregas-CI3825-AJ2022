#ifndef __LIST_H__
#define __LIST_H__

#include <aio.h>

#include "pair.h"

typedef struct List List;

List *List_new();
u_int8_t List_push(List *list, Pair *data);
void List_print(List *list);

#endif