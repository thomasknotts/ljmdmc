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
/* allocate.c                                                               */
/*                                                                          */
/* This subroutine allocates memory by using the malloc command on          */
/* structures found in defines.h.                                           */
/* ======================================================================== */

#include "includes.h"

int allocate(void)
{
  atom = (struct atom_struct*) calloc(sim.N, sizeof(struct atom_struct));
  if (atom == NULL) { fprintf(stdout, "ERROR: cannot allocate memory for atom\n"); return(11); }
  return(0);
}