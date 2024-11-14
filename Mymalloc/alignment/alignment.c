#include "alignment.h"

#include <stddef.h>

size_t align(size_t size)
{
    size_t alignment = sizeof(long double);
    size_t reste = size % alignment;
    if (reste == 0)
    {
        return size;
    }
    size_t size_final;
    if (__builtin_add_overflow(size, alignment - reste, &size_final))
    {
        return 0;
    }
    return size_final;
}
