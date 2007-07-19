#ifndef __VIEWSTATE_H
#define __VIEWSTATE_H

#include "transx.h"
#include "transxstate.h"

class viewstate
{
	int mfdposition;
	int viewmode;
	int varviewmode;
	int viewfunction,varviewfunction;
	bool switchmode;
	static bool renderviewport;
	bool mfdactive;//If there's an MFD associated with this
	class TransxMFD *imfd;
	viewstate(UINT tmfd,TransxMFD *mfdptr,class shipptrs *shipptrs);
	class transxstate *state;//Pointer to the transxstate associated with this
	void selfdownshift();
	void resetshift();
public:
	bool doupdate(HDC hDC,int tw,int th,TransxMFD *mfdptr);
	bool getrenderviewport(){return renderviewport;};
	class MFDvariable *GetCurrVariable(); 
	static void preparetoclose();
	void setmfdactive(bool temp){mfdactive=temp;};
	void movetonextfunction();
	void movetopreviousfunction();
	void inc_viewmode();
	void togglefunctionswitch(){switchmode=!switchmode;viewfunction=varviewfunction;viewmode=varviewmode;};
	void savecurrent(FILEHANDLE scn);
	void restoresave(FILEHANDLE scn);
	int getvariableviewmode(){return varviewmode;};
	class MFDvarhandler *GetVarhandler();
	void fliphelpsystem();
	~viewstate();
	friend class shipptrs;
};

#endif