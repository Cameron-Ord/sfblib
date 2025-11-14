#include "../include/sfb_camera.h"
#include "../include/sfb_fb_def.h"

sfb_mat sfb_identity(void) {
  return (sfb_mat){1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

sfb_mat sfb_translate(sfb_mat mat, int x, int y) {
  mat.m2 = x;
  mat.m5 = y;
  return mat;
}

sfb_mat sfb_scale(sfb_mat mat, int w, int h) {
  mat.m0 = w;
  mat.m4 = h;
  return mat;
}

sfb_mat sfb_mmult(const sfb_mat *const a, const sfb_mat *const b) {
  sfb_mat r;

  r.m0 = a->m0 * b->m0 + a->m1 * b->m3 + a->m2 * b->m6;
  r.m1 = a->m0 * b->m1 + a->m1 * b->m4 + a->m2 * b->m7;
  r.m2 = a->m0 * b->m2 + a->m1 * b->m5 + a->m2 * b->m8;

  r.m3 = a->m3 * b->m0 + a->m4 * b->m3 + a->m5 * b->m6;
  r.m4 = a->m3 * b->m1 + a->m4 * b->m4 + a->m5 * b->m7;
  r.m5 = a->m3 * b->m2 + a->m4 * b->m5 + a->m5 * b->m8;

  r.m6 = a->m6 * b->m0 + a->m7 * b->m3 + a->m8 * b->m6;
  r.m7 = a->m6 * b->m1 + a->m7 * b->m4 + a->m8 * b->m7;
  r.m8 = a->m6 * b->m2 + a->m7 * b->m5 + a->m8 * b->m8;

  return r;
}

void (*sfb_obj_move(const int x, const int y,
                    sfb_obj *o))(sfb_camera *const, const sfb_obj *const) {
  sfb_mat delta = sfb_translate(sfb_identity(), x, y);
  o->mat = sfb_mmult(&delta, &o->mat);
  return sfb_camera_set_position_target;
}
