#define IMAGE_LOADING
#include "../sfblib/sfb.h"

int main(void) {
  sfb_framebuffer fb = sfb_buffer_alloc(800, 600);
  sfb_fb_clear(&fb, 0xFF00FF00);
  sfb_obj sprite = sfb_image_load("sprite.png");
  sfb_write_obj(&sprite, &fb);
  sfb_save_ppm(&fb, "sprite.ppm");
  sfb_sprite_free(sprite.pixels);
  sfb_free(fb.data);
  return 0;
}
