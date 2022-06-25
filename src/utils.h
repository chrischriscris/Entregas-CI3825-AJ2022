#ifndef __WALK_DIR_TREE_H__
#define __WALK_DIR_TREE_H__

int is_directory(char *path);
int walk_dir_tree(char *dirpath);
int is_regular_file(char *path);

#endif