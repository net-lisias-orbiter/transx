#define STRICT

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "smartptr.h"
#include "orbitersdk.h"
#include "mfd.h"
#include "parser.h"
#include "mfdvarhandler.h"

void MFDvarhandler::addtolist(MFDvariable *item)
{
	listhead.addend(item);
}

void MFDvarhandler::saveallvariables(FILEHANDLE scn)
{
	char buffer[80];
	class dblliter *iterator=listhead.getiterator();
	class MFDvariable *pointer=static_cast<MFDvariable*>(iterator->front());
	while (pointer!=NULL)
	{
		pointer->getname(buffer);
		oapiWriteLine(scn,buffer);
		pointer->getsaveline(buffer);
		oapiWriteLine(scn,buffer);
		pointer=static_cast<MFDvariable*>(iterator->next());
	}
	strcpy(buffer,"Finvars");
	oapiWriteLine(scn,buffer);
	delete iterator;
}

bool MFDvarhandler::loadallvariables(FILEHANDLE scn)
{
	PARSER parser;
	char *buffer,*member;
	char namebuffer[20];
	int length,tadjmode;

	MFDvariable *pointer;
	XPtr<dblliter> iterator=listhead.getiterator();

	pointer=static_cast<MFDvariable*>(iterator->front());
	while (pointer!=NULL)
	{
		if (!oapiReadScenario_nextline(scn,buffer)) return false;
		pointer->getname(namebuffer);
		if (strcmp(buffer,namebuffer)==0)
		{
			//Name matches
			if (!oapiReadScenario_nextline(scn,buffer)) return false;//Unexpected end
			parser.parseline(buffer);
			if (!parser.getlineelement(0,&member,&length)) return false;//No required element
			sscanf(member,"%i",&tadjmode);
			pointer->setadjmode(tadjmode);//Set the adjustment mode
			if (!parser.getlineelement(1,&member,&length)) return false;//No required element
			pointer->loadvalue(member);//Set the variable to the saved value
		}
		else
		{
			//No match
			if (strcmp(buffer,"Finvars")==0) return true;//Somehow reached end of section - return anyway
		}
		pointer=static_cast<MFDvariable*>(iterator->next());
	}
	//Now finished variables, look for the end label
	do
	{
		if (!oapiReadScenario_nextline(scn,buffer)) return false;
	}
	while (strcmp(buffer,"Finvars")!=0);
	return true;
}


MFDvarhandler::MFDvarhandler()
{
	for (int a=0;a<5;a++)currvariable[a]=0;
	listhead.empty();
	variterator=listhead.getiterator();
	variterator->front();
}


MFDvarhandler::~MFDvarhandler()
{
	listhead.empty();
	delete variterator;
}




void MFDvarhandler::setnextcurrent(int viewmode)
{
	if (viewmode<0 || viewmode>4) viewmode=0;
	currviewmode=viewmode;
	MFDvariable *startpos;
	startpos=static_cast<MFDvariable*>(variterator->next());
	if (startpos==NULL) startpos=static_cast<MFDvariable*>(variterator->front());
	if (startpos==NULL) return;
	current=startpos;
	do
	{
		if (current!=NULL)
		{
			if ((current->viewmode1==viewmode || current->viewmode2==viewmode)&&current->showvariable)
			{
				currvariable[viewmode]=current;
				return;
			}
		}
		current=static_cast<MFDvariable*>(variterator->next());
	}
	while (startpos!=current);//causes termination if no legal outcome
	currvariable[viewmode]=current;
	return;
}

bool MFDvarhandler::crosscopy(MFDvarhandler &othervars)
{
	dblliter *iterator=listhead.getiterator();
	dblliter *remoteiterator=othervars.listhead.getiterator();
	MFDvariable *current=static_cast<MFDvariable*>(iterator->front());
	MFDvariable *remotecurrent=static_cast<MFDvariable*>(remoteiterator->front());
	while (current!=NULL)
	{
		current->setall(remotecurrent);
		current=static_cast<MFDvariable*>(iterator->next());
		remotecurrent=static_cast<MFDvariable*>(remoteiterator->next());
	}
	delete iterator;
	delete remoteiterator;
	return true;
}

void MFDvarhandler::setprevcurrent(int viewmode)
{
	if (viewmode<0 || viewmode>4) viewmode=0;
	currviewmode=viewmode;
	MFDvariable *startpos;
	startpos=static_cast<MFDvariable*>(variterator->previous());
	if (startpos==NULL) startpos=static_cast<MFDvariable*>(variterator->back());
	if (startpos==NULL) return;
	current=startpos;
	do
	{
		if (current!=NULL)
		{
			if ((current->viewmode1==viewmode || current->viewmode2==viewmode)&&current->showvariable)
			{
				currvariable[viewmode]=current;
				return;
			}
		}
		current=static_cast<MFDvariable*>(variterator->previous());
	}
	while (startpos!=current);//causes termination if no legal outcome
	currvariable[viewmode]=current;
	return;
}



MFDvariable* MFDvarhandler::getcurrent(int viewmode)
{
	if (viewmode==currviewmode) return static_cast<MFDvariable*>(variterator->current());
	current=currvariable[viewmode];
	current=static_cast<MFDvariable*>(listhead.isinlist(current));
	if (current==NULL)
	{
		setnextcurrent(viewmode);
	}
	return current;
}





