#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <GPData.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CGPData::CGPData()
{
	InitParams();
}
CGPData::~CGPData()
{
	DestroyObjs();
}

int CGPData::SetGPE( GPELEM* srcgpe )
{
	m_gpe = *srcgpe;

	CalcMatWorld();

	return 0;
}
int CGPData::CalcMatWorld()
{

#ifdef INEASY3D
	Deg2Q( m_gpe.e3drot, &m_q );
#else
	Deg2Q( m_gpe.rot, &m_q );
#endif

	m_matWorld = m_q.MakeRotMatX();

#ifdef INEASY3D
	m_matWorld._41 = m_gpe.e3dpos.x;
	m_matWorld._42 = m_gpe.e3dpos.y;
	m_matWorld._43 = m_gpe.e3dpos.z;
#else
	m_matWorld._41 = m_gpe.pos.x;
	m_matWorld._42 = m_gpe.pos.y;
	m_matWorld._43 = m_gpe.pos.z;
#endif
	return 0;
}

int CGPData::GetMatWorld( D3DXMATRIX* dstmat )
{
	*dstmat = m_matWorld;
	return 0;
}

int CGPData::InitParams()
{
	m_gpe.ongmode = GROUND_NONE;
	m_gpe.pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_gpe.rot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_gpe.rayy = 5000.0f;
	m_gpe.rayleng = 8000.0f;
	m_gpe.offsety = 10.0f;

	m_gpe.e3dpos = m_gpe.pos;
	m_gpe.e3drot = m_gpe.rot;

	m_q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	D3DXMatrixIdentity( &m_matWorld );

	return 0;
}
int CGPData::DestroyObjs()
{
	return 0;
}

int CGPData::Deg2Q( D3DXVECTOR3 rot, CQuaternion* dstq )
{
	CQuaternion qx, qy, qz;
	D3DXVECTOR3 axisX( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 axisY( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 axisZ( 0.0f, 0.0f, 1.0f );
	qx.SetAxisAndRot( axisX, rot.x * (float)DEG2PAI );
	qy.SetAxisAndRot( axisY, rot.y * (float)DEG2PAI );
	qz.SetAxisAndRot( axisZ, rot.z * (float)DEG2PAI );

	*dstq = qy * qx * qz;

	return 0;
}
