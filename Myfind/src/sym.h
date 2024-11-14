#ifndef SYM_H
#define SYM_H
/*
#define S_IFMT     0170000
#define S_IFSOCK   0140000
#define S_IFLNK    0120000
#define S_IFREG    0100000
#define S_IFBLK    0060000
#define S_IFDIR    0040000
#define S_IFCHR    0020000
#define S_IFIFO    0010000

*/

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

enum token
{
    NAME = 0,
    TYPE,
    NEWER,
    GROUP,
    PERM,
    USER,
    EXEC,
    EXECP,
    PRINT,
    EXECDIR,
    DELETE,
};

enum type
{
    OR = 0,
    AND,
    NOT,
    PARG,
    PARD,
    LEAF
};

enum flag
{
    L = 1 << 1,
    H = 1 << 2,
    P = 1 << 3,
    D = 1 << 4
};

#endif /* SYM_H */
