#include "../include/sfb_rgba.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const uint8_t CHANNEL_MAX = UINT8_MAX;

static uint8_t sfb_channel_clamp(uint8_t c) {
  return (c > CHANNEL_MAX) ? CHANNEL_MAX : c;
}

uint32_t sfb_argb32_to_rgba32(uint32_t argb) {
  const uint32_t rgba = (argb << 8) | (argb >> 24);
  return rgba;
}

uint32_t sfb_rgba32_to_argb32(uint32_t rgba) {
  const uint32_t argb = (rgba >> 8) | (rgba << 24);
  return argb;
}

void sfb_shift_bits_left_uint32(uint32_t *pixel, uint8_t col, int bit) {
  *pixel |= (uint32_t)(col << bit);
}

void sfb_shift_bits_right_uint8(uint8_t *col, uint32_t pixel, int bit) {
  *col |= (uint8_t)(pixel >> bit) & 0xFF;
}

uint32_t *sfb_argb8_to_argb32(const uint8_t pixels[], int w, int h,
                              int channels) {
  if (channels != SFB_COL_CHANNELS) {
    fprintf(stderr, "Incorrect channel count! Need: %d Have: %d\n",
            SFB_COL_CHANNELS, channels);
    return NULL;
  }

  uint32_t *buf = malloc(w * h * sizeof(uint32_t));
  if (!buf) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  const int stride = channels;
  for (int i = 0; i < w * h; i++) {
    uint32_t argb = 0;
    for (int c = 0, bit = 24; c < channels && bit >= 0; c++, bit -= 8) {
      sfb_shift_bits_left_uint32(&argb, pixels[i * stride + c], bit);
    }
    buf[i] = argb;
  }

  return buf;
}

uint32_t *sfb_rgba8_to_argb32(const uint8_t pixels[], int w, int h,
                              int channels) {
  if (channels != SFB_COL_CHANNELS) {
    fprintf(stderr, "Incorrect channel count! Need: %d Have: %d\n",
            SFB_COL_CHANNELS, channels);
    return NULL;
  }

  uint32_t *buf = malloc(w * h * sizeof(uint32_t));
  if (!buf) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  const int stride = channels;
  for (int i = 0; i < w * h; i++) {
    uint32_t rgba = 0;
    for (int c = 0, bit = 24; c < channels && bit >= 0; c++, bit -= 8) {
      sfb_shift_bits_left_uint32(&rgba, pixels[i * stride + c], bit);
    }
    buf[i] = sfb_rgba32_to_argb32(rgba);
  }

  return buf;
}

uint8_t *sfb_argb32_to_rgba8(const uint32_t pixels[], int w, int h,
                             int channels) {
  if (channels != SFB_COL_CHANNELS) {
    fprintf(stderr, "Incorrect channel count! Need: %d Have: %d\n",
            SFB_COL_CHANNELS, channels);
    return NULL;
  }

  uint8_t *buf = malloc(w * h * channels * sizeof(uint8_t));
  if (!buf) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  const int stride = channels;
  for (int i = 0; i < w * h; i++) {
    uint32_t rgba = sfb_argb32_to_rgba32(pixels[i]);
    for (int c = 0, bit = 24; c < channels && bit >= 0; c++, bit -= 8) {
      sfb_shift_bits_right_uint8(&buf[i * stride + c], rgba, bit);
    }
  }

  return buf;
}

uint32_t sfb_blend_pixel(uint32_t dstp, uint32_t srcp) {
  uint8_t a1 = (dstp >> 24) & 0xFF;
  uint8_t r1 = (dstp >> 16) & 0xFF;
  uint8_t g1 = (dstp >> 8) & 0xFF;
  uint8_t b1 = dstp & 0xFF;

  uint8_t a2 = (srcp >> 24) & 0xFF;
  uint8_t r2 = (srcp >> 16) & 0xFF;
  uint8_t g2 = (srcp >> 8) & 0xFF;
  uint8_t b2 = srcp & 0xFF;

  uint8_t a3 = sfb_mix_alpha(a1, a2);
  uint8_t r3 = sfb_col_blended(r1, r2, a2);
  uint8_t g3 = sfb_col_blended(g1, g2, a2);
  uint8_t b3 = sfb_col_blended(b1, b2, a2);

  return (a3 << 24) | (r3 << 16) | (g3 << 8) | b3;
}

uint32_t sfb_light_additive(uint32_t dstp, uint32_t srcp) {
  uint8_t r1 = (dstp >> 16) & 0xFF;
  uint8_t g1 = (dstp >> 8) & 0xFF;
  uint8_t b1 = dstp & 0xFF;

  uint8_t r2 = (srcp >> 16) & 0xFF;
  uint8_t g2 = (srcp >> 8) & 0xFF;
  uint8_t b2 = srcp & 0xFF;

  uint8_t a = (srcp >> 24) & 0xFF;
  uint8_t r3 = sfb_col_additive(r1, r2);
  uint8_t g3 = sfb_col_additive(g1, g2);
  uint8_t b3 = sfb_col_additive(b1, b2);

  return (a << 24) | (r3 << 16) | (g3 << 8) | b3;
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
