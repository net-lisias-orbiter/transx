#ifndef __MFDVARHANDLER_H
#define __MFDVARHANDLER_H

#include "mfdvariable.h"
#include "doublelink.h"


class MFDvarhandler{
private:
	class dbllist listhead;
	class dblliter *variterator;
	class MFDvariable *currvariable[5]; //The current variable for each viewmode
	class MFDvariable *current;//The current variable in the current viewmode
	int currviewmode;
public:
	void addtolist(class MFDvariable *item);
	bool crosscopy(MFDvarhandler &othervars);
	//void removefromlist(MFDvariable *item);
	void setprevcurrent(int viewmode);
	void setnextcurrent(int viewmode);
	void saveallvariables(FILEHANDLE scn);
	bool loadallvariables(FILEHANDLE scn);
	class MFDvariable* getcurrent(int viewmode);
	MFDvarhandler();//Sets MFDvariable list to 0
	~MFDvarhandler();//Deletes all MFDvariables created through the class
	void deleteall(); // Deletes and resets MFD variable system - typically following inability to assign space
	void resetvars();
};

#endif