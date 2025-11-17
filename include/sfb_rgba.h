#ifndef SFB_RGBA_H
#define SFB_RGBA_H
#include "sfb_pix_def.h"
// RGBA manipulation
void sfb_blend_pixel(uint8_t *blended, const uint8_t *dstp,
                     const uint8_t *srcp);
uint8_t sfb_mix_alpha(uint8_t dst, uint8_t src);
uint8_t sfb_col_blended(uint8_t dst, uint8_t src, uint8_t a);
uint8_t sfb_col_multiply(uint8_t dst, uint8_t src);
void sfb_multiply_pixel(uint8_t *add, const uint8_t *dstp, const uint8_t *srcp);
uint32_t SFB_RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
uint32_t SFB_RGB(uint8_t r, uint8_t g, uint8_t b);

#endif
