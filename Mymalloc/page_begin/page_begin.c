#include "page_begin.h"

void *page_begin(void *ptr, size_t page_size)
{
    char *base = ptr;
    return base - ((size_t)base & (page_size - 1));
}
