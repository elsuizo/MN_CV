/* -------------------------------------------------------------------------
@file cv_mn_image.c

@date 03/27/18 11:14:32
@author Martin Noblia
@email mnoblia@disroot.org

@brief

@detail

Licence:
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
---------------------------------------------------------------------------*/
#define STB_IMAGE_IMPLEMENTATION
#include "../inc/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../inc/stb_image_write.h"

#include "../inc/mv_cv_image.h"

/*-------------------------------------------------------------------------
                     private functions
-------------------------------------------------------------------------*/
static void mn_cv_set_pixel(struct Image m, int x, int y, int c, float val)
{
    if (x < 0 || y < 0 || c < 0 || x >= m.width || y >= m.height || c >= m.channels) return;
    assert(x < m.width && y < m.height && c < m.channels);
    m.data[c*m.height*m.width + y*m.width + x] = val;
}

static void mn_cv_add_pixel(struct Image m, int x, int y, int c, float val)
{
    assert(x < m.width && y < m.height && c < m.channels);
    m.data[c*m.height*m.width + y*m.width + x] += val;
}

static float mn_cv_get_pixel(struct Image m, int x, int y, int c)
{
    assert(x < m.width && y < m.height && c < m.channels);
    return m.data[c*m.height*m.width + y*m.width + x];
}

static float mn_cv_get_pixel_extend(struct Image m, int x, int y, int c)
{
    if(x < 0 || x >= m.width || y < 0 || y >= m.height) return 0;
    /*
    if(x < 0) x = 0;
    if(x >= m.w) x = m.w-1;
    if(y < 0) y = 0;
    if(y >= m.h) y = m.h-1;
    */
    if(c < 0 || c >= m.channels) return 0;
    return mn_cv_get_pixel(m, x, y, c);
}

/*-------------------------------------------------------------------------
                        implementations
-------------------------------------------------------------------------*/
struct Image
mn_cv_make_empty_image(int width, int height, int channels)
{
    struct Image result = {
       .width = width,
       .height = height,
       .channels = channels,
       .data = NULL,

    };
    return result;
}

struct Image
mn_cv_make_image(int width, int height, int channels)
{
    struct Image result = mn_cv_make_empty_image(width,height,channels);
    result.data = calloc(height * width * channels, sizeof(float));
    return result;
}

struct Image
mn_cv_load_image_stb(char* filename, int channels_in) {
   int width, height;
   int channels = 0;
   uchar *data = stbi_load(filename, &width, &height, &channels_in, channels);
   if (!data) {
      fprintf(stderr, "Cannot load image \"%s\"\nSTB Reason: %s\n", filename, stbi_failure_reason());
      exit(0);
   }
   if(channels_in) {
      channels = channels_in;
   }
   int i,j,k;
   struct Image im = mn_cv_make_image(width, height, channels);
   for(k = 0; k < channels; ++k){
      for(j = 0; j < height; ++j){
         for(i = 0; i < width; ++i){
            int dst_index = i + width * j + width *height * k;
            int src_index = k + channels * i + channels * width * j;
            im.data[dst_index] = ((float)data[src_index] / 255.0);
         }
      }
   }
   free(data);
   return im;
}


struct Image
mn_cv_load_image_color(char *filename, int w, int h)
{
    return mn_cv_load_image(filename, w, h, 3);
}

struct Image
resize_image(struct Image im, int width, int height)
{
    struct Image resized = mn_cv_make_image(width, height, im.channels);
    struct Image part = mn_cv_make_image(width, im.height, im.channels);
    int r, c, k;
    float w_scale = (float)(im.width - 1) / (width - 1);
    float h_scale = (float)(im.height - 1) / (height - 1);
    for(k = 0; k < im.channels; ++k){
        for(r = 0; r < im.height; ++r){
            for(c = 0; c < width; ++c){
                float val = 0;
                if(c == width-1 || im.width == 1){
                    val = mn_cv_get_pixel(im, im.width-1, r, k);
                } else {
                    float sx = c*w_scale;
                    int ix = (int) sx;
                    float dx = sx - ix;
                    val = (1 - dx) * mn_cv_get_pixel(im, ix, r, k) + dx * mn_cv_get_pixel(im, ix+1, r, k);
                }
                mn_cv_set_pixel(part, c, r, k, val);
            }
        }
    }
    for(k = 0; k < im.channels; ++k){
        for(r = 0; r < height; ++r){
            float sy = r*h_scale;
            int iy = (int) sy;
            float dy = sy - iy;
            for(c = 0; c < width; ++c){
                float val = (1-dy) * mn_cv_get_pixel(part, c, iy, k);
                mn_cv_set_pixel(resized, c, r, k, val);
            }
            if(r == height-1 || im.height == 1) continue;
            for(c = 0; c < width; ++c){
                float val = dy * mn_cv_get_pixel(part, c, iy+1, k);
                mn_cv_add_pixel(resized, c, r, k, val);
            }
        }
    }

    mn_cv_free_image(part);
    return resized;
}

struct Image
mn_cv_load_image(char *filename, int width, int height, int c)
{
   struct Image out = mn_cv_load_image_stb(filename, c);

   if((height && width) && (height != out.height || width != out.width)){
      struct Image resized = resize_image(out, width, height);
      mn_cv_free_image(out);
      out = resized;
   }
   return out;
}

void mn_cv_free_image(struct Image m)
{
    if(m.data){
        free(m.data);
    }
}

void mn_cv_save_image_png(struct Image im, const char *name)
{
    char buff[256];
    //sprintf(buff, "%s (%d)", name, windows);
    sprintf(buff, "%s", name);
    uchar *data = calloc(im.width *im.height *im.channels, sizeof(uchar));
    int i,k;
    for(k = 0; k < im.channels; ++k){
        for(i = 0; i < im.width * im.height; ++i){
            data[i * im.channels + k] = (uchar) (255*im.data[i + k * im.width * im.height]);
        }
    }
    int success = stbi_write_png(buff, im.width, im.height, im.channels, data, im.width * im.channels);
    free(data);
    if(!success) fprintf(stderr, "Failed to write image %s\n", buff);
}
