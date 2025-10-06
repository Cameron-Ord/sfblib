#ifndef SFB_H
#define SFB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

void sfb_free(uint32_t *buffer);
uint32_t *sfb_buffer_alloc(int width, int height);
int sfb_buffer_realloc(uint32_t **buffer, int width, int height);
void sfb_fb_fill(uint32_t *buffer, int width, int height, uint32_t color);
int sfb_save_ppm(const uint32_t *const buffer, int width, int height, const char *path);
#endif
