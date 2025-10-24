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

int sfb_thread_queue_restack(sfb_thread_ctx_renderer *ctx) {
#if POSIX
  return sfb_thread_queue_restack_posix(ctx);
#elif WINDOWS
  return 0;
#endif
}

void sfb_pause_thread(sfb_thread_ctx_renderer *ctx) {
#if POSIX
  sfb_pause_thread_posix(ctx);
#elif WINDOWS
#endif
}

void sfb_resume_thread(sfb_thread_ctx_renderer *ctx) {
#if POSIX
  sfb_resume_thread_posix(ctx);
#elif WINDOWS
#endif
}

void sfb_thread_dequeue(sfb_thread_ctx_renderer *ctx) {
#if POSIX
  sfb_thread_dequeue_posix(ctx);
#elif WINDOWS
#endif
}

void sfb_thread_signal(sfb_thread_ctx_renderer *ctx) {
#if POSIX
  sfb_thread_signal_posix(ctx);
#elif WINDOWS
  // TODO: IMPLEMENT
#endif
}

void sfb_kill_thread(sfb_thread_ctx_renderer *ctx, sfb_thread_handle *handle) {
#if POSIX
  sfb_kill_thread_posix(ctx, handle);
#elif WINDOWS
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

int sfb_thread_queue_job(sfb_thread_ctx_renderer *ctx, int rows, int start,
                         sfb_framebuffer *buf, const sfb_obj *const obj, int y0,
                         int x0) {
#if POSIX
  return sfb_thread_queue_job_posix(ctx, rows, start, buf, obj, y0, x0);
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
static int sfb_thread_queue_add(sfb_thread_ctx_renderer *ctx, int rows,
                                int start, sfb_framebuffer *buf,
                                const sfb_obj *const obj, int y0, int x0,
                                int pos);

static int sfb_thread_mutex_lock(pthread_mutex_t *m) {
  const int r = pthread_mutex_lock(m);
  if (r != 0) {
    fprintf(stderr, "Failed to acquire mutex lock!-> %s\n", strerror(r));
    return 0;
  }
  return 1;
}

static int sfb_thread_mutex_unlock(pthread_mutex_t *m) {
  const int r = pthread_mutex_unlock(m);
  if (r != 0) {
    fprintf(stderr, "Failed to unlock mutex -> %s\n", strerror(r));
    return 0;
  }
  return 1;
}

static int sfb_thread_queue_add(sfb_thread_ctx_renderer *ctx, int rows,
                                int start, sfb_framebuffer *buf,
                                const sfb_obj *const obj, int y0, int x0,
                                int pos) {
  if (!sfb_thread_mutex_lock(&ctx->mutex)) {
    return 0;
  }
  sfb_thread_job tmp = {0, 0, rows, start, buf, obj, y0, x0};
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
    }
    dst++;
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

int sfb_thread_queue_job_posix(sfb_thread_ctx_renderer *ctx, int rows,
                               int start, sfb_framebuffer *buf,
                               const sfb_obj *const obj, int y0, int x0) {
  if (ctx && buf && obj) {
    const int queue_pos = sfb_thread_queue_seek(ctx);
    if (queue_pos < 0) {
      return 0;
    }

    sfb_thread_queue_add(ctx, rows, start, buf, obj, y0, x0, queue_pos);
    return 1;
  }
  return 0;
}

int sfb_thread_signal_posix(sfb_thread_ctx_renderer *ctx) {
  if (ctx && ctx->valid) {
    if (!sfb_thread_mutex_lock(&ctx->mutex)) {
      return 0;
    }

    pthread_cond_signal(&ctx->cond);

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
    sfb_thread_stop_flag_posix(ctx);
    sfb_thread_signal_posix(ctx);

    const int retval = pthread_join(handle->handle, NULL);
    if (retval != 0) {
      fprintf(stderr, "Failed to join thread! -> %s\n", strerror(retval));
    }

    pthread_cond_destroy(&ctx->cond);
    pthread_mutex_destroy(&ctx->mutex);

    ctx->valid = 0;
    fprintf(stdout, "Thread { %zu } destroyed\n", handle->handle);
    return 1;
  }
  return 0;
}

void *sfb_posix_worker(void *arg) {
  if (!arg) {
    return NULL;
  }

  sfb_thread_ctx_renderer *ctx = (sfb_thread_ctx_renderer *)arg;
  while (ctx->active) {
    if (!sfb_thread_mutex_lock(&ctx->mutex)) {
      return NULL;
    }

    while (!ctx->working && ctx->active) {
      pthread_cond_wait(&ctx->cond, &ctx->mutex);
    }

    int process = (ctx->queued < SFB_THREAD_QUEUE_MAX) ? ctx->queued
                                                       : SFB_THREAD_QUEUE_MAX;
    for (int i = 0; i < process; i++) {
      sfb_thread_job *job = &ctx->queue[i];
      if (job->done) {
        continue;
      }

      for (int dy = 0; dy < job->rows; dy++) {
        const int rdy = dy + job->start;
        const int y = job->y0 + rdy;
        if (y < 0 || (y >= job->buffer->h || rdy >= job->obj->h)) {
          continue;
        }

        for (int dx = 0; dx < job->obj->w; dx++) {
          const int x = job->x0 + dx;
          if (x < 0 || (x >= job->buffer->w || dx >= job->obj->w)) {
            continue;
          }

          sfb_put_pixel(x, y, job->buffer->data, job->buffer->w, job->buffer->h,
                        job->obj->pixels[rdy * job->obj->w + dx],
                        job->buffer->flags, 1.0f);
        }
      }
      job->done = 1;
    }

    ctx->working = 0;
    if (!ctx->active) {
      if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
        return NULL;
      }
      break;
    }

    if (!sfb_thread_mutex_unlock(&ctx->mutex)) {
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
    ctx->valid = 1;
    ctx->active = 1;
    ctx->queued = 0;
    ctx->working = 0;

    for (int j = 0; j < SFB_THREAD_QUEUE_MAX; j++) {
      ctx->queue[j].done = 1;
      ctx->queue[j].dequeued = 1;
      ctx->queue[j].obj = NULL;
      ctx->queue[j].buffer = NULL;
      ctx->queue[j].rows = 0;
      ctx->queue[j].start = 0;
      ctx->queue[j].y0 = 0;
      ctx->queue[j].x0 = 0;
    }

    pthread_mutex_init(&ctx->mutex, NULL);
    pthread_cond_init(&ctx->cond, NULL);

    pthread_t *handle = &thread_handles[i].handle;
    int r = pthread_create(handle, NULL, sfb_posix_worker, ctx);
    if (r != 0) {
      fprintf(stderr, "Failed to create thread! ->%s\n", strerror(r));
      ctx->valid = 0;
    }
  }

  return thread_ctxs;
}

#elif WINDOWS
// TODO: IMPLEMENT
#endif
