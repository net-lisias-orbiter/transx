#ifndef __CMDNUGGET_H
#define __CMDNUGGET_H

class MFDvariable;

class cmdnugget
//This virtual base class is an interface for passing commands as objects.
{
protected:
	class MFDvariable *ivar;//Internal MFDvariable pointer - not owned here
public:
	cmdnugget(){ivar=NULL;};
	virtual void setmfdvariable(class MFDvariable *mfdvar){ivar=mfdvar;};//sets reference to the mfdvariable
	virtual ~cmdnugget(){};
	virtual void execute() = 0;
};


#endif