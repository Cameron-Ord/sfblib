
#include "../include/sfb.h"

int main(void){
  sfb_framebuffer *fb = sfb_buffer_alloc(300, 300);
  sfb_fb_clear(fb, 0xFF00FF00);
  sfb_write_circle_generic(300 / 2, 300 / 2 , 0xFFFFFFFF, fb, 50);
  sfb_save_ppm(fb, "circle.ppm");
  sfb_free_framebuffer(fb);
  return 0;
}
