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

  SDL_Texture *t = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888,
                                     SDL_TEXTUREACCESS_STREAMING, 300, 300);

  sfb_framebuffer *fb = sfb_buffer_alloc(300, 300);
  sfb_obj *obj = sfb_create_rect(0, 0, 25, 25, 0xFFFF00FF);

  SDL_ShowWindow(w);
  int running = 1;
  while (running) {
    sfb_fb_clear(fb, 0xFF00FF00);

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {

      case SDL_QUIT: {
        running = 0;
      } break;

      case SDL_KEYDOWN: {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT: {
          sfb_obj_move(-5, 0, obj);
        } break;
        case SDLK_RIGHT: {
          sfb_obj_move(5, 0, obj);
        } break;

        case SDLK_DOWN: {
          sfb_obj_move(0, 5, obj);
        } break;

        case SDLK_UP: {
          sfb_obj_move(0, -5, obj);
        } break;
        }
      } break;
      }
    }

    sfb_write_obj_rect(obj, fb);

    if (SDL_UpdateTexture(t, NULL, fb->data, sizeof(uint32_t) * 300) < 0) {
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
