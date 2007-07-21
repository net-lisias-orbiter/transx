/* Copyright (c) 2007 Duncan Sharpe, Steve Arch
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
** 
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.*/

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
		return false;//Requested element doesn't exist
	*tbuffer=memberstart[element];
	*length=memberend[element]-memberstart[element];
	if (*length>39) return false;
	strncpy(parserbuffer,memberstart[element],*length);
	parserbuffer[*length]=0;
	*tbuffer=parserbuffer;
	return true;
}
