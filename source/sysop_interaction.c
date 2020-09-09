/*
Copyright (C) 2000 Jason C. Garcowski(jcg5@po.cwru.edu), 
                   Ryan Glasnapp(rglasnap@nmt.edu)

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

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/poll.h>
#include "msgqueue.h"
#include "common.h"
#include "player_interaction.h"
#include "sysop_interaction.h"
extern int WARP_WAIT;

void processSysopCommands (int msgidin)
{
	//TODO: Make this actually send a command to the message queue for it to handle things properly. 
  struct msgcommand data;
  char buffer[BUFF_SIZE];

  struct pollfd checkInput[1];

  checkInput[0].fd = 0;
  checkInput[0].events = POLLIN | POLLPRI;


  if ( poll(checkInput, 1, 1) == 1) 
  {
	fgets(buffer, BUFF_SIZE, stdin);
	buffer[strcspn(buffer, "\n")] = '\0';
	if (strcmp(buffer, "QUIT") == 0)
	{
	  data.command = ct_quit;
	  senddata(msgidin, &data, 0);
	}
	if (strcmp(buffer, "WARP_WAIT") == 0)
	{
	  WARP_WAIT = 1;
	}
	if (strcmp(buffer, "NOWARP_WAIT") == 0)
	{
	  WARP_WAIT = 0;
	}
		}
  return NULL;
}
