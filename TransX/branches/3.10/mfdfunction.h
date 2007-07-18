/*****************************************

  This is a base class from which is derived
  the functions that go into TransX.

  */
#ifndef __MFDFUNCTION_H
#define __MFDFUNCTION_H

class MFDFunction
{
private:

	bool allowedtowrite;//Only one action outstanding allowed per instance - use this to keep control of queues!
	MFDFunction *next,*previous;//Next and previous in current chain
	static MFDFunction *lastfast,*firstfast,*lastslow,*firstslow;
protected:
	bool valid;
	virtual void dolowpriaction(); //Overloaded to carry out processing task in background.
	virtual void dohighpriaction(); //Overloaded to carry out processing task in background
	void dofastaction();//carries out preparation for fast action
	void doslowaction();//carries out preparation for slow action
public:
	MFDFunction();
	void delist();
	virtual ~MFDFunction();
	bool isvalid();

	static void donextaction(); //Executes the next action in the stack
	virtual bool addaction(int priority); //Adds a timestep action to the list
};

#endif