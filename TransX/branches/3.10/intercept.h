#ifndef __INTERCEPT_H
#define __INTERCEPT_H

#include "orbitelements.h"

typedef class intercept{
	private:
		//Variables
		double iceptradius;
		ORBITTIME alphatime,betatime;
		double icepttimeoffset;
		int iceptmethod;
		VECTOR3 iceptalpha;
		VECTOR3 iceptbeta;
		bool newintercept;
		VECTOR3 icraftpos;
		VECTOR3 icraftvel;
		VECTOR3 itargetpos;
		VECTOR3 itargetvel;
		VECTOR3 iplanecept;
		double itimeintercept;
		double gain;//oscillation controller
		double lasttimecorrection;//used in oscillation control
		int fullorbits,halforbits;//used for finding location of targets

		//Private functions
		void improveinterceptstraightline(const ORBIT &craft, const ORBIT &target);//Straight line improvement on previous intercept
		void adjustorbitsdown();//Change the orbital offset
	public:
		intercept(); //Default constructor
		void updateintercept(const ORBIT &craft, const ORBIT &target,double craftorbitsahead = 0);
		bool getvalid(){return !newintercept;};
		void resetintercept();
		void getpositions(VECTOR3 *craftpos, VECTOR3 *targetpos) const;
		void getvelocities(VECTOR3 *craftvel, VECTOR3 *targetvel) const;
		void getrelpos(VECTOR3 *relpos) const;
		void getrelvel(VECTOR3 *relvel) const;
		void getplanecept(VECTOR3 *planecept) const;
		void getorbitsoffset(int *ifullorbits,int *ihalforbits) const;
		double gettimeintercept() const;
} INTERCEPT;

#endif