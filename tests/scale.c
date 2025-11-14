#define IMAGE_LOADING
#define IMAGE_WRITING
#include "../include/sfb.h"

int main(void) {
  sfb_framebuffer *fb = sfb_buffer_alloc(800, 600);
  sfb_fb_clear(fb, 0xFF00FF00);
  sfb_obj *sprite = sfb_image_load("sprite.png");
  sprite->pixels = sfb_scale_nearest_topleft_malloc(sprite->pixels, &sprite->w,
                                                    &sprite->h, 1);
  sfb_write_obj_rect(sprite, fb);
  sfb_image_write_png(sprite->w, sprite->h, sprite->pixels, 4,
                      "sprite_scaled.png");

  sfb_free_framebuffer(fb);
  sfb_free_obj(sprite);
  return 0;
}
