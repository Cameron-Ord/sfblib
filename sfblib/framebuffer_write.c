#include "../include/sfb_camera.h"
#include "../include/sfb_flags.h"
#include "../include/sfb_framebuffer.h"
#include "../include/sfb_rgba.h"
#include "../include/sfb_threads.h"

inline void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour) {
  if (!buffer) {
    return;
  }
  for (int i = 0; i < buffer->w * buffer->h; i++) {
    buffer->data[i] = clear_colour;
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
    const int rows_per_queue = obj->h / buffer->cores;
    const int rows = obj->h;
    const int const_r = obj->h % buffer->cores;
    int start = 0, i = 0, r = 0;
    sfb_thread_ctx_renderer *threads = buffer->thread_render_data;

    while (i < rows) {
      const int thread = i % buffer->cores;
      sfb_thread_ctx_renderer *d = &threads[thread];

      if (atomic_load(&d->errcode) != 0) {
        d->valid = 0;
        // TODO: Handle error
      }

      if (d->valid) {
        sfb_thread_dequeue(d);
        sfb_thread_queue_restack(d);
        sfb_thread_queue_job(d, rows_per_queue, start, buffer, obj, y0, x0);
        start = start + rows_per_queue;
      }
      i++;
    }

    while (r < const_r) {
      const int thread = r % buffer->cores;
      sfb_thread_ctx_renderer *d = &threads[thread];

      if (atomic_load(&d->errcode) != 0) {
        d->valid = 0;
        // TODO: Handle error
      }

      if (d->valid) {
        sfb_thread_queue_job(d, r - (r - 1), start, buffer, obj, y0, x0);
        start = start + (r - (r - 1));
      }
      r++;
    }

    for (int i = 0; i < buffer->cores; i++) {
      sfb_resume_thread(&buffer->thread_render_data[i]);
      sfb_thread_signal(&buffer->thread_render_data[i]);
    }

  } else {
    for (int dy = 0; dy < obj->h; dy++) {
      const int y = y0 + dy;
      if (y < 0 || (y >= buffer->h || dy >= obj->h))
        continue;

      for (int dx = 0; dx < obj->w; dx++) {
        const int x = x0 + dx;
        if (x < 0 || (x >= buffer->w || dx >= obj->w))
          continue;

        sfb_put_pixel(x, y, buffer->data, buffer->w, buffer->h,
                      obj->pixels[dy * obj->w + dx], buffer->flags, 1.0f);
      }
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

      sfb_put_pixel(x, y, buffer->data, buffer->w, buffer->h, colour,
                    buffer->flags, 1.0f);
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
        sfb_put_pixel(x, y, buffer->data, buffer->w, buffer->h, colour,
                      buffer->flags, 1.0f);
      }
    }
  }
}

// None of the lighting flags are actually implemented yet
void sfb_put_pixel(const int x, const int y, uint32_t *const buf, const int w,
                   const int h, uint32_t colour, int pixflags,
                   float intensity) {
  if (!buf) {
    return;
  }
  const int l = y * w + x;
  const int max = w * h;
  if (l < max && l >= 0) {
    if (!(pixflags & SFB_LIGHT_SOURCE)) {
      if (pixflags & SFB_BLEND_ENABLED) {
        buf[l] = sfb_blend_pixel(buf[l], colour);
      } else {
        buf[l] = colour;
      }
    }
  }
}

void sfb_put_light(const int x, const int y, uint32_t *const buf, const int w,
                   const int h, uint32_t colour, int pixflags,
                   float intensity) {
  if (!buf)
    return;
  const int l = y * w + x;
  const int max = w * h;
  if (pixflags & SFB_LIGHT_SOURCE) {
    if (pixflags & SFB_BLEND_ENABLED) {
      buf[l] = sfb_blend_pixel(buf[l], colour);
    } else {
      buf[l] = sfb_light_additive(buf[l], colour);
    }
  }
}
