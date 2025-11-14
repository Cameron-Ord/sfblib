#define IMAGE_WRITING
#include "../include/sfb.h"

int main(void) {
  sfb_framebuffer *fb = sfb_create_framebuffer(800, 600, SFB_BLEND_ENABLED);
  sfb_fb_clear(fb, 0xFF00FF00);
  sfb_write_rect_generic(0, 0, 100, 100, 0xFFFF00FF, fb);
  sfb_image_write_png(800, 600, fb->pixels, 4, "cube.png");
  sfb_free_framebuffer(fb);
  return 0;
}
