#define STRICT

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "orbitersdk.h"
#include "parser.h"
#include "mapfunction.h"
#include "shiplist.h"
//#include <math.h>

class shipptrs* shipptrs::first=NULL;
class shipptrs* shipptrs::current=NULL;
bool shipptrs::saved=false;

shipptrs::shipptrs()
{
	OBJHANDLE hcraft=oapiGetFocusObject();//Sets up new shipptrs for focus object
	for (int a=0;a<50;a++)
	{
		shipname[a]=0;
	}
	oapiGetObjectName(hcraft,shipname,49);
	subcreate();
	state=new transxstate(hcraft,this);//A new plan base for this vessel
}

shipptrs::shipptrs(OBJHANDLE hcraft)
{
	for (int a=0;a<50;a++)
	{
		shipname[a]=0;
	}
	oapiGetObjectName(hcraft,shipname,49);
	state=new transxstate(hcraft,this);//A new plan base for this vessel
	subcreate();
}

void shipptrs::subcreate()
{
	previous=NULL;
	for (int a=0;a<20;a++)
	{
		mfdlist[a]=NULL;//No views yet
	}
	next=first;
	first=this;
	if (next!=NULL)next->previous=this;
}



shipptrs::~shipptrs()
{
	for (int a=0;a<20;a++)
	{
		delete mfdlist[a];
	}
	delete state;
	if (first==this) first=next;
	if (next!=NULL) next->previous=previous;
	if (previous!=NULL) previous->next=next;
}

void shipptrs::backgroundaction()
{
	class mapfunction *map=mapfunction::getthemap();
	if (!map->getinitialised()) return;
	if (current==NULL)
	{
		current=first;
		if (current==NULL) return;
	}
	current->state->checkbasefunction();
	current=current->next;
}

class shipptrs *shipptrs::findship(OBJHANDLE hcraft)
{
	char tname[30];
	oapiGetObjectName(hcraft,tname,30);
	return findship(tname);
}

class shipptrs *shipptrs::findship(char *tname)
{
	if (first==NULL) return NULL;
	class shipptrs *ptr=first;
	do
	{
		if (strcmp(ptr->shipname,tname)==0) return ptr;
		ptr=ptr->next;
	}
	while (ptr!=NULL);
	return NULL;
}

void shipptrs::saveallships(FILEHANDLE scn)
{
	if (saved) return;
	class shipptrs *current=first;
	while (current!=NULL)
	{
		current->savecurrent(scn);
		current=current->next;
	}
	saved=true;
}

void shipptrs::restoreallships(FILEHANDLE scn)
{
	char *buffer,*member;
	PARSER parser;
	int length;
	while (oapiReadScenario_nextline(scn,buffer))
	{
		parser.parseline(buffer);
		bool ok=parser.getlineelement(0,&member,&length);
		if (!ok) return;
		if (strcmp("Ship",member)==0)
		{
			if (!parser.getlineelement(1,&member,&length)) return;//return if ship label doesn't exist
			if (length>30) return;//return if ship label is bad
			class shipptrs *temp=findship(member);
			if (temp==NULL)
			{
				OBJHANDLE temphandle=oapiGetVesselByName(member);
				if (temphandle!=NULL) temp=new shipptrs(temphandle);
			}
			if (temp!=NULL) temp->restorecurrent(scn);
		}
	}
}

	

void shipptrs::savecurrent(FILEHANDLE scn)
{
	oapiWriteScenario_string(scn,"Ship ",shipname);
	state->savecurrent(scn);
}

void shipptrs::restorecurrent(FILEHANDLE scn)
{
	state->restoresave(scn);
}


	
class shipptrs *shipptrs::getshipptrs()
{
	OBJHANDLE hcraft=oapiGetFocusObject();
	class shipptrs *ptr=findship(hcraft);
	if (ptr==NULL)
	{
		ptr=new shipptrs();
	}
	return ptr;
}

void shipptrs::destroyshipptrs()
{
	current=NULL;
	while (first!=NULL)
	{
		class shipptrs *temp=first;
		delete temp;//Destructor modifies first
	}
	//Also destroys the map
	class mapfunction *map=mapfunction::themap;
	if (map!=NULL)
	{
		delete map;
	}
}

void shipptrs::downshift()
{
	for (int a=0;a<20;a++)
	{
		if (mfdlist[a]!=NULL) mfdlist[a]->selfdownshift();
	}
}

void shipptrs::resetshift()
{
	for (int a=0;a<20;a++)
	{
		if (mfdlist[a]!=NULL) mfdlist[a]->resetshift();
	}
}


class viewstate *shipptrs::getviewstate(int mfdpos,class TransxMFD *mfdptr)
{
	if (mfdpos<0 || mfdpos>19) mfdpos=0;
	if (mfdlist[mfdpos]==NULL)
	{
		mfdlist[mfdpos]=new viewstate(mfdpos,mfdptr,this);
	}
	return mfdlist[mfdpos];
}

class shipptrs* shipptr_itr::getnext()
{
	if (current!=NULL) current=current->next;
	return current;
}

class shipptrs* shipptr_itr::getprev()
{
	if (current!=NULL) current=current->previous;
	return current;
}