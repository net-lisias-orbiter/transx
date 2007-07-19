#define STRICT

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "orbitersdk.h"
#include "mfd.h"
#include "mapfunction.h"

mapfunction *mapfunction::themap=NULL;

mapfunction::~mapfunction()
{
	delete[] primaryarray;
	delete[] secondaryarray;
	themap=NULL;
}

mapfunction::mapfunction()
{
	primaryarray=NULL;
	secondaryarray=NULL;
	initialised=false;
	actionstage=0;//0th stage of initialisation
	actionloop=0;
	addaction(0);//Place single low priority action into queue

}

class mapfunction *mapfunction::getthemap()
{
	if (themap==NULL)
	{
		themap=new mapfunction();
	}
	return themap;
}


void mapfunction::dolowpriaction()
{//Function is called multiple times - addaction(0) prompts it to be recalled
	//System allows large initialisation computation to be broken up into parts
	// and staged through multiple timesteps
	switch (actionstage){
	case 0:
		getallhandles();
		actionstage++;
		addaction(0);
		break;
	case 1:
		sorthandles();
		actionstage++;
		addaction(0);
		break;
	case 2:
		initialisehashtable();
		actionstage++;
		addaction(0);
		break;
	case 3:
		populatehashtable();
		actionstage++;
		addaction(0);
		break;
	case 4:
		findmajor(actionloop);
		if (++actionloop==totalbodies)
		{
			initialised=true;
			actionstage++;//For safety
			return;//Last one!
		}
		addaction(0);
		break;
	}
}


void mapfunction::getorbit(OBJHANDLE handle, ORBIT *orbit)
{
	orbit->init(getmajor(handle),handle);
}


void mapfunction::getallhandles()
{
	totalbodies=oapiGetGbodyCount();//Find how many gbodies there are
	primaryarray=new GBODY[totalbodies];//Create both arrays required
	secondaryarray=new GBODY[totalbodies];
	GBODY *temp=primaryarray;//Populate primaryarray with gbodies
	for (int a=0;a<totalbodies;a++)
	{
		temp->bodyhandle=oapiGetGbodyByIndex(a);
		temp->mass=oapiGetMass(temp->bodyhandle);
		temp->overflow=-1;//Used in hash table
		temp++;
	}
	inarray=primaryarray;
	outarray=secondaryarray;
	array=inarray;
}

void mapfunction::sorthandles()
{//Merge sort
	int listlength=1;
	int jumplength;
	GBODY *inlist1,*inlist2,*endinlist,*outlist,*endinlist1,*endinlist2;

	//Initialise scan
	while (listlength<totalbodies)
	{
		endinlist=inarray+totalbodies;

		inlist1=inarray;
		outlist=outarray;
		jumplength=listlength+listlength;	
	
		while (inlist1<endinlist)
		{
			inlist2=inlist1+listlength;
			endinlist1=inlist2;
			endinlist2=inlist2+listlength;
			if (endinlist1>endinlist) 
			{
				endinlist1=inlist2=endinlist2=endinlist;
			}
			if (endinlist2>endinlist)
			{
				endinlist2=endinlist;
				if (endinlist2<inlist2) inlist2=endinlist2;
			}
			mergelists(inlist1,endinlist1,inlist2,endinlist2,outlist);
			inlist1+=jumplength;
			outlist+=jumplength;
		}
		listlength=listlength+listlength;
		inlist1=outarray;
		outarray=inarray;
		inarray=inlist1;
	}
	array=inarray;//Place where stored sorted array is
}

void mapfunction::initialisehashtable()
{
	HASHENTRY *pointer=hashtable;
	for (int a=0;a<MAX_BODIES;a++)
	{
		pointer->bodyhandle=NULL;
		pointer->pointer=-1;
		pointer++;
	}
}

void mapfunction::populatehashtable()
{
	for (int a=0;a<totalbodies;a++)
	{
		homegbody(a,(array+a)->bodyhandle);
	}
}


void mapfunction::homegbody(int listmember, OBJHANDLE handle)
{
	int hashpoint=int(handle)%MAX_BODIES;//Convert pointer, and get position in hashtable
	if (hashtable[hashpoint].bodyhandle==NULL)
	{
		//Enter pointer to ourselves directly in hash table
		hashtable[hashpoint].bodyhandle=(array+listmember)->bodyhandle;
		hashtable[hashpoint].pointer=listmember;
		return;
	}
	//Already an entry there - look for new entry
	int tempmember=hashtable[hashpoint].pointer;
	while ((array+tempmember)->overflow!=-1)
	{
		tempmember=(array+tempmember)->overflow;
	}
	(array+tempmember)->overflow=listmember;
}
	
int mapfunction::getbodybyhandle(OBJHANDLE handle)
{
	int hashpoint=int(handle)%MAX_BODIES;
	if (hashtable[hashpoint].bodyhandle==NULL) return -1;//Handle not in table
	if (hashtable[hashpoint].bodyhandle==handle)
	{
		return hashtable[hashpoint].pointer;//Found handle
	}
	int checkposition=hashtable[hashpoint].pointer;
	do 
	{
		if((array+checkposition)->bodyhandle==handle) return checkposition;//Found handle
		checkposition=(array+checkposition)->overflow;
	}
	while (checkposition!=-1);
	return -1;//Handle not in table
}

void mapfunction::findmajor(int position)//Assumes majors already found for bodies of less than position parameter
{
	VECTOR3 currentpos,bodypos,relvector;
	double distance2,currdistance2;
	oapiGetGlobalPos((array+position)->bodyhandle,&currentpos);
	if (position==0)
	{//the central body
		array->soisize2=1e100;//Infinite SOI
		array->parentbody=0;//Sun is its own parent!
		array->peerring=array->moonring=-1;
		array->backmoonring=array->backpeerring=-1;
		return;
	}
	int currentsoi=0;//In solar influence
	int moonringpos=0;//Currently assume ourselves to be the first planet in system!
	for (int a=0;a<position;a++)
	{
		if ((array+a)->parentbody==currentsoi)
		{
			oapiGetGlobalPos((array+a)->bodyhandle,&bodypos);
			moonringpos=a;
			relvector=currentpos-bodypos;
			distance2=dotproduct(relvector,relvector);
			if (distance2<(array+a)->soisize2)//In this soi
			{
				currentsoi=a;
				currdistance2=distance2;
			}

		}
	}
	//We now have the actual body for this object
	(array+position)->parentbody=currentsoi;
	//Insert ourselves onto moonring chain
	if (moonringpos==currentsoi)//first moon
	{
		(array+moonringpos)->moonring=position;
		(array+position)->backpeerring=-1;//Largest moon
	}
	else
	{
		(array+moonringpos)->peerring=position;//Add ourselves to the moon chain
		(array+position)->backpeerring=moonringpos;
	}
	(array+currentsoi)->backmoonring=position;//We are (at the moment) last member of moon chain
	(array+position)->moonring=-1;//No moons of our own yet
	(array+position)->backmoonring=-1;
	(array+position)->peerring=-1;//No peers beyond our size

	(array+position)->gravbodyratio2=pow((array+position)->mass/(array+currentsoi)->mass,0.8);
	(array+position)->soisize2=currdistance2*(array+position)->gravbodyratio2;//Internal soi size
}

OBJHANDLE mapfunction::getcurrbody(OBJHANDLE vessel)//Finds current body for current focus vessel
{
	if (!initialised || vessel==NULL) return NULL;
	VECTOR3 currentpos,bodypos,parentpos,relvector;
	double distance2,bodyfromparent2;
	oapiGetGlobalPos(vessel,&currentpos);
	int currentsoi=0;
	int a=1;
	while (a!=-1)//No more objects to check
	{//Recomputes distance as time may have passed since initialisation
		oapiGetGlobalPos((array+a)->bodyhandle,&bodypos);
		oapiGetGlobalPos((array+(array+a)->parentbody)->bodyhandle,&parentpos);
		relvector=parentpos-bodypos;
		bodyfromparent2=vectorsize2(relvector);
		relvector=currentpos-bodypos;
		distance2=vectorsize2(relvector);
		if (distance2<(array+a)->gravbodyratio2*bodyfromparent2)//In this soi
		{
			currentsoi=a;
			a=(array+a)->moonring;
		}
		else
		{
			a=(array+a)->peerring;//Move to next possible object
		}
	}
	//We now have the actual body for this object
	return (array+currentsoi)->bodyhandle;
}
	
double mapfunction::getsoisize(OBJHANDLE handle)
{
	int position=getbodybyhandle(handle);
	if (position==-1) return 0;
	int majorbody=(array+position)->parentbody;
	OBJHANDLE parenthandle=(array+majorbody)->bodyhandle;
	VECTOR3 vecradius;
	double radius;
	if (parenthandle==handle)
	{
		return 1e80;//virtually infinite SOI for largest body
	}
	else
	{
		oapiGetRelativePos(parenthandle,handle,&vecradius);
		radius=sqrt(dotproduct(vecradius,vecradius)*(array+position)->gravbodyratio2);
	}
	return radius;
}


OBJHANDLE mapfunction::getfirstmoon(OBJHANDLE handle)
{
	int position=getbodybyhandle(handle);
	if (position==-1) return NULL;
	int moon=(array+position)->moonring;
	if (moon==-1) return NULL;
	return (array+moon)->bodyhandle;
}

OBJHANDLE mapfunction::getlastmoon(OBJHANDLE handle)
{
	int position=getbodybyhandle(handle);
	if (position==-1) return NULL;
	int moon=(array+position)->backmoonring;
	if (moon==-1) return NULL;
	return (array+moon)->bodyhandle;
}

OBJHANDLE mapfunction::getnextpeer(OBJHANDLE handle)
{
	int position=getbodybyhandle(handle);
	if (position==-1) return NULL;
	int peer=(array+position)->peerring;
	if (peer==-1) return NULL;
	return (array+peer)->bodyhandle;
}

OBJHANDLE mapfunction::getpreviouspeer(OBJHANDLE handle)
{
	int position=getbodybyhandle(handle);
	if (position==-1) return NULL;
	int peer=(array+position)->backpeerring;
	if (peer==-1) return NULL;
	return (array+peer)->bodyhandle;
}


OBJHANDLE mapfunction::getmajor(OBJHANDLE handle)
{
	int position=getbodybyhandle(handle);
	if (position==-1) return NULL;
	int parent=(array+position)->parentbody;
	return (array+parent)->bodyhandle;
}

void mapfunction::mergelists(GBODY *list1, GBODY *endlist1, GBODY *list2, GBODY *endlist2, GBODY *listout)
{
	while (true)
	{
		if (list1==endlist1 && list2==endlist2) break;
		if (list1==endlist1 && list2<endlist2)
		{
			*listout=*list2;
			listout++;
			list2++;
		}
		if (list2==endlist2 && list1<endlist1)
		{
			*listout=*list1;
			listout++;
			list1++;
		}
		if (list1<endlist1 && list2<endlist2)
		{
			if (list2->mass>=list1->mass)
			{
				*listout=*list2;
				listout++;
				list2++;
			}
			else
			{
				*listout=*list1;
				listout++;
				list1++;
			}
		}
	}
}

