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
/* initialize_files.c                                                       */
/*                                                                          */
/* This function initializes the output and movie files.                    */
/* ======================================================================== */

#include "includes.h"

int initialize_files(char* input_errors)
{

  FILE *fp;
  unsigned long i;
  char fileprefix[128];

  /* ------------------------------------------------------------------- */
	/*  Initialize movie file.                                             */
  /* ------------------------------------------------------------------- */

	if (sim.movie) //if want movie
	{
    //reinitialize file 
    fp = fopen(sim.moviefile, "w");
    fclose(fp);
    
    //write the xyz file, first get the file name before the extention
    sscanf(sim.moviefile, "%[^.]", fileprefix);
    fp = fopen(strcat(fileprefix,".xyz"), "w");
    fprintf(fp, "%lu\nLoad this file in VMD before the .trr file\n", sim.N);
    for (i = 0; i<sim.N; i++) fprintf(fp, "C\t%13.6lf\t%13.6lf\t%13.6lf\n", atom[i].x, atom[i].y, atom[i].z);
    fclose(fp);
	}

  /* ------------------------------------------------------------------- */
	/* Write initial positions and velocities to the output file           */
  /* ------------------------------------------------------------------- */
	fp = fopen(sim.outputfile,"w");		
  fprintf(fp, "%s simulation of %lu LJ Particles at T*=%.4lf and rho*=%.4lf\n\n", sim.type, sim.N, sim.T, sim.rho);
  fprintf(fp, "Input File:         %s\n", sim.inputfile);
  fprintf(fp, "Output File:        %s\n\n", sim.outputfile);
  fprintf(fp, "%s", input_errors);
  if (!strcmp("generate",sim.seedkeyvalue)) fprintf(fp, "The random number seed was generated from the system clock.\n");
  else if (!strcmp("specified",sim.seedkeyvalue)) fprintf(fp, "The random number seed was specified in the input file.\n");
  else fprintf(fp, "The seed for the random number generator was not specified.  The default value was used.\n");
  fprintf(fp, "Random Number Seed: %ld\n\n", sim.seed);
  fprintf(fp, "\n    ***Input Parameters***\n");
  fprintf(fp, "sim         %s\n", sim.type);
  fprintf(fp, "N           %ld\n", sim.N);
  fprintf(fp, "temp        %lf\n", sim.T);
  fprintf(fp, "rho         %lf\n", sim.rho);
  fprintf(fp, "esteps      %lu\n", sim.eq);
  fprintf(fp, "psteps      %lu\n", sim.pr);
  fprintf(fp, "rcut        %lf\n", sim.rc);
  fprintf(fp, "dt          %lf\n", sim.dt);
  fprintf(fp, "coord       %s\n", sim.icoord);
  if(!strcmp(sim.type,"md")) fprintf(fp, "vel         %s\n", sim.ivel);
  if (!(sim.movie == 0)) fprintf(fp, "movie       %s  %u\n", sim.moviefile, sim.movie);
  if (!(sim.rdf == 0)) fprintf(fp, "rdf         %lf  %lf  %d  %u\n", sim.rdfmin, sim.rdfmax, sim.rdfN, sim.rdf);
  if(!strcmp("generate", sim.seedkeyvalue)) fprintf(fp, "seed        %s\n", sim.seedkeyvalue);
  else fprintf(fp, "seed        %ld\n", sim.seed);
  fprintf(fp, "output      %u\n\n", sim.output);
  fprintf(fp, "Energy Tail Correction:    %lf\n", sim.utail);
  fprintf(fp, "Pressure Tail Correction:  %lf\n", sim.ptail);

  fprintf(fp, "\n    ***INITIAL POSITIONS, XYZ Format***\n");
  fprintf(fp,"%lu\nYou can copy these coordinates to a file to open in a viewer.\n",sim.N);
	for (i=0; i<sim.N; i++) fprintf(fp, "C\t%13.6lf\t%13.6lf\t%13.6lf\n",atom[i].x, atom[i].y, atom[i].z);

  if (!strcmp(sim.type, "md"))
  {
    fprintf(fp, "\n         ***INITIAL VELOCITIES***\n");
    for (i = 0; i < sim.N; i++) fprintf(fp, "\t%13.6lf\t%13.6lf\t%13.6lf\n", atom[i].vx, atom[i].vy, atom[i].vz);
    fprintf(fp, "\n\nIteration         \tTemp         \tP            \tKE           \tPE           \tTotal Energy\n\n");
  }
  else   fprintf(fp, "\n\nIteration         \tP            \tPE\n\n");

  fclose(fp);

  return(0);
}	
