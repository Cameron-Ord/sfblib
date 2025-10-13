#define IMAGE_WRITING
#include "../include/sfb.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third_party/stb_image_write.h"

int sfb_image_write_png(const int w, const int h, const uint32_t *pixels, int channels, const char *filepath){
  unsigned char *buffer = malloc(w * h * channels * sizeof(unsigned char));
  if(!buffer){
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return 0;
  }

  for(int i = 0; i < w * h; i++){
    buffer[i * channels + 0] = (pixels[i] >> 16)&0xFF;
    buffer[i * channels + 1] = (pixels[i] >> 8)&0xFF;
    buffer[i * channels + 2] = (pixels[i] >> 0)&0xFF;
    buffer[i * channels + 3] = (pixels[i] >> 24)&0xFF;
  }

  if(!stbi_write_png(filepath, w, h, channels, buffer, w * channels)){
    free(buffer);
    return 0;
  }

  free(buffer);
  return 1;
}
