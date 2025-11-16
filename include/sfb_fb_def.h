#ifndef SFB_FB_DEF_H
#define SFB_FB_DEF_H

#include "sfb_mat_def.h"
#include "sfb_pix_def.h"

#include <stddef.h>
#include <stdint.h>

typedef struct sfb_camera sfb_camera;
typedef struct sfb_obj sfb_obj;
typedef struct sfb_framebuffer sfb_framebuffer;
typedef struct sfb_light_source sfb_light_source;
typedef struct sfb_thread_ctx_renderer sfb_thread_ctx_renderer;
typedef struct sfb_thread_handle sfb_thread_handle;

struct sfb_light_source {
  const sfb_mat *mat;
  sfb_pixel_data lightmap;
  size_t size;
  int channels;
  int w, h;
  int flags;
};

struct sfb_obj {
  sfb_mat mat;
  int w, h;
  sfb_pixel_data pixels;
  size_t size;
  int channels;
  int flags;
  sfb_light_source *light;
  void (*(*move)(int, int, sfb_obj *))(sfb_camera *const, const sfb_obj *const);
  sfb_light_source *(*create_light_source)(const sfb_obj *const, int, float,
                                           float, float, sfb_colour, int);
};

struct sfb_framebuffer {
  int w, h;
  size_t size;
  uint8_t channels;
  sfb_pixel_data pixels;
  int flags;
  int cores;
  sfb_thread_ctx_renderer *thread_render_data;
  sfb_thread_handle *thread_handles;
  void (*clear)(sfb_framebuffer *const, sfb_colour);
  void (*write_obj)(const sfb_obj *const, sfb_framebuffer *const,
                    const sfb_camera *const);
  void (*write_rect)(int, int, int, int, sfb_colour, sfb_framebuffer *const);
  void (*write_circle)(int, int, sfb_colour, sfb_framebuffer *const, int);
  uint8_t (*uint8_buf)(const uint32_t *const);
};

#endif // SFB_FB_DEF
