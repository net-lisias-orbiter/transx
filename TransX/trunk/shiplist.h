#ifndef __SHIPLIST_H
#define __SHIPLIST_H

#include "viewstate.h"
//#include "transxstate.h"
#include "transx.h"

class transxstate;

class shipptrs
{
	shipptrs();
	shipptrs(OBJHANDLE hcraft);
	static shipptrs *first;
	shipptrs *previous,*next;
	static shipptrs *current;//Used in background task
	void subcreate();
	class viewstate *mfdlist[20];
	class transxstate *state;
	char shipname[50];//ship name - needs to be long enough to have good chance of being complete
	static bool saved;//whether a save has been performed since last timestep
public:
	static void refreshsave(){saved=false;};
	static void saveallships(FILEHANDLE scn);//saves all ships to scenario file
	static void restoreallships(FILEHANDLE scn);//restores all ships from scenario file
	static void destroyshipptrs();//Clears the entire linked list
	static void backgroundaction();//Background task to check whether craft needs focus changed
	static class shipptrs *findship(OBJHANDLE hcraft);//Finds a ship from a pointer, if available
	static class shipptrs *findship(char *tname);//Finds a ship from a name, if available
	void savecurrent(FILEHANDLE scn);//saves current ship
	void restorecurrent(FILEHANDLE scn);//restores current ship
	void downshift();//Called when all viewstates on a vessel are asked to change their pointed position
	void resetshift();//Called to set all viewstates to position 1
	static class shipptrs *getshipptrs();//Gets ship pointer set for current focus vessel, creating new if reqd
	class transxstate *gettransxstate(){return state;};//returns transxstate for ship
	class viewstate *getviewstate(int mfdpos,TransxMFD *mfdptr);//returns viewstate for ship
	char *getname(){return shipname;};//hands over pointer to internal name buffer
	~shipptrs();
	friend class shipptr_itr;
};

class shipptr_itr
{
	class shipptrs *current;
public:
	class shipptrs* getfirst(){return current=shipptrs::first;};
	shipptr_itr(){getfirst();};
	class shipptrs* getcurrent(){return current;};
	class shipptrs* getnext();
	class shipptrs* getprev();
};


#endif //__SHIPLIST_H
