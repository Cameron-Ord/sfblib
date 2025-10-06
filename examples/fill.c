#include "sfb.h"

int main(void){
  uint32_t *framebuffer = sfb_buffer_alloc(800, 600);
  sfb_fb_fill(framebuffer, 800, 600, 0xFF00FF00);
  sfb_save_ppm(framebuffer, 800, 600, "filled.ppm");
  sfb_free(framebuffer);
  return 0;
}
