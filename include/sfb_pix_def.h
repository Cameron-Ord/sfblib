#ifndef SFB_PIX_DEF_H
#define SFB_PIX_DEF_H

#include <stdint.h>

typedef struct sfb_pixel_data sfb_pixel_data;

#define SFB_COL_CHANNELS 4
#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

struct sfb_pixel_data {
  uint8_t *pixels;
  int *flags;
};

#endif
