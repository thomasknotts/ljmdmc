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
/* scale_velocities.c                                                       */
/*                                                                          */
/* This function scales the velocities to the desired temperature. This     */
/* implements an isokinetic method which should only be used to             */
/* equilibrate an MD simulation to the desired average run temperature.     */
/* It should note be used in production steps as it doesn't generate the    */
/* correct NVT distribution of velocities.                                  */
/* ======================================================================== */

#include "includes.h"

int scale_velocities(double temp)
{
	double scale = sqrt(sim.T/temp);
	unsigned long i;
	for (i=0; i<sim.N; i++)
	{
		atom[i].vx = atom[i].vx*scale;
		atom[i].vy = atom[i].vy*scale;
		atom[i].vz = atom[i].vz*scale;
	}
  return(0);
}
