#include "../include/sfb_camera.h"
#include "../include/sfb_framebuffer.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sfb_camera_set_position_target(sfb_camera *const c,
                                    const sfb_obj *const target) {
  if (!c || !target) {
    return;
  }

  const int tx = target->mat.m2;
  const int ty = target->mat.m5;
  const int tw = target->w;
  const int th = target->h;

  const int x = (tx + tw / 2) - c->scrw / 2;
  const int y = (ty + th / 2) - c->scrh / 2;

  c->mat.m2 = x;
  c->mat.m5 = y;
}

void sfb_camera_set_position_fixed(sfb_camera *const c, const int x,
                                   const int y) {
  c->mat.m2 = x;
  c->mat.m5 = y;
}

void sfb_camera_set_screen(sfb_camera *const c, const int w, const int h) {
  c->scrw = w;
  c->scrh = h;
}

void sfb_free_camera(sfb_camera *c) {
  if (c) {
    free(c);
  }
  c = NULL;
}

sfb_camera *sfb_create_camera(int x, int y, int scrw, int scrh,
                              const sfb_obj *const target) {
  if (!target) {
    fprintf(stderr, "Camera needs a target!\n");
    return NULL;
  }

  sfb_camera *c = calloc(1, sizeof(sfb_camera));
  if (!c) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  sfb_camera_set_screen(c, scrw, scrh);
  sfb_camera_set_position_target(c, target);
  c->setpos_target = sfb_camera_set_position_target;
  c->setpos_fixed = sfb_camera_set_position_fixed;

  return c;
}
