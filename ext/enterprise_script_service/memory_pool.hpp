#ifndef ENTERPRISE_SCRIPT_SERVICE_MEMORY_POOL_H
#define ENTERPRISE_SCRIPT_SERVICE_MEMORY_POOL_H

#include <cstddef>

struct meminfo {
  std::size_t arena;
  std::size_t hblkhd;   /* space in mmapped regions */
  std::size_t uordblks; /* total allocated space */
  std::size_t fordblks; /* total free space */
};

struct me_memory_pool;

struct me_memory_pool *me_memory_pool_new(std::size_t capacity);
void me_memory_pool_destroy(struct me_memory_pool *self);

struct meminfo me_memory_pool_info(struct me_memory_pool *self);
std::size_t me_memory_pool_get_capacity(struct me_memory_pool *self);
void *me_memory_pool_malloc(struct me_memory_pool *self, std::size_t size);
void *me_memory_pool_realloc(struct me_memory_pool *self, void *block, std::size_t size);
void me_memory_pool_free(struct me_memory_pool *self, void *block);

#endif
