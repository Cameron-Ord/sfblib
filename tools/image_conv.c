
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

#define IMAGE_LOADING
#include "../include/sfb.h"

void sfb_image_free(unsigned char *img) { stbi_image_free(img); }

// returns the image to be subseq. used int sfb_rect_from_sprite
// sfb_image_free should be called after calling sfb_rect_from_sprite
unsigned char *sfb_image_load(int *w, int *h, const char *filepath, int flags) {
  if (w == NULL || h == NULL) {
    return NULL;
  }
  // Always request 4 channel RGBA
  unsigned char *img = NULL;
  *w = 0, *h = 0;
  img = stbi_load(filepath, w, h, NULL, SFB_RGBA_CHANNELS);
  if (!img) {
    fprintf(stderr, "Failed to load image -> %s\n", stbi_failure_reason());
    return NULL;
  }

  if (*w > 0 && *h > 0) {
    return img;
  }

  sfb_image_free(img);
  return NULL;
}
