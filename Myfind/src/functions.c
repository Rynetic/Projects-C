#include "functions.h"

#include <fnmatch.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "exec_ext.h"
#include "helpers.h"
#include "my_error.h"
#include "stdio.h"
#include "sym.h"
#include "tree.h"

int name(struct file *file, struct node *n, struct root *r)
{
    if (!r)
        exit(1);
    return !fnmatch(n->data->value, file->name, 0);
}
int print(struct file *file, struct node *n, struct root *r)
{
    if (!r)
        exit(1);
    if (n->token == PRINT)
    {
        printf("%s\n", file->full_name);
        return 1;
    }
    return 0;
}

int type(struct file *file, struct node *n, struct root *r)
{
    if (!r)
        exit(1);
    if ((file->stat.st_mode & S_IFMT) == n->data->md)
        return 1;
    return 0;
}

int newer(struct file *file __attribute__((unused)), struct node *n,
          struct root *r)
{
    if (!r)
        exit(1);
    struct stat x;
    if (r->flags == L || r->flags == H)
    {
        if (lstat(n->data->value, &x) < 0)
        {
            perror("newer");
            exit(1);
        }
    }
    else
    {
        if (stat(n->data->value, &x) < 0)
        {
            perror("newer");
            exit(1);
        }
    }

    if (0)
    {
        return 0;
    }
    return 1;
}

static char *split_f1(char *original, char *file_name)
{
    size_t x = 0;
    size_t o_taille = strlen(original) + 1;
    size_t f_taille = strlen(file_name);
    char *ostr = malloc(o_taille);
    if (ostr == NULL)
    {
        perror("malloc");
        exit(1);
    }
    memset(ostr, 0, o_taille);
    for (size_t i = 0; i < strlen(original); i++)
    {
        if (original[i] != '{'
            || (original[i] == '{' && original[i + 1] != '}'))
        {
            ostr[x] = original[i];
            x++;
        }
        else
        {
            ostr = realloc(ostr, o_taille + f_taille);
            if (ostr == NULL)
                exit(1);
            memcpy(ostr + x, file_name, f_taille);
            x = x + f_taille;
            o_taille += f_taille;
            i++;
        }
        ostr[x] = '\0';
    }
    return ostr;
}

static char *get_dir(char *full_name)
{
    int l = strlen(full_name);
    while (l > 0 && full_name[l] != '/')
        l--;
    char *x = malloc(l + 1);
    if (x == NULL)
    {
        perror("myfind ");
        exit(1);
    }
    memset(x, 0, l);
    memcpy(x, full_name, l);
    x[l] = '\0';
    return x;
}

static int exec_dir(struct file *file, char **cmd)
{
    int x = 0;
    char buf[4076];
    char *new_doss = get_dir(file->full_name);
    if (getcwd(buf, sizeof(buf)) == NULL
        || (strlen(new_doss) && chdir(new_doss) < 0))
    {
        err_not_found(FILE_NOT_FOUND, file->full_name);
    }
    free(new_doss);
    x = exec_cmd(cmd[0], cmd);
    chdir(buf);
    return x;
}

static int exec(struct file *file, struct node *n, struct root *r)
{
    if (!r)
        exit(1);
    char **com = malloc(sizeof(char *) * n->data->argc);
    char *temp = malloc(strlen(file->name) + 3);
    if (com == NULL || com == NULL)
        exit(1);
    snprintf(temp, strlen(file->name) + 3, "./%s", file->name);
    temp[strlen(file->name) + 2] = '\0';
    for (int i = 0; i < n->data->argc - 1; i++)
        com[i] = split_f1(n->data->argl[i],
                          n->token == EXEC ? file->full_name : temp);
    com[n->data->argc - 1] = NULL;
    int x = 0;

    if (n->token == EXECDIR)
        x = exec_dir(file, com);
    else
        x = exec_cmd(com[0], com);
    int i = 0;
    while (i < n->data->argc)
    {
        free(com[i]);
        i++;
    }
    free(com);
    free(temp);
    return x == 0 ? 1 : 0;
}

static int exec_p(struct file *file, struct node *n, struct root *r)
{
    if (!r)
        exit(1);
    n->data->argc++;
    char **x = realloc(n->data->argl, sizeof(char *) * (n->data->argc + 1));
    if (x == NULL)
        exit(1);
    n->data->argl = x;
    n->data->argl[n->data->argc - 2] = dupstr(file->full_name);
    n->data->argl[n->data->argc - 1] = NULL;
    return 1;
}

static int user(struct file *file, struct node *n, struct root *r)
{
    if (!r | !n)
        exit(1);

    struct passwd *pass = getpwuid(file->stat.st_uid);
    if (pass != 0)
        return !(strcmp(pass->pw_name, n->data->value));
    return 0;
}

static int group(struct file *file, struct node *n, struct root *r)
{
    if (!r)
        exit(1);

    struct group *groupe = getgrgid(file->stat.st_gid);
    if (groupe != 0)
        return !(strcmp(groupe->gr_name, n->data->value));
    return 0;
}

static int delete(struct file *file, struct node *n, struct root *r)
{
    if (!n || !r)
        return 0;
    if (remove(file->full_name) < 0)
    {
        fprintf(stderr, "myfind : cannot delete ` %s`: ", file->full_name);
        perror("");
        r->err = 1;
        return 0;
    }
    return 1;
}

int get_mode(int number, int k, int p)
{
    return (((1 << k) - 1) & (number >> (p - 1)));
}
int perm(struct file *file, struct node *n, struct root *r)
{
    if (!r)
        exit(1);

    int other_mode = n->data->md % 10;
    mode_t other_mode_t = get_other_mode(other_mode);
    int group_mode = (n->data->md % 100 - other_mode) / 10;
    mode_t group_mode_t = get_group_mode(group_mode);
    int user_mode = (n->data->md % 1000 - group_mode * 10 - other_mode) / 100;
    mode_t user_mode_t = get_user_mode(user_mode);
    mode_t md = file->stat.st_mode;
    if (n->data->value[0] == '/')
    {
        return ((md & user_mode_t)) || ((md & group_mode_t))
            || ((md & other_mode_t));
    }
    if (n->data->value[0] == '-')
    {
        return ((md & user_mode_t) == user_mode_t)
            && ((md & group_mode_t) == group_mode_t)
            && ((md & other_mode_t) == other_mode_t);
    }

    return (md & (user_mode_t | group_mode_t | other_mode_t))
        == (user_mode_t | group_mode_t | other_mode_t);
}

void init_functions(int (**f)(struct file *file, struct node *n,
                              struct root *r))
{
    f[NAME] = &name;
    f[PRINT] = &print;
    f[TYPE] = &type;
    f[NEWER] = &newer;
    f[EXEC] = &exec;
    f[EXECP] = &exec_p;
    f[USER] = &user;
    f[GROUP] = &group;
    f[PERM] = &perm;
    f[DELETE] = &delete;
    f[EXECDIR] = &exec;
}
