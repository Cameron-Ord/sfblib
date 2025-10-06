#define IMAGE_LOADING
#include "../sfblib/sfb.h"

int main(void) {
  uint32_t *sprite = sfb_image_load("test.png");
  sfb_image_free(sprite);
  return 0;
}
