#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <part.h>

#define	DBGH
#include <dbg.h>

#include "shdhandler.h"
#include "mothandler.h"
#include "d3ddisp.h"

#include "quaternion.h"
#include "shdelem.h"

#include <crtdbg.h>


CPart::CPart()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( "CPart : constructor : CreateObj error !!!\n" );
		isinit = 0;
	}

	m_IE = 0;//!!!!!!!
		
}

CPart::~CPart()
{
	DestroyObjs();

	if( m_IE ){
		delete [] m_IE;
		m_IE = 0;
	}

}

void	CPart::InitParams()
{
	CBaseDat::InitParams();
	meshinfo = 0;

	ZeroMemory( &jointloc, sizeof( CVec3f ) );
	bonenum = 0;
	ppBI = 0;

	m_lim0.x = -179.0;
	m_lim0.y = -179.0;
	m_lim0.z = -179.0;
	m_lim1.x = 179.0;
	m_lim1.y = 179.0;
	m_lim1.z = 179.0;

	m_ignorelim01 = 1;

	m_axisq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	m_za4_type = ZA_1;
	m_za4_rotaxis = ROTAXIS_X;
	m_za4q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
	m_za4localq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

}

void	CPart::ResetParams()
{
	DestroyObjs();
	InitParams();
}

int	CPart::CreateObjs()
{
	return 0;	
}

void	CPart::DestroyObjs()
{
	CBaseDat::DestroyObjs();

	DestroyPart();

}

int CPart::InitPart( CMeshInfo* srcmeshinfo )
{
	int  ret = 0;

	DestroyPart();
	ret = CreatePart( srcmeshinfo);
	if( ret ){
		DbgOut( "CPart : InitPart : CreatePart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CPart::CreatePart( CMeshInfo* srcmeshinfo )
{
	int ret1;

	ret1 = srcmeshinfo->NewMeshInfo( &meshinfo );
	if( ret1 ){
		DbgOut( "CPart : CreatePart : srcmeshinfo->NewMeshinfo error !!!\n" );
		return 1;
	}


//dbg
	int dbg1, dbg2;
	dbg1 = srcmeshinfo->HasColor();
	dbg2 = meshinfo->HasColor();
DbgOut( "CPart : CreatePart : srcmeshinfo->hascolor %d, meshinfo->hascolor %d\n",
	   dbg1, dbg2 );

	_ASSERT( (meshinfo->type > SHDTYPENONE) && (meshinfo->type < SHDTYPEMAX) );

	ret1 = CBaseDat::SetType( meshinfo->type );
	if( ret1 ){
		DbgOut( "CPart : CreatePart : base::SetType error !!!\n" );
		return 1;
	}

	return 0;
}

int CPart::DestroyBoneInfo()
{
	if( ppBI ){
		for( int bino = 0; bino < bonenum; bino++ ){
			CBoneInfo* curbi = *(ppBI + bino);
			delete curbi;
		}
		free( ppBI );
		ppBI = 0;
	}
	bonenum = 0;

	return 0;
}
int CPart::DestroyPart()
{
	if( meshinfo ){
		delete meshinfo;
		meshinfo = 0;
	}

	ZeroMemory( &jointloc, sizeof( CVec3f ) );

	DestroyBoneInfo();

	return 0;
}

int CPart::SetMaterial( CVec3f* srcvec )
{
	int ret1, ret2, ret3;

	ret1 = SetDiffuse( srcvec );
	ret2 = SetSpecular( srcvec + 1 );
	ret3 = SetAmbient( srcvec + 2 );

	if( ret1 || ret2 || ret3 ){
		DbgOut( "CPart : SetMaterial error %d %d %d !!!\n", ret1, ret2, ret3 );
		return 1;
	}
	return 0;
}

int CPart::SetDiffuse( CVec3f* srcvec )
{
	int ret = 0;

	ret = meshinfo->SetMem( srcvec, MAT_DIFFUSE );
	if( ret ){
		DbgOut( "CPart : SetDiffuse error !!!\n" );
		return 1;
	}
	return 0;
}

int CPart::SetSpecular( CVec3f* srcvec )
{
	int ret = 0;

	ret = meshinfo->SetMem( srcvec, MAT_SPECULAR );
	if( ret ){
		DbgOut( "CPart : SetSpecular error !!!\n" );
		return 1;
	}
	return 0;
}

int CPart::SetAmbient( CVec3f* srcvec )
{
	int ret = 0;

	ret = meshinfo->SetMem( srcvec, MAT_AMBIENT );
	if( ret ){
		DbgOut( "CPart : SetAmbient error !!!\n" );
		return 1;
	}
	return 0;
}

int CPart::SetJointLoc( CVec3f* srcvec )
{
	int ret = 0;

	ret = jointloc.CopyData( srcvec );
	if( ret ){
		DbgOut( "CPart : SetJointLoc : error !!!\n" );
		return 1;
	}
	return 0;
}

int CPart::AddBoneInfo( int srcjointno, int srcchilno, CVec3f endloc, int parno )
{
	bonenum++;
	ppBI = (CBoneInfo**)realloc( ppBI, sizeof( CBoneInfo* ) * bonenum );
	if( !ppBI ){
		DbgOut( "CPart : AddBoneInfo : ppBI NULL error !!!\n" );
		return 1;
	}

	CBoneInfo* newbi;
	newbi = new CBoneInfo();
	if( !newbi ){
		DbgOut( "CPart : AddBoneInfo : newbi NULL error !!!\n" );
		return 1;
	}
	newbi->jointno = srcjointno;
	newbi->parentjoint = parno;
	newbi->bonestart.CopyData( &jointloc );
	newbi->boneend.CopyData( &endloc );
	newbi->childno = srcchilno;//2004/4/23

	CVec3f tempvec;
	tempvec.x = endloc.x - jointloc.x;
	tempvec.y = endloc.y - jointloc.y;
	tempvec.z = endloc.z - jointloc.z;

	newbi->boneleng = tempvec.Length();

	newbi->bonecenter.x = (jointloc.x + endloc.x) * 0.5f;
	newbi->bonecenter.y = (jointloc.y + endloc.y) * 0.5f;
	newbi->bonecenter.z = (jointloc.z + endloc.z) * 0.5f;

	tempvec.Normalize();
	newbi->bonevec.CopyData( &tempvec );

	*(ppBI + bonenum - 1) = newbi;

	return 0;
}

int CPart::RestoreBoneInfo( CShdHandler* lpsh )
{
	int bino;
	CBoneInfo* biptr;

	for( bino = 0; bino < bonenum; bino++ ){

		biptr = *( ppBI + bino );
		_ASSERT( biptr );


		biptr->bonestart.CopyData( &jointloc );

		CShdElem* childelem;
		childelem = (*lpsh)( biptr->childno );
		_ASSERT( childelem );

		CVec3f endloc;
		if( !childelem->part ){
			DbgOut( "part : RestoreBoneInfo : childelem->part NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		endloc = childelem->part->jointloc;
		biptr->boneend.CopyData( &endloc );

		CVec3f tempvec;
		tempvec.x = endloc.x - jointloc.x;
		tempvec.y = endloc.y - jointloc.y;
		tempvec.z = endloc.z - jointloc.z;

		biptr->boneleng = tempvec.Length();

		tempvec.Normalize();
		biptr->bonevec.CopyData( &tempvec );
	}

	return 0;
}


int CPart::CopyData( CPart* srcdata )
{
	int ret = 0;

	ret = meshinfo->CopyData( srcdata->meshinfo );
	if( ret ){
		DbgOut( "CPart : CopyData error !!!\n" );
		return 1;
	}


	jointloc.CopyData( &(srcdata->jointloc) );

	DestroyBoneInfo();
	bonenum = srcdata->bonenum;

	CBoneInfo** srcppBI = srcdata->ppBI;
	if( srcppBI ){
		ppBI = (CBoneInfo**)malloc( sizeof( CBoneInfo* ) * bonenum );
		if( !ppBI ){
			DbgOut( "CPart : CopyData : ppBI null error !!!\n" );
			return 1;
		}
		for( int bino = 0; bino < bonenum; bino++ ){
			CBoneInfo* newbi;
			newbi = new CBoneInfo( *(srcppBI + bino) );
			if( !newbi ){
				DbgOut( "CPart : CopyData : newbi NULL error !!!\n" );
				return 1;
			}
			*(ppBI + bino) = newbi;
		}
	}

	return 0;
}

int	CPart::DumpMem( HANDLE hfile, int tabnum, int dumpflag )
{
	int ret;

	ret = CBaseDat::DumpMem( hfile, tabnum, "PART" );
	if( ret ){
		DbgOut( "CPart : DumpMem : basedat::DumpMem error !!!\n" );
		return 1;
	}

	tabnum++;
	ret = meshinfo->DumpMem( hfile, tabnum, dumpflag );
	if( ret ){
		DbgOut( "CPart : CVecMesh : DumpMem : meshinfo->DumpMem error !!!\n" );
		return 1;
	}
	return 0;	
}

int CPart::CreateInfElemIfNot( int leng )
{
	if( m_IE )
		return 0;

	m_IE = new CInfElem[ leng ];
	if( !m_IE ){
		DbgOut( "part : CreateInfElemIfNot : m_IE alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
CShdElem* CPart::GetChildSelem( CShdHandler* lpsh, int bno )
{
	if( (bno < 0) || (bno >= bonenum) ){
		DbgOut( "part : GetChildSelem : bno out of range error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( !ppBI ){
		DbgOut( "part : GetChildSelem : ppBI NULL error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	CBoneInfo* curbi;
	curbi = *(ppBI + bno);
	_ASSERT( curbi );

	CShdElem* parelem;
	CShdElem* chilelem;
	parelem = (*lpsh)( curbi->jointno );
	chilelem = (*lpsh)( curbi->childno );
	if( !parelem || !chilelem ){
		DbgOut( "part : GetChildSelem : selem error %x %x !!!\n", parelem, chilelem );
		_ASSERT( 0 );
		return 0;
	}
	
	return chilelem;
}


int CPart::GetBoneMarkMatrix( int bno, CShdHandler* lpsh, CMotHandler* lpmh, int motid, int frameno, D3DXMATRIX matWorld, float baseleng, D3DXVECTOR3 basevec, D3DXMATRIX* bmmat )
{
	if( (bno < 0) || (bno >= bonenum) ){
		DbgOut( "part : GetBoneMarkMatrix : bno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !ppBI ){
		DbgOut( "part : GetBoneMarkMatrix : ppBI NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBoneInfo* curbi;
	curbi = *(ppBI + bno);
	_ASSERT( curbi );

	CShdElem* parelem;
	CShdElem* chilelem;
	parelem = (*lpsh)( curbi->jointno );
	chilelem = (*lpsh)( curbi->childno );
	if( !parelem || !chilelem ){
		DbgOut( "part : GetBoneMarkMatrix : selem error %x %x !!!\n", parelem, chilelem );
		_ASSERT( 0 );
		return 1;
	}
	

	CD3DDisp* pardisp;
	CD3DDisp* childisp;
	pardisp = parelem->d3ddisp;
	childisp = chilelem->d3ddisp;
	if( !pardisp || !childisp ){
		DbgOut( "part : GetBoneMarkMatarix : d3ddisp error %x %x!!!\n", pardisp, childisp );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 parpos, chilpos;
	ret = pardisp->TransformOnlyWorld1Vert( lpmh, matWorld, motid, frameno, 0, &parpos, 1, 0 );
	if( ret ){
		DbgOut( "part : GetBoneMarkMatrix : par Trans1vert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = childisp->TransformOnlyWorld1Vert( lpmh, matWorld, motid, frameno, 0, &chilpos, 1, 0 );
	if( ret ){
		DbgOut( "part : GetBoneMarkMatrix : chil Trans1vert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/***
	parpos.x = pardisp->m_preTLV->sx;
	parpos.y = pardisp->m_preTLV->sy;
	parpos.z = pardisp->m_preTLV->sz;

	chilpos.x = childisp->m_preTLV->sx;
	chilpos.y = childisp->m_preTLV->sy;
	chilpos.z = childisp->m_preTLV->sz;
	***/


	D3DXVECTOR3 par2chil;
	par2chil = chilpos - parpos;
	float dist;
	//dist = D3DXVec3Length( &par2chil );
	dist = (float)sqrt( par2chil.x * par2chil.x + par2chil.y * par2chil.y + par2chil.z * par2chil.z );
	//D3DXVec3Normalize( &par2chil, &par2chil );
	if( dist != 0.0f ){
		par2chil.x /= dist;
		par2chil.y /= dist;
		par2chil.z /= dist;
	}


	CQuaternion rotq;
	float dot = D3DXVec3Dot( &basevec, &par2chil );
	if( dot >= -0.9999f ){
		rotq.RotationArc( basevec, par2chil );
		rotq.normalize();
	}else{
		//basevecとpar2chilが１８０度の時、RotationArcは不安定。
		D3DXVECTOR3 vecx( 1.0f, 0.0f, 0.0f );
		rotq.SetAxisAndRot( vecx, 180.0f * (float)DEG2PAI );
		rotq.normalize();
	}

	D3DXMATRIX rotm;
	rotm = rotq.MakeRotMatX();


	D3DXMATRIX scalem;
	float scale;
	scale = dist / baseleng;
	D3DXMatrixScaling( &scalem, scale, scale, scale );

	*bmmat = scalem * rotm;

	bmmat->_41 = parpos.x;
	bmmat->_42 = parpos.y;
	bmmat->_43 = parpos.z;

	/***
	CQuaternion rotq;
	D3DXVECTOR3 bonevecx;
	bonevecx.x = curbi->bonevec.x;
	bonevecx.y = curbi->bonevec.y;
	bonevecx.z = curbi->bonevec.z;
	rotq.RotationArc( basevec, bonevecx );

	D3DXMATRIX rotm;
	rotm = rotq.MakeRotMatX();

	D3DXMATRIX scalem;
	float scale;
	scale = curbi->boneleng / baseleng;
	D3DXMatrixScaling( &scalem, scale, scale, scale );

	*bmmat = scalem * rotm;

	bmmat->_41 = curbi->bonestart.x;
	bmmat->_42 = curbi->bonestart.y;
	bmmat->_43 = curbi->bonestart.z;
	***/

	return 0;
}

int CPart::SetCurrentPose2OrgData( CMotHandler* srclpmh, CShdElem* selem, CQuaternion* multq )
{
	int ret;

	D3DXVECTOR3 wvec;

	D3DXMATRIX iniwmat;
	D3DXMatrixIdentity( &iniwmat );
	ret = selem->TransformOnlyWorld3( srclpmh, iniwmat, &wvec );
	if( ret ){
		DbgOut( "part : SetCurrentPose2OrgData : selem TransformOnlyWorld3 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 newmikopos;
	ret = selem->d3ddisp->TransformOnlyWorldInfOneBone( srclpmh, iniwmat, selem->m_mikobonepos, &newmikopos, selem->serialno );
	if( ret ){
		DbgOut( "part : SetCurrentPose2OrgData : d3ddisp TransformOnlyWorldInfOneBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( multq ){
		multq->Rotate( &wvec, wvec );
		multq->Rotate( &newmikopos, newmikopos );
	}

	jointloc.x = wvec.x;
	jointloc.y = wvec.y;
	jointloc.z = wvec.z;

	selem->m_mikobonepos = newmikopos;//!!!!!!!!!!!!!!!

	return 0;
}

int CPart::ConvSymmXShape()
{
	jointloc.x = -jointloc.x;
	jointloc.y = jointloc.y;
	jointloc.z = jointloc.z;

	return 0;
}


int CPart::GetBoneInfo( int childno, CBoneInfo** dstppbi )
{
	*dstppbi = 0;

	int bno;
	CBoneInfo* curbi;
	for( bno = 0; bno < bonenum; bno++ ){
		curbi = *( ppBI + bno );

		if( curbi->childno == childno ){
			*dstppbi = curbi;
			break;
		}
	}

	return 0;
}

