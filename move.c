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
/* move.c                                                                   */
/*                                                                          */
/* This function generates a random displacement on a random particle,      */
/* calculates the new energy, and accepts or rejects the new position       */
/* according to the Metropolis criterion.  It is the main propogation       */
/* subroutine for an MC simulation.                                         */
/* ======================================================================== */

#include "includes.h"


int    ran_num_int(double range1, double range2);
double ran_num_double(long idum, double range1, double range2);
double forces(void);
double atomic_pe(unsigned long, double, double, double);

bool move(void)
{
	double xnew, ynew, znew;
  double peold, penew, de;
	unsigned long particle;
	
  /* ------------------------------------------------------------------- */
  /*  Select a random particle and propose a random move                 */
  /* ------------------------------------------------------------------- */
	iprop.ntrys += 1;
  particle = ran_num_int(0.0, (double)sim.N);
	xnew = atom[particle].x + ran_num_double(1, -1, 1)*sim.dt;
	ynew = atom[particle].y + ran_num_double(1, -1, 1)*sim.dt;
	znew = atom[particle].z + ran_num_double(1, -1, 1)*sim.dt;

  /* ------------------------------------------------------------------- */
  /*  Apply Periodic Boundary Conditions                                 */
  /* ------------------------------------------------------------------- */
	if(xnew<0)
		xnew += sim.length;
	else if(xnew > sim.length)
		xnew -= sim.length;

	if(ynew<0)
		ynew += sim.length;
	else if(ynew > sim.length)
		ynew -= sim.length;
		
	if(znew<0)
		znew += sim.length;
	else if(znew > sim.length)
		znew -= sim.length;

  /* ------------------------------------------------------------------- */
  /*  Calculate the new and old energies                                 */
  /* ------------------------------------------------------------------- */
  peold = atomic_pe(particle, atom[particle].x, atom[particle].y, atom[particle].z);
  penew = atomic_pe(particle, xnew, ynew, znew);

  /* ------------------------------------------------------------------- */
	/*  Accept/Reject the move                                             */
  /* ------------------------------------------------------------------- */
  de = penew - peold;
  if (ran_num_double(1, 0, 1) < (exp(-de / sim.T)))
  {
    iprop.naccept += 1;
    iprop.pe = forces();  //updates the force vectors and assigns new pe
    iprop.pe2 = iprop.pe * iprop.pe;
    atom[particle].x = xnew;
    atom[particle].y = ynew;
    atom[particle].z = znew;
    return(true);
  }
  else return(false);

}