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

void sfb_scale_nearest_topleft(sfb_pixel *src, int srcw, int srch,
                               sfb_pixel *dst, int dstw, int dsth, int scale) {
  if (!src || !dst) {
    return;
  }

  for (int y = 0; y < dsth; y++) {
    int sy = scale_clamp(0, srch - 1, y / scale);
    for (int x = 0; x < dstw; x++) {
      int sx = scale_clamp(0, srcw - 1, x / scale);

      union pixel_data *src_pixel = &src[sy * srcw + sx].pixel;
      union pixel_data *dst_pixel = &src[y * dstw + x].pixel;
      *dst_pixel = *src_pixel;
    }
  }
}

void sfb_scale_nearest_centered(sfb_pixel *src, int srcw, int srch,
                                sfb_pixel *dst, int dstw, int dsth) {
  if (!src || !dst) {
    return;
  }

  for (int y = 0; y < dsth; y++) {
    const float fy = (y + 0.5f) * ((float)srch / dsth) - 0.5f;
    int sy = scale_clamp(0, srch - 1, roundf(fy));
    for (int x = 0; x < dstw; x++) {
      const float fx = (x + 0.5f) * ((float)srcw / dsth) - 0.5f;
      int sx = scale_clamp(0, srcw - 1, roundf(fx));

      union pixel_data *src_pixel = &src[sy * srcw + sx].pixel;
      union pixel_data *dst_pixel = &src[y * dstw + x].pixel;
      *dst_pixel = *src_pixel;
    }
  }
}

// Integer scaling
sfb_pixel *sfb_scale_nearest_topleft_malloc(sfb_pixel *src, int *srcw,
                                            int *srch, const int scale) {
  if (!src || !srcw || !srch) {
    return NULL;
  }
  const int dstw = *srcw * scale;
  const int dsth = *srch * scale;

  sfb_pixel *dst = calloc(dstw * dsth, sizeof(sfb_pixel));
  if (!dst) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return src;
  }

  for (int y = 0; y < dsth; y++) {
    int sy = scale_clamp(0, *srch - 1, y / scale);
    for (int x = 0; x < dstw; x++) {
      int sx = scale_clamp(0, *srcw - 1, x / scale);
      union pixel_data *dst_pixel = &dst[y * dstw + x].pixel;
      union pixel_data *src_pixel = &src[sy * *srcw + sx].pixel;
      *dst_pixel = *src_pixel;
    }
  }

  *srcw = dstw;
  *srch = dsth;

  free(src);
  src = NULL;
  return dst;
}

// Non-Integer scaling
sfb_pixel *sfb_scale_nearest_centered_malloc(sfb_pixel *src, int *srcw,
                                             int *srch, const float scale) {
  if (!src || !srcw || !srch) {
    return NULL;
  }
  const int dstw = (int)(*srcw * scale);
  const int dsth = (int)(*srch * scale);

  sfb_pixel *dst = calloc(dstw * dsth, sizeof(sfb_pixel));
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
      union pixel_data *dst_pixel = &dst[y * dstw + x].pixel;
      union pixel_data *src_pixel = &src[sy * *srcw + sx].pixel;
      *dst_pixel = *src_pixel;
    }
  }

  *srcw = dstw;
  *srch = dsth;

  free(src);
  src = NULL;
  return dst;
}
