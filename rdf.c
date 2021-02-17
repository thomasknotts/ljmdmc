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
/* rdf.c                                                                    */
/*                                                                          */
/* This files contains subroutines to calculate and write the rdf.          */
/* ======================================================================== */

#include "includes.h"


/* ------------------------------------------------------------------- */
/* This subroutine is called at intervals specified in the input file  */
/* ------------------------------------------------------------------- */
int rdf_accumulate(tak_histogram *h)
{
  double dr, dx, dy, dz;
  unsigned long i, j;
  int return_value=0;

  /* ------------------------------------------------------------------- */
  /* Loop around all pairs of atoms and determine the distance           */
  /* ------------------------------------------------------------------- */
  for (i = 0; i<sim.N - 1; i++)
  {
    for (j = i + 1; j<sim.N; j++)
    {
      dx = atom[i].x - atom[j].x;
      dy = atom[i].y - atom[j].y;
      dz = atom[i].z - atom[j].z;

      /* ============================================ */
      /*         Minimum Image Convention             */
      /* ============================================ */
      if (fabs(dx)>(sim.length*0.5))
      {
        if (dx < 0.0)
          dx += sim.length;
        else
          dx -= sim.length;
      }
      if (fabs(dy)>(sim.length*0.5))
      {
        if (dy < 0.0)
          dy += sim.length;
        else
          dy -= sim.length;
      }
      if (fabs(dz)>(sim.length*0.5))
      {
        if (dz < 0.0)
          dz += sim.length;
        else
          dz -= sim.length;
      }

      dr = sqrt(dx*dx + dy*dy + dz*dz);

      /* ============================================ */
      /*  Increment the histogram for the calculated  */
      /*  value of dr                                 */
      /* ============================================ */
      if (tak_histogram_increment(h, dr))
      {
        //fprintf(stdout, "Distance for rdf binning was outside of range. min=%lf, max=%lf, d=%lf", sim.rdfmin, sim.rdfmax, dr);
        return_value = 1;
      }
    }
  }

  return(return_value);
}

/* ------------------------------------------------------------------- */
/*  This subroutine is called at the end of the simulation to          */
/*  calculate the rdf from the histograms.                             */
/*  See page 184 of Allen and Tildesley.                               */
/* ------------------------------------------------------------------- */
int rdf_finalize(tak_histogram *h, double Ncalls)
{
  double bin_width_2;   //half the bin width to help with computation
  double sphere = 4.0 / 3.0 * PI * sim.rho;   //constant to aid in computation of number of particle in each shell
  double r1, r2, nideal; 
 
  bin_width_2 = h->bin_width / 2.0;

  /* ------------------------------------------------------------------- */
  /*  Calculate the number of atoms that should be in each bin           */
  /*  (the denominator) and the number that were there in simulation     */
  /*  (the numerator).                                                   */
  /* ------------------------------------------------------------------- */
  for (int i = 0; i < sim.rdfN; i++)
  {
    r1 = h->vbin[i] - bin_width_2;            // lower bound of bin
    r2 = h->vbin[i] + bin_width_2;            // upper bound of bin
    nideal = sphere * (r2*r2*r2 - r1*r1*r1);  //number of particles expected to be in the shell for bin i
    h->bin[i] = h->bin[i] / Ncalls / nideal / (double)sim.N * 2.0; //The 2.0 come from the fact that we only loop over N/2 particles when binning.
  }

  return(0);
}