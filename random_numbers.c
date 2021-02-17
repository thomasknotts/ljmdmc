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
/* random_numbers.c                                                         */
/*                                                                          */
/*	 This file contains functions that calculate random numbers             */
/* between the range passed to the functions.  One function                 */
/* returns a double and the other function returns an integer.              */
/* The interval does not include the upper limit.  The algorithm            */
/* is from Numberical Recipies.                                             */
/*                                                                          */
/* Before an random number is generated, the generator must be              */
/* initialized by calling it with a negative seed.  Then, each              */
/* call after that must be called with the same positive seed.              */
/* ======================================================================== */


/* =======================Constants============================== */
#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 0.0
#define RNMX 1.0
/* ============================================================= */

/* ------------------------------------------------------------------- */
/*  This function returns a double on the interval [range1, range2)    */
/* ------------------------------------------------------------------- */
double ran_num_double(long idum,double range1,double range2)
{
 int j;
 long k;
 static long idum2=123456789;
 static long iy=0;
 static long iv[NTAB];
 double temp;
 
if (idum <= 0)
 { 
  if (-(idum) < 1) idum=1;
  else idum = -(idum);
  
  idum2=(idum);
 
  for (j=NTAB+7;j>=0;j--)
  {
   k=(idum)/IQ1;
   idum=IA1*(idum-k*IQ1)-IR1*k;
   if (idum < 0) idum += IM1;
   if (j < NTAB) iv[j] = idum;
  }
iy=iv[0];
 }
 
k=(idum)/IQ1;
idum=IA1*(idum-k*IQ1)-IR1*k;
 
if (idum < 0) idum += IM1;
k=idum2/IQ2;
idum2=IA2*(idum2-k*IQ2)-k*IR2;
 
if(idum2 < 0)
 idum2+=IM2;
 
j=iy/NDIV;
iy=iv[j]-idum2;
iv[j]=idum;
if(iy<1)
 iy+=IMM1;
temp=(double)AM*iy;
    
return range1+(range2-range1)*temp;
}
 
/* ------------------------------------------------------------------- */
/*  This function returns a integer on the interval [range1, range2)   */
/* ------------------------------------------------------------------- */
int ran_num_int(double range1, double range2)
{
 return (int)ran_num_double(1,range1,range2);
}
