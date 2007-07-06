#ifndef __GRAPH_H
#define __GRAPH_H

#include "orbitelements.h"

typedef class graphset{
//This class holds information on a display window for graphs, and vectors for the projection angle of these
//orbit graphs, and current scaling
friend class orbitelements;
public:
	void drawvectorline(HDC hDC, const VECTOR3 &line);//Draws vector line at current projection & scaling
	void drawplanet(HDC hDC, OBJHANDLE body); //Draw circle representing planet
	void setprojection(const class orbitelements &torbit);//View set from orbit's plane vector
	void setprojection(const VECTOR3 &projection);// View set from vector
	void setprojection(const VECTOR3 &txaxis, const VECTOR3 &tyaxis, const VECTOR3 &tzaxis);//Explicitly set axes
	void drawvector(HDC hDC,const VECTOR3 &line1);//Draw vector using current projection & scale
	void drawtwovector(HDC hDC, const VECTOR3 &line1, const VECTOR3 &line2);//Draw two vectors using current projection & scaling
	void draworbit(const class orbitelements &element, HDC hDC, bool drawradius);//Calls draworbit in orbitelements
	double vectorpointdisplay(HDC hDC, const VECTOR3 &target, MFD *mfd, VESSEL *vessel, bool isposition);//Nice little pointer utility
	//void init(DWORD w, DWORD h);
	void setviewscale(const class orbitelements &orbit);//Set scale of picture using orbit size
	void setviewscale(double temp);//Set scale using a number
	void setviewscalesize(double temp);//Set scale using a distance size
	void setviewwindow(DWORD xstart, DWORD ystart, DWORD xend, DWORD yend);//Set a window within MFD
	void getviewwindow(DWORD *xstart, DWORD *ystart, DWORD *xend, DWORD *yend);//Get back above info
	double getviewscale();//Get viewscale number
private:
	VECTOR3 xaxis, yaxis, zaxis; // projection vectors
	DWORD ixstart, iystart, ixend, iyend, windowsize;//window parameters
	double scale;//scaling factor for diagram
}GRAPH;

#endif