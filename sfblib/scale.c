#include "../include/sfb_scale.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int scale_clamp(int min, int max, int n) {
  if (n < min) {
    return min;
  } else if (n > max) {
    return max;
  } else {
    return n;
  }
}

void sfb_scale_nearest_topleft(uint8_t *src, int srcw, int srch, uint8_t *dst,
                               int dstw, int dsth, int scale, int channels) {
  if (!src || !dst) {
    return;
  }

  for (int y = 0; y < dsth; y++) {
    int sy = scale_clamp(0, srch - 1, y / scale);
    for (int x = 0; x < dstw; x++) {
      int sx = scale_clamp(0, srcw - 1, x / scale);

      for (int c = 0; c < channels; c++) {
        const int srci = (sy * srcw + sx);
        const int dsti = (y * dstw + x);
        dst[dsti * channels + c] = src[srci * channels + c];
      }
    }
  }
}

void sfb_scale_nearest_centered(uint8_t *src, int srcw, int srch, uint8_t *dst,
                                int dstw, int dsth, int channels) {
  if (!src || !dst) {
    return;
  }

  for (int y = 0; y < dsth; y++) {
    const float fy = (y + 0.5f) * ((float)srch / dsth) - 0.5f;
    int sy = scale_clamp(0, srch - 1, roundf(fy));
    for (int x = 0; x < dstw; x++) {
      const float fx = (x + 0.5f) * ((float)srcw / dsth) - 0.5f;
      int sx = scale_clamp(0, srcw - 1, roundf(fx));

      for (int c = 0; c < channels; c++) {
        const int srci = (sy * srcw + sx);
        const int dsti = (y * dstw + x);
        dst[dsti * channels + c] = src[srci * channels + c];
      }
    }
  }
}

// Integer scaling
uint8_t *sfb_scale_nearest_topleft_malloc(uint8_t *src, int *srcw, int *srch,
                                          const int scale, int channels) {
  if (!src || !srcw || !srch) {
    return NULL;
  }
  const int dstw = *srcw * scale;
  const int dsth = *srch * scale;

  uint8_t *dst = calloc(dstw * dsth * channels, sizeof(uint8_t));
  if (!dst) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return src;
  }

  for (int y = 0; y < dsth; y++) {
    int sy = scale_clamp(0, *srch - 1, y / scale);
    for (int x = 0; x < dstw; x++) {
      int sx = scale_clamp(0, *srcw - 1, x / scale);
      for (int c = 0; c < channels; c++) {
        const int srci = (sy * *srcw + sx);
        const int dsti = (y * dstw + x);
        dst[dsti * channels + c] = src[srci * channels + c];
      }
    }
  }

  *srcw = dstw;
  *srch = dsth;

  free(src);
  src = NULL;
  return dst;
}

// Non-Integer scaling
uint8_t *sfb_scale_nearest_centered_malloc(uint8_t *src, int *srcw, int *srch,
                                           const float scale, int channels) {
  if (!src || !srcw || !srch) {
    return NULL;
  }
  const int dstw = (int)(*srcw * scale);
  const int dsth = (int)(*srch * scale);

  uint8_t *dst = calloc(dstw * dsth * channels, sizeof(uint8_t));
  if (!dst) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return src;
  }

  for (int y = 0; y < dsth; y++) {
    const float fy = (y + 0.5f) * ((float)*srch / dsth) - 0.5f;
    int sy = scale_clamp(0, *srch - 1, roundf(fy));
    for (int x = 0; x < dstw; x++) {
      const float fx = (x + 0.5f) * ((float)*srcw / dstw) - 0.5f;
      int sx = scale_clamp(0, *srcw - 1, roundf(fx));
      for (int c = 0; c < channels; c++) {
        const int srci = (sy * *srcw + sx);
        const int dsti = (y * dstw + x);
        dst[dsti * channels + c] = src[srci * channels + c];
      }
    }
  }

  *srcw = dstw;
  *srch = dsth;

  free(src);
  src = NULL;
  return dst;
}
