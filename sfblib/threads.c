#include "../include/sfb_framebuffer.h"
#include "../include/sfb_threads.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CONTROL FLOW
int sfb_get_cores(void) {
#if POSIX
  return sfb_get_cores_posix();
#elif WINDOWS
  // TODO: IMPLEMENT
  return 0;
#endif
}

int sfb_wait_threads(sfb_thread_ctx_renderer *ctxs, int cores) {
#if POSIX
  return sfb_wait_threads_posix(ctxs, cores);
#elif WINDOWS
  return 0;
#endif
}

int sfb_thread_queue_restack(sfb_thread_ctx_renderer *ctx) {
#if POSIX
  return sfb_thread_queue_restack_posix(ctx);
#elif WINDOWS
  return 0;
#endif
}

int sfb_pause_thread(sfb_thread_ctx_renderer *ctx) {
#if POSIX
  return sfb_pause_thread_posix(ctx);
#elif WINDOWS
  return 0;
#endif
}

int sfb_resume_thread(sfb_thread_ctx_renderer *ctx) {
#if POSIX
  return sfb_resume_thread_posix(ctx);
#elif WINDOWS
  return 0;
#endif
}

int sfb_thread_dequeue(sfb_thread_ctx_renderer *ctx) {
#if POSIX
  return sfb_thread_dequeue_posix(ctx);
#elif WINDOWS
  return 0;
#endif
}

int sfb_thread_signal_resume(sfb_thread_ctx_renderer *ctx) {
#if POSIX
  return sfb_thread_signal_resume_posix(ctx);
#elif WINDOWS
  // TODO: IMPLEMENT
  return 0;
#endif
}

int sfb_kill_thread(sfb_thread_ctx_renderer *ctx, sfb_thread_handle *handle) {
#if POSIX
  return sfb_kill_thread_posix(ctx, handle);
#elif WINDOWS
  return 0;
#endif
}

sfb_thread_ctx_renderer *sfb_spawn_threads(sfb_thread_handle *thread_handles,
                                           const int cores) {
#if POSIX
  return sfb_spawn_threads_posix(thread_handles, cores);
#elif WINDOWS
  // TODO: IMPLEMENT
  return NULL;
#endif
}

int sfb_thread_queue_job(sfb_thread_ctx_renderer *ctx, sfb_framebuffer *buf,
                         const sfb_obj *const obj, int y0, int x0) {
#if POSIX
  return sfb_thread_queue_job_posix(ctx, buf, obj, y0, x0);
#elif WINDOWS
  return 0;
#endif
}

// GENERIC ; PLATFORM AGNOSTIC
sfb_thread_handle *sfb_thread_handle_allocate(const int cores) {
  sfb_thread_handle *handles = calloc(cores, sizeof(sfb_thread_handle));
  if (!handles) {
    return NULL;
  }
  return handles;
}

sfb_thread_ctx_renderer *sfb_thread_ctx_allocate(const int cores) {
  sfb_thread_ctx_renderer *thread_ctxs =
      calloc(cores, sizeof(sfb_thread_ctx_renderer));
  if (!thread_ctxs) {
    return NULL;
  }

  for (int j = 0; j < cores; j++) {
    for (int i = 0; i < SFB_THREAD_QUEUE_MAX; i++) {
      thread_ctxs[j].jptrs[i] = &thread_ctxs[j].queue[i];
    }
  }

  return thread_ctxs;
}

void sfb_free_ctxs(sfb_thread_ctx_renderer *ctxs) {
  if (ctxs) {
    free(ctxs);
  }
  ctxs = NULL;
}

void sfb_free_handles(sfb_thread_handle *handles) {
  if (handles) {
    free(handles);
  }
  handles = NULL;
}

// PLATFORM SPECIFIC CODE:
#if POSIX
#include <unistd.h>
static int sfb_thread_mutex_lock(pthread_mutex_t *m);
static int sfb_thread_mutex_unlock(pthread_mutex_t *m);
static int sfb_thread_stop_flag_posix(sfb_thread_ctx_renderer *ctx);
static int sfb_thread_queue_seek(sfb_thread_ctx_renderer *ctx);
static int sfb_thread_queue_add(sfb_thread_ctx_renderer *ctx,
                                sfb_framebuffer *buf, const sfb_obj *const obj,
                                int y0, int x0, int pos);
static int sfb_thread_cond_signal(pthread_cond_t *c);
static int sfb_thread_join(pthread_t *handle);
static int sfb_thread_destroy_mutex(pthread_mutex_t *m);
static int sfb_thread_destroy_cond(pthread_cond_t *m);
static int sfb_thread_mutex_init(pthread_mutex_t *m);
static int sfb_thread_cond_init(pthread_cond_t *c);
static int sfb_thread_create(pthread_t *handle, void *(*routine)(void *),
                             void *arg);
static int sfb_thread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m);

static int sfb_thread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m) {
  int retval = 0;
  if ((retval = pthread_cond_wait(c, m)) != 0) {
    fprintf(stderr, "Thread failed to wait on pthread_cond_t -> %s\n",
            strerror(retval));
    return 0;
  }
  return 1;
}

static int sfb_thread_create(pthread_t *handle, void *(*routine)(void *),
                             void *arg) {
  int retval = 0;
  if ((retval = pthread_create(handle, NULL, routine, arg)) != 0) {
    fprintf(stderr, "Failed to create thread -> %s\n", strerror(errno));
    return 0;
  }
  return 1;
}

static int sfb_thread_mutex_init(pthread_mutex_t *m) {
  int retval = 0;
  if ((retval = pthread_mutex_init(m, NULL)) != 0) {
    fprintf(stderr, "Failed to init  pthread_mutex_t -> %s\n", strerror(errno));
    return 0;
  }
  return 1;
}

static int sfb_thread_cond_init(pthread_cond_t *c) {
  int retval = 0;
  if ((retval = pthread_cond_init(c, NULL)) != 0) {
    fprintf(stderr, "Failed to init pthread_cond_t -> %s\n", strerror(errno));
    return 0;
  }
  return 1;
}

static int sfb_thread_destroy_cond(pthread_cond_t *c) {
  int retval = 0;
  if ((retval = pthread_cond_destroy(c)) != 0) {
    fprintf(stderr, "Failed to destroy pthread_cond_t -> %s\n",
            strerror(retval));
    return 0;
  }
  c = 0;
  return 1;
}

static int sfb_thread_destroy_mutex(pthread_mutex_t *m) {
  int retval = 0;
  if ((retval = pthread_mutex_destroy(m)) != 0) {
    fprintf(stderr, "Failed to destroy pthread_mutex_t -> %s\n",
            strerror(retval));
    return 0;
  }
  m = 0;
  return 1;
}

static int sfb_thread_join(pthread_t *handle) {
  int retval = 0;
  if ((retval = pthread_join(*handle, NULL)) != 0) {
    fprintf(stderr, "Failed to join thread -> %s\n", strerror(retval));
    return 0;
  }
  return 1;
}

static int sfb_thread_cond_signal(pthread_cond_t *c) {
  int retval = 0;
  if ((retval = pthread_cond_signal(c)) != 0) {
    fprintf(stderr, "Failed to signal pthread_cond_t -> %s\n",
            strerror(retval));
    return 0;
  }
  return 1;
}

static int sfb_thread_mutex_lock(pthread_mutex_t *m) {
  int retval = 0;
  if ((retval = pthread_mutex_lock(m)) != 0) {
    fprintf(stderr, "Failed to acquire mutex lock -> %s\n", strerror(retval));
    return 0;
  }
  return 1;
}

static int sfb_thread_mutex_unlock(pthread_mutex_t *m) {
  int retval = 0;
  if ((retval = pthread_mutex_unlock(m)) != 0) {
    fprintf(stderr, "Failed to unlock mutex -> %s\n", strerror(retval));
    return 0;
  }
  return 1;
}

static int sfb_thread_queue_add(sfb_thread_ctx_renderer *ctx,
                                sfb_framebuffer *buf, const sfb_obj *const obj,
                                int y0, int x0, int pos) {
  if (!sfb_thread_mutex_lock(&ctx->mutex)) {
    return 0;
  }
  sfb_thread_job tmp = {0, 0, buf, obj, y0, x0};
  ctx->queue[pos] = tmp;
  ctx->queued++;
  if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
    return 0;
  }
  return 1;
}

static int sfb_thread_queue_seek(sfb_thread_ctx_renderer *ctx) {
  if (!sfb_thread_mutex_lock(&ctx->mutex)) {
    return 0;
  }
  int j = -1;
  sfb_thread_job *queue = ctx->queue;
  for (int i = 0; i < SFB_THREAD_QUEUE_MAX; i++) {
    if (queue[i].done) {
      j = i;
      break;
    }
  }
  if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
    return 0;
  }
  return j;
}

// NON STATIC FUNCTIONS
int sfb_wait_threads_posix(sfb_thread_ctx_renderer *ctxs, int cores) {
  for (int i = 0; i < cores; i++) {
    sfb_thread_mutex_lock(&ctxs[i].mutex);
    while (ctxs[i].working) {
      if (!sfb_thread_cond_wait(&ctxs[i].finished, &ctxs[i].mutex)) {
        ctxs[i].valid = 0;
        return 0;
      }
    }
    sfb_thread_mutex_unlock(&ctxs[i].mutex);
  }
  return 1;
}

int sfb_thread_queue_restack_posix(sfb_thread_ctx_renderer *ctx) {
  if (!sfb_thread_mutex_lock(&ctx->mutex)) {
    return 0;
  }

  sfb_thread_job **queue = ctx->jptrs;
  int dst = 0;
  for (int src = 0; src < SFB_THREAD_QUEUE_MAX && dst < SFB_THREAD_QUEUE_MAX;
       src++) {
    if (!queue[src]->done && src != dst) {
      sfb_thread_job *tmp = queue[dst];
      queue[dst] = queue[src];
      queue[src] = tmp;
      dst++;
    }
  }

  if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
    return 0;
  }
  return 1;
}

int sfb_thread_dequeue_posix(sfb_thread_ctx_renderer *ctx) {
  if (!sfb_thread_mutex_lock(&ctx->mutex)) {
    return 0;
  }
  for (int i = 0; i < SFB_THREAD_QUEUE_MAX; i++) {
    if (ctx->queue[i].done && !ctx->queue[i].dequeued) {
      ctx->queue[i].dequeued = 1;
      ctx->queued--;
    }
  }
  if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
    return 0;
  }
  return 1;
}

int sfb_thread_queue_job_posix(sfb_thread_ctx_renderer *ctx,
                               sfb_framebuffer *buf, const sfb_obj *const obj,
                               int y0, int x0) {
  if (ctx && buf && obj) {
    int queue_pos = -1;
    if ((queue_pos = sfb_thread_queue_seek(ctx)) < 0) {
      return 0;
    }

    if (!sfb_thread_queue_add(ctx, buf, obj, y0, x0, queue_pos)) {
      return 0;
    }
    return 1;
  }
  return 0;
}

int sfb_thread_signal_resume_posix(sfb_thread_ctx_renderer *ctx) {
  if (ctx && ctx->valid) {
    if (!sfb_thread_mutex_lock(&ctx->mutex)) {
      return 0;
    }

    if (!sfb_thread_cond_signal(&ctx->cond)) {
      return 0;
    }

    if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
      return 0;
    }
    return 1;
  }
  return 0;
}

static int sfb_thread_stop_flag_posix(sfb_thread_ctx_renderer *ctx) {
  if (ctx && ctx->valid) {
    if (!sfb_thread_mutex_lock(&ctx->mutex)) {
      return 0;
    }
    ctx->active = 0;
    if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
      return 0;
    }
    return 1;
  }
  return 0;
}

int sfb_resume_thread_posix(sfb_thread_ctx_renderer *ctx) {
  if (ctx && ctx->valid) {
    if (!sfb_thread_mutex_lock(&ctx->mutex)) {
      return 0;
    }
    ctx->working = 1;
    if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
      return 0;
    }
    return 1;
  }
  return 0;
}

int sfb_pause_thread_posix(sfb_thread_ctx_renderer *ctx) {
  if (ctx && ctx->valid) {
    if (!sfb_thread_mutex_lock(&ctx->mutex)) {
      return 0;
    }
    ctx->working = 0;
    if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
      return 0;
    }
    return 1;
  }
  return 0;
}

int sfb_kill_thread_posix(sfb_thread_ctx_renderer *ctx,
                          sfb_thread_handle *handle) {
  if ((ctx && ctx->valid) && handle) {
    if (!sfb_thread_stop_flag_posix(ctx)) {
      return 0;
    }
    // Make sure its not waiting
    if (!sfb_thread_signal_resume_posix(ctx)) {
      return 0;
    }

    if (!sfb_thread_join(&handle->handle)) {
      return 0;
    }

    if (!sfb_thread_destroy_cond(&ctx->cond)) {
      return 0;
    }

    if (!sfb_thread_destroy_mutex(&ctx->mutex)) {
      return 0;
    }
    fprintf(stdout, "Thread { %zu } destroyed\n", handle->handle);
    ctx->valid = 0;
    return 1;
  }
  return 0;
}

void *sfb_thread_posix_worker(void *arg) {
  if (!arg) {
    return NULL;
  }

  sfb_thread_ctx_renderer *ctx = (sfb_thread_ctx_renderer *)arg;
  while (ctx->active) {
    if (!sfb_thread_mutex_lock(&ctx->mutex)) {
      atomic_store(&ctx->errcode, 1);
      return NULL;
    }

    while (!ctx->working && ctx->active) {
      if (!sfb_thread_cond_wait(&ctx->cond, &ctx->mutex)) {
        atomic_store(&ctx->errcode, 1);
        return NULL;
      }
    }

    int process = (ctx->queued < SFB_THREAD_QUEUE_MAX) ? ctx->queued
                                                       : SFB_THREAD_QUEUE_MAX;
    for (int i = 0; i < process; i++) {
      sfb_thread_job *job = &ctx->queue[i];
      if (job->done) {
        continue;
      }

      for (int dy = 0; dy < job->obj->h; dy++) {
        const int y = job->y0 + dy;
        if (y < 0 || y >= job->buffer->h) {
          continue;
        }

        for (int dx = 0; dx < job->obj->w; dx++) {
          const int x = job->x0 + dx;
          if (x < 0 || x >= job->buffer->w) {
            continue;
          }

          uint32_t *buf = job->buffer->data;
          uint32_t *rect = job->obj->pixels;
          const uint32_t col = rect[dy * job->obj->w + dx];

          sfb_put_pixel(x, y, buf, job->buffer->w, job->buffer->h, col,
                        job->buffer->flags, job->obj->flags);
        }
        job->done = 1;
      }
    }

    ctx->working = 0;
    if (!sfb_thread_cond_signal(&ctx->finished)) {
      atomic_store(&ctx->errcode, 1);
      return NULL;
    }

    if (!ctx->active) {
      if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
        atomic_store(&ctx->errcode, 1);
        return NULL;
      }
      break;
    }

    if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
      atomic_store(&ctx->errcode, 1);
      return NULL;
    }
  }
  return NULL;
}

int sfb_get_cores_posix(void) {
  long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
  if (nprocs < 1) {
    return 1;
  }
  return (int)nprocs;
}

sfb_thread_ctx_renderer *
sfb_spawn_threads_posix(sfb_thread_handle *thread_handles, const int cores) {
  sfb_thread_ctx_renderer *thread_ctxs = sfb_thread_ctx_allocate(cores);
  if (!thread_ctxs) {
    fprintf(stderr, "Could not allocate thread contexts! -> %s\n",
            strerror(errno));
    return NULL;
  }

  for (int i = 0; i < cores; i++) {
    sfb_thread_ctx_renderer *ctx = &thread_ctxs[i];
    ctx->errcode = 0;
    ctx->valid = 1;
    ctx->active = 1;
    ctx->queued = 0;
    ctx->working = 0;

    for (int j = 0; j < SFB_THREAD_QUEUE_MAX; j++) {
      ctx->queue[j].done = 1;
      ctx->queue[j].dequeued = 1;
      ctx->queue[j].obj = NULL;
      ctx->queue[j].buffer = NULL;
      ctx->queue[j].y0 = 0;
      ctx->queue[j].x0 = 0;
    }

    if (!sfb_thread_mutex_init(&ctx->mutex)) {
      ctx->valid = 0;
      return thread_ctxs;
    }

    if (!sfb_thread_cond_init(&ctx->cond)) {
      ctx->valid = 0;
      return thread_ctxs;
    }

    if (!sfb_thread_cond_init(&ctx->finished)) {
      ctx->valid = 0;
      return thread_ctxs;
    }

    pthread_t *handle = &thread_handles[i].handle;
    if (!sfb_thread_create(handle, sfb_thread_posix_worker, ctx)) {
      ctx->valid = 0;
      return thread_ctxs;
    }
  }

  return thread_ctxs;
}

#elif WINDOWS
// TODO: IMPLEMENT
#endif
