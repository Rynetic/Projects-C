#include "dlist.h"

#include <stdio.h>
#include <stdlib.h>

struct dlist *dlist_init(void)
{
    struct dlist *l = malloc(sizeof(struct dlist));
    if (l == NULL)
    {
        perror("malloc");
        exit(1);
    }
    l->size = 0;
    l->head = malloc(sizeof(struct dlist_item));
    l->tail = malloc(sizeof(struct dlist_item));
    if (l->head == NULL || l->tail == NULL)
    {
        perror("malloc");
        exit(1);
    }
    l->tail->next = NULL;
    l->tail->prev = NULL;
    l->head->next = NULL;
    l->head->prev = NULL;
    l->head->data = NULL;
    l->tail->data = NULL;

    return l;
}

void dlist_free(struct dlist *l)
{
    void *noeud;
    while ((noeud = dlist_pop(l)) != NULL)
        free_node(noeud);
    free(l->head);
    free(l->tail);
    free(l);
}
void dlist_print(const struct dlist *list)
{
    if (list == NULL)
    {
        perror("malloc");
        exit(1);
    }
    if (!list->size)
    {
        printf("\n Empty \n");
        return;
    }
    struct dlist_item *next = list->head->next;
    for (size_t i = 0; i < list->size; ++i)
    {
        if (next->data->data->value)
            printf("%s|", next->data->data->value);
        else
            printf("SOME|");
        next = next->next;
    }
    putchar('\n');
}

int dlist_push_front(struct dlist *list, struct node *element)
{
    if (list == NULL)
        return 0;
    struct dlist_item *item = malloc(sizeof(*item));
    if (item == NULL)
    {
        return 0;
    }
    item->data = element;
    item->next = NULL;
    item->prev = NULL;
    if (list->head->next == NULL)
    {
        list->head->next = item;
        list->tail->prev = item;
        item->next = list->tail;
        item->prev = list->head;
        ++list->size;
        return 0;
    }

    item->next = list->head->next;
    item->prev = list->head;
    list->head->next->prev = item;
    list->head->next = item;
    ++list->size;
    return 0;
}

int dlist_push_back(struct dlist *list, struct node *element)
{
    if (list == NULL)
        return 0;
    struct dlist_item *item = malloc(sizeof(*item));
    if (item == NULL)
    {
        return 0;
    }
    item->data = element;
    item->next = NULL;
    item->prev = NULL;
    if (list->head->next == NULL)
    {
        list->head->next = item;
        list->tail->prev = item;
        item->next = list->tail;
        item->prev = list->head;
        ++list->size;
        return 0;
    }

    item->prev = list->tail->prev;
    item->next = list->tail;
    list->tail->prev->next = item;
    list->tail->prev = item;
    ++list->size;
    return 0;
}
size_t dlist_size(const struct dlist *list)
{
    return list->size;
}

void *dlist_get(struct dlist *list)
{
    if (list->size == 0)
        return NULL;
    return list->head->next->data;
}

void *dlist_pop_last(struct dlist *list)
{
    if (list->size == 0)
        return NULL;
    struct dlist_item *item = list->tail->prev;
    list->tail->prev = item->prev;
    item->prev->next = list->tail;
    void *res = item->data;
    free(item);
    list->size--;
    return res;
}
void *dlist_pop(struct dlist *list)
{
    if (list->size == 0)
        return NULL;
    struct dlist_item *item = list->head->next;

    list->head->next = item->next;
    item->next->prev = list->head;
    void *x = item->data;
    free(item);
    list->size--;
    return x;
}
