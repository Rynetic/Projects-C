#include "helpers.h"

#include <stdio.h>
#include <stdlib.h>

#include "dlist.h"
#include "shunting_yard.h"
#include "string.h"
#include "tree.h"

int digit(char c)
{
    int i = c - '0';
    if (i < 0 || i > 7)
        return -1;
    return i;
}

char *dupstr(char *s)
{
    char *x = malloc(strlen(s) + 1);
    if (x == NULL)
        return NULL;
    memset(x, 0, strlen(s) + 1);
    memcpy(x, s, strlen(s));
    return x;
}

int validate_exep(char *s, int pos, int end)
{
    if (!strcmp(s, "{}") && pos < end - 1)
    {
        fprintf(stderr,
                "myfind: only one instance of {}"
                " is supported with -exec ... +\n");

        exit(1);
    }
    char *x = strchr(s, '{');
    if (x && s != x && *(x + 1) == '}')
    {
        fprintf(stderr,
                " myfind: in ‘-exec ... {} +’ the ‘{}’ must"
                "appear by itself, but you specified ‘%s’\n",
                s);
        exit(1);
    }
    return 0;
}
int validate_arg(char **s, int start)
{
    int fin = start;
    while (s[fin] && strcmp(s[fin], ";") && strcmp(s[fin], "+"))
        fin++;
    if (s[fin] == NULL)
    {
        fprintf(stderr,
                "myfind: %s: no terminating"
                "\";\" or \"+\"\n",
                s[start - 1]);
        exit(1);
    }
    if (start == fin)
    {
        fprintf(stderr, "myfind: -exec: invalid argument `%s` \n", s[start]);
        exit(1);
    }
    return fin;
}

int get_exec_cmd(char **s, int *pos, struct node *n)
{
    int start = *pos;
    int fin = validate_arg(s, start);
    int argc = fin - start + 1;
    enum token token = s[fin][0] == ';' ? EXEC : EXECP;

    char **argl = malloc(sizeof(char *) * argc);
    for (int i = start; i < fin; i++)
    {
        if (token == EXECP)
            validate_exep(s[i], i, fin);
        argl[i - start] = dupstr(s[i]);
    }
    argl[fin - start] = NULL;
    if (n->token == EXEC && token == EXECP)
    {
        free(argl[argc - 2]);
        argl[argc - 2] = NULL;
        argc--;
        n->token = EXECP;
    }

    n->data->argc = argc;
    n->data->argl = argl;
    *pos = fin + 1;
    return 0;
}

int add_par(struct dlist *l, struct dlist *ol, enum type type)
{
    struct node *noeud = init_node();
    noeud->type = type;
    return shunting_yard(l, ol, noeud);
}

int add_path(struct root *root, char *path)
{
    root->paths[root->nb_paths] = dupstr(path);
    root->nb_paths++;
    return 0;
}

mode_t get_other_mode(int m)
{
    switch (m)
    {
    case 7:
        return S_IRWXO;
    case 6:
        return S_IWOTH | S_IROTH;
    case 5:
        return S_IROTH | S_IXOTH;
    case 4:
        return S_IROTH;
    case 3:
        return S_IWOTH | S_IXOTH;
    case 2:
        return S_IWOTH;
    case 1:
        return S_IXOTH;
    default:
        return 0;
    }
}

mode_t get_group_mode(int m)
{
    switch (m)
    {
    case 7:
        return S_IRWXG;
    case 6:
        return S_IRGRP | S_IWGRP;
    case 5:
        return S_IRGRP | S_IXGRP;
    case 4:
        return S_IRGRP;
    case 3:
        return S_IWGRP | S_IXGRP;
    case 2:
        return S_IWGRP;
    case 1:
        return S_IXGRP;
    default:
        return 0;
    }
}

mode_t get_user_mode(int m)
{
    switch (m)
    {
    case 7:
        return S_IRWXU;
    case 6:
        return S_IRUSR | S_IWUSR;
    case 5:
        return S_IRUSR | S_IXUSR;
    case 4:
        return S_IRUSR;
    case 3:
        return S_IWUSR | S_IXUSR;
    case 2:
        return S_IWUSR;
    case 1:
        return S_IXUSR;
    default:
        return 0;
    }
}
