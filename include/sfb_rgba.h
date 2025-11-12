#ifndef SFB_RGBA_H
#define SFB_RGBA_H
#include "sfb_pix_def.h"

// RGBA manipulation
uint32_t sfb_blend_pixel(uint32_t srcp, uint32_t dstp);
uint8_t sfb_col_exposure(uint8_t col, float intensity);
uint8_t sfb_col_additive(uint8_t dst, uint8_t src);
uint8_t sfb_mix_alpha(uint8_t src, uint8_t dst);
uint8_t sfb_col_blended(uint8_t src, uint8_t dst, uint8_t a);
uint32_t sfb_light_additive(uint32_t dstp, uint32_t srcp);

sfb_pixel *sfb_pixels_from_rgba8(const uint8_t pixels[], int w, int h,
                                 int channels);

uint32_t *sfb_argb8_to_argb32(const uint8_t pixels[], int w, int h,
                              int channels);
uint32_t *sfb_rgba8_to_rgba32(const uint8_t pixels[], int w, int h,
                              int channels);
uint8_t *sfb_rgba32_to_rgba8(const uint32_t pixels[], int w, int h,
                             int channels);

uint32_t SFB_RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
uint32_t SFB_RGB(uint8_t r, uint8_t g, uint8_t b);

#endif
