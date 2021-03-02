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
/* nvemd.c                                                                  */
/*                                                                          */
/* This subroutine is the main driver for NVE MD simulations.               */
/* ======================================================================== */

#include "includes.h"

int    scale_velocities(double);
int    verlet1(void);
int    verlet2(void);
int    rdf_accumulate(tak_histogram*);
int    finalize_file(tak_histogram*, double);
double forces();
double kinetic_energy();
double temperature(double);
void   write_trr(unsigned long, int);

int nvemd()
{
  unsigned long i;
  unsigned long rescale_freq = 100;
  double ke, pe, T, P, Pave;
  double Nrdfcalls;
  tak_histogram *hrdf=NULL;
  FILE *fp;

  /* ============================================ */
  /* Write Interation 0 and write to file.        */
  /* ============================================ */
  // Note, pe and virial were calculated in main() for the
  // initial configuration. They were stored in iprop.
  P = sim.rho * iprop.T + 1.0 / 3.0 / pow(sim.length, 3.0) * iprop.virial + sim.ptail;
  fp = fopen(sim.outputfile, "a");
  fprintf(fp, "%-13lu    %13.6lf    %13.6lf    %13.6lf    %13.6lf    %13.6lf    %13.6lf    %13.6lf\n", (unsigned long)0, iprop.T, iprop.T, P, P, iprop.ke / (double)sim.N, iprop.pe / (double)sim.N + sim.utail, (iprop.ke + iprop.pe) / (double)sim.N + sim.utail);
  fflush(fp);
  fclose(fp);

  /* ------------------------------------------------------------------- */
  /*  Perform equilibration steps                                        */
  /* ------------------------------------------------------------------- */
  for (i = 1; i <= sim.eq; i++)
  {
    verlet1();               //first half of velocity verlet algorithm
    pe=forces();             //calculate the forces
    verlet2();               //second half of velocity verlet algorithm
    ke = kinetic_energy();   //calculate the kinetic energy
    T = temperature(ke);     //calculate the temperature
    
	  /* ============================================ */
	  /*  Accumulate the properties for the step      */
	  /* ============================================ */
    iprop.pe = pe;
    iprop.ke = ke;
    iprop.T = T;

    aprop.T += T;
    aprop.virial += iprop.virial; //iprop.virial is set in forces

	  /* ============================================ */
	  /*  Output instantaneous properties at          */
      /*  the interval specified in the input file    */
	  /* ============================================ */
    if (i%sim.output == 0)
    {
      P = sim.rho * iprop.T + 1.0 / 3.0 / pow(sim.length, 3.0) * iprop.virial + sim.ptail;
      Pave = sim.rho * aprop.T / (double)i + 1.0 / 3.0 / pow(sim.length, 3.0) * aprop.virial / (double)i + sim.ptail;
      fp = fopen(sim.outputfile, "a");
      fprintf(fp, "%-13lu    %13.6lf    %13.6lf    %13.6lf    %13.6lf    %13.6lf    %13.6lf    %13.6lf\n", (unsigned long)i, iprop.T, aprop.T/(double)i, P, Pave, iprop.ke/(double)sim.N, iprop.pe / (double)sim.N + sim.utail, (iprop.ke + iprop.pe) / (double)sim.N + sim.utail);
      fflush(fp);
      fclose(fp);
      fprintf(stdout, "Equilibration Step %-lu\n", i);
    }
    
    /* ============================================ */
    /*  Rescale the velocities to acheive the       */
    /*  temperature specified in the input file.    */
    /*  This is only done during the equilibration  */
    /*  steps of MD simulations.                    */
    /* ============================================ */
    if (i % rescale_freq == 0)
    {
      scale_velocities(aprop.T/(double)i);
    }

    /* ============================================ */
    /*  Write the movie file at the intervals       */
    /*  specified in the input file                 */
    /* ============================================ */
    if (sim.movie)
    {
      if (i%sim.movie == 0) write_trr(i, 0);
    }

  }

  /* ------------------------------------------------------------------- */
  /*  Reset accumulators for production steps                            */
  /* ------------------------------------------------------------------- */
  aprop.pe = 0.0;
  aprop.ke = 0.0;
  aprop.T = 0.0;
  aprop.virial = 0.0;
  for (i = 0; i < sim.N; i++) {
    atom[i].dx = 0.0;
    atom[i].dy = 0.0;
    atom[i].dz = 0.0;
  }
  /* ------------------------------------------------------------------- */
  /*  Initialize rdf histogram                                           */
  /* ------------------------------------------------------------------- */
  if (sim.rdf)
  {
    hrdf = tak_histogram_calloc_uniform(sim.rdfN, sim.rdfmin, sim.rdfmax);
    if (hrdf == 0 || hrdf == NULL)
    {
      fprintf(stdout, "The histogram for the rdf could not be allocated.\n");
      exit(10);
    }
    Nrdfcalls = 0;
  }

  /* ------------------------------------------------------------------- */
  /*  Perform production steps                                           */
  /* ------------------------------------------------------------------- */
    for (i = 1; i <= sim.pr; i++)
  {
    verlet1();               //first half of velocity verlet algorithm
    pe = forces();           //calculate the forces
    verlet2();               //second half of velocity verlet algorithm
    ke = kinetic_energy();   //calculate the kinetic energy
    T = temperature(ke);     //calculate the temperature

	  /* ============================================ */
	  /*  Accumulate the properties for the step      */
	  /* ============================================ */
    iprop.pe      = pe;
    iprop.ke      = ke;
    iprop.T       = T;

    aprop.ke     += ke;
    aprop.pe     += pe;
    aprop.pe2    += pe*pe;
    aprop.T      += T;
    aprop.virial += iprop.virial; //iprop.virial is set in forces
    
    
    if (sim.rdf)//accumulate the rdf if specified in the input file (production steps only)
    {
      if (i%sim.rdf == 0)
      {
        Nrdfcalls += 1;
        rdf_accumulate(hrdf);
      }
    }

    /* ============================================ */
	/*  Output instantaneous properties at          */
    /*  the interval specified in the input file    */
	/* ============================================ */
    if (i%sim.output == 0)
    {
      P = sim.rho * iprop.T + 1.0 / 3.0 / pow(sim.length, 3.0) * iprop.virial + sim.ptail;
      Pave = sim.rho * aprop.T / (double)i + 1.0 / 3.0 / pow(sim.length, 3.0) * aprop.virial / (double)i + sim.ptail;
      fp = fopen(sim.outputfile, "a");
      fprintf(fp, "%-13lu    %13.6lf    %13.6lf    %13.6lf    %13.6lf    %13.6lf    %13.6lf    %13.6lf\n", (unsigned long)i, iprop.T, aprop.T / (double)i, P, Pave, iprop.ke / (double)sim.N, iprop.pe / (double)sim.N + sim.utail, (iprop.ke + iprop.pe) / (double)sim.N + sim.utail);
      fflush(fp);
      fclose(fp);
      fprintf(stdout, "Production Step    %-lu\n", i);
    }

    /* ============================================ */
    /*  Write the movie file at the intervals       */
    /*  specified in the input file                 */
    /* ============================================ */
    if (sim.movie)
    {
      if (i%sim.movie == 0) write_trr(i, 1);
    }
  }

  /* ------------------------------------------------------------------- */
  /*  Finalize the output file after all equilibration and production    */
  /*  steps are finished.  This calculates and write the averages to the */
  /*  output file.                                                       */
  /* ------------------------------------------------------------------- */
  finalize_file(hrdf, Nrdfcalls);

  return(0);
}