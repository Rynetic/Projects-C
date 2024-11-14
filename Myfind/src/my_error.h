#ifndef MY_ERROR_H
#define MY_ERROR_H

enum error
{
    MISSING_ARG = 0,
    UNKNOWN,
    PARD_ERR,
    NOT_NEXT,
    EMPTY_NOT,
    NO_EXP_BEFORE,
    NO_EXP_AFTER,
    UNKNOWN_TYPE,
    DUPLICATED_TYPE,
    MISSING_SEPARATOR,
    BAD_END,
    FILE_NOT_FOUND,
    USER_NOT_FOUND,
    GROUP_NOT_FOUND,
    NO_BEGINNING,
    MISSING_CLOSING,
    INVALID_MODE
};

int err(enum error err, char *s);
int type_err(enum error err, char c);
int op_err(enum error err, char *s);
int err_not_found(enum error, char *file);
int err_parad(enum error err);

#endif /* MY_ERROR_H */
