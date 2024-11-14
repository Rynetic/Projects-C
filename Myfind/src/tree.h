#ifndef TREE_H
#define TREE_H
#include "dlist.h"
#include "sym.h"

struct data
{
    char *value;
    mode_t md;
    int argc;
    char **argl;
};

struct node
{
    enum type type;
    enum token token;
    struct data *data;
    struct node *right;
    struct node *left;
};

struct root
{
    int nb_paths;
    char **paths;
    int flags;
    int err;
    struct node *ast;
    struct dlist *wating_list;
};
struct node *init_node(void);
struct root *init_root(int arc);
void free_node(struct node *node);
void free_root(struct root *root);
void print_node(struct node *node);
int connect_node(struct node *n, struct dlist *l, struct root *root);

#endif /* TREE_H */
