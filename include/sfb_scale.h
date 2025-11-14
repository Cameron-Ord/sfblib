#ifndef SFB_SCALE_H
#define SFB_SCALE_H
#include "sfb_pix_def.h"
#include <stdint.h>

sfb_pixel *sfb_scale_nearest_centered_malloc(sfb_pixel *src, int *srcw,
                                             int *srch, float scale);
sfb_pixel *sfb_scale_nearest_topleft_malloc(sfb_pixel *src, int *srcw,
                                            int *srch, int scale);

void sfb_scale_nearest_centered(sfb_pixel *src, int srcw, int srch,
                                sfb_pixel *dst, int dstw, int dsth);
void sfb_scale_nearest_topleft(sfb_pixel *src, int srcw, int srch,
                               sfb_pixel *dst, int dstw, int dsth, int scale);
#endif
