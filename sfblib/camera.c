#include "../include/sfb.h"

void sfb_camera_set_position_target(sfb_camera *const c, const sfb_obj *const target) {
  if (!c || !target) {
    return;
  }

  const int tx = target->mat.m2;
  const int ty = target->mat.m5;
  const int tw = target->w;
  const int th = target->h;

  c->x = (tx + tw / 2) - c->scrw / 2;
  c->y = (ty + th / 2) - c->scrh / 2;
}

void sfb_camera_set_position_fixed(sfb_camera *const c, const int x, const int y){
  c->x = x;
  c->y = y;
}

void sfb_camera_set_screen(sfb_camera* const c, const int w, const int h){
  c->scrw = w;
  c->scrh = h;
}
