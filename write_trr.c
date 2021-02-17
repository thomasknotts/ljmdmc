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
/* write_trr.c                                                             	*/   
/*                                                                          */
/* Based on Trio. This subroutine writes positions, velocities, and forces  */
/* to a traj.trr file which can be read and	analyzed using Gromacs or VMD.  */
/*    cycle = the current iteration number                                  */
/*    flag = 0 for equilibration and 1 for production                       */
/* ======================================================================== */

#include "includes.h"

int reverse=1;									//set to 0 to turn off byte swapping
int precision=8;								//set to 4 to use float precision
FILE *das;
int write_int(long);
int write_float(float fp);
int write_bstring(char *str);
int write_vector(float *fp);
int strip_white(char *str);
void swap4(void *n);
void swap8(void *n);

int FLAG_x = 1;
int FLAG_v = 0;
int FLAG_f = 0;

void write_trr(unsigned long cycle, int flag) {
long ir_size, e_size, vir_size, pres_size, top_size, sym_size, nre;
long box_size, x_size, v_size, f_size;
float lambda;
long MAGIC;
long VERSION;
long time_ind; float time_val;
float bx[9];
unsigned long i;
float pos[3];
float vel[3];
char title[2]={""};
float force[3];

das=fopen(sim.moviefile,"ab");

//setting some variables gromacs will look for.  no clue what they do.
ir_size=0; e_size=0; vir_size=0; pres_size=0; top_size=0; sym_size=0; nre=0;

//size of array to store vectors that define the simulation box
box_size=precision*9;
x_size =0; v_size = 0; f_size=0;
//size of position, velocity, and force sections
if (FLAG_x !=0) x_size=precision*3*sim.N;
if (FLAG_v !=0) v_size=precision*3*sim.N;
if (FLAG_f !=0) f_size=precision*3*sim.N;

//no clue what these do
lambda = 0.0;
MAGIC=1993;

//version
VERSION = 13;

if (flag==0) {
	time_val = (float) (sim.dt * 1.0*cycle);
	time_ind = cycle;
}
else {
	time_val = (float) (sim.dt * 1.0*(sim.eq+cycle));
	time_ind = sim.eq+cycle;
}

write_int(MAGIC);
write_int(VERSION);

write_bstring(title);

write_int(ir_size);
write_int(e_size);
write_int(box_size);
write_int(vir_size);
write_int(pres_size);
write_int(top_size);
write_int(sym_size);
write_int(x_size);
write_int(v_size);
write_int(f_size);
write_int(sim.N);
write_int(time_ind);
write_int(nre);

write_float(time_val);
write_float(lambda);

//writes the array for  box dimensions

for (i=0; i<9; i++)
	bx[i]=0.0;

bx[0]=(float)(sim.length/10.0);
bx[4]=(float)(sim.length/10.0);
bx[8]=(float)(sim.length/10.0);

write_vector(&bx[0]);
write_vector(&bx[3]);
write_vector(&bx[6]);

//writes the array for positions
//converts units to nm

//printf("atom posits\n");
if (FLAG_x !=0) {
	for (i=0; i<sim.N; i++) {
		pos[0]=(float)(atom[i].x/10.0);				// figure out with or w/o pbc
		pos[1]=(float)(atom[i].y/10.0);
		pos[2]=(float)(atom[i].z/10.0);
		write_vector(pos);
	}
}

//writes velocities

//printf("atom velocities\n");
if (FLAG_v !=0) {
	for (i=0; i<sim.N; i++) {
		vel[0]=(float)(atom[i].vx/10.0);
		vel[1]=(float)(atom[i].vy/10.0);
		vel[2]=(float)(atom[i].vz/10.0);
		write_vector(vel);
	}
}

//writes forces

//printf("atom forces\n");
if (FLAG_f !=0) {
	for (i=0; i<sim.N; i++) {
		force[0]=(float)(atom[i].fx/10.0);
		force[1]=(float)(atom[i].fy/10.0);
		force[2]=(float)(atom[i].fz/10.0);
		write_vector(force);
	//	printf("%lf %lf %lf\n",force[0],force[1],force[2]);
	}
}

fclose(das);
}



int write_int(long i) {
	long si;

	if(!reverse) {
		if (fwrite(&i,4,1,das)!=1) 
			return 0;
		else
			return 1;
	}
	else {
		si = i;
		swap4((void *) &si);
		if (fwrite(&si,4,1,das)!=1)
			return 0;
		else
			return 1;
	}
}


int write_float(float fp) {
	if (precision==4) {
		float sfp;
		sfp=fp;
		if (reverse)
			swap4((void *) &sfp);
		if(fwrite(&sfp,precision,1,das)!=1)
			return 0;
		else 
			return 1;
	}

	else {
		double sfp;
		sfp = (double) fp;
		if (reverse)
			swap8((void *) &sfp);

		if(fwrite(&sfp,precision,1,das)!=1)
			return 0;
		else
			return 1;
	}
}


int write_vector(float *fp) {
	if ( (!write_float(fp[0])) || (!write_float(fp[1])) ||
		(!write_float(fp[2])) ) {
		printf("failed to write vector!\n");
		exit(1);
	}
	else 
		return 1;
}


int write_bstring(char *str) {
//	printf("length: %d\nstring:\n%s\n",strlen(str),str);

	if(!write_int(strlen(str))) {
		fprintf(stdout,"failed to write string length!\n");
		exit(1);
	}
	
	if (fwrite(str,1,strlen(str),das)!=1) 
		return 0;
	else 
		return -1;
}


void swap4(void *n) {
	char temp[2];
	char *c = (char *) n;
	temp[0] = c[3];
	temp[1] = c[2];
	c[3] = c[0];
	c[2] = c[1];
	c[1] = temp[1];
	c[0] = temp[0];
	return;
}

void swap8(void *n) {
	char temp[4];
	char *c = (char *) n;
	temp[0]=c[7];
	temp[1]=c[6];
	temp[2]=c[5];
	temp[3]=c[4];
	c[7]=c[0];
	c[6]=c[1];
	c[5]=c[2];
	c[4]=c[3];
	c[3]=temp[3];
	c[2]=temp[2];
	c[1]=temp[1];
	c[0]=temp[0];
	return;
}



//strips leading and trailing white space from a string
int strip_white(char *buf) {
	int i,j,k;
	/* check arguments */
	if (!buf) return -1;
	if (!strlen(buf)) return -1;

	//ditches trailing white space
	for (i=strlen(buf)-1; buf[i]==' ' || buf[i]=='\t' || buf[i]=='\n' || buf[i]=='\r'; i--)
		buf[i]=0;

	//leading white space
	for (i=0;  buf[i]==' ' || buf[i]=='\t' || buf[i]=='\n' || buf[i]=='\r'; i++);
	if(i) {
		k=0;
		for (j=i; buf[j]; j++)
			buf[k++]=buf[j];
		
		buf[k]=0;
	}
	return strlen(buf);

}


