#ifndef __WC_H__
#define __WC_H__

#include "list.h"

Node *extract_words_from_file(char *path);
int replace_words(char *path, Node *l);
char *rand_filename();

#endif