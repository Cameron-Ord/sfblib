#include "../include/sfb_allocator.h"
#include "../include/sfb_flags.h"
#include "../include/sfb_framebuffer.h"
#include "../include/sfb_threads.h"

#include <stdio.h>
#include <string.h>

static int sfb_fb_init_threads(sfb_framebuffer *f);
static void sfb_fb_init(sfb_framebuffer_meta *m, const int width,
                        const int height, int flags, int channels);
static void sfb_fb_init(sfb_framebuffer_meta *m, const int width,
                        const int height, int flags, int channels) {
  m->channels = channels;
  m->flags = flags;
  m->cores = 0;
  m->w = width;
  m->h = height;
  m->size = width * height * channels;
}

static int sfb_fb_init_threads(sfb_framebuffer *f) {
  int cores = sfb_get_cores();
  sfb_thread_handle *handles = NULL;
  sfb_thread_ctx_renderer *ctx = NULL;

  handles = sfb_thread_handle_allocate(cores);
  if (handles) {
    ctx = sfb_spawn_threads(handles, cores);
  }

  if (!sfb_threads_validate(ctx, cores)) {
    sfb_free_handles(handles);
    f->meta.flags &= SFB_ENABLE_MULTITHREADED;
    return 0;
  }

  f->meta.cores = cores;
  f->r.thread_handles = handles;
  f->r.thread_render_data = ctx;
  return 1;
}

void sfb_render_init(sfb_framebuffer *f, int tile_size, int flags) {
  f->r.thread_render_data = NULL;
  f->r.thread_handles = NULL;
  f->r.flags = flags;
  f->r.tile_size = tile_size;
  f->r.world_height = 0;
  f->r.world_width = 0;

  if (flags & SFB_ENABLE_MULTITHREADED) {
    if (!sfb_fb_init_threads(f)) {
      fprintf(stderr, "Failed to spawn threads!\n");
    }
  }
}

// allocate and return a framebuffer of arbitrary render sizing. Pass OR'd
// together flags or 0
sfb_framebuffer *sfb_create_framebuffer(int width, int height, int tile_size,
                                        int fflags, int rflags) {
  int channels = SFB_RGB_CHANNELS;
  if (fflags & SFB_BLEND_ENABLED) {
    channels = SFB_RGBA_CHANNELS;
  }

  uint8_t *framebuffer = sfb_calloc(width * height * channels, sizeof(uint8_t));
  if (!framebuffer) {
    return NULL;
  }

  sfb_framebuffer *f = sfb_calloc(1, sizeof(sfb_framebuffer));
  if (!f) {
    return NULL;
  }
  f->pixels.data = framebuffer;
  f->pixels.flags = NULL;

  sfb_fb_init(&f->meta, width, height, fflags, channels);
  sfb_render_init(f, tile_size, rflags);
  return f;
}

// This lighting system IS NOT what I will be actually using im just trying to
// learn different sfb_create_framebuffer What I actually want is tile light
// propogation like Terraria
// sfb_light_source *sfb_create_light_source(const sfb_obj *const obj,
// const int channels, float size_coeff,
// float intensity, float dither_rate,
// sfb_colour c, int flags) {
// if (obj && !(obj->flags & SFB_LIGHT_SOURCE)) {
// sfb_light_source *light = malloc(sizeof(sfb_light_source));
// if (!light) {
// fprintf(stderr, "Could not create light source (calloc())-> %s\n",
// strerror(errno));
// return NULL;
//}
//
// light->w = obj->w * size_coeff;
// light->h = obj->h * size_coeff;
// const size_t heap_size = light->w * light->h * channels;
//
// if (intensity > 1.0f) {
// intensity = 1.0f;
//}
//
// if (dither_rate > 1.0f) {
// dither_rate = 1.0f;
//}
//
// light->lightmap.data = malloc(heap_size * sizeof(uint8_t));
// if (!light->lightmap.data) {
// fprintf(stderr, "Could not create light map (malloc())-> %s\n",
// strerror(errno));
// sfb_free_light_source(light);
// return NULL;
//}
//
// light->lightmap.flags = calloc(heap_size, sizeof(int));
// if (!light->lightmap.flags) {
// fprintf(stderr, "Could not create light map flags (calloc())-> %s\n",
// strerror(errno));
// sfb_free_light_source(light);
// return NULL;
//}
//
// light->channels = channels;
// light->flags = flags;
// light->mat = &obj->mat;
// light->size = heap_size;
//
// const uint8_t colours[SFB_MAX_CHANNELS] = {c.r, c.g, c.b, c.a};
//
// const int cx = light->w / 2;
// const int cy = light->h / 2;
// const float radius = sqrtf(cx * cx + cy * cy);
// const float inv = 1.0f / radius;
//
// uint8_t *data = light->lightmap.data;
// for (int y = 0; y < light->h; y++) {
// for (int x = 0; x < light->w; x++) {
// const float dx = (float)x - cx;
// const float dy = (float)y - cy;
// const float dist = sqrtf(dx * dx + dy * dy);
// const float fade = 1.0f - (dist * inv);
//
// int i = y * light->w + x;
// for (int c = 0; c < channels; c++) {
// const float pix_int = intensity * fade * dither_rate;
// data[i * channels + c] = sfb_col_exposure(colours[c], pix_int);
//}
//}
//}
//
// return light;
//}
//
// if (!obj) {
// fprintf(stderr, "Could not create light source-> Passed a NULL pointer \n");
//} else if (!(obj->flags & SFB_LIGHT_SOURCE)) {
// fprintf(stderr,
//"Could not create light source-> Object is not a light source\n");
//}
//
// return NULL;
//}
