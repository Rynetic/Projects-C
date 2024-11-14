#include "allocator.h"

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

struct blk_allocator *blka_new(void)
{
    struct blk_allocator *alloc = malloc(sizeof(struct blk_allocator));
    if (!alloc)
        return NULL;
    alloc->meta = NULL;
    return alloc;
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t total = sizeof(struct blk_meta) + size;
    size_t besoin = (total + page_size - 1) / page_size;
    size_t alloc = besoin * page_size;

    struct blk_meta *block = mmap(NULL, alloc, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block == MAP_FAILED)
    {
        return NULL;
    }

    block->next = blka->meta;
    block->size = alloc - sizeof(struct blk_meta);
    blka->meta = block;

    return block;
}

void blka_free(struct blk_meta *blk)
{
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t total = sizeof(struct blk_meta) + blk->size;
    size_t pages = (total + page_size - 1) / page_size;
    munmap(blk, pages * page_size);
}

void blka_pop(struct blk_allocator *blka)
{
    if (!blka->meta)
    {
        return;
    }
    struct blk_meta *block = blka->meta;
    blka->meta = blka->meta->next;
    blka_free(block);
}

void blka_delete(struct blk_allocator *blka)
{
    while (blka->meta)
    {
        blka_pop(blka);
    }
    free(blka);
}
