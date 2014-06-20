#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <billboardelem.h>


CBillboardElem::CBillboardElem()
{
	InitParams();

}
CBillboardElem::~CBillboardElem()
{
	DestroyObjs();
}

void CBillboardElem::InitParams()
{
	selem = 0;
	pos.x = 0.0f;
	pos.y = 0.0f;
	pos.z = 0.0f;

	befpos = pos;
	particleR = 0.0f;

	dwOffset = 0;

	dispflag = 1;/////
	useflag = 0;

	D3DXMatrixIdentity( &rotmat );

	InitParticleParams();
}
void CBillboardElem::InitParticleParams()
{

	createtime = 0.0f;//[sec]
	ZeroMemory( &pos0, sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &vel0, sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &pos, sizeof( D3DXVECTOR3 ) );

	m_textileno = -1;
}
void CBillboardElem::DestroyObjs()
{

}


int CBillboardElem::Rotate( float fdeg, int rotkind )
{

	float rotrad;
	rotrad = fdeg * (float)DEG2PAI;
	
	if( rotkind == 0 ){
		D3DXMATRIX addrot;
		D3DXMatrixRotationZ( &addrot, rotrad );

		rotmat = rotmat * addrot;

	}else{
		D3DXMatrixRotationZ( &rotmat, rotrad );
	}

	return 0;
}



