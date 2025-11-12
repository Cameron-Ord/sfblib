#ifndef SFB_PIX_DEF_H
#define SFB_PIX_DEF_H

#include <stdint.h>

typedef struct sfb_pixel sfb_pixel;

#define SFB_COL_CHANNELS 4
#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

union pixel_data {
  uint32_t uint32_pixel;
  uint8_t uint8_pixel_array[SFB_COL_CHANNELS];
};

struct sfb_pixel {
  union pixel_data pixel;
  int flags;
};

#endif
