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
/* finalize_file.c                                                          */
/*                                                                          */
/* This function calculates the simulation averages for the thermodynamic   */
/* properties stored in aprop and writes them to the output file.           */
/* ======================================================================== */

#include "includes.h"

int rdf_finalize(tak_histogram*, double);

int finalize_file(tak_histogram *h, double Nrdfcalls)
{
  FILE *fp;
  double ke, pe, pe2, T, virial, P, cv, Dmsd;
  double pr = (double)sim.pr;
  double N = (double)sim.N;
  unsigned long i;

  /* ------------------------------------------------------------------- */
  /*  Calculate the simple averages                                      */
  /* ------------------------------------------------------------------- */
  ke = 0.0;
  pe = aprop.pe / pr;
  pe2 = aprop.pe2 / pr;
  virial = aprop.virial / pr;
  T = sim.T;
  if (!strcmp(sim.type, "md"))
  {
    ke = aprop.ke / pr;
    T = aprop.T / pr;
  }
  else
  {
    pe = pe / N;           //Divide by N here because each production step
    pe2 = pe2 / N;         //is composed of N trials (1 MC sweep)
    virial = virial / N;
  }

  /* ------------------------------------------------------------------- */
  /*  Calculate heat capacity and pressure                               */
  /* ------------------------------------------------------------------- */
  P = sim.rho*T + 1.0 / 3.0 / pow(sim.length, 3.0)*virial + sim.ptail;
  if (!strcmp(sim.type, "mc")) cv = (pe2 - pe*pe) / (T*T) / N + 3.0/2.0; //nvt expression
  else cv = 3.0 / 2.0 / (1 - 2.0 / 3.0*(pe2 - pe*pe) / N / (T*T));       //nve expression
  
  /* ------------------------------------------------------------------- */
  /*  Calculate diffusion from MSD                                       */
  /* ------------------------------------------------------------------- */
  Dmsd = 0.0;
  for (i = 0; i < sim.N; i++) Dmsd += atom[i].dx*atom[i].dx+ atom[i].dy*atom[i].dy+ atom[i].dz*atom[i].dz;
  Dmsd = Dmsd / pr / N / 6 / sim.dt;

  /* ------------------------------------------------------------------- */
  /*  Write the data to file                                             */
  /* ------------------------------------------------------------------- */
  fp = fopen(sim.outputfile, "a");

  fprintf(fp, "\n    ***FINAL POSITIONS, XYZ Format***\n");
  fprintf(fp, "%lu\nYou can copy these coordinates to a file to open in a viewer.\n", sim.N);
  for (i = 0; i<sim.N; i++) fprintf(fp, "C\t%13.6lf\t%13.6lf\t%13.6lf\n", atom[i].x, atom[i].y, atom[i].z);
  if (!strcmp(sim.type, "md"))
  {
    fprintf(fp, "\n         ***FINAL VELOCITIES***\n");
    for (i = 0; i < sim.N; i++) fprintf(fp, "\t%13.6lf\t%13.6lf\t%13.6lf\n", atom[i].vx, atom[i].vy, atom[i].vz);
  }

  if (sim.rdf)
  {
    fprintf(fp, "\n***Radial Distribution Function***\n\n");
    rdf_finalize(h, Nrdfcalls);
    tak_histogram_fwrite(fp, h);
    tak_histogram_free(h);
  }

  if (sim.pr > 0)
  {
    fprintf(fp, "\n***Simulation Averages***\n\n");
    fprintf(fp, "Temperature:       %10.6lf\n", T);
    fprintf(fp, "Pressure:          %10.6lf\n", P);
    fprintf(fp, "Potential Energy:  %10.6lf\n", pe / N + sim.utail);
    fprintf(fp, "Heat Capacity:     %10.6lf\n\n", cv);
    if (!strcmp(sim.type, "md"))
    {
      fprintf(fp, "Kinetic Energy:    %10.6lf\n", ke / N);
      fprintf(fp, "Total Energy:      %10.6lf\n", ((ke + pe) / N)+sim.utail);
      fprintf(fp, "Diffusivity:       %10.6lf\n", Dmsd);
    }
    if (!strcmp(sim.type, "mc"))
    {
        if (aprop.ntrys != 0)
        {
            fprintf(fp, "MC Moves Accepted:        %10.6lf%%\n", (double)aprop.naccept / (double)aprop.ntrys * 100);
            fprintf(fp, "Final Max Displacement:   %10.6lf\n\n", sim.dt);
        }
    }
    
  }
  else fprintf(fp, "\nNo productions steps were specified, so simulation averages were not calculated.\n\n");
  
  fclose(fp);

  free(atom);

  return(0);
}