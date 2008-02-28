/* Copyright (c) 2007 Duncan Sharpe, Steve Arch
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
** 
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.*/

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "orbitersdk.h"
#include "mfd.h"
#include "graph.h"
#include "TransXFunction.h"


void Graph::setviewwindow(DWORD xstart,DWORD ystart,DWORD xend,DWORD yend)
{
	ixstart=xstart;
	ixend=xend;
	iystart=ystart;
	iyend=yend;
	windowsize=ixend-ixstart;
	DWORD temp=iyend-iystart;
	if (temp<windowsize) windowsize=temp;
}

void Graph::getviewwindow(DWORD *xstart, DWORD *ystart, DWORD *xend, DWORD *yend)
{
	*xstart=ixstart;
	*xend=ixend;
	*ystart=iystart;
	*yend=iyend;
}

// Viewscale is used to scale everything that is drawn graphically
void Graph::setviewscale(double temp)
{
	scale=temp;
}

double Graph::getviewscale()
{
	return scale;
}


void Graph::setviewscalesize(double temp)
{
	double tempscale=(double(windowsize)/temp)/2;
	if (scale==0 || scale>tempscale) scale=tempscale;
}

void Graph::setviewscale(const OrbitElements &orbit)
{
	if (!orbit.isvalid()) return;
	double tempscale=0;
	if (orbit.geteccentricity()<1)
	{
		tempscale=-(1+orbit.geteccentricity())*orbit.getsemimajor();
	}
	else
	{
		tempscale=2*orbit.getcurrradius();
	}
	tempscale=(double(windowsize)/tempscale)/2;
	if (scale==0 || scale>tempscale) scale=tempscale;
}

void Graph::setprojection(const VECTOR3 &txaxis, const VECTOR3 &tyaxis, const VECTOR3 &tzaxis)
// Set projection vectors
{
	xaxis=unitise(txaxis);
	yaxis=unitise(tyaxis);
	zaxis=unitise(tzaxis);
}

void Graph::setprojection(const OrbitElements &torbit)
{
	if (!torbit.isvalid()) return;
	VECTOR3 tvector=torbit.getplanevector();
	setprojection(tvector);
}

void Graph::setprojection(const VECTOR3 &projection)
//Set projection vectors using a viewpoint vector
// Angle of peojection is set by the same method used by Orbiter
{
	if (projection.x==0 && projection.z==0)
	{
		VECTOR3 temp={0,0,0};
		xaxis=yaxis=zaxis=temp;
		xaxis.x=1;
		yaxis.z=-1;
		zaxis.y=-1;
	}
	else
	{
		VECTOR3 temp={0,-1,0}; // Ecliptic vector
		zaxis=unitise(projection);
		VECTOR3 cross=unitise(crossp(temp, zaxis)); // Vector of LAN x+z only
		VECTOR3 xcross=crossp(cross, zaxis); //90 around - max out of plane
		xaxis=cross*cross.x+xcross*cross.z; // Back around the orbit
		yaxis=xcross*cross.x-cross*cross.z;
	}
}

void Graph::draworbit(const OrbitElements &element, HDC hDC, bool drawradius)
{
	element.draworbit(hDC,this ,drawradius);
}


void Graph::drawtwovector(HDC hDC, const VECTOR3 &line1, const VECTOR3 &line2)
// Draw two vectors from the planet centre
{
	const double xoffset=(ixstart+ixend)*0.5;
	const double yoffset=(iystart+iyend)*0.5;
	int xpos, ypos;
	xpos=int(xoffset);
	ypos=int(yoffset);
	MoveToEx(hDC, xpos, ypos, NULL);
	xpos=int(dotp(xaxis, line1)*scale+xoffset);
	ypos=int(dotp(yaxis, line1)*scale+yoffset);
	LineTo(hDC, xpos, ypos);
	xpos=int(xoffset);
	ypos=int(yoffset);
	MoveToEx(hDC, xpos, ypos, NULL);
	xpos=int(dotp(xaxis, line2)*scale+xoffset);
	ypos=int(dotp(yaxis, line2)*scale+yoffset);
	LineTo(hDC, xpos, ypos);
}


void Graph::drawvector(HDC hDC,const VECTOR3 &line1)
{
	const double xoffset=(ixstart+ixend)*0.5;
	const double yoffset=(iystart+iyend)*0.5;
	int xpos, ypos;
	xpos=int(xoffset);
	ypos=int(yoffset);
	MoveToEx(hDC, xpos, ypos, NULL);
	xpos=int(dotp(xaxis, line1)*scale+xoffset);
	ypos=int(dotp(yaxis, line1)*scale+yoffset);
	LineTo(hDC, xpos, ypos);
}


void Graph::drawvectorline(HDC hDC, const VECTOR3 &line)
// Draw a vector from the planet centre
{
	const double xoffset=(ixstart+ixend)*0.5;
	const double yoffset=(iystart+iyend)*0.5;
	int xpos, ypos;
	VECTOR3 temp=unitise(line)*(windowsize/2);
	double xline=dotp(xaxis, temp);
	double yline=dotp(yaxis, temp);
	xpos=int(xoffset-xline);
	ypos=int(yoffset-yline);
	MoveToEx(hDC, xpos, ypos, NULL);
	xpos=int(xoffset+xline);
	ypos=int(yoffset+yline);
	LineTo(hDC, xpos, ypos);
}

void Graph::drawplanet(HDC hDC, OBJHANDLE body)
{
	// Draw a circle of the right size to represent a planet
	double size=oapiGetSize(body);
	int x=int((ixstart+ixend)/2);
	int y=int((iystart+iyend)/2);
	if (size*scale>2)
		drawcircle(hDC, size);
	else
	{
		MoveToEx(hDC,x-6,y,NULL);
		LineTo(hDC,x+6,y);
		MoveToEx(hDC,x,y-6,NULL);
		LineTo(hDC,x,y+6);
	}
}

void Graph::drawatmosphere(HDC hDC, OBJHANDLE body)
{
	// Draw a circle of the right size to represent the atmosphere of a planet
	if(oapiPlanetHasAtmosphere(body))
	{
		double size = oapiGetPlanetAtmConstants(body)->radlimit;
		int x=int((ixstart+ixend)/2);
		int y=int((iystart+iyend)/2);
		if (size*scale>2)
			drawcircle(hDC, size);
		else
		{
			MoveToEx(hDC,x-6,y,NULL);
			LineTo(hDC,x+6,y);
			MoveToEx(hDC,x,y-6,NULL);
			LineTo(hDC,x,y+6);
		}
	}
}

void Graph::drawcircle(HDC hDC, double size)
{
	int x=int((ixstart+ixend)/2);
	int y=int((iystart+iyend)/2);
	int highx=int(x+size*scale);
	int lowx=int(x-size*scale);
	int highy=int(y+size*scale);
	int lowy=int(y-size*scale);
	Arc(hDC,lowx, lowy, highx, highy, highx, y, highx, y);	
}

double Graph::vectorpointdisplay(HDC hDC, const VECTOR3 &target, MFD *mfd, VESSEL *vessel, bool isposition)
//targetvector is a vector in the global reference plane
//isposition true
{
	VECTOR3 trtarget,temp;
	VESSELSTATUS status;
	if (isposition)
	{
		vessel->GetGlobalPos(temp);
		temp=target-temp;
	}
	else
	{
		temp=target;
	}
	vessel->GetStatus(status);
	VECTOR3 arot=status.arot;
	MATRIX3 rotmatrix;
	getinvrotmatrix(arot,&rotmatrix);
	trtarget = mul(rotmatrix, temp);
	int lines=int(windowsize/10);
	int edge=int(windowsize/1.5);
	int centre=int(windowsize/3);
	HPEN pen;
	pen=mfd->SelectDefaultPen(hDC,TransXFunction::White);
	Arc(hDC, ixstart,iystart+lines,ixstart+edge,iystart+edge+lines,ixstart,iystart+centre,ixstart,iystart+centre);
	edge-=centre/3;
	int nearedge=centre/3;
	Arc(hDC,ixstart+nearedge,iystart+nearedge+lines,ixstart+edge,iystart+edge+lines,ixstart+nearedge,iystart+centre,ixstart+nearedge,iystart+centre);
	edge-=centre/3;
	nearedge+=centre/3;
	Arc(hDC,ixstart+nearedge,iystart+nearedge+lines,ixstart+edge,iystart+edge+lines,ixstart+nearedge,iystart+centre,ixstart+nearedge,iystart+centre);
	edge=int(windowsize/1.5);
	MoveToEx(hDC,ixstart+centre,iystart+lines,NULL);
	LineTo(hDC,ixstart+centre,iystart+edge+lines);
	MoveToEx(hDC,ixstart,iystart+centre+lines,NULL);
	LineTo(hDC, ixstart+edge,iystart+centre+lines);
	double offsetsize=sqrt(trtarget.x*trtarget.x+trtarget.y*trtarget.y);
	double scalar=sqrt(2.0) / 2;
	double xang=trtarget.x/offsetsize;
	double yang=-trtarget.y/offsetsize;
	offsetsize=sqrt(offsetsize/trtarget.z);
	if (offsetsize>scalar || trtarget.z<0) offsetsize=scalar;
	offsetsize=offsetsize/scalar*windowsize/3;
	int xpos=int(offsetsize*xang+centre+ixstart);
	int ypos=int(offsetsize*yang+centre+lines+iystart);
	pen=mfd->SelectDefaultPen(hDC,TransXFunction::Green);
	MoveToEx(hDC,xpos-3,ypos-3,NULL);
	LineTo(hDC,xpos+3,ypos+3);
	MoveToEx(hDC,xpos-3,ypos+3,NULL);
	LineTo(hDC,xpos+3,ypos-3);
	return length(trtarget);
}

void Graph::drawmarker(HDC hDC, const VECTOR3 location, Shape shape)
{
	int x = (ixstart + ixend) / 2;
	int y = (iystart + iyend) / 2;

	const int radius = 3;
	double xpos = dotp(xaxis, location) * scale;
	double ypos = dotp(yaxis, location) * scale;
	int left	= (int)(x + xpos - radius + 0.5);	// add 0.5 to round off rather than floor
	int right	= (int)(x + xpos + radius + 0.5);
	int top		= (int)(y + ypos - radius + 0.5);
	int bottom	= (int)(y + ypos + radius + 0.5);

	if(shape == Circle)
		Ellipse(hDC, left, top, right, bottom);
	else if(shape == Rectangle)
		::Rectangle(hDC, left, top, right, bottom);
}