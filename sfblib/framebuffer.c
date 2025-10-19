#include "../include/sfb_framebuffer.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sfb_framebuffer *sfb_buffer_alloc(const int width, const int height) {
  uint32_t *framebuffer = calloc(width * height, sizeof(uint32_t));
  if (!framebuffer) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  sfb_framebuffer *fb_obj = calloc(1, sizeof(sfb_framebuffer));
  if (!fb_obj) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return NULL;
  }

  fb_obj->w = width;
  fb_obj->h = height;
  fb_obj->size = width * height * sizeof(uint32_t);
  fb_obj->data = framebuffer;

  fb_obj->clear = sfb_fb_clear;
  fb_obj->write_obj = sfb_write_obj_rect3x3;
  fb_obj->write_rect = sfb_write_rect_generic3x3;
  fb_obj->write_circle = sfb_write_circle_generic3x3;

  return fb_obj;
}

void sfb_free_framebuffer(sfb_framebuffer *f) {
  if (f && f->data) {
    free(f->data);
    free(f);
  }
  f = NULL;
}

sfb_obj3x3 *sfb_create_rect3x3(int x, int y, int w, int h, uint32_t colour) {
  sfb_obj3x3 *obj = calloc(1, sizeof(sfb_obj3x3));
  if (!obj) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return NULL;
  }

  obj->pixels = malloc(w * h * sizeof(uint32_t));
  if (!obj->pixels) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    free(obj);
    return NULL;
  }

  for (int i = 0; i < w * h; i++) {
    obj->pixels[i] = colour;
  }

  sfb_mat3x3 delta = sfb_identity3x3();
  sfb_mat3x3 object_matrix = sfb_identity3x3();
  delta = sfb_translate3x3(delta, x, y);

  obj->mat = sfb_mmult3x3(&delta, &object_matrix);
  obj->w = w;
  obj->h = h;
  obj->move = sfb_obj3x3_move;

  return obj;
}

sfb_obj3x3 *sfb_rect_from_sprite3x3(const int w, const int h,
                                    const uint32_t *spr) {
  sfb_obj3x3 *obj = calloc(1, sizeof(sfb_obj3x3));
  if (!obj) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return NULL;
  }

  obj->pixels = malloc(w * h * sizeof(uint32_t));
  if (!obj->pixels) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    free(obj);
    return NULL;
  }
  memcpy(obj->pixels, spr, w * h * sizeof(uint32_t));

  sfb_mat3x3 delta = sfb_identity3x3();
  sfb_mat3x3 object_matrix = sfb_identity3x3();
  delta = sfb_translate3x3(delta, 0, 0);

  obj->mat = sfb_mmult3x3(&delta, &object_matrix);
  obj->w = w;
  obj->h = h;
  obj->move = sfb_obj3x3_move;

  return obj;
}

void sfb_free_obj3x3(sfb_obj3x3 *o) {
  if (o && o->pixels) {
    free(o->pixels);
    free(o);
  }
  o = NULL;
}
