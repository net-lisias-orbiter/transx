#define STRICT

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "orbitersdk.h"
#include "shiplist.h"
#include "viewstate.h"

bool viewstate::renderviewport=true;

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
}

viewstate::~viewstate()
{}

void viewstate::preparetoclose()
{}

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
