#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

#define NUM_BUCKETS 8
#define MIN_ALLOC 32
#define PAGE_SIZE 4096

struct free_block
{
    struct free_block *next;
};

struct bucket
{
    struct free_block *free_blocks;
    size_t block_size;
    void *page;
    size_t used_count;
    pthread_mutex_t mutex;
};

struct meta_block
{
    size_t size;
    int is_free;
    struct meta_block *next;
    size_t magic;
};

struct global_meta
{
    struct meta_block *head;
    pthread_mutex_t large_mutex;
    struct bucket buckets[NUM_BUCKETS];
    size_t page_size;
};

extern struct global_meta global;

void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);
void *my_calloc(size_t number, size_t size);
size_t align_size(size_t size);
void *page_start(void *ptr);
size_t bucket_index(size_t size);
void *alloc_bucket(size_t size);
void *alloc_large(size_t size);
void init_bucket(struct bucket *bucket, size_t size);

#endif /* MY_MALLOC_H */
