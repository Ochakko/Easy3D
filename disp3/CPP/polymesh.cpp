#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <coef.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <polymesh.h>

#include <mqomaterial.h>

#include <bsphere.h>
#include <mothandler.h>
#include <d3ddisp.h>

#include <mothandler.h>
#include <shdhandler.h>
#include <shdelem.h>
#include <quaternion.h>

#include <Toon1Params.h>
#include <mqomaterial.h>

#ifndef LINECHARLENG
#define LINECHARLENG 2048
#endif
extern int g_index32bit;


// texture UV セット用
static int		next_texpcnt[2] = {1, 0};
static int		next_texvcnt[2] = {1, 0};

static float	texu4[4][2][2] = { { {0.0f, 0.5f}, {0.0f, 0.5f} }, { {0.5f, 1.0f}, {0.5f, 1.0f} }, 
								   { {0.0f, 0.5f}, {0.0f, 0.5f} }, { {0.5f, 1.0f}, {0.5f, 1.0f} } };// [texkind][texpcnt][texvcnt]

static float	texv4[4][2][2] = { { {0.0f, 0.0f}, {0.5f, 0.5f} }, { {0.0f, 0.0f}, {0.5f, 0.5f} }, 
								   { {0.5f, 0.5f}, {1.0f, 1.0f} }, { {0.5f, 0.5f}, {1.0f, 1.0f} }  };

	// 境界線で 他色が 出ないように 境界線の値を 避ける
static float	texu42[4][2][2] = { { {0.0f, 0.4f}, {0.0f, 0.4f} }, { {0.6f, 0.9f}, {0.6f, 0.9f} }, 
								   { {0.0f, 0.4f}, {0.0f, 0.4f} }, { {0.6f, 0.9f}, {0.6f, 0.9f} } };// [texkind][texpcnt][texvcnt]

static float	texv42[4][2][2] = { { {0.0f, 0.0f}, {0.4f, 0.4f} }, { {0.0f, 0.0f}, {0.4f, 0.4f} }, 
								   { {0.6f, 0.6f}, {0.9f, 0.9f} }, { {0.6f, 0.6f}, {0.9f, 0.9f} }  };


	// 境界線で 他色が 出ないように 境界線の値を 避ける
static float	texu43[4][2][2] = { { {0.1f, 0.4f}, {0.1f, 0.4f} }, { {0.6f, 0.9f}, {0.6f, 0.9f} }, 
								   { {0.1f, 0.4f}, {0.1f, 0.4f} }, { {0.6f, 0.9f}, {0.6f, 0.9f} } };// [texkind][texpcnt][texvcnt]

static float	texv43[4][2][2] = { { {0.15f, 0.15f}, {0.4f, 0.4f} }, { {0.15f, 0.15f}, {0.4f, 0.4f} }, 
								   { {0.6f, 0.6f}, {0.9f, 0.9f} }, { {0.6f, 0.6f}, {0.9f, 0.9f} }  };



static int compareTex( void *context, const void *elem1, const void *elem2);


////////////
CPolyMesh::CPolyMesh()
{
	InitParams();

}

CPolyMesh::~CPolyMesh()
{
	DestroyObjs();
}

void CPolyMesh::InitParams()
{
	CBaseDat::InitParams();
	
	meshinfo = 0;
	pointbuf = 0;
	indexbuf = 0;
	
	groundflag = 0;
	billboardflag = 0;
	uvbuf = 0;
	texkindbuf = 0;

// material
	D3DCOLORVALUE diff, amb, spec, emi;
	diff.r = 1.0f;
	diff.g = 1.0f;
	diff.b = 1.0f;
	diff.a = 1.0f;

	amb.r = 0.3f;
	amb.g = 0.3f;
	amb.b = 0.3f;
	amb.a = 1.0f;

	//spec.r = 0.1f;
	//spec.g = 0.1f;
	//spec.b = 0.1f;
	//spec.a = 1.0f;

	spec.r = 0.0f;
	spec.g = 0.0f;
	spec.b = 0.0f;
	spec.a = 1.0f;


	emi.r = 0.0f;
	emi.g = 0.0f;
	emi.b = 0.0f;
	emi.a = 1.0f;

	m_IE = 0;


	diffusebuf = 0;
	ambientbuf = 0;
	specularbuf = 0;

	powerbuf = 0;
	emissivebuf = 0;

	orgnobuf = 0;

	m_mikoapplychild = 0;
	m_mikotarget = 0;

	tempapplychild = 0;
	temptarget = 0;

	m_center.x = 0.0f;
	m_center.y = 0.0f;
	m_center.z = 0.0f;

	tempdiffuse = 0;
	tempambient = 0;
	tempspecular = 0;
	tempemissive = 0;
	temppower = 0;

	m_material = 0;
	m_materialnum = 0;
	m_attrib = 0;
	m_indexbuf2 = 0;
	m_materialblock = 0;
	//m_toon1tex = 0;
	m_toon1 = 0;
	m_toonface2oldface = 0;

	m_attrib0 = 0;

	m_sorttex = 0;
	m_optindexbuftex = 0;
	m_texblocknum = 0;
	m_texblock = 0;

	chkalpha.alphanum = 0;
	chkalpha.notalphanum = 0;

	m_dirtymat = 0;

	ZeroMemory( infcol, sizeof( E3DCOLOR3UC ) * CALCMODE_MAX );
	infcol[CALCMODE_ONESKIN0].r = 255;
	infcol[CALCMODE_ONESKIN0].g = 0;
	infcol[CALCMODE_ONESKIN0].b = 0;

	infcol[CALCMODE_ONESKIN1].r = 0;
	infcol[CALCMODE_ONESKIN1].g = 255;
	infcol[CALCMODE_ONESKIN1].b = 0;

	infcol[CALCMODE_NOSKIN0].r = 0;
	infcol[CALCMODE_NOSKIN0].g = 0;
	infcol[CALCMODE_NOSKIN0].b = 255;

	infcol[CALCMODE_DIRECT0].r = 255;
	infcol[CALCMODE_DIRECT0].g = 255;
	infcol[CALCMODE_DIRECT0].b = 0;	

	infcol[CALCMODE_SYM].r = 0;
	infcol[CALCMODE_SYM].g = 255;
	infcol[CALCMODE_SYM].b = 255;

	m_texorgflag = 1;
}

void CPolyMesh::DestroyObjs()
{
	CBaseDat::DestroyObjs();
	DestroyPolyMesh();	
	DestroyTempMaterial();
	DestroyToon1Buffer();
	DestroySortTexBuffer();

}
int CPolyMesh::DestroySortTexBuffer()
{
	if( m_sorttex ){
		free( m_sorttex );
		m_sorttex = 0;
	}

	if( m_optindexbuftex ){
		free( m_optindexbuftex );
		m_optindexbuftex = 0;
	}

	if( m_texblock ){
		free( m_texblock );
		m_texblock = 0;
	}

	if( m_dirtymat ){
		DIRTYMAT* deldm;
		int dmno;
		for( dmno = 0; dmno < m_texblocknum; dmno++ ){
			deldm = m_dirtymat + dmno;
			if( deldm ){
				if( deldm->dirtyflag ){
					free( deldm->dirtyflag );
				}
			}
		}
		free( m_dirtymat );
		m_dirtymat = 0;
	}


	m_texblocknum = 0;

	return 0;
}



int CPolyMesh::DestroyToon1Buffer()
{
	if( m_material ){
		D3DXMATERIAL* curmat;
		int matno;
		for( matno = 0; matno < m_materialnum; matno++ ){
			curmat = m_material + matno;

			if( curmat->pTextureFilename ){
				delete [] curmat->pTextureFilename;
				curmat->pTextureFilename = 0;
			}
		}

		delete [] m_material;
		m_material = 0;

	}

	if( m_toon1 ){
		delete [] m_toon1;
		m_toon1 = 0;
	}
	if( m_toonface2oldface ){
		free( m_toonface2oldface );
		m_toonface2oldface = 0;
	}

	m_materialnum = 0;

	if( m_attrib ){
		delete [] m_attrib;
		m_attrib = 0;
	}

	if( m_indexbuf2 ){
		free( m_indexbuf2 );
		m_indexbuf2 = 0;
	}

	if( m_materialblock ){
		free( m_materialblock );
		m_materialblock = 0;
	}

	return 0;
}

int CPolyMesh::DestroyTempMaterial()
{
	if( tempdiffuse ){
		free( tempdiffuse );
		tempdiffuse = 0;
	}
	if( tempambient ){
		free( tempambient );
		tempambient = 0;
	}
	if( tempspecular ){
		free( tempspecular );
		tempspecular = 0;
	}
	if( tempemissive ){
		free( tempemissive );
		tempemissive = 0;
	}
	if( temppower ){
		free( temppower );
		temppower = 0;
	}

	return 0;
}

void CPolyMesh::DestroyPolyMesh()
{
	if( meshinfo ){
		delete meshinfo;
		meshinfo = 0;
	}

	if( pointbuf ){
		free( pointbuf );
		pointbuf = 0;
	}

	if( indexbuf ){
		free( indexbuf );
		indexbuf = 0;
	}

	if( uvbuf ){
		free( uvbuf );
		uvbuf = 0;
	}
	if( texkindbuf ){
		free( texkindbuf );
		texkindbuf = 0;
	}

	if( m_IE ){
		delete [] m_IE;
		m_IE = 0;
	}

	if( diffusebuf ){
		free( diffusebuf );
		diffusebuf = 0;
	}

	if( ambientbuf ){
		free( ambientbuf );
		ambientbuf = 0;
	}

	if( specularbuf ){
		free( specularbuf );
		specularbuf = 0;
	}

	if( powerbuf ){
		free( powerbuf );
		powerbuf = 0;
	}

	if( emissivebuf ){
		free( emissivebuf );
		emissivebuf = 0;
	}

	if( orgnobuf ){
		free( orgnobuf );
		orgnobuf = 0;
	}

	if( m_attrib0 ){
		free( m_attrib0 );
		m_attrib0 = 0;
	}

}


int CPolyMesh::CreatePointBuf( CMeshInfo* srcmeshinfo, float srcalpha )
{
	int ret;

	DestroyPolyMesh();

	// meshinfo
	ret = srcmeshinfo->NewMeshInfo( &meshinfo );
	if( ret ){
		DbgOut( "CPolyMesh : CreatePointBuf ; srcmeshinfo->NewMeshinfo error !!!\n" );
		return 1;
	}

	_ASSERT( (meshinfo->type > SHDTYPENONE) && (meshinfo->type < SHDTYPEMAX) );

	ret = CBaseDat::SetType( meshinfo->type );
	if( ret ){
		DbgOut( "CPolyMesh : CreatePointBuf : base::SetType error !!!\n" );
		return 1;
	}

	int pointbufleng;
	pointbufleng = meshinfo->m;
	pointbuf = (VEC3F*)malloc( sizeof( VEC3F ) * pointbufleng );
	if( !pointbuf ){
		DbgOut( "CPolyMesh : CreatePointBuf : pointbuf alloc error !!!\n" );
		return 1;
	}
	ZeroMemory( pointbuf, sizeof( VEC3F ) * pointbufleng );


	orgnobuf = (int*)malloc( sizeof( int ) * pointbufleng );
	if( !orgnobuf ){
		DbgOut( "polymesh : CreatePointBuf : orgnobuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( orgnobuf, sizeof( int ) * pointbufleng );


////////////
	diffusebuf = (ARGBF*)malloc( sizeof( ARGBF ) * pointbufleng );
	if( !diffusebuf ){
		DbgOut( "polymesh : CreatePointBuf : diffusebuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ambientbuf = (ARGBF*)malloc( sizeof( ARGBF ) * pointbufleng );
	if( !ambientbuf ){
		DbgOut( "polymesh : CreatePointBuf : ambientbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	specularbuf = (ARGBF*)malloc( sizeof( ARGBF ) * pointbufleng );
	if( !specularbuf ){
		DbgOut( "polymesh : CreatePointBuf : specularbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	powerbuf = (float*)malloc( sizeof( float ) * pointbufleng );
	if( !powerbuf ){
		DbgOut( "pm : CreatePointBuf : powerbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( powerbuf, sizeof( float ) * pointbufleng );


	emissivebuf = (ARGBF*)malloc( sizeof( ARGBF ) * pointbufleng );
	if( !emissivebuf ){
		DbgOut( "pm : CreatePointBuf : emissivebuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( emissivebuf, sizeof( ARGBF ) * pointbufleng );


	CVec3f* diffuse = 0;
	CVec3f* specular = 0;
	CVec3f* ambient = 0;

	diffuse = meshinfo->GetMaterial( MAT_DIFFUSE );
	specular = meshinfo->GetMaterial( MAT_SPECULAR );
	ambient = meshinfo->GetMaterial( MAT_AMBIENT );
	if( !diffuse || !specular || !ambient ){
		DbgOut( "polymesh : CreatePointBuf : material error %x %x %x\n", diffuse, specular, ambient );
		_ASSERT( 0 );
		return 1;
	}
	
	ARGBF srcdiff;
	ARGBF srcamb;
	ARGBF srcspec;

	srcdiff.a = srcalpha;
	srcdiff.r = diffuse->x;
	srcdiff.g = diffuse->y;
	srcdiff.b = diffuse->z;

	srcamb.a = srcalpha;
	srcamb.r = ambient->x;
	srcamb.g = ambient->y;
	srcamb.b = ambient->z;

	srcspec.a = srcalpha;
	srcspec.r = specular->x;
	srcspec.g = specular->y;
	srcspec.b = specular->z;

	SetDiffuse( -1, 0, srcdiff, 0, 0 );
	SetAmbient( -1, 0, srcamb );
	SetSpecular( -1, 0, srcspec, 0, 0 );

	// ここではまだ　meshinfo->n == 0　！！！！！！！！


	return 0;
}
int CPolyMesh::CreateIndexBuf( int srcfacenum )
{
	if( indexbuf ){
		DbgOut( "CPolyMesh : CreateIndexBuf : indexbuf already exist error !!!\n" );
		return 1;
	}
	
	int maxfacenum;
	//maxfacenum = 65535 / 3;
	if( g_index32bit ){
		maxfacenum = 715827882;
	}else{
		maxfacenum = 65535 / 3;
	}
	if( srcfacenum >= maxfacenum ){
		DbgOut( "CPolyMesh : CreateIndexBuf : facenum range error !!!\n" );
		return 1;
	}

	meshinfo->n = srcfacenum;//!!!
	int ibleng;
	ibleng = srcfacenum * 3;

	indexbuf = (int*)malloc( sizeof( int ) * ibleng );
	ZeroMemory( indexbuf, sizeof( int ) * ibleng );


	if( m_attrib0 ){
		free( m_attrib0 );
		m_attrib0 = 0;
	}
	m_attrib0 = (int*)malloc( sizeof( int ) * meshinfo->n );
	if( !m_attrib0 ){
		DbgOut( "polymesh : CreatePointBuf : m_attrib0 alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_attrib0, sizeof( int ) * meshinfo->n );


	return 0;
}

int CPolyMesh::GetPointBuf( int srcpno, D3DXVECTOR3* dstvec )
{
	if( (srcpno < 0) || (srcpno >= meshinfo->m) ){
		DbgOut( "CPolyMesh : GetPointBuf : pointno out of range error !!!\n" );
		return 1;
	}

	VEC3F* srcpoint = pointbuf + srcpno;
	
	dstvec->x = srcpoint->x;
	dstvec->y = srcpoint->y;
	dstvec->z = srcpoint->z;

	return 0;

}

int CPolyMesh::SetPointBuf( int srcpno, float srcx, float srcy, float srcz, int orgno )
{
	if( (srcpno < 0) || (srcpno >= meshinfo->m) ){
		DbgOut( "CPolyMesh : SetPointBuf : pointno out of range error !!!\n" );
		return 1;
	}

	VEC3F* dstpoint = pointbuf + srcpno;
	dstpoint->x = srcx;
	dstpoint->y = srcy;
	dstpoint->z = srcz;

	if( !orgnobuf ){
		DbgOut( "polymesh : SetPointBuf : orgnobuf not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( orgno != -999 ){// shandler :: SetVertPos から呼ばれる場合は、除外する！！！
		*( orgnobuf + srcpno ) = orgno;
	}

	return 0;
}


int CPolyMesh::SetPointBuf( int srcpnum, VEC3F* srcvec )
{
	if( (srcpnum < 0) || (srcpnum > meshinfo->m) ){
		DbgOut( "CPolyMesh : SetPointBuf : srcpnum out of range error !!!\n" );
		return 1;
	}
	
	MoveMemory( (void*)pointbuf, (void*)srcvec, sizeof( VEC3F ) * srcpnum );

	return 0;
}
int CPolyMesh::InvIndexBuf( int srcfaceno )
{
	int facemax;
	facemax = meshinfo->n;
	if( srcfaceno >= facemax ){
		DbgOut( "CPolyMesh : InvIndexBuf : faceno range error !!!\n" );
		return 1;
	}

	int setno;
	setno = srcfaceno * 3;

	int copybuf[3];
	copybuf[0] = *(indexbuf + setno);
	copybuf[1] = *(indexbuf + setno + 1);
	copybuf[2] = *(indexbuf + setno + 2);

	*(indexbuf + setno) = copybuf[2];
	*(indexbuf + setno + 1) = copybuf[1];
	*(indexbuf + setno + 2) = copybuf[0];

	return 0;

}

int CPolyMesh::SetIndexBuf( int srcfaceno, int* srclist )
{
	int facemax;
	facemax = meshinfo->n;
	if( srcfaceno >= facemax ){
		DbgOut( "CPolyMesh : SetIndexBuf : faceno range error !!!\n" );
		return 1;
	}

	int setno;
	setno = srcfaceno * 3;

	*(indexbuf + setno) = *srclist;
	*(indexbuf + setno + 1) = *(srclist + 1);
	*(indexbuf + setno + 2) = *(srclist + 2);

	return 0;
}

int CPolyMesh::SetIndexBuf( int srcdatanum, int* srclist, int srcflag )
{
	int dataleng;
	dataleng = meshinfo->n * 3;
	if( (srcdatanum < 0) || (srcdatanum > dataleng) ){
		DbgOut( "CPolyMesh : SetIndexBuf : srcdatanum range error !!!\n" );
		return 1;
	}

	MoveMemory( (void*)indexbuf, (void*)srclist, sizeof( int ) * srcdatanum );
	return 0;
}

int CPolyMesh::SetInvisibleWallIndex( int pmfnum )
{
	if( pmfnum != meshinfo->n ){
		DbgOut( "polymesh : SetInvisibleWallIndex : pmfnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int offset = 0;
	int faceno = 0;
	int wallno;
	for( wallno = 0; wallno < pmfnum / 2; wallno++ ){
		//*( indexbuf + faceno * 3 ) = offset;
		//*( indexbuf + faceno * 3 + 1 ) = offset + 1;
		//*( indexbuf + faceno * 3 + 2 ) = offset + 2;
		*( indexbuf + faceno * 3 ) = offset;
		*( indexbuf + faceno * 3 + 1 ) = offset + 2;
		*( indexbuf + faceno * 3 + 2 ) = offset + 1;


		faceno++;
		
		//*( indexbuf + faceno * 3 ) = offset + 2;
		//*( indexbuf + faceno * 3 + 1 ) = offset + 1;
		//*( indexbuf + faceno * 3 + 2 ) = offset + 3;
		*( indexbuf + faceno * 3 ) = offset + 2;
		*( indexbuf + faceno * 3 + 1 ) = offset + 3;
		*( indexbuf + faceno * 3 + 2 ) = offset + 1;


		faceno++;
		offset += 4;
	}

	if( faceno != pmfnum ){
		DbgOut( "polymesh : SetInvisibleWallIndex : faceno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( offset != meshinfo->m ){
		DbgOut( "polymesh : SetInvisibleWallIndex : offset error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CPolyMesh::SetDefaultIndex( int srcxnum, int srcznum )
{
	//四角の格子状のデータと仮定して、indexをセットする。
	int facenum;
	facenum = (srcxnum - 1) * (srcznum - 1) * 2;
	if( facenum != meshinfo->n ){
		DbgOut( "polymesh : SetDefaultIndex : facenum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno = 0;
	int xno, zno;
	for( zno = 0; zno < srcznum - 1; zno++ ){
		for( xno = 0; xno < srcxnum - 1; xno++ ){
			*(indexbuf + faceno * 3) = zno * srcxnum + xno;
			*(indexbuf + faceno * 3 + 1) = (zno + 1) * srcxnum + xno;
			*(indexbuf + faceno * 3 + 2) = (zno + 1) * srcxnum + xno + 1;

			faceno++;

			*(indexbuf + faceno * 3) = zno * srcxnum + xno;
			*(indexbuf + faceno * 3 + 1) = (zno + 1) * srcxnum + xno + 1;
			*(indexbuf + faceno * 3 + 2) = zno * srcxnum + xno + 1;

			faceno++;
		}
	}


	if( faceno != facenum ){
		DbgOut( "polymesh : SetDefaultIndex : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}


int CPolyMesh::SetGroundFlag( int srcflag )
{
	groundflag = srcflag;
	return 0;
}

int CPolyMesh::CreateTextureBuffer()
{
	int vertnum = meshinfo->m;

	if( uvbuf || texkindbuf ){
		DbgOut( "polymesh : CreateTextureBuffer : buffer already exist warning return !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!
	}

	uvbuf = (COORDINATE*)malloc( sizeof( COORDINATE ) * vertnum );
	if( !uvbuf ){
		DbgOut( "polymesh : CreateTextureBuffer : uvbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( uvbuf, sizeof( COORDINATE ) * vertnum );


	texkindbuf = (int*)malloc( sizeof( int ) * vertnum );
	if( !texkindbuf ){
		DbgOut( "polymesh : CreateTextureBuffer : texkindbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( texkindbuf, sizeof( int ) * vertnum );

	return 0;
}

int CPolyMesh::SetTextureKind( int srcpno, int srckind )
{
	if( !texkindbuf ){
		DbgOut( "polymesh : SetTextureKind : texkindbuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (srcpno < 0) || (srcpno >= meshinfo->m) ){
		DbgOut( "polymesh : SetTextureKind : srcpno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*(texkindbuf + srcpno) = srckind;


	return 0;
}
int CPolyMesh::CopyUVFromD3DDisp( CD3DDisp* d3ddispptr )
{
	if( !d3ddispptr ){
		DbgOut( "polymesh : CopyUVFromD3DDisp : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int vnum;
	vnum = (int)d3ddispptr->m_numTLV;

	if( vnum != meshinfo->m ){
		DbgOut( "polymesh : CopyUVFromD3DDisp : vnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int pno;
	SKINVERTEX* curskinv = d3ddispptr->m_skinv;
	COORDINATE* dstuv = uvbuf;
	for( pno = 0; pno < vnum; pno++ ){
		dstuv->u = curskinv->tex1[0];
		dstuv->v = curskinv->tex1[1];
		curskinv++;
		dstuv++;
	}

	return 0;
}

int CPolyMesh::SetUVTile( int texrule, int unum, int vnum, int texno )
{
	if( !uvbuf ){
		DbgOut( "polymesh : SetUVTile : uvbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////
	int texnum, settexno;
	texnum = unum * vnum;

	if( texno >= 0 ){
		settexno = texno % texnum;
	}else{
		int divnum;
		divnum = -texno / texnum;
		int temptexno;
		temptexno = texno + texnum * ( divnum + 1 );
		_ASSERT( temptexno >= 0 );

		settexno = temptexno % texnum;
	}


	int uno, vno;
	vno = settexno / unum;
	uno = settexno - unum * vno;

	float stepu, stepv;
	stepu = 1.0f / unum;
	stepv = 1.0f / vnum;

	float startu, endu;
	float startv, endv;
	startu = uno * stepu;
	endu = startu + stepu;

	startv = vno * stepv;
	endv = startv + stepv;


///////
	int ret;

	switch( texrule ){
	case TEXRULE_X:
		ret = SetUV_X( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh : SetUVTile : SetUV_X error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case TEXRULE_Y:
		ret = SetUV_Y( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh : SetUVTile : SetUV_Y error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case TEXRULE_Z:
		ret = SetUV_Z( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh : SetUVTile : SetUV_Z error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case TEXRULE_CYLINDER:
		ret = SetUV_Cylinder( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh : SetUVTile : SetUV_CYLINDER error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case TEXRULE_SPHERE:
		ret = SetUV_Sphere( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh : SetUVTile : SetUV_SPHERE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		DbgOut( "polymesh : SetUVTile : unknown texrule : Set TEXRULE_Z warning !!!\n" );
		ret = SetUV_Z( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh : SetUVTile : SetUV_Z error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	}


	return 0;
}

int CPolyMesh::SetUV_X( float startu, float endu, float startv, float endv )
{

	int pno;
	VEC3F* curtlv = pointbuf;
	float minz = 1e6;
	float maxz = -1e6;
	float miny = 1e6;
	float maxy = -1e6;
	for( pno = 0; pno < meshinfo->m; pno++ )
	{
		if( curtlv->z > maxz )
			maxz = curtlv->z;

		if( curtlv->z < minz )
			minz = curtlv->z;

		if( curtlv->y > maxy )
			maxy = curtlv->y;

		if( curtlv->y < miny )
			miny = curtlv->y;

		curtlv++;
	}

	float ystep, zstep;
	if( maxy != miny )
		ystep = (endv - startv) / ( maxy - miny );
	else
		ystep = 0.0f;

	if( maxz != minz )
		zstep = (endu - startu) / ( maxz - minz );
	else
		zstep = 0.0f;

	COORDINATE* curuv = uvbuf;
	curtlv = pointbuf;
	for( pno = 0; pno < meshinfo->m; pno++ ){
		curuv->u = startu + (curtlv->z - minz) * zstep;
		curuv->v = endv - (curtlv->y - miny) * ystep;
		curtlv++;
		curuv++;
	}

	return 0;
}
int CPolyMesh::SetUV_Y( float startu, float endu, float startv, float endv )
{
	// U : X, V : Z

	int pno;
	VEC3F* curtlv = pointbuf;
	float minz = 1e6;
	float maxz = -1e6;
	float minx = 1e6;
	float maxx = -1e6;
	for( pno = 0; pno < meshinfo->m; pno++ )
	{
		if( curtlv->z > maxz )
			maxz = curtlv->z;

		if( curtlv->z < minz )
			minz = curtlv->z;

		if( curtlv->x > maxx )
			maxx = curtlv->x;

		if( curtlv->x < minx )
			minx = curtlv->x;

		curtlv++;
	}

	float zstep, xstep;
	if( maxz != minz )
		zstep = (endv - startv) / ( maxz - minz );
	else
		zstep = 0.0f;

	if( maxx != minx )
		xstep = (endu - startu) / ( maxx - minx );
	else
		xstep = 0.0f;

	COORDINATE* curuv = uvbuf;
	curtlv = pointbuf;
	for( pno = 0; pno < meshinfo->m; pno++ ){
		curuv->u = startu + (curtlv->x - minx) * xstep;
		curuv->v = endv - (curtlv->z - minz) * zstep;
		curtlv++;
		curuv++;
	}

	return 0;
}
int CPolyMesh::SetUV_Z( float startu, float endu, float startv, float endv )
{
	// U : X, V : Y
	int pno;
	VEC3F* curtlv = pointbuf;
	float minx = 1e6;
	float maxx = -1e6;
	float miny = 1e6;
	float maxy = -1e6;
	for( pno = 0; pno < meshinfo->m; pno++ )
	{
		if( curtlv->x > maxx )
			maxx = curtlv->x;

		if( curtlv->x < minx )
			minx = curtlv->x;

		if( curtlv->y > maxy )
			maxy = curtlv->y;

		if( curtlv->y < miny )
			miny = curtlv->y;

		curtlv++;
	}

	float xstep, ystep;
	if( maxy != miny )
		ystep = (endv - startv) / ( maxy - miny );
	else
		ystep = 0.0f;

	if( maxx != minx )
		xstep = (endu - startu) / ( maxx - minx );
	else
		xstep = 0.0f;

	COORDINATE* curuv = uvbuf;
	curtlv = pointbuf;
	for( pno = 0; pno < meshinfo->m; pno++ ){
		curuv->u = startu + (curtlv->x - minx) * xstep;
		curuv->v = endv - (curtlv->y - miny) * ystep;
		curtlv++;
		curuv++;
	}

	return 0;
}
int CPolyMesh::SetUV_Cylinder( float startu, float endu, float startv, float endv )
{
	int pno;
	D3DXVECTOR3 center;
	center.x = 0.0f; center.y = 0.0f; center.z = 0.0f;
	VEC3F* curtlv = pointbuf;
	float miny = 1e6;
	float maxy = -1e6;
	for( pno = 0; pno < meshinfo->m; pno++ )
	{
		center.x += curtlv->x;
		center.y += curtlv->y;
		center.z += curtlv->z;

		if( curtlv->y > maxy )
			maxy = curtlv->y;

		if( curtlv->y < miny )
			miny = curtlv->y;

		curtlv++;
	}
	center.x /= (float)meshinfo->m;
	center.y /= (float)meshinfo->m;
	center.z /= (float)meshinfo->m;
	float ystep;
	if( maxy != miny )
		ystep = (endv - startv) / ( maxy - miny );
	else
		ystep = 0.0f;

	D3DXVECTOR2 base;
	base.x = 0.0f; base.y = -1.0f;// y にはz座標！！！

	COORDINATE* curuv = uvbuf;
	curtlv = pointbuf;
	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;
	float dot, kaku, flccw;
	float xstep = (endu - startu) / ( 2.0f * PI );

	for( pno = 0; pno < meshinfo->m; pno++ ){
		curuv->v = endv - (curtlv->y - miny) * ystep;
			
		vec.x = curtlv->x - center.x;
		vec.y = curtlv->z - center.z;
		D3DXVec2Normalize( &nvec, &vec );

		dot = D3DXVec2Dot( &nvec, &base );
		if( dot > 1.0f )
			dot = 1.0f;
		if( dot < -1.0f )
			dot = -1.0f;
		kaku = (float)acos( dot );
		flccw = D3DXVec2CCW( &nvec, &base );
		if( flccw > 0.0f ){
			kaku = 2.0f * PI - kaku;
		}

		curuv->u = startu + kaku * xstep;

		curtlv++;
		curuv++;
	}

	return 0;
}
int CPolyMesh::SetUV_Sphere( float startu, float endu, float startv, float endv )
{
	int pno;
	D3DXVECTOR3 center;
	center.x = 0.0f; center.y = 0.0f; center.z = 0.0f;
	VEC3F* curtlv = pointbuf;
	for( pno = 0; pno < meshinfo->m; pno++ )
	{
		center.x += curtlv->x;
		center.y += curtlv->y;
		center.z += curtlv->z;

		curtlv++;
	}
	center.x /= (float)meshinfo->m;
	center.y /= (float)meshinfo->m;
	center.z /= (float)meshinfo->m;

	D3DXVECTOR2 base_xz;
	base_xz.x = 0.0f; base_xz.y = -1.0f;// y にはz座標！！！
	D3DXVECTOR2 base_xy;
	base_xy.x = 0.0f; base_xy.y = 1.0f;

	COORDINATE* curuv = uvbuf;
	curtlv = pointbuf;
	D3DXVECTOR2 vec_xz;
	D3DXVECTOR2 nvec_xz;
	D3DXVECTOR2 vec_xy;
	D3DXVECTOR2 nvec_xy;
	float dot_xz, kaku_xz, flccw_xz;
	float dot_xy, kaku_xy;//, flccw_xy;
	float stepxz = (endu - startu) / ( 2.0f * PI );
	float stepxy = (endv - startv) / (float)PI;

	for( pno = 0; pno < meshinfo->m; pno++ ){
			
		vec_xz.x = curtlv->x - center.x;
		vec_xz.y = curtlv->z - center.z;
		D3DXVec2Normalize( &nvec_xz, &vec_xz );

		dot_xz = D3DXVec2Dot( &nvec_xz, &base_xz );
		if( dot_xz > 1.0f )
			dot_xz = 1.0f;
		if( dot_xz < -1.0f )
			dot_xz = -1.0f;

		kaku_xz = (float)acos( dot_xz );
		flccw_xz = D3DXVec2CCW( &nvec_xz, &base_xz );
		if( flccw_xz > 0.0f ){
			kaku_xz = 2.0f * PI - kaku_xz;
		}

		curuv->u = startu + kaku_xz * stepxz;
		///////////
		vec_xy.x = curtlv->x - center.x;
		vec_xy.y = curtlv->y - center.y;
		D3DXVec2Normalize( &nvec_xy, &vec_xy );

		dot_xy = D3DXVec2Dot( &nvec_xy, &base_xy );
		if( dot_xy > 1.0f )
			dot_xy = 1.0f;
		if( dot_xy < -1.0f )
			dot_xy = -1.0f;
		kaku_xy = (float)acos( dot_xy );
		//flccw_xy = D3DXVec2CCW( &nvec_xy, &base_xy );
		//if( flccw_xy > 0.0f ){
		//	kaku_xy = 2.0f * PI - kaku_xy;
		//}

		curuv->v = startv + kaku_xy * stepxy;

		curtlv++;
		curuv++;
	}

	return 0;
}


int CPolyMesh::AdjustUVBuf()
{
	int pno;
	COORDINATE* curuv;

	float minu, maxu, minv, maxv;
	minu = 1e6;
	maxu = -1e6;
	minv = 1e6;
	maxv = -1e6;

	for( pno = 0; pno < meshinfo->m; pno++ ){
		curuv = uvbuf + pno;
		
		if( curuv->u < minu )
			minu = curuv->u;
		if( curuv->u > maxu )
			maxu = curuv->u;

		if( curuv->v < minv )
			minv = curuv->v;
		if( curuv->v > maxv )
			maxv = curuv->v;
	}

	if( (minu < 0.0f) || (maxu > 1.0f)){
		float urange;
		urange = maxu - minu;
		for( pno = 0; pno < meshinfo->m; pno++ ){
			curuv = uvbuf + pno;
			
			curuv->u = (curuv->u - minu) / urange;
		}
	}
	if( (minv < 0.0f) || (maxv > 1.0f)){
		float vrange;
		vrange = maxv - minv;

		for( pno = 0; pno < meshinfo->m; pno++ ){
			curuv = uvbuf + pno;
			
			curuv->v = (curuv->v - minv) / vrange;

		}
	}

	return 0;
}


int CPolyMesh::SetUV( int vertno, float fu, float fv, int setflag, int clampflag )
{
	if( !uvbuf ){
		DbgOut( "polymesh : SetUV : uvbuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= meshinfo->m) ){
		DbgOut( "polymesh : SetUV : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	COORDINATE* curuv;
	curuv = uvbuf + vertno;

	if( setflag == 0 ){
		curuv->u = fu;
		curuv->v = fv;
	}else{
		curuv->u += fu;
		curuv->v += fv;
	}

	if( clampflag == 1 ){
		curuv->u = max( 0.0f, curuv->u );
		curuv->u = min( 1.0f, curuv->u );

		curuv->v = max( 0.0f, curuv->v );
		curuv->v = min( 1.0f, curuv->v );
	}


	return 0;
}


int CPolyMesh::SetTex4UV( int srcxnum, int srcznum )
{
	if( !uvbuf ){
		DbgOut( "polymesh : SetTex4UV : uvbuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( meshinfo->m != (srcxnum * srcznum) ){
		DbgOut( "polymesh : SetTex4UV : vertnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texpcnt = 0;
	int texvcnt = 0;
	int xno, zno;
	int vertno = 0;
	int curkind;
	COORDINATE* dstuv;
	for( zno = 0; zno < srcznum; zno++ ){
		
		for( xno = 0; xno < srcxnum; xno++ ){
			curkind = *(texkindbuf + vertno);
			if( (curkind < 0) || (curkind >= 4) ){
				DbgOut( "polymesh : SetTex4UV : curkind error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			dstuv = uvbuf + vertno;
			dstuv->u = texu43[curkind][texpcnt][texvcnt];
			dstuv->v = texv43[curkind][texpcnt][texvcnt];

			//dstuv->u = texu42[curkind][texpcnt][texvcnt];
			//dstuv->v = texv42[curkind][texpcnt][texvcnt];

			//dstuv->u = texu4[curkind][texpcnt][texvcnt];
			//dstuv->v = texv4[curkind][texpcnt][texvcnt];

			texvcnt = next_texvcnt[texvcnt];
			vertno++;
		}
		texpcnt = next_texpcnt[texpcnt];
	}



	return 0;
}

int CPolyMesh::SetBillboardUV( int unum, int vnum, int texno, int revuflag )
{
	if( (meshinfo->m != 4) || (meshinfo->n != 2) || (billboardflag != 1) ){
		DbgOut( "polymesh : SetBillboardUV : this is not billboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !uvbuf ){
		DbgOut( "polymesh : SetBillboardUV : uvbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////
	int texnum, settexno;
	texnum = unum * vnum;

	if( texno >= 0 ){
		settexno = texno % texnum;
	}else{
		int divnum;
		divnum = -texno / texnum;
		int temptexno;
		temptexno = texno + texnum * ( divnum + 1 );
		_ASSERT( temptexno >= 0 );

		settexno = temptexno % texnum;
	}


	int uno, vno;
	vno = settexno / unum;
	uno = settexno - unum * vno;

	float stepu, stepv;
	stepu = 1.0f / unum;
	stepv = 1.0f / vnum;

	float startu, endu;
	float startv, endv;
	startu = uno * stepu;
	endu = startu + stepu;

	startv = vno * stepv;
	endv = startv + stepv;


///////

	if( revuflag == 0 ){
		uvbuf->u = startu;
		uvbuf->v = endv;

		(uvbuf + 1)->u = startu;
		(uvbuf + 1)->v = startv;

		(uvbuf + 2)->u = endu;
		(uvbuf + 2)->v = endv;

		(uvbuf + 3)->u = endu;
		(uvbuf + 3)->v = startv;
	}else{
		//uを反転
		uvbuf->u = endu;
		uvbuf->v = endv;

		(uvbuf + 1)->u = endu;
		(uvbuf + 1)->v = startv;

		(uvbuf + 2)->u = startu;
		(uvbuf + 2)->v = endv;

		(uvbuf + 3)->u = startu;
		(uvbuf + 3)->v = startv;
	}

	return 0;
}


int CPolyMesh::SetBillboardPoints( float width, float height, int orgflag )
{
	if( (meshinfo->m != 4) || (meshinfo->n != 2) || (billboardflag != 1) ){
		DbgOut( "polymesh : SetBillboardPoints : this is not billboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	int ret;

	
	if( orgflag == 0 ){
		float w2;

		w2 = width * 0.5f;
	//set point buf
		ret = SetPointBuf( 0, -w2, 0.0f, 0.0f, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = SetPointBuf( 1, -w2, height, 0.0f, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = SetPointBuf( 2, w2, 0.0f, 0.0f, 2 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = SetPointBuf( 3, w2, height, 0.0f, 3 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		float w2, h2;

		w2 = width * 0.5f;
		h2 = height * 0.5f;

		ret = SetPointBuf( 0, -w2, -h2, 0.0f, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = SetPointBuf( 1, -w2, h2, 0.0f, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = SetPointBuf( 2, w2, -h2, 0.0f, 2 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = SetPointBuf( 3, w2, h2, 0.0f, 3 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
//set index buf
	
	int srcindex[3];

	srcindex[0] = 0;
	srcindex[1] = 1;
	srcindex[2] = 2;
	ret = SetIndexBuf( 0, srcindex );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	srcindex[0] = 2;
	srcindex[1] = 1;
	srcindex[2] = 3;
	ret = SetIndexBuf( 1, srcindex );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//////////
	ret = CalcAnchorCenter();
	if( ret ){
		DbgOut( "pm : SetBillboardPoints : CalcAnchorCenter error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CPolyMesh::MakeFaceno2Materialno( int srcshader, int sigmagicno, int** arrayptr, CMQOMaterial* newmathead, CMQOMaterial* shmathead, int srcseri )
{
	int faceno;

	int* faceno2matno;
	faceno2matno = (int*)malloc( sizeof( int ) * meshinfo->n );
	if( !faceno2matno ){
		DbgOut( "polymesh : MakeFaceno2Materialno : faceno2matno alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		*( faceno2matno + faceno ) = -1;
	}

	int topindex;
	ARGBF* curdif;
	ARGBF* curamb;
	ARGBF* curspc;
	ARGBF* curemi;
	float curpow;
	CMQOMaterial curmaterial;
	//ARGBF* curorgdif;

	float curalpha;
	char* curtexname = 0;
	int curattrib0;
	CMQOMaterial* curshmat;

	for( faceno = 0; faceno < meshinfo->n; faceno++ ){

		curattrib0 = *( m_attrib0 + faceno );
		curshmat = GetMaterialFromNo( shmathead, curattrib0 );
		if( !curshmat ){
			DbgOut( "pm2 : MakeFaceno2Materialno : curshmat NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curalpha = curshmat->col.a;
		curtexname = curshmat->tex;

		topindex = *(indexbuf + faceno * 3);
///////

		sprintf_s( curmaterial.name, 256, "mat_polymesh_%d_%d", srcseri, faceno );

		curdif = diffusebuf + topindex;
		curamb = ambientbuf + topindex;
		curemi = emissivebuf + topindex;
		curpow = *( powerbuf + topindex );
		curspc = specularbuf + topindex;

		//int colr, colg, colb, specr;
		curmaterial.shader = srcshader;

		curmaterial.col.a = curalpha;

		curmaterial.col.r = curdif->r;
		curmaterial.col.g = curdif->g;
		curmaterial.col.b = curdif->b;
		curmaterial.dif = 1.0f;
			
		if( curmaterial.col.r != 0.0f ){
			curmaterial.spc = curspc->r / curmaterial.col.r;
			curmaterial.amb = curamb->r / curmaterial.col.r;
			curmaterial.emi = curemi->r / curmaterial.col.r;
		}else{
			curmaterial.spc = curspc->r;
			curmaterial.amb = curamb->r;
			curmaterial.emi = curemi->r;
		}

		curmaterial.power = curpow;


		if( curtexname && *curtexname ){
			int nameleng;
			nameleng = (int)strlen( curtexname );
			if( nameleng >= 255 ){
				DbgOut( "polymesh : MakeFaceno2Materialno : nameleng error !!!\n" );
				_ASSERT( 0 );
				free( faceno2matno );
				return 1;
			}
			strcpy_s( curmaterial.tex, 256, curtexname );
		}else{
			curmaterial.tex[0] = 0;
		}
		
		curmaterial.alpha[0] = 0;
		curmaterial.bump[0] = 0;

//////
		CMQOMaterial* srcmat = newmathead;
		CMQOMaterial* findmat = 0;
		int issame;
		while( srcmat ){			
			int cmplevel = 1;
			issame = srcmat->IsSame( &curmaterial, 0, cmplevel );
			if( issame ){
				findmat = srcmat;
				break;
			}

			srcmat = srcmat->next;
		}
//////

		if( findmat ){
			//すでに、同じmaterialがあった場合
			*( faceno2matno + faceno ) = findmat->materialno;
		}else{
			//同じマテリアルが無かった場合。
			CMQOMaterial* befmat = 0;
			CMQOMaterial* lastmat = newmathead;
			while( lastmat ){
				befmat = lastmat;
				lastmat = lastmat->next;
			}

			if( befmat == 0 ){
				DbgOut( "polymesh : MakeFaceno2Materialno : befmat error !!!\n" );
				_ASSERT( 0 );
				free( faceno2matno );
				return 1;
			}


			CMQOMaterial* newmat;
			newmat = new CMQOMaterial();
			if( !newmat ){
				DbgOut( "polymesh : MakeFaceno2Materialno : newmat alloc error !!!\n" );
				_ASSERT( 0 );
				free( faceno2matno );
				return 1;
			}

			int newmatno;
			newmatno = befmat->materialno + 1;

			newmat->shader = srcshader;

			*newmat = curmaterial;
			newmat->materialno = newmatno;
			befmat->next = newmat;
			newmat->next = 0;

			*( faceno2matno + faceno ) = newmatno;
		}
	}

	*arrayptr = faceno2matno;

	return 0;
}



int CPolyMesh::SetCurrentPose2OrgData( CMotHandler* srcmh, CShdElem* selem, CQuaternion* multq )
{
	int ret;

	if( !pointbuf ){
		DbgOut( "pm : SetCurrentPose2OrgData : pointbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3* wvec;
	wvec = new D3DXVECTOR3[ meshinfo->m ];
	if( !wvec ){
		DbgOut( "pm : WriteMQOObjectOnFrame : wvec alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXMATRIX iniwmat;
	D3DXMatrixIdentity( &iniwmat );
	ret = selem->TransformOnlyWorld3( srcmh, iniwmat, wvec );
	if( ret ){
		DbgOut( "pm : WriteMQOObjectOnFrame : selem TransformOnlyWorld3 error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}

	int vno;
	VEC3F* curdst;
	D3DXVECTOR3* cursrc;
	for( vno = 0; vno < meshinfo->m; vno++ ){
		curdst = pointbuf + vno;
		cursrc = wvec + vno;

		if( multq ){
			multq->Rotate( cursrc, *cursrc );
		}


		curdst->x = cursrc->x;
		curdst->y = cursrc->y;
		curdst->z = cursrc->z;
	}

	delete [] wvec;


	return 0;
}

int CPolyMesh::ConvSymmXShape()
{
	//int ret;

	if( !pointbuf ){
		DbgOut( "pm : ConvSymmXShape : pointbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int vno;
	VEC3F* curdst;
	for( vno = 0; vno < meshinfo->m; vno++ ){
		curdst = pointbuf + vno;

		curdst->x = -curdst->x;
		curdst->y = curdst->y;
		curdst->z = curdst->z;
	}

	return 0;
}

int CPolyMesh::WriteMQOObjectOnFrameMorph( HANDLE hfile, int* matnoindex, 
	int motid, int frameno, int serialno, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult, int writeRevFlag )
{
	int ret;
	char tempchar[LINECHARLENG];

	
	D3DXVECTOR3* wvec;
	wvec = new D3DXVECTOR3[ meshinfo->m ];
	if( !wvec ){
		DbgOut( "pm : WriteMQOObjectOnFrameMorph : wvec alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*lpsh)( serialno );
	if( !selem ){
		DbgOut( "pm : WriteMQOObjectOnFrameMorph : selem NULL error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}

	ret = lpmh->SetCurrentMotion( lpsh, motid, frameno );
	if( ret ){
		DbgOut( "pm : WriteMQOObjectOnFrameMorph : mh SetCurrentMotion error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}


	ret = selem->TransformDMorphPM();
	if( ret ){
		DbgOut( "pm : WriteMQOObjectOnFrameMorph : se TransformDMorphPM error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXMATRIX iniwmat;
	D3DXMatrixIdentity( &iniwmat );


	ret = selem->d3ddisp->TransformOnlyWorld3( lpmh, iniwmat, wvec, selem->d3ddisp->m_IEptr );
	if( ret ){
		DbgOut( "pm : WriteMQOObjectOnFrameMorph : d3ddisp TransformOnlyWorld3 error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}


// vertex
	//	vertex 9 {
	//		15.0621 87.7051 -57.7583
	//		...
	//	}	
	sprintf_s( tempchar, LINECHARLENG, "\tvertex %d {", meshinfo->m );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}

	int vertno;
	D3DXVECTOR3* curpoint;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		curpoint = wvec + vertno;

		//!!!!!!!
		// Z の符号を反転する。
		//!!!!!!!

		//sprintf( tempchar, "\t\t%.4f %.4f %.4f", curpoint->x, curpoint->y, curpoint->z );
		sprintf_s( tempchar, LINECHARLENG, "\t\t%.4f %.4f %.4f", curpoint->x * srcmult, curpoint->y * srcmult, -curpoint->z * srcmult );
		ret = WriteChar( hfile, tempchar, 1 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			delete [] wvec;
			return 1;
		}

	}


	delete [] wvec;


	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// face
	//	face 511 {
	//		4 V(487 19 4 482) M(0) UV(0.73213 0.24561 0.72100 0.24600 0.72100 0.26500 0.73217 0.26610)
	//		...	
	//	}
	
	int outputfacenum;

	if( selem->clockwise == 3 ){
		outputfacenum = meshinfo->n * 2;
	}else{
		outputfacenum = meshinfo->n;
	}


	sprintf_s( tempchar, LINECHARLENG, "\tface %d {", outputfacenum );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno;
	int i1, i2, i3;
	int curmatno;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){

		if( selem->clockwise == 1 ){
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 2 );
			i3 = *( indexbuf + faceno * 3 + 1 );
		}else if( selem->clockwise == 2 ){
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 1 );
			i3 = *( indexbuf + faceno * 3 + 2 );
		}else{
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 2 );
			i3 = *( indexbuf + faceno * 3 + 1 );
		}

		//i1 = *( indexbuf + faceno * 3 );
		//i2 = *( indexbuf + faceno * 3 + 1 );
		//i3 = *( indexbuf + faceno * 3 + 2 );
		//i1 = *( indexbuf + faceno * 3 );
		//i2 = *( indexbuf + faceno * 3 + 2 );
		//i3 = *( indexbuf + faceno * 3 + 1 );

		if( matnoindex ){
			curmatno = *( matnoindex + faceno );
		}else{
			curmatno = *( m_attrib0 + faceno );
		}
		
		// faceno
		sprintf_s( tempchar, LINECHARLENG, "\t\t3 V(%d %d %d)", i1, i2, i3 );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		// materialno
		if( curmatno >= 0 ){
			sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		// UV
		CD3DDisp* d3ddisp;
		d3ddisp = selem->d3ddisp;
		if( !d3ddisp ){
			DbgOut( "polymesh : WriteMQOObject : d3ddisp NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float u1, v1, u2, v2, u3, v3;
		ret = d3ddisp->GetUV( i1, &u1, &v1 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = d3ddisp->GetUV( i2, &u2, &v2 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = d3ddisp->GetUV( i3, &u3, &v3 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
			u1, v1, u2, v2, u3, v3 );

		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}



		// 改行
		strcpy_s( tempchar, LINECHARLENG, "\r\n" );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( selem->clockwise == 3 ){

		//裏面の出力。

		for( faceno = 0; faceno < meshinfo->n; faceno++ ){
			if( selem->clockwise == 1 ){
				i1 = *( indexbuf + faceno * 3 );
				i2 = *( indexbuf + faceno * 3 + 1 );
				i3 = *( indexbuf + faceno * 3 + 2 );
			}else if( selem->clockwise == 2 ){
				i1 = *( indexbuf + faceno * 3 );
				i2 = *( indexbuf + faceno * 3 + 2 );
				i3 = *( indexbuf + faceno * 3 + 1 );
			}else{
				i1 = *( indexbuf + faceno * 3 );
				i2 = *( indexbuf + faceno * 3 + 1 );
				i3 = *( indexbuf + faceno * 3 + 2 );
			}

			if( matnoindex ){
				curmatno = *( matnoindex + faceno );
			}else{
				curmatno = *( m_attrib0 + faceno );
			}
			
			// faceno
			sprintf_s( tempchar, LINECHARLENG, "\t\t3 V(%d %d %d)", i1, i2, i3 );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			// materialno
			if( curmatno >= 0 ){
				sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			// UV
			CD3DDisp* d3ddisp;
			d3ddisp = selem->d3ddisp;
			if( !d3ddisp ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			float u1, v1, u2, v2, u3, v3;
			ret = d3ddisp->GetUV( i1, &u1, &v1 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = d3ddisp->GetUV( i2, &u2, &v2 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = d3ddisp->GetUV( i3, &u3, &v3 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
				u1, v1, u2, v2, u3, v3 );

			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}



			// 改行
			strcpy_s( tempchar, LINECHARLENG, "\r\n" );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

	}


	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}


int CPolyMesh::WriteMQOObjectOnFrame( HANDLE hfile, int* matnoindex, int motid, int frameno, int serialno, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult, int writeRevFlag )
{
	int ret;
	char tempchar[LINECHARLENG];

	
	D3DXVECTOR3* wvec;
	wvec = new D3DXVECTOR3[ meshinfo->m ];
	if( !wvec ){
		DbgOut( "pm : WriteMQOObjectOnFrame : wvec alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*lpsh)( serialno );
	if( !selem ){
		DbgOut( "pm : WriteMQOObjectOnFrame : selem NULL error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}

	ret = lpmh->SetCurrentMotion( lpsh, motid, frameno );
	if( ret ){
		DbgOut( "pm : WriteMQOObjectOnFrame : mh SetCurrentMotion error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}

	D3DXMATRIX iniwmat;
	D3DXMatrixIdentity( &iniwmat );

	if( selem->m_mtype == M_NONE ){
		ret = selem->TransformOnlyWorld3( lpmh, iniwmat, wvec );
		if( ret ){
			DbgOut( "pm : WriteMQOObjectOnFrame : selem TransformOnlyWorld3 error !!!\n" );
			_ASSERT( 0 );
			delete [] wvec;
			return 1;
		}
	}else{
		_ASSERT( selem->d3ddisp );
		_ASSERT( selem->d3ddisp->m_skinv );
		int vno;
		for( vno = 0; vno < (int)selem->d3ddisp->m_numTLV; vno++ ){
			( wvec + vno )->x = ( selem->d3ddisp->m_skinv + vno )->pos[0];
			( wvec + vno )->y = ( selem->d3ddisp->m_skinv + vno )->pos[1];
			( wvec + vno )->z = ( selem->d3ddisp->m_skinv + vno )->pos[2];
		}
	}

// vertex
	//	vertex 9 {
	//		15.0621 87.7051 -57.7583
	//		...
	//	}	
	sprintf_s( tempchar, LINECHARLENG, "\tvertex %d {", meshinfo->m );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}

	int vertno;
	D3DXVECTOR3* curpoint;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		curpoint = wvec + vertno;

		//!!!!!!!
		// Z の符号を反転する。
		//!!!!!!!

		//sprintf( tempchar, "\t\t%.4f %.4f %.4f", curpoint->x, curpoint->y, curpoint->z );
		sprintf_s( tempchar, LINECHARLENG, "\t\t%.4f %.4f %.4f", curpoint->x * srcmult, curpoint->y * srcmult, -curpoint->z * srcmult );
		ret = WriteChar( hfile, tempchar, 1 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			delete [] wvec;
			return 1;
		}

	}


	delete [] wvec;


	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// face
	//	face 511 {
	//		4 V(487 19 4 482) M(0) UV(0.73213 0.24561 0.72100 0.24600 0.72100 0.26500 0.73217 0.26610)
	//		...	
	//	}
	
	int outputfacenum;

	if( selem->clockwise == 3 ){
		outputfacenum = meshinfo->n * 2;
	}else{
		outputfacenum = meshinfo->n;
	}


	sprintf_s( tempchar, LINECHARLENG, "\tface %d {", outputfacenum );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno;
	int i1, i2, i3;
	int curmatno;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){

		if( selem->clockwise == 1 ){
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 2 );
			i3 = *( indexbuf + faceno * 3 + 1 );
		}else if( selem->clockwise == 2 ){
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 1 );
			i3 = *( indexbuf + faceno * 3 + 2 );
		}else{
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 2 );
			i3 = *( indexbuf + faceno * 3 + 1 );
		}

		//i1 = *( indexbuf + faceno * 3 );
		//i2 = *( indexbuf + faceno * 3 + 1 );
		//i3 = *( indexbuf + faceno * 3 + 2 );
		//i1 = *( indexbuf + faceno * 3 );
		//i2 = *( indexbuf + faceno * 3 + 2 );
		//i3 = *( indexbuf + faceno * 3 + 1 );

		if( matnoindex ){
			curmatno = *( matnoindex + faceno );
		}else{
			curmatno = *( m_attrib0 + faceno );
		}
		
		// faceno
		sprintf_s( tempchar, LINECHARLENG, "\t\t3 V(%d %d %d)", i1, i2, i3 );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		// materialno
		if( curmatno >= 0 ){
			sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		// UV
		if( uvbuf ){
			COORDINATE* uv1;
			COORDINATE* uv2;
			COORDINATE* uv3;
			uv1 = uvbuf + i1;
			uv2 = uvbuf + i2;
			uv3 = uvbuf + i3;

			sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
				uv1->u, uv1->v, uv2->u, uv2->v, uv3->u, uv3->v );

			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			CD3DDisp* d3ddisp;
			d3ddisp = selem->d3ddisp;
			if( !d3ddisp ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			float u1, v1, u2, v2, u3, v3;
			ret = d3ddisp->GetUV( i1, &u1, &v1 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = d3ddisp->GetUV( i2, &u2, &v2 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = d3ddisp->GetUV( i3, &u3, &v3 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
				u1, v1, u2, v2, u3, v3 );

			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
		// 改行
		strcpy_s( tempchar, LINECHARLENG, "\r\n" );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( selem->clockwise == 3 ){

		//裏面の出力。

		for( faceno = 0; faceno < meshinfo->n; faceno++ ){
			if( selem->clockwise == 1 ){
				i1 = *( indexbuf + faceno * 3 );
				i2 = *( indexbuf + faceno * 3 + 1 );
				i3 = *( indexbuf + faceno * 3 + 2 );
			}else if( selem->clockwise == 2 ){
				i1 = *( indexbuf + faceno * 3 );
				i2 = *( indexbuf + faceno * 3 + 2 );
				i3 = *( indexbuf + faceno * 3 + 1 );
			}else{
				i1 = *( indexbuf + faceno * 3 );
				i2 = *( indexbuf + faceno * 3 + 1 );
				i3 = *( indexbuf + faceno * 3 + 2 );
			}

			if( matnoindex ){
				curmatno = *( matnoindex + faceno );
			}else{
				curmatno = *( m_attrib0 + faceno );
			}
			
			// faceno
			sprintf_s( tempchar, LINECHARLENG, "\t\t3 V(%d %d %d)", i1, i2, i3 );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			// materialno
			if( curmatno >= 0 ){
				sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			// UV
			if( uvbuf ){
				COORDINATE* uv1;
				COORDINATE* uv2;
				COORDINATE* uv3;
				uv1 = uvbuf + i1;
				uv2 = uvbuf + i2;
				uv3 = uvbuf + i3;

				sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
					uv1->u, uv1->v, uv2->u, uv2->v, uv3->u, uv3->v );

				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				CD3DDisp* d3ddisp;
				d3ddisp = selem->d3ddisp;
				if( !d3ddisp ){
					DbgOut( "polymesh : WriteMQOObject : d3ddisp NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				float u1, v1, u2, v2, u3, v3;
				ret = d3ddisp->GetUV( i1, &u1, &v1 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = d3ddisp->GetUV( i2, &u2, &v2 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = d3ddisp->GetUV( i3, &u3, &v3 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
					u1, v1, u2, v2, u3, v3 );

				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
			// 改行
			strcpy_s( tempchar, LINECHARLENG, "\r\n" );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

	}


	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CPolyMesh::WriteMQOObject( HANDLE hfile, int* matnoindex, CShdElem* selem )
{
	int ret;
	char tempchar[LINECHARLENG];

// vertex
	//	vertex 9 {
	//		15.0621 87.7051 -57.7583
	//		...
	//	}	
	sprintf_s( tempchar, LINECHARLENG, "\tvertex %d {", meshinfo->m );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int vertno;
	VEC3F* curpoint;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		curpoint = pointbuf + vertno;

		//!!!!!!!
		// Z の符号を反転する。
		//!!!!!!!

		//sprintf( tempchar, "\t\t%.4f %.4f %.4f", curpoint->x, curpoint->y, curpoint->z );
		sprintf_s( tempchar, LINECHARLENG, "\t\t%.4f %.4f %.4f", curpoint->x, curpoint->y, -curpoint->z );
		ret = WriteChar( hfile, tempchar, 1 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// face
	//	face 511 {
	//		4 V(487 19 4 482) M(0) UV(0.73213 0.24561 0.72100 0.24600 0.72100 0.26500 0.73217 0.26610)
	//		...	
	//	}
	
	int outputfacenum;

	if( selem->clockwise == 3 ){
		outputfacenum = meshinfo->n * 2;
	}else{
		outputfacenum = meshinfo->n;
	}


	sprintf_s( tempchar, LINECHARLENG, "\tface %d {", outputfacenum );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno;
	int i1, i2, i3;
	int curmatno;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){

		if( selem->clockwise == 1 ){
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 2 );
			i3 = *( indexbuf + faceno * 3 + 1 );
		}else if( selem->clockwise == 2 ){
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 1 );
			i3 = *( indexbuf + faceno * 3 + 2 );
		}else{
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 2 );
			i3 = *( indexbuf + faceno * 3 + 1 );
		}

		//i1 = *( indexbuf + faceno * 3 );
		//i2 = *( indexbuf + faceno * 3 + 1 );
		//i3 = *( indexbuf + faceno * 3 + 2 );
		//i1 = *( indexbuf + faceno * 3 );
		//i2 = *( indexbuf + faceno * 3 + 2 );
		//i3 = *( indexbuf + faceno * 3 + 1 );

		if( matnoindex ){
			curmatno = *( matnoindex + faceno );
		}else{
			curmatno = *( m_attrib0 + faceno );
		}
		
		// faceno
		sprintf_s( tempchar, LINECHARLENG, "\t\t3 V(%d %d %d)", i1, i2, i3 );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		// materialno
		if( curmatno >= 0 ){
			sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		// UV
		if( uvbuf ){
			COORDINATE* uv1;
			COORDINATE* uv2;
			COORDINATE* uv3;
			uv1 = uvbuf + i1;
			uv2 = uvbuf + i2;
			uv3 = uvbuf + i3;

			sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
				uv1->u, uv1->v, uv2->u, uv2->v, uv3->u, uv3->v );

			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			CD3DDisp* d3ddisp;
			d3ddisp = selem->d3ddisp;
			if( !d3ddisp ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			float u1, v1, u2, v2, u3, v3;
			ret = d3ddisp->GetUV( i1, &u1, &v1 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = d3ddisp->GetUV( i2, &u2, &v2 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = d3ddisp->GetUV( i3, &u3, &v3 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
				u1, v1, u2, v2, u3, v3 );

			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
		// 改行
		strcpy_s( tempchar, LINECHARLENG, "\r\n" );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( selem->clockwise == 3 ){

		//裏面の出力。

		for( faceno = 0; faceno < meshinfo->n; faceno++ ){
			if( selem->clockwise == 1 ){
				i1 = *( indexbuf + faceno * 3 );
				i2 = *( indexbuf + faceno * 3 + 1 );
				i3 = *( indexbuf + faceno * 3 + 2 );
			}else if( selem->clockwise == 2 ){
				i1 = *( indexbuf + faceno * 3 );
				i2 = *( indexbuf + faceno * 3 + 2 );
				i3 = *( indexbuf + faceno * 3 + 1 );
			}else{
				i1 = *( indexbuf + faceno * 3 );
				i2 = *( indexbuf + faceno * 3 + 1 );
				i3 = *( indexbuf + faceno * 3 + 2 );
			}

			if( matnoindex ){
				curmatno = *( matnoindex + faceno );
			}else{
				curmatno = *( m_attrib0 + faceno );
			}
			
			// faceno
			sprintf_s( tempchar, LINECHARLENG, "\t\t3 V(%d %d %d)", i1, i2, i3 );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			// materialno
			if( curmatno >= 0 ){
				sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			// UV
			if( uvbuf ){
				COORDINATE* uv1;
				COORDINATE* uv2;
				COORDINATE* uv3;
				uv1 = uvbuf + i1;
				uv2 = uvbuf + i2;
				uv3 = uvbuf + i3;

				sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
					uv1->u, uv1->v, uv2->u, uv2->v, uv3->u, uv3->v );

				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				CD3DDisp* d3ddisp;
				d3ddisp = selem->d3ddisp;
				if( !d3ddisp ){
					DbgOut( "polymesh : WriteMQOObject : d3ddisp NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				float u1, v1, u2, v2, u3, v3;
				ret = d3ddisp->GetUV( i1, &u1, &v1 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = d3ddisp->GetUV( i2, &u2, &v2 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = d3ddisp->GetUV( i3, &u3, &v3 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
					u1, v1, u2, v2, u3, v3 );

				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
			// 改行
			strcpy_s( tempchar, LINECHARLENG, "\r\n" );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

	}


	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}

/***
int CPolyMesh::WriteMQOObject( HANDLE hfile, int* matnoindex, CShdElem* selem )
{

	int ret;
	char tempchar[LINECHARLENG];

// vertex
	//	vertex 9 {
	//		15.0621 87.7051 -57.7583
	//		...
	//	}	
	sprintf_s( tempchar, LINECHARLENG, "\tvertex %d {", meshinfo->m );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int vertno;
	VEC3F* curpoint;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		curpoint = pointbuf + vertno;



		//!!!!!!!
		// Z の符号を反転する。
		//!!!!!!!

		//sprintf( tempchar, "\t\t%.4f %.4f %.4f", curpoint->x, curpoint->y, curpoint->z );
		sprintf_s( tempchar, LINECHARLENG, "\t\t%.4f %.4f %.4f", curpoint->x, curpoint->y, -curpoint->z );
		ret = WriteChar( hfile, tempchar, 1 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// face
	//	face 511 {
	//		4 V(487 19 4 482) M(0) UV(0.73213 0.24561 0.72100 0.24600 0.72100 0.26500 0.73217 0.26610)
	//		...	
	//	}
	
	sprintf_s( tempchar, LINECHARLENG, "\tface %d {", meshinfo->n );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno;
	int i1, i2, i3;
	int curmatno;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		i1 = *( indexbuf + faceno * 3 );
		i2 = *( indexbuf + faceno * 3 + 1 );
		i3 = *( indexbuf + faceno * 3 + 2 );
		//i1 = *( indexbuf + faceno * 3 );
		//i2 = *( indexbuf + faceno * 3 + 2 );
		//i3 = *( indexbuf + faceno * 3 + 1 );

		curmatno = *( matnoindex + faceno );
		
		// faceno
		sprintf_s( tempchar, LINECHARLENG, "\t\t3 V(%d %d %d)", i1, i2, i3 );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		// materialno
		if( curmatno >= 0 ){
			sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		// UV
		if( uvbuf ){
			COORDINATE* uv1;
			COORDINATE* uv2;
			COORDINATE* uv3;
			uv1 = uvbuf + i1;
			uv2 = uvbuf + i2;
			uv3 = uvbuf + i3;

			sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
				uv1->u, uv1->v, uv2->u, uv2->v, uv3->u, uv3->v );

			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			CD3DDisp* d3ddisp;
			d3ddisp = selem->d3ddisp;
			if( !d3ddisp ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			float u1, v1, u2, v2, u3, v3;
			ret = d3ddisp->GetUV( i1, &u1, &v1 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = d3ddisp->GetUV( i2, &u2, &v2 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = d3ddisp->GetUV( i3, &u3, &v3 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : d3ddisp GetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
				u1, v1, u2, v2, u3, v3 );

			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	
		// 改行
		strcpy_s( tempchar, LINECHARLENG, "\r\n" );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}


	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/

int CPolyMesh::WriteChar( HANDLE hfile, char* writechar, int addreturn )
{
	unsigned long wleng, writeleng;

	if( addreturn )
		strcat_s( writechar, LINECHARLENG, "\r\n" );

	wleng = (unsigned long)strlen( writechar );
	if( wleng >= LINECHARLENG ){
		DbgOut( "polymesh : WriteChar : wleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	WriteFile( hfile, writechar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CPolyMesh::CreateInfElemIfNot( int leng )
{
	if( m_IE )
		return 0;

	m_IE = new CInfElem[ leng ];
	if( !m_IE ){
		DbgOut( "polymesh : CreateInfElemIfNot : m_IE alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CPolyMesh::SetMaterialDiffuse( int matno, int setflag, ARGBF srcdiffuse )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	ARGBF newargb;
	newargb.a = 1.0f;

	float fr, fg, fb;
	fr = srcdiffuse.r;
	fg = srcdiffuse.g;
	fb = srcdiffuse.b;

	if( fr < 0.0f )
		fr = 0.0f;
	if( fr > 1.0f )
		fr = 1.0f;

	if( fg < 0.0f )
		fg = 0.0f;
	if( fg > 1.0f )
		fg = 1.0f;

	if( fb < 0.0f )
		fb = 0.0f;
	if( fb > 1.0f )
		fb = 1.0f;



	int vertno;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		
		if( *( dirtyptr + vertno ) != 0 ){

			ARGBF* srcbuf = diffusebuf + vertno;

			switch( setflag ){
			case 0://そのままセット
				newargb.r = fr;
				newargb.g = fg;
				newargb.b = fb;
				break;
			case 1://乗算
				newargb.r = srcbuf->r * fr;
				newargb.g = srcbuf->g * fg;
				newargb.b = srcbuf->b * fb;
				break;
			case 2://足し算
				newargb.r = srcbuf->r + fr;
				if( newargb.r > 1.0f )
					newargb.r = 1.0f;

				newargb.g = srcbuf->g + fg;
				if( newargb.g > 1.0f )
					newargb.g = 1.0f;

				newargb.b = srcbuf->b + fb;
				if( newargb.b > 1.0f )
					newargb.b = 1.0f;

				break;
			case 3://引き算
				newargb.r = srcbuf->r - fr;
				if( newargb.r < 0.0f )
					newargb.r = 0.0f;

				newargb.g = srcbuf->g - fg;
				if( newargb.g < 0.0f )
					newargb.g = 0.0f;

				newargb.b = srcbuf->b - fb;
				if( newargb.b < 0.0f )
					newargb.b = 0.0f;
				
				break;
			default:
				break;
			}

			*( diffusebuf + vertno ) = newargb;

		}

	}
	return 0;
}
int CPolyMesh::SetMaterialSpecular( int matno, int setflag, ARGBF srcspecular )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	ARGBF newargb;
	newargb.a = 1.0f;

	float fr, fg, fb;
	fr = srcspecular.r;
	fg = srcspecular.g;
	fb = srcspecular.b;

	if( fr < 0.0f )
		fr = 0.0f;
	if( fr > 1.0f )
		fr = 1.0f;

	if( fg < 0.0f )
		fg = 0.0f;
	if( fg > 1.0f )
		fg = 1.0f;

	if( fb < 0.0f )
		fb = 0.0f;
	if( fb > 1.0f )
		fb = 1.0f;



	int vertno;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		
		if( *( dirtyptr + vertno ) != 0 ){

			ARGBF* srcbuf = specularbuf + vertno;

			switch( setflag ){
			case 0://そのままセット
				newargb.r = fr;
				newargb.g = fg;
				newargb.b = fb;
				break;
			case 1://乗算
				newargb.r = srcbuf->r * fr;
				newargb.g = srcbuf->g * fg;
				newargb.b = srcbuf->b * fb;
				break;
			case 2://足し算
				newargb.r = srcbuf->r + fr;
				if( newargb.r > 1.0f )
					newargb.r = 1.0f;

				newargb.g = srcbuf->g + fg;
				if( newargb.g > 1.0f )
					newargb.g = 1.0f;

				newargb.b = srcbuf->b + fb;
				if( newargb.b > 1.0f )
					newargb.b = 1.0f;

				break;
			case 3://引き算
				newargb.r = srcbuf->r - fr;
				if( newargb.r < 0.0f )
					newargb.r = 0.0f;

				newargb.g = srcbuf->g - fg;
				if( newargb.g < 0.0f )
					newargb.g = 0.0f;

				newargb.b = srcbuf->b - fb;
				if( newargb.b < 0.0f )
					newargb.b = 0.0f;
				
				break;
			default:
				break;
			}

			*( specularbuf + vertno ) = newargb;

		}

	}

	return 0;
}
int CPolyMesh::SetMaterialAmbient( int matno, int setflag, ARGBF srcambient )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	ARGBF newargb;
	newargb.a = 1.0f;

	float fr, fg, fb;
	fr = srcambient.r;
	fg = srcambient.g;
	fb = srcambient.b;

	if( fr < 0.0f )
		fr = 0.0f;
	if( fr > 1.0f )
		fr = 1.0f;

	if( fg < 0.0f )
		fg = 0.0f;
	if( fg > 1.0f )
		fg = 1.0f;

	if( fb < 0.0f )
		fb = 0.0f;
	if( fb > 1.0f )
		fb = 1.0f;



	int vertno;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		
		if( *( dirtyptr + vertno ) != 0 ){

			ARGBF* srcbuf = ambientbuf + vertno;

			switch( setflag ){
			case 0://そのままセット
				newargb.r = fr;
				newargb.g = fg;
				newargb.b = fb;
				break;
			case 1://乗算
				newargb.r = srcbuf->r * fr;
				newargb.g = srcbuf->g * fg;
				newargb.b = srcbuf->b * fb;
				break;
			case 2://足し算
				newargb.r = srcbuf->r + fr;
				if( newargb.r > 1.0f )
					newargb.r = 1.0f;

				newargb.g = srcbuf->g + fg;
				if( newargb.g > 1.0f )
					newargb.g = 1.0f;

				newargb.b = srcbuf->b + fb;
				if( newargb.b > 1.0f )
					newargb.b = 1.0f;

				break;
			case 3://引き算
				newargb.r = srcbuf->r - fr;
				if( newargb.r < 0.0f )
					newargb.r = 0.0f;

				newargb.g = srcbuf->g - fg;
				if( newargb.g < 0.0f )
					newargb.g = 0.0f;

				newargb.b = srcbuf->b - fb;
				if( newargb.b < 0.0f )
					newargb.b = 0.0f;
				
				break;
			default:
				break;
			}

			*( ambientbuf + vertno ) = newargb;

		}

	}

	return 0;
}
int CPolyMesh::SetMaterialEmissive( int matno, int setflag, ARGBF srcemissive )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	ARGBF newargb;
	newargb.a = 1.0f;

	float fr, fg, fb;
	fr = srcemissive.r;
	fg = srcemissive.g;
	fb = srcemissive.b;

	if( fr < 0.0f )
		fr = 0.0f;
	if( fr > 1.0f )
		fr = 1.0f;

	if( fg < 0.0f )
		fg = 0.0f;
	if( fg > 1.0f )
		fg = 1.0f;

	if( fb < 0.0f )
		fb = 0.0f;
	if( fb > 1.0f )
		fb = 1.0f;



	int vertno;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		
		if( *( dirtyptr + vertno ) != 0 ){

			ARGBF* srcbuf = emissivebuf + vertno;

			switch( setflag ){
			case 0://そのままセット
				newargb.r = fr;
				newargb.g = fg;
				newargb.b = fb;
				break;
			case 1://乗算
				newargb.r = srcbuf->r * fr;
				newargb.g = srcbuf->g * fg;
				newargb.b = srcbuf->b * fb;
				break;
			case 2://足し算
				newargb.r = srcbuf->r + fr;
				if( newargb.r > 1.0f )
					newargb.r = 1.0f;

				newargb.g = srcbuf->g + fg;
				if( newargb.g > 1.0f )
					newargb.g = 1.0f;

				newargb.b = srcbuf->b + fb;
				if( newargb.b > 1.0f )
					newargb.b = 1.0f;

				break;
			case 3://引き算
				newargb.r = srcbuf->r - fr;
				if( newargb.r < 0.0f )
					newargb.r = 0.0f;

				newargb.g = srcbuf->g - fg;
				if( newargb.g < 0.0f )
					newargb.g = 0.0f;

				newargb.b = srcbuf->b - fb;
				if( newargb.b < 0.0f )
					newargb.b = 0.0f;
				
				break;
			default:
				break;
			}

			*( emissivebuf + vertno ) = newargb;

		}

	}

	return 0;
}
int CPolyMesh::SetMaterialPower( int matno, int setflag, float srcpow )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	float newpow;

	int vertno;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		
		if( *( dirtyptr + vertno ) != 0 ){

			float* srcbuf = powerbuf + vertno;

			switch( setflag ){
			case 0://そのままセット
				newpow = srcpow;
				break;
			case 1://乗算
				newpow = *srcbuf * srcpow;
				break;
			case 2://足し算
				newpow = *srcbuf + srcpow;
				break;
			case 3://引き算
				newpow = *srcbuf - srcpow;
				break;
			default:
				break;
			}

			*( powerbuf + vertno ) = newpow;

		}

	}

	return 0;
}


int CPolyMesh::SetDiffuse( int srcvertno, int setflag, ARGBF srcdiffuse, CD3DDisp* d3dptr, int tlmode )
{
	int ret;

	if( srcvertno < 0 ){
		int vertno;
		for( vertno = 0; vertno < meshinfo->m; vertno++ ){
			ret = SetDiffuse( vertno, setflag, srcdiffuse, d3dptr, tlmode );
			if( ret ){
				DbgOut( "polymesh : SetDiffuse : vert %d SetDiffuse error !!!\n", vertno );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		if( srcvertno >= meshinfo->m ){
			DbgOut( "polymesh : SetDiffuse : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ARGBF newargb;
		newargb.a = 1.0f;


		float fr, fg, fb;
		fr = srcdiffuse.r;
		fg = srcdiffuse.g;
		fb = srcdiffuse.b;

		if( fr < 0.0f )
			fr = 0.0f;
		if( fr > 1.0f )
			fr = 1.0f;

		if( fg < 0.0f )
			fg = 0.0f;
		if( fg > 1.0f )
			fg = 1.0f;

		if( fb < 0.0f )
			fb = 0.0f;
		if( fb > 1.0f )
			fb = 1.0f;

		ARGBF* srcbuf = diffusebuf + srcvertno;


		switch( setflag ){
		case 0://そのままセット
			newargb.r = fr;
			newargb.g = fg;
			newargb.b = fb;
			break;
		case 1://乗算
			newargb.r = srcbuf->r * fr;
			newargb.g = srcbuf->g * fg;
			newargb.b = srcbuf->b * fb;
			break;
		case 2://足し算
			newargb.r = srcbuf->r + fr;
			if( newargb.r > 1.0f )
				newargb.r = 1.0f;

			newargb.g = srcbuf->g + fg;
			if( newargb.g > 1.0f )
				newargb.g = 1.0f;

			newargb.b = srcbuf->b + fb;
			if( newargb.b > 1.0f )
				newargb.b = 1.0f;

			break;
		case 3://引き算
			newargb.r = srcbuf->r - fr;
			if( newargb.r < 0.0f )
				newargb.r = 0.0f;

			newargb.g = srcbuf->g - fg;
			if( newargb.g < 0.0f )
				newargb.g = 0.0f;

			newargb.b = srcbuf->b - fb;
			if( newargb.b < 0.0f )
				newargb.b = 0.0f;
			
			break;
		default:
			break;
		}

		*( diffusebuf + srcvertno ) = newargb;


		// disp データの更新
		if( d3dptr && (tlmode == TLMODE_D3D) ){
			DWORD dwcol;
			int inewa, inewr, inewg, inewb;
			inewa = (int)( newargb.a * 255.0f );
			inewr = (int)( newargb.r * 255.0f );
			inewg = (int)( newargb.g * 255.0f );
			inewb = (int)( newargb.b * 255.0f );

			dwcol = D3DCOLOR_ARGB( inewa, inewr, inewg, inewb );
			ret = d3dptr->SetOrgTlvColor( srcvertno, dwcol, MAT_DIFFUSE );
			if( ret ){
				DbgOut( "polymesh : SetDiffuse : d3dptr SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}
int CPolyMesh::SetEmissive( int srcvertno, int setflag, ARGBF srcemissive, CD3DDisp* d3dptr, int tlmode )
{
	int ret;

	if( srcvertno < 0 ){
		int vertno;
		for( vertno = 0; vertno < meshinfo->m; vertno++ ){
			ret = SetEmissive( vertno, setflag, srcemissive, d3dptr, tlmode );
			if( ret ){
				DbgOut( "polymesh : SetEmissive : vert %d SetEmissive error !!!\n", vertno );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		if( srcvertno >= meshinfo->m ){
			DbgOut( "polymesh : SetEmissive : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ARGBF newargb;
		newargb.a = 1.0f;


		float fr, fg, fb;
		fr = srcemissive.r;
		fg = srcemissive.g;
		fb = srcemissive.b;

		if( fr < 0.0f )
			fr = 0.0f;
		if( fr > 1.0f )
			fr = 1.0f;

		if( fg < 0.0f )
			fg = 0.0f;
		if( fg > 1.0f )
			fg = 1.0f;

		if( fb < 0.0f )
			fb = 0.0f;
		if( fb > 1.0f )
			fb = 1.0f;

		ARGBF* srcbuf = emissivebuf + srcvertno;


		switch( setflag ){
		case 0://そのままセット
			newargb.r = fr;
			newargb.g = fg;
			newargb.b = fb;
			break;
		case 1://乗算
			newargb.r = srcbuf->r * fr;
			newargb.g = srcbuf->g * fg;
			newargb.b = srcbuf->b * fb;
			break;
		case 2://足し算
			newargb.r = srcbuf->r + fr;
			if( newargb.r > 1.0f )
				newargb.r = 1.0f;

			newargb.g = srcbuf->g + fg;
			if( newargb.g > 1.0f )
				newargb.g = 1.0f;

			newargb.b = srcbuf->b + fb;
			if( newargb.b > 1.0f )
				newargb.b = 1.0f;

			break;
		case 3://引き算
			newargb.r = srcbuf->r - fr;
			if( newargb.r < 0.0f )
				newargb.r = 0.0f;

			newargb.g = srcbuf->g - fg;
			if( newargb.g < 0.0f )
				newargb.g = 0.0f;

			newargb.b = srcbuf->b - fb;
			if( newargb.b < 0.0f )
				newargb.b = 0.0f;
			
			break;
		default:
			break;
		}

		*( emissivebuf + srcvertno ) = newargb;

		/***	
		// disp データの更新
		if( d3dptr && (tlmode == TLMODE_D3D) ){
			DWORD dwcol;
			int inewa, inewr, inewg, inewb;
			inewa = (int)( newargb.a * 255.0f );
			inewr = (int)( newargb.r * 255.0f );
			inewg = (int)( newargb.g * 255.0f );
			inewb = (int)( newargb.b * 255.0f );

			dwcol = D3DCOLOR_ARGB( inewa, inewr, inewg, inewb );
			ret = d3dptr->SetOrgTlvColor( srcvertno, dwcol, MAT_SPECULAR );
			if( ret ){
				DbgOut( "polymesh : SetSpecular : d3dptr SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		***/
	
	}

	return 0;
}
int CPolyMesh::SetSpecularPower( int srcvertno, int setflag, float srcpow, CD3DDisp* d3dptr, int tlmode )
{
	int ret;

	if( srcvertno < 0 ){
		int vertno;
		for( vertno = 0; vertno < meshinfo->m; vertno++ ){
			ret = SetSpecularPower( vertno, setflag, srcpow, d3dptr, tlmode );
			if( ret ){
				DbgOut( "polymesh : SetspecularPower : vert %d SetspecularPower error !!!\n", vertno );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		if( srcvertno >= meshinfo->m ){
			DbgOut( "polymesh : SetspecularPower : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float newpow;

		if( srcpow < 0.0f )
			srcpow = 0.0f;
		if( srcpow > 100.0f )
			srcpow = 100.0f;


		float* srcbuf = powerbuf + srcvertno;


		switch( setflag ){
		case 0://そのままセット
			newpow = srcpow;
			break;
		case 1://乗算
			newpow = *srcbuf * srcpow; 
			break;
		case 2://足し算
			newpow = *srcbuf + srcpow;
			if( newpow > 100.0f )
				newpow = 100.0f;
			break;
		case 3://引き算
			newpow = *srcbuf - srcpow;
			if( newpow < 0.0f )
				newpow = 0.0f;			
			break;
		default:
			break;
		}

		*( powerbuf + srcvertno ) = newpow;

		/***
		// disp データの更新
		if( d3dptr && (tlmode == TLMODE_D3D) ){
			DWORD dwcol;
			int inewa, inewr, inewg, inewb;
			inewa = (int)( newargb.a * 255.0f );
			inewr = (int)( newargb.r * 255.0f );
			inewg = (int)( newargb.g * 255.0f );
			inewb = (int)( newargb.b * 255.0f );

			dwcol = D3DCOLOR_ARGB( inewa, inewr, inewg, inewb );
			ret = d3dptr->SetOrgTlvColor( srcvertno, dwcol, MAT_SPECULAR );
			if( ret ){
				DbgOut( "polymesh : SetSpecular : d3dptr SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		***/
	
	}

	return 0;

}


int CPolyMesh::SetSpecular( int srcvertno, int setflag, ARGBF srcspecular, CD3DDisp* d3dptr, int tlmode )
{
	int ret;

	if( srcvertno < 0 ){
		int vertno;
		for( vertno = 0; vertno < meshinfo->m; vertno++ ){
			ret = SetSpecular( vertno, setflag, srcspecular, d3dptr, tlmode );
			if( ret ){
				DbgOut( "polymesh : Setspecular : vert %d Setspecular error !!!\n", vertno );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		if( srcvertno >= meshinfo->m ){
			DbgOut( "polymesh : Setspecular : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ARGBF newargb;
		newargb.a = 1.0f;


		float fr, fg, fb;
		fr = srcspecular.r;
		fg = srcspecular.g;
		fb = srcspecular.b;

		if( fr < 0.0f )
			fr = 0.0f;
		if( fr > 1.0f )
			fr = 1.0f;

		if( fg < 0.0f )
			fg = 0.0f;
		if( fg > 1.0f )
			fg = 1.0f;

		if( fb < 0.0f )
			fb = 0.0f;
		if( fb > 1.0f )
			fb = 1.0f;

		ARGBF* srcbuf = specularbuf + srcvertno;


		switch( setflag ){
		case 0://そのままセット
			newargb.r = fr;
			newargb.g = fg;
			newargb.b = fb;
			break;
		case 1://乗算
			newargb.r = srcbuf->r * fr;
			newargb.g = srcbuf->g * fg;
			newargb.b = srcbuf->b * fb;
			break;
		case 2://足し算
			newargb.r = srcbuf->r + fr;
			if( newargb.r > 1.0f )
				newargb.r = 1.0f;

			newargb.g = srcbuf->g + fg;
			if( newargb.g > 1.0f )
				newargb.g = 1.0f;

			newargb.b = srcbuf->b + fb;
			if( newargb.b > 1.0f )
				newargb.b = 1.0f;

			break;
		case 3://引き算
			newargb.r = srcbuf->r - fr;
			if( newargb.r < 0.0f )
				newargb.r = 0.0f;

			newargb.g = srcbuf->g - fg;
			if( newargb.g < 0.0f )
				newargb.g = 0.0f;

			newargb.b = srcbuf->b - fb;
			if( newargb.b < 0.0f )
				newargb.b = 0.0f;
			
			break;
		default:
			break;
		}

		*( specularbuf + srcvertno ) = newargb;

	
		// disp データの更新
		if( d3dptr && (tlmode == TLMODE_D3D) ){
			DWORD dwcol;
			int inewa, inewr, inewg, inewb;
			inewa = (int)( newargb.a * 255.0f );
			inewr = (int)( newargb.r * 255.0f );
			inewg = (int)( newargb.g * 255.0f );
			inewb = (int)( newargb.b * 255.0f );

			dwcol = D3DCOLOR_ARGB( inewa, inewr, inewg, inewb );
			ret = d3dptr->SetOrgTlvColor( srcvertno, dwcol, MAT_SPECULAR );
			if( ret ){
				DbgOut( "polymesh : SetSpecular : d3dptr SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	
	
	}

	return 0;
}
int CPolyMesh::SetAmbient( int srcvertno, int setflag, ARGBF srcambient )
{
	int ret;

	if( srcvertno < 0 ){
		int vertno;
		for( vertno = 0; vertno < meshinfo->m; vertno++ ){
			ret = SetAmbient( vertno, setflag, srcambient );
			if( ret ){
				DbgOut( "polymesh : SetAmbient : vert %d SetAmbient error !!!\n", vertno );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		if( srcvertno >= meshinfo->m ){
			DbgOut( "polymesh : SetAmbient : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ARGBF newargb;
		newargb.a = 1.0f;


		float fr, fg, fb;
		fr = srcambient.r;
		fg = srcambient.g;
		fb = srcambient.b;

		if( fr < 0.0f )
			fr = 0.0f;
		if( fr > 1.0f )
			fr = 1.0f;

		if( fg < 0.0f )
			fg = 0.0f;
		if( fg > 1.0f )
			fg = 1.0f;

		if( fb < 0.0f )
			fb = 0.0f;
		if( fb > 1.0f )
			fb = 1.0f;

		ARGBF* srcbuf = ambientbuf + srcvertno;


		switch( setflag ){
		case 0://そのままセット
			newargb.r = fr;
			newargb.g = fg;
			newargb.b = fb;
			break;
		case 1://乗算
			newargb.r = srcbuf->r * fr;
			newargb.g = srcbuf->g * fg;
			newargb.b = srcbuf->b * fb;
			break;
		case 2://足し算
			newargb.r = srcbuf->r + fr;
			if( newargb.r > 1.0f )
				newargb.r = 1.0f;

			newargb.g = srcbuf->g + fg;
			if( newargb.g > 1.0f )
				newargb.g = 1.0f;

			newargb.b = srcbuf->b + fb;
			if( newargb.b > 1.0f )
				newargb.b = 1.0f;

			break;
		case 3://引き算
			newargb.r = srcbuf->r - fr;
			if( newargb.r < 0.0f )
				newargb.r = 0.0f;

			newargb.g = srcbuf->g - fg;
			if( newargb.g < 0.0f )
				newargb.g = 0.0f;

			newargb.b = srcbuf->b - fb;
			if( newargb.b < 0.0f )
				newargb.b = 0.0f;
			
			break;
		default:
			break;
		}

		*( ambientbuf + srcvertno ) = newargb;
	}

	return 0;
}

int CPolyMesh::GetDiffuse( int srcvertno, ARGBF* dstdiffuse )
{
	if( (srcvertno < 0) || (srcvertno >= meshinfo->m) ){
		DbgOut( "polymesh : GetDiffuse : srcvertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstdiffuse = *( diffusebuf + srcvertno );


	return 0;
}
int CPolyMesh::GetAmbient( int srcvertno , ARGBF* dstambient )
{
	if( (srcvertno < 0) || (srcvertno >= meshinfo->m) ){
		DbgOut( "polymesh : GetAmbient : srcvertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstambient = *( ambientbuf + srcvertno );

	return 0;
}
int CPolyMesh::GetEmissive( int srcvertno, ARGBF* dstemissive )
{

	if( (srcvertno < 0) || (srcvertno >= meshinfo->m) ){
		DbgOut( "polymesh : GetEmissive : srcvertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstemissive = *( emissivebuf + srcvertno );

	return 0;

}
int CPolyMesh::GetSpecularPower( int srcvertno, float* powerptr )
{
	if( (srcvertno < 0) || (srcvertno >= meshinfo->m) ){
		DbgOut( "polymesh : GetSpecularPower : srcvertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*powerptr = *(powerbuf + srcvertno);
	return 0;

}


int CPolyMesh::GetSpecular( int srcvertno, ARGBF* dstspecular )
{

	if( (srcvertno < 0) || (srcvertno >= meshinfo->m) ){
		DbgOut( "polymesh : GetSpecular : srcvertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstspecular = *( specularbuf + srcvertno );

	return 0;
}

int CPolyMesh::GetMaterialDiffuse( int matno, ARGBF* dstdiffuse )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int firstv = 0;
	int vertno;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){
			firstv = vertno;
			break;
		}
	}
	
	*dstdiffuse = *( diffusebuf + firstv );

	return 0;
}
int CPolyMesh::GetMaterialSpecular( int matno, ARGBF* dstspecular )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int firstv = 0;
	int vertno;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){
			firstv = vertno;
			break;
		}
	}
	
	*dstspecular = *( specularbuf + firstv );

	return 0;
}
int CPolyMesh::GetMaterialAmbient( int matno, ARGBF* dstambient )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int firstv = 0;
	int vertno;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){
			firstv = vertno;
			break;
		}
	}
	
	*dstambient = *( ambientbuf + firstv );

	return 0;
}
int CPolyMesh::GetMaterialEmissive( int matno, ARGBF* dstemissive )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int firstv = 0;
	int vertno;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){
			firstv = vertno;
			break;
		}
	}
	
	*dstemissive = *( emissivebuf + firstv );

	return 0;
}
int CPolyMesh::GetMaterialPower( int matno, float* dstpower )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int firstv = 0;
	int vertno;
	for( vertno = 0; vertno < meshinfo->m; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){
			firstv = vertno;
			break;
		}
	}
	
	*dstpower = *( powerbuf + firstv );

	return 0;
}



int CPolyMesh::GetOrgVertNo( int vertno, int* orgnoarray, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;

	if( (vertno < 0) || (vertno >= meshinfo->m) ){
		DbgOut( "polymesh : GetOrgVertNo : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( orgnoarray && (arrayleng >= 1) ){
		//*orgnoarray = vertno;//同じ
		//*getnumptr = 1;

	//2004/09/15
		*orgnoarray = *( orgnobuf + vertno );
		*getnumptr = 1;

	}

	return 0;
}

int CPolyMesh::ChangeColor( CPolyMesh* srcpm )
{

	int pointbufleng;
	pointbufleng = meshinfo->m;
	if( pointbufleng != srcpm->meshinfo->m ){
		DbgOut( "pm : ChangeColor : pointbufleng not equal return\r\n" );
		return 2;
	}

	if( !diffusebuf || !ambientbuf || !specularbuf || !powerbuf || !emissivebuf ){
		DbgOut( "pm : ChangeColor : colorbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !srcpm->diffusebuf || !srcpm->ambientbuf || !srcpm->specularbuf || !srcpm->powerbuf || !srcpm->emissivebuf ){
		DbgOut( "pm : ChangeColor : src colorbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	memcpy( diffusebuf, srcpm->diffusebuf, sizeof( ARGBF ) * pointbufleng );
	memcpy( ambientbuf, srcpm->ambientbuf, sizeof( ARGBF ) * pointbufleng );
	memcpy( specularbuf, srcpm->specularbuf, sizeof( ARGBF ) * pointbufleng );
	memcpy( powerbuf, srcpm->powerbuf, sizeof( float ) * pointbufleng );
	memcpy( emissivebuf, srcpm->emissivebuf, sizeof( ARGBF ) * pointbufleng );

	return 0;
}

int CPolyMesh::CalcInitialInfElem( int srcmatno, int srcchildno )
{
	if( !m_IE ){
		DbgOut( "pm : CalcInitialInfElem : IE NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	int pointnum = meshinfo->m;
	int pno;
	CInfElem* curie;
	for( pno = 0; pno < pointnum; pno++ ){
		curie = m_IE + pno;

		//ret = curie->SetInfElemInitial();
		ret = curie->SetInfElemDefaultNoSkin( srcmatno, srcchildno );
		if( ret ){
			DbgOut( "pm : CalcInitialInfElem : ie SetInfElemIDefaultNoSkin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CPolyMesh::CheckVertInShape( D3DXVECTOR3 srcv, int clockwise, int* insideptr )
{
	*insideptr = 0;

	//面の法線と、面の中心-->srcvとの内積が、全て負だったら、アンカーの内側。

	int facenum = meshinfo->n;
	int faceno;

	D3DXVECTOR3 vec1, vec2;
	D3DXVECTOR3 facenormal;

	D3DXVECTOR3 facecenter;
	D3DXVECTOR3 center2src;

	float dot;

	int findplusdot = 0;

	for( faceno = 0; faceno < facenum; faceno++ ){
		VEC3F* p0;
		VEC3F* p1;
		VEC3F* p2;

		int i1, i2, i3;
		if( clockwise == 2 ){
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 1 );
			i3 = *( indexbuf + faceno * 3 + 2 );
		}else{
			i1 = *( indexbuf + faceno * 3 );
			i2 = *( indexbuf + faceno * 3 + 2 );
			i3 = *( indexbuf + faceno * 3 + 1 );
		}

		p0 = pointbuf + i1;
		p1 = pointbuf + i2;
		p2 = pointbuf + i3;

		vec1.x = p1->x - p0->x;
		vec1.y = p1->y - p0->y;
		vec1.z = p1->z - p0->z;

		vec2.x = p2->x - p0->x;
		vec2.y = p2->y - p0->y;
		vec2.z = p2->z - p0->z;

		D3DXVec3Cross( &facenormal, &vec1, &vec2 );

		facecenter.x = (p0->x + p1->x + p2->x) / 3.0f;
		facecenter.y = (p0->y + p1->y + p2->y) / 3.0f;
		facecenter.z = (p0->z + p1->z + p2->z) / 3.0f;

		center2src = srcv - facecenter;

		dot = D3DXVec3Dot( &facenormal, &center2src );
		if( dot > 0.0f ){
			findplusdot = 1;
			break;
		}

	}

	if( findplusdot == 0 ){
		*insideptr = 1;
	}else{
		*insideptr = 0;
	}
	
	return 0;
}

int CPolyMesh::CalcAnchorCenter()
{
	int pnum = meshinfo->m;

	float minx = 1e6;
	float maxx = -1e6;
	float miny = 1e6;
	float maxy = -1e6;
	float minz = 1e6;
	float maxz = -1e6;

	int pno;
	VEC3F* curp;
	for( pno = 0; pno < pnum; pno++ ){
		curp = pointbuf + pno;

		if( curp->x < minx )
			minx = curp->x;
		if( curp->x > maxx )
			maxx = curp->x;

		if( curp->y < miny )
			miny = curp->y;
		if( curp->y > maxy )
			maxy = curp->y;

		if( curp->z < minz )
			minz = curp->z;
		if( curp->z > maxz )
			maxz = curp->z;
	}

	m_center.x = ( minx + maxx ) * 0.5f;
	m_center.y = ( miny + maxy ) * 0.5f;
	m_center.z = ( minz + maxz ) * 0.5f;

	return 0;
}

int CPolyMesh::CreateAndSaveTempMaterial()
{
	DestroyTempMaterial();

	int pointbufleng;
	pointbufleng = meshinfo->m;
	
	tempdiffuse = (ARGBF*)malloc( sizeof( ARGBF ) * pointbufleng );
	if( !tempdiffuse ){
		DbgOut( "pm : CreateAndSaveTempMaterial : tempdiffuse alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	tempambient = (ARGBF*)malloc( sizeof( ARGBF ) * pointbufleng );
	if( !tempambient ){
		DbgOut( "pm : CreateAndSaveTempMaterial : tempambient alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	tempspecular = (ARGBF*)malloc( sizeof( ARGBF ) * pointbufleng );
	if( !tempspecular ){
		DbgOut( "pm : CreateAndSaveTempMaterial : tempspecular alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	tempemissive = (ARGBF*)malloc( sizeof( ARGBF ) * pointbufleng );
	if( !tempemissive ){
		DbgOut( "pm : CreateAndSaveTempMaterial : tempemissive alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	temppower = (float*)malloc( sizeof( float ) * pointbufleng );
	if( !temppower ){
		DbgOut( "pm : CreateAndSaveTempMaterial : temppower alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MoveMemory( tempdiffuse, diffusebuf, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( tempambient, ambientbuf, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( tempspecular, specularbuf, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( tempemissive, emissivebuf, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( temppower, powerbuf, sizeof( float ) * pointbufleng );

	return 0;
}
int CPolyMesh::RestoreAndDestroyTempMaterial()
{
	if( (!tempdiffuse) || (!tempambient) || (!tempspecular) || (!tempemissive) || (!temppower) ){
		return 0;//!!!!!!!!
	}

	int pointbufleng;
	pointbufleng = meshinfo->m;

	MoveMemory( diffusebuf, tempdiffuse, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( ambientbuf, tempambient, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( specularbuf, tempspecular, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( emissivebuf, tempemissive, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( powerbuf, temppower, sizeof( float ) * pointbufleng );

	DestroyTempMaterial();

	return 0;
}

int CPolyMesh::SetMaterialFromInfElem( int srcchildno, int srcvert )
{
	int ret;
	if( srcvert < 0 ){
		int vert;
		for( vert = 0; vert < meshinfo->m; vert++ ){
			ret = SetMaterialFromInfElem( srcchildno, vert );
			if( ret ){
				DbgOut( "pm : SetMaterialFromInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		return 0;
	}

	if( (srcvert < 0) || (srcvert >= meshinfo->m) ){
		DbgOut( "pm : SetMaterialFromInfElem : srcvert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	_ASSERT( m_IE );

	CInfElem* curie;
	curie = m_IE + srcvert;
	
	int infno;
	INFELEM* curIE;
	INFELEM* findIE = 0;
	for( infno = 0; infno < curie->infnum; infno++ ){
		curIE = curie->ie + infno;
		
		if( curIE->childno == srcchildno ){
			findIE = curIE;
			break;
		}
	}

	if( srcchildno >= 0 ){
		if( findIE ){
			RDBColor3f calccol;
			MakeColor( 0.0f, 1.0f, findIE->dispinf, infcol[findIE->kind], &calccol );
			( diffusebuf + srcvert )->r = calccol.r;
			( diffusebuf + srcvert )->g = calccol.g;
			( diffusebuf + srcvert )->b = calccol.b;

			float ambr, ambg, ambb;
			ambr = calccol.r * 0.25f;
			ambg = calccol.g * 0.25f;
			ambb = calccol.b * 0.25f;
			( ambientbuf + srcvert )->r = ambr;
			( ambientbuf + srcvert )->g = ambg;
			( ambientbuf + srcvert )->b = ambb;

			( specularbuf + srcvert )->r = 0.0f;
			( specularbuf + srcvert )->g = 0.0f;
			( specularbuf + srcvert )->b = 0.0f;

			( emissivebuf + srcvert )->r = 0.0f;
			( emissivebuf + srcvert )->g = 0.0f;
			( emissivebuf + srcvert )->b = 0.0f;

			*( powerbuf + srcvert ) = 0.0f;			

		}else{
			( diffusebuf + srcvert )->r = 1.0f;
			( diffusebuf + srcvert )->g = 1.0f;
			( diffusebuf + srcvert )->b = 1.0f;

			( ambientbuf + srcvert )->r = 0.25f;
			( ambientbuf + srcvert )->g = 0.25f;
			( ambientbuf + srcvert )->b = 0.25f;

			( specularbuf + srcvert )->r = 0.0f;
			( specularbuf + srcvert )->g = 0.0f;
			( specularbuf + srcvert )->b = 0.0f;

			( emissivebuf + srcvert )->r = 0.0f;
			( emissivebuf + srcvert )->g = 0.0f;
			( emissivebuf + srcvert )->b = 0.0f;

			*( powerbuf + srcvert ) = 0.0f;
		}
	}else{
		int infno2;
		INFELEM* curIE2;
		int findsym = 0;
		for( infno2 = 0; infno2 < curie->infnum; infno2++ ){
			curIE2 = curie->ie + infno2;
			
			if( curIE2->kind == CALCMODE_SYM ){
				findsym = 1;
				break;
			}
		}

		if( findsym == 1 ){
			float difr, difg, difb;
			difr = (float)infcol[CALCMODE_SYM].r / 255.0f;
			difr = min( 1.0f, difr );
			difr = max( 0.0f, difr );
			difg = (float)infcol[CALCMODE_SYM].g / 255.0f;
			difg = min( 1.0f, difg );
			difg = max( 0.0f, difg );
			difb = (float)infcol[CALCMODE_SYM].b / 255.0f;
			difb = min( 1.0f, difb );
			difb = max( 0.0f, difb );
			( diffusebuf + srcvert )->r = difr;
			( diffusebuf + srcvert )->g = difg;
			( diffusebuf + srcvert )->b = difb;

			float ambr, ambg, ambb;
			ambr = difr * 0.25f;
			ambg = difg * 0.25f;
			ambb = difb * 0.25f;
			( ambientbuf + srcvert )->r = ambr;
			( ambientbuf + srcvert )->g = ambg;
			( ambientbuf + srcvert )->b = ambb;

/***
			( diffusebuf + srcvert )->r = 0.0f;
			( diffusebuf + srcvert )->g = 1.0f;
			( diffusebuf + srcvert )->b = 1.0f;

			( ambientbuf + srcvert )->r = 0.0f;
			( ambientbuf + srcvert )->g = 0.25f;
			( ambientbuf + srcvert )->b = 0.25f;
***/
			( specularbuf + srcvert )->r = 0.0f;
			( specularbuf + srcvert )->g = 0.0f;
			( specularbuf + srcvert )->b = 0.0f;

			( emissivebuf + srcvert )->r = 0.0f;
			( emissivebuf + srcvert )->g = 0.0f;
			( emissivebuf + srcvert )->b = 0.0f;

			*( powerbuf + srcvert ) = 0.0f;			

		}else{
			( diffusebuf + srcvert )->r = 1.0f;
			( diffusebuf + srcvert )->g = 1.0f;
			( diffusebuf + srcvert )->b = 1.0f;

			( ambientbuf + srcvert )->r = 0.25f;
			( ambientbuf + srcvert )->g = 0.25f;
			( ambientbuf + srcvert )->b = 0.25f;

			( specularbuf + srcvert )->r = 0.0f;
			( specularbuf + srcvert )->g = 0.0f;
			( specularbuf + srcvert )->b = 0.0f;

			( emissivebuf + srcvert )->r = 0.0f;
			( emissivebuf + srcvert )->g = 0.0f;
			( emissivebuf + srcvert )->b = 0.0f;

			*( powerbuf + srcvert ) = 0.0f;
		}
	}

	//ひとつのボーンの影響も受けていない頂点は、黒
	if( (curie->infnum == 1) && (curie->ie->bonematno <= 0) ){
		( diffusebuf + srcvert )->r = 0.0f;
		( diffusebuf + srcvert )->g = 0.0f;
		( diffusebuf + srcvert )->b = 0.0f;

		( ambientbuf + srcvert )->r = 0.0f;
		( ambientbuf + srcvert )->g = 0.0f;
		( ambientbuf + srcvert )->b = 0.0f;

		( specularbuf + srcvert )->r = 0.0f;
		( specularbuf + srcvert )->g = 0.0f;
		( specularbuf + srcvert )->b = 0.0f;

		( emissivebuf + srcvert )->r = 0.0f;
		( emissivebuf + srcvert )->g = 0.0f;
		( emissivebuf + srcvert )->b = 0.0f;

		*( powerbuf + srcvert ) = 0.0f;
	}


	return 0;
}

int CPolyMesh::GetNearVert( D3DXVECTOR3 findpos, float symdist, int* findvert, float* finddist )
{
	*findvert = -1;
	*finddist = 1e8;

	int vno;
	VEC3F* curp;
	D3DXVECTOR3 diffv;
	for( vno = 0; vno < meshinfo->m; vno++ ){
		curp = pointbuf + vno;

		diffv.x = curp->x - findpos.x;
		diffv.y = curp->y - findpos.y;
		diffv.z = curp->z - findpos.z;

		float tempdist;
		tempdist = D3DXVec3Length( &diffv );
		if( (tempdist <= symdist) && (tempdist < *finddist) ){
			*findvert = vno;
			*finddist = tempdist;
		}
	}

	return 0;
}

/***
int CPolyMesh::ConvColor2XMaterial( float srcalpha, char* srctexname, D3DXMATERIAL** ppmat, DWORD** ppattr, int* matnumptr )
{
	//int ret;

	if( !diffusebuf ){
		DbgOut( "pm : ConvColor2XMaterial : diffusebuf not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	*ppmat = 0;
	*ppattr = 0;
	*matnumptr = 0;

/////////
	D3DXMATERIAL* mathead = 0;
	int matnum = 0;
	DWORD* attrib = 0;


/////////
	attrib = new DWORD[ meshinfo->n ];
	if( !attrib ){
		DbgOut( "pm : ConvColor2XMaterial : ppattr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( attrib, sizeof( DWORD ) * meshinfo->n );


	int topindex;
	ARGBF* curdiff;
	ARGBF* curamb;
	ARGBF* curspec;
	float curpow;
	ARGBF* curemi;
	D3DMATERIAL9 curmat;
	int faceno;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		topindex = *(indexbuf + faceno * 3);
		curdiff = diffusebuf + topindex;
		curamb = ambientbuf + topindex;
		curspec = specularbuf + topindex;
		curpow = *(powerbuf + topindex);
		curemi = emissivebuf + topindex;
	/////////
		
		curmat.Diffuse.r = curdiff->r;
		curmat.Diffuse.g = curdiff->g;
		curmat.Diffuse.b = curdiff->b;
		curmat.Diffuse.a = srcalpha;

		curmat.Ambient.r = curamb->r;
		curmat.Ambient.g = curamb->g;
		curmat.Ambient.b = curamb->b;
		curmat.Ambient.a = srcalpha;

		curmat.Specular.r = curspec->r;
		curmat.Specular.g = curspec->g;
		curmat.Specular.b = curspec->b;
		curmat.Specular.a = srcalpha;

		curmat.Emissive.r = curemi->r;
		curmat.Emissive.g = curemi->g;
		curmat.Emissive.b = curemi->b;
		curmat.Emissive.a = srcalpha;

		curmat.Power = curpow;
////////////
		D3DXMATERIAL* curxmat;
		int findmatno = -1;
		int issame;
		int matno;
		for( matno = 0; matno < matnum; matno++ ){
			curxmat = mathead + matno;
			issame = IsSameMaterial( &(curxmat->MatD3D), &curmat );
			if( issame ){
				findmatno = matno;
				break;
			}
		}
//////////////
		if( findmatno >= 0 ){
			//すでに、同じmaterialがあった場合
			*( attrib + faceno ) = findmatno;
		}else{
			//同じマテリアルが無かった場合。

			matnum++;
			mathead = (D3DXMATERIAL*)realloc( mathead, sizeof( D3DXMATERIAL ) * matnum );
			if( !mathead ){
				DbgOut( "pm : ConvColor2XMaterial : mathead alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			
			( mathead + matnum - 1 )->MatD3D = curmat;

			if( srctexname && *srctexname ){
				int tnameleng;
				tnameleng = (int)strlen( srctexname );
				char* tnameptr;
				tnameptr = new CHAR[ tnameleng + 1 ];
				if( !tnameptr ){
					DbgOut( "pm : ConvColor2XMaterial : tnameptr alloc error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				strcpy_s( tnameptr, tnameleng + 1, srctexname );
				(mathead + matnum - 1)->pTextureFilename = tnameptr;
			}else{
				(mathead + matnum - 1)->pTextureFilename = NULL;
			}

			*( attrib + faceno ) = matnum - 1;

		}
	}

	if( mathead ){
		//materialは、deleteで削除されるので、reallocでアロケートしたマテリアルを、作成しなおす。

		*ppmat = new D3DXMATERIAL[ matnum ];
		if( !*ppmat ){
			DbgOut( "pm : ConvColor2XMaterial : ppmat alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		MoveMemory( (void*)*ppmat, (void*)mathead, sizeof( D3DXMATERIAL ) * matnum );
		free( mathead );//!!!!!!!!!!!!!!
	}
	*ppattr = attrib;
	*matnumptr = matnum;


	return 0;
}
***/

int CPolyMesh::ConvColor2XMaterial( CMQOMaterial* mqomat, D3DXMATERIAL** ppmat, DWORD** ppattr, int* matnumptr )
{
	//int ret;

	if( !diffusebuf ){
		DbgOut( "pm : ConvColor2XMaterial : diffusebuf not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	*ppmat = 0;
	*ppattr = 0;
	*matnumptr = 0;

/////////
	D3DXMATERIAL* mathead = 0;
	int matnum = 0;
	DWORD* attrib = 0;


/////////
	attrib = new DWORD[ meshinfo->n ];
	if( !attrib ){
		DbgOut( "pm : ConvColor2XMaterial : ppattr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( attrib, sizeof( DWORD ) * meshinfo->n );


	int topindex;
	int curmqomatno;
	CMQOMaterial* curmqomat;
	float curalpha;
	char* curtexname;
	ARGBF* curdiff;
	ARGBF* curamb;
	ARGBF* curspec;
	float curpow;
	ARGBF* curemi;
	D3DMATERIAL9 curmat;
	int faceno;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		curmqomatno = *( m_attrib0 + faceno );
		curmqomat = GetMaterialFromNo( mqomat, curmqomatno );
		if( !curmqomat ){
			DbgOut( "pm : ConvColor2XMaterial : curmqomat NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curalpha = curmqomat->col.a;
		curtexname = curmqomat->tex;

		topindex = *(indexbuf + faceno * 3);
		curdiff = diffusebuf + topindex;
		curamb = ambientbuf + topindex;
		curspec = specularbuf + topindex;
		curpow = *(powerbuf + topindex);
		curemi = emissivebuf + topindex;
	/////////
		
		curmat.Diffuse.r = curdiff->r;
		curmat.Diffuse.g = curdiff->g;
		curmat.Diffuse.b = curdiff->b;
		curmat.Diffuse.a = curalpha;

		curmat.Ambient.r = curamb->r;
		curmat.Ambient.g = curamb->g;
		curmat.Ambient.b = curamb->b;
		curmat.Ambient.a = curalpha;

		curmat.Specular.r = curspec->r;
		curmat.Specular.g = curspec->g;
		curmat.Specular.b = curspec->b;
		curmat.Specular.a = curalpha;

		curmat.Emissive.r = curemi->r;
		curmat.Emissive.g = curemi->g;
		curmat.Emissive.b = curemi->b;
		curmat.Emissive.a = curalpha;

		curmat.Power = curpow;
////////////
		D3DXMATERIAL* curxmat;
		int findmatno = -1;
		int issame;
		int matno;
		for( matno = 0; matno < matnum; matno++ ){
			curxmat = mathead + matno;
			//issame = IsSameMaterial( &(curxmat->MatD3D), &curmat );
			issame = IsSameXMaterial( curxmat, &curmat, curtexname );
			if( issame ){
				findmatno = matno;
				break;
			}
		}
//////////////
		if( findmatno >= 0 ){
			//すでに、同じmaterialがあった場合
			*( attrib + faceno ) = findmatno;
		}else{
			//同じマテリアルが無かった場合。

			matnum++;
			mathead = (D3DXMATERIAL*)realloc( mathead, sizeof( D3DXMATERIAL ) * matnum );
			if( !mathead ){
				DbgOut( "pm : ConvColor2XMaterial : mathead alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			
			( mathead + matnum - 1 )->MatD3D = curmat;

			if( *curtexname ){
				int tnameleng;
				tnameleng = (int)strlen( curtexname );
				char* tnameptr;
				tnameptr = new CHAR[ tnameleng + 1 ];
				if( !tnameptr ){
					DbgOut( "pm : ConvColor2XMaterial : tnameptr alloc error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				strcpy_s( tnameptr, tnameleng + 1, curtexname );
				(mathead + matnum - 1)->pTextureFilename = tnameptr;
			}else{
				(mathead + matnum - 1)->pTextureFilename = NULL;
			}

			*( attrib + faceno ) = matnum - 1;

		}
	}

	if( mathead ){
		//materialは、deleteで削除されるので、reallocでアロケートしたマテリアルを、作成しなおす。

		*ppmat = new D3DXMATERIAL[ matnum ];
		if( !*ppmat ){
			DbgOut( "pm : ConvColor2XMaterial : ppmat alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		MoveMemory( (void*)*ppmat, (void*)mathead, sizeof( D3DXMATERIAL ) * matnum );
		free( mathead );//!!!!!!!!!!!!!!
	}
	*ppattr = attrib;
	*matnumptr = matnum;


	return 0;
}

int CPolyMesh::IsSameD3DColorValue( D3DCOLORVALUE* col1, D3DCOLORVALUE* col2 )
{
	if( col1->r != col2->r )
		return 0;

	if( col1->g != col2->g )
		return 0;

	if( col1->b != col2->b )
		return 0;

	if( col1->a != col2->a )
		return 0;

	return 1;
}

int CPolyMesh::IsSameMaterial( D3DMATERIAL9* mat1, D3DMATERIAL9* mat2 )
{
	int issame;

	issame = IsSameD3DColorValue( &(mat1->Diffuse), &(mat2->Diffuse) );
	if( issame == 0 )
		return 0;

	issame = IsSameD3DColorValue( &(mat1->Ambient), &(mat2->Ambient) );
	if( issame == 0 )
		return 0;

	issame = IsSameD3DColorValue( &(mat1->Specular), &(mat2->Specular) );
	if( issame == 0 )
		return 0;

	issame = IsSameD3DColorValue( &(mat1->Emissive), &(mat2->Emissive) );
	if( issame == 0 )
		return 0;

	if( mat1->Power != mat2->Power )
		return 0;

	return 1;

}

int CPolyMesh::IsSameXMaterial( D3DXMATERIAL* xmat1, D3DMATERIAL9* mat2, char* texname2 )
{
	int issame;

	int cmp0;
	if( xmat1->pTextureFilename ){
		if( !texname2 || (*texname2 == 0) ){
			return 0;
		}else{
			cmp0 = strcmp( xmat1->pTextureFilename, texname2 );
			if( cmp0 != 0 )
				return 0;
		}

	}else{
		if( *texname2 != 0 )
			return 0;
	}


	issame = IsSameD3DColorValue( &(xmat1->MatD3D.Diffuse), &(mat2->Diffuse) );
	if( issame == 0 )
		return 0;

	issame = IsSameD3DColorValue( &(xmat1->MatD3D.Ambient), &(mat2->Ambient) );
	if( issame == 0 )
		return 0;

	issame = IsSameD3DColorValue( &(xmat1->MatD3D.Specular), &(mat2->Specular) );
	if( issame == 0 )
		return 0;

	issame = IsSameD3DColorValue( &(xmat1->MatD3D.Emissive), &(mat2->Emissive) );
	if( issame == 0 )
		return 0;

	if( xmat1->MatD3D.Power != mat2->Power )
		return 0;

	return 1;
}



int CPolyMesh::CreateToon1Buffer( LPDIRECT3DDEVICE9 pdev, D3DXMATERIAL* pmat, DWORD* pattrib, int matnum, CMQOMaterial* mqomat )
{
	int ret;
	DestroyToon1Buffer();

	if( !pmat || !pattrib || !matnum ){
		DbgOut( "pm : CreateToon1Buffer : parameter error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_material = pmat;
	m_attrib = pattrib;
	m_materialnum = matnum;

	if( !indexbuf || !pointbuf ){
		DbgOut( "pm : CreateToon1Buffer : buffer not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	m_indexbuf2 = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !m_indexbuf2 ){
		DbgOut( "pm : CreateToon1Buffer : indexbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_indexbuf2, sizeof( int ) * meshinfo->n * 3 );

	m_materialblock = (MATERIALBLOCK*)malloc( sizeof( MATERIALBLOCK ) * matnum );
	if( !m_materialblock ){
		DbgOut( "pm : CreateToon1Buffer : materialblock alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_materialblock, sizeof( MATERIALBLOCK ) * matnum );

	m_toon1 = new CToon1Params[ matnum ];
	if( !m_toon1 ){
		DbgOut( "pm : CreateToon1Buffer : toon1params alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_toonface2oldface = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !m_toonface2oldface ){
		DbgOut( "pm : CreateToon1Buffer : toonface2oldface alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_toonface2oldface, sizeof( int ) * meshinfo->n * 3 );

//////////
	int matno;
	int faceno;
	int setfaceno = 0;
	int isfirst;
	for( matno = 0; matno < matnum; matno++ ){
		
		isfirst = 1;
		(m_materialblock + matno)->materialno = matno;

		for( faceno = 0; faceno < meshinfo->n; faceno++ ){
			DWORD curattrib;
			curattrib = *(m_attrib + faceno);
			int curattrib0;
			curattrib0 = *(m_attrib0 + faceno);
			CMQOMaterial* curmqomat;
			curmqomat = GetMaterialFromNo( mqomat, curattrib0 );
			if( !curmqomat ){
				DbgOut( "pm2 : CreateToon1Buffer : curmqomat NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			if( (int)curattrib == matno ){
				*(m_indexbuf2 + setfaceno * 3) = *(indexbuf + faceno * 3);
				*(m_indexbuf2 + setfaceno * 3 + 1) = *(indexbuf + faceno * 3 + 1);
				*(m_indexbuf2 + setfaceno * 3 + 2) = *(indexbuf + faceno * 3 + 2);

				*(m_toonface2oldface + setfaceno) = faceno;//!!!!!!!!

				if( isfirst == 1 ){
					(m_materialblock + matno)->startface = setfaceno;
					(m_materialblock + matno)->mqomat = curmqomat;
					isfirst = 0;
				}

				setfaceno++;
			}
		}
		(m_materialblock + matno)->endface = setfaceno;//次の最初の番号
	}

	if( setfaceno != meshinfo->n ){
		DbgOut( "pm : CreateToon1Buffer : setfaceno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//// texture
	if( pdev ){
		int texno;
		for( texno = 0; texno < m_materialnum; texno++ ){

			D3DCOLORVALUE curdiffuse;
			curdiffuse = (m_material + texno)->MatD3D.Diffuse;

			ret = (m_toon1 + texno)->SetDiffuse( curdiffuse, 1 );
			_ASSERT( !ret );

			ret = (m_toon1 + texno)->CreateTexture( pdev );
			if( ret ){
				DbgOut( "pm2 : CreateToon1Buffer : toon1 CreateTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
	//DbgOut( "pm : check matd3d : texno %d : diffuse %f, %f, %f, %f\r\n", texno, curdiffuse.a, curdiffuse.r, curdiffuse.g, curdiffuse.b );
		}
	}

	return 0;
}


int CPolyMesh::CreateToon1Buffer2( LPDIRECT3DDEVICE9 pdev, CMQOMaterial* mqomat )
{
	int ret;
	int matnum = m_materialnum;

//free
	if( m_indexbuf2 ){
		free( m_indexbuf2 );
		m_indexbuf2 = 0;
	}
	if( m_materialblock ){
		free( m_materialblock );
		m_materialblock = 0;
	}

	if( m_toonface2oldface ){
		free( m_toonface2oldface );
		m_toonface2oldface = 0;
	}

	if( !indexbuf || !pointbuf ){
		DbgOut( "pm : CreateToon1Buffer2 : buffer not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !m_attrib || !m_toon1 ){
		DbgOut( "pm : CreateToon1Buffer2 : toon1 buf not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



/////////
	m_indexbuf2 = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !m_indexbuf2 ){
		DbgOut( "pm : CreateToon1Buffer : indexbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_indexbuf2, sizeof( int ) * meshinfo->n * 3 );

	m_materialblock = (MATERIALBLOCK*)malloc( sizeof( MATERIALBLOCK ) * matnum );
	if( !m_materialblock ){
		DbgOut( "pm : CreateToon1Buffer : materialblock alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_materialblock, sizeof( MATERIALBLOCK ) * matnum );

	m_toonface2oldface = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !m_toonface2oldface ){
		DbgOut( "pm : CreateToon1buffer2 : toonface2oldface alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_toonface2oldface, sizeof( int ) * meshinfo->n * 3 );

//////////

	int matno;
	int faceno;
	int setfaceno = 0;
	int isfirst;
	for( matno = 0; matno < matnum; matno++ ){
		
		isfirst = 1;
		(m_materialblock + matno)->materialno = matno;

		for( faceno = 0; faceno < meshinfo->n; faceno++ ){
			DWORD curattrib;
			curattrib = *(m_attrib + faceno);
			int curattrib0;
			curattrib0 = *(m_attrib0 + faceno);
			CMQOMaterial* curmqomat;
			curmqomat = GetMaterialFromNo( mqomat, curattrib0 );
			if( !curmqomat ){
				DbgOut( "pm2 : CreateToon1Buffer : curmqomat NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			if( (int)curattrib == matno ){
				*(m_indexbuf2 + setfaceno * 3) = *(indexbuf + faceno * 3);
				*(m_indexbuf2 + setfaceno * 3 + 1) = *(indexbuf + faceno * 3 + 1);
				*(m_indexbuf2 + setfaceno * 3 + 2) = *(indexbuf + faceno * 3 + 2);

				*(m_toonface2oldface + setfaceno) = faceno;

				if( isfirst == 1 ){
					(m_materialblock + matno)->startface = setfaceno;
					(m_materialblock + matno)->mqomat = curmqomat;
					isfirst = 0;
				}

				setfaceno++;
			}
		}
		(m_materialblock + matno)->endface = setfaceno;//次の最初の番号
	}

	if( setfaceno != meshinfo->n ){
		DbgOut( "pm : CreateToon1Buffer2 : setfaceno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//// texture
	int texno;
	for( texno = 0; texno < m_materialnum; texno++ ){

		ret = (m_toon1 + texno)->CreateTexture( pdev );
		if( ret ){
			DbgOut( "pm2 : CreateToon1Buffer2 : toon1 CreateTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
//DbgOut( "pm : check matd3d : texno %d : diffuse %f, %f, %f, %f\r\n", texno, curdiffuse.a, curdiffuse.r, curdiffuse.g, curdiffuse.b );
	}

	return 0;
}

int CPolyMesh::GetInfluenceVertNo( int srcjointno, int* vertarray, int* vertnum )
{

	*vertnum = 0;//!!!!!!!!!!

	if( !m_IE ){
		DbgOut( "pm : GetInfluenceVertNo : m_IE NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int hasinf;
	int vno;
	CInfElem* curie;
	for( vno = 0; vno < meshinfo->m; vno++ ){
		curie = m_IE + vno;

		hasinf = 0;
		hasinf = curie->CheckInfluence( srcjointno, 0.04f );

		if( hasinf != 0 ){
			(*vertnum)++;

			if( vertarray ){
				*( vertarray + *vertnum - 1 ) = vno;
			}
		}
	}

	return 0;
}

int CPolyMesh::CopyNearestInfElem( CPolyMesh* srcpm, SERICONV* sericonv, int serinum )
{
	int ret;
	
	//dstが自分、srcが相手。

	int dstvno, dstvnum;
	dstvnum = meshinfo->m;

	int srcvnum;
	srcvnum = srcpm->meshinfo->m;

	for( dstvno = 0; dstvno < dstvnum; dstvno++ ){
		VEC3F* dstv;
		dstv = pointbuf + dstvno;
		D3DXVECTOR3 findpos;
		findpos.x = dstv->x;
		findpos.y = dstv->y;
		findpos.z = dstv->z;

		int nearv = 0;
		float neardist = 1e8;
		ret = srcpm->GetNearVert( findpos, 100.0f, &nearv, &neardist );
		if( ret ){
			DbgOut( "pm : CopyNearestInfElem : GetNearVert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		if( (nearv >= 0) && (nearv < srcvnum) ){
			CInfElem* srcie;
			srcie = srcpm->m_IE + nearv;
			_ASSERT( srcie );

			CInfElem* dstie;
			dstie = m_IE + dstvno;
			_ASSERT( dstie );

			ret = dstie->CopyIE( srcie, sericonv, serinum );
			if( ret ){
				DbgOut( "pm : CopyNearestInfElem : ie CopyIE error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CPolyMesh::CreateSortTexBuffer( CMQOMaterial* mathead )
{
	//int* m_attrib0;//meshinfo->n rok読み込み時にsh->m_matheadのマテリアル番号を面ごとに保持する。
	//SORTTEXTURE* m_sorttex;// meshinfo->n 面ごとのテクスチャ名、テクスチャ名でソートする
	//int* m_optindexbuftex;// meshinfo->n * 3, sorttexの順番で並び替えたインデックス
	//int m_texblocknum;
	//TEXTUREBLOCK* m_texblock;// texblocknum個のレンダー単位

	DestroySortTexBuffer();

	int facenum = meshinfo->n;
	
	m_sorttex = (SORTTEXTURE*)malloc( sizeof( SORTTEXTURE ) * facenum );
	if( !m_sorttex ){
		DbgOut( "pm : CreateSortTexBuffer : m_sorttex alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_sorttex, sizeof( SORTTEXTURE ) * facenum );

	m_optindexbuftex = (int*)malloc( sizeof( int ) * facenum * 3 );
	if( !m_optindexbuftex ){
		DbgOut( "pm : CreateSortTexBuffer : m_optindexbuftex alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_texblock = 0;

	int faceno;
	for( faceno = 0; faceno < facenum; faceno++ ){
		SORTTEXTURE* curst = m_sorttex + faceno;

		_ASSERT( m_attrib0 );
		int curmatno = *( m_attrib0 + faceno );

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( mathead, curmatno );
		if( !curmat ){
			DbgOut( "pm : CreateSortTexBuffer : curmat NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		curst->mqomat = curmat;
		curst->faceno = faceno;
	}

	qsort_s( m_sorttex, facenum, sizeof( SORTTEXTURE ), compareTex, (void*)mathead );


	m_texblocknum = 0;//!!!!!!!!!
	int isfirst = 1;
	int befmatno = -2;
	for( faceno = 0; faceno < facenum; faceno++ ){
		SORTTEXTURE* curst = m_sorttex + faceno;

		int curfaceno;
		curfaceno = curst->faceno;

		*( m_optindexbuftex + faceno * 3 ) = *( indexbuf + curfaceno * 3 );
		*( m_optindexbuftex + faceno * 3 + 1 ) = *( indexbuf + curfaceno * 3 + 1 );
		*( m_optindexbuftex + faceno * 3 + 2 ) = *( indexbuf + curfaceno * 3 + 2 );


///////////// 以下を変更するときは、この直下のブロックのfor( faceno = ...も同様に変更する。
		int cmp = 1;
		cmp = befmatno - curst->mqomat->materialno;
		if( (isfirst == 1) || (cmp != 0) ){
			m_texblocknum++;
		}

		befmatno = curst->mqomat->materialno;

		isfirst = 0;	
	}
	
	m_texblock = (TEXTUREBLOCK*)malloc( sizeof( TEXTUREBLOCK ) * m_texblocknum );
	if( !m_texblock ){
		DbgOut( "pm : CreateSortTexBuffer : m_texblock alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_texblock, sizeof( TEXTUREBLOCK ) * m_texblocknum );
	m_texblock->mqomat = m_sorttex->mqomat;//!!!!!!
	m_texblock->startface = 0;//!!!!!!
	( m_texblock + m_texblocknum - 1 )->endface = facenum;//!!!!!

	int setno = 0;
	isfirst = 1;
	befmatno = m_sorttex->mqomat->materialno;
	int startface = 0;
	int endface = 0;
	for( faceno = 0; faceno < facenum; faceno++ ){
		SORTTEXTURE* curst = m_sorttex + faceno;

		int cmp = 1;
		cmp = befmatno - curst->mqomat->materialno;		
		if( cmp != 0 ){
			( m_texblock + setno )->endface = faceno;

			( m_texblock + setno + 1 )->mqomat = curst->mqomat;
			( m_texblock + setno + 1 )->startface = faceno;

			if( faceno == (facenum - 1) ){
				( m_texblock + setno + 1 )->endface = faceno + 1;
			}
			setno++;
		}

		befmatno = curst->mqomat->materialno;
		
		isfirst = 0;	
	}

	if( setno != (m_texblocknum - 1) ){
		DbgOut( "pm : CreateSortTexBuffer : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/***
	//!!!!for debug
	DbgOut( "pm : CreateSortTexBuffer\r\n!!!!check textureblock  facenum %d!!!!\r\n", facenum );
	int tbno;
	for( tbno = 0; tbno < m_texblocknum; tbno++ ){
		TEXTUREBLOCK* curtb;
		curtb = m_texblock + tbno;

		DbgOut( "materialno %d, texname %s, startface %d, endface %d\r\n", curtb->mqomat->materialno, curtb->mqomat->tex, curtb->startface, curtb->endface );
	}
	***/

	m_dirtymat = (DIRTYMAT*)malloc( sizeof( DIRTYMAT ) * m_texblocknum );
	if( !m_dirtymat ){
		DbgOut( "pm : CreateSortTexBuf : dirtymat alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_dirtymat, sizeof( DIRTYMAT ) * m_texblocknum );

	int dmno;
	for( dmno = 0; dmno < m_texblocknum; dmno++ ){
		TEXTUREBLOCK* curtb;
		curtb = m_texblock + dmno;
		DIRTYMAT* curdm;
		curdm = m_dirtymat + dmno;

		curdm->materialno = curtb->mqomat->materialno;

		int* dirtyflag;
		dirtyflag = (int*)malloc( sizeof( int ) * meshinfo->m );
		if( !dirtyflag ){
			DbgOut( "pm : CreateSortTexBuf : dirtyflag alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( dirtyflag, sizeof( int ) * meshinfo->m );

		int fno;
		for( fno = 0; fno < meshinfo->n; fno++ ){
			int curattr;
			curattr = *( m_attrib0 + fno );
			if( curattr == curdm->materialno ){
				int p1, p2, p3;
				p1 = *( indexbuf + fno * 3 );
				p2 = *( indexbuf + fno * 3 + 1 );
				p3 = *( indexbuf + fno * 3 + 2 );

				*( dirtyflag + p1 ) = 1;
				*( dirtyflag + p2 ) = 1;
				*( dirtyflag + p3 ) = 1;
			}
		}

		curdm->dirtyflag = dirtyflag;
	}

	return 0;
}

int compareTex( void *context, const void *elem1, const void *elem2)
{
	SORTTEXTURE* sot1;
	SORTTEXTURE* sot2;
	sot1 = (SORTTEXTURE*)elem1;
	sot2 = (SORTTEXTURE*)elem2;

	CMQOMaterial* mat1 = sot1->mqomat;
	CMQOMaterial* mat2 = sot2->mqomat;

	CMQOMaterial* mathead;
	mathead = (CMQOMaterial*)context;


	return ( mat1->materialno - mat2->materialno );
}


CMQOMaterial* CPolyMesh::GetMaterialFromNo( CMQOMaterial* mathead, int matno )
{
	CMQOMaterial* retmat = 0;
	CMQOMaterial* chkmat = mathead;

	while( chkmat ){
		if( chkmat->materialno == matno ){
			retmat = chkmat;
			break;
		}
		chkmat = chkmat->next;
	}
	
	return retmat;
}

int CPolyMesh::ChkAlphaNum( CMQOMaterial* mathead )
{
	_ASSERT( m_attrib0 );

	chkalpha.alphanum = 0;
	chkalpha.notalphanum = 0;


	int faceno;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		int curmatno = *( m_attrib0 + faceno );
		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( mathead, curmatno );
		if( !curmat ){
			DbgOut( "pm : ChkAlphaNum : curmat NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( (curmat->col.a != 1.0f) || (curmat->transparent != 0) ){
			chkalpha.alphanum++;
		}else{
			chkalpha.notalphanum++;
		}
	}

	return 0;
}

int* CPolyMesh::GetDirtyMaterial( int matno )
{
	if( !m_dirtymat ){
		_ASSERT( 0 );
		return 0;
	}

	int* retdirty = 0;

	int dmno;
	for( dmno = 0; dmno < m_texblocknum; dmno++ ){
		DIRTYMAT* curdm;
		curdm = m_dirtymat + dmno;
		_ASSERT( curdm );

		if( curdm->materialno == matno ){
			retdirty = curdm->dirtyflag;
			break;
		}
	}
	return retdirty;
}

int CPolyMesh::SetInfCol( E3DCOLOR3UC* srcinfcol )
{
	if( !srcinfcol ){
		return 0;
	}
	MoveMemory( infcol, srcinfcol, sizeof( E3DCOLOR3UC ) * CALCMODE_MAX );
	return 0;
}

int CPolyMesh::MakeColor( float minval, float maxval, float val, E3DCOLOR3UC target_col, RDBColor3f* dstcol )
{
    //min = 比重の最小値
    //max = 比重の最大値
    //val = 比重の値
	float rate = (1.0f - ((val - minval) / (maxval - minval))) * 0.8F;

	float tmpr, tmpg, tmpb;
	tmpr = ( (float)target_col.r + ((255.0f - (float)target_col.r) * rate ) ) / 255.0f;
	tmpr = min( 1.0f, tmpr );
	tmpr = max( 0.0f, tmpr );

	tmpg = ( (float)target_col.g + ((255.0f - (float)target_col.g) * rate ) ) / 255.0f;
	tmpg = min( 1.0f, tmpg );
	tmpg = max( 0.0f, tmpg );

	tmpb = ( (float)target_col.b + ((255.0f - (float)target_col.b) * rate ) ) / 255.0f;
	tmpb = min( 1.0f, tmpb );
	tmpb = max( 0.0f, tmpb );

	dstcol->r = tmpr;
	dstcol->g = tmpg;
	dstcol->b = tmpb;

	return 0;
}

int CPolyMesh::CloneDispObj( CPolyMesh* srcpm, float srcalpha )
{
	int ret;
	ret = CreatePointBuf( srcpm->meshinfo, srcalpha );
	if( ret ){
		DbgOut( "pm : CloneDispObj : CreatePointBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int pointbufleng;
	pointbufleng = meshinfo->m;
	MoveMemory( pointbuf, srcpm->pointbuf, sizeof( VEC3F ) * pointbufleng );
	MoveMemory( orgnobuf, srcpm->orgnobuf, sizeof( int ) * pointbufleng );
	MoveMemory( diffusebuf, srcpm->diffusebuf, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( ambientbuf, srcpm->ambientbuf, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( specularbuf, srcpm->specularbuf, sizeof( ARGBF ) * pointbufleng );
	MoveMemory( powerbuf, srcpm->powerbuf, sizeof( float ) * pointbufleng );
	MoveMemory( emissivebuf, srcpm->emissivebuf, sizeof( ARGBF ) * pointbufleng );
////////////
	if( srcpm->indexbuf ){
		ret = CreateIndexBuf( srcpm->meshinfo->n );
		if( ret ){
			DbgOut( "pm : CloneDispObj : CreateIndexBuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int ibleng;
		ibleng = meshinfo->n * 3;
		MoveMemory( indexbuf, srcpm->indexbuf, sizeof( int ) * ibleng );
		MoveMemory( m_attrib0, srcpm->m_attrib0, sizeof( int ) * meshinfo->n );
	}
////////////
	if( srcpm->uvbuf ){
		ret = CreateTextureBuffer();
		if( ret ){
			DbgOut( "pm : CloneDispObj : CreateTextureBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int vertnum = meshinfo->m;
		MoveMemory( uvbuf, srcpm->uvbuf, sizeof( COORDINATE ) * vertnum );
		MoveMemory( texkindbuf, srcpm->texkindbuf, sizeof( int ) * vertnum );
	}

////////////
	groundflag = srcpm->groundflag;
	billboardflag = srcpm->billboardflag;

////////////

	ret = CreateInfElemIfNot( meshinfo->m );
	if( ret ){
		DbgOut( "pm : CloneDispObj : CreateInfElemIfNot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int infno;
	for( infno = 0; infno < meshinfo->m; infno++ ){
		CInfElem* srcie = srcpm->m_IE + infno;
		CInfElem* dstie = m_IE + infno;
		ret = dstie->CopyIE( srcie, 0, 0 );
		if( ret ){
			DbgOut( "pm : CloneDispObj : ie CopyIE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}