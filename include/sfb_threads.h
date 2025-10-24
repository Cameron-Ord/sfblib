#ifndef SFB_THREADS_H
#define SFB_THREADS_H
#include <stdint.h>
#define SFB_THREAD_QUEUE_MAX 64

typedef struct sfb_thread_job sfb_thread_job;
typedef struct sfb_framebuffer sfb_framebuffer;
typedef struct sfb_obj sfb_obj;

struct sfb_thread_job {
  int done, dequeued;
  int rows, start;
  sfb_framebuffer *buffer;
  const sfb_obj *obj;
  int y0, x0;
};

typedef struct sfb_thread_ctx_renderer sfb_thread_ctx_renderer;
typedef struct sfb_thread_handle sfb_thread_handle;

int sfb_thread_queue_restack(sfb_thread_ctx_renderer *ctx);
void sfb_resume_thread(sfb_thread_ctx_renderer *ctx);
void sfb_pause_thread(sfb_thread_ctx_renderer *ctx);
void sfb_thread_dequeue(sfb_thread_ctx_renderer *ctx);
int sfb_thread_queue_job(sfb_thread_ctx_renderer *ctx, int rows, int start,
                         sfb_framebuffer *buf, const sfb_obj *const obj, int y0,
                         int x0);
sfb_thread_ctx_renderer *sfb_thread_ctx_allocate(const int cores);
sfb_thread_handle *sfb_thread_handle_allocate(const int cores);
int sfb_get_cores(void);
sfb_thread_ctx_renderer *sfb_spawn_threads(sfb_thread_handle *handles,
                                           int cores);
void sfb_kill_thread(sfb_thread_ctx_renderer *ctx, sfb_thread_handle *handle);
// Ensure threads are joined and finished before calling these functions
void sfb_free_handles(sfb_thread_handle *handles);
void sfb_free_ctxs(sfb_thread_ctx_renderer *ctxs);
void sfb_thread_signal(sfb_thread_ctx_renderer *ctx);

#if defined(__unix__) || defined(__unix) || defined(unix) ||                   \
    (defined(__APPLE__) && defined(__MACH__))

#define POSIX 1
#define WINDOWS 0

#include <pthread.h>
// This is not heap allocated or stored in a struct, this is managed by the
// programmer and can be heap or stack allocated as sfb_thread_handle
// handles[cores] once the core count from get_cores() has been called, this is
// needed before threads can be spawned
struct sfb_thread_handle {
  pthread_t handle;
};

struct sfb_thread_ctx_renderer {
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  int active;
  int valid;
  int queued;
  int working;
  sfb_thread_job queue[SFB_THREAD_QUEUE_MAX];
  sfb_thread_job *jptrs[SFB_THREAD_QUEUE_MAX];
};

int sfb_thread_queue_restack_posix(sfb_thread_ctx_renderer *ctx);
int sfb_resume_thread_posix(sfb_thread_ctx_renderer *ctx);
int sfb_pause_thread_posix(sfb_thread_ctx_renderer *ctx);
int sfb_thread_dequeue_posix(sfb_thread_ctx_renderer *ctx);
int sfb_thread_queue_job_posix(sfb_thread_ctx_renderer *ctx, int rows,
                               int start, sfb_framebuffer *buf,
                               const sfb_obj *const obj, int y0, int x0);
int sfb_kill_thread_posix(sfb_thread_ctx_renderer *ctx,
                          sfb_thread_handle *handle);
int sfb_thread_signal_posix(sfb_thread_ctx_renderer *ctx);
int sfb_get_cores_posix(void);
sfb_thread_ctx_renderer *
sfb_spawn_threads_posix(sfb_thread_handle *thread_handles, int cores);
void *sfb_thread_posix_worker(void *arg);

// TODO: not impl
#elif defined(_WIN32) || defined(_WIN64)
#define POSIX 0
#define WINDOWS 1

#endif

#endif
