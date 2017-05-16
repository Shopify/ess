#include "dlmalloc.hpp"
#include "error.hpp"
#include "memory_pool.hpp"
#include "units.hpp"
#include <sys/mman.h>
#include <unistd.h>
#include <cstdint>

struct me_memory_pool {
  mspace mspace_;
  uint8_t *start;
  std::size_t capacity;
};

#define CAPACITY_MIN ((std::size_t)(256 * KiB))
#define CAPACITY_MAX ((std::size_t)(256 * MiB))
#define ALLOC_MAX ((std::size_t)(256 * MiB))

static std::size_t round_capacity(std::size_t capacity) {
  std::size_t page_size = (std::size_t)sysconf(_SC_PAGE_SIZE);
  std::size_t partial_page_p = capacity & (page_size - 1);
  if (partial_page_p)
    capacity = (capacity & ~(page_size - 1)) + page_size;
  return capacity;
}

struct me_memory_pool *me_memory_pool_new(std::size_t capacity) {
  std::size_t rounded_capacity = round_capacity(capacity);
  if (rounded_capacity < CAPACITY_MIN || CAPACITY_MAX < rounded_capacity) {
    leave(status_code::bad_capacity);
  }

  std::uint8_t *bytes = reinterpret_cast<std::uint8_t *>(
    mmap(NULL, rounded_capacity, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  if (bytes == MAP_FAILED) {
    leave(status_code::mmap_failed);
  }

  mspace mspace_ = create_mspace_with_base(bytes, rounded_capacity, 0);
  mspace_set_footprint_limit(mspace_, rounded_capacity);
  struct me_memory_pool *self = static_cast<struct me_memory_pool *>(
    mspace_malloc(mspace_, sizeof(struct me_memory_pool)));
  self->mspace_ = mspace_;
  self->start = bytes;
  self->capacity = rounded_capacity;

  return self;
}

struct meminfo me_memory_pool_info(struct me_memory_pool *self) {
  struct meminfo info;
  struct mallinfo dlinfo = mspace_mallinfo(self->mspace_);
  info.arena = dlinfo.arena;
  info.hblkhd = dlinfo.hblkhd;
  info.uordblks = dlinfo.uordblks;
  info.fordblks = dlinfo.fordblks;
  return info;
}

std::size_t me_memory_pool_get_capacity(struct me_memory_pool *self) {
  return self->capacity;
}

void *me_memory_pool_malloc(struct me_memory_pool *self, std::size_t size) {
  return mspace_malloc(self->mspace_, size);
}

void *me_memory_pool_realloc(struct me_memory_pool *self, void *block, std::size_t size) {
  return mspace_realloc(self->mspace_, block, size);
}

void me_memory_pool_free(struct me_memory_pool *self, void *block) {
  return mspace_free(self->mspace_, block);
}

void me_memory_pool_destroy(struct me_memory_pool *self) {
  uint8_t *start = self->start;
  std::size_t capacity = self->capacity;
  destroy_mspace(self->mspace_);
  munmap(start, capacity);
}
