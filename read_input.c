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
/* read_input.c                                                             */
/*                                                                          */
/* This function reads the input file.                                      */
/* ======================================================================== */

#include "includes.h"

bool readline(char*, int, FILE*);

int read_input(char* fn_i, char* fn_o, char* input_errors)
{
  FILE *fp;
  char buff[MAX_LINE];
  char *token, junk;
  char keyword[64], keyvalue[64];
  char missing[32][64];
  int  Nmissing = 0;
  char *ext;
  bool output_flag, movie_flag, seed_flag, eq_flag, pr_flag;
  bool coord_flag, vel_flag, keyword_flag;
  bool read_flag;
  //size_t len;

  /* ------------------------------------------------------------------- */
  /*  Check to see if file exists and open it if it does                 */
  /* ------------------------------------------------------------------- */
  if (!(fp = fopen(fn_i, "r")))
  {
    fprintf(stdout, "Input file \"%s\" does not exist.\n", fn_i);
    return(ERROR_FILE_NOT_FOUND);
  }

  /* ------------------------------------------------------------------- */
  /*  Read the first line of the file                                    */
  /*  readline will return false if nothing is read or if the end of the */
  /*  file is found.                                                     */
  /* ------------------------------------------------------------------- */
  read_flag = readline(buff, MAX_LINE, fp);
  if (!read_flag)
  {
    fprintf(stdout, "Input file \"%s\" is empty.\n", fn_i);
    return(ERROR_INPUT_FILE);
  }

  /* ------------------------------------------------------------------- */
  /*  Set defaults for optional parameters                               */
  /* ------------------------------------------------------------------- */
  strcpy(sim.inputfile, fn_i);
  strcpy(sim.outputfile, fn_o);
 

  /* ------------------------------------------------------------------- */
  /*  Set initial values for required parameters to act as check flags   */
  /* ------------------------------------------------------------------- */
  strcpy(sim.type, "0");
  sim.N = 0;
  sim.T = 0.0;
  sim.rho = 0.0;
  sim.rc = 0.0;
  sim.seed = 0;
  sim.rdf = 0;
  sim.dt = 0.0;

  eq_flag = false;
  pr_flag = false;
  output_flag = false;
  movie_flag = false;
  seed_flag = false;
  coord_flag = false;
  vel_flag = false;
  keyword_flag = false;

  /* ------------------------------------------------------------------- */
  /*  Parse the line and continue reading the next line until the end of */
  /*  the file is found.                                                 */
  /* ------------------------------------------------------------------- */
  while (read_flag == true)
  {
    /* ============================================ */
    /*  This strtok command will read the first     */
    /*  string that is separated from the next      */
    /*  string by a space or a tab                  */
    /*                                              */
    /*  The strtok command is put in while loop     */
    /*  which keeps reading new lines if the        */
    /*  current line is blank.  If the end of file  */
    /*  is found then the loop breaks. Basically,   */
    /*  this loop simply handles blank lines.       */
    /* ============================================ */

    while (strspn(buff, " \r\n\t") == strlen(buff))
    {
      read_flag = readline(buff, MAX_LINE, fp); //Read the next line in the file.
      if (feof(fp)) break;
    }
    if (feof(fp)) break;
     //if the end of file is found and the last token was NULL
                                          //then the last line was blank.  But you can be and the end of file
                                          //but have a token (on the last line).  That's why you need to 
                                          //check both.
    token = strtok(buff, " \t\n");
    strcpy(keyword, token);

    /* ============================================ */
    /*  Read the next string on the line            */
    /* ============================================ */
    token = strtok(NULL, " \t\n");
    if (token == NULL)
    {
      fprintf(stdout, "There was a problem reading the value for keyword \"%s\"\n", keyword);
      return(ERROR_INPUT_FILE);
    }
    strcpy(keyvalue, token);

    /* ============================================ */
    /*  Copy the key value to the correct variable  */
    /*  This parsing works using a series of        */
    /*  if/else statements to compare the keyword   */
    /*  to those needed by the program.  If the     */
    /*  keyword is not found an error is returned.  */
    /*  Once the correct keyword is found, its      */
    /*  value (keyvalue) is set to the appropriate  */
    /*  simulation variable. When this occurs for   */
    /*  numerical inputs (vs strings), the code     */
    /*  checks to ensure a valid number is entered. */
    /*  If multiple values are needed for a         */
    /*  keyword, these are also read and checked    */
    /*  before the next line is read.               */
    /* ============================================ */

    /* -------------------------------------- */
    /* keyword: sim                           */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    if (!strcmp("sim", keyword))
    {
      if(!strcmp("md",keyvalue) || !strcmp("mc",keyvalue)) strcpy(sim.type, keyvalue);
      else
      {
        fprintf(stdout, "The value of keyword \"sim\" in input file \"%s\" must be either \"md\" or \"mc\".\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
    }

    /* -------------------------------------- */
    /* keyword: coord                         */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("coord", keyword))
    {
      strcpy(sim.icoord, keyvalue);
      coord_flag = true;
    }

    /* -------------------------------------- */
    /* keyword: vel                           */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("vel", keyword))
    {
      strcpy(sim.ivel, keyvalue);
      vel_flag = true;
    }

    /* -------------------------------------- */
    /* keyword: N                             */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("N", keyword)) 
    {
      if (!(sscanf(keyvalue, "%lu%c", &sim.N, &junk) == 1))
      {
        fprintf(stdout, "The value of keyword \"N\" in input file \"%s\" is not a valid number.\n",fn_i);
        return(ERROR_INPUT_FILE);
      }
    } 

    /* -------------------------------------- */
    /* keyword: temp                          */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("temp", keyword))
    {
      if (!(sscanf(keyvalue, "%lf%c", &sim.T, &junk) == 1))
      {
        fprintf(stdout, "The value of keyword \"temp\" in input file \"%s\" is not a valid number.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
    }

    /* -------------------------------------- */
    /* keyword: rho                           */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("rho", keyword))
    {
      if (!(sscanf(keyvalue, "%lf%c", &sim.rho, &junk) == 1))
      {
        fprintf(stdout, "The value of keyword \"rho\" in input file \"%s\" is not a valid number.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
    }

    /* -------------------------------------- */
    /* keyword: esteps                        */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("esteps", keyword))
    {
      if (!(sscanf(keyvalue, "%lu%c", &sim.eq, &junk) == 1))
      {
        fprintf(stdout, "The value of keyword \"esteps\" in input file \"%s\" is not a valid number.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      else eq_flag = true;
    }

    /* -------------------------------------- */
    /* keyword: psteps                        */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("psteps", keyword))
    {
      if (!(sscanf(keyvalue, "%lu%c", &sim.pr, &junk) == 1))
      {
        fprintf(stdout, "The value of keyword \"psteps\" in input file \"%s\" is not a valid number.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      else pr_flag = true;
    }

    /* -------------------------------------- */
    /* keyword: rcut                          */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("rcut", keyword))
    {
      if (!(sscanf(keyvalue, "%lf%c", &sim.rc, &junk) == 1))
      {
        fprintf(stdout, "The value of keyword \"rcut\" in input file \"%s\" is not a valid number.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      else sim.rc2 = sim.rc*sim.rc;
    }

    /* -------------------------------------- */
    /* keyword: dt                            */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("dt", keyword))
    {
      if (!(sscanf(keyvalue, "%lf%c", &sim.dt, &junk) == 1))
      {
        fprintf(stdout, "The value of keyword \"dt\" in input file \"%s\" is not a valid number.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
    }

    /* -------------------------------------- */
    /* keyword: seed                          */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("seed", keyword))
    {
      if (!strcmp("generate", keyvalue))
      {
        strcpy(sim.seedkeyvalue, "generate");
        seed_flag = true;
      }
      else
      {
        if (!(sscanf(keyvalue, "%ld%c", &sim.seed, &junk) == 1))
        {
          fprintf(stdout, "The value of keyword \"seed\" in input file \"%s\" is not valid and must be a negative integer.\n", fn_i);
          return(ERROR_INPUT_FILE);
        }
        if ((int)sim.seed >=0)
        {
          fprintf(stdout, "The value of keyword \"seed\" in input file \"%s\" must be a negative integer.\n", fn_i);
          return(ERROR_INPUT_FILE);
        }
        else
        {
          strcpy(sim.seedkeyvalue, "specified");
          seed_flag = true;
        }
      }
    }

    /* -------------------------------------- */
    /* keyword: output                        */
    /* number of keyvalues required: 1        */
    /* -------------------------------------- */
    else if (!strcmp("output", keyword))
    {
      if (!(sscanf(keyvalue, "%u%c", &sim.output, &junk) == 1))
      {
        fprintf(stdout, "The value of keyword \"output\" in input file \"%s\" is not a valid number.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      else output_flag = true;
    }

    /* -------------------------------------- */
    /* keyword: movie                         */
    /* number of keyvalues required: 2        */
    /* -------------------------------------- */
    else if (!strcmp("movie", keyword))  
    {
      if (sscanf(keyvalue, "%u", &sim.movie) == 1)
      {
        fprintf(stdout, "The keyword \"movie\" must be followed by a file name and an interval in input file \"%s\".\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      strcpy(sim.moviefile, keyvalue);

      //check to see if the file has a .trr extention
      ext = strrchr(sim.moviefile, '.'); //gets the location of the pointer to the .
      if (!ext || strcmp(ext+1,"trr"))   //if no . or not equal to .trr
      {
        fprintf(stdout, "The movie file must have a .trr extention.\n");
        return(ERROR_INPUT_FILE);
      }
      token = strtok(NULL, " \t\n"); //read the next string on the line
      if (token == NULL)
      {
        fprintf(stdout, "No interval was specified for keyword \"movie\" in input file \"%s\"\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      if (!(sscanf(token, "%u%c", &sim.movie, &junk) == 1))
      {
        fprintf(stdout, "The interval of keyword \"movie\" in input file \"%s\" is not a valid.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      movie_flag = true; 
    }

    /* -------------------------------------- */
    /* keyword: rdf                           */
    /* number of keyvalues required: 4        */
    /* -------------------------------------- */
    else if (!strcmp("rdf", keyword))
    {
      // read and check the value for rdfmin
      if (token == NULL)
      {
        fprintf(stdout, "The keyword \"rdf\" in input file \"%s\" is not a valid.\nIt must have four numbers: rmin, rmax, number of bins, and frequency for accumulation.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      if (!(sscanf(token, "%lf%c", &sim.rdfmin, &junk) == 1))
      {
        fprintf(stdout, "The keyword \"rdf\" in input file \"%s\" is not a valid.\nIt must have four numbers: rmin, rmax, number of bins, and frequency for accumulation.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }

      // read and check the value for rdfmax
      token = strtok(NULL, " \t\n"); //read the next string on the line
      if (token == NULL)
      {
        fprintf(stdout, "The keyword \"rdf\" in input file \"%s\" is not a valid.\nIt must have four numbers: rmin, rmax, number of bins, and frequency for accumulation.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      if (!(sscanf(token, "%lf%c", &sim.rdfmax, &junk) == 1))
      {
        fprintf(stdout, "The keyword \"rdf\" in input file \"%s\" is not a valid.\nIt must have four numbers: rmin, rmax, number of bins, and frequency for accumulation.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }

      // read and check the value for rdfN
      token = strtok(NULL, " \t\n"); //read the next string on the line
      if (token == NULL)
      {
        fprintf(stdout, "The keyword \"rdf\" in input file \"%s\" is not a valid.\nIt must have four numbers: rmin, rmax, number of bins, and frequency for accumulation.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      if (!(sscanf(token, "%d%c", &sim.rdfN, &junk) == 1))
      {
        fprintf(stdout, "The keyword \"rdf\" in input file \"%s\" is not a valid.\nIt must have four numbers: rmin, rmax, number of bins, and frequency for accumulation.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }

      // read and check the value for rdfoutput
      token = strtok(NULL, " \t\n"); //read the next string on the line
      if (token == NULL)
      {
        fprintf(stdout, "The keyword \"rdf\" in input file \"%s\" is not a valid.\nIt must have four numbers: rmin, rmax, number of bins, and frequency for accumulation.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
      if (!(sscanf(token, "%u%c", &sim.rdf, &junk) == 1))
      {
        fprintf(stdout, "The keyword \"rdf\" in input file \"%s\" is not a valid.\nIt must have four numbers: rmin, rmax, number of bins, and frequency for accumulation.\n", fn_i);
        return(ERROR_INPUT_FILE);
      }
    }

    /* -------------------------------------- */
    /* keyword is not found                   */
    /* -------------------------------------- */
    else
    {
      if (read_flag) //this if statement is needed to distinguish blank lines from bad keywords
      {
        fprintf(stdout, "Keyword \"%s\" not recognized in input file \"%s\"\n", keyword, fn_i);
        input_errors += sprintf(input_errors, "Keyword \"%s\" not recognized in input file \"%s\"\n", keyword, fn_i);
        keyword_flag = true;
      }
    }
    /* ============================================ */
    /*  Read the next line in the file              */
    /* ============================================ */
    read_flag = readline(buff, MAX_LINE, fp); //Read the next line in the file.
    if (read_flag == feof(fp)) break;
  }

  fclose(fp);

  /* ------------------------------------------------------------------- */
  /*  Check to make sure all 'required' parameters are set in the input  */
  /*  file                                                               */
  /* ------------------------------------------------------------------- */
  if (!(strcmp(sim.type, "0")))
  {
    strcpy(missing[Nmissing], "sim");
    Nmissing++;
  }

  if (sim.N == 0)
  {
    strcpy(missing[Nmissing], "N");
    Nmissing++;
  }

  if (!eq_flag)
  {
    strcpy(missing[Nmissing], "esteps");
    Nmissing++;
  }

  if (!pr_flag)
  {
    strcpy(missing[Nmissing], "psteps");
    Nmissing++;
  }

  if (sim.rho == 0)
  {
    strcpy(missing[Nmissing], "rho");
    Nmissing++;
  }

  if (sim.T == 0)
  {
    strcpy(missing[Nmissing], "temp");
    Nmissing++;
  }

  if (sim.rc == 0)
  {
    strcpy(missing[Nmissing], "rcut");
    Nmissing++;
  }

  if (sim.dt == 0)
  {
    strcpy(missing[Nmissing], "dt");
    Nmissing++;
  }

  if (Nmissing > 0)
  {
    fprintf(stdout, "The following required parameters were missing from input file \"%s\"\n", fn_i);
    for (int i = 0; i < Nmissing; i++) fprintf(stdout, "%s\n", missing[i]);
    fprintf(stdout, "\n");
    return(ERROR_INPUT_FILE);
  }

  if (!(output_flag) || !(movie_flag) || !(seed_flag) || !(coord_flag) || !(vel_flag))
  {
    if (keyword_flag)
    {
      input_errors += sprintf(input_errors, "\n");
    }
    if (!output_flag)
    {
      input_errors += sprintf(input_errors, "The output interval was not specified in input file \"%s\".\nA default value of 2000 will be used.\n\n", fn_i);
      sim.output = 2000;
    }

    if (!movie_flag)
    {
      input_errors += sprintf(input_errors, "The movie interval was not specified in input file \"%s\".\nNo movie file will be produced.\n\n", fn_i);
      sim.movie = 0;
    }
    if (!seed_flag)
    {
      //input_errors += sprintf(input_errors, "The seed for the random number generator was not specified in input file \"%s\".\nThe default value of -827165783 will be used.\n\n", fn_i);
      sim.seed = -827165783;
      strcpy(sim.seedkeyvalue, "default");
    }

    if (!coord_flag)
    {
      input_errors += sprintf(input_errors, "The initial coordinates were not specified in input file \"%s\".\nThe default value of \"generate\" will be used.\n\n", fn_i);
      strcpy(sim.icoord, "generate");
    }

    if (!strcmp(sim.type, "md"))
    {
      if (!vel_flag)
      {
        input_errors += sprintf(input_errors, "The initial velocities were not specified in input file \"%s\".\nThe default value of \"generate\" will be used.\n\n", fn_i);
        strcpy(sim.ivel, "generate");
      }
    }
  }

  return(0);
}