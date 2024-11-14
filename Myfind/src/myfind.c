#include "myfind.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "exec_ext.h"
#include "functions.h"
#include "parser.h"

struct myfind_ctx
{
    struct root *root;
    int (*functions[11])(struct file *, struct node *, struct root *r);
};

static struct myfind_ctx ctx = { NULL, { 0 } };

int get_file(struct file *file, char *name, char *full_name, int incmd)
{
    int x = 0;
    file->name = name;
    file->full_name = full_name;
    switch (ctx.root->flags)
    {
    case L:
        x = stat(full_name, &file->stat);
        if (x < 0)
            x = lstat(full_name, &file->stat);
        break;
    case P:
        x = lstat(full_name, &file->stat);
        break;
    case H:
        if (incmd)
            x = stat(full_name, &file->stat);
        else
            x = lstat(full_name, &file->stat);
        break;
    default:
        x = lstat(full_name, &file->stat);
    }
    return x;
}

int evalnode(struct file *file, struct node *node)
{
    if (node == NULL)
        return 0;
    if (node->type == LEAF)
        return ctx.functions[node->token](file, node, ctx.root);
    if (node->type == NOT)
        return !evalnode(file, node->right);
    if (node->type == OR)
        return evalnode(file, node->left) || evalnode(file, node->right);
    if (node->type == AND)
        return evalnode(file, node->left) && evalnode(file, node->right);
    return -1;
}

int examinate(struct file *file)
{
    int x = 0;
    if (!(ctx.root->flags & D))
    {
        evalnode(file, ctx.root->ast);
        if (S_ISDIR(file->stat.st_mode)
            && read_path_and_check_files(file->full_name) < 0)
            x = -1;
    }
    else
    {
        if (S_ISDIR(file->stat.st_mode)
            && read_path_and_check_files(file->full_name) < 0)
            x = -1;
        evalnode(file, ctx.root->ast);
    }
    return x;
}

int read_path_and_check_files(char *path)
{
    DIR *d = opendir(path);
    if (d == NULL)
    {
        fprintf(stderr, "myfind: %s", path);
        perror("-->");
        return -1;
    }
    int x = 0;
    struct dirent *f;
    struct file file;
    if (path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = '\0';
    while ((f = readdir(d)))
    {
        if (!strcmp(f->d_name, ".") || !strcmp(f->d_name, ".."))
            continue;
        int taille_new_ch = strlen(path) + strlen(f->d_name) + 2;
        char *newch = malloc(taille_new_ch);
        if (newch == NULL)
            exit(1);
        memset(newch, '\0', taille_new_ch);
        snprintf(newch, taille_new_ch, "%s/%s", path, f->d_name);
        newch[taille_new_ch - 1] = '\0';
        if (get_file(&file, f->d_name, newch, 0) < 0)
        {
            free(newch);
            continue;
        }
        if (examinate(&file) < 0)
            x = -1;
        free(newch);
    }
    closedir(d);
    return x;
}

int find(void)
{
    struct file file;
    int i = 0;
    int x = 0;
    while (i < ctx.root->nb_paths)
    {
        char *d = ctx.root->paths[i];
        if (access(d, F_OK) < 0)
        {
            fprintf(stderr, "myfind : `%s` ", d);
            perror("");
            i++;
            x = -1;
            continue;
        }
        if (get_file(&file, d, d, 1) < 0)
            return -1;
        if (examinate(&file) < 0)
            x = -1;
        i++;
    }
    return x;
}

int exec_wating(struct dlist *l)
{
    struct node *noeud = dlist_pop(l);
    int x = 0;
    while (noeud != NULL)
    {
        if (exec_cmd(noeud->data->argl[0], noeud->data->argl))
            x = 1;
        noeud = dlist_pop(l);
    }
    return x;
}

int main(int arc, char **argv)
{
    argv++;
    ctx.root = parse_line(argv, arc);
    if (ctx.root == NULL || ctx.root->ast == NULL)
        exit(1);
    init_functions(ctx.functions);
    int x = find();
    if (x == -1)
        exit(1);
    x = exec_wating(ctx.root->wating_list);
    if (x != 0 || ctx.root->err)
        exit(1);
    free_root(ctx.root);
    return 0;
}
