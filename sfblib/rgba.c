#include "../include/sfb.h"

void sfb_pack_argb32(uint8_t c[COL_COUNT], uint32_t pixel) {
  c[ALPHA] = (pixel >> (24)) & 0xFF;
  c[RED] = (pixel >> (16)) & 0xFF;
  c[GREEN] = (pixel >> (8)) & 0xFF;
  c[BLUE] = pixel & 0xFF;
}

uint32_t sfb_unpack_argb32(const uint8_t c[COL_COUNT]) {
  uint32_t pixel =
      (c[ALPHA] << 24) | (c[RED] << 16) | (c[GREEN] << 8) | (c[BLUE]);
  return pixel;
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
  uint8_t r3 = sfb_mix_col(r1, r2, a2);
  uint8_t g3 = sfb_mix_col(g1, g2, a2);
  uint8_t b3 = sfb_mix_col(b1, b2, a2);

  return (a3 << 24) | (r3 << 16) | (g3 << 8) | b3;
}

uint8_t sfb_mix_alpha(uint8_t dst, uint8_t src) {
  return src + (dst * (255 - src)) / 255;
}

uint8_t sfb_mix_col(uint8_t dst, uint8_t src, uint8_t a) {
    const uint8_t blended = (src * a + dst * (255 - a)) / 255;
  return (blended > 255) ? 255 : blended;
}
