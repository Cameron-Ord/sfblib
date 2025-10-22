#ifndef SFB_FRAMEBUFFER_H
#define SFB_FRAMEBUFFER_H
#include "sfb_mat.h"
#include <stddef.h>
#include <stdint.h>

typedef struct sfb_camera sfb_camera;
typedef struct sfb_obj sfb_obj;
typedef struct sfb_framebuffer sfb_framebuffer;
typedef struct sfb_light_source sfb_light_source;
typedef struct sfb_pixel sfb_pixel;
typedef struct sfb_thread_ctx_renderer sfb_thread_ctx_renderer;
typedef struct sfb_thread_handle sfb_thread_handle;

// Light source flags
#define SFB_LIGHT_SOURCE (1 << 2)
// Framebuffer flags
#define SFB_BLEND_ENABLED (1 << 3)
#define SFB_ENABLE_MULTITHREADED (1 << 1)

// Just using an array of uint32_t works for now but if I want to have actual
// lighting logic and just in general, have good conditional logic, then it will
// need to be abstracted to this datatype with flags that define its
// characteristics
//
// IE blocks light or shiny etc
struct sfb_pixel {
  uint32_t pixel;
  int flags;
};

struct sfb_light_source {
  const sfb_mat *mat;
  // TODO: The mat is just a ptr to the obj's mat so it tracks location, so when
  // the object changes scale - apply the scaling logic to radius
  int radius;
  float intensity;
  int flags;
};

// Any object can be a light source. For an object to become a light source, it
// must be assigned as one after creation of the object using
// sfb_create_light_source and then assigned to the object. This will allocate a
// pointer and assign a bit flag. Any light source can also be removed through
// sfb_remove_light_source()
struct sfb_obj {
  sfb_mat mat;
  int w, h;
  uint32_t *pixels;
  int flags;
  sfb_light_source *light;
  // fns
  void (*(*move)(int, int, sfb_obj *))(sfb_camera *const, const sfb_obj *const);
  sfb_light_source *(*create_light_source)(const sfb_obj *const, int radius,
                                           float intensity, int flags);
};

struct sfb_framebuffer {
  int w, h;
  size_t size;
  uint32_t *data;
  int flags;

  // dft zero
  int cores;
  // NULL if flag is not set
  sfb_thread_ctx_renderer *thread_render_data;
  sfb_thread_handle *thread_handles;

  // fns
  void (*clear)(sfb_framebuffer *const, uint32_t);
  void (*write_obj)(const sfb_obj *const, sfb_framebuffer *const,
                    const sfb_camera *const);
  void (*write_rect)(int, int, int, int, uint32_t, sfb_framebuffer *const);
  void (*write_circle)(int, int, uint32_t, sfb_framebuffer *const, int);
  uint8_t (*uint8_buf)(const uint32_t *const);
};

// Framebuffer writes
void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour);
void sfb_write_obj_rect(const sfb_obj *const obj, sfb_framebuffer *const buffer,
                        const sfb_camera *const camera);
void sfb_put_pixel(int x, int y, uint32_t *const buf, int w, int h,
                   uint32_t colour, int flag);
void sfb_write_rect_generic(int x0, int y0, int w0, int h0, uint32_t colour,
                            sfb_framebuffer *const buffer);
void sfb_write_circle_generic(int xc, int yc, uint32_t colour,
                              sfb_framebuffer *const buffer, int radius);
sfb_framebuffer *sfb_create_framebuffer(int width, int height, int flags);
void sfb_free_framebuffer(sfb_framebuffer *f);
void sfb_free_obj(sfb_obj *o);
sfb_obj *sfb_rect_from_sprite(int w, int h, const uint32_t *spr);
sfb_obj *sfb_create_rect(int x, int y, int w, int h, uint32_t colour);
sfb_light_source *sfb_create_light_source(const sfb_obj *const obj, int radius,
                                          float intensity, int flags);
void sfb_assign_light(sfb_obj *const obj, sfb_light_source *light);
void sfb_remove_light_source(sfb_obj *const obj, sfb_light_source *light);
void sfb_free_light_source(sfb_light_source *light);
#endif
