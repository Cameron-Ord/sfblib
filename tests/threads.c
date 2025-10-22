#include "../include/sfb.h"

int main(void) {
  const int flags = SFB_BLEND_ENABLED | SFB_ENABLE_MULTITHREADED;
  sfb_framebuffer *buffer = sfb_create_framebuffer(300, 300, flags);
  sfb_free_framebuffer(buffer);
  return 0;
}
