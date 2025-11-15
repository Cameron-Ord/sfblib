#define IMAGE_WRITING
#include "../include/sfb.h"

int main(void) {
  sfb_framebuffer *fb = sfb_create_framebuffer(800, 600, SFB_BLEND_ENABLED);
  sfb_fb_clear(fb, (sfb_colour){0, 0, 0, 255});
  sfb_write_rect_generic(0, 0, 100, 100, (sfb_colour){255, 255, 255, 255}, fb);
  sfb_image_write_png(800, 600, fb->pixels.data, fb->channels, "cube.png");
  sfb_free_framebuffer(fb);
  return 0;
}
