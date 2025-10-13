#include "../include/sfb.h"

int main(void){
  sfb_framebuffer *fb = sfb_buffer_alloc(800, 600);
  sfb_fb_clear(fb, 0xFF00FF00);
  sfb_write_rect_generic(0, 0, 100, 100, 0xFFFF00FF, fb);
  sfb_save_ppm(fb, "rect.ppm");
  sfb_free_framebuffer(fb);
  return 0;
}
