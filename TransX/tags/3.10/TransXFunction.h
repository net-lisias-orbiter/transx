#ifndef __TRANSXFUNCTION_H
#define __TRANSXFUNCTION_H

#include "mfdfunction.h"
//#include "transxstate.h"
#include "parser.h"
#include "mfdvarhandler.h"
#include "intercept.h"
#include "graph.h"

class transxstate;

class TransXFunction: public MFDFunction
{
private:
	int numberviews;
	void initpens(void);
	virtual bool initialisevars() = 0;
	void deletepens();
	PARSER parser;
	char helpstring1[40],helpstring2[40],helpstring3[40],helpstring4[40],helpstring5[40];
protected:
	OBJHANDLE hmajor, hminor, hmajtarget, hcraft, hbase;//Standard set of OBJHANDLES for the TransX MFD
	double gravbodyratio; //Specific computation associated with hmajor+hminor
	double simstartMJD; //Time at which current scenario commenced
	class MFDvarhandler vars; //Variable set associated with this MFDFunction
	class transxstate *state; //Pointer to calling transxstate
	HPEN pens[6];//Replacement pens for MFD



public:
	HPEN SelectDefaultPen(HDC hDC, int value);
	void setnumberviews(int tnumberviews);


	MFDvarhandler* getvariablehandler();//Passes pointer to variable handler
	TransXFunction(class transxstate *tstate, OBJHANDLE thmajor, OBJHANDLE thminor, OBJHANDLE thtarget, OBJHANDLE thcraft, OBJHANDLE thbase);//Constructor
	TransXFunction(class transxstate *tstate, OBJHANDLE thmajor, OBJHANDLE thminor,OBJHANDLE thcraft);//Constructor
	virtual ~TransXFunction();//Destructor
	virtual void restoreself(FILEHANDLE scn);//Attempts to restore previous saved state
	virtual void saveself(FILEHANDLE scn);//Saves current state to file
	void findfinish(FILEHANDLE scn);//Find end of function
	bool loadhandle(FILEHANDLE scn,OBJHANDLE *handle);//Loads an objecthandle from a file
	void savehandle(FILEHANDLE scn, OBJHANDLE handle);//Saves handle to scenario file
	void savevector(FILEHANDLE scn, VECTOR3 &vector);
	void savedouble(FILEHANDLE scn, double savenumber);
	bool loadint(FILEHANDLE scn, int *loadedint);
	bool loaddouble(FILEHANDLE scn, double *loadednumber);
	bool loadvector(FILEHANDLE scn, VECTOR3 *loadedvector);
	void saveorbit(FILEHANDLE scn, const ORBIT &saveorbit);//Saves an orbit structure
	bool loadorbit(FILEHANDLE scn, ORBIT *loadorbit);//Loads an orbit structure
	virtual void doupdate(HDC hDC, int tw, int th, int viewmode){return;};//overloaded to create views
	MFDvariable *getcurrentvariable(int view);
	void gethandles(OBJHANDLE *thmajor, OBJHANDLE *thminor, OBJHANDLE *thtarget, OBJHANDLE *thcraft, OBJHANDLE *thbase); //Gets handles
	OBJHANDLE gethmajor(){return hmajor;};//Return central body
	OBJHANDLE gethcraft(){return hcraft;};
	OBJHANDLE gethtarget(){return hmajtarget;};//Return target handle
	void sethandles(OBJHANDLE thmajor, OBJHANDLE thminor, OBJHANDLE thtarget, OBJHANDLE thcraft, OBJHANDLE thbase);
	void sethmajor(OBJHANDLE handle);
	virtual bool sethminor(OBJHANDLE handle);
	bool sethminorstd(OBJHANDLE handle);//Standard operations to set hminor

	bool sethmajtarget(OBJHANDLE handle);
	void sethcraft(OBJHANDLE handle);
	void sethbase(OBJHANDLE handle);
	void sethelp(char *help1,char *help2,char *help3,char *help4,char *help5);
	void gethelp(char *help1,char *help2,char *help3,char *help4,char *help5) const;
};

#endif