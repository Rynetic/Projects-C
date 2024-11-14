#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "dlist.h"
#include "helpers.h"
#include "lexer.h"
#include "my_error.h"
#include "shunting_yard.h"
#include "string.h"
#include "tree.h"

struct g_root
{
    int g_override_print;
    int g_add_d;
};

struct g_root g_root;

int end_of_parsing(struct dlist *l, struct dlist *ol)
{
    struct node *noeud = dlist_get(l);
    if (noeud->token > USER)
        g_root.g_override_print = 1;
    if (add_par(l, ol, PARD) < 0)
        exit(1);
    if (g_root.g_override_print == 0)
    {
        struct node *anoeud = init_node();
        anoeud->type = AND;
        anoeud->data->value = dupstr("AND");
        shunting_yard(l, ol, anoeud);

        struct node *noeudp = init_node();
        noeudp->type = LEAF;
        noeudp->data->value = dupstr("PRINT");
        noeudp->token = PRINT;
        shunting_yard(l, ol, noeudp);
    }
    return shunting_yard(l, ol, NULL);
}
int process_line(struct dlist *l, struct dlist *ol, char **s, int *pos)
{
    if (read_pref(s, pos, l, ol) < 0)
        return -1;
    if (s[*pos] == NULL)
        return end_of_parsing(l, ol);
    struct node *noeud;
    int op = eat_op(s, pos);
    if (op != -1)
    {
        noeud = init_node();
        noeud->type = op;
        noeud->data->value = op == 0 ? dupstr("OR") : dupstr("AND");
        if (shunting_yard(l, ol, noeud) < 0)
            return -1;
    }
    if (read_pref(s, pos, l, ol) < 0)
        return -1;

    if ((noeud = get_leaf(s, pos, l, ol)) == NULL)
        return -1;
    if (noeud->token == DELETE)
        g_root.g_add_d = 1;
    if (noeud->token > USER)
        g_root.g_override_print = 1;
    if (shunting_yard(l, ol, noeud) < 0)
        return -1;
    if (noeud->token == TYPE)
        add_par(l, ol, PARD);
    return process_line(l, ol, s, pos);
}

struct node *build_ast(char **s, int *pos, struct root *root)
{
    if (s[*pos] == NULL)
    {
        struct node *noeudp = init_node();
        noeudp->type = LEAF;
        noeudp->token = PRINT;
        return noeudp;
    }

    if (!strcmp(s[*pos], "-a") || !strcmp(s[*pos], "-o"))
        op_err(NO_EXP_BEFORE, s[*pos]);
    s[*pos - 1] = "-a";
    struct dlist *l = dlist_init();
    struct dlist *ol = dlist_init();
    add_par(l, ol, PARG);
    struct node *noeud = NULL;
    if (read_pref(s, pos, l, ol) < 0
        || (noeud = get_leaf(s, pos, l, ol)) == NULL
        || shunting_yard(l, ol, noeud) < 0
        || (noeud->token == TYPE && add_par(l, ol, PARD) < 0)
        || (noeud->token == DELETE && (g_root.g_add_d = 1) < 0)
        || process_line(l, ol, s, pos) < 0)
    {
        dlist_free(l);
        dlist_free(ol);
        return NULL;
    }
    noeud = dlist_pop_last(l);
    connect_node(noeud, l, root);
    dlist_free(l);
    dlist_free(ol);
    return noeud;
}

struct root *parse_line(char **args, int arc)
{
    g_root.g_override_print = 0;
    g_root.g_add_d = 0;
    struct root *root = init_root(arc);
    int pos = 0;
    int x;
    x = eat_option(root, args, &pos);
    if (x == -1)
    {
        fprintf(stderr, "myfind: illegal option -- '%s'\n", args[pos]);
        free_root(root);
        return NULL;
    }
    eat_paths(root, args, &pos);
    if (root->nb_paths == 0)
    {
        add_path(root, ".");
    }
    if ((root->ast = build_ast(args, &pos, root)) == NULL)
    {
        free_root(root);
        return NULL;
    }
    if (g_root.g_add_d)
        root->flags = D;
    return root;
}
