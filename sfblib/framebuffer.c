#include "../include/sfb_framebuffer.h"
#include "../include/sfb_threads.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sfb_free_obj(sfb_obj *o) {
  if (o && o->pixels) {
    free(o->pixels);
    free(o);
  }
  o = NULL;
}

void sfb_free_framebuffer(sfb_framebuffer *f) {
  if (f) {

    if (f->flags & SFB_ENABLE_MULTITHREADED) {
      fprintf(stdout, "Killing threads..\n");
      for (int i = 0; i < f->cores; i++) {
        sfb_thread_ctx_renderer *ctx = &f->thread_render_data[i];
        sfb_thread_handle *handle = &f->thread_handles[i];
        sfb_kill_thread(ctx, handle);
      }
      sfb_free_ctxs(f->thread_render_data);
      sfb_free_handles(f->thread_handles);
      fprintf(stdout, "..Done\n");
    }

    if (f->data) {
      free(f->data);
    }

    free(f);
  }
  f = NULL;
}

void sfb_free_light_source(sfb_light_source *light) {
  if (light) {
    free(light);
  }
  light = NULL;
}

// allocate and return a framebuffer of arbitrary render sizing. Pass OR'd
// together flags or 0
sfb_framebuffer *sfb_create_framebuffer(const int width, const int height,
                                        int flags) {
  uint32_t *framebuffer = calloc(width * height, sizeof(uint32_t));
  if (!framebuffer) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  sfb_framebuffer *fb_obj = calloc(1, sizeof(sfb_framebuffer));
  if (!fb_obj) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return NULL;
  }

  fb_obj->cores = 0;
  fb_obj->flags = flags;
  fb_obj->w = width;
  fb_obj->h = height;
  fb_obj->size = width * height * sizeof(uint32_t);
  fb_obj->data = framebuffer;

  fb_obj->clear = sfb_fb_clear;
  fb_obj->write_obj = sfb_write_obj_rect;
  fb_obj->write_rect = sfb_write_rect_generic;
  fb_obj->write_circle = sfb_write_circle_generic;

  if (fb_obj->flags & SFB_ENABLE_MULTITHREADED) {
    fb_obj->cores = sfb_get_cores();
    fb_obj->thread_handles = sfb_thread_handle_allocate(fb_obj->cores);

    if (fb_obj->thread_handles) {
      fb_obj->thread_render_data =
          sfb_spawn_threads(fb_obj->thread_handles, fb_obj->cores);
    }

    if (!fb_obj->thread_render_data) {
      sfb_free_handles(fb_obj->thread_handles);
      fb_obj->flags &= SFB_ENABLE_MULTITHREADED;
    }
  }

  return fb_obj;
}

void sfb_remove_light_source(sfb_obj *const obj, sfb_light_source *light) {
  if ((obj && obj->flags & SFB_LIGHT_SOURCE)) {
    obj->flags &= ~SFB_LIGHT_SOURCE;
    sfb_free_light_source(light);
  }
}

void sfb_assign_light(sfb_obj *const obj, sfb_light_source *light) {
  if ((obj && !(obj->flags & SFB_LIGHT_SOURCE)) && light) {
    obj->light = light;
    obj->flags |= SFB_LIGHT_SOURCE;
  }
}

sfb_light_source *sfb_create_light_source(const sfb_obj *const obj, int radius,
                                          float intensity, int flags) {
  if (obj && !(obj->flags & SFB_LIGHT_SOURCE)) {
    sfb_light_source *light = malloc(sizeof(sfb_light_source));
    if (!light) {
      fprintf(stderr, "Could not create light source (calloc())-> %s\n",
              strerror(errno));
      return NULL;
    }

    light->flags = flags;
    light->intensity = intensity;
    light->mat = &obj->mat;
    light->radius = radius;

    return light;
  }

  if (!obj) {
    fprintf(stderr, "Could not create light source-> Passed a NULL pointer \n");
  } else if (!(obj->flags & SFB_LIGHT_SOURCE)) {
    fprintf(stderr,
            "Could not create light source-> Object is not a light source\n");
  }

  return NULL;
}

sfb_obj *sfb_create_rect(int x, int y, int w, int h, uint32_t colour) {
  sfb_obj *obj = calloc(1, sizeof(sfb_obj));
  if (!obj) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return NULL;
  }

  obj->pixels = malloc(w * h * sizeof(uint32_t));
  if (!obj->pixels) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    free(obj);
    return NULL;
  }

  for (int i = 0; i < w * h; i++) {
    obj->pixels[i] = colour;
  }

  sfb_mat delta = sfb_identity();
  sfb_mat object_matrix = sfb_identity();
  delta = sfb_translate(delta, x, y);

  obj->flags = 0;
  obj->light = NULL;
  obj->mat = sfb_mmult(&delta, &object_matrix);
  obj->w = w;
  obj->h = h;
  obj->move = sfb_obj_move;
  obj->create_light_source = sfb_create_light_source;

  return obj;
}

sfb_obj *sfb_rect_from_sprite(const int w, const int h, const uint32_t *spr) {
  sfb_obj *obj = calloc(1, sizeof(sfb_obj));
  if (!obj) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return NULL;
  }

  obj->pixels = malloc(w * h * sizeof(uint32_t));
  if (!obj->pixels) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    free(obj);
    return NULL;
  }
  memcpy(obj->pixels, spr, w * h * sizeof(uint32_t));

  sfb_mat delta = sfb_identity();
  sfb_mat object_matrix = sfb_identity();
  delta = sfb_translate(delta, 0, 0);

  obj->flags = 0;
  obj->light = NULL;
  obj->mat = sfb_mmult(&delta, &object_matrix);
  obj->w = w;
  obj->h = h;
  obj->move = sfb_obj_move;
  obj->create_light_source = sfb_create_light_source;

  return obj;
}
