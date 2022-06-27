#ifndef __UTILS_H__
#define __UTILS_H__

int is_regular_file(char *path);
int is_directory(char *path);
int walk_dir_tree(char *root, int (*fn1)(char *, void *),
    int (*fn2)(char *, void *), void *arg1, void *arg2);

#endif