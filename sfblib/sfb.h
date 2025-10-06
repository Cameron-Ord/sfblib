#ifndef SFB_H
#define SFB_H

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef IMAGE_LOADING
uint32_t *sfb_image_load(const char *filepath);
void sfb_image_free(uint32_t *sprite);
#endif // IMAGE_LOADING

void sfb_free(uint32_t *buffer);
uint32_t *sfb_buffer_alloc(int width, int height);
int sfb_buffer_realloc(uint32_t **buffer, int width, int height);
void sfb_fb_fill(uint32_t *buffer, int width, int height, uint32_t color);
int sfb_save_ppm(const uint32_t *const buffer, int width, int height,
                 const char *path);
void sfb_fb_draw_sprite(uint32_t *sprite, uint32_t *buffer, const int swidth,
                        const int sheight, const int bwidth, const int bheight,
                        const int x0, const int y0);
#endif
