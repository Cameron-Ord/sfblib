#ifndef SFB_H
#define SFB_H

// Since this is just a lib that writes pixels to a framebuffer,
// any kind of projection is unnecessary since things are just painted in 2D at
// their sizes anyway
// Matrices are still useful for transformations and rotations, so I'm using
// them, and if I want to eventually do 3D then it's easy to add it, and then
// the choice of ortho, projection ,

// Functions for creating perspective matrices and ortho matrices exist for this
// reason, but they arent used at the moment

#include "sfb_camera.h"
#include "sfb_flags.h"
#include "sfb_framebuffer.h"
#include "sfb_rgba.h"
#include "sfb_scale.h"
#include "sfb_threads.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct sfb_camera3x3 sfb_camera3x3;
typedef struct sfb_framebuffer3x3 sfb_framebuffer3x3;
typedef struct sfb_obj3x3 sfb_obj3x3;

#ifdef IMAGE_LOADING
sfb_obj *sfb_image_load(const char *filepath, int flags);
#endif // IMAGE_LOADING

#ifdef IMAGE_WRITING
int sfb_image_write_png(int w, int h, const uint8_t *pixels,
                        const char *filepath);
#endif // IMAGE_WRITING

#endif // SFB_H
