#include "../include/sfb.h"

void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour) {
  for(int i = 0; i < buffer->w * buffer->h; i++){
    buffer->data[i] = clear_colour;
  }
}

void sfb_write_obj_rect(const sfb_obj *const obj,
                        sfb_framebuffer *const buffer) {
  int x0, y0;
  const int cond = (buffer->c && *buffer->c) ? 1 : 0;
  switch (cond) {
  default: {
    y0 = obj->mat.m5;
    x0 = obj->mat.m2;
  } break;
  case 1: {
    y0 = obj->mat.m5 - (*buffer->c)->y;
    x0 = obj->mat.m2 - (*buffer->c)->x;
  } break;
  }

  for (int dy = 0; dy < obj->h; dy++) {
    const int y = y0 + dy;
    if (y < 0 || y >= buffer->h)
      continue;

    for (int dx = 0; dx < obj->w; dx++) {
      const int x = x0 + dx;
      if (x < 0 || x >= buffer->w)
        continue;

      sfb_put_pixel(x, y, buffer->data, buffer->w, buffer->h,
                    obj->pixels[dy * obj->w + dx]);
    }
  }
}

void sfb_write_rect_generic(int x0, int y0, int w0, int h0, uint32_t colour,
                            sfb_framebuffer *const buffer) {
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
  const int l = y * w + x;
  const int max = w * h;
  if (l < max && l >= 0) {
    buf[l] = sfb_blend_pixel(buf[l], colour);
  }
}
