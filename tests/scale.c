#define IMAGE_LOADING
#define IMAGE_WRITING
#include "../include/sfb.h"

int main(void) {
  sfb_framebuffer *fb = sfb_create_framebuffer(800, 600, SFB_BLEND_ENABLED);
  sfb_fb_clear(fb, (sfb_colour){0, 0, 0, 255});
  sfb_obj *sprite = sfb_image_load("sprite.png", 0);
  if (!sprite) {
    fprintf(stderr, "Failed to load sprite\n");
    return 1;
  }
  sprite->pixels.data = sfb_scale_nearest_topleft_malloc(
      sprite->pixels.data, &sprite->w, &sprite->h, 2, sprite->channels);
  sfb_write_obj_rect(sprite, fb, NULL);
  sfb_image_write_png(sprite->w, sprite->h, sprite->pixels.data,
                      "sprite_scaled.png");

  sfb_free_framebuffer(fb);
  sfb_free_obj(sprite);
  return 0;
}
