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
/* untils.c                                                                 */
/*                                                                          */
/* This functions and subroutines in this file are small functions called   */
/* multiple times in other subroutines.                                     */
/* ======================================================================== */

# include "includes.h"


/* ------------------------------------------------------------------- */
/* readline                                                            */
/* This function is used to ensure the entire line is read by fgets.   */
/* It returns a true or false.  True if the read is successful, false  */
/* if not.  fgets by itself will not read the entire line if the       */
/* number of characters on the line is greater than the size of the    */
/* variable 'buffer'.                                                  */
/* ------------------------------------------------------------------- */
bool readline(char *buffer, int size, FILE *fp)
{
  if (fgets(buffer, size, fp)) {
    size_t len = strlen(buffer);
    return feof(fp) || (len != 0 && buffer[len - 1] == '\n');
  }
  return false;
}

/* ------------------------------------------------------------------- */
/* error_exit                                                          */
/* This function is used make sure the code pauses in interactive mode */
/* when errors are encountered. This is mainly used for debugging.  If */
/* running a batch job with no user interface, the printf and getchar  */
/* lines should be commented out.                                      */
/* ------------------------------------------------------------------- */
int error_exit(int error_code)
{
  //printf("Error Code: %d\nPress enter to continue...\n", error_code);
  //getchar();
  exit(error_code);
}
