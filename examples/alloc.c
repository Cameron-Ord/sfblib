#include "../sfblib/sfb.h"

int main(int argc, char **argv){
  sfb_framebuffer fb = sfb_buffer_alloc(800, 600);
  if(!fb.data){
    return 0;
  }
  
  if(sfb_buffer_realloc(&fb.data, 1000, 1000) < 0){
    free(fb.data);
    return 0;
  }
  free(fb.data);
  
  fb = sfb_buffer_alloc(200, 200);
  if(!fb.data){
    return 0;
  }
  free(fb.data);
  return 1;
}
