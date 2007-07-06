#define STRICT

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "orbitersdk.h"
#include "shiplist.h"
#include "viewstate.h"


//class transxstate *viewstate::state=NULL;
//bool viewstate::viewstatesinitialised=false;
bool viewstate::renderviewport=true;
//class viewstate *viewstate::viewstates[20];

extern double debug;

viewstate::viewstate(UINT tmfd,class TransxMFD *mfdptr,class shipptrs *shipptrs)
{
	viewmode=2;
	varviewmode=2;
	viewfunction=1;
	varviewfunction=1;
	switchmode=true;
	mfdactive=false;
	renderviewport=true;
	imfd=mfdptr;
	mfdposition=tmfd;
	state=shipptrs->gettransxstate();

	
	/*if (state==NULL)
	{
		state=new transxstate();
	}
	if (!viewstatesinitialised)
	{
		for (int a=0;a<20;a++)viewstates[a]=NULL;
		viewstatesinitialised=true;
	}
	imfd=mfdptr;
	if (tmfd<20)
	{
		viewstates[tmfd]=this;
		mfdposition=tmfd;
	}*/
}

viewstate::~viewstate()
{
	/*if (mfdposition>-1 && mfdposition<20)
	{
		viewstates[mfdposition]=NULL;
	}
	bool alldone=true;
	for (int a=0;a<20;a++)
	{
		if (viewstates[a]!=NULL) alldone=false;
	}
	if (alldone)//If all viewstates deleted, delete the main data structure
	{
		delete state;
		state=NULL;
	}*/
}

/*void viewstate::updatefocusvessel(OBJHANDLE newfocus)
{
	if (state!=NULL) state->updateownfocusvessel(newfocus);
}*/


/*bool viewstate::getviewstate(UINT tmfd,class viewstate **tviewstate,class TransxMFD *mfdptr,VESSEL *vessel)
{
	if (tmfd>20 || tmfd<0) return false;

	if (!viewstatesinitialised)
	{
		*tviewstate=NULL;
	}
	else
	{
		*tviewstate=viewstates[tmfd];
	}
	if (*tviewstate==NULL)
	{
		*tviewstate=new viewstate(tmfd,mfdptr);
	}
	else
	{
		viewstates[tmfd]->imfd=mfdptr;
	}
	return true;
}*/


void viewstate::preparetoclose()
{
	renderviewport=false;//tell any active mfd's to delete their states as they close
	/*for (int a=0;a<20;a++)//Delete all inactive mfdstates
	{
		if (viewstates[a]!=NULL)
		{
			if (!viewstates[a]->mfdactive)
			{
				delete viewstates[a];
				viewstates[a]=NULL;
			}
		}
	}*/
}

void viewstate::movetonextfunction()
{
	varviewfunction=state->movetonextfunction(varviewfunction);
	if (switchmode) viewfunction=varviewfunction;
}

void viewstate::movetopreviousfunction()
{
	varviewfunction=state->movetopreviousfunction(varviewfunction);
	if (switchmode) viewfunction=varviewfunction;
}

void viewstate::inc_viewmode()
{
	varviewmode=state->inc_viewmode(varviewfunction,varviewmode);
	if (switchmode) viewmode=varviewmode;
}


void viewstate::savecurrent(FILEHANDLE scn)
{
	state->savecurrent(scn);
}

void viewstate::restoresave(FILEHANDLE scn)
{
	state->restoresave(scn);
}


bool viewstate::doupdate(HDC hDC,int tw,int th,TransxMFD *mfdptr)
{
	int numfunctions=state->getnumfunctions();
	if (viewfunction>numfunctions && numfunctions>0) viewfunction=numfunctions;
	if (varviewfunction>numfunctions && numfunctions>0) varviewfunction=numfunctions;
	return state->doupdate(hDC,tw,th,viewfunction,viewmode,varviewfunction,varviewmode,mfdptr);
}

/*void viewstate::downshift()
//Moved to shipptrs
{
	for (int a=0;a<20;a++)
	{
		if (viewstates[a]!=NULL)
		{
			viewstates[a]->selfdownshift();
		}
	}
}*/

void viewstate::selfdownshift()
{
	if (viewfunction>1) viewfunction--;
	if (varviewfunction>1) varviewfunction--;
}

void viewstate::resetshift()
{
	viewfunction=varviewfunction=1;
}

class MFDvariable *viewstate::GetCurrVariable()
{
	return state->GetCurrVariable(varviewfunction,varviewmode);
}


void viewstate::fliphelpsystem()
{
	state->fliphelpsystem();
}


class MFDvarhandler *viewstate::GetVarhandler()
{
	return state->GetVarhandler(varviewfunction);
}
