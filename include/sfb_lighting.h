#ifndef SFB_LIGHTING_H
#define SFB_LIGHTING_H
#include "sfb_pix_def.h"

struct sfb_light_map {
  sfb_rgb *lightmap;
};

struct sfb_light_source {
  const int *x;
  const int *y;
  sfb_rgb col;
  int radius;
};

#endif
