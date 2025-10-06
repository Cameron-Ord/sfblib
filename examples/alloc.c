#include "sfb.h"

int main(int argc, char **argv){
  uint32_t *framebuffer = sfb_buffer_alloc(800, 600);
  if(!framebuffer){
    return 0;
  }
  
  if(sfb_buffer_realloc(&framebuffer, 1000, 1000) < 0){
    free(framebuffer);
    return 0;
  }
  return 1;
}
