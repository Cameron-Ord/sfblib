#ifndef SFB_MAT_DEF_H
#define SFB_MAT_DEF_H
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
#endif
