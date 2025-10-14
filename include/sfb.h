#ifndef SFB_H
#define SFB_H

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct sfb_camera sfb_camera;
typedef struct sfb_framebuffer sfb_framebuffer;
typedef struct sfb_obj sfb_obj;

#define SFB_COL_CHANNELS 4
#define ALPHA 0
#define RED 1
#define GREEN 2
#define BLUE 3

typedef struct sfb_camera {
  // Camera x,y and screenw, screenh
  int x, y, scrw, scrh;
  void (*setpos_target)(sfb_camera *const, const sfb_obj *const);
  void (*setpos_fixed)(sfb_camera *const, int, int);
} sfb_camera;

typedef struct sfb_mat3x3 {
  //    w, ang, x
  //    ang, h, y
  float m0, m1, m2;
  float m3, m4, m5;
  float m6, m7, m8;
} sfb_mat3x3;

//TODO: add variables for format of pixels/channels(should always be 4 but a good idea to store that information)
typedef struct sfb_obj {
  sfb_mat3x3 mat;
  int w, h;
  uint32_t *pixels;
  //fns
  void (*(*move)(int, int, sfb_obj *))(sfb_camera *const , const sfb_obj *const);
} sfb_obj;

typedef struct sfb_framebuffer {
  int w, h;
  size_t size;
  uint32_t *data;
  //fns
  void (*clear)(sfb_framebuffer *const, uint32_t);
  void (*write_obj)(const sfb_obj *const, sfb_framebuffer *const, const sfb_camera *const);
  void (*write_rect)(int, int, int, int, uint32_t, sfb_framebuffer *const);
  void (*write_circle)(int, int, uint32_t, sfb_framebuffer *const, int);
  uint8_t (*uint8_buf)(const uint32_t *const);

} sfb_framebuffer;

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
sfb_obj *sfb_rect_from_sprite(int w, int h, const uint32_t *spr);
sfb_obj *sfb_create_rect(int x, int y, int w, int h, uint32_t colour);
sfb_obj *sfb_create_rects_n(int w, int h, uint32_t colour, size_t count);
sfb_camera *sfb_create_camera(int x, int y, int scrw, int scrh, const sfb_obj *const target);

// Scaling
uint32_t *sfb_scale_nearest_centered_malloc(uint32_t *src, int *srcw, int *srch, float scale);
uint32_t *sfb_scale_nearest_topleft_malloc(uint32_t *src, int *srcw, int *srch, int scale);
void sfb_scale_nearest_centered(uint32_t *src, int srcw, int srch, uint32_t *dst, int dstw, int dsth);
void sfb_scale_nearest_topleft(uint32_t *src, int srcw, int srch, uint32_t *dst, int dstw, int dsth, int scale);

// Camera
void sfb_camera_set_position_target(sfb_camera *const c, const sfb_obj *const target);
void sfb_camera_set_position_fixed(sfb_camera *const c, int x, int y);
void sfb_camera_set_screen(sfb_camera* const c, int w, int h);

// RGBA manipulation
uint32_t sfb_blend_pixel(uint32_t srcp, uint32_t dstp);
uint8_t sfb_mix_alpha(uint8_t src, uint8_t dst);
uint8_t sfb_mix_col(uint8_t src, uint8_t dst, uint8_t a);
void sfb_shift_bits_left_uint32(uint32_t *pixel, uint8_t col, int bit);
void sfb_shift_bits_right_uint8(uint8_t *col, uint32_t pixel, int bit);
uint32_t *sfb_argb8_to_argb32(const uint8_t pixels[], int w, int h, int channels);
uint32_t *sfb_rgba8_to_argb32(const uint8_t pixels[], int w, int h, int channels);
uint8_t *sfb_argb32_to_rgba8(const uint32_t pixels[], int w, int h, int channels);
uint32_t sfb_rgba32_to_argb32(uint32_t rgba);
uint32_t sfb_argb32_to_rgba32(uint32_t argb);

// Framebuffer writes
void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour);
void sfb_write_obj_rect(const sfb_obj *const obj, sfb_framebuffer *const buffer, const sfb_camera *const camera);
void sfb_put_pixel(int x, int y, uint32_t *const buf, int w, int h, uint32_t colour);
void sfb_write_rect_generic(int x0, int y0, int w0, int h0, uint32_t colour, sfb_framebuffer *const buffer);
void sfb_write_circle_generic(int xc, int yc, uint32_t colour, sfb_framebuffer *const buffer, int radius);

// Matrices
sfb_mat3x3 sfb_identity(void);
sfb_mat3x3 sfb_scale(sfb_mat3x3 mat, int w, int h);
sfb_mat3x3 sfb_translate(sfb_mat3x3 mat, int x, int y);
sfb_mat3x3 sfb_mmult(const sfb_mat3x3 *const a, const sfb_mat3x3 *const b);
void (*sfb_obj_move(int x, int y, sfb_obj *obj))(sfb_camera *const, const sfb_obj* const);

// Utils
int sfb_save_ppm(const sfb_framebuffer *const buffer, const char *path);

#endif
