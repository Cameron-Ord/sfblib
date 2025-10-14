#define IMAGE_WRITING
#include "../include/sfb.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third_party/stb_image_write.h"

int sfb_image_write_png(const int w, const int h, const uint32_t pixels[], int channels, const char *filepath){
  uint8_t *b = sfb_argb32_to_rgba8(pixels, w, h, channels);
  if(!b){
    return  0;
  }

  if(!stbi_write_png(filepath, w, h, channels, b, w * channels)){
    free(b);
    return 0;
  }

  free(b);
  return 1;
}
