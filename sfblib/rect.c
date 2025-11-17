#include "../include/sfb_allocator.h"
#include "../include/sfb_fb_def.h"
#

sfb_rect *sfb_create_rect(int x, int y, int w, int h, sfb_rgba c,
                          int channels) {
  sfb_rect *rect = sfb_calloc(1, sizeof(sfb_rect));
  if (!rect) {
    return NULL;
  }

  const size_t nmemb = w * h * channels;
  rect->pixels.data = sfb_calloc(nmemb, sizeof(uint8_t));
  if (!rect->pixels.data) {
    sfb_free_rect(rect);
    return NULL;
  }

  rect->pixels.flags = sfb_calloc(nmemb, sizeof(int));
  if (!rect->pixels.flags) {
    sfb_free_rect(rect);
    return NULL;
  }

  const uint8_t colours[SFB_MAX_CHANNELS] = {c.r, c.g, c.b, c.a};
  for (int i = 0; i < w * h; i++) {
    for (int c = 0; c < channels; c++) {
      rect->pixels.data[i * channels + c] = colours[c];
    }
  }

  rect->channels = channels;
  rect->x = x;
  rect->y = y;
  rect->w = w;
  rect->h = h;
  rect->size = nmemb * sizeof(uint8_t);

  return rect;
}

sfb_rect *sfb_rect_from_sprite(int x, int y, const int w, const int h,
                               uint8_t *spr, int channels) {
  sfb_rect *rect = sfb_calloc(1, sizeof(sfb_rect));
  if (!rect) {
    return NULL;
  }

  const size_t nmemb = w * h * channels;
  rect->pixels.data = sfb_calloc(nmemb, sizeof(uint8_t));
  if (!rect->pixels.data) {
    sfb_free_rect(rect);
    return NULL;
  }
  sfb_memcpy(rect->pixels.data, spr, nmemb * sizeof(uint8_t));

  rect->pixels.flags = sfb_calloc(nmemb, sizeof(int));
  if (!rect->pixels.flags) {
    sfb_free_rect(rect);
    return NULL;
  }

  rect->channels = channels;
  rect->x = x;
  rect->y = y;
  rect->w = w;
  rect->h = h;
  rect->size = nmemb * sizeof(uint8_t);

  return rect;
}
