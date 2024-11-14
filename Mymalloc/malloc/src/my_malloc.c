#include "my_malloc.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct global_meta global = {
    .head = NULL,
    .large_mutex = PTHREAD_MUTEX_INITIALIZER,
    .buckets = { { NULL, 32, NULL, 0, PTHREAD_MUTEX_INITIALIZER },
                 { NULL, 64, NULL, 0, PTHREAD_MUTEX_INITIALIZER },
                 { NULL, 128, NULL, 0, PTHREAD_MUTEX_INITIALIZER },
                 { NULL, 256, NULL, 0, PTHREAD_MUTEX_INITIALIZER },
                 { NULL, 512, NULL, 0, PTHREAD_MUTEX_INITIALIZER },
                 { NULL, 1024, NULL, 0, PTHREAD_MUTEX_INITIALIZER },
                 { NULL, 2048, NULL, 0, PTHREAD_MUTEX_INITIALIZER },
                 { NULL, 4096, NULL, 0, PTHREAD_MUTEX_INITIALIZER } },
    .page_size = 4096
};
size_t align_size(size_t size)
{
    size_t alignment = sizeof(long double);
    return ((size + alignment - 1) / alignment) * alignment;
}
void *page_start(void *ptr)
{
    char *base = ptr;
    return base - ((size_t)base & (PAGE_SIZE - 1));
}
size_t bucket_index(size_t size)
{
    size_t aligned_size = align_size(size);
    size_t index = 0;
    size_t current_size = MIN_ALLOC;
    while (index < NUM_BUCKETS && current_size < aligned_size)
    {
        current_size *= 2;
        index++;
    }
    if (index < NUM_BUCKETS)
    {
        return index;
    }
    else
    {
        return NUM_BUCKETS;
    }
}
void init_bucket(struct bucket *bucket, size_t size)
{
    size_t total_size = PAGE_SIZE;
    void *page = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (page == MAP_FAILED)
    {
        return;
    }
    bucket->page = page;
    bucket->block_size = size;
    bucket->used_count = 0;
    bucket->free_blocks = page;
    struct free_block *current = bucket->free_blocks;
    size_t num_blocks = PAGE_SIZE / size;
    for (size_t i = 0; i < num_blocks - 1; i++)
    {
        current->next = (struct free_block *)((char *)current + size);
        current = current->next;
    }
    current->next = NULL;
}
void *alloc_bucket(size_t size)
{
    size_t index = bucket_index(size);
    if (index == NUM_BUCKETS)
    {
        return NULL;
    }
    struct bucket *bucket = &global.buckets[index];
    void *result = NULL;
    pthread_mutex_lock(&bucket->mutex);
    if (bucket->free_blocks == NULL)
    {
        if (bucket->page == NULL
            || bucket->used_count * bucket->block_size >= PAGE_SIZE)
        {
            init_bucket(bucket, bucket->block_size);
        }
        if (bucket->free_blocks == NULL)
        {
            pthread_mutex_unlock(&bucket->mutex);
            return NULL;
        }
    }
    struct free_block *block = bucket->free_blocks;
    bucket->free_blocks = block->next;
    bucket->used_count++;
    result = block;
    pthread_mutex_unlock(&bucket->mutex);
    if (result != NULL)
    {
        memset(result, 0, bucket->block_size);
    }
    return result;
}
void *alloc_large(size_t size)
{
    size_t total_size = sizeof(struct meta_block) + size;
    total_size = ((total_size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
    void *block = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block == MAP_FAILED)
    {
        return NULL;
    }
    pthread_mutex_lock(&global.large_mutex);
    struct meta_block *meta = block;
    meta->size = total_size - sizeof(struct meta_block);
    meta->is_free = 0;
    meta->next = global.head;
    meta->magic = 0xDEADBEEF;
    global.head = meta;
    pthread_mutex_unlock(&global.large_mutex);
    void *data = meta + 1;
    memset(data, 0, meta->size);
    return data;
}
void *my_malloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }
    size_t aligned_size = align_size(size);
    void *ptr = NULL;
    if (aligned_size <= 4096)
    {
        ptr = alloc_bucket(aligned_size);
        if (ptr == NULL)
        {
            ptr = alloc_large(aligned_size);
        }
    }
    else
    {
        ptr = alloc_large(aligned_size);
    }
    return ptr;
}
