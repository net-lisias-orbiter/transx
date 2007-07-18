#ifndef __TRANSXSTATE_H
#define __TRANSXSTATE_H

#include "mfdfunction.h"
#include "mfdvarhandler.h"
#include "mfdvartypes.h"
#include "cmdnugget.h"
#include <deque>
#define NUMFUNCTIONS 100

class transxstate:private MFDFunction {
public:
	transxstate(OBJHANDLE thcraft,class shipptrs *tshipptrs);
	~transxstate();
	void baseonvessel(OBJHANDLE p_craft);
	bool baseonexisting(class transxstate *existing);

	virtual void dolowpriaction();

	bool doupdate(HDC hDC,int tw, int th, int currfunction,int currview, int curvarfunction, int currvarview,class TransxMFD *tmfdpointer);
	void savecurrent(FILEHANDLE scn);
	bool restoresave(FILEHANDLE scn);

	class TransxMFD *GetMFDpointer();//Returns pointer to current MFD
	//class basefunction *GetCurrFunction(); //Returns the current function
	class MFDvarhandler *GetVarhandler(int currvarfunction); //Gets the variable handler for the current function
	class MFDvariable *GetCurrVariable(int currvarfunction,int currviewmode); //Gets the current variable in the current function
	static void updatefocusvessel(OBJHANDLE newfocus); //Updates handles when vessel changes
	void updateownfocusvessel(OBJHANDLE newfocus); //Non-void section that updates focus vessel in every function
	bool sethelpsystem(bool thelpsystem) {helpsystem=thelpsystem;};
	bool fliphelpsystem(){return helpsystem=!helpsystem;};
	void togglefunctionswitch();
	int getnumfunctions(){return baselist.size();};
	int movetonextfunction(int curvarfunction);//Step forward to next function, creating one if needed/possible
	int movetopreviousfunction(int curvarfunction);//Step back to previous function
	int inc_viewmode(int currfunction, int currview);//Increment viewmode
	class basefunction *getnextfunction(int positionnumber);
	class basefunction *getpreviousfunction(int positionnumber);
	class basefunction *getbasefn(int stagenumber);
	class shipptrs *getshipptrs(){return shipptrs;};
	void setshipptrs(class shipptrs *ptr){shipptrs=ptr;};
	void showinitialstage(HDC hDC,int linespacing,int tw);
	bool checkbasefunction();//checks if first function should now be deleted due to non-validity
private:
	bool initfunctions();
	bool initialisevars();
	bool restartallfunctions();
	bool initflag;
	bool saveflag;
	bool selectshipvars;//whether ship level stuff is shown or not
	bool checkdelete();//delete one of the functions on the to delete list
	bool helpsystem;//On or off
	bool functionswitch;//whether to switch view with the variables
	bool mfdactive;
	int eastereggswitch;//Choose which Easter Egg to use
	int currcalcfunction;
	int actionframe;
	class TransxMFD *mfdpointer;
	class shipptrs *shipptrs;//List of viewstates for this transxstate

	class MFDvarhandler vars;
	class MFDvarshiplist m_ships;


	class mapfunction *mappointer;//The mapfunction pointer
	//static bool statelistinitflag;//Has the list been initialised?
	OBJHANDLE hcraft;
	std::deque<class basefunction*> baselist,todeletelist;
};

class copytransxstatecmd : public cmdnugget
{
private:
	class transxstate *mytxstate;
public:
	virtual void execute();
	void settransxstate(class transxstate *state){mytxstate=state;};
};


#endif