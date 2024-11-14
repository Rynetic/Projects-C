#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <sys/stat.h>

#include "tree.h"

struct file
{
    char *name;
    char *full_name;
    struct stat stat;
};

void init_functions(int (**f)(struct file *file, struct node *n,
                              struct root *r));

#endif /* FUNCTIONS_H */
