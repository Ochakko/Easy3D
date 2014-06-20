#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <windows.h>

#include <planes.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>
///////////////////////////
// CPlanes
///////////////////////////

CPlanes::CPlanes()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( "charpos2.cpp : CPlanes::CPlanes : CreateObj error !!!\n" );
		isinit = 0;
	}	
}
CPlanes::~CPlanes()
{
	DestroyObjs();
}

void CPlanes::InitParams()
{
	CVecMesh::InitParams();

}
int CPlanes::SetMem( CPlanes* srcplanes, __int64 setflag )
{
	int ret;
	if( setflag == SHDPLANES ){
		if( !meshinfo || !srcplanes )
			return 1;
		ret = CVecMesh::SetMem( (CVecMesh*)srcplanes, setflag );
		_ASSERT( !ret );
		return ret;
	}
	return 1;
}
int	CPlanes::CopyData( CPlanes* srcdata )
{
	int ret;
	ret = CVecMesh::CopyData( (CVecMesh*)srcdata );
	chkflag = srcdata->chkflag;
	return ret;
}
int CPlanes::InitVecMesh( CPlanes* srcdata )
{
	int ret;
	ret = CVecMesh::InitVecMesh( (CVecMesh*)srcdata );
	return ret;
}

int CPlanes::CheckMeshInfo( CMeshInfo* srcmeshinfo )
{
	//meshinfo->m = 2; meshinfo->n = îCà”;
	int mnum, nnum, totalnum;

	if( !srcmeshinfo )
		return 1;

	mnum = srcmeshinfo->m;
	nnum = srcmeshinfo->n;
	totalnum = srcmeshinfo->total;

	if( (mnum == LIM2_MAX) && totalnum && (totalnum == mnum * nnum) )
		return 0;
	else
		return 1;

}
int CPlanes::CheckLineInfo( CMeshInfo* lineinfo, int infonum )
{
	//eachline->meshinfo->m = îCà”(àÍíË); eachline->meshinfo->n = 1;
	int i, mnum0;

	mnum0 = lineinfo->m;
	for( i = 0; i < infonum; i++ ){
		if( !(lineinfo + i) )
			return 1;

		if( (mnum0 != (lineinfo + i)->m) || ((lineinfo + i)->n != 1) )
			return 1;
	}
	return 0;
}

int CPlanes::Transform( CMatrix2& transmat, CVecMesh* srcvmesh )
{
	// Ç∑Ç◊ÇƒÇÃlineÇåvéZ
	int i, srclnum;
	int ret = 0;
	CVecLine* srcline = 0;
	CVecLine* dstline = 0;

	srclnum = srcvmesh->allocleng;
	if( srclnum != allocleng )
		return 1;

	for( i = 0; i < allocleng; i++ ){
		dstline = (*this)( i );
		srcline = (*srcvmesh)( i );
		ret += dstline->Transform( transmat, srcline );
		_ASSERT( !ret );
	}
	return ret;
}

//////////////////////////////////
// CRevolved
//////////////////////////////////

CRevolved::CRevolved()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( "planes.cpp : CRevolved::CRevolved : CreateObj error !!!\n" );
		isinit = 0;
	}	

}
CRevolved::~CRevolved()
{
	DestroyObjs();

}
void CRevolved::InitParams()
{
	CPlanes::InitParams();

	axis.Identity();
	isnormalset = 0;
	radfrom = 0.0f;
	radto = 0.0f;
}
int CRevolved::SetMem( int* srcint, __int64 setflag )
{
	int ret;
	if( setflag & BASE_TYPE ){
		ret = CBaseDat::SetMem( srcint, BASE_TYPE );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecMesh : SetMem : BASE_TYPE error !!!\n" );
			return 1;
		}
		return 0;
	}else if( setflag & REV_NSET ){
		isnormalset = *srcint;
		chkflag |= REV_NSET;
		return 0;
	}
	return 1;
	

}
int CRevolved::SetMem( CRevolved* srcrev, __int64 setflag )
{
	int ret, i, j;

	if( setflag == REVOLVED_ALL ){
		if( !meshinfo || !srcrev )
			return 1;

		ret = CVecMesh::SetMem( (CVecMesh*)srcrev, setflag );
		_ASSERT( !ret );

		radfrom = srcrev->radfrom;
		radto = srcrev->radto;
		isnormalset = srcrev->isnormalset;

		for( i = 0; i < 4; i++ )
			for( j = 0; j < 4; j++ )
				axis[i][j] = (srcrev->axis)[i][j];

		chkflag |= REVOLVED_ALL;
		return ret;
	}
	return 1;
}
int CRevolved::SetMem( float* srcfloat, __int64 setflag )
{
	switch( setflag ){
	case REV_FROM:
		radfrom = *srcfloat;
		chkflag |= REV_FROM;
		break;
	case REV_TO:
		radto = *srcfloat;
		chkflag |= REV_TO;
		break;
	default:
		return 1;
		break;
	}
	return 0;
}


int CRevolved::SetMem( CMatrix2* srcmatrix, __int64 setflag )
{
	int i, j;
	if( setflag == REV_AXIS ){
		for( i = 0; i < 4; i++ )
			for( j = 0; j < 4; j++ )
				axis[i][j] = (*srcmatrix)[i][j];
		chkflag |= REV_AXIS;
		return 0;
	}
	return 1;

}

int	CRevolved::CopyData( CRevolved* srcdata )
{
	int ret, i, j;
	ret = CVecMesh::CopyData( (CVecMesh*)srcdata );
	if( ret ){
		DbgOut( "planes.cpp : CRevolved: CopyData : CVecMesh::CopyData error !!!\n" );
		return 1;
	}

	radfrom = srcdata->radfrom;
	radto = srcdata->radto;
	isnormalset = srcdata->isnormalset;
	for( i = 0; i < 4; i++ )
		for( j = 0; j < 4; j++ )
			axis[i][j] = (srcdata->axis)[i][j];
	
	chkflag = srcdata->chkflag;
	return 0;
}

int CRevolved::InitVecMesh( CRevolved* srcrev )
{
	int ret;
	ret = CVecMesh::InitVecMesh( (CVecMesh*)srcrev );
	return ret;
}
int CRevolved::DumpMem( HANDLE hfile, int tabnum, int dumpflag )
{
	/***
	int ret, i;
	char outline[1024];
	char tempchar[256];

	ret = CVecMesh::DumpMem( hfile, tabnum, dumpflag );
	if( ret ){
		DbgOut( "planes.cpp : CRevolved: DumpMem : CVecMesh::DumpMem error !!!\n" );
		return 1;
	}

	SetTabChar( tabnum );

	ZeroMemory( outline, 1024 );
	sprintf( outline, "%s radfrom : %f, radto : %f, isnormalset %d\n%s axis\n",
		tabchar, radfrom, radto, isnormalset, tabchar );

	for( i = 0; i < 4; i++ ){
		ZeroMemory( tempchar, 256 );
		sprintf( tempchar, "%s %f, %f, %f, %f\n",
			tabchar, axis[i][0], axis[i][1], axis[i][2], axis[i][3] );
		strcat( outline, tempchar );
	}

	ret = Write2File( hfile, outline );
	_ASSERT( !ret );

	return ret;
	***/
	return 0;
}
////////////////////////////
// CSphere
////////////////////////////

CSphere::CSphere()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( "charpos2.cpp : CSphere::CSphere : CreateObj error !!!\n" );
		isinit = 0;
	}	

}
CSphere::~CSphere()
{
	DestroyObjs();
}

void CSphere::InitParams()
{
	CPlanes::InitParams();
	defmat.Identity();	
}

int CSphere::SetMem( CSphere* srcdata, __int64 setflag )
{
	int ret, i, j;

	if( setflag == SPH_ALL ){
		if( !meshinfo || !srcdata )
			return 1;
		ret = CVecMesh::SetMem( (CVecMesh*)srcdata, setflag );
		_ASSERT( !ret );

		for( i = 0; i < 4; i++ )
			for( j = 0; j < 4; j++ )
				defmat[i][j] = srcdata->defmat[i][j];

		return ret;
	}
	return 1;

}
int CSphere::SetMem( CMatrix2* srcdefmat, __int64 setflag )
{
	int i, j;

	if( setflag == SPH_DEFMAT ){
		for( i = 0; i < 4; i++ )
			for( j = 0; j < 4; j++ )
				defmat[i][j] = (*srcdefmat)[i][j];
		return 0;
	}
	return 1;
}
int	CSphere::CopyData( CSphere* srcdata )
{
	int ret, i, j;

	ret = CVecMesh::CopyData( (CVecMesh*)srcdata );
	if( ret ){
		DbgOut( "charpos2.cpp : CSphere: CopyData : CVecMesh::CopyData error !!!\n" );
		return 1;
	}

	for( i = 0; i < 4; i++ )
		for( j = 0; j < 4; j++ )
			defmat[i][j] = srcdata->defmat[i][j];

	chkflag = srcdata->chkflag;
	return 0;
}
int CSphere::InitVecMesh( CSphere* srcdata )
{
	int ret;

	ret = CVecMesh::InitVecMesh( (CVecMesh*)srcdata );
	return ret;
}
int CSphere::DumpMem( HANDLE hfile, int tabnum, int dumpflag )
{
	/***
	char tempchar[256];
	char outline[1024];
	int ret, i;

	ret = CVecMesh::DumpMem( hfile, tabnum, dumpflag );
	if( ret ){
		DbgOut( "planes.cpp : CSphere: DumpMem : CVecMesh::DumpMem error !!!\n" );
		return 1;
	}

	SetTabChar( tabnum );

	ZeroMemory( outline, 1024 );
	sprintf( outline, "%s defmat\n", tabchar );

	for( i = 0; i < 4; i++ ){
		ZeroMemory( tempchar, 256 );
		sprintf( tempchar, "%s %f, %f, %f, %f\n",
			tabchar, defmat[i][0], defmat[i][1], defmat[i][2], defmat[i][3] );
		strcat( outline, tempchar );
	}
	ret = Write2File( hfile, outline );
	_ASSERT( !ret );
	***/
	return 0;
}

///////////////////////
// CDisk
///////////////////////
CDisk::CDisk()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( "charpos2.cpp : CDisk::CDisk : CreateObj error !!!\n" );
		isinit = 0;
	}	

}
CDisk::~CDisk()
{
	DestroyObjs();
}

void CDisk::InitParams()
{
	CPlanes::InitParams();
	defmat.Identity();	
	radfrom = 0.0f;
	radto = 0.0f;
	isfilled = 0;
}

int CDisk::SetMem( float* srcdata, __int64 setflag )
{

	if( setflag & DISK_FROM ){
		radfrom = *srcdata;
		chkflag |= DISK_FROM;
		return 0;
	}else if( setflag & DISK_TO ){
		radto = *srcdata;
		chkflag |= DISK_TO;
		return 0;
	}
	return 1;
}
int CDisk::SetMem( int* srcdata, __int64 setflag )
{
	int ret;

	if( setflag & DISK_ISFILLED ){
		isfilled = *srcdata;
		chkflag |= DISK_ISFILLED;
		return 0;
	}else{
		ret = CVecMesh::SetMem( srcdata, setflag );
		return ret;
	}
}

int CDisk::SetMem( CDisk* srcdata, __int64 setflag )
{
	int ret, i, j;

	if( setflag == DISK_ALL ){
		if( !meshinfo || !srcdata )
			return 1;
		ret = CVecMesh::SetMem( (CVecMesh*)srcdata, setflag );
		_ASSERT( !ret );

		radfrom = srcdata->radfrom;
		radto = srcdata->radto;
		isfilled = srcdata->isfilled;

		for( i = 0; i < 4; i++ )
			for( j = 0; j < 4; j++ )
				defmat[i][j] = (srcdata->defmat)[i][j];
		
		chkflag |= DISK_ALL;

		return ret;
	}
	return 1;

}
int CDisk::SetMem( CMatrix2* srcdefmat, __int64 setflag )
{
	int i, j;

	if( setflag == DISK_DEFMAT ){
		for( i = 0; i < 4; i++ )
			for( j = 0; j < 4; j++ )
				defmat[i][j] = (*srcdefmat)[i][j];
		return 0;
	}
	return 1;
}
int	CDisk::CopyData( CDisk* srcdata )
{
	int ret, i, j;

	ret = CVecMesh::CopyData( (CVecMesh*)srcdata );
	if( ret ){
		DbgOut( "charpos2.cpp : CDisk: CopyData : CVecMesh::CopyData error !!!\n" );
		return 1;
	}

	for( i = 0; i < 4; i++ )
		for( j = 0; j < 4; j++ )
			defmat[i][j] = srcdata->defmat[i][j];

	radfrom = srcdata->radfrom;
	radto = srcdata->radto;
	isfilled = srcdata->isfilled;

	chkflag = srcdata->chkflag;
	return 0;
}
int CDisk::InitVecMesh( CDisk* srcdata )
{
	int ret;

	ret = CVecMesh::InitVecMesh( (CVecMesh*)srcdata );
	return ret;
}

int CDisk::DumpMem( HANDLE hfile, int tabnum, int dumpflag )
{
	/***
	char tempchar[256];
	char outline[1024];
	int ret, i;

	ret = CVecMesh::DumpMem( hfile, tabnum, dumpflag );
	if( ret ){
		DbgOut( "planes.cpp : CDisk: DumpMem : CVecMesh::DumpMem error !!!\n" );
		return 1;
	}

	SetTabChar( tabnum );

	ZeroMemory( outline, 1024 );
	sprintf( outline, "%s defmat\n", tabchar );

	for( i = 0; i < 4; i++ ){
		ZeroMemory( tempchar, 256 );
		sprintf( tempchar, "%s %f, %f, %f, %f\n",
			tabchar, defmat[i][0], defmat[i][1], defmat[i][2], defmat[i][3] );
		strcat( outline, tempchar );
	}
	ret = Write2File( hfile, outline );
	_ASSERT( !ret );
	***/
	return 0;
}

//////////////////////////
// CPolygon
//////////////////////////
CPolygon::CPolygon()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( "charpos2.cpp : CPolygon::CPolygon : CreateObj error !!!\n" );
		isinit = 0;
	}	

}
CPolygon::~CPolygon()
{
	DestroyObjs();
}
int CPolygon::ConvDataNo( int* linenoptr, int* vecnoptr, int datano )
{
	int vnum, kindnum, vecno, linevno, tempno, i, lineno, pointnum, kindno;
	CMeshInfo* lineinfo = 0;

	if( !meshinfo ){
		return 1;
	}


	vnum = meshinfo->n;
	kindnum = meshinfo->m;
	
	vecno = datano;
	linevno = 0;
	for( i = 0; i < vnum; i++ ){
		lineinfo = (*this)( i, 0 )->meshinfo;
		_ASSERT( lineinfo );
		pointnum = lineinfo->m;
		tempno = vecno - pointnum * kindnum;
		if( tempno > 0 ){
			vecno = tempno;
		}else{
			linevno = i;
			break;
		}
	}

	kindno = vecno / pointnum;
	lineno = linevno * kindnum + kindno;
	vecno -= kindno * pointnum;

	*linenoptr = lineno;
	*vecnoptr = vecno;

	return 0;
}

int CPolygon::SetMem( CPolygon* srcdata, __int64 setflag )
{
	int ret;

	if( setflag == POLY_ALL ){
		if( !meshinfo || !srcdata )
			return 1;

		ret = CVecMesh::SetMem( (CVecMesh*)srcdata, setflag );
		_ASSERT( !ret );

		return ret;
	}
	return 1;

}

int	CPolygon::CopyData( CPolygon* srcdata )
{
	int ret;

	ret = CVecMesh::CopyData( (CVecMesh*)srcdata );
	if( ret ){
		DbgOut( "charpos2.cpp : CPolygon: CopyData : CVecMesh::CopyData error !!!\n" );
		return 1;
	}
	chkflag = srcdata->chkflag;
	return ret;
}
int CPolygon::InitVecMesh( CPolygon* srcdata )
{
	int ret;

	ret = CVecMesh::InitVecMesh( (CVecMesh*)srcdata );
	return ret;
}

int CPolygon::CheckMeshInfo( CMeshInfo* srcmeshinfo )
{
	//meshinfo->m = 2; meshinfo->n = îCà”;
	int mnum, nnum, totalnum;


	if( !srcmeshinfo )
		return 1;

	mnum = srcmeshinfo->m;
	nnum = srcmeshinfo->n;
	totalnum = srcmeshinfo->total;

	if( totalnum && ( totalnum == (mnum * nnum) ) )
		return 0;
	else
		return 1;
}

int CPolygon::CheckLineInfo( CMeshInfo* lineinfo, int infonum )
{
	//eachline->meshinfo->m = îCà”(â¬ïœ); eachline->meshinfo->n = 1;
	// ÇŸÇ∆ÇÒÇ«ÅAÇ±ÇÃÅ@m â¬ïœÇÃÇΩÇﬂÇæÇØÇ…çÏÇ¡ÇΩÉNÉâÉX

	int i, mnum, nnum, totalnum;

	DbgOut( "CPolygon : CheckLineInfo\n" );

	_ASSERT( lineinfo );

	for( i = 0; i < infonum; i++ ){
		_ASSERT( lineinfo + i );

		mnum = (lineinfo + i)->m;
		nnum = (lineinfo + i)->n;
		totalnum = (lineinfo + i)->total;

		if( (nnum == 1) && totalnum && (totalnum == mnum) )
			continue;
		else
			return 1;
	}
	return 0;
}

////////////////////////////////
// CExtruede
////////////////////////////////
CExtrude::CExtrude()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( "charpos2.cpp : CExtrude::CExtrude : CreateObj error !!!\n" );
		isinit = 0;
	}	
}
CExtrude::~CExtrude()
{
	DestroyObjs();
}

void CExtrude::InitParams()
{
	CVecMesh::InitParams();

	exvec.x = 0.0f;
	exvec.y = 0.0f;
	exvec.z = 0.0f;
}

int CExtrude::SetExVec( VEC3F srcvec )
{
	exvec.x = srcvec.x;
	exvec.y = srcvec.y;
	exvec.z = srcvec.z;

	return 0;
}

int CExtrude::CheckMeshInfo( CMeshInfo* srcmeshinfo )
{
	//meshinfo->m = 1; meshinfo->n = 1;
	int mnum, nnum, totalnum;

	if( !srcmeshinfo )
		return 1;

	mnum = srcmeshinfo->m;
	nnum = srcmeshinfo->n;
	totalnum = srcmeshinfo->total;

	if( (mnum == 1) && (nnum == 1) && (totalnum == 1) )
		return 0;
	else
		return 1;

}
int CExtrude::CheckLineInfo( CMeshInfo* lineinfo, int infonum )
{
	//eachline->meshinfo->m = îCà”; eachline->meshinfo->n = 1; infonum = 1

	if( (infonum == 1) && (lineinfo->m != 0) && (lineinfo->n == 1) )
		return 0;
	else
		return 1;

}

int	CExtrude::DumpMem( HANDLE hfile, int tabnum, int dumpflag )
{
	int ret;

	ret = CVecMesh::DumpMem( hfile, tabnum, dumpflag );
	_ASSERT( !ret );

	SetTabChar( tabnum + 1 );			
	ret = Write2File( hfile, "%s ExtrudeVec\n%s X: %f, Y: %f, Z: %f,\n", 
		tabchar, tabchar, exvec.x, exvec.y, exvec.z );
	_ASSERT( !ret );
	
	return ret;
}