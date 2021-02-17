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
/* atomic_pe.c                                                              */
/*                                                                          */
/* This function calculates the potential energy of each atom               */
/* with each of its neighbors.  It is used in MC to hold the                */
/* "old" energies for use in the metropolis criterion.                      */
/* ======================================================================== */

#include "includes.h"

double atomic_pe(unsigned long particle, double x, double y, double z)
{
	unsigned long i;
	double drx, dry, drz;
	double u;
	double dr2, dr4, dr6, dr12, dr;

  /* ------------------------------------------------------------------- */
  /*  Calculate the energy of the new configuration                      */
  /* ------------------------------------------------------------------- */
	u = 0.0;
	for (i = 0; i < sim.N; i++)
	{
		if (particle != i)
		{
			drx = atom[i].x - x;
			dry = atom[i].y - y;
			drz = atom[i].z - z;

			/* ============================================ */
			/*         Minimum Image Convention             */
			/* ============================================ */
			if (fabs(drx) > (sim.length*0.5))
			{
				if (drx < 0.0)
					drx += sim.length;
				else
					drx -= sim.length;
			}
			if (fabs(dry)>(sim.length*0.5))
			{
				if (dry < 0.0)
					dry += sim.length;
				else
					dry -= sim.length;
			}
			if (fabs(drz)>(sim.length*0.5))
			{
				if (drz < 0.0)
					drz += sim.length;
				else
					drz -= sim.length;
			}

			/* ============================================ */
			/*         Distance and Energy Calculation      */
			/* ============================================ */
			dr = drx*drx + dry*dry + drz*drz;
			if (dr < sim.rc2)
			{
				dr2 = 1 / dr;
				dr4 = dr2*dr2;
				dr6 = dr4*dr2;
				dr12 = dr6*dr6;

				u += 4.0*(dr12 - dr6);
			}//if  rcut
		}//if particle != i
	}//for i

	return(u);
}