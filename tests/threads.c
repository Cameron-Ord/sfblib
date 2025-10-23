#define IMAGE_WRITING
#include "../include/sfb.h"

int main(void) {
  const int flags = SFB_BLEND_ENABLED | SFB_ENABLE_MULTITHREADED;
  sfb_framebuffer *buffer = sfb_create_framebuffer(300, 300, flags);
  sfb_obj *objs[24 * 24];
  int a = (24 * 24) / 300;
  int k = 300 / a;
  for (int i = 0; i < (24 * 24); i++) {
    objs[i] = sfb_create_rect(k * i, 0, k, k, 0xFF00FF00);
  }
  sfb_fb_clear(buffer, 0xFFFFFFFF);
  int j = 0;
  for (int i = 0; i < 24 * 24; i++) {
    sfb_write_obj_rect(objs[i], buffer, NULL);
  }
  sfb_image_write_png(300, 300, buffer->data, 4, "thread.png");
  sfb_free_framebuffer(buffer);
  return 0;
}
