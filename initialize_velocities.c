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
/* initialize_velocities.c                                                  */
/*                                                                          */
/* This subroutine initializes the velocities for the simulation.           */
/* If the user specifies a velocity file in the input file, the             */
/* velocities are read in.  Otherwise, they are generated from a random     */
/* gaussian distribution.  The rand approach is the default behavior.       */
/* ======================================================================== */

#include "includes.h"

bool readline(char*, int, FILE*);
double ran_num_double(long, double, double);
double kinetic_energy(void);
double temperature(double);
double scale_velocities(double);
int zero_momentum(void);
int check_momentum(void);
int error_exit(int);

int initialize_velocities(char* fn_c, char* fn_i )
{
  char buff[MAX_LINE];
  unsigned long i;
  FILE *fp;
  bool read_flag;
  double vmax = sqrt(sim.T*3.0);
  double ke, temp;
  int return_flag;
  
  /* ------------------------------------------------------------------- */
  /*  If the input files specifies "generate" then generate the          */
  /*  initial velocities randomly.                                       */
  /* ------------------------------------------------------------------- */
  if (!strcmp(fn_c, "generate")) 
  {
    for (i = 0; i<sim.N; i++)
    {
      atom[i].vx = ran_num_double(1, -1, 1)*vmax;
      atom[i].vy = ran_num_double(1, -1, 1)*vmax;
      atom[i].vz = ran_num_double(1, -1, 1)*vmax;
    }

    return_flag = zero_momentum();
    if (return_flag) error_exit(return_flag);

    ke = kinetic_energy();
    temp = temperature(ke);
    scale_velocities(temp);
    if (check_momentum())
    {
      fprintf(stdout, "Linear momentum could not be zeroed.\n");
      return(ERROR_LINEAR_MOMENTUM);
    }
    return(0);
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
      fprintf(stdout, "Velocity file \"%s\" does not exist.\n", fn_c);
      return(ERROR_FILE_NOT_FOUND);
    }
    unsigned long N = 0; //counter for number of velocities 

    /* ============================================ */
    /*  Read the velocities from the file           */
    /* ============================================ */
    for (i = 0; i<sim.N; i++)
    {
      read_flag = readline(buff, MAX_LINE, fp);
      if (!read_flag) break;

      if (sscanf(buff, "%lf %lf %lf", &atom[i].vx, &atom[i].vy, &atom[i].vz) != 3)//if statement checks to see if three numbers are read
      {
        fprintf(stdout, "There is a problem with the velocities for atom %lu in \"%s\"\n", i+1, fn_c);
        return(ERROR_INPUT_FILE);
      }
      else N++;
    }
   
    /* ============================================ */
    /*  Check to see if the number of velocities    */
    /*  in the file is equal to the number          */
    /*  specified in the input file                 */
    /* ============================================ */
    if (N != sim.N)
    {
      fprintf(stdout, "The number of velocities (%lu) in \"%s\" is not equal to the number of atoms (%lu) in \"%s\"\n", N, fn_c,sim.N, fn_i);
      return(ERROR_INPUT_FILE);
    }

    fclose(fp);
    
    return(0);
  }
}