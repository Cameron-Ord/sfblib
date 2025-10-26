#define IMAGE_WRITING
#include "../include/sfb.h"
#include "../include/sfb_flags.h"

#define WIDTH 400
#define HEIGHT 300
#define SIZE ((WIDTH / 8) * (HEIGHT / 6))

int main(void) {
  const int flags = SFB_BLEND_ENABLED | SFB_ENABLE_MULTITHREADED;
  sfb_framebuffer *buffer = sfb_create_framebuffer(WIDTH, HEIGHT, flags);
  sfb_obj *objs[(WIDTH / 8) * (HEIGHT / 6)];
  for (int i = 0; i < (WIDTH / 8) * (HEIGHT / 6); i++) {
    objs[i] = sfb_create_rect(i * 8, i * 6, 8, 6, 0xFF00FF00);
  }
  sfb_fb_clear(buffer, 0xFFFFFFFF);

  int h = 0;
  while (h < 100 * 100) {
    printf("%d\n", h);
    for (int i = 0; i < buffer->cores; i++) {
      sfb_thread_dequeue(&buffer->thread_render_data[i]);
      sfb_thread_queue_restack(&buffer->thread_render_data[i]);
    }

    int j = 0;
    while (j < 10 * 10) {
      sfb_write_obj_rect(objs[j % SIZE], buffer, NULL);
      j++;
    }

    for (int i = 0; i < buffer->cores; i++) {
      sfb_resume_thread(&buffer->thread_render_data[i]);
      sfb_thread_signal_resume(&buffer->thread_render_data[i]);
    }

    sfb_wait_threads(buffer->thread_render_data, buffer->cores);
    h++;
  }
  sfb_image_write_png(WIDTH, HEIGHT, buffer->data, 4, "thread.png");
  sfb_free_framebuffer(buffer);
  return 0;
}
