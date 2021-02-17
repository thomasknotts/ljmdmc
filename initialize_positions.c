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
/* initialize_positions.c                                                   */
/*                                                                          */
/* This subroutine initializes the coordinates for the simulation.          */
/* If the user specifies a coordinate file in the input file, the           */
/* coordinates are read in.  Otherwise, they are generated as an FCC        */
/* lattice.  The FCC approach is the default behavior.                      */
/* ======================================================================== */

#include "includes.h"

bool readline(char*, int, FILE*);

int initialize_positions(char* fn_c, char* fn_i )
{
  int nlin; //number of lines needed on each side of the box
  double a; //length of simulation box side, length of one unit cell
  int particle; //counter for the number of particles
  char ch, buff[MAX_LINE];
  int xdir, ydir, zdir;
  unsigned long i;
  FILE *fp;
  bool read_flag;
  
  sim.length = pow(((double)sim.N / sim.rho), 1.0 / 3.0);
 
  /* ------------------------------------------------------------------- */
  /*  If the input files specifies "generate" then generate the          */
  /*  initial coordinates on an FCC lattice.                             */
  /* ------------------------------------------------------------------- */
  if (!strcmp(fn_c, "generate"))  
  {
    /* ============================================ */
    /*  Zero the counters                           */
    /* ============================================ */
    particle = 0;
    ch = '0';

    /* ============================================ */
    /*  Calculate the number of particles needed    */
    /*  on each side of the box                     */
    /* ============================================ */
    nlin = (int)pow((double)sim.N / 4.0, 1.0 / 3.0);
    if (((double)nlin*(double)nlin*(double)nlin) < (double)sim.N / 4.0) nlin = nlin + 1;//if N is not a cube root, add 1 to nlin

    /* Calculate the length of one unit cell */
    a = sim.length / (double)nlin;

    /* Assign the positions to an fcc lattice */
    for (zdir = 0; zdir<nlin; zdir++)
    {
      for (ydir = 0; ydir<nlin; ydir++)
      {
        for (xdir = 0; xdir<nlin; xdir++)
        {
          for (i = 0; i<4; i++)
          {
            if (particle == sim.N) return(0);
            switch (ch) {
            case '0':
              atom[particle].x = 0.0 + (double)xdir*a;
              atom[particle].y = 0.0 + (double)ydir*a;
              atom[particle].z = 0.0 + (double)zdir*a;
              ch = '1';
              particle++;

              break;
            case '1':

              atom[particle].x = 0.0 + (double)xdir*a;
              atom[particle].y = 0.5*a + (double)ydir*a;
              atom[particle].z = 0.5*a + (double)zdir*a;
              ch = '2';
              particle++;

              break;
            case '2':
              atom[particle].x = 0.5*a + (double)xdir*a;
              atom[particle].y = 0.0 + (double)ydir*a;
              atom[particle].z = 0.5*a + (double)zdir*a;
              ch = '3';
              particle++;

              break;
            case '3':
              atom[particle].x = 0.5*a + (double)xdir*a;
              atom[particle].y = 0.5*a + (double)ydir*a;
              atom[particle].z = 0.0 + (double)zdir*a;
              ch = '0';
              particle++;

              break;
            }// switch		
          }
        }
      }
    }
  }

  /* ------------------------------------------------------------------- */
  /*  If the input file specified a coordinate file then read the        */
  /*  corrdinates from file.                                             */
  /* ------------------------------------------------------------------- */
  else
  {

    /* ============================================ */
    /*  Check to see if the file exists and open it */
    /*  if it does                                  */
    /* ============================================ */
    if (!(fp = fopen(fn_c, "r")))
    {
      fprintf(stdout, "Coordinate file \"%s\" does not exist.\n", fn_c);
      return(ERROR_FILE_NOT_FOUND);
    }
    unsigned long N = 0; //counter for number of atoms 

    /* ============================================ */
    /*  Read the coordinates from the file          */
    /* ============================================ */
    for (i = 0; i<sim.N; i++)
    {
      read_flag = readline(buff, MAX_LINE, fp);
      if (!read_flag) break;

      if (sscanf(buff, "%lf %lf %lf", &atom[i].x, &atom[i].y, &atom[i].z) != 3)//if statement checks to see if three numbers are read
      {
        fprintf(stdout, "There is a problem with the coordinates for atom %lu in \"%s\"\n", i+1, fn_c);
        return(ERROR_INPUT_FILE);
      }
      else
      {
        N++;
        /* ============================================ */
        /*  Check to see if any of the input            */
        /*  coordinates are outside of the box.         */
        /* ============================================ */
        if (atom[i].x > sim.length || atom[i].y > sim.length || atom[i].z > sim.length)
        {
          fprintf(stdout, "The coordinates (%lf, %lf, %lf) for atom %lu read in from coordinate file \"%s\" is outside of the box of length %lf\n", atom[i].x, atom[i].y, atom[i].z, i, fn_c, sim.length);
          return(ERROR_INPUT_FILE);
        }
      }
    }
    fclose(fp);   

    /* ============================================ */
    /*  Check to see if the number of coordinates   */
    /*  in the file is equal to the number          */
    /*  specified in the input file                 */
    /* ============================================ */
    if (N != sim.N)
    {
      fprintf(stdout, "The number of coordinates (%lu) in \"%s\" is not equal to the number of atoms (%lu) in \"%s\"\n", N, fn_c,sim.N, fn_i);
      return(ERROR_INPUT_FILE);
    }
  }
  return(0);
}