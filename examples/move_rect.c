#include "../sfblib/sfb.h"

int main(void){
  sfb_framebuffer *fb = sfb_buffer_alloc(800, 600);
  sfb_fb_clear(fb, 0xFF00FF00);
 
  sfb_obj *rect = sfb_create_rect(200, 200, 150, 150, 0xFFFFFFFF);
  sfb_write_obj_rect(rect, fb);
  sfb_save_ppm(fb, "rect.ppm");
  sfb_fb_clear(fb, 0xFF00FF00);
  
  sfb_obj_move(200, 200, rect);
  sfb_write_obj_rect(rect, fb);
  sfb_save_ppm(fb, "rectmoved.ppm");

  sfb_free_framebuffer(fb);
  sfb_free_obj(rect);

  return 0;
}
