
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"
// Optional exposed functions that can be omitted when building
// Uses stb_image, either the user will need to call this to load sprites,
// or will have to provide their own uint32_t sprite buffer when drawing their
// own sprites.

#define IMAGE_LOADING
#include "../include/sfb.h"

sfb_obj *sfb_image_load(const char *filepath, int flags) {
  int w = 0, h = 0, channels = 0;
  const int requested_channels = SFB_COL_CHANNELS;

  unsigned char *img =
      stbi_load(filepath, &w, &h, &channels, requested_channels);
  if (!img) {
    fprintf(stderr, "Failed to load image -> %s\n", stbi_failure_reason());
    return NULL;
  }
  printf("width: %d height: %d channels: %d requested channels: %d\n", w, h,
         channels, requested_channels);

  // TODO: Add support for RGB later I guess
  if (w > 0 && h > 0 && channels == requested_channels) {
    sfb_pixel *pixels = sfb_pixels_from_rgba8(img, w, h, channels);
    if (!pixels) {
      stbi_image_free(img);
      return NULL;
    }

    sfb_obj *o = sfb_rect_from_sprite(w, h, &pixels);
    return o;
  } else {
    fprintf(stderr,
            "Incorrect channel count or size! CHANNELS-> Need:%d Have:%d "
            "SIZE-> %dx%d\n",
            SFB_COL_CHANNELS, channels, w, h);
  }

  stbi_image_free(img);
  return NULL;
}
