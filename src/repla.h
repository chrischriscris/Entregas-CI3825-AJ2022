#ifndef __REPLA_H__
#define __REPLA_H__

#include <stdio.h>
#include "list.h"

Node *extract_words_from_file(char *path);
int replace_words(char *path, Node *l);
char *rand_filename();

#endif