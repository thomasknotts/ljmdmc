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
/* kinetic.c                                                                */
/*                                                                          */
/* This file contains functions to calculate the kinetic energy of the      */
/* system and the momentum temperature for a given time step.               */
/* ======================================================================== */

#include "includes.h"

/* ------------------------------------------------------------------- */
/*  This function calculates the kinetic energy of the system          */
/* ------------------------------------------------------------------- */
double kinetic_energy(void)
{
	double ke = 0.0;
	double v2;
	unsigned long i;
	for(i=0; i<sim.N; i++)
	{
		v2 = atom[i].vx*atom[i].vx + atom[i].vy*atom[i].vy + atom[i].vz*atom[i].vz;
		ke += 0.5*v2;
	}
	return(ke);
}

/* ------------------------------------------------------------------- */
/*  This function calculates the temperature of the system             */
/* ------------------------------------------------------------------- */
double temperature(double ke)
{
	return (2.0/3.0/(double)sim.N*ke);
}
