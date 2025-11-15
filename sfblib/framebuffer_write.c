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

inline void sfb_fb_clear(sfb_framebuffer *const buffer,
                         sfb_colour clear_colour) {
  if (!buffer) {
    return;
  }
  uint8_t channels[SFB_MAX_CHANNELS] = {clear_colour.r, clear_colour.g,
                                        clear_colour.b, clear_colour.a};
  for (int i = 0; i < buffer->w * buffer->h; i++) {
    for (int c = 0; c < buffer->channels; c++) {
      buffer->pixels.data[i * buffer->channels + c] = channels[c];
    }
  }
}

static inline void sfb_loop_obj_lighting(const sfb_light_source *light,
                                         sfb_framebuffer *const buffer,
                                         const int y0, const int x0) {
  const int rows = light->h, cols = light->w;
  const uint8_t *rect = light->lightmap.data;
  uint8_t *framebuffer = buffer->pixels.data;

  for (int dy = 0; dy < rows; dy++) {
    const int y = y0 + dy;
    if (y < 0 || y > buffer->h)
      continue;

    for (int dx = 0; dx < cols; dx++) {
      const int x = x0 + dx;
      if (x < 0 || x >= buffer->w)
        continue;

      const uint8_t *src_start = rect + ((dy * cols + dx) * light->channels);
      const int location = (y * buffer->w + x) * buffer->channels;

      if (location < buffer->size && location > 0) {
        uint8_t *dst_start = framebuffer + location;
        if (buffer->flags & SFB_BLEND_ENABLED) {
          uint8_t blended[SFB_RGBA_CHANNELS];
          sfb_blend_pixel(blended, dst_start, src_start);
          sfb_put_pixel(dst_start, blended, buffer->channels);
        } else {
          uint8_t add[SFB_RGB_CHANNELS];
          sfb_light_additive(add, dst_start, src_start);
          sfb_put_pixel(dst_start, add, buffer->channels);
        }
      }
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
  uint8_t *framebuffer = buffer->pixels.data;
  const uint8_t *rect = obj->pixels.data;

  for (int dy = 0; dy < obj->h; dy++) {
    const int y = y0 + dy;
    if (y < 0 || y >= buffer->h)
      continue;

    for (int dx = 0; dx < obj->w; dx++) {
      const int x = x0 + dx;
      if (x < 0 || x >= buffer->w)
        continue;

      const uint8_t *src_start = rect + ((dy * obj->w + dx) * obj->channels);
      const int location = (y * buffer->w + x) * buffer->channels;

      if (location < buffer->size && location >= 0) {
        uint8_t *dst_start = framebuffer + location;
        if (buffer->flags & SFB_BLEND_ENABLED) {
          uint8_t blended[SFB_RGBA_CHANNELS];
          sfb_blend_pixel(blended, dst_start, src_start);
          sfb_put_pixel(dst_start, blended, buffer->channels);
        } else {
          sfb_put_pixel(dst_start, src_start, buffer->channels);
        }
      }
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

void sfb_write_rect_generic(int x0, int y0, int w0, int h0, sfb_colour colour,
                            sfb_framebuffer *const buffer) {
  if (!buffer) {
    return;
  }

  uint8_t *framebuffer = buffer->pixels.data;
  uint8_t src[SFB_MAX_CHANNELS] = {colour.r, colour.g, colour.b, colour.a};

  for (int dy = 0; dy < h0; dy++) {
    const int y = y0 + dy;
    if (y < 0 || y >= buffer->h)
      continue;
    for (int dx = 0; dx < w0; dx++) {
      const int x = x0 + dx;
      if (x < 0 || x >= buffer->w)
        continue;

      const int location = (y * buffer->w + x) * buffer->channels;

      if (location < buffer->size && location >= 0) {
        uint8_t *dst_start = framebuffer + location;
        if (buffer->flags & SFB_BLEND_ENABLED) {
          uint8_t blended[SFB_RGBA_CHANNELS];
          sfb_blend_pixel(blended, dst_start, src);
          sfb_put_pixel(dst_start, blended, buffer->channels);
        } else {
          sfb_put_pixel(dst_start, src, buffer->channels);
        }
      }
    }
  }
}

void sfb_write_circle_generic(const int xc, const int yc, sfb_colour colour,
                              sfb_framebuffer *const buffer, const int radius) {
  if (!buffer) {
    return;
  }

  uint8_t *framebuffer = buffer->pixels.data;
  uint8_t src[SFB_MAX_CHANNELS] = {colour.r, colour.g, colour.b, colour.a};

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

      const int location = (y * buffer->w + x) * buffer->channels;
      const int dx = x - xc;
      const int dy = y - yc;

      if ((dx * dx + dy * dy <= radius * radius) && location < buffer->size &&
          location >= 0) {
        uint8_t *dst_start = framebuffer + location;
        if (buffer->flags & SFB_BLEND_ENABLED) {
          uint8_t blended[SFB_RGBA_CHANNELS];
          sfb_blend_pixel(blended, dst_start, src);
          sfb_put_pixel(dst_start, blended, buffer->channels);
        } else {
          sfb_put_pixel(dst_start, src, buffer->channels);
        }
      }
    }
  }
}

void sfb_put_pixel(uint8_t *const framebuffer, const uint8_t *colour,
                   uint8_t buffer_channels) {
  if (!framebuffer)
    return;

  switch (buffer_channels) {
  default:
    break;
  case SFB_RGBA_CHANNELS: {
    *(framebuffer + RED) = *(colour + RED);
    *(framebuffer + GREEN) = *(colour + GREEN);
    *(framebuffer + BLUE) = *(colour + BLUE);
    *(framebuffer + ALPHA) = *(colour + ALPHA);
  } break;

  case SFB_RGB_CHANNELS: {
    *(framebuffer + RED) = *(colour + RED);
    *(framebuffer + GREEN) = *(colour + GREEN);
    *(framebuffer + BLUE) = *(colour + BLUE);
  } break;
  }
}
