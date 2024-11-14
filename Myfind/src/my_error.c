#include "my_error.h"

#include <stdio.h>
#include <stdlib.h>

int op_err(enum error err, char *s)
{
    switch (err)
    {
    case NO_EXP_BEFORE:
        fprintf(stderr, "myfind: %s: no expression before %s\n", s, s);
        break;

    case NO_EXP_AFTER:
        fprintf(stderr, "myfind: %s: no expression after %s\n", s, s);
        break;

    default:
        printf("NOT MATCHED");
    }
    exit(1);
}

int type_err(enum error err, char c)
{
    switch (err)
    {
    case UNKNOWN_TYPE:
        fprintf(stderr, "myfind: Unknown argument to -type: %c\n", c);
        break;
    case DUPLICATED_TYPE:
        fprintf(stderr,
                "myfind: Duplicate file type '%c' in the "
                "argument list to -type.\n",
                c);
        break;
    case MISSING_SEPARATOR:
        fprintf(stderr,
                "my find: Must separate multiple arguments to"
                "-type using: '%c'\n",
                c);
        break;
    case BAD_END:
        fprintf(stderr,
                "myfind: Last file type in list argument to "
                "-type is missing, i.e., list is ending on: '%c'\n",
                c);
        break;
    default:
        fprintf(stderr, "NOT MATCHED\n");
        exit(1);
    }
    exit(1);
    ;
}
int err(enum error err, char *s)
{
    switch (err)
    {
    case MISSING_ARG:
        fprintf(stderr,
                "myfind: %s: requires additional "
                "arguments \n",
                s);
        break;
    case UNKNOWN:
        fprintf(stderr,
                "myfind: %s unknown primary or "
                "operator\n",
                s);
        break;

    case PARD_ERR:

        fprintf(stderr, "myfind: %s: empty inner expression\n", s);
        break;
    case NOT_NEXT:
        fprintf(stderr, "myfind: %s: no following expression\n", s);
        break;

    case EMPTY_NOT:
        fprintf(stderr, "myfind: !: nothing between ! and -%s\n", s);
        break;
    default:
        fprintf(stderr, "NOT MATCHED");
    }
    exit(1);
}

int err_not_found(enum error err, char *s)
{
    switch (err)
    {
    case FILE_NOT_FOUND:

        fprintf(stderr, "%s: No such file or directory\n", s);
        break;
    case USER_NOT_FOUND:
        fprintf(stderr,
                "myfind: ‘%s’ is not the "
                "name of a known user\n",
                s);
        break;

    case GROUP_NOT_FOUND:
        fprintf(stderr,
                "myfind: ‘%s’ is not the "
                "name of a known group\n",
                s);
        break;
    case INVALID_MODE:
        fprintf(stderr, "myfind: invalid mode `%s` \n", s);
        break;
    default:
        fprintf(stderr, "NOT MATCHED");
    }
    exit(1);
}

int err_parad(enum error err)
{
    switch (err)
    {
    case NO_BEGINNING:
        fprintf(stderr, "myfind: ): no beginning '('\n");
        break;
    case MISSING_CLOSING:

        fprintf(stderr, "myfind: (: missing closing ')'\n");
        break;
    default:
        fprintf(stderr, "NOY MATCHED");
    }

    exit(1);
}
