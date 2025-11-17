#ifndef SFB_PIX_DEF_H
#define SFB_PIX_DEF_H

#include <stdint.h>

typedef struct sfb_pixel_data sfb_pixel_data;
typedef struct sfb_rgba sfb_rgba;
typedef struct sfb_rgb sfb_rgb;

#define SFB_MAX_CHANNELS 4
#define SFB_RGB_CHANNELS 3
#define SFB_RGBA_CHANNELS 4

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

struct sfb_rgb {
  uint8_t r, g, b;
};

struct sfb_rgba {
  uint8_t r, g, b, a;
};

struct sfb_pixel_data {
  uint8_t *data;
  int *flags;
};

#endif
