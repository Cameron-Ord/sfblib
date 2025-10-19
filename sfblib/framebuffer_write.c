#include "../include/sfb_camera.h"
#include "../include/sfb_framebuffer.h"
#include "../include/sfb_rgba.h"

inline void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour) {
  if (!buffer) {
    return;
  }
  for (int i = 0; i < buffer->w * buffer->h; i++) {
    buffer->data[i] = clear_colour;
  }
}

inline void sfb_write_obj_rect3x3(const sfb_obj3x3 *const obj,
                                  sfb_framebuffer *const buffer,
                                  const sfb_camera3x3 *const camera) {
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

  for (int dy = 0; dy < obj->h; dy++) {
    const int y = y0 + dy;
    if (y < 0 || (y >= buffer->h || y >= obj->h))
      continue;

    for (int dx = 0; dx < obj->w; dx++) {
      const int x = x0 + dx;
      if (x < 0 || (x >= buffer->w || x >= obj->w))
        continue;

      sfb_put_pixel3x3(x, y, buffer->data, buffer->w, buffer->h,
                       obj->pixels[dy * obj->w + dx]);
    }
  }
}

void sfb_write_rect_generic3x3(int x0, int y0, int w0, int h0, uint32_t colour,
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

      sfb_put_pixel3x3(x, y, buffer->data, buffer->w, buffer->h, colour);
    }
  }
}

void sfb_write_circle_generic3x3(const int xc, const int yc, uint32_t colour,
                                 sfb_framebuffer *const buffer,
                                 const int radius) {
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
        sfb_put_pixel3x3(x, y, buffer->data, buffer->w, buffer->h, colour);
      }
    }
  }
}

void sfb_put_pixel3x3(const int x, const int y, uint32_t *const buf,
                      const int w, const int h, uint32_t colour) {
  if (!buf) {
    return;
  }
  const int l = y * w + x;
  const int max = w * h;
  if (l < max && l >= 0) {
    buf[l] = sfb_blend_pixel(buf[l], colour);
  }
}
