#ifndef SPPARAMH
#define SPPARAMH

#include <D3DX9.h>
#include <coef.h>

class CSPParam
{
public:
	CSPParam();
	~CSPParam();

//	int ScaleMvDist( float mult );

private:
	int InitParams();
	int DestroyObjs();

public:
	SPPARAM m_rotparam;
	SPPARAM m_mvparam;
	SPPARAM m_scparam;
	int reserved[4];
};

#endif