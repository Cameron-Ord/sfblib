#include "../include/sfb.h"

int main(void) {
  sfb_framebuffer *fb = sfb_buffer_alloc(300, 300);
  sfb_fb_clear(fb, 0xFF000000);
  sfb_obj *o = sfb_create_rect(0, 0, 25, 25, 0xFFFFFFFF);
  sfb_camera_entity *c = sfb_create_camera(0, 0, fb->w, fb->h, o);
  sfb_set_camera_entity(fb, &c);
  sfb_write_obj_rect(o, fb);
  sfb_save_ppm(fb, "camera.ppm");
  return 0;
}
