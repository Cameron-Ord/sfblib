#define IMAGE_WRITING
#include "../include/sfb.h"

int main(void) {
  const int flags = SFB_BLEND_ENABLED | SFB_ENABLE_MULTITHREADED;
  sfb_framebuffer *buffer = sfb_create_framebuffer(300, 300, flags);
  sfb_obj *objs[12] = {
      sfb_create_rect(0, 0, 25, 25, 0xFF00FFFF),
      sfb_create_rect(25, 25, 25, 25, 0xFF00FFFF),
      sfb_create_rect(50, 50, 25, 25, 0xFF00FFFF),
      sfb_create_rect(75, 75, 25, 25, 0xFF00FFFF),
      sfb_create_rect(100, 100, 25, 25, 0xFF00FFFF),
      sfb_create_rect(125, 125, 25, 25, 0xFF00FFFF),
      sfb_create_rect(150, 150, 25, 25, 0xFF00FFFF),
      sfb_create_rect(175, 175, 25, 25, 0xFF00FFFF),
      sfb_create_rect(200, 200, 25, 25, 0xFF00FFFF),
      sfb_create_rect(225, 225, 25, 25, 0xFF00FFFF),
      sfb_create_rect(250, 250, 25, 25, 0xFF00FFFF),
      sfb_create_rect(275, 275, 25, 25, 0xFF00FFFF),
  };
  sfb_fb_clear(buffer, 0xFFFFFFFF);
  for (int i = 0; i < 12; i++) {
    sfb_write_obj_rect(objs[i], buffer, NULL);
  }
  sfb_image_write_png(300, 300, buffer->data, 4, "thread.png");
  sfb_free_framebuffer(buffer);
  return 0;
}
