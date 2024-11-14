#ifndef LEXER_H
#define LEXER_H

#include "dlist.h"
#include "sym.h"
#include "tree.h"

char *dupstr(char *s);
int eat_op(char **s, int *pos);
int str_cat(char *data, char *s);
struct node *get_leaf(char **s, int *pos, struct dlist *l, struct dlist *ol);
int add_option(struct root *root, enum flag flags);
int read_pref(char **s, int *pos, struct dlist *l, struct dlist *ol);
int is_op(char **s, int pos);
int eat_token(char **s, int pos);

int eat_option(struct root *root, char **args, int *pos);
int eat_paths(struct root *root, char **args, int *pos);

#endif /* LEXER_H */
