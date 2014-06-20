#include <stdafx.h> //ダミー
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#ifndef SHDTREEVIEWH
#include <windows.h>
#endif

#include <math.h>

#include <bezmesh.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


///////////////////
// CBezMesh
///////////////////

CBezMesh::CBezMesh()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( " : CBezMesh::CBezMesh : CreateObj error !!!\n" );
		isinit = 0;
	}
}

CBezMesh::~CBezMesh()
{
	DestroyObjs();
}

void	CBezMesh::InitParams()
{
	CBaseDat::InitParams();

	meshinfo = 0;
	//blineptr = 0;	
	hbline = 0;
	hblseri = 0;
}
void	CBezMesh::ResetParams()
{
	DestroyObjs();
	InitParams();
}
int	CBezMesh::CreateObjs()
{
	return 0;
}
void	CBezMesh::DestroyObjs()
{
	CBaseDat::DestroyObjs();

	DestroyBezMesh();
}
		
int CBezMesh::InitBezMesh( CMeshInfo* srcmeshinfo, int setleng )
{
	int ret;


#ifdef _DEBUG

	ret = CheckMeshInfo( srcmeshinfo );
	if( ret ){
		DbgOut( " : CBezMesh : InitBezMesh : CheckMeshInfo error !!!\n" );
		return 1;
	}

#endif

	DestroyBezMesh();
	

	ret = CreateBezMesh( srcmeshinfo, setleng );
	if( ret ){
		DbgOut( " : CBezMesh : InitBezMesh : CreateBezMesh error !!!\n" );
		return 1;
	}

	DbgOut( " : CBezMesh : InitBezMesh : CreateBezMesh\n" );

	return 0;	
	
}
int CBezMesh::InitBezMesh( CBezMesh* srcbmesh )
{
	int ret, setleng;
	CMeshInfo* tempinfo = 0;

	if( !srcbmesh )
		return 1;

	setleng = srcbmesh->meshinfo->n;

	ret = InitBezMesh( srcbmesh->meshinfo, setleng );
	delete [] tempinfo;
	return ret;


}
int	CBezMesh::CreateBezMesh( CMeshInfo* srcmeshinfo, int setleng )
{
	// meshinfo のチェックをしてから呼ぶ

	int ret1;
	CMeshInfo lineinfo0;
	CBezLine* curbl = 0;

	// meshinfo
	ret1 = srcmeshinfo->NewMeshInfo( &meshinfo );
	if( ret1 ){
		DbgOut( "charpos2.cpp : CBezMesh : CreateBezMesh ; srcmeshinfo->NewMeshinfo error !!!\n" );
		return 1;
	}

	_ASSERT( (meshinfo->type > SHDTYPENONE) && (meshinfo->type < SHDTYPEMAX) );

	ret1 = CBaseDat::SetType( meshinfo->type );
	if( ret1 ){
		DbgOut( "charpos2.cpp : CBezMesh : CreateBezMesh : base::SetType error !!!\n" );
		return 1;
	}

	hbline = (CBezLine**)malloc( sizeof(CBezLine*) * setleng );
	if( !hbline ){
		DbgOut( "CBezMesh : CreateBezMesh : hbline error !!!\n" );
		return 1;
	}
	ZeroMemory( hbline, sizeof(CBezLine*) * setleng );
	
	hblseri = (int*)malloc( sizeof( int ) * setleng );
	if( !hblseri ){
		DbgOut( "CBezMesh : CreateBezMesh : hblseri error !!!\n" );
		return 1;
	}
	ZeroMemory( hblseri, sizeof( int ) * setleng );

	return 0;


}
int CBezMesh::DestroyBezMesh()
{

	if( meshinfo ){
		delete meshinfo;
		meshinfo = 0;
	}

	// BezLine自体の開放はここではしない。
	if( hbline ){
		free( hbline );
		hbline = 0;
	}

	if( hblseri ){
		free( hblseri );
		hblseri = 0;
	}

	return 0;	
}

int CBezMesh::SetMem( int* srcint, __int64 setflag )
{
	int ret;
	if( setflag & BASE_TYPE ){
		ret = CBaseDat::SetMem( srcint, BASE_TYPE );
		if( ret ){
			DbgOut( "charpos2.cpp : CBezMesh : SetMem : BASE_TYPE error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;
}
int CBezMesh::SetMem( int* srcint, int lineno, int kindno, __int64 setflag )
{
	int ret;
	if( setflag & MESHI_SKIP ){
		ret = (*this)( lineno, kindno )->meshinfo->SetMem( srcint, setflag );
		if( ret ){
			DbgOut( "charpos2.cpp : CBezMesh : SetMem : MESHI_SKIP error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;

}

int CBezMesh::SetMem( CBezLine* srcbline, int lineno, int blseri, __int64 setflag )
{
	int ret;
	if( setflag & SHDBEZLINE ){
		ret = CheckNo( lineno );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		*(hbline + lineno) = srcbline;
		*(hblseri + lineno) = blseri;

		return 0;
	}
	return 1;
}

int CBezMesh::SetMem( CBezMesh* srcbmesh, __int64 setflag )
{
	int ret, setleng, i;
	CBezLine* srcbline = 0;

	// setflag は　BEZ_MESH
	// srcdata は　CBezMesh*
	if( setflag == BEZ_MESH ){
		ret = CheckMeshInfo( srcbmesh->meshinfo );
		if( ret || !meshinfo ){
			DbgOut( "charpos2.cpp : CBezMesh : SetMem : type CBezMesh : meshinfo NULL !!!\n" );
			return 1;
		}

		ret = meshinfo->IsSameData( srcbmesh->meshinfo, chkmeshi, 5 );
		if( !ret ){
			DbgOut( "charpos2.cpp : CBezMesh : SetMem : type CBezMesh : meshinfo->IsSameData error !!!\n" );
			return 1;
		}

		setleng = srcbmesh->meshinfo->n;
		for( i = 0; i < setleng; i++ ){
			srcbline = (*srcbmesh)( i );
			ret = (*this)( i )->SetMem( srcbline, BEZLINE_ALL );
			if( ret ){
				DbgOut( "charpos2.cpp : CBezMesh : SetMem : type CBezMesh : bline->SetMem error !!!\n" );
				return 1;
			}
		}
		return 0;
	}
	return 1;
}

int CBezMesh::SetMem( CBezData* srcbez, int srclineno, int srcdatno, __int64 setflag )
{
	CBezLine* dstbl = 0;
	int ret;

	if( setflag == BEZPOINT_ALL ){
		dstbl = (*this)( srclineno );
		ret = dstbl->SetMem( srcbez, srcdatno, BEZPOINT_ALL );		
		if( ret ){
			DbgOut( "CBezMesh : SetMem : type CBezData : bline->SetMem error !!!\n" );
			return 1;
		}
	}

	return 1;
}

int	CBezMesh::CopyData( CBezMesh* srcdata )
{
	int ret, ret1, setleng, i;
	CBezLine* dstbl = 0;


	if( !srcdata ){
		DbgOut( "charpos2.cpp : CBezMesh : CopyData : srcdata NULL !!!\n" );
		return 1;
	}

	ret1 = CheckMeshInfo( srcdata->meshinfo );
	if( ret1 ){
		DbgOut( "charpos2.cpp : CBezMesh : CopyData : srcdata->meshinfo invalid !!!\n" );
		return 1;
	}


	ret1 = CBaseDat::CopyData( (CBaseDat*)srcdata );
	if( ret1 ){
		DbgOut( "charpos2.cpp : CBezMesh : CopyData : basedat->CopyData error !!!\n" );
		return 1;
	}

	ret1 = InitBezMesh( srcdata );
	if( ret1 ){
		DbgOut( "charpos2.cpp : CBezMesh : CopyData1 : InitBezMesh error !!!\n" );
		return 1;
	}

	setleng = srcdata->meshinfo->n;

	for( i = 0; i < setleng; i++ ){
		dstbl = (*this)( i );
		ret = dstbl->CopyData( (*srcdata)( i ) );
		if( ret1 ){
			DbgOut( "charpos2.cpp : CBezMesh : CopyData : bline->CopyData error !!!\n" );
			return 1;
		}
	}

	chkflag = srcdata->chkflag;

	return 0;
}


int	CBezMesh::DumpMem( HANDLE hfile, int tabnum, int dumpflag )
{
	int ret;
	//int i;

	ret = CBaseDat::DumpMem( hfile, tabnum, "3DOBJ" );
	if( ret ){
		DbgOut( "CBezMesh : DumpMem : basedat::DumpMem error !!!\n" );
		return 1;
	}

	tabnum++;
	ret = meshinfo->DumpMem( hfile, tabnum, dumpflag );
	if( ret ){
		DbgOut( "CBezMesh : DumpMem : meshinfo->DumpMem error !!!\n" );
		return 1;
	}

	/***
	// hblineは、独立したtreeelemとして存在するので、ダンプも任せる。
	for( i = 0; i < meshinfo->n; i++ ){
		ret = (*this)( i )->DumpMem( hfile, tabnum, dumpflag );
		if( ret ){
			DbgOut( "CBezMesh : DumpMem : bline->DumpMem error !!!\n" );
			return 1;
		}
	}
	***/

	return 0;
}


int CBezMesh::CheckMeshInfo( CMeshInfo* srcmeshinfo )
{
	//meshinfo->m = 1; meshinfo->n = 任意;
	int mnum, nnum, totalnum;

	if( !srcmeshinfo )
		return 1;

	mnum = srcmeshinfo->m;
	nnum = srcmeshinfo->n;
	totalnum = srcmeshinfo->total;

	if( (mnum == 1) && totalnum && (totalnum == (mnum * nnum)) )
		return 0;
	else
		return 1;
	
}

int CBezMesh::CheckLineInfo( CMeshInfo* lineinfo, int infonum )
{
	//eachline->meshinfo->m = LIM_MAX; eachline->meshinfo->n = 1;
	int i, mnum0;

	if( !lineinfo )
		return 1;

	mnum0 = lineinfo->m;
	for( i = 0; i < infonum; i++ ){
		if( !(lineinfo + i) )
			return 1;

		if( ((lineinfo + i)->m != mnum0) || ((lineinfo + i)->n != 1) )
			return 1;
	}
	return 0;
	
}

int CBezMesh::GetSkipMax()
{
	int i;
	int leng = meshinfo->n;
	int curskip = 0;
	int skipmax = 0;

	if( !hbline ){
		DbgOut( "CBezMesh : GetSkipMax : hbline NULL error !!!\n" );
		return -1;
	}

	for( i = 0; i < leng; i++ ){
		curskip = (*this)( i )->meshinfo->skip;
		if( curskip > skipmax )
			skipmax = curskip;
	}

	return skipmax;
}
int CBezMesh::SetEvenData( CBezMesh* srcbm, int srcbdivU, int srcbdivV )
{
	int ret;
	int dstlnum, dstpnum, dstkindnum, srclnum, srcpnum, srckindnum, mclosed;
	CBezLine* dstbl;
	CBezLine* srcbl;
	CVecLine* dstvl;
	CVecLine* srcvl;
	int lno, kno, pno;
	int dstlstep, dstpstep;
	VEC3F* srcvec;

	dstlnum = meshinfo->n;
	mclosed = meshinfo->mclosed;
	dstbl = (*this)( 0 );
	dstkindnum = dstbl->meshinfo->m; 
	dstvl = (*dstbl)( 0 );
	dstpnum = dstvl->meshinfo->m;

	srclnum = srcbm->meshinfo->n;
	srcbl = (*srcbm)( 0 );
	srckindnum = srcbl->meshinfo->m;
	srcvl = (*srcbl)( 0 );
	srcpnum = srcvl->meshinfo->m;

	_ASSERT( dstkindnum == srckindnum );

	if( srcbdivV ){
		_ASSERT( dstlnum == (srclnum * 2 - 1) );
		dstlstep = 2;
	}else{
		_ASSERT( dstlnum == srclnum );
		dstlstep = 1;
	}

	if( srcbdivU ){
		if( mclosed == 0 ){
			_ASSERT( dstpnum == (srcpnum * 2 - 1) );
		}else{
			_ASSERT( dstpnum == (srcpnum * 2) );
		}
		dstpstep = 2;
	}else{
		_ASSERT( dstpnum == srcpnum );
		dstpstep = 1;
	}

	for( lno = 0; lno < srclnum; lno++ ){
		for( kno = 0; kno < dstkindnum; kno++ ){
			dstvl = (*this)( lno * dstlstep, kno );
			srcvl = (*srcbm)( lno, kno );

			for( pno = 0; pno < srcpnum; pno++ ){
				srcvec = srcvl->GetVec( pno );
				ret = dstvl->SetMem( srcvec, pno * dstpstep, VEC_ALL );
				_ASSERT( !ret );
			}
		}
	}
	return 0;
}


/***
int CBezMesh::SetEvenData( CBezMesh* srcbm )
{
	int ret;
	int dstlnum, dstpnum, dstkindnum, srclnum, srcpnum, srckindnum, mclosed;
	CBezLine* dstbl;
	CBezLine* srcbl;
	CVecLine* dstvl;
	CVecLine* srcvl;
	int lno, kno, pno;
	VEC3F* srcvec;

	dstlnum = meshinfo->n;
	mclosed = meshinfo->mclosed;
	dstbl = (*this)( 0 );
	dstkindnum = dstbl->meshinfo->m; 
	dstvl = (*dstbl)( 0 );
	dstpnum = dstvl->meshinfo->m;

	srclnum = srcbm->meshinfo->n;
	srcbl = (*srcbm)( 0 );
	srckindnum = srcbl->meshinfo->m;
	srcvl = (*srcbl)( 0 );
	srcpnum = srcvl->meshinfo->m;

	_ASSERT( dstlnum == (srclnum * 2 - 1) );
	_ASSERT( dstkindnum == srckindnum );
	if( mclosed == 0 ){
		_ASSERT( dstpnum == (srcpnum * 2 - 1) );
	}else{
		_ASSERT( dstpnum == (srcpnum * 2) );
	}

	//srclno = 0;
	for( lno = 0; lno < srclnum; lno++ ){
		for( kno = 0; kno < dstkindnum; kno++ ){
			dstvl = (*this)( lno * 2, kno );
			srcvl = (*srcbm)( lno, kno );

			for( pno = 0; pno < srcpnum; pno++ ){
				srcvec = srcvl->GetVec( pno );
				ret = dstvl->SetMem( srcvec, pno * 2, VEC_ALL );
				_ASSERT( !ret );
			}
		}
		//srclno++;
	}
	return 0;
}
***/

int CBezMesh::CalcBezDiv_U( int lineno, int newno, int befno, int aftno )
{
	VEC3F newp[LIM_MAX];
	VEC3F befp[LIM_MAX];
	VEC3F aftp[LIM_MAX];
	VEC3F S1;
	VEC3F befp_out, aftp_in;

	int ret, kno;
	CVecLine* vl;

	ZeroMemory( &newp, sizeof( VEC3F ) );
	ZeroMemory( &befp, sizeof( VEC3F ) );
	ZeroMemory( &aftp, sizeof( VEC3F ) );

	for( kno = 0; kno < LIM_MAX; kno++ ){
		vl = (*this)( lineno, kno );
		befp[kno] = *(vl->GetVec( befno ));
		aftp[kno] = *(vl->GetVec( aftno ));
	}
	
	//befp_out = 0.50f * ( befp[LIM_POS] + befp[LIM_OUTH] ); // S0
	befp_out.x = 0.50f * ( (befp[LIM_POS]).x + (befp[LIM_OUTH]).x );
	befp_out.y = 0.50f * ( (befp[LIM_POS]).y + (befp[LIM_OUTH]).y );
	befp_out.z = 0.50f * ( (befp[LIM_POS]).z + (befp[LIM_OUTH]).z );
	//S1 = 0.50f * ( befp[LIM_OUTH] + aftp[LIM_INH] ); // S1
	S1.x = 0.50f * ( (befp[LIM_OUTH]).x + (aftp[LIM_INH]).x );
	S1.y = 0.50f * ( (befp[LIM_OUTH]).y + (aftp[LIM_INH]).y );
	S1.z = 0.50f * ( (befp[LIM_OUTH]).z + (aftp[LIM_INH]).z );
	//aftp_in = 0.50f * ( aftp[LIM_INH] + aftp[LIM_POS] ); // S2
	aftp_in.x = 0.50f * ( (aftp[LIM_INH]).x + (aftp[LIM_POS]).x );
	aftp_in.y = 0.50f * ( (aftp[LIM_INH]).y + (aftp[LIM_POS]).y );
	aftp_in.z = 0.50f * ( (aftp[LIM_INH]).z + (aftp[LIM_POS]).z );

	//newp[LIM_INH] = 0.50f * ( befp_out + S1 ); // S3
	(newp[LIM_INH]).x = 0.50f * ( befp_out.x + S1.x );
	(newp[LIM_INH]).y = 0.50f * ( befp_out.y + S1.y );
	(newp[LIM_INH]).z = 0.50f * ( befp_out.z + S1.z );
	//newp[LIM_OUTH] = 0.50f * ( S1 + aftp_in ); // S4
	(newp[LIM_OUTH]).x = 0.50f * ( S1.x + aftp_in.x );
	(newp[LIM_OUTH]).y = 0.50f * ( S1.y + aftp_in.y );
	(newp[LIM_OUTH]).z = 0.50f * ( S1.z + aftp_in.z );
	//newp[LIM_POS] = 0.50f * ( newp[LIM_INH] + newp[LIM_OUTH] ); // S5
	(newp[LIM_POS]).x = 0.50f * ( (newp[LIM_INH]).x + (newp[LIM_OUTH]).x );
	(newp[LIM_POS]).y = 0.50f * ( (newp[LIM_INH]).y + (newp[LIM_OUTH]).y );
	(newp[LIM_POS]).z = 0.50f * ( (newp[LIM_INH]).z + (newp[LIM_OUTH]).z );
	
	for( kno = 0; kno < LIM_MAX; kno++ ){
		vl = (*this)( lineno, kno );

		ret = vl->SetMem( (newp + kno), newno, VEC_ALL );
		_ASSERT( !ret );

		if( kno == LIM_INH ){
			ret = vl->SetMem( &aftp_in, aftno, VEC_ALL );
			_ASSERT( !ret );
		}else if( kno == LIM_OUTH ){
			ret = vl->SetMem( &befp_out, befno, VEC_ALL );
			_ASSERT( !ret );
		}
	}

	return 0;
}

int CBezMesh::CalcNewpLat( int lineno, int beflno, int aftlno, int pno )
{
	int ret;
	CVecLine* vl;
	VEC3F newp;
	VEC3F newplatin, newplatout;

	/////
	vl = (*this)( lineno, LIM_POS );
	newp = *(vl->GetVec( pno ));


	//newp[LIM_LATINH] = newp[LIM_POS];
	newplatin.x = newp.x;
	newplatin.y = newp.y;
	newplatin.z = newp.z;

	//newp[LIM_LATOUTH] = newp[LIM_POS];
	newplatout.x = newp.x;
	newplatout.y = newp.y;
	newplatout.z = newp.z;

	vl = (*this)( lineno, LIM_LATINH );
	ret = vl->SetMem( &newplatin, pno, VEC_ALL );
	_ASSERT( !ret );

	vl = (*this)( lineno, LIM_LATOUTH );
	ret = vl->SetMem( &newplatout, pno, VEC_ALL );
	_ASSERT( !ret );

	return 0;
}

/***
int CBezMesh::CalcNewpLat( int lineno, int beflno, int aftlno, int pno )
{
	int ret;
	float magout2, magout, magin2, magin;
	float rate;
	CVecLine* vl;
	VEC3F newp, beflp, aftlp;
	VEC3F newplatin, newplatout;

	/////
	vl = (*this)( lineno, LIM_POS );
	newp = *(vl->GetVec( pno ));

	vl = (*this)( beflno, LIM_POS );
	beflp = *(vl->GetVec( pno ));

	vl = (*this)( aftlno, LIM_POS );
	aftlp = *(vl->GetVec( pno ));

	//newplatout = newp - beflp;
	newplatout.x = newp.x - beflp.x;
	newplatout.y = newp.y - beflp.y;
	newplatout.z = newp.z - beflp.z;
	magout2 = newplatout.x * newplatout.x + newplatout.y * newplatout.y + newplatout.z * newplatout.z;
	if( magout2 > 1e-6 )
		magout = sqrtf( magout2 );
	else
		magout = 0.0f;
	if( magout != 0.0f ){
		newplatout.x /= magout;
		newplatout.y /= magout;
		newplatout.z /= magout;
	}else{
		newplatout.x = 0.0f;
		newplatout.y = 0.0f;
		newplatout.z = 0.0f;
	}


	//newplatin = newp - aftlp
	newplatin.x = newp.x - aftlp.x;
	newplatin.y = newp.y - aftlp.y;
	newplatin.z = newp.z - aftlp.z;
	magin2 = newplatin.x * newplatin.x + newplatin.y * newplatin.y + newplatin.z * newplatin.z;
	if( magin2 > 1e-6 )
		magin = sqrtf( magin2 );
	else
		magin = 0.0f;
	if( magin != 0.0f ){
		newplatin.x /= magin;
		newplatin.y /= magin;
		newplatin.z /= magin;
	}else{
		newplatin.x = 0.0f;
		newplatin.y = 0.0f;
		newplatin.z = 0.0f;
	}

	rate = 0.10f;
	// newplatout = newplatout * magin * rate + newp;
	newplatout.x = newplatout.x * magin * rate + newp.x;
	newplatout.y = newplatout.y * magin * rate + newp.y;
	newplatout.z = newplatout.z * magin * rate + newp.z;

	// newplatin = newplatin * magout * rate + newp;
	newplatin.x = newplatin.x * magout * rate + newp.x;
	newplatin.y = newplatin.y * magout * rate + newp.y;
	newplatin.z = newplatin.z * magout * rate + newp.z;

	vl = (*this)( lineno, LIM_LATINH );
	ret = vl->SetMem( &newplatin, pno, VEC_ALL );
	_ASSERT( !ret );

	vl = (*this)( lineno, LIM_LATOUTH );
	ret = vl->SetMem( &newplatout, pno, VEC_ALL );
	_ASSERT( !ret );

	return 0;
}
***/
/***
int CBezMesh::CalcNewpLat( int lineno, int beflno, int aftlno, int pno )
{
	int ret;
	CVecLine* vl;
	VEC3F newp, beflp, aftlp;
	VEC3F newplatin, newplatout;

	/////
	vl = (*this)( lineno, LIM_POS );
	newp = *(vl->GetVec( pno ));

	vl = (*this)( beflno, LIM_POS );
	beflp = *(vl->GetVec( pno ));

	vl = (*this)( aftlno, LIM_POS );
	aftlp = *(vl->GetVec( pno ));

	//newp[LIM_LATINH] = newp[LIM_POS];
	//newp[LIM_LATOUTH] = newp[LIM_POS];

	//newp[LIM_LATINH] = 0.3333f * ( beflp[LIM_POS] + 2.0f * newp[LIM_POS] );
	newplatin.x = 0.3333f * ( beflp.x + 2.0f * newp.x );
	newplatin.y = 0.3333f * ( beflp.y + 2.0f * newp.y );
	newplatin.z = 0.3333f * ( beflp.z + 2.0f * newp.z );

	//newp[LIM_LATOUTH] = 0.3333f * ( aftlp + 2.0f * newp[LIM_POS] ); 
	newplatout.x = 0.3333f * ( aftlp.x + 2.0f * newp.x ); 
	newplatout.y = 0.3333f * ( aftlp.y + 2.0f * newp.y ); 
	newplatout.z = 0.3333f * ( aftlp.z + 2.0f * newp.z ); 

	vl = (*this)( lineno, LIM_LATINH );
	ret = vl->SetMem( &newplatin, pno, VEC_ALL );
	_ASSERT( !ret );

	vl = (*this)( lineno, LIM_LATOUTH );
	ret = vl->SetMem( &newplatout, pno, VEC_ALL );
	_ASSERT( !ret );

	return 0;
}
***/

int CBezMesh::CalcBezDiv_V( int pno, int newlno, int beflno, int aftlno )
{
	VEC3F newp[LIM_MAX];
	VEC3F befp[LIM_MAX];
	VEC3F aftp[LIM_MAX];
	CVecLine* befline;
	CVecLine* aftline;
	CVecLine* newline;
	int ret, kno;

	ZeroMemory( newp, sizeof( VEC3F ) * LIM_MAX );
	ZeroMemory( befp, sizeof( VEC3F ) * LIM_MAX );
	ZeroMemory( aftp, sizeof( VEC3F ) * LIM_MAX );
	
	for( kno = 0; kno < LIM_MAX; kno++ ){
		befline = (*this)( beflno, kno );
		aftline = (*this)( aftlno, kno );

		befp[kno] = *(befline->GetVec( pno ));
		aftp[kno] = *(aftline->GetVec( pno ));
	}

	// newp[LIM_POS] = 0.1250f * ( befp[LIM_POS] + 3.0f * befp[LIM_LATOUTH] + 3.0f * aftp[LIM_LATINH] + aftp[LIM_POS] );
	(newp[LIM_POS]).x = 0.1250f * ( (befp[LIM_POS]).x + 3.0f * (befp[LIM_LATOUTH]).x + 3.0f * (aftp[LIM_LATINH]).x + (aftp[LIM_POS]).x );
	(newp[LIM_POS]).y = 0.1250f * ( (befp[LIM_POS]).y + 3.0f * (befp[LIM_LATOUTH]).y + 3.0f * (aftp[LIM_LATINH]).y + (aftp[LIM_POS]).y );
	(newp[LIM_POS]).z = 0.1250f * ( (befp[LIM_POS]).z + 3.0f * (befp[LIM_LATOUTH]).z + 3.0f * (aftp[LIM_LATINH]).z + (aftp[LIM_POS]).z );

	newline = (*this)( newlno, LIM_POS );
	ret = newline->SetMem( newp, pno, VEC_ALL );
	_ASSERT( !ret );	


	return 0;
}





