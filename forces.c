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
/* forces.c                                                                 */
/*                                                                          */
/* This function calculates the energies and forces between each Lennard    */
/* Jones particle.  It returns the potential energy.                        */
/* ======================================================================== */

#include "includes.h"

double forces(void)
{
  double dr2, d2, d4, d8, d14;
	double dx, dy, dz;
	double fr;
	double virial = 0.0;
	double pe = 0.0;
	unsigned long i,j;

  /* ------------------------------------------------------------------- */
  /*  Zero out the force accumulators                                    */
  /* ------------------------------------------------------------------- */
	for(i=0; i<sim.N; i++)
	{
		atom[i].fx = 0.0;
		atom[i].fy = 0.0;
		atom[i].fz = 0.0;
	}
	
  /* ------------------------------------------------------------------- */
  /*  Calculate the forces by looping over all pairs of sites            */
  /* ------------------------------------------------------------------- */
	for(i=0; i<sim.N-1; i++)
	{
		for(j=i+1; j<sim.N; j++)
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

      dr2 = dx*dx + dy*dy + dz*dz;

      /* ============================================ */
      /*         Distance and Energy Calculation      */
      /* ============================================ */
			if (dr2 < sim.rc2)
			{
        d2 = 1.0 / dr2;
				d4 = d2*d2;
				d8 = d4*d4;
				d14 = d8*d4*d2;
				fr = 48.0*(d14-0.5*d8);
				
				//components of forces
				atom[i].fx += fr*dx;
				atom[i].fy += fr*dy;
				atom[i].fz += fr*dz;
				atom[j].fx -= fr*dx;
				atom[j].fy -= fr*dy;
				atom[j].fz -= fr*dz;
			
				//viral and potential energy
				virial += dr2*fr;
				pe  += 4.0*(d14-d8)*dr2;

			}//if for sim.rc2 
		}// j
	}// i

   /* ------------------------------------------------------------------- */
   /*  Assign the instantaneous virial value                              */
   /* ------------------------------------------------------------------- */
	iprop.virial = virial;

  return(pe);
}	





