#ifndef __MFDVARIABLE_H
#define __MFDVARIABLE_H
#include "mfdvarhandler.h"
#include "doublelink.h"
class cmdnugget;

class MFDvariable : listelement
{
private:
	int viewmode1,viewmode2;
	int execstatus,execcountdown;//Simple system to provide feedback message on pressing of EXE
	cmdnugget *inugget;
protected:
	char name[20]; //Name of variable as displayed on MFD
	char helpstring1[40]; //Help string
	char helpstring2[40];//Help string
	int adjmode; // Integer representing the adjustment mode - use depends on class
	bool showvariable;
public:
	MFDvariable();
	void setcmdnugget(cmdnugget *nugget);
	void execute();//Executes any command nugget installed.
	void initialise(class MFDvarhandler *vars,int viewmode1,int viewmode2);
	virtual ~MFDvariable();
	void setshow(bool value);
	bool getshow();
	bool showgeneric(HDC hDC,int width,int line, char *inbuff);
	virtual void showadjustment(HDC hDC, int width, int line) const; //Show the adjustment mode
	void getsaveline1(char *buffer) const;//Creates first part of save line
	void gethelpstrings(char *help1,char *help2) const;//Returns help strings
	void sethelpstrings(char *help1, char *help2);//Sets help strings
	void getname(char *buffer) const;
	virtual void getsaveline(char *buffer) const;//Creates second line of save - overloaded
	virtual bool loadvalue(char *buffer);//Set value according to string
	virtual void setadjmode(int tadjmode);//Sets the adjustment mode
	virtual void ch_adjmode(); // Change the adjustment mode
	virtual void chm_adjmode();
	int getadjmode() const; //Get the current adjustment mode
	virtual int getadjtype() const;//Get the correct adjustment method
	// Following four implemented to do nothing much - designed to overload
	virtual bool inc_variable(); //Increase the variable
	virtual bool dec_variable(); //Decrease the variable
	virtual bool flip_variable(); // Change the variable in discrete amounts
	virtual bool flipback_variable();//Change the variable in discrete amounts
	virtual bool show(HDC hDC, int width, int line);
	virtual double getvalue() const; // Virtual functions for data access - implemented as safety values here
	virtual int getivalue() const;
	virtual void getsincos(double *sin, double *cos) const;
	virtual double getsin() const;
	virtual double getcos() const;
	virtual void setvalue(double tvalue);
	virtual void setivalue(int tvalue);
	virtual void sethandle(OBJHANDLE tpointer);
	virtual void setall(class MFDvariable *var);
	virtual OBJHANDLE gethandle() const;
	friend class MFDvarhandler;
};

#endif