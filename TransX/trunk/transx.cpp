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
#define ORBITER_MODULE
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "orbitersdk.h"

#include "mfd.h"
#include "graph.h"
#include "intercept.h"
#include "mfdvarhandler.h"
#include "basefunction.h"
#include "viewstate.h"
#include "shiplist.h"
#include "transx.h"

int TransxMFD::MfdCount=0;

double debug;

TransxMFD::TransxMFD (DWORD w, DWORD h, VESSEL *vessel, UINT mfd)
: MFD (w, h, vessel)

// Initialise TransXMFD
{
	//Check to see if new Transxstate is required
	valid=false;
	viewstate=NULL;
	if (mfd>20) mfd=1;//Set to safe value in this instance
	class shipptrs *shptr=shipptrs::getshipptrs();//Knows ship must be current focus
	viewstate=shptr->getviewstate(mfd,this);
	viewstate->setmfdactive(true);
	++MfdCount;
}

TransxMFD::~TransxMFD()
{
	viewstate->setmfdactive(false);
	if (!viewstate->getrenderviewport())//If we're closing, take the state with you, otherwise leave it.
	{
		shipptrs::destroyshipptrs();//destroy all pointer structures
	}
	--MfdCount;
}

// Called by Orbiter when a screen update is needed
void TransxMFD::Update (HDC hDC)

{
	Title (hDC, "TransX MFD");
	HPEN pen=SelectDefaultPen(hDC,TransXFunction::Green);//Selects an Orbiter default pen, and retrieves called pen
	valid=viewstate->doupdate(hDC,W,H,this);
	shipptrs::refreshsave();//allow save again, as new values are now available
	SelectObject(hDC,pen);//Replaces initial pen back into hDC. Now the MFD causes no pen creation
	if (!valid) return;
	int linespacing=H/24;
	
	if (debug!=0)
	{
		char buffer[20];

		int length=sprintf(buffer,"Debug:%g",debug);
		TextOut(hDC,0,linespacing*22,buffer,length);
	}

	MFDvariable *varpointer=viewstate->GetCurrVariable();
	if (varpointer==NULL) return;
	varpointer->show(hDC,W,linespacing);
}

int TransxMFD::getwidth()
{
	return W;
}

int TransxMFD::getheight()
{
	return H;
}

int TransxMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
// Standard message parser for messages passed from Orbiter
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		return (int) new TransxMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam, mfd);
	}
	return 0;
}

char *TransxMFD::ButtonLabel (int bt)
// Routine to pass button label back to Orbiter. Called by Orbiter
{
	char *label[11] = {"HLP","FWD","BCK", "VW","VAR","-VR", "ADJ", "-AJ","++", "--","EXE"};
	return (bt < 11 ? label[bt] : 0);
}

int TransxMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
// Routine to pass menu description for buttons back to Orbiter. Called by Orbiter
{
	static const MFDBUTTONMENU mnu[11] = {
		{"Toggle Help","On/Off",'H'},
		{"Step Forward","/ Add Step",'F'},
		{"Step Back",0,'R'},
		{"Select View",0,'W'},
		{"Next Variable", 0, '.'},
		{"Prev. Variable", 0, ','},
		{"Set adjustment", "method (+)", '{'},
		{"Set adjustment", "method (-)", '}'},
		{"Increment", "variable", '='},
		{"Decrement", "variable", '-'},
		{"Execute","Command",'X'}
	};
	if (menu) *menu = mnu;
	return 11;
}

bool TransxMFD::ConsumeKeyImmediate(char *kstate)
// Routine which processes the keyboard state. Allows continuous change of variable.
{
	if (!valid) return false;
	MFDvariable *varpointer=viewstate->GetCurrVariable();
	if (varpointer==NULL) return false;
	if  (*(kstate+OAPI_KEY_MINUS)==-128)
	{
		varpointer->dec_variable();
		return true;
	}
	if (*(kstate+OAPI_KEY_EQUALS)==-128)
	{
		varpointer->inc_variable();
		return true;
	}
	return false;
}

bool TransxMFD::ConsumeButton(int bt, int event)
// Deal with mouse pressing of keys
{
	static const DWORD btkey[11]={OAPI_KEY_H, OAPI_KEY_F, OAPI_KEY_R, OAPI_KEY_W, OAPI_KEY_PERIOD, 
		OAPI_KEY_COMMA, OAPI_KEY_LBRACKET, OAPI_KEY_RBRACKET, OAPI_KEY_EQUALS, OAPI_KEY_MINUS, OAPI_KEY_X};
	if (!valid) return false;
	MFDvariable *varpointer=viewstate->GetCurrVariable();
	if ((event & PANEL_MOUSE_LBDOWN) && (bt<8 || bt==10)) return ConsumeKeyBuffered (btkey[bt]);
	if (bt>9) return false; //No buttons above 10
	if (varpointer==NULL) return false;
	if ((event & PANEL_MOUSE_LBDOWN)/* && varpointer->getadjtype()==0*/) // FIXME what does this do???
	{
		// Button 8 or 9 is pressed and the mode is discrete
		return ConsumeKeyBuffered (btkey[bt]);
	}
	// At this point, only the possibility of immediate consumption...
	if (!(event & PANEL_MOUSE_LBPRESSED) || !varpointer->IsContinuous()) return false; //Mouse button is down on 9 or 10
	if (bt==9)
		varpointer->dec_variable();
	if (bt==8)
		varpointer->inc_variable();
	return true;
}


bool TransxMFD::ConsumeKeyBuffered (DWORD key)

{
	if (!valid) return false;
	MFDvariable *currvar=viewstate->GetCurrVariable();
	bool access=(currvar!=NULL);
	switch (key) {
	case OAPI_KEY_H:
		viewstate->fliphelpsystem();
		return true;
	case OAPI_KEY_F:
		viewstate->movetonextfunction();
		return true;
	case OAPI_KEY_R:
		viewstate->movetopreviousfunction();
		return true;
	case OAPI_KEY_W:
		viewstate->inc_viewmode();
		return true;
	case OAPI_KEY_X:
		if (access) currvar->execute();
		return true;
	case OAPI_KEY_PERIOD: //Switch variable
		{
			int viewmode=viewstate->getvariableviewmode();
			viewstate->GetVarhandler()->setnextcurrent(viewmode);
		}
		return true;
	case OAPI_KEY_COMMA:
		{
			int viewmode=viewstate->getvariableviewmode();
			viewstate->GetVarhandler()->setprevcurrent(viewmode);
		}
		return true;
	case OAPI_KEY_LBRACKET:
		if (access) currvar->ch_adjmode();
		return true;
	case OAPI_KEY_RBRACKET:
		if (access) currvar->chm_adjmode();
		return true;
	case OAPI_KEY_MINUS:
		if (access)	currvar->dec_variable();
		return true;
	case OAPI_KEY_EQUALS:
		if (access) currvar->inc_variable();
		return true;
	}
	return false; //Key not one of cases above
}

void TransxMFD::WriteStatus(FILEHANDLE scn) const
{
	if (!valid) 
		return;
	shipptrs::saveallships(scn);
}

void TransxMFD::ReadStatus(FILEHANDLE scn)
{
	shipptrs::restoreallships(scn);
}
