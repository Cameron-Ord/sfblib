#include "../include/sfb.h"

int main(void) {
  sfb_framebuffer *f = sfb_buffer_alloc(600, 400);
  sfb_fb_clear(f, 0xFFFFFFFF);
  sfb_write_rect_generic(10, 10, 50, 50, 0xFF000000, f);
  sfb_write_rect_generic(20, 20, 50, 50, 0xAAFF0000, f);
  sfb_save_ppm(f, "blend.ppm");
  sfb_free_framebuffer(f);

  return 0;
}
