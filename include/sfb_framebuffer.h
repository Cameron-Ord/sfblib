#ifndef SFB_FRAMEBUFFER_H
#define SFB_FRAMEBUFFER_H
#include "sfb_mat.h"
#include <stddef.h>
#include <stdint.h>

typedef struct sfb_camera4x4 sfb_camera4x4;
typedef struct sfb_camera3x3 sfb_camera3x3;

typedef struct sfb_obj3x3 sfb_obj3x3;
typedef struct sfb_obj4x4 sfb_obj4x4;

typedef struct sfb_framebuffer sfb_framebuffer;

struct sfb_obj4x4 {
  sfb_mat4x4 mat;
  int w, h;
  uint32_t *pixels;
  // fns
  void (*(*move)(int, int, sfb_obj4x4 *))(sfb_camera4x4 *const,
                                          const sfb_obj4x4 *const);
};

struct sfb_obj3x3 {
  sfb_mat3x3 mat;
  int w, h;
  uint32_t *pixels;
  // fns
  void (*(*move)(int, int, sfb_obj3x3 *))(sfb_camera3x3 *const,
                                          const sfb_obj3x3 *const);
};

struct sfb_framebuffer {
  int w, h;
  size_t size;
  uint32_t *data;
  // fns
  void (*clear)(sfb_framebuffer *const, uint32_t);
  void (*write_obj)(const sfb_obj3x3 *const, sfb_framebuffer *const,
                    const sfb_camera3x3 *const);
  void (*write_rect)(int, int, int, int, uint32_t, sfb_framebuffer *const);
  void (*write_circle)(int, int, uint32_t, sfb_framebuffer *const, int);
  uint8_t (*uint8_buf)(const uint32_t *const);
};

// Framebuffer writes
void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour);
void sfb_write_obj_rect3x3(const sfb_obj3x3 *const obj,
                           sfb_framebuffer *const buffer,
                           const sfb_camera3x3 *const camera);
void sfb_put_pixel3x3(int x, int y, uint32_t *const buf, int w, int h,
                      uint32_t colour);
void sfb_write_rect_generic3x3(int x0, int y0, int w0, int h0, uint32_t colour,
                               sfb_framebuffer *const buffer);
void sfb_write_circle_generic3x3(int xc, int yc, uint32_t colour,
                                 sfb_framebuffer *const buffer, int radius);
sfb_framebuffer *sfb_buffer_alloc(int width, int height);
void sfb_free_framebuffer(sfb_framebuffer *f);
void sfb_free_obj3x3(sfb_obj3x3 *o);
sfb_obj3x3 *sfb_rect_from_sprite3x3(int w, int h, const uint32_t *spr);
sfb_obj3x3 *sfb_create_rect3x3(int x, int y, int w, int h, uint32_t colour);
#endif
