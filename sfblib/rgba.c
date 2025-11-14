#include "../include/sfb_rgba.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const uint8_t CHANNEL_MAX = UINT8_MAX;

static uint8_t sfb_channel_clamp(uint8_t c) {
  return (c > CHANNEL_MAX) ? CHANNEL_MAX : c;
}

// static uint32_t sfb_argb32_to_rgba32(uint32_t argb) {
// const uint32_t rgba = (argb << 8) | (argb >> 24);
// return rgba;
//}
//
// static uint32_t sfb_rgba32_to_argb32(uint32_t rgba) {
// const uint32_t argb = (rgba >> 8) | (rgba << 24);
// return argb;
//}

void sfb_shift_bits_left_uint32(uint32_t *pixel, uint8_t col, int bit) {
  *pixel |= (uint32_t)(col << bit);
}

void sfb_shift_bits_right_uint8(uint8_t *col, uint32_t pixel, int bit) {
  *col |= (uint8_t)(pixel >> bit) & 0xFF;
}

sfb_pixel *sfb_pixels_from_rgba8(const uint8_t pixels[], int w, int h,
                                 int channels) {
  if (channels != SFB_COL_CHANNELS) {
    fprintf(stderr, "Incorrect channel count! Need: %d Have: %d\n",
            SFB_COL_CHANNELS, channels);
    return NULL;
  }

  sfb_pixel *buf = malloc(w * h * sizeof(sfb_pixel));
  if (!buf) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  for (int i = 0; i < w * h; i++) {
    buf[i].flags = 0;
    memset(&buf[i].pixel, 0, sizeof(union pixel_data));
  }

  for (int i = 0; i < w * h; i++) {
    sfb_pixel *target = &buf[i];
    for (int c = 0; c < channels; c++) {
      uint8_t val = pixels[i * channels + c];
      target->pixel.uint8_pixel_array[c] = val;
    }
  }

  return buf;
}

// Maybe in the future - add blendmodes
uint32_t sfb_blend_pixel(const uint8_t dstp[SFB_COL_CHANNELS],
                         const uint8_t srcp[SFB_COL_CHANNELS]) {

  uint8_t r = sfb_col_blended(dstp[RED], srcp[RED], srcp[ALPHA]);
  uint8_t g = sfb_col_blended(dstp[GREEN], srcp[GREEN], srcp[ALPHA]);
  uint8_t b = sfb_col_blended(dstp[BLUE], srcp[BLUE], srcp[ALPHA]);
  uint8_t a = sfb_mix_alpha(dstp[ALPHA], srcp[ALPHA]);

  return (r << 24) | (g << 16) | (b << 8) | a;
}

uint32_t sfb_light_additive(const uint8_t dstp[SFB_COL_CHANNELS],
                            const uint8_t srcp[SFB_COL_CHANNELS]) {

  uint8_t r = sfb_col_additive(dstp[RED], srcp[RED]);
  uint8_t g = sfb_col_additive(dstp[GREEN], srcp[GREEN]);
  uint8_t b = sfb_col_additive(dstp[BLUE], srcp[BLUE]);

  return (r << 24) | (g << 16) | (b << 8) | CHANNEL_MAX;
}

uint8_t sfb_mix_alpha(uint8_t dst, uint8_t src) {
  return src + (dst * (CHANNEL_MAX - src)) / CHANNEL_MAX;
}

uint8_t sfb_col_exposure(uint8_t col, float intensity) {
  return sfb_channel_clamp(col * intensity);
}

uint8_t sfb_col_additive(uint8_t dst, uint8_t src) {
  return sfb_channel_clamp(src + dst);
}

uint8_t sfb_col_blended(uint8_t dst, uint8_t src, uint8_t a) {
  const uint8_t blended = (src * a + dst * (CHANNEL_MAX - a)) / CHANNEL_MAX;
  return sfb_channel_clamp(blended);
}
