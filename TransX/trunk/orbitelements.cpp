#define STRICT

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "orbitersdk.h"
#include "mfd.h"
#include "graph.h"
#include "orbitelements.h"
#include "intercept.h"
#include "mapfunction.h"

extern double debug;

void orbitelements::gettimeorbit(int *orbitnumber,double *orbittime, double timefromnow) const
{
	*orbittime=orbitconstant*2*PI;
	*orbitnumber=int(floor(timefromnow/ (*orbittime)));
}

double orbitelements::getvelocityatdist(double tradius)
{
	double energy=planet/semimajor+2*planet/tradius;
	energy=((energy>0) ? sqrt(energy) : 0);
	return energy;
}
	


void orbitelements::minortomajorinit(const ORBIT &craftinrmin, const ORBIT &rmininrmaj, double soisize)
//Now aims close to transfer coordinates very close to the minor planet - should be very accurate!
{
	if (craftinrmin.eccentricity<1 || !craftinrmin.valid || !rmininrmaj.valid)
	{
		setinvalid();
		return;
	}
	double costhi=craftinrmin.angularmomentum2/(craftinrmin.planet*3*soisize*craftinrmin.eccentricity)-1/craftinrmin.eccentricity;//take vectors at 3x SOI radius
	double sinthi=1-costhi*costhi;
	if (sinthi>0)
	{
		sinthi=sqrt(sinthi); //Positive thi = outward motion
	}
	else
	{
		sinthi=0;
	}
	VECTOR3 pos, vel, plpos, plvel;
	pos=(craftinrmin.majoraxis*costhi+craftinrmin.minoraxis*sinthi)*10*soisize; //Position vector
	double time=craftinrmin.simpletimetoradius(10*soisize); //elapsed time from periapsis to soi

	// Using the velocity for INFINITY rather than current position as it gives better results for onward orbit in most cases
	// Note that this may be non-standard for some applications
	costhi=-1/craftinrmin.eccentricity;
	sinthi=sqrt(1-costhi*costhi); //Positive thi again
	
	//At infinity position and velocity vectors are aligned
	vel=(craftinrmin.majoraxis*costhi+craftinrmin.minoraxis*sinthi)*sqrt(craftinrmin.planet/craftinrmin.semimajor);

	//Calculate position at time zero - without the planet!
	pos=pos-vel*time;
	
	//Get the planet's position at the time of ejection
	time=craftinrmin.gettimestamp()-craftinrmin.deltatime;
	rmininrmaj.timetovectors(time-rmininrmaj.gettimestamp(), &plpos, &plvel);//works well in this context
	pos=pos+plpos;// Convert to rmaj coordinates using future position of planet at time 'time'
	vel=vel+plvel;

	// Get new orbital elements
	init(pos,vel,time, rmininrmaj.planet);
}

double orbitelements::getinfinityvelocity() const
{
	if (semimajor>1) return sqrt(planet/semimajor);
	return -1;
}

void orbitelements::majortominorinit(OBJHANDLE target, OBJHANDLE object, const class intercept &closestapproach, double soisize)
{
	if (target==NULL)
	{
		setinvalid();
		return;
	}
	//Get position and velocity of object relative to target
	VECTOR3 minorpos,minorvel;
	closestapproach.getrelpos(&minorpos);
	double pos2=vectorsize2(minorpos);
	if (pos2>soisize*soisize)//outside SOI, no intercept
	{
		setinvalid();
		return;
	}
	closestapproach.getrelvel(&minorvel);

	//First use dotproduct to get to closest approach exactly
	double minorvelsize=vectorsize(minorvel);
	double firsttime=dotproduct(minorpos,minorvel)/(minorvelsize*minorvelsize);//Small correction - not the problem, though
	minorpos=minorpos-minorvel*(dotproduct(minorpos,minorvel)/minorvelsize);
	//Now precisely at closest approach
	//Now use pythagoras to find required length back to 10xSOI
	double backradius=100*soisize*soisize;
	//Now get actual distance from minor body - compensates for any speedup as planet is approached
	VECTOR3 currvector;
	if (object!=NULL)//can be a legal call
	{
		oapiGetRelativePos(object, target,&currvector);
		double actdistance2=vectorsize2(currvector);
		if (actdistance2<backradius) backradius=actdistance2;
		backradius-=dotproduct(minorpos,minorpos);//Make allowance for distance of closest approach
	}
	backradius=sqrt(backradius);
	double timeoffset=backradius/minorvelsize-firsttime;
	minorpos=minorpos-minorvel*(1/minorvelsize)*backradius;
	//Generate new orbit
	init(minorpos,minorvel,closestapproach.gettimeintercept()-timeoffset,oapiGetMass(target)*GRAVITY);
}



void orbitelements::getinfinityvelvector(bool outward,VECTOR3 *velocity) const
{
	if (!valid || eccentricity<1) return;
	double costhi=-1/eccentricity;
	double sinthi=sqrt(1-costhi*costhi);
	if (outward)
	{
		*velocity=(majoraxis*costhi+minoraxis*sinthi)*sqrt(planet/semimajor);
	}
	else
	{
		*velocity=(minoraxis*sinthi-majoraxis*costhi)*sqrt(planet/semimajor);
	}
}



void orbitelements::radiustovectors(double radius, bool outward, VECTOR3 *position, VECTOR3 *velocity) const
{
	if (!valid) return;
	double costhi=(angularmomentum2/(radius*planet)-1)/eccentricity;
	double sinthi=sqrt(1-costhi*costhi);
	if (!outward) sinthi=-sinthi;
	*position=majoraxis*(radius*costhi)+minoraxis*(radius*sinthi);
	VECTOR3 outvector=unitise(*position);
	VECTOR3 roundvector=unitise(crossproduct(planevector, outvector));
	double rndvel2=angularmomentum2/(radius*radius);
	double outvel=eccentricity*planet*sinthi/sqrt(angularmomentum2);
	*velocity=outvector*outvel+roundvector*sqrt(rndvel2);
}



void orbitelements::thitovectors(double costhi, double sinthi,VECTOR3 *position, VECTOR3 *velocity) const
// Finds position and velocity vectors at a given angle thi
// thi is the angle from periapsis, as measured from the centre of the body orbited
{
	if (!valid) return;
	double radius=angularmomentum2/(planet*(eccentricity*costhi+1));
	*position=majoraxis*(radius*costhi)+minoraxis*(radius*sinthi);
	VECTOR3 outward=majoraxis*costhi+minoraxis*sinthi;
	VECTOR3 roundward=minoraxis*costhi-majoraxis*sinthi;
	double angmom=sqrt(angularmomentum2);
	double outvel=eccentricity*planet*sinthi/angmom;
	*velocity=outward*outvel+roundward*(angmom/radius);
}



orbitelements::orbitelements()
{
	valid=false; //Not a valid orbit yet
}

orbitelements::orbitelements(OBJHANDLE hmajor, OBJHANDLE hminor)
{
	init(hmajor,hminor);
}



orbitelements::orbitelements(VECTOR3 rposition, VECTOR3 rvelocity, double gmplanet)
{
	init(rposition,rvelocity, gmplanet);
}

void orbitelements::init(OBJHANDLE hmajor, OBJHANDLE hminor)
{
	valid=false;
	if (hmajor !=NULL && hminor !=NULL && hmajor!=hminor)
	{
		// derive orbits of the barycentre of the minor about the major.
		// There seems to be a problem about deriving them about the barycentre of the major. I don't
		// know why it appears to throw the orbits off, so use the global pos and vel of the major

		VECTOR3 craftpos, craftvel;
		mapfunction *map = mapfunction::getthemap();
		VECTOR3 minbary, majbary, minvel, majvel;
		if(oapiGetObjectType(hmajor) == OBJTP_PLANET)
			majbary = map->getbarycentre(hmajor);
		else
			oapiGetGlobalPos(hmajor, &majbary);
		minbary = map->getbarycentre(hminor);
		craftpos = minbary - majbary;

		oapiGetGlobalVel(hmajor, &majvel);
		minvel = map->getbarycentrevel(hminor);
		craftvel = minvel - majvel;

		double gmmajor=GRAVITY*oapiGetMass(hmajor);
		double timestamp=oapiGetSimTime();
		init(craftpos, craftvel, timestamp, gmmajor);
	}
}

double orbitelements::gettimestamp() const
{
	return timestamp;
}

void orbitelements::init(const VECTOR3 &rposition, const VECTOR3 &rvelocity, double gmplanet)
{
	timestamp=oapiGetSimTime();
	init(rposition, rvelocity, timestamp, gmplanet);
}

void orbitelements::init(const VECTOR3 &rposition, const VECTOR3 &rvelocity, double ttimestamp, double gmplanet)
{
	valid=true;
	planevector=crossproduct(rposition,rvelocity);
	angularmomentum2=dotproduct(planevector, planevector); //Angular momentum squared (don't usually need it non-squared)

	double rvel2=dotproduct(rvelocity, rvelocity);
	double radius=vectorsize(rposition);
	eccvector=(rposition*(rvel2-gmplanet/radius)-rvelocity*dotproduct(rposition, rvelocity))*(1/gmplanet); //Eccentricity vector
	semimajor=gmplanet/(rvel2-2*gmplanet/radius); //Length of semimajor axis  - is NEGATIVE if orbit is elliptical, POSITIVE if hyperbolic
	eccentricity=vectorsize(eccvector); //Eccentricity of orbit
	majoraxis=unitise(eccvector); //Vector towards Periapsis
	minoraxis=unitise(crossproduct(planevector, majoraxis)); // Vector parallel to Minor axis of orbit - important for extracting vectors later
	currcosthi=dotproduct(rposition, majoraxis)/radius; //cos thi is angle from periapsis, as measured from planet centre
	currsinthi=dotproduct(rposition, minoraxis)/radius;
	currposition=rposition;
	currvelocity=rvelocity;
	planet=gmplanet;
	if (semimajor>0)
	{
		orbitconstant=-sqrt(semimajor*semimajor*semimajor/planet);
	}
	else
	{
		orbitconstant=sqrt(-semimajor*semimajor*semimajor/planet);
	}
	deltatime=0;
	deltatime=GetTimeToThi(currcosthi, currsinthi);
	timestamp=ttimestamp;
}

double orbitelements::simpletimetoradius(double radius) const
{
	double costhi,coscosh, sinsinh, e_angle, loc_deltatime;
	//Calculate costhi from radius
	costhi=(angularmomentum2/(radius*planet)-1)/eccentricity;
	//Can use radius in expression for coscosh, reducing error in hyperbolics dramatically
	coscosh=(eccentricity+costhi)*radius*planet/angularmomentum2;
	if (eccentricity<=1)
	{
		//Elliptical case
		double temp=1-coscosh*coscosh;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=acos(coscosh);
		}
		else
		{
			sinsinh=0;
			if (coscosh>0)
			{
				e_angle=0;
			}
			else
			{
				e_angle=PI;
			}
		}
	}
	else
	{
		//Hyperbolic case
		double temp=coscosh*coscosh-1;
		if (temp>0)
		{
			sinsinh=sqrt(coscosh*coscosh-1);
			e_angle=log(coscosh+sinsinh);
		}
		else
		{
			sinsinh=0;
			e_angle=0;
		}
	}
	loc_deltatime=orbitconstant*(e_angle-eccentricity*sinsinh); 
	return loc_deltatime;
}


double orbitelements::GetTimeToRadius(double radius, bool outward) const
{
	if (!valid) return 0;
	double costhi,coscosh, sinsinh, e_angle, loc_deltatime;
	//Calculate costhi from radius
	costhi=(angularmomentum2/(radius*planet)-1)/eccentricity;
	//Can use radius in expression for coscosh, reducing error in hyperbolics dramatically
	coscosh=(eccentricity+costhi)*radius*planet/angularmomentum2;
	if (eccentricity<=1)
	{
		//Elliptical case
		double temp=1-coscosh*coscosh;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=acos(coscosh);
		}
		else
		{
			sinsinh=0;
			if (coscosh>0)
			{
				e_angle=0;
			}
			else
			{
				e_angle=PI;
			}
		}
	}
	else
	{
		//Hyperbolic case
		double temp=coscosh*coscosh-1;
		if (temp>0)
		{
			sinsinh=sqrt(coscosh*coscosh-1);
			e_angle=log(coscosh+sinsinh);
		}
		else
		{
			sinsinh=0;
			e_angle=0;
		}
	}
	loc_deltatime=orbitconstant*(e_angle-eccentricity*sinsinh); 
	if (!outward) loc_deltatime=-loc_deltatime;
	loc_deltatime-=deltatime;
	if (loc_deltatime<0 && eccentricity<1)
	{
		loc_deltatime+=orbitconstant*(PI+PI); //If calculated time is in the past, add another orbit on_
		if (loc_deltatime<0) loc_deltatime+=orbitconstant*(PI+PI);
		if (loc_deltatime<0) loc_deltatime+=orbitconstant*(PI+PI);
	}
	return loc_deltatime;
}


double orbitelements::simpletimetothi(double costhi,double sinthi) const
//returns time from periapsis
{
	double coscosh;
	if (1+eccentricity*costhi<0)
	{
		//This is an error condition - substituting partially sensible result
		//This should never occur
		coscosh=1e8; //Very, very large figure
	}
	else
	{
		coscosh=(eccentricity+costhi)/(1+eccentricity*costhi);
	}
	double sinsinh, e_angle, loc_deltatime;
	if (eccentricity<=1)
	{
		//Elliptical case
		double temp=1-coscosh*coscosh;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=acos(coscosh);
		}
		else
		{
			sinsinh=0;
			if (coscosh>0)
			{
				e_angle=0;
			}
			else
			{
				e_angle=PI;
			}
		}
	}
	else
	{
		//Hyperbolic case
		double temp=coscosh*coscosh-1;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=log(coscosh+sinsinh);
		}
		else
		{
			sinsinh=0;
			e_angle=0;
		}
	}
	loc_deltatime=orbitconstant*(e_angle-eccentricity*sinsinh); 
	if (sinthi<0) return -loc_deltatime;
	return loc_deltatime;
}




double orbitelements::GetTimeToThi(double costhi, double sinthi,int fullorbits,int halforbits) const

//Gets time from present position to the position pointed to by cos and sin thi.

{
	if (!valid) return 0;
	double coscosh;
	if (1+eccentricity*costhi<0)
	{
		//This is an error condition - substituting partially sensible result
		//This should never occur
		coscosh=1e8; //Very, very large figure
	}
	else
	{
		coscosh=(eccentricity+costhi)/(1+eccentricity*costhi);
	}
	double sinsinh, e_angle, loc_deltatime;
	if (eccentricity<=1)
	{
		//Elliptical case
		double temp=1-coscosh*coscosh;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			if (fabs(coscosh)<1)
			{
				e_angle=acos(coscosh);
			}
			else
			{
				if (coscosh>0)
					e_angle=0;
				else
					e_angle=PI;
			}
		}
		else
		{
			sinsinh=0;
			if (coscosh>0)
			{
				e_angle=0;
			}
			else
			{
				e_angle=PI;
			}
		}
	}
	else
	{
		//Hyperbolic case
		double temp=coscosh*coscosh-1;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=log(coscosh+sinsinh);
		}
		else
		{
			sinsinh=0;
			e_angle=0;
		}
	}
	loc_deltatime=orbitconstant*(e_angle-eccentricity*sinsinh); 
	if (sinthi<0) loc_deltatime=-loc_deltatime;
	loc_deltatime-=deltatime;
	double orbittime=orbitconstant*(PI+PI);
	if (loc_deltatime<0 && eccentricity<1)
	{
		loc_deltatime+=orbitconstant*(PI+PI); //If calculated time is in the past, add another orbit on
		if (loc_deltatime<0) loc_deltatime+=orbittime;
		if (loc_deltatime<0) loc_deltatime+=orbittime;
	}//Adjusts to one orbit following present position

	if (loc_deltatime<orbitconstant*PI && halforbits>0) loc_deltatime+=orbittime;
	loc_deltatime+=orbittime*fullorbits;
	return loc_deltatime;//Adjusts this forward as prescribed
}

void orbitelements::vectortothi(const VECTOR3 &vector,double *costhi,double *sinthi) const
{
	double majorsize=dotproduct(vector,majoraxis);
	double minorsize=dotproduct(vector,minoraxis);
	double scaling=sqrt(majorsize*majorsize+minorsize*minorsize);
	*costhi=majorsize/scaling;
	*sinthi=minorsize/scaling;
}

double orbitelements::thitoradius(double costhi) const
{
	return angularmomentum2/(planet*(eccentricity*costhi+1));
}

double orbitelements::radiustothi(double radius) const
{
	return (angularmomentum2/(planet*radius)-1)/eccentricity;
}

double orbitelements::getpedistance() const
{
	return semimajor*(eccentricity-1);
}

double orbitelements::getapodistance() const
{
	return -semimajor*(eccentricity+1);
}

double orbitelements::getcurrradius() const
{
	return vectorsize(currposition);
}

VECTOR3 orbitelements::getintersectvector(const ORBIT &torbit) const
{
	return crossproduct(planevector, torbit.planevector);
}

double orbitelements::geteccentricity() const 
{
	return eccentricity;
}

double orbitelements::getangmomentum2() const
{
	return angularmomentum2;
}

double orbitelements::getcurrcosthi() const
{
	return currcosthi;
}

double orbitelements::getcurrsinthi() const 
{
	return currsinthi;
}

void orbitelements::getaxes(VECTOR3 *tmajoraxis, VECTOR3 *tminoraxis) const
{
	*tmajoraxis=majoraxis;
	*tminoraxis=minoraxis;
}
	
void orbitelements::getcurrentvectors(VECTOR3 *tpos, VECTOR3 *tvel) const
{
	*tpos=currposition;
	*tvel=currvelocity;
}

double orbitelements::getsemimajor() const
{
	return semimajor;
}

VECTOR3 orbitelements::geteccentricityvector() const
{
	return eccvector;
}

VECTOR3 orbitelements::getplanevector() const
{
	return planevector;
}

double orbitelements::getpedeltatime() const
{
	//Gets time to next Pe passage
	//Deltatime is always time from the last one (unless ecc>1, in which case there is only one)
	double temp;
	if (eccentricity<1)
	{
		
		temp=orbitconstant*(PI+PI)-deltatime;//Time to next passage
	}
	else
	{
		temp=-deltatime; //Only one passage
	}
	return temp;
}

bool orbitelements::isvalid() const
{
	return valid;
}

double orbitelements::getgmplanet() const
{
	return planet;
}





void orbitelements::GetTimesToThi(double costhi, double *time1, double *time2,int fullorbits,int halforbits) const

// There are two solutions for any value of costhi
// Useful as costhi can be derived from a radius from a body

{
	if (!valid) return;
	double coscosh;
	if (1+eccentricity*costhi<0)
	{
		//hyperbolic orbit - costhi in range which orbit never reaches
		coscosh=1e8; //Very, very large figure
	}
	else
	{
		coscosh=(eccentricity+costhi)/(1+eccentricity*costhi);
	}
	double sinsinh, e_angle, loc_deltatime;
	if (eccentricity<=1)
	{
		//Elliptical case
		sinsinh=1-coscosh*coscosh;
		if (sinsinh>0)
		{
			sinsinh=sqrt(sinsinh);
			e_angle=acos(coscosh);
		}
		else
		{
			sinsinh=0;
			if (coscosh>0)
			{
				e_angle=0;
			}
			else
			{
				e_angle=PI;
			}
		}
	}
	else
	{
		//Hyperbolic case
		sinsinh=coscosh*coscosh-1;
		if (sinsinh>0)
		{
			sinsinh=sqrt(coscosh*coscosh-1);
			e_angle=log(coscosh+sinsinh);
		}
		else
		{
			sinsinh=0;
			e_angle=0;
		}
	}
	loc_deltatime=orbitconstant*(e_angle-eccentricity*sinsinh); //Gives deltatime from periapsis
	*time1=-deltatime+loc_deltatime;
	*time2=-deltatime-loc_deltatime;//Can be positive if deltatime sufficiently negative
	double orbittime=orbitconstant*(PI+PI);
	if (eccentricity<1)
	{
		if (*time1<0) *time1+=orbittime;
		if (*time2<0) *time2+=orbittime;
		if (*time1<0) *time1+=orbittime;
		if (*time2<0) *time2+=orbittime;
	}
	if (*time1>*time2)
	{
		double temp=*time1;
		*time1=*time2;
		*time2=temp;
	}
	if (halforbits!=0)
	{
		if (*time1<orbittime/2) *time1+=orbittime;
		if (*time2<orbittime/2) *time2+=orbittime;
	}
	*time1+=orbittime*fullorbits;
	*time2+=orbittime*fullorbits;
}


void orbitelements::draworbit(HDC hDC, const GRAPH *graph, bool drawradius) const
{
	// Create projection vectors
	if (!valid) return;
	const double xoffset = (graph->ixstart+graph->ixend)/2;
	const double yoffset = (graph->iystart+graph->iyend)/2;
	VECTOR3 xaxis=graph->xaxis;
	VECTOR3 yaxis=graph->yaxis;
	DWORD xstart=graph->ixstart;
	DWORD xend=graph->ixend;
	DWORD ystart=graph->iystart;
	DWORD yend=graph->iyend;
	double scale=graph->scale;
	POINT pointarray[50];
	DWORD numpoints;
	int xpos, ypos;
	double xposd,yposd;
	double step=PI/50;
	double cosstep=cos(step);
	double sinstep=sqrt(1-cosstep*cosstep);
	double currcos=1;
	double currsin=0;
	double currdivisor, currentradius, ratio, difference;
	double tempsin;
	VECTOR3 currvector;
	int ct;
	double topconstant=semimajor*(eccentricity*eccentricity-1);
	do //This loop runs twice to draw both orbit halves
	{
		ct=0;
		currvector=majoraxis*(topconstant/(eccentricity+1));
		xposd= dotproduct(xaxis, currvector)*scale +xoffset;
		yposd= dotproduct(yaxis, currvector)*scale +yoffset;
		if (xposd<xstart || xposd>xend || yposd<xstart || yposd>xend) return; // Makes safe if orbit too large to fit screen!
		xpos=int(xposd);
		ypos=int(yposd);
		MoveToEx (hDC, xpos, ypos, NULL);
		bool exit=false;
		do //This inner loop draws half an orbit from periapsis outwards
		{
			tempsin=currsin;
			currsin=tempsin*cosstep+currcos*sinstep;
			currcos=currcos*cosstep-tempsin*sinstep;
			currdivisor=eccentricity*currcos+1;
			if (currdivisor<=0)  //Puts in special final line in hyperbolic orbits
			{
				currcos=-1/eccentricity+0.0001;
				currsin=sqrt(1-currcos*currcos);
				if (sinstep<0) currsin=-currsin;
				currdivisor=eccentricity*currcos+1;
				exit=true;
			}
			currentradius=topconstant/currdivisor;
			currvector=majoraxis*(currcos*currentradius)+minoraxis*(currentradius*currsin);
			xposd=dotproduct(xaxis,currvector)*scale+xoffset;
			yposd=dotproduct(yaxis,currvector)*scale+yoffset;
			// Following four if statements ensure proper truncation of line segments attempting to draw
			// beyond the boundaries of the MFD
			if (xposd<xstart)
			{
				ratio=(pointarray[ct-1].x-xstart)/(pointarray[ct-1].x-xposd);
				difference=yposd-pointarray[ct-1].y;
				yposd=difference*ratio+pointarray[ct-1].y;
				xposd=xstart;
				exit=true;
			}
			if (xposd>xend)
			{
				ratio=(xend-pointarray[ct-1].x)/(xposd-pointarray[ct-1].x);
				difference=yposd-pointarray[ct-1].y;
				yposd=difference*ratio+pointarray[ct-1].y;
				xposd=xend;
				exit=true;
			}
			if (yposd<ystart)
			{
				ratio=(pointarray[ct-1].y-ystart)/(pointarray[ct-1].y-yposd);
				difference=xposd-pointarray[ct-1].x;
				xposd=difference*ratio+pointarray[ct-1].x;
				yposd=ystart;
				exit=true;
			}
			if (yposd>yend)
			{
				ratio=(yend-pointarray[ct-1].y)/(yposd-pointarray[ct-1].y);
				difference=xposd-pointarray[ct-1].x;
				xposd=difference*ratio+pointarray[ct-1].x;
				yposd=yend;
				exit=true;
			}
			pointarray[ct].x=int(xposd); //Thunks down
			pointarray[ct].y=int(yposd); //to integer
			if (++ct>49) exit=true;
		}
		while (!exit);
		numpoints=DWORD(ct);
		PolylineTo(hDC, pointarray, numpoints);
		sinstep=-sinstep;
		currcos=1;
		currsin=0;
		ct=0;
	}
	while (sinstep<0); // Only true after the first pass - false after the second
	if (drawradius)
	{
		xpos=(int) xoffset;
		ypos=(int) yoffset;
		MoveToEx (hDC, xpos, ypos, NULL);
		currvector=currposition;
		xpos= int(dotproduct(xaxis, currvector)*scale +xoffset);
		ypos= int(dotproduct(yaxis, currvector)*scale +yoffset);
		LineTo(hDC, xpos, ypos);
	}
}




void orbitelements::timetovectors(double timefromnow,ORBITTIME *posvel) const
{
	if (!valid) return;
	double timetarget=timefromnow+deltatime;//Now in units of time from Pe

	if (eccentricity<1)
	{//Adjust to nearest orbit - simple optimisation which has no effect on result
		double orbittime=orbitconstant*2*PI;
		timetarget-=floor(timetarget/orbittime+0.5)*orbittime;
	}
	
	if (posvel->processed)
	{//We've scanned this structure already, go ahead and improve on it
		if (improve(timetarget,posvel)) return;//If untrue, it's because result indicates major overhaul needed
	}
	//At this point, we have a not very initialised structure to sort out
	double topthi;
	if (eccentricity>1)//split off the hyperbolic from the elliptical, and handle differently
	{
		topthi=-1/eccentricity+0.2;//Not at infinity - a bit closer
		double timeattopthi=simpletimetothi(topthi,timetarget);
		if (fabs(timeattopthi)<fabs(timetarget))//In asymptote region, work by radius at first
		{
			improvebyradius(timetarget,topthi,timeattopthi,posvel);
		}
		else
		{
			improvebysubdivision(timetarget,topthi,timeattopthi,posvel);
		}
	}
	else
	{
		topthi=-1;
		double timeattopthi=orbitconstant*PI;
		improvebysubdivision(timetarget,topthi,timeattopthi,posvel);
	}
	improve(timetarget,posvel);//improves, and converts to vectors
	posvel->processed=true;
}

void orbitelements::improvebyradius(double timetarget,double topthi,double timeattopthi,ORBITTIME *posvel) const
{//Only ever called at the asymptotes of a hyperbola where velocity is hopefully reasonably constant
	double radius=thitoradius(topthi);
	bool reversed=false;
	if (timetarget<0)
	{
		timetarget=-timetarget;
		reversed=true;
	}
	if (timeattopthi<0) timeattopthi=-timeattopthi;
	double outvel;
	double tempthi=topthi;
	outvel=eccentricity*planet*sqrt(fabs(1-tempthi*tempthi)/angularmomentum2);
	do
	{//this is never called when topthi=1 or -1
		radius=(timetarget-timeattopthi)*outvel+radius;
		timeattopthi=GetTimeToRadius(radius,true);
	}
	while (fabs(timetarget-timeattopthi)*1e5<timetarget);
	tempthi=radiustothi(radius);
	posvel->icosthi=tempthi;
	if (fabs(tempthi)<1)
	{
		posvel->currangle=acos(tempthi);
	}
	else
	{
		if (tempthi>0)
		{
			posvel->currangle=0;
		}
		else
		{
			posvel->currangle=PI;
		}
	}
	if (reversed) posvel->currangle=-posvel->currangle;
}

void orbitelements::improvebysubdivision(double timetarget,double topthi,double timeattopthi,ORBITTIME *posvel) const
{
	//double angmomentum=sqrt(angularmomentum2);
	bool reversed=false;
	double lowthi=1;
	double timeatlowthi=0;
	double ratio;
	double guess;
	double timeatguess;
	if (timetarget<0)
	{
		timetarget=-timetarget;
		reversed=true;
	}
	if (timeattopthi<0) timeattopthi=-timeattopthi;
	for (int a=0;a<8;a++)
	{
		if ((timeattopthi-timeatlowthi)*1e7<timeattopthi+timeatlowthi) break;
		ratio=(topthi-lowthi)/(timeattopthi-timeatlowthi);
		guess=(topthi+lowthi)/2;
		timeatguess=simpletimetothi(guess,timetarget);
		if (timeatguess>timetarget)
		{
			timeattopthi=timeatguess;
			topthi=guess;
		}
		else
		{
			timeatlowthi=timeatguess;
			lowthi=guess;
		}
	}
	posvel->icosthi=guess;
	if (fabs(guess)<1)
	{
		guess=acos(guess);
	}
	else
	{
		if (guess>0)
		{
			guess=0;
		}
		else
		{
			guess=PI;
		}
	}
	if (reversed) guess=-guess;
	posvel->currangle=guess;
	return;
}






bool orbitelements::improve(double timetarget,ORBITTIME *posvel) const
{
	// Prevent NaN errors
	if(posvel->currangle != posvel->currangle)
		posvel->currangle = 0;
	if(posvel->icosthi != posvel->icosthi)
		posvel->icosthi = 0;

	double angmomentum=sqrt(angularmomentum2);
	double timeerr,iradius,iangleerr,icosthi;
	VECTOR3 *tpos=&(posvel->pos);
	VECTOR3 *tvel=&(posvel->vel);
	double tempangle=posvel->currangle;
	icosthi=posvel->icosthi;
	do
	{
		if (1+eccentricity*icosthi<0) return false;//check for beyond asymptote result
		if (fabs(tempangle)<PI)
		{
			timeerr=simpletimetothi(icosthi,timetarget)-timetarget;
		}
		else
		{
			timeerr=simpletimetothi(icosthi,-timetarget)-timetarget;
			if (timetarget>0)
			{
				timeerr+=orbitconstant*2*PI;
			}
			else
			{
				timeerr-=orbitconstant*2*PI;
			}
		}
		iradius=thitoradius(icosthi);
		iangleerr=timeerr*angmomentum/(iradius*iradius);
		if (fabs(iangleerr)>0.5)
		{
			return false;
		}
		tempangle-=iangleerr;
		icosthi=cos(tempangle);
	}
	while (fabs(iangleerr)>1e-6);
	posvel->currangle=tempangle;
	posvel->icosthi=icosthi;
	double isinthi=sin(posvel->currangle);
	thitovectors(icosthi,isinthi,tpos,tvel);
	return true;
}




void orbitelements::timetovectors(double timefromnow, VECTOR3 *pos, VECTOR3 *vel) const
{
	// Find the position of a planet at timefromnow in the future

	// This routine is written to be most efficient at low eccentricity typical of planet - to find planet positions
	if (!valid) return;
	if (eccentricity>1)
	{
		
		double e_angle;
		double loc_deltatime,delta,ex;
		double to=(timefromnow+deltatime)/orbitconstant; //Required time / orbitconstant
		if (to>PI)
		{
			e_angle=-to-eccentricity/2;
		}
		else
		{
			e_angle=-to+eccentricity/2;
		}
		for (int i=0;i<20;i++){// Should quickly converge to correct value of e_angle
			ex=exp(e_angle);
			delta=(eccentricity*(ex-1/ex)*0.5-e_angle+to)/(1-eccentricity*(ex+1/ex)*0.5); //(sinh and cosh are the ex expressions)
			e_angle+=delta;
		}
		ex=exp(e_angle);
		loc_deltatime=orbitconstant*(e_angle-eccentricity*(ex-1/ex)*0.5); //Probably still slightly different from required time
		double cosh_e=(ex+1/ex)*0.5;
		double costhi=(eccentricity-cosh_e)/(eccentricity*cosh_e-1);
		double sinthi=1-costhi*costhi;
		if (sinthi>0)
		{
			sinthi=sqrt(sinthi);
		}
		else
		{
			sinthi=0;
		}
		if (ex-1/ex>0) sinthi=-sinthi;
		thitovectors(costhi, sinthi, pos, vel);
		// Finally adjust the vectors using deltatime
		*pos=*pos+*vel*(timefromnow-loc_deltatime+deltatime); //Make final (small) correction to deltatime error
	}
	else
	{
		double e_angle;
		// Assume that the first initial e_angle is thi (present position)! 
		double loc_deltatime,delta;
		double to=(timefromnow+deltatime)/orbitconstant; //Required time / orbitconstant
		if (to>PI)
		{
			e_angle=to-eccentricity/2;
		}
		else
		{
			e_angle=to+eccentricity/2;
		}		
		for (int i=0;i<20;i++){// Should quickly converge to correct value of e_angle
			delta=(eccentricity*sin(e_angle)-e_angle+to)/(1-eccentricity*cos(e_angle));
			e_angle+=delta;
		}
		loc_deltatime=orbitconstant*(e_angle-eccentricity*sin(e_angle)); //Probably still slightly different from required time
		double cos_e=cos(e_angle);
		double costhi=(eccentricity-cos_e)/(eccentricity*cos_e-1);
		double sinthi=sin(e_angle)*sqrt(1-eccentricity*eccentricity)/(1-eccentricity*cos_e);
		thitovectors(costhi, sinthi, pos, vel);
		// Finally adjust the vectors using deltatime
		*pos=*pos+*vel*(timefromnow-loc_deltatime+deltatime); //Make final (small) correction to deltatime error
	}
}


void orbittime::getposvel(VECTOR3 *tpos,VECTOR3 *tvel)
{
	*tpos=pos;
	*tvel=vel;
}

orbittime::orbittime()
{
	pos.x=pos.y=pos.z=vel.x=vel.y=vel.z=3;
	processed=false;
}

