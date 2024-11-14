#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dlist.h"
#include "helpers.h"
#include "my_error.h"
#include "shunting_yard.h"
#include "tree.h"
#include "validator.h"

int eat_option(struct root *root, char **args, int *pos)
{
    if (args[*pos] == NULL || args[*pos][0] != '-')
        return 0;
    else if (strcmp(args[*pos], "-") == 0)
        return 0;

    else if (strcmp(args[*pos], "-H") == 0)
        root->flags = H;

    else if (strcmp(args[*pos], "-L") == 0)
        root->flags = L;

    else if (strcmp(args[*pos], "-d") == 0)
        root->flags = D;

    else if (strcmp(args[*pos], "-P") == 0)
        root->flags = P;
    else
        return 0;
    *pos = *pos + 1;
    return eat_option(root, args, pos);
}

int eat_paths(struct root *root, char **args, int *pos)
{
    if (args[*pos] == NULL)
        return 0;
    if ((args[*pos][0] == '-' && strlen(args[*pos]) > 1)
        || !strcmp(args[*pos], "(") || !strcmp(args[*pos], "!"))
        return 0;
    add_path(root, args[*pos]);
    *pos = *pos + 1;
    return eat_paths(root, args, pos);
}
int add_and(struct dlist *l, struct dlist *ol)
{
    struct node *noeud = init_node();
    noeud->type = AND;
    noeud->data->value = dupstr("AND");
    if (shunting_yard(l, ol, noeud) < 0)
        return -1;
    return 0;
}

int add_and_before_not(char **s, int *pos, struct dlist *l, struct dlist *ol)
{
    if (!strcmp(s[*pos - 1], "-o") || !strcmp(s[*pos - 1], "-a")
        || !strcmp(s[*pos - 1], "(") || !strcmp(s[*pos - 1], "!"))
        return 0;

    return add_and(l, ol);
}

int read_pref_aux(char **s, int *pos, struct dlist *l, struct dlist *ol)
{
    int typ = -1;
    int act = s[*pos][0];
    switch (act)
    {
    case ')':
        typ = PARD;
        break;
    case '(':
        if (s[*pos + 1] == NULL || strcmp(s[*pos + 1], ")") == 0)
            return err(PARD_ERR, "()");
        typ = PARG;
        break;
    case '!':
        if (s[*pos + 1] == NULL || !strcmp(s[*pos + 1], ")"))
            return err(NOT_NEXT, "!");
        if (!strcmp(s[*pos + 1], "-o") || !strcmp(s[*pos + 1], "-a"))
            return err(EMPTY_NOT, s[*pos + 1]);
        add_and_before_not(s, pos, l, ol);
        typ = NOT;
        break;
    default:
        return typ;
    }
    return typ;
}
int read_pref(char **s, int *pos, struct dlist *l, struct dlist *ol)
{
    if (!s[*pos]
        || (strcmp(s[*pos], ")") && strcmp(s[*pos], "!")
            && strcmp(s[*pos], "(")))
        return 0;
    int typ = read_pref_aux(s, pos, l, ol);
    if (typ == -1)
        return 0;
    struct node *noeud = init_node();
    noeud->type = typ;
    noeud->data->value = dupstr(s[*pos]);
    if (shunting_yard(l, ol, noeud) < 0)
        return -1;

    if (!strcmp(s[*pos], ")") && s[*pos + 1] && !strcmp(s[*pos + 1], "("))
        add_and(l, ol);

    *pos = *pos + 1;
    return read_pref(s, pos, l, ol);
}

int eat_op(char **s, int *pos)
{
    if (s[*pos] == NULL)
        return -1;
    if (strcmp(s[*pos], "-o") == 0)
    {
        return check_regular_operator(s, pos) ? -1 : OR;
    }
    if (strcmp(s[*pos], "-a") == 0)
    {
        return check_regular_operator(s, pos) ? -1 : AND;
    }

    if (s[*pos] && strcmp(s[*pos - 1], "!") && strcmp(s[*pos - 1], "("))
        return AND;
    return -1;
}

int eat_token(char **s, int pos)
{
    if (s[pos] == NULL)
        return -1;
    if (strcmp(s[pos], "-print") == 0)
        return PRINT;
    if (strcmp(s[pos], "-delete") == 0)
        return DELETE;

    int x = (s[pos + 1] != NULL);
    if (strcmp(s[pos], "-group") == 0)
        return x > 0 ? GROUP : -2;
    if (strcmp(s[pos], "-perm") == 0)
        return x > 0 ? PERM : -2;
    if (strcmp(s[pos], "-user") == 0)
        return x > 0 ? USER : -2;
    if (strcmp(s[pos], "-name") == 0)
        return x > 0 ? NAME : -2;
    if (strcmp(s[pos], "-type") == 0)
        return x > 0 ? TYPE : -2;
    if (strcmp(s[pos], "-newer") == 0)
        return x > 0 ? NEWER : -2;
    if (strcmp(s[pos], "-exec") == 0)
        return x > 0 ? EXEC : -2;
    if (strcmp(s[pos], "-execdir") == 0)
        return x > 0 ? EXECDIR : -2;
    return -1;
}

struct node *get_leaf(char **s, int *pos, struct dlist *l, struct dlist *ol)
{
    int tok = eat_token(s, *pos);
    struct node *noeud = init_node();
    int x = 0;

    *pos = *pos + 1;

    struct check_params params = {
        .l = l, .ol = ol, .n = noeud, .s = s, .pos = pos
    };

    switch (tok)
    {
    case -2:
        x = err(MISSING_ARG, s[*pos - 1]);
        break;
    case -1:
        x = err(UNKNOWN, s[*pos - 1]);
        break;
    default:
        x = check_leaf(tok, &params);
    }

    if (x < 0)
    {
        free_node(noeud);
        return NULL;
    }

    return noeud;
}
