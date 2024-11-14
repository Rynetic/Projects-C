#include "my_malloc.h"

void *my_calloc(size_t number, size_t size)
{
    if (number == 0 || size == 0)
    {
        return NULL;
    }
    size_t total;
    if (size > 0 && number > (size_t)-1 / size)
    {
        return NULL;
    }
    total = number * size;
    void *ptr = my_malloc(total);
    if (ptr == NULL)
    {
        return NULL;
    }
    char *p = ptr;
    for (size_t i = 0; i < total; i++)
    {
        p[i] = 0;
    }
    return ptr;
}
