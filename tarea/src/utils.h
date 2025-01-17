#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdlib.h>
#include <stdio.h>

#include "list.h"

FILE *open_file(char *path, char *mode);
void verify_malloc(void *ptr);
Node *extract_words_from_file(char *path);
void replace_words(char *path, Node *l);

#endif