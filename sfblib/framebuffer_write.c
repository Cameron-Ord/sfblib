#include "../include/sfb_camera.h"
#include "../include/sfb_flags.h"
#include "../include/sfb_framebuffer.h"
#include "../include/sfb_rgba.h"
#include "../include/sfb_threads.h"

static inline void sfb_loop_obj_lighting(const sfb_light_source *light,
                                         sfb_framebuffer *const buffer,
                                         const int y0, const int x0);
static inline void sfb_loop_obj_rect(const sfb_obj *const obj,
                                     sfb_framebuffer *const buffer,
                                     const int y0, const int x0);
static inline void sfb_obj_queue_threaded(const sfb_obj *const obj,
                                          sfb_framebuffer *const buffer,
                                          const int y0, const int x0);

inline void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour) {
  if (!buffer) {
    return;
  }
  for (int i = 0; i < buffer->w * buffer->h; i++) {
    buffer->data[i] = clear_colour;
  }
}

static inline void sfb_loop_obj_lighting(const sfb_light_source *light,
                                         sfb_framebuffer *const buffer,
                                         const int y0, const int x0) {
  const int rows = light->h, cols = light->w;
  const uint32_t *rect = light->lightmap;
  uint32_t *buf = buffer->data;

  for (int dy = 0; dy < rows; dy++) {
    const int y = y0 + dy;
    if (y < 0 || y > buffer->h)
      continue;

    for (int dx = 0; dx < cols; dx++) {
      const int x = x0 + dx;
      if (x < 0 || x >= buffer->w)
        continue;

      const uint32_t col = rect[dy * cols + dx];
      sfb_put_light(x, y, buf, buffer->w, buffer->h, col, buffer->flags,
                    light->flags);
    }
  }
}
// sfb_thread_dequeue and sfb_thread_queue_restack need to be called
// at the top level of the event loop before queuing more jobs
static inline void sfb_obj_queue_threaded(const sfb_obj *const obj,
                                          sfb_framebuffer *const buffer,
                                          const int y0, const int x0) {
  static int current_thread;
  const int cores = buffer->cores;

  if (cores > 0 && buffer->flags & SFB_ENABLE_MULTITHREADED) {
    sfb_thread_ctx_renderer *threads = buffer->thread_render_data;
    if (!(current_thread >= 0 && current_thread < cores)) {
      current_thread = 0;
    }

    if (threads) {
      sfb_thread_ctx_renderer *ctx = &threads[current_thread];
      if (atomic_load(&ctx->errcode) != 0) {
        ctx->valid = 0;
        // TODO: Handle error
      }

      if (ctx->valid) {
        sfb_thread_queue_job(ctx, buffer, obj, y0, x0);
      }

      current_thread++;
    }
  }
}

static inline void sfb_loop_obj_rect(const sfb_obj *const obj,
                                     sfb_framebuffer *const buffer,
                                     const int y0, const int x0) {
  uint32_t *buf = buffer->data;
  uint32_t *rect = obj->pixels;
  const int rows = obj->h, cols = obj->w;

  for (int dy = 0; dy < rows; dy++) {
    const int y = y0 + dy;
    if (y < 0 || y >= buffer->h)
      continue;

    for (int dx = 0; dx < cols; dx++) {
      const int x = x0 + dx;
      if (x < 0 || x >= buffer->w)
        continue;

      const uint32_t col = rect[dy * cols + dx];
      sfb_put_pixel(x, y, buf, buffer->w, buffer->h, col);
    }
  }
}

inline void sfb_write_obj_rect(const sfb_obj *const obj,
                               sfb_framebuffer *const buffer,
                               const sfb_camera *const camera) {
  if (!obj || !buffer) {
    return;
  }

  int x0, y0;
  const int cond = camera ? 1 : 0;
  switch (cond) {
  default: {
    y0 = obj->mat.m5;
    x0 = obj->mat.m2;
  } break;
  case 1: {
    y0 = obj->mat.m5 - camera->mat.m5;
    x0 = obj->mat.m2 - camera->mat.m2;
  } break;
  }

  if (buffer->flags & SFB_ENABLE_MULTITHREADED) {
    sfb_obj_queue_threaded(obj, buffer, y0, x0);
  } else {
    sfb_loop_obj_rect(obj, buffer, y0, x0);
    if (obj->flags & SFB_LIGHT_SOURCE && obj->light) {
      // y0 and x0 will obviously need to be offset so the light is sourced from
      // the center of the source (not impl)
      sfb_loop_obj_lighting(obj->light, buffer, y0, x0);
    }
  }
}

void sfb_write_rect_generic(int x0, int y0, int w0, int h0, uint32_t colour,
                            sfb_framebuffer *const buffer) {
  if (!buffer) {
    return;
  }

  for (int dy = 0; dy < h0; dy++) {
    const int y = y0 + dy;
    if (y < 0 || y >= buffer->h)
      continue;
    for (int dx = 0; dx < w0; dx++) {
      const int x = x0 + dx;
      if (x < 0 || x >= buffer->w)
        continue;

      sfb_put_pixel(x, y, buffer->data, buffer->w, buffer->h, colour);
    }
  }
}

void sfb_write_circle_generic(const int xc, const int yc, uint32_t colour,
                              sfb_framebuffer *const buffer, const int radius) {
  if (!buffer) {
    return;
  }

  const int xstart = xc - radius;
  const int ystart = yc - radius;
  const int xend = xc + radius;
  const int yend = yc + radius;

  for (int y = ystart; y <= yend; y++) {
    if (!(y >= 0 && y < buffer->h))
      continue;

    for (int x = xstart; x <= xend; x++) {
      if (!(x >= 0 && x < buffer->w))
        continue;

      const int dx = x - xc;
      const int dy = y - yc;
      if (dx * dx + dy * dy <= radius * radius) {
        sfb_put_pixel(x, y, buffer->data, buffer->w, buffer->h, colour);
      }
    }
  }
}

void sfb_put_pixel(const int x, const int y, uint32_t *const buf, const int w,
                   const int h, uint32_t colour) {
  if (!buf)
    return;

  const int l = y * w + x;
  const int max = w * h;
  // Simply write the pixels RGB values with max alpha
  if (l < max && l >= 0) {
    buf[l] = (255 << 24) | (colour << 16) | (colour << 8) | (colour << 0);
  }
}

void sfb_put_light(const int x, const int y, uint32_t *const buf, const int w,
                   const int h, uint32_t colour, int bufflags, int pixflags) {
  if (!buf)
    return;

  const int l = y * w + x;
  const int max = w * h;
  if (l < max && l >= 0) {
    if (bufflags & SFB_BLEND_ENABLED) {
      buf[l] = sfb_blend_pixel(buf[l], colour);
    } else {
      buf[l] = sfb_light_additive(buf[l], colour);
    }
  }
}
