#ifndef __WC_H__
#define __WC_H__

#include "list.h"

Node *extract_words_from_file(char *path);
int repla(char *path, void *l);
char *rand_filename();

#endif