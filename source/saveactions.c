#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse.h"
#include "saveactions.h"
#include "universe.h"

extern struct sector **sectors;
extern struct player **players;
extern struct ship **ships;
extern struct port **ports;
extern struct planet **planets;

void saveplayer(int pnumb, char *filename)
{
	char *intptr = (char *)malloc(50);
	char *buffer = (char *)malloc(BUFF_SIZE);
	char *stufftosave = (char *)malloc(BUFF_SIZE);
	FILE *playerfile;
	int loop = 0, len = 0;

	strcpy(buffer, "\0");
	strcpy(intptr, "\0");
	strcpy(stufftosave, "\0");

	//sprintf (intptr, "%d:", pnumb - 1);
	sprintf(stufftosave, "%d:", pnumb);
	if (players[pnumb - 1] == NULL)
	{
		strcat(stufftosave, "(Null):(Null):0:0:0:0:0:0:0:0:0:0:");
	}
	else
	{
		addstring(stufftosave, players[pnumb - 1]->name, ':', BUFF_SIZE);
		addstring(stufftosave, players[pnumb - 1]->passwd, ':', BUFF_SIZE);
		addint(stufftosave, players[pnumb - 1]->sector, ':', BUFF_SIZE);
		addint(stufftosave, players[pnumb - 1]->ship, ':', BUFF_SIZE);
		addint(stufftosave, players[pnumb - 1]->experience, ':', BUFF_SIZE);
		addint(stufftosave, players[pnumb - 1]->alignment, ':', BUFF_SIZE);
		addint(stufftosave, players[pnumb - 1]->turns, ':', BUFF_SIZE);
		//addint (stufftosave, players[pnumb - 1]->credits, ':', BUFF_SIZE);
		sprintf(intptr, "%ld", players[pnumb - 1]->credits);
		addstring(stufftosave, intptr, ':', BUFF_SIZE);
		//addint(stufftosave, players[pnumb - 1]->bank_balance, ':', BUFF_SIZE);
		sprintf(intptr, "%ld", players[pnumb - 1]->bank_balance);
		addstring(stufftosave, intptr, ':', BUFF_SIZE);
		addint(stufftosave, players[pnumb - 1]->flags, ':', BUFF_SIZE);
		addint(stufftosave, players[pnumb - 1]->sysop, ':', BUFF_SIZE);
	}
	//Now to use intptr to find where to place the person.
	sprintf(intptr, "%d:", pnumb - 1);
	len = strlen(stufftosave);
	//fprintf(stderr, "saveplayer: Player save string is (%s)", stufftosave);

	for (loop = 1; loop <= 199 - len; loop++)
		strcat(stufftosave, " ");
	strcat(stufftosave, "\n");


	playerfile = fopen(filename, "r+");
	if (playerfile == NULL)
	{
		fprintf(stderr, "\nsaveplayer: No playerfile! Saving to new one!");
		if ((pnumb - 1) != 0)
		{
			fprintf(stderr,
				"\nsaveplayer: Player is not player 1 for new save file!");
			free(intptr);
			free(buffer);
			free(stufftosave);
			return;
		}
		playerfile = fopen(filename, "w");
		fprintf(playerfile, "%s", stufftosave);
		fclose(playerfile);
		free(intptr);
		free(buffer);
		free(stufftosave);
		return;
	}
	if (pnumb == 1)
	{
		fprintf(playerfile, "%s", stufftosave);
		fclose(playerfile);
		free(intptr);
		free(buffer);
		free(stufftosave);
		return;
	}
	while (strncmp(buffer, intptr, strlen(intptr)) != 0)
	{
		strcpy(buffer, "\0");
		fgets(buffer, BUFF_SIZE, playerfile);
		if (strlen(buffer) == 0)
			return;
	}
	fprintf(playerfile, "%s", stufftosave);
	fflush(playerfile);
	fclose(playerfile);
	free(intptr);
	free(buffer);
	free(stufftosave);
}

void saveship(int snumb, char *filename)
{
	char *intptr = (char *)malloc(10 * sizeof(char));
	char *buffer = (char *)malloc(BUFF_SIZE * sizeof(char));
	char *stufftosave = (char *)malloc(BUFF_SIZE * sizeof(char));
	FILE *playerfile;
	int loop = 0, len;

	strcpy(buffer, "\0");
	strcpy(intptr, "\0");
	strcpy(stufftosave, "\0");

	sprintf(intptr, "%d:", snumb - 1);
	sprintf(stufftosave, "%d:", snumb);
	if (ships[snumb - 1] == NULL)
	{
		strcat(stufftosave, "(Null):0:0:0:0:0:0:0:0:0:0:0:");
	}
	else
	{
		addstring(stufftosave, ships[snumb - 1]->name, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->type, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->location, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->fighters, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->shields, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->holds, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->colonists, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->equipment, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->organics, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->ore, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->owner, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->flags, ':', BUFF_SIZE);
		addint(stufftosave, ships[snumb - 1]->onplanet, ':', BUFF_SIZE);
	}
	len = strlen(stufftosave);
	for (loop = 1; loop <= 199 - len; loop++)	//This puts a buffer of space in the save
		strcat(stufftosave, " ");	//file so things don't get overwritten
	strcat(stufftosave, "\n");	//when saving.

	playerfile = fopen(filename, "r+");
	if (playerfile == NULL)
	{
		fprintf(stderr, "\nsaveship: No ship file! Saving to new one!");
		if ((snumb - 1) != 0)
		{
			fprintf(stderr, "\nsaveship: Ship is not #1 for new save file!");
			exit(-1);
		}
		playerfile = fopen(filename, "w");
		fprintf(playerfile, "%s", stufftosave);
		fclose(playerfile);
		free(intptr);
		free(buffer);
		free(stufftosave);
		return;
	}
	if (snumb == 1)
	{
		fprintf(playerfile, "%s", stufftosave);
		fclose(playerfile);
		free(intptr);
		free(buffer);
		free(stufftosave);
		return;
	}
	while (strncmp(buffer, intptr, strlen(intptr)) != 0)
	{
		strcpy(buffer, "\0");
		fgets(buffer, BUFF_SIZE, playerfile);
		if (strlen(buffer) == 0)
			return;
	}
	fprintf(playerfile, "%s", stufftosave);
	fclose(playerfile);
	free(intptr);
	free(buffer);
	free(stufftosave);

}

void saveallports (char *filename)
{
    char *stufftosave = (char *) malloc (BUFF_SIZE);
    FILE *portfile;
    int loop = 0, len;
    int portnumb = 1;

    portfile = fopen (filename, "w");
    while (ports[portnumb - 1] != NULL)
    {
        strcpy (stufftosave, "\0");
        sprintf (stufftosave, "%d:", portnumb);
        addstring (stufftosave, ports[portnumb - 1]->name, ':', BUFF_SIZE);
        addint (stufftosave, ports[portnumb - 1]->location, ':', BUFF_SIZE);
        addint (stufftosave, ports[portnumb - 1]->maxproduct[0], ':',
                BUFF_SIZE);
        addint (stufftosave, ports[portnumb - 1]->maxproduct[1], ':',
                BUFF_SIZE);
        addint (stufftosave, ports[portnumb - 1]->maxproduct[2], ':',
                BUFF_SIZE);
        addint (stufftosave, ports[portnumb - 1]->product[0], ':', BUFF_SIZE);
        addint (stufftosave, ports[portnumb - 1]->product[1], ':', BUFF_SIZE);
        addint (stufftosave, ports[portnumb - 1]->product[2], ':', BUFF_SIZE);
        addint (stufftosave, ports[portnumb - 1]->credits, ':', BUFF_SIZE);
        addint (stufftosave, ports[portnumb - 1]->type, ':', BUFF_SIZE);
        addint (stufftosave, ports[portnumb - 1]->invisible, ':', BUFF_SIZE);

        len = strlen (stufftosave);
        for (loop = 1; loop <= 199 - len; loop++)	//This puts a buffer of space in the save
            strcat (stufftosave, " ");	//file so things don't get overwritten
        strcat (stufftosave, "\n");	//when saving.
        //fprintf(stderr, "\nsaveallports: Saving port '%s'", stufftosave);
        //fflush(stderr);

        fprintf (portfile, "%s", stufftosave);
        portnumb++;
    }
    fclose (portfile);
    free (stufftosave);

}
