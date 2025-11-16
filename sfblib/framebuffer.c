#include "../include/sfb_flags.h"
#include "../include/sfb_framebuffer.h"
#include "../include/sfb_mat.h"
#include "../include/sfb_rgba.h"
#include "../include/sfb_threads.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int sfb_threads_validate(sfb_thread_ctx_renderer *ctxs, int cores) {
  if (!ctxs) {
    return 0;
  }

  for (int i = 0; i < cores; i++) {
    sfb_thread_ctx_renderer *ctx = &ctxs[i];
    if (!ctx->valid) {
      return 0;
    }
  }

  return 1;
}

void sfb_free_obj(sfb_obj *o) {
  if (o && o->pixels.data) {
    free(o->pixels.data);
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

    if (f->pixels.data) {
      free(f->pixels.data);
    }

    free(f);
  }
  f = NULL;
}

void sfb_free_light_source(sfb_light_source *light) {
  if (light) {
    if (light->lightmap.data) {
      free(light->lightmap.data);
    }
    free(light);
  }
  light = NULL;
}

// allocate and return a framebuffer of arbitrary render sizing. Pass OR'd
// together flags or 0
sfb_framebuffer *sfb_create_framebuffer(const int width, const int height,
                                        int flags) {
  int channels = SFB_RGB_CHANNELS;
  if (flags & SFB_BLEND_ENABLED) {
    channels = SFB_RGBA_CHANNELS;
  }

  uint8_t *framebuffer = calloc(width * height * channels, sizeof(uint8_t));
  if (!framebuffer) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  sfb_framebuffer *f = calloc(1, sizeof(sfb_framebuffer));
  if (!f) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return NULL;
  }

  f->cores = 0;
  f->channels = channels;
  f->flags = flags;
  f->w = width;
  f->h = height;
  f->size = width * height * channels;
  f->pixels.data = framebuffer;
  f->pixels.flags = NULL;

  f->clear = sfb_fb_clear;
  f->write_obj = sfb_write_obj_rect;
  f->write_rect = sfb_write_rect_generic;
  f->write_circle = sfb_write_circle_generic;

  if (f->flags & SFB_ENABLE_MULTITHREADED) {
    f->cores = sfb_get_cores();
    f->thread_handles = sfb_thread_handle_allocate(f->cores);

    if (f->thread_handles) {
      f->thread_render_data = sfb_spawn_threads(f->thread_handles, f->cores);
    }

    if (!sfb_threads_validate(f->thread_render_data, f->cores)) {
      sfb_free_handles(f->thread_handles);
      f->flags &= SFB_ENABLE_MULTITHREADED;
    }
  }

  return f;
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

sfb_light_source *sfb_create_light_source(const sfb_obj *const obj,
                                          const int channels, float size_coeff,
                                          float intensity, float dither_rate,
                                          sfb_colour c, int flags) {
  if (obj && !(obj->flags & SFB_LIGHT_SOURCE)) {
    sfb_light_source *light = malloc(sizeof(sfb_light_source));
    if (!light) {
      fprintf(stderr, "Could not create light source (calloc())-> %s\n",
              strerror(errno));
      return NULL;
    }

    light->w = obj->w * size_coeff;
    light->h = obj->h * size_coeff;
    const size_t heap_size = light->w * light->h * channels;

    if (intensity > 1.0f) {
      intensity = 1.0f;
    }

    if (dither_rate > 1.0f) {
      dither_rate = 1.0f;
    }

    light->lightmap.data = malloc(heap_size * sizeof(uint8_t));
    if (!light->lightmap.data) {
      fprintf(stderr, "Could not create light map (malloc())-> %s\n",
              strerror(errno));
      sfb_free_light_source(light);
      return NULL;
    }

    light->lightmap.flags = calloc(heap_size, sizeof(int));
    if (!light->lightmap.flags) {
      fprintf(stderr, "Could not create light map flags (calloc())-> %s\n",
              strerror(errno));
      sfb_free_light_source(light);
      return NULL;
    }

    light->channels = channels;
    light->flags = flags;
    light->mat = &obj->mat;
    light->size = heap_size;

    const uint8_t colours[SFB_MAX_CHANNELS] = {c.r, c.g, c.b, c.a};

    const int cx = light->w / 2;
    const int cy = light->h / 2;
    const float radius = sqrtf(cx * cx + cy * cy);
    const float inv = 1.0f / radius;

    uint8_t *data = light->lightmap.data;
    for (int y = 0; y < light->h; y++) {
      for (int x = 0; x < light->w; x++) {
        const float dx = (float)x - cx;
        const float dy = (float)y - cy;
        const float dist = sqrtf(dx * dx + dy * dy);
        const float fade = 1.0f - (dist * inv);

        int i = y * light->w + x;
        for (int c = 0; c < channels; c++) {
          const float pix_int = intensity * fade * dither_rate;
          data[i * channels + c] = sfb_col_exposure(colours[c], pix_int);
        }
      }
    }

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

sfb_obj *sfb_create_rect(int x, int y, int w, int h, sfb_colour c,
                         int channels) {
  sfb_obj *obj = calloc(1, sizeof(sfb_obj));
  if (!obj) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return NULL;
  }

  const size_t heap_size = w * h * channels;
  obj->pixels.data = malloc(heap_size * sizeof(uint8_t));
  if (!obj->pixels.data) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    sfb_free_obj(obj);
    return NULL;
  }

  obj->pixels.flags = calloc(heap_size, sizeof(int));
  if (!obj->pixels.flags) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    sfb_free_obj(obj);
    return NULL;
  }

  const uint8_t colours[SFB_MAX_CHANNELS] = {c.r, c.g, c.b, c.a};
  for (int i = 0; i < w * h; i++) {
    for (int c = 0; c < channels; c++) {
      obj->pixels.data[i * channels + c] = colours[c];
    }
  }

  sfb_mat delta = sfb_identity();
  sfb_mat object_matrix = sfb_identity();
  delta = sfb_translate(delta, x, y);

  obj->channels = channels;
  obj->flags = 0;
  obj->light = NULL;
  obj->mat = sfb_mmult(&delta, &object_matrix);
  obj->w = w;
  obj->h = h;
  obj->size = heap_size;
  obj->move = sfb_obj_move;
  obj->create_light_source = sfb_create_light_source;

  return obj;
}

sfb_obj *sfb_rect_from_sprite(const int w, const int h, uint8_t *spr,
                              int channels) {
  sfb_obj *obj = calloc(1, sizeof(sfb_obj));
  if (!obj) {
    fprintf(stderr, "!calloc()->%s\n", strerror(errno));
    return NULL;
  }

  const size_t heap_size = w * h * channels;
  obj->pixels.data = malloc(heap_size * sizeof(uint8_t));
  if (!obj->pixels.data) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    sfb_free_obj(obj);
    return NULL;
  }
  memcpy(obj->pixels.data, spr, w * h * channels * sizeof(uint8_t));

  obj->pixels.flags = calloc(heap_size, sizeof(int));
  if (!obj->pixels.flags) {
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    sfb_free_obj(obj);
    return NULL;
  }

  sfb_mat delta = sfb_identity();
  sfb_mat object_matrix = sfb_identity();
  delta = sfb_translate(delta, 0, 0);

  obj->channels = channels;
  obj->flags = 0;
  obj->light = NULL;
  obj->mat = sfb_mmult(&delta, &object_matrix);
  obj->w = w;
  obj->h = h;
  obj->size = heap_size;
  obj->move = sfb_obj_move;
  obj->create_light_source = sfb_create_light_source;

  return obj;
}
