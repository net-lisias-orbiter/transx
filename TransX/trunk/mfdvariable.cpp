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
#include "orbitersdk.h"
#include "mfd.h"
#include "cmdnugget.h"
#include "mfdvariable.h"


MFDvariable::MFDvariable()
{
	showvariable=true;
	inugget=NULL;
	execstatus=execcountdown=0;
	adjmode=0;
}

void MFDvariable::execute()
{
	if (inugget!=NULL) 
	{	
		inugget->execute();
		execstatus=1;
	}
	else
	{
		execstatus=2;
	}
	execcountdown=10;
}

void MFDvariable::initialise(MFDvarhandler *vars,int tviewmode1,int tviewmode2)
{
	viewmode1=tviewmode1;
	viewmode2=tviewmode2;
	if (vars!=NULL)
	{
		vars->addtolist(this);
	}
}

void MFDvariable::setshow(bool value)
{
	showvariable=value;
}

void MFDvariable::setall(class MFDvariable *var)
{
	sethandle(var->gethandle());
}

bool MFDvariable::getshow()
{
	return showvariable;
}

void MFDvariable::setcmdnugget(cmdnugget *nugget)
{
	if (nugget==NULL) return;
	nugget->setmfdvariable(this);
	inugget=nugget;
}

bool MFDvariable::showgeneric(HDC hDC,int width,int line, char *inbuff)
{
// This is a helper function that formats output to the MFD screen
	char buffer[MAX_NAME_LENGTH];
	int linecentre=(int) width/2;
	int linepos= 6*line;
	int inlength=strlen(inbuff);
	strcpy(buffer,name);
	int length=strlen(buffer);
	TextOut(hDC, linecentre, linepos, buffer, length);
	showadjustment(hDC, width, line);
	linepos+=line+line;
	TextOut(hDC, linecentre, linepos, inbuff,inlength);
	if (execcountdown>0)
	{
		linepos+=line;
		if (execstatus==1)
		{
			strcpy(buffer,"Executed");
		}
		else
		{
			strcpy(buffer,"No Command");
		}
		execcountdown--;
		if (execcountdown==0) execstatus=0;
		length=strlen(buffer);
		TextOut(hDC,linecentre,linepos,buffer,length);
	}
	return true;
}

bool MFDvariable::show(HDC hDC, int width, int line)
//This is a virtual function that will not normally be used. Although MFDvariable is not
// a pure virtual class, it is only the derived classes that are created in practice.
// All these show() functions describe the MFDvariable on the screen
{
	return showgeneric(hDC,width,line," ");
}

void MFDvariable::getsaveline1(char *buffer) const
{
	sprintf(buffer," %i ",adjmode);
}

void MFDvariable::getsaveline(char *buffer) const//Overloaded by final function
{
	getsaveline1(buffer);
	strcat(buffer,"0");
}

bool MFDvariable::loadvalue(char *buffer)
{
	return true;
}

void MFDvariable::getname(char *buffer) const
{
	strcpy(buffer,name);
}

void MFDvariable::setadjmode(int tadjmode)
{
	adjmode=tadjmode;
}

int MFDvariable::getadjtype() const
{
	return adjmode;
}

void MFDvariable::gethelpstrings(char *help1,char *help2) const
{
	strcpy(help1,helpstring1);
	strcpy(help2,helpstring2);
}

void MFDvariable::sethelpstrings(char *help1,char *help2)
{
	strcpy(helpstring1,help1);
	strcpy(helpstring2,help2);
}

int MFDvariable::getadjmode() const
{
	return adjmode;
}

void MFDvariable::showadjustment(HDC hDC, int width, int line) const
// This shows the mode of adjustment currently in force for the current MFDvariable
{
	char buffer[MAX_NAME_LENGTH];
	int ypos=int(7*line);
	int xpos=int(width/2);
	int length;
	switch (adjmode) 
	{
	case 0:
		length=sprintf(buffer," ");
		break;
	case 1:
		length=sprintf(buffer,"Coarse");
		break;
	case 2:
		length=sprintf(buffer,"Medium");
		break;
	case 3:
		length=sprintf(buffer,"Fine");
		break;
	case 4:
		length=sprintf(buffer,"Super");
		break;
	case 5:
		length=sprintf(buffer,"Ultra");
		break;
	case 6:
		length=sprintf(buffer,"Hyper");
		break;
	case 7:
		length=sprintf(buffer,"Reset");
		break;
	}
	TextOut(hDC, xpos, ypos, buffer, length);
}


void MFDvariable::ch_adjmode()
// Change the adjustment mode of this MFDvariable
{
	if (adjmode==0) return;
	if (++adjmode>7) adjmode=1;
}

void MFDvariable::chm_adjmode()
{
	if (adjmode==0) return;
	if (--adjmode<1) adjmode=7;
}

bool MFDvariable::flip_variable()

// Used to manipulate MFDvardiscrete class - this is run for those classes which
// do not use this method of changing the variable.

{
	return false;
}

bool MFDvariable::flipback_variable()
{
	return flip_variable();
}

void MFDvariable::inc_variable()
{
}

void MFDvariable::dec_variable()
{
}

void MFDvariable::sethandle(OBJHANDLE tpointer)
{}

OBJHANDLE MFDvariable::gethandle() const
{
	OBJHANDLE temp=NULL;
	return temp;
}


MFDvariable::~MFDvariable() 
{
	if (inugget!=NULL) delete inugget;
}
