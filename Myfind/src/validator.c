#include "validator.h"

#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "dlist.h"
#include "helpers.h"
#include "my_error.h"
#include "shunting_yard.h"
#include "sym.h"
#include "tree.h"

mode_t get_type(int value)
{
    switch (value)
    {
    case 'b':
        return S_IFBLK;
    case 'c':
        return S_IFCHR;
    case 'd':
        return S_IFDIR;
    case 'f':
        return S_IFREG;
    case 'l':
        return S_IFLNK;
    case 'p':
        return S_IFIFO;
    case 's':
        return S_IFSOCK;
    default:
        return 0;
    }
}

int check_regular_operator(char **s, int *pos)
{
    if (s[*pos - 1]
        && (!strcmp(s[*pos - 1], "(") || !strcmp(s[*pos - 1], "-a")
            || !strcmp(s[*pos - 1], "-o")))
        return op_err(NO_EXP_BEFORE, s[*pos]);

    if (!s[*pos + 1]
        || (!strcmp(s[*pos + 1], ")") || !strcmp(s[*pos + 1], "-o")
            || !strcmp(s[*pos + 1], "-a")))
        return op_err(NO_EXP_AFTER, s[*pos]);
    *pos = *pos + 1;
    return 0;
}

int add_new_type_node(struct dlist *l, struct dlist *ol, mode_t type)
{
    struct node *noeud = init_node();
    noeud->type = LEAF;
    noeud->token = TYPE;
    noeud->data->md = type;

    if (shunting_yard(l, ol, noeud) < 0)
        return -1;

    noeud = init_node();
    noeud->type = OR;
    noeud->data->value = dupstr("OR");
    if (shunting_yard(l, ol, noeud) < 0)
        return -1;
    return 0;
}

mode_t check_type(char *s, struct dlist *l, struct dlist *ol)
{
    mode_t type;
    int x = 0;
    int buf[26] = { 0 };
    add_par(l, ol, PARG);
    for (size_t i = 0; i < strlen(s); i++)
    {
        if (i % 2 == 0)
        {
            x = get_type(s[i]);
            if (x == 0)
                return type_err(UNKNOWN_TYPE, s[i]);
            if ((buf[s[i] - 'a']) == 0)
            {
                type = x;
                buf[s[i] - 'a'] = 1;
            }
            else
                return type_err(DUPLICATED_TYPE, s[i]);
            continue;
        }
        if (s[i] != ',')
            return type_err(MISSING_SEPARATOR, ',');
        if (i == strlen(s) - 1)
            return type_err(BAD_END, ',');
        if (add_new_type_node(l, ol, type) < 0)
            return 0;
    }
    return type;
}

int check_newer(char *file)
{
    if (access(file, F_OK) != 0)
        return err_not_found(FILE_NOT_FOUND, file);
    return 0;
}

int check_user(char *s)
{
    struct passwd *pass;
    if ((pass = getpwnam(s)) == NULL)
        return err_not_found(USER_NOT_FOUND, s);
    return 0;
}

int check_group(char *s)
{
    struct group *groupe;
    if ((groupe = getgrnam(s)) == NULL)
        return err_not_found(GROUP_NOT_FOUND, s);
    return 0;
}

mode_t check_perm(char *s)
{
    char *pe = s;
    if (digit(s[0]) == -1 && (s[0] != '-' && s[0] != '/'))
        err_not_found(INVALID_MODE, pe);
    if (digit(s[0]) <= 0)
        s++;
    while (*s != '\0' && *s == '0')
        s++;
    if (s[0] == '\0' || strlen(s) > 4)
        err_not_found(INVALID_MODE, pe);
    for (size_t i = 0; i < strlen(s); i++)
        if (digit(s[i]) < 0)
            err_not_found(INVALID_MODE, pe);
    return atoi(s);
}

int check_leaf_aux(int token, char **s, int *pos, struct node *n)
{
    switch (token)
    {
    case USER:
        if (check_user(s[*pos]) < 0)
            return -1;
        break;
    case GROUP:
        if (check_group(s[*pos]) < 0)
            return -1;
        break;
    case NAME:
        break;
    case NEWER:
        if (check_newer(s[*pos]) < 0)
            return -1;
        break;
    case PRINT:
    case DELETE:
        n->data->value = dupstr("DELETE");
        return 0;
    default:
        return -1;
    }
    n->data->value = dupstr(s[*pos]);
    *pos = *pos + 1;
    return 0;
}

int check_leaf(int token, struct check_params *params)
{
    mode_t md = 0;
    int x = 0;
    params->n->type = LEAF;
    params->n->token = token;

    switch (token)
    {
    case EXEC:
    case EXECDIR:
        return get_exec_cmd(params->s, params->pos, params->n);
    case TYPE:
        md = check_type(params->s[*(params->pos)], params->l, params->ol);
        if (md == 0)
            return -1;
        params->n->data->md = md;
        *(params->pos) = *(params->pos) + 1;
        return 0;
    case PERM:
        x = check_perm(params->s[*(params->pos)]);
        params->n->data->md = x;
        params->n->data->value = dupstr(params->s[*(params->pos)]);
        *(params->pos) = *(params->pos) + 1;
        return 0;
    default:
        return check_leaf_aux(token, params->s, params->pos, params->n);
    }
}
