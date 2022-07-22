#ifndef __UTILS_H__
#define __UTILS_H__

#include <aio.h>

void *safe_malloc(size_t n);
int **initialize_pipes(int n);
int walk_dir_tree_proc(char *root, int sorter_queue, int *to_sorter);
int is_directory(char *path);
int is_regular_file(char *path);
int is_txt_file(char *path);

#endif