#include "recycler.h"

#include <errno.h>
#include <stdlib.h>

struct recycler *recycler_create(size_t block_size, size_t total_size)
{
    if (block_size == 0 || total_size == 0 || total_size % block_size != 0
        || block_size % sizeof(size_t) != 0)
    {
        return NULL;
    }
    struct recycler *r = malloc(sizeof(struct recycler));
    if (r == NULL)
    {
        return NULL;
    }
    size_t block_count = total_size / block_size;
    r->chunk = malloc(block_count * sizeof(struct free_list));
    if (r->chunk == NULL)
    {
        free(r);
        return NULL;
    }
    r->block_size = block_size;
    r->capacity = block_count;
    r->free = r->chunk;
    struct free_list *actuel = r->chunk;
    for (size_t i = 0; i < block_count - 1; i++)
    {
        actuel[i].next = &actuel[i + 1];
    }
    actuel[block_count - 1].next = NULL;
    return r;
}
void recycler_destroy(struct recycler *r)
{
    if (r == NULL)
    {
        return;
    }
    free(r->chunk);
    free(r);
}
void *recycler_allocate(struct recycler *r)
{
    if (r == NULL || r->free == NULL)
    {
        return NULL;
    }
    struct free_list *block = r->free;
    r->free = block->next;
    return block;
}
void recycler_free(struct recycler *r, void *block)
{
    if (r == NULL || block == NULL)
    {
        return;
    }
    struct free_list *free_block = block;
    free_block->next = r->free;
    r->free = free_block;
}
