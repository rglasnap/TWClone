/*
Copyright (C) 2002 Scott Long (link@kansastubacrew.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/* Modification History **
**************************
** 
** LAST MODIFICATION DATE: 08 June 2002
** Author: Rick Dearman
** 1) Modified all defined items to allow them to be user defined instead. 
**    With one exception which was the MAXJUMPPERCENT which was caused problems
**    with other defined items in the universe.h file. 
**
** 2) Modified all comments from // to C comments in case a users complier isn't C99 
**    complilant. (like some older Sun or HP compilers)
**
** 3) Added random name generation for the ports.
** 
** 4) Added consellation names for sectors.
**
** 5) Added randomly placed Ferringhi sector.
**
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "config.h"
#include "universe.h"


/*  This is the max length of tunnels and dead ends. */
#define MAXTUNLEN		6
int maxTunnel = MAXTUNLEN;

/*  Variables that will eventually be inputted by the user when this program */
/*  is initally run. */
#define NUMSECTORS		500
int numSectors = NUMSECTORS;

/*  This is the number of ports  */
#define NUMPORTS		140
int numPorts = NUMPORTS;

/*  Percentage of sectors that will have the maximum number of warps in them  */
#define MAXJUMPPERCENT		3
/* Still hard-coded */
int percentJump = MAXJUMPPERCENT;

/*  Percentage chance that a final jump will be a one-way */
#define ONEWAYJUMPPERCENT	3
int percentDeadend = ONEWAYJUMPPERCENT;

/*  Percentage chance that a tunnel will be a dead end */
#define DEADENDPERCENT		30
int percentOneway = DEADENDPERCENT;

/* Length of strings for names, memory is an issue */
int strNameLength = 25;

/*  THESE ARE THE SET-IN-STONE FEDSPACE LINKS */
/*  DON'T EVEN THINK ABOUT TOUCHING THESE... */
const int fedspace[10][6] = {
  {2, 3, 4, 5, 6, 7},
  {1, 3, 7, 8, 9, 10},
  {1, 2, 4, 0, 0, 0},
  {1, 3, 5, 0, 0, 0},
  {1, 4, 6, 0, 0, 0},
  {1, 5, 7, 0, 0, 0},
  {1, 2, 6, 8, 0, 0},
  {2, 7, 0, 0, 0, 0},
  {2, 10, 0, 0, 0, 0},
  {2, 9, 0, 0, 0, 0}
};

struct sector **sectorlist;
struct config *configdata;
/*  struct port *portlist[NUMPORTS]; */
struct port **portlist;
/*  int randsectornum[NUMSECTORS-10]; */
int *randsectornum;

int compsec (const void *cmp1, const void *cmp2);
int randjump (int maxjumplen);
int randomnum (int min, int max);
void secjump (int from, int to);
int freewarp (int sector);
int warpsfull (int sector);
int numwarps (int sector);
void makeports ();
void sectorsort (struct sector *base[configdata->maxwarps], int elements);
extern char *randomname (char *name);
extern char *consellationName (char *name);
extern void init_usedNames ();



int
main (int argc, char **argv)
{
  int c;
  char *tmpname;
  int x, y, z, tempint, randint, tosector, fromsector, startsec, secptrcpy,
    jumpsize;
  int maxjumpsize;
  int usedsecptr;
  int len;
  char *fileline, *tempstr;
  FILE *file;
  struct sector *secptr;

  char *usageinfo =
    "Usage: bigbang [options]
    Options:
    -t < integer >
       indicate the max length of tunnels and dead ends.(default /minimum 6)
    - s < integer >
       indicate the max number of sectors.(default /minimum 500)
    - p < integer >
       indicate the max number of ports which MUST be at least 10 LESS than the
       number of sectors.(default /minimum 190)
    - o < integer >
       indicate the percentage chance that a final jump will be a one -
       way.(default /minimum 3)
    - d < integer >
       indicate the percentage chance that a tunnel will be a dead end.
       (default /minimum 30) \n ";
    /* This has to be taken out because of the knockon affect it was having with the rest of the program.
       -j <integer>  indicate the percentage of sectors that will have the maximum number of warps in them. (must be between 3 and 7) 
     */
    opterr = 0;

/*    while ((c = getopt (argc, argv, "t:s:p:j:d:o:")) != -1) */
  while ((c = getopt (argc, argv, "t:s:p:d:o:")) != -1)
    {
      switch (c)
	{
	case 't':
	  maxTunnel = (MAXTUNLEN > atoi (optarg)) ? MAXTUNLEN : atoi (optarg);
	  break;
	case 's':
	  numSectors =
	    (NUMSECTORS > atoi (optarg)) ? NUMSECTORS : atoi (optarg);
	  break;
	case 'p':
	  numPorts = (NUMPORTS > atoi (optarg)) ? NUMPORTS : atoi (optarg);
	  break;
/*        case 'j': */
/*          percentJump = (MAXJUMPPERCENT > atoi(optarg)) ? MAXJUMPPERCENT : atoi(optarg); */
/*          percentJump = (7 > percentJump) ? 7 : percentJump; */
/*          break; */
	case 'd':
	  percentDeadend =
	    (DEADENDPERCENT > atoi (optarg)) ? DEADENDPERCENT : atoi (optarg);
	  break;
	case 'o':
	  percentOneway =
	    (ONEWAYJUMPPERCENT >
	     atoi (optarg)) ? ONEWAYJUMPPERCENT : atoi (optarg);
	  break;
	case '?':
	  if (isprint (optopt))
	    fprintf (stderr, "Unknown option `-%c'.\n\n%s", optopt,
		     usageinfo);
	  else
	    fprintf (stderr,
		     "Unknown option character `\\x%x'.\n\n%s",
		     optopt, usageinfo);
	  return 1;
	default:
	  abort ();
	}
    }


  if (numPorts > (numSectors - 10))
    {
      fprintf (stderr,
	       "The max number of sectors MUST be greater than the number of ports. Program aborted.");
      exit (0);
    }
  maxjumpsize = (int) (numSectors * ((double) percentJump / 100));
  usedsecptr = numSectors - 11;

  randsectornum = (int *) malloc ((numSectors - 11) * sizeof (int));
  tmpname = malloc (sizeof (strNameLength));

  /*  Seed our randomizer */
  srand ((unsigned int) time (NULL));

  init_usedNames ();

  /*  Reading config.data file for config data (Duh...) */
  printf ("\nReading in config.data...");
  init_config ("config.data");
  printf ("done.\n");

  printf ("Creating sector array...");

  sectorlist = malloc (numSectors * sizeof (struct sector *));

  for (x = 0; x < numSectors; x++)
    sectorlist[x] = malloc (sizeof (struct sector));
  printf ("done.\n");

  printf ("Creating port array...");
  portlist = malloc (numPorts * sizeof (struct port *));

  for (x = 0; x < numPorts; x++)
    {
      portlist[x] = malloc (sizeof (struct port));
    }
  printf ("done.\n");

  printf ("Creating port array...");
  for (x = 0; x < numPorts; x++)
    {
      portlist[x] = NULL;
      sectorlist[x]->portptr = NULL;
    }
  printf ("done.\n");

  /*  Fills in the randsectornum array with numbers 10 to (numsectors - 1) */
  for (x = 0; x < numSectors - 10; x++)
    randsectornum[x] = x + 10;

  printf ("Randomly picking sector numbers...");
  /*  Randomly creates sector numbers to use: */
  for (x = numSectors - 11; x > 0; x--)
    {
      randint = randomnum (0, x);
      tempint = randsectornum[randint];
      randsectornum[randint] = randsectornum[x];
      randsectornum[x] = tempint;
    }
  printf ("done.\n");

  /*  Initalize sectorlist with data */
  for (x = 0; x < numSectors; x++)
    sectorlist[x]->number = x + 1;

  printf ("Creating Fedspace...");
  /*  Sets up Fed Space */
  for (x = 0; x < 10; x++)
    {
      for (y = 0; y < 6; y++)
	if (fedspace[x][y] != 0)
	  sectorlist[x]->sectorptr[y] = sectorlist[(fedspace[x][y]) - 1];
      sectorlist[x]->beacontext = "The Federation -- Do Not Dump!";
      sectorlist[x]->nebulae = "The Federation";
    }
  printf ("done.\n");

  printf ("Setting up links from FedSpace out to other sectors...");
  /*  Sets up 13 jumps from Fed Space to 6jump sectors  */
  for (x = 0; x <= 13; x++)
    {
      randint = randomnum (1, 5);
      do
	{
	  fromsector = randomnum (2, 9);
	}
      while (warpsfull (fromsector));
      jumpsize = randjump (3);

      for (y = 0; y < jumpsize; y++)
	{
	  tempint = randomnum (maxjumpsize, usedsecptr);
	  tosector = randsectornum[tempint];
	  randsectornum[tempint] = randsectornum[usedsecptr];
	  randsectornum[usedsecptr] = tosector;
	  usedsecptr--;
	  secjump (fromsector, tosector);
	  secjump (tosector, fromsector);
	  fromsector = tosector;
	}
      if (randint < 4)
	{
	  secjump (fromsector, x);
	  secjump (x, fromsector);
	}
    }
  printf ("done.\n");

  printf ("Setting up the max warp sectors...");
  /*  Sets up rest of links for the maxwarp sectors (the meat and potatoes) */
  for (x = 0; x < maxjumpsize; x++)
    {
      for (y = freewarp (x); y < configdata->maxwarps; y++)
	{
	  randint = randomnum (1, 100);
	  jumpsize = randjump (maxTunnel);
	  startsec = randsectornum[x];
	  fromsector = startsec;
	  for (z = 0; z < jumpsize; z++)
	    {
	      tempint = randomnum (maxjumpsize, usedsecptr);
	      tosector = randsectornum[tempint];
	      randsectornum[tempint] = randsectornum[usedsecptr];
	      randsectornum[usedsecptr] = tosector;
	      usedsecptr--;

	      secjump (fromsector, tosector);
	      secjump (tosector, fromsector);
	      fromsector = tosector;
	    }
	  if (randint <= (100 - percentDeadend))
	    {
	      do
		{
		  tosector = randsectornum[randomnum (0, maxjumpsize - 1)];
		}
	      while (tosector == startsec);
	      secjump (fromsector, tosector);
	      if (randomnum (1, 100) <= (100 - percentOneway)
		  && !warpsfull (tosector))
		secjump (tosector, fromsector);
	    }
	}
    }
  printf ("done.\n");

  for (x = 0; x < maxjumpsize; x++)
    {
      tempint = randsectornum[x];
      randsectornum[x] = randsectornum[usedsecptr];
      randsectornum[usedsecptr] = tempint;
      usedsecptr--;
    }

  printf ("Using up leftover sector numbers...");
  while (usedsecptr >= 0)	/*  finishes up creating other sector links... */
    {
      randint = randomnum (1, 100);
      tempint = maxTunnel;
      if (usedsecptr + 1 < maxTunnel)
	tempint = usedsecptr + 1;
      jumpsize = randjump (tempint);
      startsec = randsectornum[randomnum (usedsecptr + 1, numSectors - 11)];
      if (freewarp (startsec))
	{
	  fromsector = startsec;
	  secptrcpy = usedsecptr;
	  for (z = 0; z < jumpsize; z++)
	    {
	      tempint = randomnum (0, usedsecptr);
	      tosector = randsectornum[tempint];
	      randsectornum[tempint] = randsectornum[usedsecptr];
	      randsectornum[usedsecptr] = tosector;
	      usedsecptr--;

	      secjump (fromsector, tosector);
	      secjump (tosector, fromsector);
	      fromsector = tosector;
	    }
	  if (randint <= (100 - percentDeadend))
	    {
	      do
		{
		  tosector = randsectornum[randomnum (0, secptrcpy)];
		}
	      while (tosector == startsec || warpsfull (tosector));
	      secjump (fromsector, tosector);
	      if (randomnum (1, 100) <= (100 - percentOneway))
		secjump (tosector, fromsector);
	    }
	}
    }
  printf ("done.\n");

  printf ("Creating %d ports...", numPorts);
  makeports ();
  printf ("done.\n");


  printf ("Creating Ferringhi Home Sector...");
  tempint = randomnum (21, (numSectors-1));
  sectorlist[tempint]->beacontext = "Ferringhi";
  sectorlist[tempint]->nebulae = "Ferringhi";
  printf ("done.\n");
  /*  Sorts each sector's warps into numeric order */
  for (x = 0; x < numSectors; x++)
    {
      sectorsort (sectorlist[x]->sectorptr, numwarps (x));
    }

  /*  Writing data to universe.data file */
  printf ("Saving universe to file...");
  file = fopen ("./universe.data", "w");

  fileline = malloc (1024 * sizeof (char));
  tempstr = malloc (10 * sizeof (char));

  for (x = 0; x < numSectors; x++)
    {
      sprintf (fileline, "%d", (x + 1));
      fileline = strcat (fileline, ":");
      for (y = 0; y < numwarps (x); y++)
	{
	  secptr = sectorlist[x]->sectorptr[y];
	  sprintf (tempstr, "%d", secptr->number);
	  fileline = strcat (fileline, tempstr);
	  if (y + 1 != numwarps (x))
	    fileline = strcat (fileline, ",");
	}
      fileline = strcat (fileline, ":");
      /* Adds in names for sectors */
      if (sectorlist[x]->nebulae == NULL)
	{
	  sectorlist[x]->nebulae = malloc (sizeof (strNameLength));
	  tmpname = consellationName (tmpname);
	  sectorlist[x]->nebulae = tmpname;
	}
      if (sectorlist[x]->beacontext != NULL)
	fileline = strcat (fileline, sectorlist[x]->beacontext);
      fileline = strcat (fileline, ":");
      if (sectorlist[x]->nebulae != NULL)
	fileline = strcat (fileline, sectorlist[x]->nebulae);
      fileline = strcat (fileline, ":\n");
      /*  Later put in whitespace buffer for saving */
      /*  Not needed until user created beacons put in */
      fprintf (file, fileline);
    }
  fclose (file);
  free (fileline);
  free (tempstr);
  printf ("done.\n");

  /*  Writing data to ports.data file */
  printf ("Saving ports to file...");
  file = fopen ("./ports.data", "w");
  fileline = malloc (1024 * sizeof (char));

  tempstr = malloc (10 * sizeof (char));

  for (x = 0; x < numPorts; x++)
    {
      sprintf (fileline, "%d:%s:%d:%d:%d:%d:%d:%d:%d:%ld:%d:%d", (x + 1),
	       portlist[x]->name, portlist[x]->location,
	       portlist[x]->maxproduct[0], portlist[x]->maxproduct[1],
	       portlist[x]->maxproduct[2], portlist[x]->product[0],
	       portlist[x]->product[1], portlist[x]->product[2],
	       (long int) portlist[x]->credits, portlist[x]->type,
	       (int) portlist[x]->invisible);
      fileline = strcat (fileline, ":");
      len = strlen (fileline);
      for (y = 0; y <= 99 - len; y++)
	strcat (fileline, " ");
      strcat (fileline, "\n");
      fprintf (file, fileline);
    }
  fclose (file);

  printf ("done.\nUniverse sucessfully created!\n\n");

  return 0;
}

int
compsec (const void *cmp1, const void *cmp2)
{
  const struct sector *a = *(struct sector **) cmp1;
  const struct sector *b = *(struct sector **) cmp2;

  if (a->number > b->number)
    return 1;
  if (a->number < b->number)
    return -1;
  return 0;
}

int
randjump (int maxjumplen)
{
  if (maxjumplen > 2)
    {
      int temprandnum = randomnum (0, 2 + 2 + 1 + maxjumplen);
      /*  int temprandnum = 1 + ((int)((double)rand() / ((double) RAND_MAX + 1) * (2+2+1+maxjumplen))); */

      if (temprandnum == 0)
	return 0;
      if (temprandnum >= 1 && temprandnum <= 3)
	return 1;
      if (temprandnum >= 4 && temprandnum <= 6)
	return 2;
      if (temprandnum == 7 || temprandnum == 8)
	return 3;
      if (temprandnum == 9)
	return 4;
      if (temprandnum == 10)
	return 5;
      if (temprandnum == 11)
	return 6;
      if (temprandnum == 12)
	return 7;
      if (temprandnum == 13)
	return 8;
      if (temprandnum == 14)
	return 9;
      if (temprandnum == 15)
	return 10;
      return 1;
    }
  return maxjumplen;
}

int
randomnum (int min, int max)
{
  return (min +
	  ((int)
	   ((double) rand () / ((double) RAND_MAX + 1) * (1 + max - min))));
}

void
secjump (int from, int to)
{
  int y = freewarp (from);
  if (y != -1)
    sectorlist[from]->sectorptr[y] = sectorlist[to];
}

int
freewarp (int sector)
{
  int x;
  for (x = 0; x < configdata->maxwarps; x++)
    if (sectorlist[sector]->sectorptr[x] == NULL)
      return x;
  return -1;
}

int
warpsfull (int sector)
{
  if (freewarp (sector) == -1)
    return 1;
  return 0;
}

int
numwarps (int sector)
{
  int x = freewarp (sector);
  if (x == -1)
    return configdata->maxwarps;
  return x;
}

void
sectorsort (struct sector *base[configdata->maxwarps], int elements)
{
  struct sector *holdersector;
  int x = 0;
  int done = 0, alldone = 1;	/* This allows for exiting the sort */
  /*This could be done better, but for now it works */
  if (elements == 1)
    return;
  if (elements == 2)
    {
      if (base[0]->number > base[1]->number)
	{
	  holdersector = base[0];
	  base[0] = base[1];
	  base[1] = holdersector;
	}
      return;

    }
  while (1)
    {
      alldone = 1;
      for (x = 0; x < (elements / 2 - 1 + elements % 2); x++)
	{
	  if (base[2 * x]->number > base[2 * x + 1]->number)
	    {
	      holdersector = base[2 * x];
	      base[2 * x] = base[2 * x + 1];
	      base[2 * x + 1] = holdersector;
	    }
	}
      for (x = 1; x <= (elements / 2 - 1 + elements % 2); x++)
	{
	  if (base[2 * x - 1]->number > base[2 * x]->number)
	    {
	      alldone = 0;
	      done = 0;
	      holdersector = base[2 * x - 1];
	      base[2 * x - 1] = base[2 * x];
	      base[2 * x] = holdersector;
	    }
	  else if (alldone)
	    done = 1;
	}
      if (done)
	break;
    }
}

void
makeports ()
{
  struct port *curport;
  int type = 0;
  int loop = 0;
  int sector = 0;
  char name[25];
  char *tmpname;

  tmpname = malloc (sizeof (strNameLength));

  for (loop = 0; loop < numPorts; loop++)
    {
      curport = (struct port *) malloc (sizeof (struct port));
      curport->number = loop + 1;
      tmpname = randomname (tmpname);
      curport->name = (char *) malloc (sizeof (struct port));
      strcpy (name, "\0");
      sprintf (name, "%s", tmpname);
      strcpy (curport->name, name);
      curport->maxproduct[0] = randomnum (2800, 3000);
      curport->maxproduct[1] = randomnum (2800, 3000);
      curport->maxproduct[2] = randomnum (2800, 3000);
      type = randomnum (1, 8);
      curport->type = type;
      curport->product[0] = 0;
      curport->product[1] = 0;
      curport->product[2] = 0;
      curport->credits = 50000;
      curport->invisible = 0;	/*  Only *special* ports are invisible; */


      switch (type)
	{
	case 1:
	  curport->product[2] = curport->maxproduct[2];
	  break;
	case 2:
	  curport->product[1] = curport->maxproduct[1];
	  break;
	case 3:
	  curport->product[1] = curport->maxproduct[1];
	  curport->product[2] = curport->maxproduct[2];
	  break;
	case 4:
	  curport->product[0] = curport->maxproduct[0];
	  break;
	case 5:
	  curport->product[0] = curport->maxproduct[0];
	  curport->product[2] = curport->maxproduct[2];
	  break;
	case 6:
	  curport->product[0] = curport->maxproduct[0];
	  curport->product[1] = curport->maxproduct[1];
	  break;
	case 7:
	  curport->product[0] = curport->maxproduct[0];
	  curport->product[1] = curport->maxproduct[1];
	  curport->product[2] = curport->maxproduct[2];
	  break;
	}
      portlist[loop] = curport;
      curport = NULL;

      /*  Now for assigning the port to a sector */
      sector = randomnum (0, numSectors - 1);
      while (sectorlist[sector]->portptr != NULL)
	sector = randomnum (0, NUMSECTORS - 1);
      portlist[loop]->location = sector + 1;
    }
}
