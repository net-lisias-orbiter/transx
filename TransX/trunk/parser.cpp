#define STRICT

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "parser.h"


void parser::parseline(char *xbuffer)//Parses the line
{
	char *tbuffer;
	tbuffer=xbuffer;
	currmember=0;
	totalmembers=0;
	bool whitespace=true;//Currently moving through whitespace
	bool loop=true;
	while (loop) //While not end of line
	{
		if (*tbuffer!=0)
		{
			if (whitespace)
			{
				if (*tbuffer!=' ')
				{
					memberstart[currmember]=tbuffer;
					whitespace=false;
					totalmembers++;
				}
				else
				{
					tbuffer++;
				}
			}
			else
			{
				if (*tbuffer!=' ')
				{
					tbuffer++;
				}
				else
				{
					whitespace=true;
					memberend[currmember]=tbuffer;
					currmember++;
				}
			}
		}
		if (*tbuffer==0 && whitespace==false)
		{
			memberend[currmember]=tbuffer;
			currmember++;
		}
		if (*tbuffer==0 || currmember>9) loop=false;
	}
}

bool parser::getlineelement(int element, char **tbuffer, int *length)
{
	if (element>totalmembers-1 || element<0)
	{
		return false;//Requested element doesn't exist
	}
	*tbuffer=memberstart[element];
	*length=memberend[element]-memberstart[element];
	if (*length>39) return false;
	strncpy(parserbuffer,memberstart[element],*length);
	parserbuffer[*length]=0;
	*tbuffer=parserbuffer;
	return true;
}
