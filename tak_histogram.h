/* Copyright (C) 2017 Thomas Allen Knotts IV - All Rights Reserved          */
/* This file is part of the program ljmcmd                                  */
/*                                                                          */
/* ljmcmd is free software: you can redistribute it and/or modify           */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* ljmcmd is distributed in the hope that it will be useful,                */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with ljmcmd.  If not, see <http://www.gnu.org/licenses/>.          */

/* ======================================================================== */
/* tak_histogram.h                                                          */
/*                                                                          */
/* This file contains the histogram structures and subroutine prototypes.   */
/* It is based on the GNU Scientific Library.                               */
/*                                                                          */
/* Written by Thomas A. Knotts IV,22 Sep 2005.                              */
/* ======================================================================== */

typedef struct{
  int n;            //Number of bins
  double xmin;      //Lower bound
  double xmax;      //Upper bound
  double bin_width; //Bin width
  double *vbin;     //Mid point value of bin
  double *bin;      //Accumulating value
}tak_histogram;

tak_histogram* tak_histogram_alloc             (int n);
tak_histogram* tak_histogram_calloc            (int n);
tak_histogram* tak_histogram_calloc_uniform    (int n, double xmin, double xmax);
int tak_histogram_index_find                   (tak_histogram *h,double x,int *index);
int tak_histogram_increment                    (tak_histogram *h, double x);
int tak_histogram_accumulate                   (tak_histogram *h, double x, double weight);
int tak_histogram_fwrite                       (FILE *fp, tak_histogram *h);
void tak_histogram_free                        (tak_histogram *h);
int tak_histogram_equal_bins_p                 (const tak_histogram *h1, const tak_histogram *h2);
tak_histogram* tak_histogram_clone             (const tak_histogram *src);
tak_histogram* tak_histogram_div               (const tak_histogram *h1, const tak_histogram *h2);

