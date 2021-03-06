#ifndef __MFDVARTYPES_H
#define __MFDVARTYPES_H

#include "mapfunction.h"
#include "mfdvariable.h"
#include "doublelink.h"

/*class MFDvarbody : public MFDvariable {
protected:
	OBJHANDLE pointer;
	char label[40];
public:
	MFDvarbody();
	void init(MFDvarhandler *vars,int viewmode1,int viewmode2, char *vname,char *vlabel);
	~MFDvarbody();
	void sethandle(OBJHANDLE tpointer);
	virtual OBJHANDLE gethandle() const;
	bool flip_variable();
	bool SetVariableBody(char *str);
	virtual bool show(HDC hDC, int width, int line) const;
	virtual void getsaveline(char *buffer) const;
	virtual bool loadvalue(char *buffer);
};*/


class liststring : public listelement
{
	char buffer[40];
public:
	void setbuffer(char *tbuffer);
	char *getbuffer(){return buffer;};//returns pointer to actual buffer.
	liststring(bool manageme = true);
	virtual ~liststring(){};//virtual destructor to guard against memory leak
};

class MFDvarshiplist : public MFDvariable
{
private:
	class dbllist shiplisthead;//Used to contain the ship names
	class dblliter *iterator;//Iterator for moving through list
	bool initialised;//States whether this variable has been initialised yet or not
public:
	void initbybody(OBJHANDLE craft,bool reset);//Used to create list to select from
	void addtolist(char *name);
	virtual bool show(HDC hDC, int width, int line);
	virtual OBJHANDLE gethandle() const;
	virtual void showadjustment(HDC hDC, int width, int line) const{};
	virtual void ch_adjmode(){};
	virtual void chm_adjmode(){};
	virtual bool flip_variable();
	virtual bool flipback_variable();
	void init(MFDvarhandler *vars,int viewmode1, int viewmode2, char *vname);
	MFDvarshiplist();
	virtual ~MFDvarshiplist();

	virtual void getsaveline(char *buffer) const{};//This MFDvariable type doesn't support load/save
	virtual bool loadvalue(char *buffer){return true;};//
};
	




class MFDvarmoon : public MFDvariable {
protected:
	OBJHANDLE pointer;
	OBJHANDLE centralbody;
	class mapfunction *mappointer;
	int ivalue;
	char intbuffer[30];
	char *ibuffer;
public:
	virtual bool flip_variable();
	virtual void setall(class MFDvariable *var);
	bool validate();
	void initvalidate();
	virtual bool flipback_variable();
	virtual bool show(HDC hDC, int width, int line);
	virtual void ch_adjmode();
	virtual void chm_adjmode();
	virtual void showadjustment(HDC hDC, int width, int line) const;


	virtual int getivalue() const;
	virtual OBJHANDLE gethandle() const;
	virtual bool loadvalue(char *buffer);
	virtual bool SetVariableBody(char *str);
	virtual int getadjtype() const {return 0;};
	virtual void getsaveline(char *buffer) const;
	void updatecentralbody(OBJHANDLE tcentral){centralbody=tcentral;};
	operator int(){return ivalue;}
	void init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname,OBJHANDLE tcentralbody);
	~MFDvarmoon();
};

class MFDvarfloat : public MFDvariable {
protected:
	double value,defaultvalue; // Value of the variable
	double min; // Minimum legal value
	double max; //Maximum legal value
	double increment; //Increment fraction
	double logborder; // Number below which increment is linear scaled
public:
	operator double() {return value;};
	double operator = (double tvalue){value=tvalue;return value;};
	virtual bool inc_variable(); // Increase the variable
	virtual bool dec_variable(); //Decrease the variable
	bool show(HDC hDC, int width, int line);
	double getvalue() const; //Get the value
	void setvalue(double tvalue);
	virtual void getsaveline(char *buffer) const;
	virtual bool loadvalue(char *buffer);
	void init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname, int vadjmode, double vvalue, double vmin, double vmax, double vincrement, double vlogborder);
	MFDvarfloat();
	~MFDvarfloat();
};

class MFDvarMJD: public MFDvarfloat {
public:
	bool show(HDC hDC, int width, int line);
	virtual bool inc_variable(); // Increase the variable
	virtual bool dec_variable(); //Decrease the variable

	double operator = (double tvalue){value=tvalue;return value;};
	MFDvarMJD();
	~MFDvarMJD();
};


class MFDvardiscrete: public MFDvariable {
private:
	char label[5][15]; //Label array
	int value; // Variable indicating the projection type
	int limit; // Highest permissible value - Note - 5 labels MAX!!
public:
	bool flip_variable(); //Change to next projection
	operator int() {return value;};
	int operator = (int tvalue){value=tvalue;return value;};
	int getivalue() const; // Function to obtain value
	void setivalue(int tvalue);
	bool show(HDC hDC, int width, int line);
	virtual void getsaveline(char *buffer) const;
	virtual bool loadvalue(char *buffer);
	void init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname, int vvalue, int vlimit, char *st1, char *st2, char *st3, char *st4, char *st5);
	MFDvardiscrete();
	~MFDvardiscrete();
};

class MFDsemiintdiscrete: public MFDvariable {
	int value;//variable indicating double the actual value
public:
	virtual bool flip_variable();
	virtual bool flipback_variable();
	operator double(){return value*0.5;};
	double operator = (double tvalue){value=int(floor(2.0*tvalue));return value;};
	double getvalue() const{return double(value)*0.5;};
	void setvalue(double tvalue){value=int(floor(tvalue*2.0));};
	virtual void getsaveline(char *buffer) const;
	virtual bool loadvalue(char *buffer);
	void init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname,int tvalue);
	virtual void showadjustment(HDC hDC,int width,int line){};//There's no adjustment to show
	virtual void ch_adjmode(){};//or adjust
	virtual void chm_adjmode(){};
	virtual bool show(HDC hDC,int width,int line);
	MFDsemiintdiscrete(){value=0;};

	~MFDsemiintdiscrete();
};




class MFDvarangle: public MFDvariable {
private:
	double value,defaultvalue; // Value of the angle
	double cosvalue;//Cosine of the angle - calculated here to avoid repeated recalculation later
	double sinvalue; //Sin of the angle - as above
	double min; // Minimum legal value
	double max; // Maximum legal value
	double increment; // Increment value
	bool loop; //If true, then loops from min to max
public:
	operator double() {return value;};
	double operator = (double tvalue){setvalue(tvalue);return tvalue;};
	bool inc_variable(); //Increase the variable
	bool dec_variable(); //Decrease the variable
	bool show(HDC hDC, int width, int line);
	double getvalue() const;
	void setvalue(double tvalue);
	void getsincos(double *sin, double *cos) const;
	double getsin() const;
	double getcos() const;
	virtual void getsaveline(char *buffer) const;
	virtual bool loadvalue(char *buffer);
	void init(MFDvarhandler *vars,int viewmode1,int viewmode2,char *vname, int vadjmode, double vvalue, double vcosvalue, double vsinvalue,
						 double vmin, double vmax, double vincrement, bool vloop);
	MFDvarangle();
	~MFDvarangle();
};

#endif