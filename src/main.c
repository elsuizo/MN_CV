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
   /* const char* path = "Images/DKF4tPVXkAAP8FY.jpg"; */
   /* const char* path_red = "Images/red.png"; */
   /* const char* path_green = "Images/green.png"; */
   /* const char* path_blue = "Images/blue.png"; */
   const char* path_hands = "Images/manos3.png";

   /* struct Image img_red = mn_cv_load_image_color(path_red, 300, 300); */
   /* struct Image img_blue = mn_cv_load_image_color(path_blue, 300, 300); */
   /* struct Image img_green = mn_cv_load_image_color(path_green, 300, 300); */

   /* struct Image img_sum1 = mn_cv_sum(&img_red, &img_blue); */
   /* struct Image img_sum2 = mn_cv_sum(&img_red, &img_green); */
   /* struct Image img_sum3 = mn_cv_sum(&img_blue, &img_green); */
   /* mn_cv_save_image_png(&img_sum1, "Images/red_plus_blue.png"); */
   /* mn_cv_save_image_png(&img_sum2, "Images/red_plus_green.png"); */
   /* mn_cv_save_image_png(&img_sum3, "Images/blue_plus_green.png"); */

   struct Image img_hands = mn_cv_load_image_color(path_hands, 296, 107);
   /* struct Image red_channel = mn_cv_red_channel(&img_hands); */
   /* struct Image blue_channel = mn_cv_blue_channel(&img_hands); */
   /* struct Image green_channel = mn_cv_green_channel(&img_hands); */
   struct Image chromatics_hand = mn_cv_chromatics_coordinates(&img_hands);
   /* mn_cv_save_image_png(&red_channel, "Images/pepe1.png"); */
   /* mn_cv_save_image_png(&green_channel, "Images/pepe2.png"); */
   /* mn_cv_save_image_png(&blue_channel, "Images/pepe3.png"); */
   mn_cv_save_image_png(&img_hands, "Images/chromatics_hand.png");

   return(0);
}
