#include "../include/sfb.h"

sfb_camera *sfb_create_camera(int x, int y, int scrw, int scrh,
                                     const sfb_obj *const tracked) {
  if (!tracked) {
    fprintf(stderr, "Must provide a valid tracked object\n");
    return NULL;
  }

  sfb_camera *c = calloc(1, sizeof(sfb_camera));
  if (!c) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  c->scrw = scrw, c->scrh = scrh;
  c->tracked = tracked;
  sfb_camera_update_location(c);

  return c;
}

void sfb_camera_update_location(sfb_camera *const c) {
  if (!c || !c->tracked) {
    return;
  }

  const int x = c->tracked->mat.m2;
  const int y = c->tracked->mat.m5;
  const int ew = c->tracked->w;
  const int eh = c->tracked->h;

  c->x = (x + ew / 2) - c->scrw / 2;
  c->y = (y + eh / 2) - c->scrh / 2;
}

void sfb_fb_set_camera(sfb_framebuffer *const fb, const sfb_camera *c) {
  fb->camera = c;
}
