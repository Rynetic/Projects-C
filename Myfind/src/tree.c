#include "tree.h"

#include <stdio.h>
#include <stdlib.h>

#include "dlist.h"
struct node *init_node(void)
{
    struct node *noeud = malloc(sizeof(struct node));
    if (noeud == NULL)
    {
        perror("malloc");
        exit(1);
    }
    noeud->type = 0;
    noeud->token = 0;
    noeud->right = NULL;
    noeud->left = NULL;
    noeud->data = malloc(sizeof(struct data));
    if (noeud->data == NULL)
    {
        perror("malloc");
        exit(1);
    }
    noeud->data->value = NULL;
    noeud->data->md = 0;
    noeud->data->argc = 0;
    noeud->data->argl = NULL;
    return noeud;
}

struct root *init_root(int arc)
{
    struct root *racine = malloc(sizeof(struct root));
    if (racine == NULL)
    {
        perror("malloc tree.h l 39 \n");
        exit(1);
    }

    racine->nb_paths = 0;
    racine->paths = malloc(arc * sizeof(char *));
    if (racine->paths == NULL)
    {
        perror("malloc tree.c line 47\n");
        exit(1);
    }
    racine->err = 0;
    racine->wating_list = dlist_init();
    racine->paths[0] = NULL;
    racine->flags = 0;
    racine->ast = NULL;
    return racine;
}

void free_node(struct node *node)
{
    if (node == NULL)
        return;
    if (node->data->value != NULL)
        free(node->data->value);
    if (node->data->argl)
    {
        for (int i = 0; i < node->data->argc; i++)
        {
            free(node->data->argl[i]);
        }

        free(node->data->argl);
    }
    free(node->data);
    node->data = NULL;
    free_node(node->right);
    free_node(node->left);
    free(node);
    node = NULL;
}

void free_root(struct root *root)
{
    int i = 0;
    while (i < root->nb_paths)
    {
        free(root->paths[i]);
        i++;
    }
    free(root->paths);
    free_node(root->ast);
    free(root->wating_list->head);
    free(root->wating_list->tail);
    free(root->wating_list);
    free(root);
}

void print_node(struct node *node)
{
    if (node == NULL)
    {
        printf("NULL\n");
        return;
    }
    if (node->data->value)
        fprintf(stdout, "%s \n", node->data->value);
    for (int i = 0; i < node->data->argc; i++)
        printf("%s ", node->data->argl[i]);
    fprintf(stdout, "left\n");
    print_node(node->left);
    fprintf(stdout, "right\n");
    print_node(node->right);
}

int connect_node(struct node *n, struct dlist *l, struct root *root)
{
    if (n == NULL)
        return 0;
    if (n->type == LEAF && n->token == EXECP)
    {
        dlist_push_front(root->wating_list, n);
        return 0;
    }
    if (n->type == LEAF)
        return 0;
    n->right = dlist_pop_last(l);
    connect_node(n->right, l, root);
    if (n->type != NOT)
    {
        n->left = dlist_pop_last(l);
        connect_node(n->left, l, root);
    }
    return 0;
}
