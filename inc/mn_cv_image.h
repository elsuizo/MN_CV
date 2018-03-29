/* -------------------------------------------------------------------------
@file mv_cv_image.h

@date 03/27/18 11:16:03
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
#ifndef MN_CV_IMAGE_H
#define MN_CV_IMAGE_H

#include <stdio.h>
#include <stdlib.h>
/*-------------------------------------------------------------------------
                              datatypes
-------------------------------------------------------------------------*/
typedef unsigned char uchar;
struct Image {
   int     width;
   int     height;
   int     channels;
   float*  data;
};

/* TODO(elsuizo:2018-03-29):manejo de errores */
enum ERRORS {
   FATAL,
   READ,
   OK,
};
/*-------------------------------------------------------------------------
                              prototypes
-------------------------------------------------------------------------*/
struct Image
mn_cv_load_image_color(const char* filename, int w, int h);

struct Image
mn_cv_load_image(const char* filename, int w, int h, int c);

struct Image
mn_cv_load_image_stb(const char* filename, int channels_in);

struct Image
mn_cv_make_image(int width, int height, int channels);

struct Image
mn_cv_make_empty_image(int width, int height, int channels);

struct Image
mn_cv_sum(struct Image* img1, struct Image* img2);

void mn_cv_free_image(struct Image* m);

void mn_cv_save_image_png(struct Image im, const char* name);

struct Image
mn_cv_grayscale_image(struct Image* img);

struct Image
mn_cv_threshold_image(struct Image* img, float thresh);

struct Image
mn_cv_copy_image(struct Image* img);

struct Image
mn_cv_binarize(struct Image* img, float value);

#endif
