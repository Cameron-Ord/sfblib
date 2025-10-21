#ifndef SFB_CAMERA_H
#define SFB_CAMERA_H

#include "sfb_mat.h"
typedef struct sfb_obj sfb_obj;

typedef struct sfb_camera sfb_camera;

struct sfb_camera {
  int scrw, scrh;
  sfb_mat mat;
  void (*setpos_target)(sfb_camera *const, const sfb_obj *const);
  void (*setpos_fixed)(sfb_camera *const, int, int);
};

void sfb_camera_set_position_target(sfb_camera *const c,
                                       const sfb_obj *const target);
void sfb_camera_set_position_fixed(sfb_camera *const c, int x, int y);
void sfb_camera_set_screen(sfb_camera *const c, int w, int h);
void sfb_free_camera(sfb_camera *c);
sfb_camera *sfb_create_camera(int x, int y, int scrw, int scrh,
                                    const sfb_obj *const target);
#endif
