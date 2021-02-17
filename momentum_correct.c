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
/* momentum_correct.c                                                       */
/*                                                                          */
/* This file contains two functions.  The first checks to see if the linear */
/* momentum of the box is zero.  The second adjusts the velocities to       */
/* ensure that the linear momentum is zero.                                 */
/* ======================================================================== */

#include "includes.h"

/* ------------------------------------------------------------------- */
/*  This function checks to see if the linear momentum is zero         */
/* ------------------------------------------------------------------- */
int check_momentum(void)
{
	unsigned long i;
	double vcumx, vcumy, vcumz;
	vcumx = 0.0;
	vcumy = 0.0;
	vcumz = 0.0;
	for (i = 0; i<sim.N; i++)
	{
		vcumx += atom[i].vx;
		vcumy += atom[i].vy;
		vcumz += atom[i].vz;
	}
	//printf("These numbers should be zero if linear momentum is zero.\n");
	//printf("->%lf\n->%lf\n->%lf\n",vcumx,vcumy,vcumz);
	if(vcumx+vcumy+vcumz<pow(10.0,-10.0))
		return(0);
  else
  {
  //printf("These numbers should be zero if linear momentum is zero.\n");
  //printf("->%lf\n->%lf\n->%lf\n",vcumx,vcumy,vcumz);
		return(ERROR_LINEAR_MOMENTUM);
  }

}

/* ------------------------------------------------------------------- */
/* This function sets the linear momentum to zero                      */
/* ------------------------------------------------------------------- */
int zero_momentum(void)
{
  unsigned long i;
	double vcumx, vcumy, vcumz;
	vcumx = 0.0;
	vcumy = 0.0;
	vcumz = 0.0;
	for (i = 0; i<sim.N; i++)
	{
		vcumx += atom[i].vx;
		vcumy += atom[i].vy;
		vcumz += atom[i].vz;
	}

	vcumx = vcumx/(double)sim.N;
	vcumy = vcumy/(double)sim.N;
	vcumz = vcumz/(double)sim.N;

	for (i=0; i<sim.N; i++)
	{
		atom[i].vx -= vcumx;
		atom[i].vy -= vcumy;
		atom[i].vz -= vcumz;
	}
  if (!check_momentum()) return 0;
  else return(1);
}
