
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"
// Optional exposed functions that can be omitted when building
// Uses stb_image, either the user will need to call this to load sprites,
// or will have to provide their own uint32_t sprite buffer when drawing their
// own sprites.

#define IMAGE_LOADING
#include "../include/sfb.h"

sfb_obj *sfb_image_load(const char *filepath, int flags) {
  int w = 0, h = 0, img_channels = 0;
  fprintf(stdout, "Loading image..\n");

  // Always request 4 channel RGBA
  unsigned char *img =
      stbi_load(filepath, &w, &h, &img_channels, SFB_RGBA_CHANNELS);
  if (!img) {
    fprintf(stderr, "Failed to load image -> %s\n", stbi_failure_reason());
    return NULL;
  }
  printf("width: %d height: %d image channels: %d buffer channels: %d\n", w, h,
         img_channels, SFB_RGBA_CHANNELS);

  sfb_obj *obj = NULL;
  if (w > 0 && h > 0) {
    obj = sfb_rect_from_sprite(w, h, img, SFB_RGBA_CHANNELS);
  }

  fprintf(stdout, "Loaded: %p\n", (void *)obj);
  stbi_image_free(img);
  return obj;
}
