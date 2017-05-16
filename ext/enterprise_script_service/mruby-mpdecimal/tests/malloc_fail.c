#include "mpdecimal.h"
#include <stdlib.h>
#include "malloc_fail.h"


/* Test allocation failures */
int alloc_count;
int alloc_fail;
int alloc_idx;

void *
mpd_malloc_count(void *data, size_t size)
{
  (void)data;
	++alloc_count;
	return malloc(size);
}

void *
mpd_calloc_count(void *data, size_t nmemb, size_t size)
{
  (void)data;
	++alloc_count;
	return calloc(nmemb, size);
}

void *
mpd_realloc_count(void *data, void *ptr, size_t size)
{
  (void)data;
	++alloc_count;
	return realloc(ptr, size);
}


void *
mpd_malloc_fail(void *data, size_t size)
{
  (void)data;
#ifdef TEST_ALLOC
	if (++alloc_idx >= alloc_fail) {
		return NULL;
	}
#endif
	return malloc(size);
}

void *
mpd_calloc_fail(void *data, size_t nmemb, size_t size)
{
  (void)data;
#ifdef TEST_ALLOC
	if (++alloc_idx >= alloc_fail) {
		return NULL;
	}
#endif
	if (rand()%2) {
		return calloc(nmemb, size);
	}
	else {
		return mpd_callocfunc_em(data, nmemb, size);
	}
}

void *
mpd_realloc_fail(void *data, void *ptr, size_t size)
{
  (void)data;
#ifdef TEST_ALLOC
	if (++alloc_idx >= alloc_fail) {
		return NULL;
	}
#endif
	return realloc(ptr, size);
}

static void *
malloc_adaptor(void *data, size_t size)
{
  (void)data;
  return malloc(size);
}

static void *
calloc_adaptor(void *data, size_t count, size_t size)
{
  (void)data;
	return calloc(count, size);
}

static void *
calloc_em_adaptor(void *data, size_t count, size_t size)
{
  (void)data;
	return mpd_callocfunc_em(data, count, size);
}

static void *
realloc_adaptor(void *data, void *mem, size_t size)
{
  (void)data;
  return realloc(mem, size);
}

static void
free_adaptor(void *data, void *mem)
{
  (void)data;
  free(mem);
}

void
mpd_set_alloc_count(mpd_context_t *ctx)
{
	ctx->allocator.mallocfunc = mpd_malloc_count;
	ctx->allocator.callocfunc = mpd_calloc_count;
	ctx->allocator.reallocfunc = mpd_realloc_count;
  ctx->allocator.freefunc = free_adaptor;
  ctx->allocator.data = ctx;

	ctx->traps = MPD_Malloc_error;
	alloc_count = 0;
}

void
mpd_set_alloc_fail(mpd_context_t *ctx)
{
	ctx->allocator.mallocfunc = mpd_malloc_fail;
	ctx->allocator.callocfunc = mpd_calloc_fail;
	ctx->allocator.reallocfunc = mpd_realloc_fail;
  ctx->allocator.freefunc = free_adaptor;
  ctx->allocator.data = ctx;

	ctx->traps = 0;
	alloc_idx = 0;
}

void
mpd_set_alloc(mpd_context_t *ctx)
{
	ctx->allocator.mallocfunc = malloc_adaptor;
	ctx->allocator.callocfunc = (rand() % 2) ? calloc_adaptor : calloc_em_adaptor;
	ctx->allocator.reallocfunc = realloc_adaptor;
  ctx->allocator.freefunc = free_adaptor;
  ctx->allocator.data = ctx;

	ctx->traps = MPD_Malloc_error;
}
