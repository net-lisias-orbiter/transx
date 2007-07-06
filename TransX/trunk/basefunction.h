//#include "transxstate.h"

#ifndef __BASEFUNCTION_H
#define __BASEFUNCTION_H

#include "mfdvartypes.h"
#include "TransXFunction.h"
#include "planfunction.h"

class transxstate;


class basefunction : public TransXFunction
{
private:
	void switchplantype();
	class basefunction *previousfunc,*nextfunc;
	void setnextfunc(class basefunction *temp){nextfunc=temp;};
	void setpreviousfunc(class basefunction *temp){previousfunc=temp;};
	void switchmanoevremode();
	void switchadvanced();
	void loadplan(int plan);
	void Getmode2hypo(VECTOR3 *targetvel);
	virtual bool initialisevars();
	virtual void dolowpriaction();
	int iplantype,iplan;
	bool interceptflag;
	bool previousexists;
	class mapfunction *mappointer;
	class plan *planpointer;
	int interceptwith;
	ORBITTIME mode2orbittime,deltavel;
protected:
	INTERCEPT primary, pecorrintercept;
	ORBIT craft, rmin, basisorbit, hypormaj, craftincontext, target, context;
	OBJHANDLE hcontext;
	GRAPH graph;
	bool hypoorcraft, showintercept, showrinc;
	MFDvarmoon m_target,m_minor;
	MFDvardiscrete m_planauto, m_plantype,m_planinitial,m_planthrough,m_planminor;
	MFDvardiscrete m_manoevremode,m_updbaseorbit;
	MFDvarfloat m_prograde,m_outwardvel,m_chplvel;
	MFDvarMJD m_ejdate;
	MFDvardiscrete m_intwith,m_graphprj,m_scale,m_advanced;
	MFDsemiintdiscrete m_orbitsahead;//Overloaded double
	double oldorbitsahead;
	int gettargettype(){return m_plantype;};
	void autoplan();
public:
	class basefunction *getpreviousfunc(){return previousfunc;};
	class basefunction *getnextfunc(){return nextfunc;};
	void basefunction::onplaceindeletebuffer();//Do not call unless you know what it's for.
	void calculate(VECTOR3 *targetvel);
	int calcnewview(int oldview,bool firststage);
	bool soistatus();
	void processvisiblevars();
	void updateplan();//Actually changes the plan
	void setplanstate(int plantype,int plan);//selects the type of plan to be carried out
	void setnextplanstate(int plantype,int plan,int targettype);
	void getplanstate(int *xplantype,int *xplan,int *targettype);
	virtual void doupdate(HDC hDC, int tw, int th,int viewmode);

	virtual void saveself(FILEHANDLE scn);
	virtual void restoreself(FILEHANDLE scn);
	void handlesfornextfunction(OBJHANDLE *thmajor, OBJHANDLE *thminor);
	class plan *getplanpointer(){return planpointer;};
	ORBIT getcraftorbit(){return craft;};
	ORBIT getpassforwardorbit();
	void getcraftorbitattarget(ORBIT *tcraft);
	ORBIT getcontextorbit(){return context;};//Returns copy of context orbit
	ORBIT getminororbit(){return rmin;};
	ORBIT getmanoevreorbit(){return hypormaj;};
	ORBIT gettargetorbit(){return target;};//Returns copy of target orbit
	basefunction(class transxstate *tstate, class basefunction *tpreviousfunc,OBJHANDLE thmajor, OBJHANDLE thminor,OBJHANDLE thcraft);
	basefunction(class transxstate *tstate, class basefunction *tpreviousfunc, class basefunction *templbase, OBJHANDLE thcraft);
	~basefunction();
};

#endif