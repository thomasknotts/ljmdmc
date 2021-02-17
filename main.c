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
/* ljmcmd                                                                   */
/*                                                                          */
/* Thomas A. Knotts IV                                                      */
/* Brigham Young University                                                 */
/* Department of Chemical Engineering                                       */
/* Provo, UT  84606                                                         */
/* Email: thomas.knotts@byu.edu                                             */
/* ======================================================================== */
/* Version 1.0 - January 2017                                               */
/* ======================================================================== */

/* ======================================================================== */
/* main.c                                                                   */
/* This is the main subroutine for the the program ljmcmd.  This program    */
/* performs simulations of Lennard Jones fluids.  The code uses             */
/* dimensionless variables and does either NVT MC or NVE MD simulations.    */
/* ======================================================================== */

#define MAIN
#include "includes.h"

int read_input(char*, char*, char*);
int allocate(void);
int initialize_positions(char*, char*);
int initialize_velocities(char*, char*);
int initialize_files(char*);
int initialize_counters(void);
int error_exit(int);
double ran_num_double(long, int, int);
int nvemd(void);
int nvtmc(void);
double forces(void);
double kinetic_energy(void);
double temperature(double);

int main(int argc, char *argv[])
{
  FILE *fp;
  int return_flag;
  time_t start_time, end_time;
  char input_errors[8192];

  /* ------------------------------------------------------------------- */
  /* Initialize the timer                                                */
  /* ------------------------------------------------------------------- */
  start_time = time(NULL);

  /* ------------------------------------------------------------------- */
  /*  Check the argument                                                 */
  /* ------------------------------------------------------------------- */
  if (argc != 3)
  {
    fprintf(stdout, "This program requires two command line arguments: the path to the input file\nand the path to the output file.\n");
    error_exit(ERROR_ARGUMENTS);
  }



  /* ------------------------------------------------------------------- */
  /*  Read the input file                                                */
  /* ------------------------------------------------------------------- */
  return_flag = read_input(argv[1], argv[2], input_errors);
  if (return_flag) error_exit(return_flag);

  /* ------------------------------------------------------------------- */
  /*  Allocate memory to arrays                                          */
  /* ------------------------------------------------------------------- */
  return_flag = allocate();
  if (return_flag) error_exit(return_flag);

  /* ------------------------------------------------------------------- */
  /*  Initialize or read in positions                                    */
  /* ------------------------------------------------------------------- */
  return_flag = initialize_positions(sim.icoord, sim.inputfile);
  if (return_flag) error_exit(return_flag);

  /* ------------------------------------------------------------------- */
  /*  Initialize the random number generator                             */
  /* ------------------------------------------------------------------- */
  if (!strcmp("generate",sim.seedkeyvalue))
  {
    time_t idum_clock;
    time(&idum_clock);
    sim.seed = -1 * (long)idum_clock;
  }
  ran_num_double(sim.seed, 0, 1);

  /* ------------------------------------------------------------------- */
  /*  If md, initialize or read in velocities                            */
  /* ------------------------------------------------------------------- */
  if (!strcmp(sim.type, "md")) {
    return_flag = initialize_velocities(sim.ivel, sim.inputfile);
    if (return_flag) error_exit(return_flag);
  }

  /* ------------------------------------------------------------------- */
  /*  Initialize the instantaneous forces, energies, and properties for  */
  /*  iteration 0                                                        */
  /* ------------------------------------------------------------------- */
  iprop.pe = forces();
  if (!strcmp(sim.type, "md"))
  {
    iprop.ke = kinetic_energy();          //calculate the kinetic energy
    iprop.T = temperature(iprop.ke);      //calculate the temperature
  }

  /* ------------------------------------------------------------------- */
  /*  Initialize the accumulators and corrections                        */
  /* ------------------------------------------------------------------- */
  return_flag = initialize_counters();

  /* ------------------------------------------------------------------- */
  /*  Initialize the output and movie files                              */
  /* ------------------------------------------------------------------- */
  return_flag = initialize_files(input_errors);

  /* ------------------------------------------------------------------- */
  /*  Call the driver for the md or mc simulation                        */
  /* ------------------------------------------------------------------- */
  if (!strcmp(sim.type, "md")) 
  {
    return_flag = nvemd();
    if (return_flag) error_exit(return_flag);
  }
  else
  {
    return_flag = nvtmc();
    if (return_flag) error_exit(return_flag);
  }

  /* ------------------------------------------------------------------- */
  /*  Calculate the wall time and finalize the simulation                */
  /* ------------------------------------------------------------------- */
  end_time = time(NULL);
  fprintf(stdout, "Total Wall Time: %f minutes.\n", difftime(end_time, start_time) / 60.0);
  fp = fopen(sim.outputfile, "a");
  fprintf(fp, "Total Wall Time: %f minutes\n", difftime(end_time, start_time) / 60.0);
  fclose(fp);
  //printf("Press enter to continue...\n");
  //getchar();

  return(0);
}