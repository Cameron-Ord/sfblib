#include "../include/sfb.h"
#include <SDL2/SDL.h>

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    printf("SDL ERROR -> %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *w =
      SDL_CreateWindow("2D FB TEST", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 1000, 1000, SDL_WINDOW_HIDDEN);
  SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_SOFTWARE);

  SDL_Texture *t = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_STREAMING, 300, 300);

  sfb_framebuffer *fb = sfb_create_framebuffer(
      300, 300, SFB_ENABLE_MULTITHREADED | SFB_BLEND_ENABLED);
  sfb_obj *obj = sfb_create_rect(0, 0, 25, 25, (sfb_colour){255, 255, 255, 255},
                                 fb->channels);

  SDL_ShowWindow(w);
  int running = 1;
  while (running) {
    sfb_fb_clear(fb, (sfb_colour){0, 0, 0, 255});

    for (int i = 0; i < fb->cores; i++) {
      sfb_thread_dequeue(&fb->thread_render_data[i]);
      sfb_thread_queue_restack(&fb->thread_render_data[i]);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {

      case SDL_QUIT: {
        running = 0;
      } break;

      case SDL_KEYDOWN: {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT: {
          sfb_obj_move(-5, 0, obj)(NULL, obj);
        } break;
        case SDLK_RIGHT: {
          sfb_obj_move(5, 0, obj)(NULL, obj);
        } break;

        case SDLK_DOWN: {
          sfb_obj_move(0, 5, obj)(NULL, obj);
        } break;

        case SDLK_UP: {
          sfb_obj_move(0, -5, obj)(NULL, obj);
        } break;
        }
      } break;
      }
    }

    sfb_write_obj_rect(obj, fb, NULL);
    for (int i = 0; i < fb->cores; i++) {
      sfb_resume_thread(&fb->thread_render_data[i]);
      sfb_thread_signal_resume(&fb->thread_render_data[i]);
    }
    sfb_wait_threads(fb->thread_render_data, fb->cores);

    // Using SDL_UpdateTexture here, but this is slow and the real way to do
    // this is to use SDL_LockTexture()
    if (SDL_UpdateTexture(t, NULL, fb->pixels.data, sizeof(uint32_t) * 300) <
        0) {
      printf("SDLERR->%s\n", SDL_GetError());
    }
    if (SDL_RenderCopy(r, t, NULL, NULL) < 0) {
      printf("SDLERR->%s\n", SDL_GetError());
    }

    SDL_RenderPresent(r);
    SDL_Delay(16);
  }

  sfb_free_framebuffer(fb);
  sfb_free_obj(obj);

  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(w);
  SDL_Quit();
  return 0;
}
