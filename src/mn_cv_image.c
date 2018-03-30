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

#include "../inc/mn_cv_image.h"
/* #include "../inc/test.h" */

/*-------------------------------------------------------------------------
                        private functions
-------------------------------------------------------------------------*/

/**
 * @brief Set the value in the correspondiente (x, y) pixel in a image
 *
 * @param img
 * @param x
 * @param y
 * @param c
 * @param value
 */
static void mn_cv_set_pixel(struct Image* img, int x, int y, int c, float value)
{

   /* check the imputs */
   /* TODO(elsuizo:2018-03-28):porque no hacer que la imagen tenga uint directamente */
   if (x < 0 || y < 0 || c < 0 || x >= img->width || y >= img->height || c >= img->channels) {
      /* TODO(elsuizo:2018-03-28):imprimir un error */
      fprintf(stderr, "Failed to write image");
      return;
   }
   assert(x < img->width && y < img->height && c < img->channels);
   img->data[c * img->height * img->width + y * img->width + x] = value;
}

/**
 * @brief : Add the value in the corr
 *
 * @param img
 * @param x
 * @param y
 * @param c
 * @param value
 */
static void mn_cv_add_pixel(struct Image* img, int x, int y, int c, float value)
{
   /* check the inputs */
   assert(x < img->width && y < img->height && c < img->channels);
   img->data[c * img->height * img->width + y * img->width + x] += value;
}

static void mn_cv_mul_pixel(struct Image* img, int x, int y, int c, float value)
{
   /* check the inputs */
   assert(x < img->width && y < img->height && c < img->channels);
   img->data[c * img->height * img->width + y * img->width + x] *= value;
}

static float mn_cv_get_pixel(struct Image* img, int x, int y, int c)
{
   assert(x < img->width && y < img->height && c < img->channels);
   return img->data[c * img->height * img->width + y * img->width + x];
}

static float mn_cv_get_pixel_extend(struct Image* img, int x, int y, int c)
{
   if(x < 0 || x >= img->width || y < 0 || y >= img->height) {
      return 0;
   }
   /*
      if(x < 0) x = 0;
      if(x >= img.w) x = img.w-1;
      if(y < 0) y = 0;
      if(y >= img.h) y = img.h-1;
      */
   if(c < 0 || c >= img->channels) {
      return 0;
   }
   return mn_cv_get_pixel(img, x, y, c);
}

static struct Image
mn_cv_resize_image(struct Image im, int width, int height)
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
            if(c == width - 1 || im.width == 1){
               val = mn_cv_get_pixel(&im, im.width - 1, r, k);
            } else {
               float sx = c * w_scale;
               int ix = (int) sx;
               float dx = sx - ix;
               val = (1 - dx) * mn_cv_get_pixel(&im, ix, r, k) + dx * mn_cv_get_pixel(&im, ix + 1, r, k);
            }
            mn_cv_set_pixel(&part, c, r, k, val);
         }
      }
   }
   for(k = 0; k < im.channels; ++k){
      for(r = 0; r < height; ++r){
         float sy = r*h_scale;
         int iy = (int)sy;
         float dy = sy - iy;
         for(c = 0; c < width; ++c){
            float val = (1-dy) * mn_cv_get_pixel(&part, c, iy, k);
            mn_cv_set_pixel(&resized, c, r, k, val);
         }
         if(r == height-1 || im.height == 1) {
            continue;
         }
         for(c = 0; c < width; ++c){
            float val = dy * mn_cv_get_pixel(&part, c, iy + 1, k);
            mn_cv_add_pixel(&resized, c, r, k, val);
         }
      }
   }

   mn_cv_free_image(&part);
   return resized;
}

static struct Image
mn_cv_get_image_channel(struct Image* img, int channel_selected) {

    struct Image result = mn_cv_make_image(img->width, img->height, 1);
    int i;
    for(i = 0; i < img->height * img->width; ++i){
        result.data[i] = img->data[i + channel_selected * img->height * img->width];
    }
    return result;
}

/*-------------------------------------------------------------------------
                     Public implementations
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
   struct Image result = mn_cv_make_empty_image(width, height, channels);
   result.data = calloc(height * width * channels, sizeof(float));
   return result;
}

struct Image
mn_cv_load_image_stb(const char* filename, int channels_in) {
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
mn_cv_load_image_color(const char* filename, int w, int h)
{
   return mn_cv_load_image(filename, w, h, 3);
}

struct Image
mn_cv_load_image(const char* filename, int width, int height, int c)
{
   struct Image out = mn_cv_load_image_stb(filename, c);

   if((height && width) && (height != out.height || width != out.width)){
      struct Image resized = mn_cv_resize_image(out, width, height);
      mn_cv_free_image(&out);
      out = resized;
   }
   return out;
}

void mn_cv_free_image(struct Image* img)
{
   if(img->data){
      free(img->data);
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
   if(!success) {
      fprintf(stderr, "Failed to write image %s\n", buff);
   }
}

/* TODO(elsuizo:2018-03-29):hacer una funcion que tome los dos casos de suma: color y grayscale  */
struct Image
mn_cv_sum(struct Image* img1, struct Image* img2) {

   /* test dimentions */
   if (img1->width != img2->width && img1->height != img2->height) {
      fprintf(stderr, "Failed to sum images");
   }
   if (img1->channels != img2->channels) {
      fprintf(stderr, "Failed to sum the images");
   }
   struct Image result = mn_cv_make_image(img1->width, img2->height, img2->channels);
   int i, j, k;
   float pix1, pix2;
   for(k = 0; k < result.channels; ++k) {
      for(j = 0; j < result.height; ++j) {
         for(i = 0; i < result.width; ++i) {
            pix1 = mn_cv_get_pixel(img1, i, j, k);
            pix2 = mn_cv_get_pixel(img2, i, j, k);
            /* printf("p1 + p2 ---> %f\n", pix1 + pix2); */
            mn_cv_set_pixel(&result, i, j, k, pix1 + pix2);
         }
      }
   }

   return result;
}

struct Image
mn_cv_red_channel(struct Image* img) {
   struct Image result = mn_cv_get_image_channel(img, 1);

   return result;
}

struct Image
mn_cv_blue_channel(struct Image* img) {

   struct Image result = mn_cv_get_image_channel(img, 2);

   return result;
}

struct Image
mn_cv_green_channel(struct Image* img) {

   struct Image result = mn_cv_get_image_channel(img, 3);

   return result;
}

struct Image
mn_cv_grayscale_image(struct Image* img) {

    assert(img->channels == 3);
    int i, j, k;
    struct Image result = mn_cv_make_image(img->width, img->height, 1);
    float scale[] = {0.299, 0.587, 0.114};
    for(k = 0; k < img->channels; ++k){
        for(j = 0; j < img->height; ++j){
            for(i = 0; i < img->width; ++i){
                result.data[i + img->width * j] += scale[k] * mn_cv_get_pixel(img, i, j, k);
            }
        }
    }
    return result;
}

struct Image
mn_cv_threshold_image(struct Image* img, float thresh) {
    int i;
    struct Image result = mn_cv_make_image(img->width, img->height, img->channels);
    for(i = 0; i < img->width * img->height * img->channels; ++i){
        result.data[i] = img->data[i] > thresh ? 1 : 0;
    }
    return result;
}

struct Image
mn_cv_copy_image(struct Image* img)
{
    struct Image result = *img;
    result.data = calloc(img->height * img->width * img->channels, sizeof(float));
    memcpy(result.data, img->data, img->height * img->width * img->channels * sizeof(float));

    return result;
}

struct Image
mn_cv_binarize(struct Image* img, float value)
{
    struct Image result = mn_cv_copy_image(img);
    int i;
    for(i = 0; i < img->width * img->height * img->channels; ++i){
        if(result.data[i] > value) {
           result.data[i] = 1;
        } else {
           result.data[i] = 0;
        }
    }
    return result;
}

