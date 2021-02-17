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
/* nvtmc.c                                                                  */
/*                                                                          */
/* This subroutine is the main driver for NVT MC simulations.               */
/* ======================================================================== */

#include "includes.h"

bool   move(void);
int    rdf_accumulate(tak_histogram*);
int    finalize_file(tak_histogram*, double);
double forces(void);
void   write_trr(unsigned long, int);
void   scale_delta(void);

int nvtmc()
{
  unsigned long i, j;
  double P;
  FILE *fp;
  aprop.Nhist = 0;
  double Nrdfcalls;
  tak_histogram *hrdf = NULL;

  /* ------------------------------------------------------------------- */
  /*  Perform equilibration steps                                        */
  /* ------------------------------------------------------------------- */
  for (i = 1; i <= sim.eq; i++)
  {
    for (j = 0; j < sim.N; j++) // This loop performs sim.N moves per interation
    {
      move();

      /* ============================================ */
      /*  Accumulate the properties for the step      */
      /* ============================================ */
      aprop.pe += iprop.pe;
      aprop.virial += iprop.virial;
      aprop.pe2 += iprop.pe2;
    }

    /* ============================================ */
    /*  Output equilibration progress at            */
    /*  the interval specified in the input file    */
    /* ============================================ */
    if (i%sim.output == 0)
    {
      P = sim.rho*sim.T + 1.0 / 3.0 / pow(sim.length, 3.0)*aprop.virial / (double)(i)/(double)(sim.N) + sim.ptail;
      fp = fopen(sim.outputfile, "a");
      fprintf(fp, "%-13lu\t%13.6lf\t%13.6lf\n", i, P, iprop.pe / (double)sim.N + sim.utail);
      fflush(fp);
      fclose(fp);
      fprintf(stdout, "Equilibrium Step %-lu\n", i);
    }

    /* ============================================ */
    /*  Scale delta to obtain 30% acceptance        */
    /* ============================================ */
    if (i % 100 == 0) scale_delta();

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
  iprop.ntrys = 0;
  iprop.naccept = 0;
  aprop.pe = 0.0;
  aprop.virial = 0.0;
  aprop.pe2 = 0.0;

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
    for (j = 0; j < sim.N; j++) // This loop performs sim.N moves per interation (one MC sweep)
    {
      move(); 

	  /* ============================================ */
	  /*  Accumulate the properties for the step      */
	  /* ============================================ */
      aprop.pe += iprop.pe;
      aprop.virial += iprop.virial;
      aprop.pe2 += iprop.pe2;

      if (sim.rdf)//accumulate the rdf if specified in the input file (production steps only)
      {
        if (i%sim.rdf == 0)
        {
          Nrdfcalls += 1;
          rdf_accumulate(hrdf);
        }
      }
    }

    /* ============================================ */
    /*  Output instantaneous properties at          */
    /*  the interval specified in the input file    */
    /* ============================================ */
    if (i%sim.output == 0)
    {
      P = sim.rho*sim.T + 1.0 / 3.0 / pow(sim.length, 3.0)*aprop.virial / (double)(i)/(double)(sim.N) + sim.ptail;
      fp = fopen(sim.outputfile, "a");
      fprintf(fp, "%-13lu\t%13.6lf\t%13.6lf\n", i, P, iprop.pe / (double)sim.N + sim.utail);
      fflush(fp);
      fclose(fp);
      fprintf(stdout, "Production Step %-lu\n", i);
    }

    /* ============================================ */
    /*  Scale delta to obtain 30% acceptance        */
    /* ============================================ */
    if (i % 100 == 0) scale_delta();                

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
