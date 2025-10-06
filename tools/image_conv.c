
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"
// Optional exposed functions that can be omitted when building
// Uses stb_image, either the user will need to call this to load sprites,
// or will have to provide their own uint32_t sprite buffer when drawing their
// own sprites.

#define IMAGE_LOADING
#include "../sfblib/sfb.h"

sfb_obj sfb_image_load(const char *filepath) {
  // Todo: error/access handling
  int w, h, channels;
  const int bpp = 4;
  unsigned char *img = stbi_load(filepath, &w, &h, &channels, bpp);
  if (!img) {
    fprintf(stderr, "Failed to load image -> %s\n", stbi_failure_reason());
    return (sfb_obj){0};
  }
  printf("width: %d height: %d channels: %d\n", w, h, channels);
  
  uint32_t *sprite = calloc(w * h, sizeof(uint32_t));
  if (!sprite) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return (sfb_obj){0};
  }

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      // stbi_image always produces RGBA format
      const int access = (y * w + x) * bpp;

      const uint8_t r = img[access + 0];
      const uint8_t g = img[access + 1];
      const uint8_t b = img[access + 2];
      const uint8_t a = img[access + 3];

      const uint32_t pixel = (a << 24) | (r << 16) | (g << 8) | b;
      sprite[y * w + x] = pixel;
    }
  }
  
  sfb_mat3x3 delta = sfb_identity();
  sfb_mat3x3 obj = sfb_identity();

  delta = sfb_scale(delta, 1.0f, 1.0f);
  delta = sfb_translate(delta, 0, 0);

  return (sfb_obj){sfb_mmult(&delta, &obj), w, h, RECT, sprite};
}

void sfb_sprite_free(uint32_t *sprite) {
  if (sprite) {
    free(sprite);
  }
}
