#ifndef SFB_PIX_DEF_H
#define SFB_PIX_DEF_H

#include <stdint.h>

typedef struct sfb_pixel_data sfb_pixel_data;
typedef struct sfb_colour sfb_colour;

#define SFB_MAX_CHANNELS 4
#define SFB_RGB_CHANNELS 3
#define SFB_RGBA_CHANNELS 4

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

struct sfb_colour {
  uint8_t r, g, b, a;
};

struct sfb_pixel_data {
  uint8_t *data;
  int *flags;
};

#endif
