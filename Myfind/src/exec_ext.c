#include "exec_ext.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

short exec_cmd(char *path, char **argv)
{
    int x;
    int s;
    do
    {
        s = fork();
    } while (s == -1 && errno == EAGAIN);

    switch (s)
    {
    case -1:
        perror("myfind");
        return 1;

    case 0:
        if (execvp(path, argv) != 0)
        {
            perror("myfind");
            exit(1);
        }
        break;
    default:
        waitpid(s, &x, 0);
        return (x == 0) ? 0 : 1;
    }
    return 0;
}
