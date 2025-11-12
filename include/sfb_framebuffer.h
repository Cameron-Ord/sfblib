#ifndef SFB_FRAMEBUFFER_H
#define SFB_FRAMEBUFFER_H
#include "sfb_fb_def.h"
#include "sfb_mat_def.h"
#include <stddef.h>
#include <stdint.h>

void sfb_fb_clear(sfb_framebuffer *const buffer, uint32_t clear_colour);
void sfb_write_obj_rect(const sfb_obj *const obj, sfb_framebuffer *const buffer,
                        const sfb_camera *const camera);
void sfb_put_pixel(int x, int y, sfb_pixel *const framebuffer, int w, int h,
                   uint32_t colour);
void sfb_write_rect_generic(int x0, int y0, int w0, int h0, uint32_t colour,
                            sfb_framebuffer *const buffer);
void sfb_write_circle_generic(int xc, int yc, uint32_t colour,
                              sfb_framebuffer *const buffer, int radius);
sfb_framebuffer *sfb_create_framebuffer(int width, int height, int flags);
void sfb_free_framebuffer(sfb_framebuffer *f);
void sfb_free_obj(sfb_obj *o);
sfb_obj *sfb_rect_from_sprite(int w, int h, sfb_pixel **spr);
sfb_obj *sfb_create_rect(int x, int y, int w, int h, uint32_t colour);
sfb_light_source *sfb_create_light_source(const sfb_obj *const obj, float size,
                                          float intensity, float range,
                                          uint32_t colour, int flags);
void sfb_assign_light(sfb_obj *const obj, sfb_light_source *light);
void sfb_remove_light_source(sfb_obj *const obj, sfb_light_source *light);
void sfb_free_light_source(sfb_light_source *light);
#endif
