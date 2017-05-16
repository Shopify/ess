#ifndef MALLOC_FAIL_H
#define MALLOC_FAIL_H

#include "mpdecimal.h"
#include <stdio.h>

extern int alloc_count;
extern int alloc_fail;
extern int alloc_idx;
void *mpd_malloc_count(void *data, size_t size);
void *mpd_calloc_count(void *data, size_t nmemb, size_t size);
void *mpd_realloc_count(void *data, void *ptr, size_t size);
void *mpd_malloc_fail(void *data, size_t size);
void *mpd_calloc_fail(void *data, size_t nmemb, size_t size);
void *mpd_realloc_fail(void *data, void *ptr, size_t size);

void mpd_set_alloc_count(mpd_context_t *ctx);
void mpd_set_alloc_fail(mpd_context_t *ctx);
void mpd_set_alloc(mpd_context_t *ctx);


#endif
