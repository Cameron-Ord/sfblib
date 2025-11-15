#ifndef SFB_FRAMEBUFFER_H
#define SFB_FRAMEBUFFER_H
#include "sfb_fb_def.h"
#include "sfb_mat_def.h"
#include <stddef.h>
#include <stdint.h>

void sfb_fb_clear(sfb_framebuffer *const buffer, sfb_colour clear_colour);
void sfb_write_obj_rect(const sfb_obj *const obj, sfb_framebuffer *const buffer,
                        const sfb_camera *const camera);
void sfb_put_pixel(uint8_t *const framebuffer, const uint8_t *colour,
                   uint8_t buffer_channels);
void sfb_write_rect_generic(int x0, int y0, int w0, int h0, sfb_colour colour,
                            sfb_framebuffer *const buffer);
void sfb_write_circle_generic(int xc, int yc, sfb_colour colour,
                              sfb_framebuffer *const buffer, int radius);
sfb_framebuffer *sfb_create_framebuffer(int width, int height, int flags);
void sfb_free_framebuffer(sfb_framebuffer *f);
void sfb_free_obj(sfb_obj *o);
sfb_obj *sfb_rect_from_sprite(int w, int h, uint8_t *spr, int channels);
sfb_obj *sfb_create_rect(int x, int y, int w, int h, sfb_colour colour,
                         int channels);
sfb_light_source *sfb_create_light_source(const sfb_obj *const obj,
                                          const int channels, float size_coeff,
                                          float intensity, float range,
                                          float dither_rate, sfb_colour c,
                                          int flags);
void sfb_assign_light(sfb_obj *const obj, sfb_light_source *light);
void sfb_remove_light_source(sfb_obj *const obj, sfb_light_source *light);
void sfb_free_light_source(sfb_light_source *light);
#endif
