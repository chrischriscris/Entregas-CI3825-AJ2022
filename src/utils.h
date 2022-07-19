#ifndef __UTILS_H__
#define __UTILS_H__

#include <aio.h>

void *safe_malloc(size_t n);
int **initialize_pipes(int n);

#endif