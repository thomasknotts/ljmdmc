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
/* verlet.c                                                                 */
/*                                                                          */
/* This file containes the two functions to integrate the equations of      */
/* motion using the velocity verlet algorithm.                              */
/* ======================================================================== */

#include "includes.h"

/* ------------------------------------------------------------------- */
/* This function is the first needed to use the velocity verlet        */
/* algorithm.  It uses the data at time step t to update the positions */
/* to the next time step and the velocities to the next half time      */
/* step.                                                               */
/* ------------------------------------------------------------------- */
int verlet1(void)
{
	unsigned long i;
  double dx, dy, dz;
	
  for(i=0; i<sim.N; i++)
	{
    /* ------------------------------------------------------------------- */
    /*  Update the positions to a full time step                           */
    /* ------------------------------------------------------------------- */
		dx = sim.dt*atom[i].vx + sim.dt*sim.dt*atom[i].fx / 2.0;
    dy = sim.dt*atom[i].vy + sim.dt*sim.dt*atom[i].fy / 2.0;
    dz = sim.dt*atom[i].vz + sim.dt*sim.dt*atom[i].fz / 2.0;
    atom[i].x = atom[i].x+dx;
		atom[i].y = atom[i].y+dy;
		atom[i].z = atom[i].z+dz;
    atom[i].dx += dx; //displacement accumulator for diffusivity 
    atom[i].dy += dy; //displacement accumulator for diffusivity
    atom[i].dz += dz; //displacement accumulator for diffusivity

    /* ------------------------------------------------------------------- */
    /*  Apply Periodic Boundary Conditions                                 */
    /* ------------------------------------------------------------------- */
		if(atom[i].x<0)
			atom[i].x += sim.length;
		else if(atom[i].x > sim.length)
			atom[i].x -= sim.length;

		if(atom[i].y<0)
			atom[i].y += sim.length;
		else if(atom[i].y > sim.length)
			atom[i].y -= sim.length;
		
		if(atom[i].z<0)
			atom[i].z += sim.length;
		else if(atom[i].z > sim.length)
			atom[i].z -= sim.length;

    /* ------------------------------------------------------------------- */
    /*  Update the velocities to half a time step                          */
    /* ------------------------------------------------------------------- */
		atom[i].vx = atom[i].vx+sim.dt*atom[i].fx/2.0;
		atom[i].vy = atom[i].vy+sim.dt*atom[i].fy/2.0;
		atom[i].vz = atom[i].vz+sim.dt*atom[i].fz/2.0;
	}
  return(0);
}


/* ------------------------------------------------------------------- */
/* This function is the second needed to use the velocity verlet       */
/* algorithm.  It updates the velocites from the half time step to the */
/* full time step.                                                     */
/* ------------------------------------------------------------------- */
int verlet2(void)
{
	unsigned long i;
	for(i=0; i<sim.N;i++)
	{
		atom[i].vx = atom[i].vx+sim.dt*atom[i].fx/2.0;
		atom[i].vy = atom[i].vy+sim.dt*atom[i].fy/2.0;
		atom[i].vz = atom[i].vz+sim.dt*atom[i].fz/2.0;
	}
  return(0);
}
