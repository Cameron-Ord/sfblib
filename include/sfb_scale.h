#ifndef SFB_SCALE_H
#define SFB_SCALE_H
#include <stdint.h>

uint32_t *sfb_scale_nearest_centered_malloc(uint32_t *src, int *srcw, int *srch,
                                            float scale);
uint32_t *sfb_scale_nearest_topleft_malloc(uint32_t *src, int *srcw, int *srch,
                                           int scale);
void sfb_scale_nearest_centered(uint32_t *src, int srcw, int srch,
                                uint32_t *dst, int dstw, int dsth);
void sfb_scale_nearest_topleft(uint32_t *src, int srcw, int srch, uint32_t *dst,
                               int dstw, int dsth, int scale);
#endif
