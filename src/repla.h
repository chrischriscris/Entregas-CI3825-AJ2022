#ifndef __REPLA_H__
#define __REPLA_H__

#include "list.h"

Node *extract_words_from_file(char *path);
void replace_words(char *path, Node *l);

#endif