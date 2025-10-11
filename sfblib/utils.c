#include "../include/sfb.h"

int sfb_save_ppm(const sfb_framebuffer *const buffer, const char *path) {
  FILE *f = NULL;
  if (!(f = fopen(path, "wb"))) {
    fprintf(stderr, "!fopen()->%s\n", strerror(errno));
    return -1;
  }

  fprintf(f, "P6\n%d %d 255\n", buffer->w, buffer->h);
  if (ferror(f)) {
    fprintf(stderr, "!fprintf()->%s\n", strerror(errno));
    fclose(f);
    return -1;
  }

  for (int i = 0; i < buffer->w * buffer->h; i++) {
    const uint32_t pixel = buffer->data[i];
    // SFB FORMAT: ARGB ; PPM FORMAT RGB
    uint8_t bytes[3] = {
        (pixel >> (16)) & 0xFF,
        (pixel >> (8)) & 0xFF,
        (pixel >> (0)) & 0xFF,
    };

    fwrite(bytes, sizeof(bytes), 1, f);
    if (ferror(f)) {
      fprintf(stderr, "!fwrite()->%s\n", strerror(errno));
      fclose(f);
      return -1;
    }
  }
  return 1;
}
