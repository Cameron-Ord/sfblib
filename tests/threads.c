#define IMAGE_WRITING
#include "../include/sfb.h"
#include "../include/sfb_flags.h"

#define WIDTH 400
#define HEIGHT 300
#define SIZE ((WIDTH / 8) * (HEIGHT / 6))

int main(void) {
  const int flags = SFB_BLEND_ENABLED | SFB_ENABLE_MULTITHREADED;
  sfb_framebuffer *buffer = sfb_create_framebuffer(WIDTH, HEIGHT, flags);
  sfb_obj *objs[(WIDTH / 8) * (HEIGHT / 6)];
  for (int i = 0; i < (WIDTH / 8) * (HEIGHT / 6); i++) {
    objs[i] = sfb_create_rect(i * 8, i * 6, 8, 6, 0xFF00FF00);
  }
  sfb_fb_clear(buffer, 0xFFFFFFFF);

  int j = 0;
  while (1) {
    sfb_write_obj_rect(objs[j % SIZE], buffer, NULL);
    j++;
  }

  sfb_image_write_png(WIDTH, HEIGHT, buffer->data, 4, "thread.png");
  sfb_free_framebuffer(buffer);
  return 0;
}
