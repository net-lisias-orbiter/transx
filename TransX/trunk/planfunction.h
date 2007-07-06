#ifndef __PLANFUNCTION_H
#define __PLANFUNCTION_H

#include "mfdvarhandler.h"
#include "mfdvartypes.h"
//#include "basefunction.h"
#include "orbitelements.h"

class basefunction;

class plan
{
public:
	virtual void getplanorbit(ORBIT *planorbit) = 0;//All plans can return an orbit to the main function
	virtual bool maingraph(HDC hDC,GRAPH *graph,basefunction *base){return true;};
	virtual int getplanid() = 0;
	virtual class plan *clone(class MFDvarhandler *vars,basefunction *base);//returns a pointer to a clone of this plan
	virtual class plan *iclone() = 0; 
	virtual void graphscale(GRAPH *graph){};//Use to scale graph for later update
	virtual void graphupdate(HDC hDC, GRAPH *graph,basefunction *base){};//Update graphs
	virtual void wordupdate(HDC hDC,int width, int height, basefunction *base){};//Update text
	virtual bool init(class MFDvarhandler *vars, basefunction *base) = 0;//Initialises plan - sets up variableset
	virtual void calculate(class MFDvarhandler *vars,basefunction *base) = 0;
	virtual double getentryvelocity(){return 0;};
	virtual double getratio2radius(){return 0;};
	virtual VECTOR3 getvelocityvector();
	virtual void getlabel(char *buffer) = 0;
	virtual void getviewname(char *buffer) = 0;
	virtual double geteventtime(){return 0;};
	virtual ~plan(){};
};

class minorejectplan:public plan
{
public:
	virtual int getplanid(){return 1;};
	virtual void getlabel(char *buffer){strcpy(buffer,"Plan:Escape");};
	virtual void getviewname(char *buffer){strcpy(buffer,"View:Escape Plan");};
	virtual void getplanorbit(ORBIT *tplanorbit);
	virtual void graphscale(GRAPH *graph);//Use to scale graph for later update
	virtual bool maingraph(HDC hDC,GRAPH *graph, basefunction *base);
	virtual void wordupdate(HDC hDC, int width, int height, basefunction *base);
	virtual void graphupdate(HDC hDC,GRAPH *graph,basefunction *base);
	virtual bool init(class MFDvarhandler *vars, basefunction *base);
	virtual void calculate(class MFDvarhandler *vars,basefunction *base);
	virtual ~minorejectplan(){};
	virtual class plan *iclone();
private:
	ORBIT planorbit;
	basefunction *ibase;
	MFDvarfloat m_ped;
	MFDvarangle m_ejorient;
	MFDvardiscrete m_equatorial;
};

class majejectplan:public plan
{
public:
	virtual void getplanorbit(ORBIT *tplanorbit){*tplanorbit=planorbit;};

	virtual void graphscale(GRAPH *graph);//Use to scale graph for later update
	virtual void graphupdate(HDC hDC,GRAPH *graph,basefunction *base);
	virtual void wordupdate(HDC hDC,int width, int height, basefunction *base);
	virtual void calculate(class MFDvarhandler *vars,basefunction *base);
	virtual VECTOR3 getvelocityvector();
	virtual double geteventtime(){return timefromnow;};
	virtual ~majejectplan(){};
protected:
	ORBITTIME minorplanetattime;
	virtual void calcejectvector(const VECTOR3 &rminplane,const VECTOR3 &minorvel, double inheritedvelocity) = 0;
	MFDvarMJD m_ejdate;
	MFDvardiscrete m_inheritvel;
	double ratioorbit;
	double timefromnow;
	ORBIT planorbit;
	VECTOR3 ejectvector;
};

class majorejectplan:public majejectplan
{
private:
	MFDvarfloat m_prograde,m_outwardvel,m_chplvel;
	virtual bool init(class MFDvarhandler *vars, basefunction *base);
	virtual void calcejectvector(const VECTOR3 &rminplane,const VECTOR3 &minorvel, double inheritedvelocity);
public:
	virtual int getplanid(){return 2;};
	virtual void getlabel(char *buffer){strcpy(buffer,"Plan:Eject");};
	virtual void getviewname(char *buffer){strcpy(buffer,"View:Eject Plan");};
	virtual class plan *iclone(){return new majorejectplan();};
	virtual ~majorejectplan(){};
};

class slingejectplan:public majejectplan
{
private:
	MFDvarfloat m_totalvel;
	MFDvarangle m_outwardangle, m_incangle;


	virtual bool init(class MFDvarhandler *vars, basefunction *base);

	void calcejectvector(const VECTOR3 &rminplane,const VECTOR3 &minorvel, double inheritedvelocity);
public:
	virtual ~slingejectplan(){};
	virtual class plan *iclone(){return new slingejectplan();};
	virtual int getplanid(){return 3;};
	virtual void getlabel(char *buffer){strcpy(buffer,"Plan:Sling Direct");};
	virtual void getviewname(char *buffer){strcpy(buffer,"View:Sling Direct");};

};

class encounterplan:public plan
{
private:
	virtual bool init(class MFDvarhandler *vars,class basefunction *base);
	MFDvardiscrete m_drawbase;
	VECTOR3 baseposition;
	bool drawnbase;
public:
	virtual class plan *iclone(){ return new encounterplan();};
	virtual int getplanid(){return 5;};
	virtual void getplanorbit(ORBIT *planorbit);
	virtual void calculate(class MFDvarhandler *vars,basefunction *base){};
	encounterplan(){drawnbase=false;};
	virtual void graphupdate(HDC hDC, GRAPH *graph, basefunction *base);
	virtual void wordupdate(HDC hDC, int width, int height, basefunction *base);
	virtual void getlabel(char *buffer){strcpy(buffer,"Plan:Encounter");};
	virtual void getviewname(char *buffer){strcpy(buffer,"View:Encounter");};
	virtual ~encounterplan(){};
};
	
class slingshot:public plan
{
public:
	slingshot(){periapsisguess=-1;inwardvelocity=0;ejectvelocity2=0;};
	virtual class plan *iclone(){return new slingshot();};
	virtual void getplanorbit(ORBIT *tplanorbit);
	virtual void graphscale(GRAPH *graph);//Use to scale graph for later update
	virtual void graphupdate(HDC hDC, GRAPH *graph,basefunction *base);
	virtual void wordupdate(HDC hDC, int width, int height, basefunction *base);
	virtual void calculate(class MFDvarhandler *vars,basefunction *base);
	virtual bool maingraph(HDC hDC,GRAPH *graph,basefunction *base);//bool returns whether base should draw a graph
	virtual double geteventtime(){return eventtime;};
	virtual double getentryvelocity(){return inwardvelocity;};
	virtual double getratio2radius(){return ratiotoradius;};
	virtual int getplanid(){return 4;};
	virtual void getlabel(char *buffer){strcpy(buffer,"Plan:Slingshot");};
	virtual void getviewname(char *buffer){strcpy(buffer,"View:Slingshot");};
private:
	virtual bool init(class MFDvarhandler *vars, class basefunction *base);
	MFDvardiscrete m_selectorbit;
	double goodness;
	double inwardvelocity,ejectvelocity2;
	double eventtime;
	double ratiotoradius;//Ratio of periapsis distance to the planet radius
	double periapsisguess;//guess of periapsis of slingshot
	ORBIT planorbit;

};


#endif