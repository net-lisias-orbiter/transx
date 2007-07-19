#ifndef __MFDGLOBALS_H
#define __MFDGLOBALS_H


// Needed due to the implementation of the selection functions in Orbiter
bool SelectVariableBody(void *id, char *str, void *usrdata);
DLLCLBK void opcDLLInit (HINSTANCE hDLL);
DLLCLBK void opcDLLExit (HINSTANCE hDLL);
double dotproduct(const VECTOR3 &vector1, const VECTOR3 &vector2);
VECTOR3 crossproduct(const VECTOR3 &vector1, const VECTOR3 &vector2);
VECTOR3 unitise(const VECTOR3 &vector);
double vectorsize(const VECTOR3 &vector);
double cosangle(const VECTOR3 &veca,const VECTOR3 &vecb);
double vectorsize2(const VECTOR3 &vector);
void matrixmultiply(const MATRIX3 &first, const MATRIX3 &second, MATRIX3 *result);
void matrixmultiply(const MATRIX3 &first, const VECTOR3 &second, VECTOR3 *result);
void getinvrotmatrix(VECTOR3 arot, MATRIX3 *invrotmatrix);

// Standard formatting function
void TextShow(HDC hDC, const char *label, int wpos, int hpos, double value);
void TextShow(HDC hDC, const char *label, int wpos, int hpos, OBJHANDLE handle);
void TextForm(char *buffer,const char *label,double value);

const double GRAVITY=6.67259e-11; // Gravitational constant

#endif