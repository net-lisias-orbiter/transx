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

#ifndef __TRANSXFUNCTION_H
#define __TRANSXFUNCTION_H

#include "mfdfunction.h"
#include "parser.h"
#include "mfdvarhandler.h"
#include "intercept.h"
#include "graph.h"

#define MAX_HELPSTRING_LENGTH	40
#define PEN_DEFAULT				Grey
#define PEN_PLANET				Grey
#define PEN_HYPO				Yellow
#define PEN_CRAFT				Green
#define PEN_ATMOSPHERE			Blue

class transxstate;

class TransXFunction: public MFDFunction
{
public:
	enum Pen
	{ 
		Hollow = -1,
		Green = 0,
		Blue,
		Yellow,
		Red,
		Grey,
		White,
		NUM_PENS
	};

private:
	void initpens(void);
	virtual bool initialisevars() = 0;
	void deletepens();
	Parser parser;
	char helpstring1[MAX_HELPSTRING_LENGTH],
		 helpstring2[MAX_HELPSTRING_LENGTH],
		 helpstring3[MAX_HELPSTRING_LENGTH],
		 helpstring4[MAX_HELPSTRING_LENGTH],
		 helpstring5[MAX_HELPSTRING_LENGTH];
protected:
	OBJHANDLE hmajor, hminor, hmajtarget, hcraft, hbase;//Standard set of OBJHANDLES for the TransX MFD
	double gravbodyratio; //Specific computation associated with hmajor+hminor
	double simstartMJD; //Time at which current scenario commenced
	class MFDvarhandler vars; //Variable set associated with this MFDFunction
	class transxstate *state; //Pointer to calling transxstate
	HPEN pens[NUM_PENS];//Replacement pens for MFD
	HBRUSH brush[NUM_PENS];

public:
	HPEN SelectDefaultPen(HDC hDC, int value);
	HBRUSH TransXFunction::SelectBrush(HDC hDC, int value);

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