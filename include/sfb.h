#ifndef SFB_H
#define SFB_H

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { SFB_RECT = 0, SFB_TRIANGLE = 1, SFB_CIRCLE = 2 } obj_type;

typedef struct sfb_camera sfb_camera;
typedef struct sfb_obj sfb_obj;

typedef struct sfb_camera {
  // The entity it is tracking.
  const sfb_obj *tracked;
  // Camera x,y and screenw, screenh
  int x, y, scrw, scrh;
} sfb_camera;

typedef struct sfb_mat3x3 {
  //    w, ang, x
  //    ang, h, y
  float m0, m1, m2;
  float m3, m4, m5;
  float m6, m7, m8;
} sfb_mat3x3;

typedef struct sfb_obj {
  sfb_mat3x3 mat;
  int w, h;
  int type;
  uint32_t *pixels;
} sfb_obj;

typedef struct sfb_framebuffer {
  int w, h;
  size_t size;
  uint32_t *data;
  // Read only pointer, can be swapped out with other const pointers
  const sfb_camera *camera;
} sfb_framebuffer;

typedef enum { ALPHA = 0, RED = 1, GREEN = 2, BLUE = 3, COL_COUNT = 4 } PACK;

#ifdef IMAGE_LOADING
sfb_obj *sfb_image_load(const char *filepath);
#endif // IMAGE_LOADING

#ifdef IMAGE_WRITING
int sfb_image_write_png(int w, int h, const uint32_t *pixels, int channels, const char *filepath);
#endif // IMAGE_WRITING

// Allocation
void sfb_free_framebuffer(sfb_framebuffer *f);
void sfb_free_obj(sfb_obj *o);
void sfb_free_camera(sfb_camera *c);
sfb_framebuffer *sfb_buffer_alloc(int width, int height);
sfb_obj *sfb_rect_from_sprite(const int w, const int h, uint32_t *spr);
sfb_obj *sfb_create_rect(int x, int y, int w, int h, uint32_t colour);
sfb_obj *sfb_create_rects_n(int w, int h, uint32_t colour, size_t count);
sfb_camera *sfb_create_camera(int x, int y, int scrw, int scrh,
                                     const sfb_obj *const tracked);

// Scaling
uint32_t *sfb_scale_nearest_centered_malloc(uint32_t *src, int *srcw, int *srch, float scale);
uint32_t *sfb_scale_nearest_topleft_malloc(uint32_t *src, int *srcw, int *srch, int scale);

// Camera
void sfb_fb_set_camera(sfb_framebuffer *const fb, const sfb_camera *c);
void sfb_camera_update_location(sfb_camera *const c);

// RGBA manipulation
void sfb_pack_argb32(uint8_t c[COL_COUNT], uint32_t pixel);
uint32_t sfb_unpack_argb32(const uint8_t c[COL_COUNT]);
uint32_t sfb_blend_pixel(uint32_t srcp, uint32_t dstp);
uint8_t sfb_mix_alpha(uint8_t src, uint8_t dst);
uint8_t sfb_mix_col(uint8_t src, uint8_t dst, uint8_t a);

// Framebuffer writes
void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour);
void sfb_write_obj_rect(const sfb_obj *const obj,
                        sfb_framebuffer *const buffer);
void (*sfb_obj_move(const int x, const int y, sfb_obj *o))(sfb_camera *);
void sfb_put_pixel(int x, int y, uint32_t *const buf, int w, int h,
                   uint32_t colour);
void sfb_write_rect_generic(int x0, int y0, int w0, int h0, uint32_t colour,
                            sfb_framebuffer *const buffer);
void sfb_write_circle_generic(int xc, int yc, uint32_t colour,
                              sfb_framebuffer *const buffer, int radius);

// Matrices
sfb_mat3x3 sfb_identity(void);
sfb_mat3x3 sfb_scale(sfb_mat3x3 mat, int w, int h);
sfb_mat3x3 sfb_translate(sfb_mat3x3 mat, int x, int y);
sfb_mat3x3 sfb_mmult(const sfb_mat3x3 *const a, const sfb_mat3x3 *const b);

// Utils
int sfb_save_ppm(const sfb_framebuffer *const buffer, const char *path);

#endif
