#include "../include/sfb_camera.h"
#include "../include/sfb_flags.h"
#include "../include/sfb_framebuffer.h"
#include "../include/sfb_rgba.h"
#include "../include/sfb_threads.h"

static void sfb_loop_rect(const sfb_rect *const obj,
                          sfb_framebuffer *const buffer, const int y0,
                          const int x0);
static void sfb_rect_queue_threaded(const sfb_rect *const obj,
                                    sfb_framebuffer *const buffer, const int y0,
                                    const int x0);

void sfb_fb_clear(sfb_framebuffer *const buffer, sfb_rgba clear_colour) {
  if (!buffer) {
    return;
  }
  uint8_t channels[SFB_MAX_CHANNELS] = {clear_colour.r, clear_colour.g,
                                        clear_colour.b, clear_colour.a};
  for (int i = 0; i < buffer->meta.w * buffer->meta.h; i++) {
    for (int c = 0; c < buffer->meta.channels; c++) {
      buffer->pixels.data[i * buffer->meta.channels + c] = channels[c];
    }
  }
}

// sfb_thread_dequeue and sfb_thread_queue_restack need to be called
// at the top level of the event loop before queuing more jobs
static void sfb_rect_queue_threaded(const sfb_rect *const obj,
                                    sfb_framebuffer *const buffer, const int y0,
                                    const int x0) {
  static int current_thread;
  const int cores = buffer->meta.cores;

  if (cores > 0 && buffer->r.flags & SFB_ENABLE_MULTITHREADED) {
    sfb_thread_ctx_renderer *threads = buffer->r.thread_render_data;
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

static void sfb_loop_rect(const sfb_rect *const obj,
                          sfb_framebuffer *const buffer, const int y0,
                          const int x0) {
  uint8_t *framebuffer = buffer->pixels.data;
  const uint8_t *rect = obj->pixels.data;

  for (int dy = 0; dy < obj->h; dy++) {
    const int y = y0 + dy;
    if (y < 0 || y >= buffer->meta.h)
      continue;

    for (int dx = 0; dx < obj->w; dx++) {
      const int x = x0 + dx;
      if (x < 0 || x >= buffer->meta.w)
        continue;

      const int srci = ((dy * obj->w + dx) * obj->channels);
      const int location = (y * buffer->meta.w + x) * buffer->meta.channels;

      if ((location < buffer->meta.size && location >= 0) &&
          (srci < obj->size && srci >= 0)) {
        uint8_t *dst_start = framebuffer + location;
        const uint8_t *src_start = rect + srci;
        if (buffer->meta.flags & SFB_BLEND_ENABLED) {
          uint8_t blended[SFB_RGBA_CHANNELS];
          sfb_blend_pixel(blended, dst_start, src_start);
          sfb_put_pixel(dst_start, blended, buffer->meta.channels);
        } else {
          sfb_put_pixel(dst_start, src_start, buffer->meta.channels);
        }
      }
    }
  }
}

void sfb_write_rect(const sfb_rect *const obj, sfb_framebuffer *const buffer) {
  if (!obj || !buffer) {
    return;
  }

  int x0, y0;
  y0 = obj->y - buffer->r.cam.y;
  x0 = obj->x - buffer->r.cam.x;

  if (buffer->r.flags & SFB_ENABLE_MULTITHREADED) {
    sfb_rect_queue_threaded(obj, buffer, y0, x0);
  } else {
    sfb_loop_rect(obj, buffer, y0, x0);
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
