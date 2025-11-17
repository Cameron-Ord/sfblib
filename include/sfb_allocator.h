#ifndef SFB_ALLOCATOR_H
#define SFB_ALLOCATOR_H

#include <stddef.h>

typedef struct sfb_thread_handle sfb_thread_handle;
typedef struct sfb_thread_ctx_renderer sfb_thread_ctx_renderer;
typedef struct sfb_light_source sfb_light_source;
typedef struct sfb_rect sfb_rect;
typedef struct sfb_framebuffer sfb_framebuffer;

void sfb_memcpy(void *dst, void *src, size_t size);
void sfb_free_framebuffer(sfb_framebuffer *f);
void sfb_free_rect(sfb_rect *rect);
void sfb_free(void *chunk);
void *sfb_malloc(const size_t size);
void *sfb_calloc(const size_t nmemb, const size_t size);
void sfb_free_handles(sfb_thread_handle *handles);
void sfb_free_ctxs(sfb_thread_ctx_renderer *ctxs);
#endif
