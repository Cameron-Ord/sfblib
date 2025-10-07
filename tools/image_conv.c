
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
  const int requested_channels = 4;
  unsigned char *img = stbi_load(filepath, &w, &h, &channels, requested_channels);
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
      // stbi_image always produces RGBA or RGB format
      // [RGBA] [RGBA] [RGBA] [RGBA] 
      const int access = (y * w + x) * channels;

      const uint8_t r = img[access + 0];
      const uint8_t g = img[access + 1];
      const uint8_t b = img[access + 2];
      const uint8_t a = (channels == requested_channels) ? img[access + 3] : 255;

      const uint32_t pixel = (a << 24) | (r << 16) | (g << 8) | b;
      sprite[y * w + x] = pixel;
    }
  }
  return (sfb_obj){sfb_identity(), w, h, RECT, sprite};
}

void sfb_sprite_free(uint32_t *sprite) {
  if (sprite) {
    free(sprite);
  }
}
