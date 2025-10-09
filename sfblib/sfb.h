#ifndef SFB_H
#define SFB_H

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { SFB_RECT = 0, SFB_TRIANGLE = 1, SFB_CIRCLE = 2 } obj_type;

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
} sfb_framebuffer;

#ifdef IMAGE_LOADING
sfb_obj sfb_image_load(const char *filepath);
void sfb_sprite_free(uint32_t *sprite);
#endif // IMAGE_LOADING

void sfb_free(uint32_t *data);
sfb_framebuffer sfb_buffer_alloc(int width, int height);
int sfb_buffer_realloc(uint32_t **data, int width, int height);
void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour);
int sfb_save_ppm(const sfb_framebuffer *const buffer, const char *path);
void sfb_write_rect(const sfb_obj *const obj, sfb_framebuffer *const buffer);
sfb_obj sfb_alloc_obj(const int type, int x, int y, int w, int h,
                      uint32_t colour);
void sfb_write_obj(const sfb_obj *const obj, sfb_framebuffer *const buffer);
void sfb_obj_move(const int x, const int y, sfb_obj *obj);

// Matrices
sfb_mat3x3 sfb_identity(void);
sfb_mat3x3 sfb_scale(sfb_mat3x3 mat, int w, int h);
sfb_mat3x3 sfb_translate(sfb_mat3x3 mat, int x, int y);
sfb_mat3x3 sfb_mmult(const sfb_mat3x3 *const a, const sfb_mat3x3 *const b);
#endif
