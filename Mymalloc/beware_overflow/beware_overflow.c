#include "beware_overflow.h"

#include <errno.h>
#include <stddef.h>

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t resultat;
    if (__builtin_mul_overflow(nmemb, size, &resultat))
    {
        errno = ERANGE;
        return NULL;
    }
    char *p = ptr;
    p += resultat;
    return p;
}
