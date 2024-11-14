#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "dlist.h"
#include "tree.h"

struct check_params
{
    struct dlist *l;
    struct dlist *ol;
    struct node *n;
    char **s;
    int *pos;
};

mode_t check_type(char *s, struct dlist *l, struct dlist *ol);
int check_leaf(int token, struct check_params *params);
int check_regular_operator(char **s, int *pos);

#endif /* VALIDATOR_H */
