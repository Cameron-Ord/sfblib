#define IMAGE_WRITING
#include "../include/sfb.h"
#include "../include/sfb_flags.h"

#define WIDTH 400
#define HEIGHT 300
#define SIZE ((WIDTH / 8) * (HEIGHT / 6))

int main(void) {
  const int flags = SFB_BLEND_ENABLED;
  sfb_framebuffer *buffer = sfb_create_framebuffer(WIDTH, HEIGHT, flags);

  sfb_obj *obj =
      sfb_create_rect(WIDTH / 2, HEIGHT / 2, 32, 32,
                      (sfb_colour){255, 0, 0, 255}, buffer->channels);
  sfb_light_source *light =
      sfb_create_light_source(obj, buffer->channels, 2.0f, 1.00f, 0.80f,
                              (sfb_colour){255, 165, 0, 255}, 0);
  sfb_assign_light(obj, light);
  sfb_fb_clear(buffer, (sfb_colour){0, 0, 0, 255});

  sfb_write_obj_rect(obj, buffer, NULL);

  sfb_image_write_png(WIDTH, HEIGHT, buffer->pixels.data, "thread.png");
  sfb_free_framebuffer(buffer);
  return 0;
}
