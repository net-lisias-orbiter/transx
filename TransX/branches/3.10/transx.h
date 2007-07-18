#ifndef __TRANSX_H
#define __TRANSX_H
#include "graph.h"

class TransxMFD: public MFD {
public:
	TransxMFD (DWORD w, DWORD h, VESSEL *Vessel, UINT mfd);
	~TransxMFD();
	char *ButtonLabel (int bt);
	int  ButtonMenu (const MFDBUTTONMENU **menu) const;
	bool ConsumeKeyImmediate(char *kstate);
	bool ConsumeButton(int bt, int event);
	bool ConsumeKeyBuffered(DWORD key);
	void Update (HDC hDC);
	void WriteStatus(FILEHANDLE scn) const;
	void ReadStatus(FILEHANDLE scn);
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

	int getwidth();
	int getheight();
	static int GetMfdCount(){return MfdCount;}	
	bool isvalid() { return valid;};
private:
	class viewstate *viewstate;// Pointer to viewstate
	static int MfdCount;

	bool valid;
};

#endif