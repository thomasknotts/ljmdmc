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
/* scale_delta.c                                                            */
/*                                                                          */
/* This function adjusts the maximum displacement for the MC                */
/* simulation to obtain an acceptance ratio of 30%.                         */
/* ======================================================================== */

#include "includes.h"

int scale_delta(void)
{
	double ratio;
	
	ratio = ((double)iprop.naccept)/((double)iprop.ntrys);
	
	if (sim.dt < 2.0) 
	{
		if ((ratio < .28) || (ratio > .32))
		{
			if (ratio<0.3) 	
				sim.dt = sim.dt*.95;
			if (ratio >0.3)
				sim.dt = sim.dt*1.05;
		}
	}

  aprop.naccept += iprop.naccept;
  aprop.ntrys += iprop.ntrys;
	iprop.naccept = 0;
	iprop.ntrys = 0;

  return(0);
}
