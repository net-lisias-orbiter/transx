#define STRICT

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "orbitersdk.h"
#include "mfd.h"
#include "mfdfunction.h"


// static initialisation
MFDFunction *MFDFunction::lastfast=NULL;
MFDFunction *MFDFunction::firstfast=NULL;
MFDFunction *MFDFunction::lastslow=NULL;
MFDFunction *MFDFunction::firstslow=NULL;

MFDFunction::MFDFunction()
{
	allowedtowrite=true;
	valid=false;
	next=previous=NULL;
}

MFDFunction::~MFDFunction()
{
	delist();
}

void MFDFunction::delist()
{
	if (lastfast==this)
		lastfast=previous;
	if (firstfast==this)
		firstfast=next;
	if (lastslow==this)
		lastslow=previous;
	if (firstslow==this)
		firstslow=next;
	if (next!=NULL)
		next->previous=previous;
	if (previous!=NULL)
		previous->next=next;
	allowedtowrite=true;
}

bool MFDFunction::isvalid()
{
	return valid;
}

void MFDFunction::dohighpriaction()
{
}

void MFDFunction::dolowpriaction()
{}

void MFDFunction::donextaction()
{
	if (firstfast!=NULL)
	{
		firstfast->dofastaction();
		return;
	}
	if (firstslow!=NULL)
	{
		firstslow->doslowaction();
		return;
	}
}

void MFDFunction::dofastaction()
{
	firstfast=next;
	if (next!=NULL)	next->previous=NULL;
	next=NULL;
	previous=NULL;
	if (lastfast==this) lastfast=NULL;
	allowedtowrite=true;
	dohighpriaction();
}

void MFDFunction::doslowaction()
{
	firstslow=next;
	if (next!=NULL) next->previous=NULL;
	next=NULL;
	previous=NULL;
	if (lastslow==this)lastslow=NULL;
	allowedtowrite=true;
	dolowpriaction();
}

bool MFDFunction::addaction(int priority)
{
	if (!allowedtowrite) return false;
	allowedtowrite=false;
	if (priority==1)
	{
		//High priority action
		if (lastfast!=NULL)
		{
			previous=lastfast;
			previous->next=this;
		}
		lastfast=this;
		if (firstfast==NULL)
		{
			firstfast=this;
		}
		return true;
	}
	else
	{
		if (lastslow!=NULL)
		{
			previous=lastslow;
			previous->next=this;
		}
		lastslow=this;
		if (firstslow==NULL)
		{
			firstslow=this;
		}
		return true;
	}
	return true;
}
