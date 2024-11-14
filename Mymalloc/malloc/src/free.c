#define _GNU_SOURCE
#include <sys/mman.h>

#include "my_malloc.h"

void bucket_add(void *ptr, size_t size)
{
    size_t index = bucket_index(size);
    if (index == (size_t)-1)
    {
        return;
    }
    struct bucket *bucket = &global.buckets[index];
    pthread_mutex_lock(&bucket->mutex);
    struct free_block *block = ptr;
    block->next = bucket->free_blocks;
    bucket->free_blocks = block;
    bucket->used_count--;
    if (bucket->used_count == 0)
    {
        munmap(bucket->page, PAGE_SIZE);
        bucket->page = NULL;
        bucket->free_blocks = NULL;
    }
    pthread_mutex_unlock(&bucket->mutex);
}
void free_large_block(struct meta_block *meta)
{
    pthread_mutex_lock(&global.large_mutex);
    if (meta->size > PAGE_SIZE * 1024)
    {
        pthread_mutex_unlock(&global.large_mutex);
        return;
    }
    struct meta_block *prev = NULL;
    struct meta_block *current = global.head;
    while (current != NULL && current != meta)
    {
        if (current->magic != 0xDEADBEEF)
        {
            pthread_mutex_unlock(&global.large_mutex);
            return;
        }
        prev = current;
        current = current->next;
    }
    if (current != NULL)
    {
        if (prev != NULL)
        {
            prev->next = current->next;
        }
        else
        {
            global.head = current->next;
        }
        munmap(current, sizeof(struct meta_block) + current->size);
    }
    pthread_mutex_unlock(&global.large_mutex);
}
void free_bucket_block(void *ptr, void *page_addr)
{
    for (size_t i = 0; i < NUM_BUCKETS; i++)
    {
        if (global.buckets[i].page == page_addr)
        {
            bucket_add(ptr, global.buckets[i].block_size);
            return;
        }
    }
}

void my_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    void *page_addr = page_start(ptr);
    if (page_addr == NULL)
    {
        return;
    }
    struct meta_block *meta = page_addr;
    if (meta->magic == 0xDEADBEEF)
    {
        free_large_block(meta);
    }
    else
    {
        free_bucket_block(ptr, page_addr);
    }
}

size_t get_old_size(void *page_addr, struct meta_block *meta)
{
    if (meta->magic == 0xDEADBEEF)
    {
        return meta->size;
    }
    else
    {
        for (size_t i = 0; i < NUM_BUCKETS; i++)
        {
            if (global.buckets[i].page == page_addr)
            {
                return global.buckets[i].block_size;
            }
        }
    }
    return 0;
}
void copy_memory(void *dst, void *src, size_t size)
{
    char *d = dst;
    char *s = src;
    for (size_t i = 0; i < size; i++)
    {
        d[i] = s[i];
    }
}
void *try_mremap_block(void *ptr, size_t new_size)
{
    void *page_addr = page_start(ptr);
    struct meta_block *meta = page_addr;
    if (meta->magic != 0xDEADBEEF)
    {
        return NULL;
    }
    size_t old_total = sizeof(struct meta_block) + meta->size;
    size_t new_total = sizeof(struct meta_block) + new_size;
    old_total = ((old_total + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
    new_total = ((new_total + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
    pthread_mutex_lock(&global.large_mutex);
    void *new_addr = mremap(meta, old_total, new_total, MREMAP_MAYMOVE);
    if (new_addr == MAP_FAILED)
    {
        pthread_mutex_unlock(&global.large_mutex);
        return NULL;
    }
    meta = new_addr;
    meta->size = new_total - sizeof(struct meta_block);
    struct meta_block *prev = NULL;
    struct meta_block *current = global.head;
    while (current != NULL && current != page_addr)
    {
        prev = current;
        current = current->next;
    }
    if (current != NULL)
    {
        if (prev != NULL)
        {
            prev->next = meta;
        }
        else
        {
            global.head = meta;
        }
    }
    pthread_mutex_unlock(&global.large_mutex);
    meta = new_addr;
    return meta + 1;
}
void *my_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
    {
        return my_malloc(size);
    }
    if (size == 0)
    {
        my_free(ptr);
        return NULL;
    }
    void *page_addr = page_start(ptr);
    struct meta_block *meta = page_addr;
    if (meta->magic == 0xDEADBEEF)
    {
        void *new_ptr = try_mremap_block(ptr, size);
        if (new_ptr != NULL)
        {
            return new_ptr;
        }
    }
    void *new_ptr = my_malloc(size);
    if (new_ptr == NULL)
    {
        return NULL;
    }
    size_t old_size = get_old_size(page_addr, meta);
    size_t copy_size;
    if (old_size < size)
    {
        copy_size = old_size;
    }
    else
    {
        copy_size = size;
    }
    copy_memory(new_ptr, ptr, copy_size);
    my_free(ptr);
    return new_ptr;
}
