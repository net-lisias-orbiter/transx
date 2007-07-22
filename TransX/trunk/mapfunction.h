#ifndef __MAPFUNCTION_H
#define __MAPFUNCTION_H

#include "mfdfunction.h"
#include "orbitelements.h"

typedef struct {
	OBJHANDLE bodyhandle;
	int parentbody;
	int peerring;
	int backpeerring;
	int moonring;
	int backmoonring;
	int overflow;
	double soisize2;
	double gravbodyratio2;
	double mass;
} GBODY;

typedef struct {
	OBJHANDLE bodyhandle;
	int pointer;
} HASHENTRY;

class mapfunction: public MFDFunction
{
private:
	GBODY *primaryarray, *secondaryarray,*array,*inarray,*outarray;
	int totalbodies;
	int actionstage,actionloop;
	bool initialised;
	static class mapfunction *themap;
	HASHENTRY hashtable[101];
	void getallhandles();
	void sorthandles();
	void mergelists(GBODY *list1, GBODY *endlist1, GBODY *list2, GBODY *endlist2, GBODY *listout);
	void initialisehashtable();
	void homegbody(int listmember, OBJHANDLE handle);
	int getbodybyhandle(OBJHANDLE handle);
	void populatehashtable();
	void findmajor(int position);
	mapfunction();
	~mapfunction();
	VECTOR3 getweightedvector(OBJHANDLE, void(OBJHANDLE, VECTOR3*));
public:
	static class mapfunction *getthemap();
	virtual void dolowpriaction();
	bool getinitialised(){return initialised;};
	void getorbit(OBJHANDLE handle, ORBIT *orbit);
	OBJHANDLE getmajor(OBJHANDLE handle);//Gets central body that handle orbits
	OBJHANDLE getfirstmoon(OBJHANDLE handle);//Gets heaviest moon of handle
	OBJHANDLE getlastmoon(OBJHANDLE handle);//Gets lightest moon of handle
	OBJHANDLE getnextpeer(OBJHANDLE handle);//Gets lighter peer in mass order (a peer is a body orbiting same major as handle)
	OBJHANDLE getpreviouspeer(OBJHANDLE handle);//Gets heavier peer in mass order
	double getsoisize(OBJHANDLE handle);//Gets the size of the SOI for this body
	OBJHANDLE getcurrbody(OBJHANDLE vessel);//Get current body for arbitrary vessel
	friend class shipptrs;//Allows the map to be deleted by this object
	VECTOR3 getbarycentre(OBJHANDLE body); // Gets the global pos of the barycentre of the system with body as the main object
	VECTOR3 getbarycentrevel(OBJHANDLE body); // Gets the global velocity of the barycentre of the system with body as the main object
};

#endif