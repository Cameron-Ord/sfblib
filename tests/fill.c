#include "../sfblib/sfb.h"

int main(void){
  sfb_framebuffer *fb = sfb_buffer_alloc(800, 600);
  sfb_fb_clear(fb, 0xFF00FF00);
  sfb_save_ppm(fb, "filled.ppm");
  sfb_free_framebuffer(fb);
  return 0;
}
