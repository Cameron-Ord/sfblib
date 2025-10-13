
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"
// Optional exposed functions that can be omitted when building
// Uses stb_image, either the user will need to call this to load sprites,
// or will have to provide their own uint32_t sprite buffer when drawing their
// own sprites.

#define IMAGE_LOADING
#include "../include/sfb.h"

sfb_obj *sfb_image_load(const char *filepath) {
  int w = 0, h = 0, channels = 0;
  const int requested_channels = 4;

  unsigned char *img = stbi_load(filepath, &w, &h, &channels, requested_channels);
  if (!img) {
    fprintf(stderr, "Failed to load image -> %s\n", stbi_failure_reason());
    return NULL;
  }
  printf("width: %d height: %d channels: %d\n", w, h, channels);

  if(w > 0 && h > 0 && channels > 0){
    uint32_t sprite[w * h];
    memset(sprite, 0, w * h * sizeof(uint32_t));
    const uint8_t *end = img + (w * h * channels);
    
    for(int y = 0; y < h; y++){
      const int offset = y * w;
      for(int x = 0; x < w; x++){
        const int access = offset + x;
        uint8_t *slice = img + (access * channels);
        uint8_t colours[channels];
      
        for(int c = 0; c < channels && ((slice + c) < end); c++){
          colours[c] = *(slice + c);
        }

        const uint8_t r = colours[0];
        const uint8_t g = colours[1];
        const uint8_t b = colours[2];
        const uint8_t a = (channels == requested_channels) ? colours[3] : 255;

        const uint32_t pixel = (a << 24) | (r << 16) | (g << 8) | b;
        sprite[y * w + x] = pixel;
      }
    }
    stbi_image_free(img);
    return sfb_rect_from_sprite(w, h, sprite);
  } 
  
  stbi_image_free(img);
  return NULL;
}
