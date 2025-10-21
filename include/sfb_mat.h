#ifndef SFB_MAT_H
#define SFB_MAT_H

typedef struct sfb_obj sfb_obj;
typedef struct sfb_camera sfb_camera;
typedef struct sfb_mat sfb_mat;

struct sfb_mat {
  //    w, ang, x
  //    ang, h, y
  float m0, m1, m2;
  float m3, m4, m5;
  float m6, m7, m8;
};

// Matrices
sfb_mat sfb_identity(void);
sfb_mat sfb_scale(sfb_mat mat, int w, int h);
sfb_mat sfb_translate(sfb_mat mat, int x, int y);
sfb_mat sfb_mmult(const sfb_mat *const a, const sfb_mat *const b);
void (*sfb_obj_move(int x, int y, sfb_obj *obj))(sfb_camera *const,
                                                       const sfb_obj *const);

#endif
