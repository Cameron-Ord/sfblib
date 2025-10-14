#include "../include/sfb.h"
#include <math.h>

static int scale_clamp(int min, int max, int n){
  if(n < min){
    return min;
  } else if (n > max){
    return max;
  } else {
    return n;
  }
}

void sfb_scale_nearest_topleft(uint32_t *src, int srcw, int srch, uint32_t *dst, int dstw, int dsth, int scale){
  if(!src || !dst){
    return;
  }

  for(int y = 0; y < dsth; y++){
    int sy = scale_clamp(0, srch - 1, y / scale);
    for(int x = 0; x < dstw; x++){
      int sx = scale_clamp(0, srcw - 1, x / scale);
      dst[y * dstw + x] = src[sy * srcw + sx];
    }
  }
}

void sfb_scale_nearest_centered(uint32_t *src, int srcw, int srch, uint32_t *dst, int dstw, int dsth){
  if(!src || !dst){
    return;
  }

  for(int y = 0; y < dsth; y++){
    const float fy = (y + 0.5f) * ((float)srch / dsth) - 0.5f;
    int sy = scale_clamp(0, srch - 1, roundf(fy));
    for(int x = 0; x < dstw; x++){
      const float fx = (x + 0.5f) * ((float)srcw / dsth) - 0.5f;
      int sx = scale_clamp(0, srcw - 1, roundf(fx));
      dst[y * dstw + x] = src[sy * srcw + sx];
    }
  }
}

//Integer scaling
uint32_t *sfb_scale_nearest_topleft_malloc(uint32_t *src, int *srcw, int *srch, const int scale){
  if(!src || !srcw || !srch){
    return NULL;
  }
  const int dstw = *srcw * scale;
  const int dsth = *srch * scale;

  uint32_t *dst = malloc(dstw * dsth * sizeof(uint32_t));
  if(!dst){
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return src;
  }

  for(int y = 0; y < dsth; y++){
    int sy = scale_clamp(0, *srch - 1, y / scale);
    for(int x = 0; x < dstw; x++){
      int sx = scale_clamp(0, *srcw - 1, x / scale);
      dst[y * dstw + x] = src[sy * *srcw + sx];
    }
  }
  *srcw = dstw;
  *srch = dsth;

  free(src);
  return dst;
}

//Non-Integer scaling
uint32_t *sfb_scale_nearest_centered_malloc(uint32_t *src, int *srcw, int *srch, const float scale){
  if(!src || !srcw || !srch){
    return NULL;
  }
  const int dstw = (int)(*srcw * scale);
  const int dsth = (int)(*srch * scale);

  uint32_t *dst = malloc(dstw * dsth * sizeof(uint32_t));
  if(!dst){
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return src;
  }

  for(int y = 0; y < dsth; y++){
    const float fy = (y + 0.5f) * ((float)*srch / dsth) - 0.5f;
    int sy = scale_clamp(0, *srch - 1, roundf(fy));
    for(int x = 0; x < dstw; x++){
      const float fx = (x + 0.5f) * ((float)*srcw / dstw) - 0.5f;
      int sx = scale_clamp(0, *srcw - 1, roundf(fx));
      dst[y * dstw + x] = src[sy * *srcw + sx];
    }
  }

  *srcw = dstw;
  *srch = dsth;

  free(src);
  return dst;
}
