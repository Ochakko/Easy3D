#ifndef QELEMH
#define QELEMH

#include <coef.h>
#include <quaternion.h>

#include <d3dx9.h>

class CQElem
{
public:
	CQElem();
	~CQElem();

private:
	int InitParams();
	int DestroyObjs();

public:
	int frameno;
	CQuaternion q;
	D3DXVECTOR3 eul;
};

#endif
