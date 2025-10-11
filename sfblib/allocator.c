#include "../include/sfb.h"

sfb_framebuffer *sfb_buffer_alloc(const int width, const int height) {
  uint32_t *framebuffer = calloc(width * height, sizeof(uint32_t));
  if (!framebuffer) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  sfb_framebuffer *fb_obj = calloc(1, sizeof(sfb_framebuffer));

  fb_obj->w = width;
  fb_obj->h = height;
  fb_obj->size = width * height * sizeof(uint32_t);
  fb_obj->data = framebuffer;
  fb_obj->c = NULL;

  return fb_obj;
}

sfb_obj *sfb_create_rect(int x, int y, int w, int h, uint32_t colour) {
  sfb_obj *obj = calloc(1, sizeof(sfb_obj));
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
  memset(obj->pixels, colour, w * h * sizeof(uint32_t));

  sfb_mat3x3 delta = sfb_identity();
  sfb_mat3x3 object_matrix = sfb_identity();
  delta = sfb_translate(delta, x, y);

  obj->mat = sfb_mmult(&delta, &object_matrix);
  obj->w = w;
  obj->h = h;
  obj->type = SFB_RECT;

  return obj;
}

sfb_obj *sfb_rect_from_sprite(const int w, const int h, uint32_t *spr) {
  sfb_obj *obj = calloc(1, sizeof(sfb_obj));
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

  sfb_mat3x3 delta = sfb_identity();
  sfb_mat3x3 object_matrix = sfb_identity();
  delta = sfb_translate(delta, 0, 0);

  obj->mat = sfb_mmult(&delta, &object_matrix);
  obj->w = w;
  obj->h = h;
  obj->type = SFB_RECT;

  return obj;
}

void sfb_free_framebuffer(sfb_framebuffer *f) {
  if (f && f->data) {
    free(f->data);
    free(f);
  }
  f = NULL;
}

void sfb_free_obj(sfb_obj *o) {
  if (o && o->pixels) {
    free(o->pixels);
    free(o);
  }
  o = NULL;
}
