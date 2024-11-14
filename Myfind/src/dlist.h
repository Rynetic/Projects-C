#ifndef DLIST_H
#define DLIST_H
#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

struct dlist_item
{
    struct node *data;
    struct dlist_item *next;
    struct dlist_item *prev;
};

struct dlist
{
    size_t size;
    struct dlist_item *head;
    struct dlist_item *tail;
};

struct dlist *dlist_init(void);
void dlist_print(const struct dlist *list);
int dlist_push_front(struct dlist *list, struct node *element);
int dlist_push_back(struct dlist *list, struct node *element);
size_t dlist_size(const struct dlist *list);
void *dlist_get(struct dlist *list);
void *dlist_pop_last(struct dlist *list);
void *dlist_pop(struct dlist *list);
void dlist_free(struct dlist *l);

#endif /* DLIST_H */
