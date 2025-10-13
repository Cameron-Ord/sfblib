#include "../sfblib/sfb.h"

int main(int argc, char **argv){
  sfb_framebuffer *f = sfb_buffer_alloc(800, 600);
  if(!f || !f->data){
    //free(), variables are checked before freeing
    sfb_free_framebuffer(f);
    return 0;
  }
  sfb_free_framebuffer(f);
  return 1;
}
