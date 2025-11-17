#ifndef SFB_FRAMEBUFFER_H
#define SFB_FRAMEBUFFER_H
#include "sfb_fb_def.h"
#include <stddef.h>
#include <stdint.h>

sfb_framebuffer *sfb_create_framebuffer(int width, int height, int tile_size,
                                        int fflags, int rflags);
sfb_rect *sfb_rect_from_sprite(int w, int h, uint8_t *spr, int channels);
sfb_rect *sfb_create_rect(int x, int y, int w, int h, sfb_rgba colour,
                          int channels);
void sfb_render_init(sfb_framebuffer *f, int tile_size, int flags);

void sfb_write_rect(const sfb_rect *const obj, sfb_framebuffer *const buffer);
void sfb_put_pixel(uint8_t *const framebuffer, const uint8_t *colour,
                   uint8_t buffer_channels);
void sfb_fb_clear(sfb_framebuffer *const buffer, sfb_rgba clear_colour);
#endif
