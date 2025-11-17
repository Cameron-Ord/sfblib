#include "../include/sfb_allocator.h"
#include "../include/sfb_fb_def.h"
#include "../include/sfb_flags.h"
#include "../include/sfb_threads.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sfb_memcpy(void *restrict dst, void *restrict src, size_t size) {
  memcpy(dst, src, size);
}

void sfb_free(void *chunk) {
  if (chunk) {
    free(chunk);
  }
  chunk = NULL;
}

void *sfb_malloc(const size_t size) {
  void *chunk = malloc(size);
  if (!chunk) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }
  return chunk;
}

void *sfb_calloc(const size_t nmemb, const size_t size) {
  void *chunk = calloc(nmemb, size);
  if (!chunk) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return NULL;
  }
  return chunk;
}

void sfb_free_ctxs(sfb_thread_ctx_renderer *ctxs) {
  if (ctxs) {
    sfb_free(ctxs);
  }
}

void sfb_free_handles(sfb_thread_handle *handles) {
  if (handles) {
    sfb_free(handles);
  }
}

void sfb_free_rect(sfb_rect *rect) {
  if (rect && rect->pixels.data) {
    sfb_free(rect->pixels.data);
    sfb_free(rect);
  }
}

void sfb_free_framebuffer(sfb_framebuffer *f) {
  if (f) {
    if (f->meta.flags & SFB_ENABLE_MULTITHREADED) {
      sfb_close_threads(f);
    }

    if (f->pixels.data) {
      sfb_free(f->pixels.data);
    }

    sfb_free(f);
  }
}
