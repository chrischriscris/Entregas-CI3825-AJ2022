#ifndef __WALK_DIR_TREE_H__
#define __WALK_DIR_TREE_H__

int is_directory(char *path);
int walk_dir_tree(char *root, int (*fn1)(char *, void *),
    int (*fn2)(char *, void *), void *arg1, void *arg2);
int is_regular_file(char *path);

#endif