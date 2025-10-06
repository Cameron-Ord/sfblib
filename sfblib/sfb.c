#include "sfb.h"

//TODO: log file
//Return a mutable  pointer for an allocated buffer (Allows multiple different frame buffers and also allow users to choose what resolution they want to render at)
uint32_t *sfb_buffer_alloc(const int width, const int height){
  uint32_t *framebuffer = malloc(width * height * sizeof(uint32_t));
  if(!framebuffer){
    fprintf(stderr, "!malloc()->%s\n", strerror(errno));
    return NULL;
  }

  return framebuffer;
}

int sfb_buffer_realloc(uint32_t **buffer, const int width, const int height){
  uint32_t *tmp = realloc(*buffer, width * height *sizeof(uint32_t));
  if(!tmp){
    fprintf(stderr, "!realloc()->%s\n", strerror(errno));
    return -1;
  }
  *buffer = tmp;
  return 1;
}

void sfb_free(uint32_t *buffer){
  if(buffer){
    free(buffer);
  }
}

void sfb_fb_fill(uint32_t *const buffer, const int width, const int height, const uint32_t color){
  for(int i = 0; i < width * height; i++){
    buffer[i] = color;
  }
}

//Draw a sprite at a location in the framebuffer.
//The sprite needs to be in ARGB format and the user will have to use a tool to load an image and create a sprite[width*height] array from such
//The array is 1D so the data will need to be filled correctly to properly display the sprite
//I could use stbi image to streamline this and write that part myself though.
void sfb_fb_draw_sprite(uint32_t *sprite, uint32_t *buffer, const int swidth, const int sheight, const int bwidth, const int bheight, const int x0, const int y0){

}

int sfb_save_ppm(const uint32_t *const buffer, const int width, const int height, const char *path){
  FILE *f = NULL;
  if(!(f = fopen(path, "wb"))){
    fprintf(stderr, "!fopen()->%s\n", strerror(errno));
    return -1;
  }

  fprintf(f, "P6\n%d %d 255\n", width, height);
  if(ferror(f)){
    fprintf(stderr, "!fprintf()->%s\n", strerror(errno));
    fclose(f);
    return -1;
  }

  for(int i = 0; i < width * height; i++){
    const uint32_t pixel = buffer[i];
    //SFB FORMAT: ARGB ; PPM FORMAT ABGR
    uint8_t bytes[3] = { 
      (pixel >> (0))&0xFF,
      (pixel >> (8))&0xFF,
      (pixel >> (16))&0xFF,
    };

    fwrite(bytes, sizeof(bytes), 1, f);
    if(ferror(f)){
      fprintf(stderr, "!fwrite()->%s\n", strerror(errno));
      fclose(f);
      return -1;
    }
  }
  return 1;
}
