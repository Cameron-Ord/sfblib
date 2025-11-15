#include "../include/sfb_rgba.h"
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

// Maybe in the future - add blendmodes
void sfb_blend_pixel(uint8_t *blended, const uint8_t *dstp,
                     const uint8_t *srcp) {
  uint8_t rdst = *(dstp + RED);
  uint8_t gdst = *(dstp + GREEN);
  uint8_t bdst = *(dstp + BLUE);
  uint8_t adst = *(dstp + ALPHA);

  uint8_t rsrc = *(srcp + RED);
  uint8_t gsrc = *(srcp + GREEN);
  uint8_t bsrc = *(srcp + BLUE);
  uint8_t asrc = *(srcp + ALPHA);

  blended[RED] = sfb_col_blended(rdst, rsrc, asrc);
  blended[GREEN] = sfb_col_blended(gdst, gsrc, asrc);
  blended[BLUE] = sfb_col_blended(bdst, bsrc, asrc);
  blended[ALPHA] = sfb_mix_alpha(adst, asrc);
}

void sfb_light_additive(uint8_t *add, const uint8_t *dstp,
                        const uint8_t *srcp) {

  uint8_t rdst = *(dstp + RED);
  uint8_t gdst = *(dstp + GREEN);
  uint8_t bdst = *(dstp + BLUE);

  uint8_t rsrc = *(srcp + RED);
  uint8_t gsrc = *(srcp + GREEN);
  uint8_t bsrc = *(srcp + BLUE);

  add[RED] = sfb_col_additive(rdst, rsrc);
  add[GREEN] = sfb_col_additive(gdst, gsrc);
  add[BLUE] = sfb_col_additive(bdst, bsrc);
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
