#include "../include/sfb.h"

void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour) {
  for (int i = 0; i < buffer->w * buffer->h; i++) {
    buffer->data[i] = clear_colour;
  }
}

void sfb_write_obj_rect(const sfb_obj *const obj, sfb_framebuffer *const buffer) {
  const int y0 = obj->mat.m5;
  const int x0 = obj->mat.m2;
  // TODO: nearest neighbor scaling so that the scale coeffs in the mat actually
  // do something Otherwise I can only draw stuff at their exact pixel sizing
  for (int i = 0, y = y0; y < buffer->h && i < obj->h; i++, y++) {
    for (int j = 0, x = x0; x < buffer->w && j < obj->w; j++, x++) {
      const uint32_t obj_pixel = obj->pixels[i * obj->w + j];
      const uint8_t a = (obj_pixel >> 24) & 0xFF;
      if (a == 0){ 
        continue;
      }
      sfb_put_pixel(x, y, buffer->data, buffer->w, buffer->h, obj_pixel);
    }
  }
}

void sfb_write_rect_generic(int x0, int y0, int w0, int h0, uint32_t colour, sfb_framebuffer *const buffer){
  for(int i = 0, y = y0; y < buffer->h && i < h0; i++, y++){
    for(int j = 0, x = x0; x < buffer->w && j < w0; j++, x++){
      sfb_put_pixel(x, y, buffer->data, buffer->w, buffer->h, colour);
    }
  }
}

void sfb_write_circle_generic(const int xc, const int yc, uint32_t colour, sfb_framebuffer *const buffer, const int radius){
  const int xstart = xc - radius;
  const int ystart = yc - radius;
  const int xend = xc + radius;
  const int yend = yc + radius;

  for(int y = ystart; y <= yend; y++){
    for(int x = xstart; x <= xend; x++){
      const int dx = x - xc;
      const int dy = y - yc;
      if(dx*dx + dy*dy <= radius*radius){
        sfb_put_pixel(x, y, buffer->data, buffer->w, buffer->h, colour);
      }
    }
  }
}

void sfb_put_pixel(const int x, const int y, uint32_t *const buf, const int w, const int h, uint32_t colour){
  if(y <= h && x <= w){
    buf[y * w + x] = colour;
  }
}
