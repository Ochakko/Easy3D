#ifndef GPDATAH
#define GPDATAH

#include <D3DX9.h>
#include <coef.h>
#include <quaternion.h>

class CGPData
{
public:
	CGPData();
	~CGPData();

	int InitParams();
	int SetGPE( GPELEM* srcgpe );
	int GetMatWorld( D3DXMATRIX* dstmat );
	int CalcMatWorld();

private:
	int DestroyObjs();
	int Deg2Q( D3DXVECTOR3 rot, CQuaternion* dstq );

public:
	GPELEM m_gpe;
	CQuaternion m_q;
	D3DXMATRIX m_matWorld;
};

#endif

