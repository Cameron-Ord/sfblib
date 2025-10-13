#define IMAGE_LOADING
#include "../include/sfb.h"

int main(void) {
  sfb_framebuffer *fb = sfb_buffer_alloc(800, 600);
  sfb_fb_clear(fb, 0xFF00FF00);
  sfb_obj *sprite = sfb_image_load("sprite.png");
  sfb_write_obj_rect(sprite, fb);
  sfb_save_ppm(fb, "sprite.ppm");

  sfb_free_framebuffer(fb);
  sfb_free_obj(sprite);
  return 0;
}
