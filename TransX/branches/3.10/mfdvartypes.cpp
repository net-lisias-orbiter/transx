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
#include "mfdvartypes.h"
#include "doublelink.h"
#include "basefunction.h"
#include "shiplist.h"

liststring::liststring(bool manageme) : listelement(manageme)
{
	for (int a=0;a<40;a++) buffer[a]=0;
}

void MFDsemiintdiscrete::init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname,int tvalue)
{
	value=tvalue;
	initialise(vars,viewmode1,viewmode2);
	strcpy(name,vname);
}

void MFDvarmoon::init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname,OBJHANDLE tcentralbody)
{
	initialise(vars,viewmode1,viewmode2);
	strcpy(name,vname);
	ibuffer=intbuffer;
	strcpy(ibuffer,"");
	centralbody=tcentralbody;
	mappointer=mapfunction::getthemap();
	ivalue=0;
	pointer=NULL;
	adjmode=0;
}

void MFDvarmoon::setall(class MFDvariable *var)
{
	class MFDvarmoon *ivar=dynamic_cast<class MFDvarmoon*>(var);
	if (ivar==NULL) return;
	pointer=ivar->pointer;
	centralbody=ivar->centralbody;
	ivalue=ivar->ivalue;
	initvalidate();
}

MFDsemiintdiscrete::~MFDsemiintdiscrete()
{}

bool MFDsemiintdiscrete::flipback_variable()
{
	if (value>0) value--;
	return true;
}

MFDvarshiplist::MFDvarshiplist()
{
	iterator=shiplisthead.getiterator();
	initialised=false;
	adjmode=0;
}

MFDvarshiplist::~MFDvarshiplist()
{
	delete iterator;
}

void MFDvarshiplist::initbybody(OBJHANDLE craft,bool reset)
{
	if (craft==NULL) return;
	if (initialised && !reset) return;//Check to see if reinitialisation necessary
	initialised=true;
	shiplisthead.empty();
	class mapfunction *map=mapfunction::getthemap();
	class shipptr_itr shiterator;
	OBJHANDLE currship=NULL;
	OBJHANDLE craftplanet=map->getcurrbody(craft);
	OBJHANDLE currplanet=NULL;
	class shipptrs *curr=shiterator.getfirst();
	while (curr!=NULL)
	{
		currship=oapiGetVesselByName(curr->getname());
		if (currship!=NULL && currship!=craft)
		{//vessel exists and isn't our own vessel
			currplanet=curr->gettransxstate()->getbasefn(1)->gethmajor();//Get the current planet
			//from the first stage for this vessel
			if (currplanet==craftplanet)
			{
				addtolist(curr->getname());
			}
		}
		curr=shiterator.getnext();
	}
	iterator->front();
}

		
void MFDvarshiplist::addtolist(char *name)
{
	class liststring *temp=new liststring();
	char *buffer=temp->getbuffer();
	int length=strlen(name);
	strcpy(buffer,name);
	buffer[length]=0;
	shiplisthead.addfront(temp);
}

bool MFDsemiintdiscrete::flip_variable()
{
	value++;
	return true;
}

bool MFDvarmoon::flipback_variable()
{
	if (adjmode==0)
	{
		switch (ivalue){
		case 0:
			++ivalue;
			return true;
		case 1:
			++ivalue;
			pointer=mappointer->getfirstmoon(centralbody);
			initvalidate();
			if (pointer==NULL)
			{
				ivalue=0;
			}
			return true;
		case 2:
			pointer=mappointer->getnextpeer(pointer);
			initvalidate();
			if (pointer==NULL)
			{
				ivalue=0;
				strcpy(ibuffer,"");
			}
			return true;
		}
		ivalue=0;
		pointer=NULL;
		return true;
	}
	oapiOpenInputBox("Select Ship",SelectVariableBody,0,20, (void*)this);
	return true;
}

void MFDvarmoon::initvalidate()
{
	if (pointer!=NULL)
		oapiGetObjectName(pointer,ibuffer,30);
}

bool MFDvarmoon::flip_variable()
{
	if (adjmode==0)
	{
		switch (ivalue){
		case 0:
			ivalue=2;
			pointer=mappointer->getlastmoon(centralbody);
			initvalidate();
			if (pointer==NULL)
				ivalue=1;
			return true;
		case 1:
			--ivalue;
			return true;
		case 2:
			pointer=mappointer->getpreviouspeer(pointer);
			initvalidate();
			if (pointer==NULL)
			{
				ivalue=1;
				strcpy(ibuffer,"");
			}
			return true;
		}
		ivalue=0;
		pointer=NULL;
		return true;
	}
	oapiOpenInputBox("Select Ship",SelectVariableBody,0,20, (void*)this);
	return true;
}

void MFDvarmoon::ch_adjmode()
{
	adjmode=1-adjmode;
}

void MFDvarmoon::chm_adjmode()
{
	adjmode=1-adjmode;
}

void MFDvarmoon::showadjustment(HDC hDC, int width, int line) const
// This shows the mode of adjustment currently in force for the current MFDvariable
{
	char buffer[20];
	int ypos=int(7*line);
	int xpos=int(width/2);
	int length;
	switch (adjmode) 
	{
	case 0:
		length=sprintf(buffer,"Planets/Moons");
		break;
	case 1:
		length=sprintf(buffer,"Ships");
		break;
	}
	TextOut(hDC, xpos, ypos, buffer, length);
}

void MFDvarmoon::getsaveline(char *buffer) const
{
	getsaveline1(buffer);
	char tbuffer[30];
	if (pointer!=NULL)
		oapiGetObjectName(pointer,tbuffer,30);
	else
	{
		if (ivalue==1)
			strcpy(tbuffer,"Escape");
		else
			strcpy(tbuffer,"None");
	}
	strcat(buffer,tbuffer);
}

bool MFDvarmoon::loadvalue(char *buffer)
{
	strcpy(ibuffer,buffer);
	if (strcmp(buffer,"None")==0)
	{
		pointer=NULL;
		ivalue=0;
		return true;
	}
	if (strcmp(buffer,"Escape")==0)
	{
		pointer=NULL;
		ivalue=1;
		return true;
	}
	ivalue=2;
	pointer=oapiGetGbodyByName(buffer);
	if (pointer==NULL)
	{
		pointer=oapiGetObjectByName(buffer);
		ivalue=3;
	}
	if (pointer==NULL)
		ivalue=0;
	else
		strcpy(ibuffer,buffer);
	return true;
}


bool MFDvarmoon::SetVariableBody(char *str)
{
	OBJHANDLE temp=oapiGetGbodyByName(str);
	if (temp !=NULL)
		return false;
	temp=oapiGetObjectByName(str);
	if (temp==NULL) return false;
	OBJHANDLE temp2=mappointer->getcurrbody(temp);
	if (temp2!=centralbody)
		return false;
	pointer=temp;
	strcpy(ibuffer,str);
	ivalue=3;
	return true;
}

bool MFDvarmoon::validate()
{
	OBJHANDLE temp=oapiGetObjectByName(ibuffer);
	if (temp==pointer) return true;
	pointer=NULL;
	ivalue=0;
	strcpy(ibuffer," ");
	return false;
}

bool MFDvarmoon::show(HDC hDC, int width, int line)
{
	char buffer[20];
	if (pointer!=NULL)
	{
		oapiGetObjectName(pointer,buffer,20);
		strcpy(ibuffer,buffer);
	}
	else
	{
		if (ivalue==1)
			strcpy(buffer,"Escape");
		else
			strcpy(buffer,"None");
	}
	showgeneric(hDC,width,line,buffer);
	return true;
}

OBJHANDLE MFDvarmoon::gethandle() const
{
	if (ivalue!=1) return pointer;
	OBJHANDLE temp=mappointer->getmajor(centralbody);
	if (temp==centralbody)
		return NULL;
	else
		return temp;
}

int MFDvarmoon::getivalue() const
{
	return ivalue;
}

MFDvarmoon::~MFDvarmoon()
{}

MFDvarfloat::MFDvarfloat()
{}

void MFDvarfloat::init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname, int vadjmode, double vvalue, double vmin, double vmax, double vincrement, double vlogborder)
{
	initialise(vars,viewmode1,viewmode2);
	strcpy(name,vname);
	adjmode=vadjmode;
	defaultvalue=value=vvalue;
	min=vmin;
	max=vmax;
	increment=vincrement;
	logborder=vlogborder;
}

bool MFDvarshiplist::show(HDC hDC,int width,int line)
{
	char buffer[20];
	liststring *entry=static_cast<liststring*>(iterator->current());//It is this type
	if (entry==NULL)
		strcpy(buffer,"New Plan");
	else
	{
		char *shipname=entry->getbuffer();
		int length=strlen(shipname);
		if (length>19)
			length=19;
		strncpy(buffer,shipname,length);
		buffer[length]=0;
	}
	return showgeneric(hDC, width,line,buffer);
}


OBJHANDLE MFDvarshiplist::gethandle() const
{
	liststring *entry=static_cast<liststring*>(iterator->current());//It is this type
	if (entry==NULL)
		return NULL;
	return oapiGetVesselByName(entry->getbuffer());
}
	
bool MFDvarshiplist::flip_variable()
{
	iterator->next();
	return true;
}

bool MFDvarshiplist::flipback_variable()
{
	iterator->previous();
	return true;
}

bool MFDvarfloat::show(HDC hDC, int width, int line)
{
	char buffer[20];
	int linecentre=(int) width/2;
	int linepos= 6*line;
	strcpy(buffer,name);
	int length=strlen(buffer);
	TextOut(hDC, linecentre, linepos, buffer, length);
	showadjustment(hDC, width, line);
	linepos+=line+line;
	TextForm(buffer," ",value);
	showgeneric(hDC,width,line,buffer);
	return true;
}

void MFDvarfloat::getsaveline(char *buffer) const
{
	getsaveline1(buffer);
	char tbuffer[30];
	sprintf(tbuffer," %.12g",value);
	strcat(buffer,tbuffer);
}

bool MFDvarfloat::loadvalue(char *buffer)
{
	value=atof(buffer);
	return true;
}

bool MFDvarMJD::inc_variable()
{
	double temp=value;
	double adjuster=0;
	switch (adjmode){
	case 1:
		adjuster=0.1;
		break;
	case 2:
		adjuster=0.01;
		break;
	case 3:
		adjuster=0.001;
		break;
	case 4:
		adjuster=0.0001;
		break;
	case 5:
		adjuster=0.00001;
		break;
	case 6:
		value=oapiGetSimMJD();
		return true;
	}
	if (temp>logborder || temp<-logborder)
		temp+=fabs(temp)*adjuster*increment;
	else
		temp+=logborder*adjuster*increment;
	if (temp>max) temp=max;
	value=temp;
	return true;
}

bool MFDvarMJD::dec_variable()
{
	double temp=value;
	double adjuster=0;
	switch (adjmode){
	case 1:
		adjuster=0.1;
		break;
	case 2:
		adjuster=0.01;
		break;
	case 3:
		adjuster=0.001;
		break;
	case 4:
		adjuster=0.0001;
		break;
	case 5:
		adjuster=0.00001;
		break;
	case 6:
		value=oapiGetSimMJD();
		return true;

	}
	if (temp>logborder || temp<-logborder)
		temp-=fabs(temp)*adjuster*increment;
	else
		temp-=logborder*adjuster*increment;
	if (temp<min) temp=min;
	value=temp;
	return true;
}

bool MFDvarfloat::inc_variable()
{
	double temp=value;
	double adjuster=0;
	switch (adjmode){
	case 1:
		adjuster=0.1;
		break;
	case 2:
		adjuster=0.01;
		break;
	case 3:
		adjuster=0.001;
		break;
	case 4:
		adjuster=0.0001;
		break;
	case 5:
		adjuster=0.00001;
		break;
	case 6:
		value=defaultvalue;
		return true;
	}
	if (temp>logborder || temp<-logborder)
		temp+=fabs(temp)*adjuster*increment;
	else
		temp+=logborder*adjuster*increment;
	if (temp>max) temp=max;
	value=temp;
	return true;
}

bool MFDvarfloat::dec_variable()
{
	double temp=value;
	double adjuster=0;
	switch (adjmode){
	case 1:
		adjuster=0.1;
		break;
	case 2:
		adjuster=0.01;
		break;
	case 3:
		adjuster=0.001;
		break;
	case 4:
		adjuster=0.0001;
		break;
	case 5:
		adjuster=0.00001;
		break;
	case 6:
		value=defaultvalue;
		return true;

	}
	if (temp>logborder || temp<-logborder)
		temp-=fabs(temp)*adjuster*increment;
	else
		temp-=logborder*adjuster*increment;
	if (temp<min) temp=min;
	value=temp;
	return true;
}

void MFDvarfloat::setvalue(double tvalue)
{
	value=tvalue;
}

double MFDvarfloat::getvalue() const
{
	return value;
}

MFDvarfloat::~MFDvarfloat()
{}


MFDvarMJD::MFDvarMJD()
{}

bool MFDsemiintdiscrete::show(HDC hDC,int width,int line)
{
	char buffer[20];
	double temp=value*0.5;
	sprintf(buffer,"%.1f",temp);
	showgeneric(hDC, width,line,buffer);
	return true;
}

bool MFDvarMJD::show(HDC hDC, int width, int line)
{
	char buffer[20];
	sprintf(buffer,"%.4f", value);
	showgeneric(hDC,width,line,buffer);
	return true;
}


MFDvarMJD::~MFDvarMJD() 
{}


MFDvardiscrete::MFDvardiscrete()
{}

void MFDvarshiplist::init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname)
{
	initialise(vars,viewmode1,viewmode2);
	strcpy(name,vname);//brings in variable name
}

void MFDvardiscrete::init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname, int vvalue, int vlimit, char *st1, char *st2, char *st3, char *st4, char *st5)
{
	initialise(vars,viewmode1,viewmode2);
	strcpy(name,vname);
	adjmode=0;
	value=vvalue;
	limit=vlimit;
	strcpy(label[0], st1);
	strcpy(label[1], st2);
	strcpy(label[2], st3);
	strcpy(label[3], st4);
	strcpy(label[4], st5);
}


bool MFDvardiscrete::show(HDC hDC, int width, int line)
{
	showgeneric(hDC, width,line,label[value]);
	return true;
}

void MFDvardiscrete::getsaveline(char *buffer) const
{
	getsaveline1(buffer);
	char tbuffer[30];
	sprintf(tbuffer," %i",value);
	strcat(buffer,tbuffer);
}

void MFDsemiintdiscrete::getsaveline(char *buffer) const
{
	getsaveline1(buffer);
	char tbuffer[30];
	sprintf(tbuffer," %i",value);
	strcat(buffer,tbuffer);
}

bool MFDsemiintdiscrete::loadvalue(char *buffer)
{
	sscanf(buffer,"%i",&value);
	return true;
}


bool MFDvardiscrete::loadvalue(char *buffer)
{
	sscanf(buffer,"%i",&value);
	return true;
}


bool MFDvardiscrete::flip_variable()
{
	if (++value>limit)
		value=0;
	return true;
}

void MFDvardiscrete::setivalue(int tvalue)
{
	value=tvalue;
}

int MFDvardiscrete::getivalue() const
{
	return value;
}

MFDvardiscrete::~MFDvardiscrete() 
{}

MFDvarangle::MFDvarangle()
{}

void MFDvarangle::init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname, int vadjmode, double vvalue, double vcosvalue, double vsinvalue,
						 double vmin, double vmax, double vincrement, bool vloop)
{
	initialise(vars,viewmode1,viewmode2);
	strcpy(name,vname);
	adjmode=vadjmode;
	defaultvalue=value=vvalue;
	cosvalue=vcosvalue;
	sinvalue=vsinvalue;
	min=vmin;
	max=vmax;
	increment=vincrement;
	loop=vloop;
}

bool MFDvarangle::show(HDC hDC, int width, int line)
{
	char buffer[20];
	sprintf(buffer,"%.4f'", value/PI*180);
	showgeneric(hDC, width,line,buffer);
	return true;
}

void MFDvarangle::getsaveline(char *buffer) const
{
	getsaveline1(buffer);
	char tbuffer[30];
	sprintf(tbuffer," %.12g",value);
	strcat(buffer,tbuffer);
}

bool MFDvarangle::loadvalue(char *buffer)
{
	value=atof(buffer);
	cosvalue=cos(value);
	sinvalue=sin(value);
	return true;
}


bool MFDvarangle::inc_variable()
{
	double temp=value;
	double adjuster=0;
	switch (adjmode){
	case 1:
		adjuster=0.1;
		break;
	case 2:
		adjuster=0.01;
		break;
	case 3:
		adjuster=0.001;
		break;
	case 4:
		adjuster=0.0001;
		break;
	case 5:
		adjuster=0.00001;
		break;
	case 6:
		value=defaultvalue;
		cosvalue=cos(value);
		sinvalue=sin(value);
		return true;
	}
	temp+=adjuster*increment;
	if (temp>max)
	{
		if (loop)
			temp=min;
		else
			temp=max;
	}
	value=temp;
	cosvalue=cos(temp);
	sinvalue=sin(temp);
	return true;
}

bool MFDvarangle::dec_variable()
{
	double temp=value;
	double adjuster=0;
	switch (adjmode){
	case 1:
		adjuster=0.1;
		break;
	case 2:
		adjuster=0.01;
		break;
	case 3:
		adjuster=0.001;
		break;
	case 4:
		adjuster=0.0001;
		break;
	case 5:
		adjuster=0.00001;
		break;
	case 6:
		value=defaultvalue;
		cosvalue=cos(value);
		sinvalue=sin(value);
		return true;
	}
	temp-=adjuster*increment;
	if (temp<min)
	{
		if (loop)
			temp=max;
		else
			temp=min;
	}
	value=temp;
	cosvalue=cos(temp);
	sinvalue=sin(temp);
	return true;
}
	

void MFDvarangle::getsincos(double *sin, double *cos) const
{
	*sin=sinvalue;
	*cos=cosvalue;
}

double MFDvarangle::getsin() const
{
	return sinvalue;
}

double MFDvarangle::getcos() const
{
	return cosvalue;
}

void MFDvarangle::setvalue(double tvalue)
{
	value=tvalue;
	cosvalue=cos(value);
	sinvalue=sin(value);
}

double MFDvarangle::getvalue() const
{
	return value;
}

MFDvarangle::~MFDvarangle() 
{}
