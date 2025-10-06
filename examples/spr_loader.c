#define IMAGE_LOADING
#include "../sfblib/sfb.h"

int main(void) {
  sfb_obj sprite = sfb_image_load("test.png");
  sfb_sprite_free(sprite.pixels);
  return 0;
}
