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
/* defines.h                                                                */
/*                                                                          */
/* This file contains the MACROS and global variables for the program.   It */
/* also defines any global structure.  It is included in the program        */
/* through the header files "includes.h".                                   */
/* ======================================================================== */

/* ------------------------------------------------------------------- */
/*  MACROS                                                             */
/* ------------------------------------------------------------------- */
#define ERROR_ARGUMENTS 100
#define ERROR_FILE_NOT_FOUND 101
#define ERROR_INPUT_FILE 102
#define ERROR_LINEAR_MOMENTUM 200
#define MAX_LINE 1024
#define _CRT_SECURE_NO_WARNINGS

#define PI 3.14159265359

/* ------------------------------------------------------------------- */
/*  This structure contains information on the simulation as read      */
/*  from the input file specified by the user.                         */
/* ------------------------------------------------------------------- */
#ifndef MAIN
extern
#endif
struct sim_struct {
  char            type[4];              /* ensemble id mode                     */
  double          T;                    /* temperature [T*]                     */
  double          rho;                  /* density [rho*]                       */
  unsigned long   N;                    /* number of particles                  */
  unsigned long   eq;                   /* equilibration steps                  */
  unsigned long   pr;                   /* production steps                     */
  unsigned long	  itrr;			            /* interval for saving trajectory       */
  double          rc;                   /* cutoff radius [r*]                   */
  double          rc2;                  /* square of cutoff radius [r*]         */
  double          dt;                   /* time step for md or max delta for mc */
  char            icoord[128];          /* initial coordinates                  */
  char            ivel[128];            /* initial velocities                   */
  char            inputfile[128];       /* name of input file                   */
  char            outputfile[128];      /* name of output file                  */
  double          length;               /* length of simulation box             */
  unsigned int    output;               /* interval for output of instan. props */
  unsigned int    movie;                /* interval for movie frames            */
  char            moviefile[128];       /* name of movie file                   */
  double          utail;                /* tail correction to energy            */
  double          ptail;                /* tail correction to pressure          */
  long            seed;                 /* seed to the random number generator  */
  char            seedkeyvalue[128];    /* keyvalue for seed                    */
  double          rdfmin;               /* minimum r value for rdf              */
  double          rdfmax;               /* maximum r value for rdf              */
  int             rdfN;                 /* number of bins for rdf               */
  unsigned int    rdf;                  /* frequency to accumulate the rdf      */
} sim;

/* ------------------------------------------------------------------- */
/*  This structure contains information on the properties of each atom */
/* ------------------------------------------------------------------- */
#ifndef MAIN
extern
#endif
struct atom_struct {
  double          x;                    /* x position                           */
  double          y;                    /* y position                           */
  double          z;                    /* z position                           */
  double          vx;                   /* vx x velocity                        */
  double          vy;                   /* vy y velocity                        */
  double          vz;                   /* vz z velocity                        */
  double          fx;                   /* ax x acceleration                    */
  double          fy;                   /* ay y acceleration                    */
  double          fz;                   /* az z acceleration                    */
  double          dr2;                  /* MSD accumulator for diffusion (MD)   */
  double          dx;                   /* x displacment for diffusion (MD)     */
  double          dy;                   /* y displacment for diffusion (MD)     */
  double          dz;                   /* z displacment for diffusion (MD)     */
} *atom;

/* ------------------------------------------------------------------- */
/*  This structure contains information on the simulation properties   */
/* ------------------------------------------------------------------- */
#ifndef MAIN
extern
#endif
struct props_struct {
  double          ke;                   /* kinetic energy              */
  double          pe;                   /* potential energy            */
  double          pe2;                  /* squared potential energy    */
  double          T;                    /* temperature                 */
  double          virial;               /* virial for pressure         */
  unsigned long   naccept;              /* number of mc moves accepted */
  unsigned long   ntrys;                /* number of mc moves tried    */
  unsigned long   Nhist;
} iprop, aprop;


