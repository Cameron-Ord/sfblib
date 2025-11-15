#define IMAGE_WRITING
#include "../include/sfb.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third_party/stb_image_write.h"

int sfb_image_write_png(const int w, const int h, const uint8_t *pixels,
                        const char *filepath) {
  if (!stbi_write_png(filepath, w, h, SFB_RGBA_CHANNELS, pixels, 0)) {
    return 0;
  }
  return 1;
}
