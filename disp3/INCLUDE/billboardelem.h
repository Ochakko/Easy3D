#ifndef BILLBOARDELEMH
#define BILLBOARDELEMH


#include <coef.h>
#include <D3DX9.h>

#include <crtdbg.h>
#include <basedat.h>

#include <shdelem.h>


class CBillboardElem
{
public:
	CBillboardElem();
	~CBillboardElem();

	void InitParams();
	void InitParticleParams();

	int Rotate( float fdeg, int rotkind );


private:
	void DestroyObjs();

public:
	CShdElem* selem;
	D3DXVECTOR3 pos;
	DWORD dwOffset;
	int dispflag;
	int useflag;

	D3DXMATRIX rotmat;

	/// for particle
	float createtime;//[sec]
	D3DXVECTOR3 pos0;
	D3DXVECTOR3 vel0;

	D3DXVECTOR3 befpos;
	float particleR;

	int m_textileno;

};


#endif