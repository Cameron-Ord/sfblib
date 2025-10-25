#ifndef SFB_RGBA_H
#define SFB_RGBA_H
#include <stdint.h>
// Just exposes functions

#define SFB_COL_CHANNELS 4
#define ALPHA 0
#define RED 1
#define GREEN 2
#define BLUE 3

// RGBA manipulation
uint32_t sfb_blend_pixel(uint32_t srcp, uint32_t dstp);
uint8_t sfb_col_exposure(uint8_t col, float intensity);
uint8_t sfb_col_additive(uint8_t dst, uint8_t src);
uint8_t sfb_mix_alpha(uint8_t src, uint8_t dst);
uint8_t sfb_col_blended(uint8_t src, uint8_t dst, uint8_t a);
uint32_t sfb_light_additive(uint32_t dstp, uint32_t srcp);

void sfb_shift_bits_left_uint32(uint32_t *pixel, uint8_t col, int bit);
void sfb_shift_bits_right_uint8(uint8_t *col, uint32_t pixel, int bit);
uint32_t *sfb_argb8_to_argb32(const uint8_t pixels[], int w, int h,
                              int channels);
uint32_t *sfb_rgba8_to_argb32(const uint8_t pixels[], int w, int h,
                              int channels);
uint8_t *sfb_argb32_to_rgba8(const uint32_t pixels[], int w, int h,
                             int channels);
uint32_t sfb_rgba32_to_argb32(uint32_t rgba);
uint32_t sfb_argb32_to_rgba32(uint32_t argb);
#endif
