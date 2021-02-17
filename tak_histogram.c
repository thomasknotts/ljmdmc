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
/* tak_histogram.c                                                          */
/*                                                                          */
/* This file contains code to do various things with the histograms.        */
/* It is based on the GNU Scientific Library.                               */
/*                                                                          */
/* Written by Thomas A. Knotts IV,22 Sep 2005.                              */
/* ======================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "tak_histogram.h"
#include <math.h>

tak_histogram* tak_histogram_alloc(int n){
  tak_histogram *h;

  if(n==0){
    fprintf(stdout,"Histogram length (%i) must be a postitive integer\n", n);
    exit(10);
  }
 

  h = (tak_histogram*) malloc(sizeof(tak_histogram));

  if (h == 0 || h == NULL){
    fprintf(stdout,"Cannot allocate histogram h\n");
    exit(11);
  }

  h->vbin = (double*) malloc(n*sizeof(double));

  if (h->vbin == 0 || h->vbin == NULL){
    free(h);
    fprintf(stdout,"Cannot allocate histogram h->vbin\n");
    exit(11);
  }

  h->bin = (double*) malloc(n*sizeof(double));

  if (h->bin == 0 || h->bin == NULL){
    free(h->vbin);
    free(h);
    fprintf(stdout,"Cannot allocate histogram h->bin\n");
    exit(11);
  }

  h->n = n;

  return h;
}

tak_histogram* tak_histogram_calloc(int n){

  tak_histogram *h = tak_histogram_alloc(n);

  if (h == 0 || h == NULL) return h;

  for(int i=0; i<n; i++){
    h->vbin[i] = i;
    h->bin[i] = 0;
  }

  return h;
}

static void make_uniform(double vbin[], int n, double xmin, double xmax, double *bin_width){
  *bin_width = (xmax-xmin)/(double)n;
  for (int i=0; i<n; i++) vbin[i] = xmin + *bin_width*(i+0.5);

}

tak_histogram* tak_histogram_calloc_uniform(int n, double xmin, double xmax){
  
  if (xmin >= xmax){
	printf("xmin = %f, xmax = %f\n", xmin, xmax);
    fprintf(stdout,"xmin must be less than xmax for histogram\n");
    exit(10);
  }

  tak_histogram *h;
  double bin_width;
  
  h=tak_histogram_calloc(n);
  if (h == 0 || h == NULL) return h;
  
  make_uniform(h->vbin,n,xmin,xmax,&bin_width);

  h->xmin = xmin;
  h->xmax = xmax;
  h->bin_width = bin_width;

  return h;
}


/*int tak_histogram_set_ranges_uniform (tak_histogram *h){

  const int n = h->n;
  const double xmin = h->xmin;
  const double xmax = h->xmax;
  const double bin_width = h->bin_width;

  for (int i=0; i<=n; i++) h->vbin[i] = xmin + bin_width*(i+0.5);

  return(0);
}
*/

int tak_histogram_index_find(tak_histogram *h,double x,int *index){
//  static int n = h->n;
  double xmin = h->xmin;
  double xmax = h->xmax;
  double bin_width = h->bin_width;

  if(x<xmin) return -1;         // These are always false with nan because nan is not a number
  if(x>xmax) return +1;
  if(isnan(x)) return 11;

  *index = (int)((x-xmin)/bin_width);
  return(0);
}

int tak_histogram_increment (tak_histogram *h, double x)
{
  int status = tak_histogram_accumulate (h, x, 1.0);
  return status;
}

int tak_histogram_accumulate (tak_histogram *h, double x, double weight){
//  const int n = h->n;
  int index = 0;

  int status = tak_histogram_index_find(h, x, &index);

  if(status){
    //fprintf(stdout,"The value, %lf lies outside of the valid range [%lf, %lf].\n",x,h->xmin,h->xmax);
    return 1;
  }

  h->bin[index] += weight;
//  h->count[index] += 1;

  return 0;
}

int tak_histogram_fwrite (FILE *fp, tak_histogram *h){
  const int n = h->n;
  for(int i=0; i<n; i++){
//    int count = h->count[i];
//    double bin = h->bin[i];
//    double vbin = h->vbin[i];

//    int flag = abs(((int)bin-count));
//    if(flag > 0.001){
//      bin = bin / (double)count;
//    }
    fprintf(fp,"%-10d\t%13.6lf\t%13.6lf\n",i+1,h->vbin[i],h->bin[i]);
  }
  return 0;

}

int tak_histogram_equal_bins_p (const tak_histogram *h1, const tak_histogram *h2){

  if(h1->n != h2->n) return 0;

  for(int i=0; i<h1->n; i++){
    if(h1->vbin[i] != h2->vbin[i]) return 0;
  }
  
  return 1;
}

tak_histogram* tak_histogram_clone(const tak_histogram *src){
  int n=src->n;
  tak_histogram *h = tak_histogram_calloc_uniform(n,src->xmin,src->xmax);

  if(h == 0 || h == NULL) return h;

  for(int i=0; i<n; i++){
    h->vbin[i] = src->vbin[i];
    h->bin[i]  = src->bin[i];
  }

  return h;
}

tak_histogram* tak_histogram_div(const tak_histogram *h1, const tak_histogram *h2){

  if(tak_histogram_equal_bins_p(h1, h2) == 0){
    fprintf(stdout,"The histograms have different binning.\n");
    return NULL;
  }
  int n = h1->n;
  tak_histogram *h = tak_histogram_clone(h1);

  if(h == 0 || h == NULL) return h;

  for(int i=0; i<n; i++) h->bin[i] = h1->bin[i] / h2->bin[i];
  
  return h;
}

void tak_histogram_free (tak_histogram *h){

  free(h->bin);
  free(h->vbin);
  free(h);

}        
                        
