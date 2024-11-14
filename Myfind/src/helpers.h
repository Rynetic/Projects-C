#ifndef HELPERS_H
#define HELPERS_H

#include "dlist.h"
#include "tree.h"
char *dupstr(char *s);
int add_path(struct root *root, char *path);
int get_exec_cmd(char **s, int *pos, struct node *n);
int add_par(struct dlist *l, struct dlist *ol, enum type type);
mode_t get_other_mode(int m);
mode_t get_group_mode(int m);
mode_t get_user_mode(int m);
int digit(char c);

#endif /* HELPERS_H */
