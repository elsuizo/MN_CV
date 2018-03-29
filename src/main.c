/* -------------------------------------------------------------------------
@file main.c

@date 03/27/18 10:07:46
@author Martin Noblia
@email mnoblia@disroot.org

@brief
 Primeros tests
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
#include "../inc/mn_cv_image.h"

int main(void)
{
   /* path to test image */
   char* path = "Images/DKF4tPVXkAAP8FY.jpg";
   struct Image img = mn_cv_load_image_color(path, 300, 300);
   mn_cv_save_image_png(img, "/home/elsuizo/flores3.png");

   /* image to grayscale */
   struct Image img_gray = mn_cv_grayscale_image(&img);
   mn_cv_save_image_png(img_gray, "/home/elsuizo/flores3_gray.png");

   /* binarize image */
   struct Image img_bin = mn_cv_binarize(&img_gray, 0.3);
   mn_cv_save_image_png(img_bin, "/home/elsuizo/flores3_bin.png");


   return(0);
}


