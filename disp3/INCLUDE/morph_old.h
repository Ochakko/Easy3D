#ifndef MORPHH
#define MORPHH

//#include "charpos.h"
#include <coef.h>
#include <D3DX9.h>

#include <crtdbg.h>

#include <basedat.h>

class CShdHandler;
class CShdElem;
class CD3DDisp;

class CMorph : public CBaseDat
{
public:
	CMorph();
	~CMorph();

	void DestroyObjs();
	int AddMorphElem( CShdElem* addselem );
	int DeleteMorphElem( CShdElem* delelem );

private:
	virtual void InitParams();

public:
	//CMeshInfo* meshinfo;

	int objtype;
	int morphnum;

	CD3DDisp** hd3ddisp;	
	CShdElem** hselem;
};

#endif