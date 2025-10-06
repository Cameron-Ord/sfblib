#include "../sfblib/sfb.h"

int main(void){
  sfb_framebuffer fb = sfb_buffer_alloc(800, 600);
  sfb_fb_clear(&fb, 0xFF00FF00);
  sfb_obj rect = sfb_alloc_obj(RECT, 100, 100, 50, 50, 0xFFFF00FF);
  sfb_write_obj(&rect, &fb);
  sfb_save_ppm(&fb, "rect.ppm");
  sfb_free(rect.pixels);
  sfb_free(fb.data);
  return 0;
}
