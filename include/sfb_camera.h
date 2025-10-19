#ifndef SFB_CAMERA_H
#define SFB_CAMERA_H

#include "sfb_mat.h"
typedef struct sfb_obj3x3 sfb_obj3x3;
typedef struct sfb_obj4x4 sfb_obj4x4;

typedef struct sfb_camera3x3 sfb_camera3x3;
typedef struct sfb_camera4x4 sfb_camera4x4;

struct sfb_camera4x4 {
  int scrw, scrh;
  sfb_mat4x4 mat;
  void (*setpos_target)(sfb_camera4x4 *const, const sfb_obj4x4 *const);
  void (*setpos_fixed)(sfb_camera4x4 *const, int, int);
};

struct sfb_camera3x3 {
  int scrw, scrh;
  sfb_mat3x3 mat;
  void (*setpos_target)(sfb_camera3x3 *const, const sfb_obj3x3 *const);
  void (*setpos_fixed)(sfb_camera3x3 *const, int, int);
};

void sfb_camera3x3_set_position_target(sfb_camera3x3 *const c,
                                       const sfb_obj3x3 *const target);
void sfb_camera3x3_set_position_fixed(sfb_camera3x3 *const c, int x, int y);
void sfb_camera3x3_set_screen(sfb_camera3x3 *const c, int w, int h);
void sfb_free_camera3x3(sfb_camera3x3 *c);
sfb_camera3x3 *sfb_create_camera3x3(int x, int y, int scrw, int scrh,
                                    const sfb_obj3x3 *const target);
#endif
