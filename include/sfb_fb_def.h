#ifndef SFB_FB_DEF_H
#define SFB_FB_DEF_H

#include "sfb_pix_def.h"

#include <stddef.h>
#include <stdint.h>

typedef struct sfb_camera sfb_camera;
typedef struct sfb_rect sfb_rect;
typedef struct sfb_framebuffer sfb_framebuffer;
typedef struct sfb_light_source sfb_light_source;
typedef struct sfb_thread_ctx_renderer sfb_thread_ctx_renderer;
typedef struct sfb_thread_handle sfb_thread_handle;
typedef struct sfb_framebuffer_meta sfb_framebuffer_meta;
typedef struct sfb_renderer sfb_renderer;

struct sfb_camera {
  int scrw, scrh;
  int x, y;
};

struct sfb_rect {
  int x, y, w, h;
  sfb_pixel_data pixels;
  size_t size;
  int channels;
};

struct sfb_renderer {
  int tile_size;
  int world_width, world_height;
  sfb_camera cam;
  int flags;

  sfb_thread_ctx_renderer *thread_render_data;
  sfb_thread_handle *thread_handles;
};

struct sfb_framebuffer_meta {
  int w, h;
  size_t size;
  uint8_t channels;
  int flags;
  int cores;
};

struct sfb_framebuffer {
  sfb_pixel_data pixels;
  sfb_renderer r;
  sfb_framebuffer_meta meta;
};

#endif // SFB_FB_DEF
