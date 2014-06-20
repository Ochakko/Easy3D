#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <pcoef.h>

#define	DBGH
#include <dbg.h>


CPCoef::CPCoef()
{
	InitParams();
}
CPCoef::~CPCoef()
{

}

int CPCoef::InitParams()
{
	m_a = 0.0f;
	m_b = 0.0f;
	m_c = 0.0f;
	m_d = 0.0f;

	m_invalidflag = 0;
	return 0;
}
	
int CPCoef::SetPCFrom3V( D3DXVECTOR3* srcv1, D3DXVECTOR3* srcv2, D3DXVECTOR3* srcv3 )
{
	if( (*srcv1 == *srcv2) || (*srcv1 == *srcv3) || (*srcv2 == *srcv3) ){
		InitParams();
		m_invalidflag = 1;
		return 0;
	}

	D3DXVECTOR3 v1, v2, cross_v1v2;
	v1 = *srcv2 - *srcv1;
	v2 = *srcv3 - *srcv1;
	
	D3DXVec3Cross( &cross_v1v2, &v1, &v2 );
	D3DXVec3Normalize( &cross_v1v2, &cross_v1v2 );

	m_a = cross_v1v2.x;
	m_b = cross_v1v2.y;
	m_c = cross_v1v2.z;


	m_d = -D3DXVec3Dot( &cross_v1v2, srcv1 );
	


	return 0;
}

int CPCoef::SetPC( D3DXVECTOR3 srcn, float srcd )
{

	m_a = srcn.x;
	m_b = srcn.y;
	m_c = srcn.z;

	m_d = srcd;

	return 0;
}

int CPCoef::OnPlane( D3DXVECTOR3 srcv, float* dstval )
{

	*dstval = m_a * srcv.x + m_b * srcv.y + m_c * srcv.z + m_d;

	return 0;
}

int CPCoef::Inverse()
{
	m_a *= -1.0f;
	m_b *= -1.0f;
	m_c *= -1.0f;
	m_d *= -1.0f;

	return 0;
}