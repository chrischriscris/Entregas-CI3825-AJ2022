#ifndef __UTILS_H__
#define __UTILS_H__

#include <aio.h>
#include "Sequence.h"

void *safe_malloc(size_t n);
int **initialize_pipes(int n);
Sequence *extract_sequence_from_file(char *path);

#endif