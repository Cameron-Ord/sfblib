#include "sfb.h"

//TODO: log file
//Return a mutable  pointer for an allocated buffer (Allows multiple different frame buffers and also allow users to choose what resolution they want to render at)
sfb_framebuffer sfb_buffer_alloc(const int width, const int height){
  uint32_t *framebuffer = calloc(width * height, sizeof(uint32_t));
  if(!framebuffer){
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return (sfb_framebuffer){0};
  }

  return (sfb_framebuffer){width, height, width * height * sizeof(uint32_t), framebuffer};
}

int sfb_buffer_realloc(uint32_t **buffer, const int width, const int height){
  uint32_t *tmp = realloc(*buffer, width * height *sizeof(uint32_t));
  if(!tmp){
    fprintf(stderr, "!realloc()->%s\n", strerror(errno));
    return -1;
  }
  *buffer = tmp;
  return 1;
}

void sfb_free(uint32_t *buffer){
  if(buffer){
    free(buffer);
  }
}

void sfb_fb_clear(sfb_framebuffer *const buffer,  uint32_t clear_colour){
  for(int i = 0; i < buffer->w * buffer->h; i++){
    buffer->data[i] = clear_colour;
  }
}

sfb_mat3x3 sfb_identity(void){
  return (sfb_mat3x3){ 
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
  };
}

sfb_mat3x3 sfb_translate(sfb_mat3x3 mat, int x, int y){
  mat.m2 = x;
  mat.m5 = y;
  return mat;
}

sfb_mat3x3 sfb_scale(sfb_mat3x3 mat, int w, int h){
  mat.m0 = w;
  mat.m4 = h;
  return mat;
}

sfb_mat3x3 sfb_mmult(const sfb_mat3x3 *const a, const sfb_mat3x3 *const b){
  sfb_mat3x3 r;

  r.m0 = a->m0*b->m0 + a->m1*b->m3 + a->m2*b->m6;
  r.m1 = a->m0*b->m1 + a->m1*b->m4 + a->m2*b->m7;
  r.m2 = a->m0*b->m2 + a->m1*b->m5 + a->m2*b->m8;

  r.m3 = a->m3*b->m0 + a->m4*b->m3 + a->m5*b->m6;
  r.m4 = a->m3*b->m1 + a->m4*b->m4 + a->m5*b->m7;
  r.m5 = a->m3*b->m2 + a->m4*b->m5 + a->m5*b->m8;
  
  r.m6 = a->m6*b->m0 + a->m7*b->m3 + a->m8*b->m6;
  r.m7 = a->m6*b->m1 + a->m7*b->m4 + a->m8*b->m7;
  r.m8 = a->m6*b->m2 + a->m7*b->m5 + a->m8*b->m8;

  return r;
}

void sfb_obj_move(const int x, const int y, sfb_obj *obj){
  sfb_mat3x3 delta = sfb_translate(sfb_identity(), x, y);
  obj->mat = sfb_mmult(&delta, &obj->mat);
}

sfb_obj sfb_alloc_obj(const int type, int x, int y, int w, int h, uint32_t colour){
  switch(type){
    default:
      return (sfb_obj){0};
    case RECT:{
      uint32_t *pixels = calloc(w * h, sizeof(uint32_t));
      for(int i = 0; i < w * h; i++){
        pixels[i] = colour;
      }

      sfb_mat3x3 delta = sfb_identity();
      sfb_mat3x3 obj = sfb_identity();

      delta = sfb_scale(delta, 1.0f, 1.0f);
      delta = sfb_translate(delta, x, y);

      return (sfb_obj){sfb_mmult(&delta, &obj), w, h, RECT, pixels};
      } break;
    break;
  }
}

void sfb_write_rect(const sfb_obj *const obj, sfb_framebuffer *const buffer){
  const int y0 = obj->mat.m2;
  const int x0 = obj->mat.m5;
  //TODO: nearest neighbor scaling so that the scale coeffs in the mat actually do something
  //Otherwise I can only draw stuff at their exact pixel sizing
  for(int i = 0, y = y0; y < buffer->h && i < obj->h; i++, y++){
    for(int j = 0, x = x0; x < buffer->w && j < obj->w; j++, x++){
      const uint32_t obj_pixel = obj->pixels[i * obj->w + j];
      buffer->data[y * buffer->w + x] = obj_pixel;
    }
  }
}


void sfb_write_obj(const sfb_obj *const obj, sfb_framebuffer *const buffer){
  switch(obj->type){
    default: 
      return;

    case RECT:
      sfb_write_rect(obj, buffer);
      break;
  }
}

int sfb_save_ppm(const sfb_framebuffer *const buffer, const char *path){
  FILE *f = NULL;
  if(!(f = fopen(path, "wb"))){
    fprintf(stderr, "!fopen()->%s\n", strerror(errno));
    return -1;
  }

  fprintf(f, "P6\n%d %d 255\n", buffer->w, buffer->h);
  if(ferror(f)){
    fprintf(stderr, "!fprintf()->%s\n", strerror(errno));
    fclose(f);
    return -1;
  }

  for(int i = 0; i < buffer->w * buffer->h; i++){
    const uint32_t pixel = buffer->data[i];
    //SFB FORMAT: ARGB ; PPM FORMAT ABGR
    uint8_t bytes[3] = { 
      (pixel >> (0))&0xFF,
      (pixel >> (8))&0xFF,
      (pixel >> (16))&0xFF,
    };

    fwrite(bytes, sizeof(bytes), 1, f);
    if(ferror(f)){
      fprintf(stderr, "!fwrite()->%s\n", strerror(errno));
      fclose(f);
      return -1;
    }
  }
  return 1;
}
