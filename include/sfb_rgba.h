#ifndef SFB_RGBA_H
#define SFB_RGBA_H
#include "sfb_pix_def.h"

// RGBA manipulation
void sfb_blend_pixel(uint8_t *blended, const uint8_t *dstp,
                     const uint8_t *srcp);
uint8_t sfb_col_exposure(uint8_t col, float intensity);
uint8_t sfb_col_additive(uint8_t dst, uint8_t src);
uint8_t sfb_mix_alpha(uint8_t dst, uint8_t src);
uint8_t sfb_col_blended(uint8_t dst, uint8_t src, uint8_t a);
void sfb_light_additive(uint8_t *add, const uint8_t *dstp, const uint8_t *srcp);

// Pixel colours are represented in a union, however, typically when overwriting
// values I  strictly use uint32_t uint8_t values are just for convience and
// clarity when manipulating individual channels and also for ease of use with
// stb libraries.
// so this function just provides a simple way to put uint8_t values into the
// uint32_t value for the user
uint32_t SFB_RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
uint32_t SFB_RGB(uint8_t r, uint8_t g, uint8_t b);

#endif
