#  Copyright (C) 2017 Thomas Allen Knotts IV - All Rights Reserved           #
#  This file is part of the program ljmcmd                                   #
#                                                                            #
#  ljmcmd is free software: you can redistribute it and/or modify            #
#  it under the terms of the GNU General Public License as published by      #
#  the Free Software Foundation, either version 3 of the License, or         #
#  (at your option) any later version.                                       #
#                                                                            #
#  ljmcmd is distributed in the hope that it will be useful,                 #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of            #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
#  GNU General Public License for more details.                              #
#                                                                            #
#  You should have received a copy of the GNU General Public License         #
#  along with ljmcmd.  If not, see <http://www.gnu.org/licenses/>.           #

#  ========================================================================  #
#  makefile                                                                  #
#                                                                            #
#  This file may be used to compile the ljmdmc code on linux with gcc.       #
#  type "make" to compile the program.                                       #
#  type "make clean" to remove the object files and executable.              #
#  ========================================================================  #

#-----------------------------------------------------------------------------
# Name of the executable
#-----------------------------------------------------------------------------

EXEC = ljmdmc

#-----------------------------------------------------------------------------
# Select a compiler and options to use (only select one CC and one CFLAGS)
#-----------------------------------------------------------------------------

# Using gcc
CC     = gcc
CFLAGS = -O3 -std=c99 -Wall

# Debugging with gcc
#CC     = gcc
#CFLAGS = -Wall -std=c99 -g

# Using icc
#CC	= icc
#CFLAGS = -O3 -fp-model precise -axCORE-AVX2 -xAVX -std=c99

#-----------------------------------------------------------------------------
# Library linking (this should always be uncommented)
#-----------------------------------------------------------------------------
LIBS = -lm

#-----------------------------------------------------------------------------
# Optional library file selection (remove the "#" before each desired option)
#-----------------------------------------------------------------------------
# Add optional libraries (remove the "#" before each desired option)
# LIBS += -efence # memory debuging

#=============================================================================
#-----------------------------------------------------------------------------
# C Source files to include (Nothing should be changed here.)
#-----------------------------------------------------------------------------

SRCS = allocate.c atomic_pe.c finalize_file.c forces.c                       \
       initialize_counters.c initialize_files.c                              \
       initialize_positions.c initialize_velocities.c                        \
       kinetic.c main.c momentum_correct.c move.c nvemd.c                    \
       nvtmc.c random_numbers.c rdf.c read_input.c scale_delta.c             \
       scale_velocities.c tak_histogram.c utils.c verlet.c write_trr.c            

#-----------------------------------------------------------------------------
# Compiling Commands (Nothing should be changed here.)
#-----------------------------------------------------------------------------

OBJS = ${SRCS:.c=.o}

all: $(EXEC)

%.o: %.c
	${CC} ${CFLAGS} ${DFLAGS} -D_XOPEN_SOURCE=500 ${INCL} -c  $< -o $@

$(EXEC):  ${OBJS}
	$(CC) ${CFLAGS} ${DFLAGS} -D_XOPEN_SOURCE=500 ${INCL} -o $@ ${OBJS} $(LIBS)
	echo $(EXEC)

clean:
	rm -f *.o
	rm -f $(EXEC)
