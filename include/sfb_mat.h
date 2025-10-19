#ifndef SFB_MAT_H
#define SFB_MAT_H

typedef struct sfb_obj3x3 sfb_obj3x3;
typedef struct sfb_camera3x3 sfb_camera3x3;
typedef struct sfb_obj4x4 sfb_obj4x4;
typedef struct sfb_camera4x4 sfb_camera4x4;

typedef struct sfb_mat3x3 sfb_mat3x3;
typedef struct sfb_mat4x4 sfb_mat4x4;

struct sfb_mat3x3 {
  //    w, ang, x
  //    ang, h, y
  float m0, m1, m2;
  float m3, m4, m5;
  float m6, m7, m8;
};

struct sfb_mat4x4 {
  float m0, m1, m2, m3;
  float m4, m5, m6, m7;
  float m8, m9, m10, m11;
  float m12, m13, m14, m15;
};

// Matrices
sfb_mat3x3 sfb_identity3x3(void);
sfb_mat3x3 sfb_scale3x3(sfb_mat3x3 mat, int w, int h);
sfb_mat3x3 sfb_translate3x3(sfb_mat3x3 mat, int x, int y);
sfb_mat3x3 sfb_mmult3x3(const sfb_mat3x3 *const a, const sfb_mat3x3 *const b);
void (*sfb_obj3x3_move(int x, int y, sfb_obj3x3 *obj))(sfb_camera3x3 *const,
                                                       const sfb_obj3x3 *const);

#endif
