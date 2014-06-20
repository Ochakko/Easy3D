#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <lightdata.h>

//#define	DBGH
//#include "dbg.h"

#include <crtdbg.h>


//// static
static int s_lightcnt = 0;

/////////

CLightData::CLightData( int* dstlid )
{
	Type = -1;
	
//	ZeroMemory( &Diffuse, sizeof( CVec3f ) );
//	ZeroMemory( &Specular, sizeof( CVec3f ) );
//	ZeroMemory( &Ambient, sizeof( CVec3f ) );

	Diffuse.x = 1.0f;	
	Diffuse.y = 1.0f;
	Diffuse.z = 1.0f;

	Specular.x = 0.7f;
	Specular.y = 0.7f;
	Specular.z = 0.7f;

	Ambient.x = 0.5f;
	Ambient.y = 0.5f;
	Ambient.z = 0.5f;

/***
	Diffuse.x = 0.5f;
	Diffuse.y = 0.5f;
	Diffuse.z = 0.5f;

	Specular.x = 0.5f;
	Specular.y = 0.5f;
	Specular.z = 0.5f;

	Ambient.x = 0.0f;
	Ambient.y = 0.0f;
	Ambient.z = 0.0f;
***/

	ZeroMemory( &orgPosition, sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &traPosition, sizeof( D3DXVECTOR3 ) );

	orgDirection.x = 0.0f;
	orgDirection.y = 0.0f;
	orgDirection.z = 1.0f;
	D3DXVec3Normalize( &orgDirection, &orgDirection );

	traDirection = orgDirection;

	Range = 1.0f;//距離 （の２乗にする）
	Falloff = 1.0f;
	Attenuation0 = 1.0f;
	Attenuation1 = 0.0f;
	Attenuation2 = 0.0f;

	Theta = 0.0f;//内部コーン　ラジアン
	Phi = 0.0f;//SPOTの外側エッジ　ラジアン

	divRange = 1.0f;// 1 / Range
	cosPhi = 0.0f;// cos( Phi )
	divcosPhi = 0.0f;// 1 / (1 - cos( Phi ))

	ZeroMemory( reserved0, sizeof( float ) * 3 );
	ZeroMemory( reserved1, sizeof( float ) * 3 );
	ZeroMemory( reserved2, sizeof( float ) * 3 );

	lblend = LBLEND_MULT;

	nextlight = 0;

	lightid = s_lightcnt;
	s_lightcnt++;
	*dstlid = lightid;

}
CLightData::~CLightData()
{


}


int CLightData::SetDirectionalLight( D3DXVECTOR3 dir, CVec3f difcol )
{
	Type = D3DLIGHT_DIRECTIONAL;

	orgDirection = dir;
	D3DXVec3Normalize( &orgDirection, &orgDirection );

	Diffuse = difcol;

	if( Diffuse.x < 0.0f )
		Diffuse.x = 0.0f;
	else if( Diffuse.x > 1.0f )
		Diffuse.x = 1.0f;

	if( Diffuse.y < 0.0f )
		Diffuse.y = 0.0f;
	else if( Diffuse.y > 1.0f )
		Diffuse.y = 1.0f;

	if( Diffuse.z < 0.0f )
		Diffuse.z = 0.0f;
	else if( Diffuse.z > 1.0f )
		Diffuse.z = 1.0f;

	return 0;
}

int CLightData::SetPointLight( D3DXVECTOR3 pos, float dist, CVec3f difcol )
{
	Type = D3DLIGHT_POINT;

	orgPosition = pos;
	
	Range = dist * dist;
	if( Range == 0.0f )
		Range = 1.0f;


	_ASSERT( Range != 0.0f );
	divRange = 1.0f / Range;


	Diffuse = difcol;

	if( Diffuse.x < 0.0f )
		Diffuse.x = 0.0f;
	else if( Diffuse.x > 1.0f )
		Diffuse.x = 1.0f;

	if( Diffuse.y < 0.0f )
		Diffuse.y = 0.0f;
	else if( Diffuse.y > 1.0f )
		Diffuse.y = 1.0f;

	if( Diffuse.z < 0.0f )
		Diffuse.z = 0.0f;
	else if( Diffuse.z > 1.0f )
		Diffuse.z = 1.0f;


	return 0;
}

int CLightData::SetSpotLight( D3DXVECTOR3 pos, D3DXVECTOR3 dir, float dist, float deg, CVec3f difcol )
{
	Type = D3DLIGHT_SPOT;
	orgPosition = pos;

	orgDirection = dir;
	D3DXVec3Normalize( &orgDirection, &orgDirection );


	Range = dist * dist;
	if( Range < 1.0f )
		Range = 1.0f;
	
	_ASSERT( Range != 0.0f );
	divRange = 1.0f / Range;

	if( deg < 0.0f )
		deg *= -1.0f;

	if( deg > 90.0f ){
		Phi = PI * 0.5f;
	}else if( deg >= 1.0f ){
		Phi = deg * (float)DEG2PAI;
	}else{
		Phi = 1.0f * (float)DEG2PAI;
	}
	
	cosPhi = (float)cos( Phi );

	_ASSERT( cosPhi != 1.0f );
	divcosPhi = 1.0f / ( 1.0f - cosPhi );


	Diffuse = difcol;

	if( Diffuse.x < 0.0f )
		Diffuse.x = 0.0f;
	else if( Diffuse.x > 1.0f )
		Diffuse.x = 1.0f;

	if( Diffuse.y < 0.0f )
		Diffuse.y = 0.0f;
	else if( Diffuse.y > 1.0f )
		Diffuse.y = 1.0f;

	if( Diffuse.z < 0.0f )
		Diffuse.z = 0.0f;
	else if( Diffuse.z > 1.0f )
		Diffuse.z = 1.0f;


	return 0;
}

int CLightData::TransformDirection( D3DXMATRIX matView, D3DXMATRIX matProj )
{
	D3DXMATRIX mat;

	//mat = matView * matProj;
	mat = matView;

	D3DXVECTOR3 templight;
	float lx = orgDirection.x;
	float ly = orgDirection.y;
	float lz = orgDirection.z;

	//！！！！　平行移動成分は、加味しない。
	templight.x = mat._11*lx + mat._21*ly + mat._31*lz;// + mat._41;
	templight.y = mat._12*lx + mat._22*ly + mat._32*lz;// + mat._42;
	templight.z = mat._13*lx + mat._23*ly + mat._33*lz;// + mat._43;

	D3DXVec3Normalize( &traDirection, &templight );

///////
	float px = orgPosition.x;
	float py = orgPosition.y;
	float pz = orgPosition.z;

	traPosition.x = mat._11*px + mat._21*py + mat._31*pz + mat._41;
	traPosition.y = mat._12*px + mat._22*py + mat._32*pz + mat._42;
	traPosition.z = mat._13*px + mat._23*py + mat._33*pz + mat._43;


	return 0;
}

