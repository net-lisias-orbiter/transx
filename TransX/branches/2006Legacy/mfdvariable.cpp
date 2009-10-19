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
#include "mfdvariable.h"


MFDvariable::MFDvariable()
{
	continuous = false;
	showvariable=true;
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
	return true;
}

bool MFDvariable::show(HDC hDC, int width, int line)
//This is a virtual function that will not normally be used. Although MFDvariable is not
// a pure virtual class, it is only the derived classes that are created in practice.
// All these show() functions describe the MFDvariable on the screen
{
	return showgeneric(hDC,width,line," ");
}

bool MFDvariable::loadvalue(char *buffer)
{
	return true;
}

void MFDvariable::getname(char *buffer) const
{
	strcpy(buffer,name);
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

void MFDvariable::sethandle(OBJHANDLE tpointer)
{}

OBJHANDLE MFDvariable::gethandle() const
{
	OBJHANDLE temp=NULL;
	return temp;
}

MFDvariable::~MFDvariable() 
{
}