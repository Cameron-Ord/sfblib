#ifndef SFB_SCALE_H
#define SFB_SCALE_H
#include "sfb_pix_def.h"
#include <stdint.h>

uint8_t *sfb_scale_nearest_centered_malloc(uint8_t *src, int *srcw, int *srch,
                                           float scale, int channels);
uint8_t *sfb_scale_nearest_topleft_malloc(uint8_t *src, int *srcw, int *srch,
                                          int scale, int channels);

void sfb_scale_nearest_centered(uint8_t *src, int srcw, int srch, uint8_t *dst,
                                int dstw, int dsth, int channels);
void sfb_scale_nearest_topleft(uint8_t *src, int srcw, int srch, uint8_t *dst,
                               int dstw, int dsth, int scale, int channels);
#endif
