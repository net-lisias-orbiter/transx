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
#include "intercept.h"
#include "mfdvartypes.h"
#include "mfdvarhandler.h"
#include "viewstate.h"
#include "graph.h"
#include "transx.h"
#include "shiplist.h"
#include "mfdfunction.h"


struct {
	int mode;
}g_transxMFD;

using namespace std;

// ==============================================================
// API interface

DLLCLBK void InitModule (HINSTANCE hDLL)
{
	static char name[] = "TransX";
	MFDMODESPEC spec;
	spec.name    = name;
	spec.msgproc = TransxMFD::MsgProc;
	//Code contributed by Dave Robotham
	ifstream kstream;
	kstream.open("config\\transx.cfg",NULL);	// this could be any file really.
	if( kstream )
	{
		try 
		{
			char kbuf;
			kstream >> kbuf;
			kbuf = toupper(kbuf);
			kstream.close();
			switch( kbuf )
			{
				case 65 : spec.key = OAPI_KEY_A; break;
				case 66 : spec.key = OAPI_KEY_B; break;
				case 67 : spec.key = OAPI_KEY_C; break; 
				case 68 : spec.key = OAPI_KEY_D; break;
				case 69 : spec.key = OAPI_KEY_E; break;
				case 70 : spec.key = OAPI_KEY_F; break; 
				case 71 : spec.key = OAPI_KEY_G; break;
				case 72 : spec.key = OAPI_KEY_H; break;
				case 73 : spec.key = OAPI_KEY_I; break;
				case 74 : spec.key = OAPI_KEY_J; break;
				case 75 : spec.key = OAPI_KEY_K; break;
				case 76 : spec.key = OAPI_KEY_L; break;
				case 77 : spec.key = OAPI_KEY_M; break;
				case 78 : spec.key = OAPI_KEY_N; break;
				case 79 : spec.key = OAPI_KEY_O; break;
				case 80 : spec.key = OAPI_KEY_P; break;
				case 81 : spec.key = OAPI_KEY_Q; break; 
				case 82 : spec.key = OAPI_KEY_R; break; 
				case 83 : spec.key = OAPI_KEY_S; break;
				case 84 : spec.key = OAPI_KEY_T; break; 
				case 85 : spec.key = OAPI_KEY_U; break;
				case 86 : spec.key = OAPI_KEY_V; break;
				case 87 : spec.key = OAPI_KEY_W; break;
				case 88 : spec.key = OAPI_KEY_X; break; 
				case 89 : spec.key = OAPI_KEY_Y; break;
				case 90 : spec.key = OAPI_KEY_Z; break;

				default : spec.key = OAPI_KEY_J; break;
			}
		}
		catch( ... )
		{
			spec.key     = OAPI_KEY_J;
		}
	}

	else
		spec.key = OAPI_KEY_J;

	g_transxMFD.mode = oapiRegisterMFDMode (spec);

}//end code from Dave Robotham

DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	oapiUnregisterMFDMode (g_transxMFD.mode);
}

DLLCLBK void opcCloseRenderViewport()
{//Clean up
	viewstate::preparetoclose();
	if (TransxMFD::GetMfdCount()==0)
	{// MFD's all closed - up to me to clean up!
		shipptrs::destroyshipptrs();
	}
}

DLLCLBK void opcOpenRenderViewport(HWND renderWnd,DWORD width,DWORD height,BOOL fullscreen)
{
	mapfunction *temp=mapfunction::getthemap();//kicks off the process of map creation
}

DLLCLBK void opcFocusChanged(OBJHANDLE newfocus, OBJHANDLE oldfocus)
{

}

DLLCLBK void opcPostStep(double SimT, double SimDT, double mjd)
{
	static int choose;
	if (choose!=0)
	{
		shipptrs::backgroundaction();
		choose=0;
	}
	else
	{
		MFDFunction::donextaction();
		choose=1;
	}
}


bool SelectVariableBody(void *id, char *str, void *usrdata)

{
	return ((MFDvarmoon*)usrdata)->SetVariableBody(str);
}

void TextShow(HDC hDC,const char *label,int wpos,int hpos,OBJHANDLE handle)
{
	char buffer[30],buffer2[20];
	oapiGetObjectName(handle,buffer2,20);
	strcpy(buffer,label);
	strcat(buffer,buffer2);
	TextOut(hDC,wpos,hpos,buffer,strlen(buffer));
}

void TextForm(char *buffer,const char *label,double value)
{
	char index[2]=" ";
	if (fabs(value)>1000)
	{
		value/=1000;
		index[0]='k';
	}
	if (fabs(value)>1000)
	{
		value/=1000;
		index[0]='M';
	}
	if (fabs(value)>1000)
	{
		value/=1000;
		index[0]='G';
	}
	if (fabs(value)>1000)
	{
		value/=1000;
		index[0]='T';
	}
	strcpy(buffer,label);
	char buffer2[20];
	sprintf(buffer2,"%.4g",value);
	strcat(buffer2,index);
	strcat(buffer,buffer2);
}
	

void TextShow(HDC hDC,const char *label, int wpos, int hpos, double value)
{
	char buffer[30];
	TextForm(buffer,label,value);

	int length=strlen(buffer);
	TextOut(hDC, wpos, hpos, buffer, length);
}


// Standard vector functions
double dotproduct(const VECTOR3 &vector1, const VECTOR3 &vector2)
{
	double temp=vector1.x*vector2.x+vector1.y*vector2.y+vector1.z*vector2.z;
	return temp;
}

VECTOR3 crossproduct (const VECTOR3 &vector1, const VECTOR3 &vector2)
{
	VECTOR3 out;
	out.x=vector1.y*vector2.z-vector1.z*vector2.y;
	out.y=vector1.z*vector2.x-vector1.x*vector2.z;
	out.z=vector1.x*vector2.y-vector1.y*vector2.x;
	return out;
}

// Create vector of length 1
VECTOR3 unitise (const VECTOR3 &vector)
{
	double scalar= 1/sqrt(vector.x*vector.x+vector.y*vector.y+vector.z*vector.z);
	return vector*scalar;
}

// Find length of vector
double vectorsize(const VECTOR3 &vector)
{
	return sqrt(vector.x*vector.x+vector.y*vector.y+vector.z*vector.z);
}

double vectorsize2(const VECTOR3 &vector)
{
	return vector.x*vector.x+vector.y*vector.y+vector.z*vector.z;
}

double cosangle(const VECTOR3 &veca,const VECTOR3 &vecb)
{
	return dotproduct(veca,vecb)/sqrt(vectorsize2(veca)*vectorsize2(vecb));
}

void matrixmultiply(const MATRIX3 &first, const MATRIX3 &second, MATRIX3 *result)
{
	int a,b,c;
	for (a=0;a<3;a++)
	{
		c=3*a;
		for (b=0;b<3;b++)
		{
			result->data[3*a+b]=first.data[c]*second.data[b]+first.data[c+1]*second.data[b+3]+first.data[c+2]*second.data[b+6];
		}
	}
}

void matrixmultiply(const MATRIX3 &first, const VECTOR3 &second, VECTOR3 *result)
{
	int a,c;
	for (a=0;a<3;a++)
	{
		c=3*a;
		result->data[a]=first.data[c]*second.data[0]+first.data[c+1]*second.data[1]+first.data[c+2]*second.data[2];
	}
}


void getinvrotmatrix(VECTOR3 arot, MATRIX3 *invrotmatrix)//arot not really a vector - see arot defn from vessel struct
{
	double tcos=cos(arot.z);
	double tsin=sin(arot.z);
	MATRIX3 z={0,0,0,0,0,0,0,0,1};
	z.m11=z.m22=tcos;
	z.m12=-tsin;
	z.m21=tsin;
	tcos=cos(arot.y);
	tsin=sin(arot.y);
	MATRIX3 y={0,0,0,0,1,0,0,0,0};
	y.m11=y.m33=tcos;
	y.m13=tsin;
	y.m31=-tsin;
	tcos=cos(arot.x);
	tsin=sin(arot.x);
	MATRIX3 x={1,0,0,0,0,0,0,0,0};
	x.m22=x.m33=tcos;
	x.m32=tsin;
	x.m23=-tsin;
	MATRIX3 temp;
	matrixmultiply(z,y,&temp);
	matrixmultiply(temp,x,invrotmatrix);
}

	
