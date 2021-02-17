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
/* initialize_counters.c                                                    */
/*                                                                          */
/* This subroutine intializes the accumulators used to calculate the        */
/* the averages of the properties calculated during simulation. It also     */
/* calculates the corrections to the energy and pressure from cutting the   */
/* potential.                                                               */
/*                                                                          */
/* ======================================================================== */

#include "includes.h"

int initialize_counters(void)
{

  /* ------------------------------------------------------------------- */
  /* Calculate the accumulators for the average properties               */
  /* ------------------------------------------------------------------- */
  aprop.T = 0.0;
  aprop.pe = 0.0;
  aprop.pe2 = 0.0;
  aprop.ke = 0.0;
  aprop.virial = 0.0;
  if (!strcmp(sim.type, "mc"))
  {
    iprop.ntrys = 0;
    iprop.naccept = 0;
    aprop.ntrys = 0;
    aprop.naccept = 0;
  }

  /* ------------------------------------------------------------------- */
  /* Initialize the squared displacement accumulator.                    */
  /* ------------------------------------------------------------------- */
  for (unsigned long i = 0; i < sim.N; i++)
  {
    atom[i].dr2 = 0.0;
    atom[i].dx = 0.0;
    atom[i].dy = 0.0;
    atom[i].dz = 0.0;
  }

  /* ------------------------------------------------------------------- */
  /* Initialize the rdf histogram.                                       */
  /* ------------------------------------------------------------------- */
  if (sim.rdf)
  {
    if (sim.rdfmax > sim.length / 2.0)
    {
      fprintf(stdout, "The max length of the rdf cannot be greater than half the box length.\n");
      exit(10);
    }
    tak_histogram* rdf;
    rdf = tak_histogram_calloc_uniform(sim.rdfN, sim.rdfmin, sim.rdfmax);
    if (!rdf)
    {
      fprintf(stdout, "The histogram for the rdf could not be allocated.");
      exit(11);
    }
  }

  /* ------------------------------------------------------------------- */
  /* Calculate the correction to the potential energy and pressure       */
  /* ------------------------------------------------------------------- */
  sim.utail = 8.0 / 3.0*PI*sim.rho*(1.0 / 3.0*pow(sim.rc, -9.0) - pow(sim.rc, -3.0));
  sim.ptail = 16.0 / 3.0*PI*sim.rho*sim.rho*(2.0 / 3.0*pow(sim.rc, -9.0) - pow(sim.rc, -3.0));

  return(0);
}