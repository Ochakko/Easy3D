#ifndef TRAELEMH
#define TRAELEMH

#include <coef.h>
#include <d3dx9.h>

class CTraElem
{
public:
	CTraElem();
	~CTraElem();

private:
	int InitParams();
	int DestroyObjs();

public:
	int frameno;
	D3DXVECTOR3 tra;
};

#endif
