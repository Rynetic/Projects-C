#include "shunting_yard.h"

#include "my_error.h"

int final(struct dlist *l, struct dlist *ol, struct node *node)
{
    node = dlist_pop(ol);
    while (node != NULL && node->type != PARG)
    {
        dlist_push_back(l, node);
        node = dlist_pop(ol);
    }
    if (node != NULL)
        return err_parad(MISSING_CLOSING);

    return 0;
}

int add_parad(struct dlist *l, struct dlist *ol)
{
    struct node *noeud = dlist_get(ol);
    while (noeud != NULL && noeud->type != PARG)
    {
        dlist_push_back(l, dlist_pop(ol));
        noeud = dlist_get(ol);
    }
    if (noeud == NULL)
        return err_parad(NO_BEGINNING);
    free_node(dlist_pop(ol));
    return 0;
}

int add_operator(struct dlist *l, struct dlist *ol, struct node *node,
                 enum type type)
{
    struct node *noeud = dlist_get(ol);
    while (noeud != NULL && noeud->type != PARG && noeud->type > type)
    {
        dlist_push_back(l, dlist_pop(ol));
        noeud = dlist_get(ol);
    }
    dlist_push_front(ol, node);

    return 0;
}
int shunting_yard(struct dlist *l, struct dlist *ol, struct node *node)
{
    if (node == NULL)
        return final(l, ol, node);
    enum type type = node->type;
    if (type == LEAF)
    {
        dlist_push_back(l, node);
        return 0;
    }

    else if (type == PARG)
    {
        dlist_push_front(ol, node);
        return 0;
    }

    else if (type == PARD)
    {
        free_node(node);
        return add_parad(l, ol);
    }
    return add_operator(l, ol, node, type);
}
