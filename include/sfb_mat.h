#ifndef SFB_MAT_H
#define SFB_MAT_H
#include "sfb_mat_def.h"
sfb_mat sfb_identity(void);
sfb_mat sfb_scale(sfb_mat mat, int w, int h);
sfb_mat sfb_translate(sfb_mat mat, int x, int y);
sfb_mat sfb_mmult(const sfb_mat *const a, const sfb_mat *const b);
void (*sfb_obj_move(int x, int y, sfb_obj *obj))(sfb_camera *const,
                                                 const sfb_obj *const);

#endif
