#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <polymesh2.h>
#include <mqomaterial.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <d3ddisp.h>

#include <shdhandler.h>
#include <shdelem.h>

#include <quaternion.h>
#include <Toon1Params.h>

#ifndef LINECHARLENG
#define LINECHARLENG 2048
#endif

extern int g_index32bit;

static int compareTex( void *context, const void *elem1, const void *elem2);


CPolyMesh2::CPolyMesh2()
{
	InitParams();

}

CPolyMesh2::~CPolyMesh2()
{
	DestroyObjs();
}

void CPolyMesh2::InitParams()
{
	CBaseDat::InitParams();
	
	meshinfo = 0;
	pointbuf = 0;

	orgnormal = 0;
	smoothbuf = 0;

	samepointbuf = 0;

	uvbuf = 0;
	diffusebuf = 0;
	ambientbuf = 0;
	specularbuf = 0;
	powerbuf = 0;
	emissivebuf = 0;
	diffbuf = 0;

	m_attrib0 = 0;

	dirtypoint = 0;

	colorbuf = 0;


	optflag = 0;
	optpleng = 0;
	opttlv = 0;//leng : optpleng
	optambient = 0;//leng : optpleng
	optcolorbuf = 0;//leng : optpleng
	optpowerbuf = 0;//leng : optpleng
	optemissivebuf = 0;//leng : optpleng

	oldpno2optpno = 0;
	optindexbuf = 0;

	optpno2oldpno = 0;
	optsamepointbuf = 0;

	groundflag = 0;
	adjustuvflag = 0;

	orgnobuf = 0;	
	
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

	spec.r = 0.1f;
	spec.g = 0.1f;
	spec.b = 0.1f;
	spec.a = 1.0f;

	emi.r = 0.0f;
	emi.g = 0.0f;
	emi.b = 0.0f;
	emi.a = 1.0f;


	m_IE = 0;

	createoptflag = 0;

	m_center.x = 0.0f;
	m_center.y = 0.0f;
	m_center.z = 0.0f;

	m_anchormaterialno = 0;
	m_mikoapplychild = 0;
	m_mikotarget = 0;

	tempapplychild = 0;
	temptarget = 0;

	tempdiffuse = 0;
	tempspecular = 0;
	tempambient = 0;
	tempemissive = 0;
	temppower = 0;

	m_loadorgnormalflag = 0;

	vcoldata = 0;
	vcolnum = 0;

	m_material = 0;
	m_materialnum = 0;
	m_attrib = 0;
	m_optindexbuf2 = 0;
	m_optindexbuf2m = 0;
	m_materialblock = 0;
	//m_toon1tex = 0;
	m_toon1 = 0;
	m_toonface2oldface = 0;

	m_sorttex = 0;
	m_optindexbuftex = 0;
	m_optindexbuftexm = 0;
	m_texblocknum = 0;
	m_texblock = 0;

	m_dirtymat = 0;

	smface = 0;

	chkalpha.alphanum = 0;
	chkalpha.notalphanum = 0;

	//E3DCOLOR3UC infcol[CALCMODE_MAX];
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

}

void CPolyMesh2::DestroyObjs()
{
	DestroySortTexBuffer();
	DestroyToon1Buffer();
	DestroyTempMaterial();
	DestroyOptData();
	DestroyPolyMesh2();
	CBaseDat::DestroyObjs();

	if( m_IE ){
		delete [] m_IE;
		m_IE = 0;
	}

}

void CPolyMesh2::DestroyPolyMesh2()
{
	if( meshinfo ){
		delete meshinfo;
		meshinfo = 0;
	}

	if( pointbuf ){
		free( pointbuf );
		pointbuf = 0;
	}
	if( orgnormal ){
		free( orgnormal );
		orgnormal = 0;
	}
	if( smoothbuf ){
		free( smoothbuf );
		smoothbuf = 0;
	}


	if( samepointbuf ){
		free( samepointbuf );
		samepointbuf = 0;
	}

	if( oldpno2optpno ){
		free( oldpno2optpno );
		oldpno2optpno = 0;
	}

	if( uvbuf ){
		free( uvbuf );
		uvbuf = 0;
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
	if( diffbuf ){
		free( diffbuf );
		diffbuf = 0;
	}

	if( m_attrib0 ){
		free( m_attrib0 );
		m_attrib0 = 0;
	}


	if( dirtypoint ){
		free( dirtypoint );
		dirtypoint = 0;
	}

	if( colorbuf ){
		free( colorbuf );
		colorbuf = 0;
	}


	if( orgnobuf ){
		free( orgnobuf );
		orgnobuf = 0;
	}

	DestroyVColData();


	DestroyOptData();

}

int CPolyMesh2::DestroySortTexBuffer()
{
	if( m_sorttex ){
		free( m_sorttex );
		m_sorttex = 0;
	}

	if( m_optindexbuftex ){
		free( m_optindexbuftex );
		m_optindexbuftex = 0;
	}
	if( m_optindexbuftexm ){
		free( m_optindexbuftexm );
		m_optindexbuftexm = 0;
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


int CPolyMesh2::DestroyToon1Buffer()
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

	if( m_optindexbuf2 ){
		free( m_optindexbuf2 );
		m_optindexbuf2 = 0;
	}
	if( m_optindexbuf2m ){
		free( m_optindexbuf2m );
		m_optindexbuf2m = 0;
	}


	if( m_materialblock ){
		free( m_materialblock );
		m_materialblock = 0;
	}

	return 0;
}
int CPolyMesh2::DestroyVColData()
{
	if( vcoldata ){
		VCOLDATA* deldata;
		int vcno;
		for( vcno = 0; vcno < vcolnum; vcno++ ){
			deldata = *(vcoldata + vcno);
			if( deldata )
				free( deldata );
		}
		vcolnum = 0;

		free( vcoldata );
		vcoldata = 0;
	}

	return 0;
}

int CPolyMesh2::CreateBuffers( CMeshInfo* srcmeshinfo )
{
	int ret;

	DestroyPolyMesh2();

DbgOut( "polymesh2 : CreateBuffers : start\n" );

	// meshinfo
	ret = srcmeshinfo->NewMeshInfo( &meshinfo );
	if( ret ){
		DbgOut( "CPolyMesh2 : CreateBuffers ; srcmeshinfo->NewMeshinfo error !!!\n" );
		return 1;
	}

	int maxfacenum;
	if( g_index32bit ){
		maxfacenum = 715827882;
	}else{
		maxfacenum = 65535 / 3;
	}
	if( meshinfo->n >= maxfacenum ){
		DbgOut( "CPolyMesh2 : CreateBuffers : facenum range error !!!\n" );
		return 1;
	}


	_ASSERT( (meshinfo->type > SHDTYPENONE) && (meshinfo->type < SHDTYPEMAX) );

	ret = CBaseDat::SetType( meshinfo->type );
	if( ret ){
		DbgOut( "CPolyMesh2 : CreateBuffers : base::SetType error !!!\n" );
		return 1;
	}

	int i, bufleng;
	bufleng = meshinfo->n * 3;
	pointbuf = (VEC3F*)malloc( sizeof( VEC3F ) * bufleng );
	if( !pointbuf ){
		DbgOut( "CPolyMesh2 : CreatBuffers : pointbuf alloc error !!!\n" );
		return 1;
	}
	ZeroMemory( pointbuf, sizeof( VEC3F ) * bufleng );


	orgnormal = (VEC3F*)malloc( sizeof( VEC3F ) * bufleng );
	if( !orgnormal ){
		DbgOut( "pm2 : CreateBuffers : orgnormal alloc error !!!\n" );
		return 1;
	}
	ZeroMemory( orgnormal, sizeof( VEC3F ) * bufleng );

	smoothbuf = (int*)malloc( sizeof( int ) * bufleng );
	if( !smoothbuf ){
		DbgOut( "pm2 : CreateBuffers : smoothbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	for( i = 0; i < bufleng; i++ ){
		*(smoothbuf + i) = -1;
	}

	samepointbuf = (int*)malloc( sizeof( int ) * bufleng );
	if( !samepointbuf ){
		DbgOut( "CPolyMesh2 : CreateBuffers : samepointbuf alloc error !!!\n" );
		return 1;
	}
	for( i = 0; i < bufleng; i++ ){
		*(samepointbuf + i) = -1;
	}

	oldpno2optpno = (int*)malloc( sizeof( int ) * bufleng );
	if( !oldpno2optpno ){
		DbgOut( "pm2 : CreateBuffers : oldpno2optpno alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( oldpno2optpno, sizeof( int ) * bufleng );


	uvbuf = (COORDINATE*)malloc( sizeof( COORDINATE ) * bufleng );
	if( !uvbuf ){
		DbgOut( "CPolyMesh2 : CreateBuffers : uvbuf alloc error !!!\n" );
		return 1;
	}
	ZeroMemory( uvbuf, sizeof( COORDINATE ) * bufleng );


	orgnobuf = (int*)malloc( sizeof( int ) * bufleng );
	if( !orgnobuf ){
		DbgOut( "CPolyMesh2 : CreateBuffers : orgnobuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////
	int colbufleng = meshinfo->n;
	diffusebuf = (ARGBF*)malloc( sizeof( ARGBF ) * colbufleng );
	if( !diffusebuf ){
		DbgOut( "CPolyMesh2 : CreateBuffers : diffusebuf alloc error !!!\n" );
		return 1;
	}
	ZeroMemory( diffusebuf, sizeof( ARGBF ) * colbufleng );

	ambientbuf = (ARGBF*)malloc( sizeof( ARGBF ) * colbufleng );
	if( !diffusebuf ){
		DbgOut( "CPolyMesh2 : CreateBuffers : ambientbuf alloc error !!!\n" );
		return 1;
	}
	ZeroMemory( ambientbuf, sizeof( ARGBF ) * colbufleng );

	specularbuf = (float*)malloc( sizeof( float ) * colbufleng );
	if( !specularbuf ){
		DbgOut( "CPolyMesh2 : CreateBuffers : specularbuf alloc error !!!\n" );
		return 1;
	}
	ZeroMemory( specularbuf, sizeof( float ) * colbufleng );

	powerbuf = (float*)malloc( sizeof( float ) * colbufleng );
	if( !powerbuf ){
		DbgOut( "pm2 : CreateBuffers : powerbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( powerbuf, sizeof( float ) * colbufleng );


	emissivebuf = (float*)malloc( sizeof( float ) * colbufleng );
	if( !emissivebuf ){
		DbgOut( "pm2 : CreateBuffers : emissivebuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( emissivebuf, sizeof( float ) * colbufleng );


	diffbuf = (float*)malloc( sizeof( float ) * colbufleng );
	if( !diffbuf ){
		DbgOut( "pm2 : CreateBuffers : diffbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int vno;
	for( vno = 0; vno < colbufleng; vno++ ){
		*(diffbuf + vno) = 1.0f;
	}


	m_attrib0 = (int*)malloc( sizeof( int ) * colbufleng );
	if( !m_attrib0 ){
		DbgOut( "pn2 : CreateBuffers : m_attrib0 alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_attrib0, sizeof( int ) * colbufleng );


	dirtypoint = (char*)malloc( bufleng );
	if( !dirtypoint ){
		DbgOut( "CPolyMesh2 : CreateBuffers : dirtypoint alloc error !!!\n" );
		return 1;
	}
	ZeroMemory( dirtypoint, bufleng );


DbgOut( "polymesh2 : CreateBuffers : end\n" );

	return 0;
}

int CPolyMesh2::MItoMaterial()
{
	if( !meshinfo || !diffusebuf || !ambientbuf || !specularbuf )
		return 0;

	int faceno, facenum;
	facenum = meshinfo->n;

	CVec3f srcdif, srcamb;
	float srcspe;

	srcdif = *(meshinfo->GetMaterial( MAT_DIFFUSE ));
	srcamb = *(meshinfo->GetMaterial( MAT_AMBIENT ));
	srcspe = (meshinfo->GetMaterial( MAT_SPECULAR ))->x;

	for( faceno = 0; faceno < facenum; faceno++ ){
		(diffusebuf + faceno)->r = srcdif.x;
		(diffusebuf + faceno)->g = srcdif.y;
		(diffusebuf + faceno)->b = srcdif.z;

		(ambientbuf + faceno)->r = srcamb.x;
		(ambientbuf + faceno)->g = srcamb.y;
		(ambientbuf + faceno)->b = srcamb.z;

		*(specularbuf + faceno) = srcspe;

	}

	return 0;
}

int CPolyMesh2::SetPointBuf( int srcpno, float srcx, float srcy, float srcz, int ono )
{
	if( (srcpno < 0) || (srcpno >= meshinfo->n * 3) ){
		DbgOut( "CPolyMesh2 : SetPointBuf : pointno out of range error !!!\n" );
		return 1;
	}

	VEC3F* dstpoint = pointbuf + srcpno;
	dstpoint->x = srcx;
	dstpoint->y = srcy;
	dstpoint->z = srcz;


	*( orgnobuf + srcpno ) = ono;


	return 0;
}


int CPolyMesh2::SetPointBuf( int srcpnum, VEC3F* srcvec )
{
	if( (srcpnum < 0) || (srcpnum > meshinfo->n * 3) ){
		DbgOut( "CPolyMesh2 : SetPointBuf : srcpnum out of range error !!!\n" );
		return 1;
	}
	
	MoveMemory( (void*)pointbuf, (void*)srcvec, sizeof( VEC3F ) * srcpnum );


	//if( orgbuf ){
	//	MoveMemory( (void*)orgnobuf, (void*)orgbuf, sizeof( int ) * srcpnum );
	//}


	return 0;
}


int CPolyMesh2::SetDefaultMaterial()
{
	int fno;
	ARGBF defdiff, defamb;
	float defspec, defpower, defemi;
	defdiff.a = 1.0f; defdiff.r = 1.0f; defdiff.g = 1.0f; defdiff.b = 1.0f;
	defamb.a = 1.0f; defamb.r = 0.5f; defamb.g = 0.5f; defamb.b = 0.5f;
	defspec = 0.2f;
	//defpower = 5.0f;
	defpower = 0.0f;
	defemi = 0.0f;

	for( fno = 0; fno < meshinfo->n; fno++ ){
		*(diffusebuf + fno) = defdiff;
		*(ambientbuf + fno) = defamb;
		*(specularbuf + fno) = defspec;
		*(powerbuf + fno) = defpower;
		*(emissivebuf + fno) = defemi;
		*(diffbuf + fno) = 1.0f;
	}

	return 0;
}

int CPolyMesh2::SetAttrib0( int faceno, int srcmaterialno )
{
	int facenum;
	facenum = meshinfo->n;
	if( (faceno < 0) || (faceno >= facenum) ){
		DbgOut( "CPolyMesh2 : SetAttrib0 : faceno range error !!!\n" );
		return 1;
	}

	*( m_attrib0 + faceno ) = srcmaterialno;


	return 0;
}

//int CPolyMesh2::SetMaterial( int faceno, float srcdiffuse, float srcambient, float srcspecular, float power, float emi, float srcalpha, float srcr, float srcg, float srcb, COORDINATE* srcuv, int srcvnum )
int CPolyMesh2::SetMaterial( int faceno, float srcdiffuse, float srcambient, float srcspecular, float power, float emi, ARGBF color, ARGBF scene_ambient, COORDINATE* srcuv, int srcvnum )
{
	int facenum;
	facenum = meshinfo->n;
	if( (faceno < 0) || (faceno >= facenum) ){
		DbgOut( "CPolyMesh2 : SetMaterial : faceno range error !!!\n" );
		return 1;
	}

	(diffusebuf + faceno)->a = color.a;
	(diffusebuf + faceno)->r = color.r;
	(diffusebuf + faceno)->g = color.g;
	(diffusebuf + faceno)->b = color.b;

	(ambientbuf + faceno)->a = color.a;
	(ambientbuf + faceno)->r = scene_ambient.r * srcambient;
	(ambientbuf + faceno)->g = scene_ambient.g * srcambient;
	(ambientbuf + faceno)->b = scene_ambient.b * srcambient;

//DbgOut( "check !!!, pm2 SetMaterial : scene_amb %f, %f, %f, srcamb %f\r\n",
//	scene_ambient.r, scene_ambient.g, scene_ambient.g, srcambient );

	*(specularbuf + faceno) = srcspecular;

	*(powerbuf + faceno) = power;
	*(emissivebuf + faceno) = emi;
	*(diffbuf + faceno) = srcdiffuse;


	int vno = faceno * 3;

	if( srcvnum == 3 ){
		//0, 1, 2
		*(uvbuf + vno) = *srcuv;
		*(uvbuf + vno + 1) = *(srcuv + 1);
		*(uvbuf + vno + 2) = *(srcuv + 2);

	}else if( srcvnum == 4 ){
		//0, 2, 3
		*(uvbuf + vno) = *srcuv;
		*(uvbuf + vno + 1) = *(srcuv + 2);
		*(uvbuf + vno + 2) = *(srcuv + 3);
	}

/***
	COORDINATE curuv;
	curuv.u = 0.1f;
	curuv.v = 0.1f;

	if( srcvnum == 3 ){
		//0, 1, 2
		*(uvbuf + vno) = curuv;
		*(uvbuf + vno + 1) = curuv;
		*(uvbuf + vno + 2) = curuv;

	}else if( srcvnum == 4 ){
		//0, 2, 3
		*(uvbuf + vno) = curuv;
		*(uvbuf + vno + 1) = curuv;
		*(uvbuf + vno + 2) = curuv;
	}
***/
	return 0;
}

int CPolyMesh2::SetVCol( int vertno, __int64 vcol )
{
	
	int facenum;
	facenum = meshinfo->n;
	int vertnum;
	vertnum = facenum * 3;

	if( (vertno < 0) || (vertno >= vertnum) ){
		DbgOut( "pm2 : SetVCol : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	VCOLDATA* newvc;
	newvc = GetFreeVColData();


	if( !newvc ){
		int addnum = 256;
		int oldnum = vcolnum;
		vcolnum += addnum;
		vcoldata = (VCOLDATA**)realloc( vcoldata, sizeof( VCOLDATA* ) * vcolnum );
		if( !vcoldata ){
			DbgOut( "pm2 : SetVCol : vcoldata alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int vcno;
		for( vcno = oldnum; vcno < vcolnum; vcno++ ){

			VCOLDATA* allocvc;
			allocvc = (VCOLDATA*)malloc( sizeof( VCOLDATA ) );
			if( !allocvc ){
				DbgOut( "pm2 : SetVCol : newvc alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			allocvc->vertno = -1;//!!!!!!
			allocvc->vcol = 0xFFFFFFFF;	

			*(vcoldata + vcno) = allocvc;//!!!!!!

			if( vcno == oldnum )
				newvc = allocvc;

		}
	}

	newvc->vertno = vertno;
	newvc->vcol = vcol;

//DbgOut( "check !!! pm2 : SetVCol : vert %d, vcol %u\r\n", newvc->vertno, newvc->vcol );


	return 0;
}

int CPolyMesh2::GetVCol2( int srcoldvno, __int64* dstcol, int* dstsetflag )
{
	*dstsetflag = 0;

	int vcno;
	for( vcno = 0; vcno < vcolnum; vcno++ ){
		VCOLDATA* curvc;
		curvc = *( vcoldata + vcno );

		if( curvc->vertno == srcoldvno ){
			*dstcol = curvc->vcol;

//DbgOut( "check !!! pm2 : GetVCol : vert %d, vcol %u\r\n", curvc->vertno, curvc->vcol );

			*dstsetflag = 1;
			break;
		}
	}

	return 0;
}


int CPolyMesh2::SetSamePoint()
{
	DbgOut( "PolyMesh2 : SetSamePoint\n" );

	if( !pointbuf || !samepointbuf ){
		_ASSERT( 0 );
		return 1;
	}

	int pnum = meshinfo->n * 3;
	int pno;

	//-1で初期化。
	//一個前の同じ座標の番号を保持する。
	//同じ座標を持つ、先頭のsamepointbufは、-2

	//init
	for( pno = 0; pno < pnum; pno++ ){
		*(samepointbuf + pno) = -1;
	}

	//search and set
	int chkno, befno;
	VEC3F curv, chkv;
	for( pno = 0; pno < pnum; pno++ ){
		curv = *(pointbuf + pno);
		befno = pno;

		if( *(samepointbuf + pno) == -1 ){
			for( chkno = 0; chkno < pnum; chkno++ ){
				chkv = *(pointbuf + chkno);

				if( (pno != chkno) && (*(samepointbuf + chkno) == -1) && 
					(curv.x == chkv.x) && (curv.y == chkv.y) && (curv.z == chkv.z)
				){
					
					*(samepointbuf + chkno) = befno;
					if( *(samepointbuf + befno) == -1 )
						*(samepointbuf + befno) = -2;

					befno = chkno;
				}
			}
		}
	}

	/***
// for debug
	int i;
	for( i = 0; i < pnum; i++ ){
		if( i == 0 )
			DbgOut( "check samepointbuf\n" );
		if( i % 128 == 0 )
			DbgOut( "\n\t" );

		DbgOut( "%d, ", *(samepointbuf + i) );
	}
	***/
	/***
	//search and set
	int chkno;
	VEC3F curv, chkv;
	for( pno = 0; pno < pnum; pno++ ){
		curv = *(pointbuf + pno);

		if( *(samepointbuf + pno) < 0 ){
			for( chkno = 0; chkno < pnum; chkno++ ){
				chkv = *(pointbuf + chkno);

				if( (*(samepointbuf + chkno) < 0) &&
					(curv.x == chkv.x) && (curv.y == chkv.y) && (curv.z == chkv.z)
				){
					*(samepointbuf + chkno) = pno;
				}
			}
		}
	}
	***/

	return 0;
}

int CPolyMesh2::SetColor( int srcpno, ARGBF* srccol )
{
	if( (srcpno < 0) || (srcpno > meshinfo->n * 3) ){
		DbgOut( "PolyMesh2 : SetColor : srcpno error !!!\n" );
		return 1;
	}

	if( colorbuf == 0 ){
		colorbuf = (ARGBF*)malloc( sizeof( ARGBF ) * meshinfo->n * 3 );
		if( !colorbuf ){
			DbgOut( "PolyMesh2 : SetColor : colorbuf alloc error !!!\n" );
			return 1;
		}
	}

	*(colorbuf + srcpno) = *srccol;

	return 0;
}


int CPolyMesh2::DestroyOptData()
{
	optflag = 0;
	optpleng = 0;

	if( opttlv ){
		delete [] opttlv;
		opttlv = 0;
	}

	if( optambient ){
		free( optambient );
		optambient = 0;
	}

	if( optpowerbuf ){
		free( optpowerbuf );
		optpowerbuf = 0;
	}

	if( optemissivebuf ){
		free( optemissivebuf );
		optemissivebuf = 0;
	}

	if( optcolorbuf ){
		free( optcolorbuf );
		optcolorbuf = 0;
	}

	//if( oldpno2optpno ){
	//	free( oldpno2optpno );
	//	oldpno2optpno = 0;
	//}

	if( optindexbuf ){
		free( optindexbuf );
		optindexbuf = 0;
	}

	if( optpno2oldpno ){
		free( optpno2oldpno );
		optpno2oldpno = 0;
	}
	if( optsamepointbuf ){
		free( optsamepointbuf );
		optsamepointbuf = 0;
	}

	if( smface ){
		int smno;
		for( smno = 0; smno < meshinfo->n * 3; smno++ ){
			SMFACE* cursm;
			cursm = *( smface + smno );
			if( cursm ){
				free( cursm );
			}
		}
		free( smface );
		smface = 0;
	}


	createoptflag = 0;//!!!!!!

	return 0;
}

int CPolyMesh2::CreateOptDataBySamePointBuf( int srcadjustuvflag, float srcfacet )
{

	//!!!!!!!!! 修正するときは、CreateOptDataもいっしょに！！！！！！！！！！
	
	
	//!!!!!!! ２度目は、作らない　!!!!!!!!!!!!!!!!
	if( createoptflag != 0 ){
		DbgOut( "polymesh2 : CreateOptData : optdata already exist exit !!!\n" );
		return 0;
	}


	if( !samepointbuf ){
		DbgOut( "polymesh2 : CreateOptDataBySamePointBuf : samepointbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DestroyOptData();

	int oldpleng;
	oldpleng = meshinfo->n * 3;

	if( srcadjustuvflag || adjustuvflag ){
		int aduv = max( srcadjustuvflag, adjustuvflag );
		AdjustUVBuf( aduv );
	}

// normal計算
	int ret;
	ret = CalcOrgNormal();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

// smoothbuf
	ret = SetSmoothBuf( srcfacet );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	smface = (SMFACE**)malloc( sizeof( SMFACE* ) * oldpleng );
	if( !smface ){
		DbgOut( "pm2 : CreateOptDataBySamePointBuf : smface alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( smface, sizeof( SMFACE* ) * oldpleng );



	ret = SetSMFace( srcfacet );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


// data長チェック

	optpleng = 0;
	ret = SetOptData( 0, &optpleng, samepointbuf, srcfacet );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

// data	allocate
	opttlv = new D3DTLVERTEX [ optpleng ];
	if( !optpleng ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( opttlv, sizeof( D3DTLVERTEX ) * optpleng );

	optambient = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
	if( !optambient ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( optambient, sizeof( ARGBF ) * optpleng );

	optpowerbuf = (float*)malloc( sizeof( float ) * optpleng );
	if( !optpowerbuf ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( optpowerbuf, sizeof( float ) * optpleng );

	optemissivebuf = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
	if( !optemissivebuf ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( optemissivebuf, sizeof( ARGBF ) * optpleng );


	if( colorbuf ){
		optcolorbuf = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
		if( !optcolorbuf ){
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( optcolorbuf, sizeof( ARGBF ) * optpleng );
	}

//	oldpno2optpno = (int*)malloc( sizeof( int ) * oldpleng );
//	if( !oldpno2optpno ){
//		_ASSERT( 0 );
//		return 1;
//	}
//	ZeroMemory( oldpno2optpno, sizeof( int ) * oldpleng );


	//optindexbuf = (int*)malloc( sizeof( int ) * oldpleng );
	//if( !optindexbuf ){
	//	_ASSERT( 0 );
	//	return 1;
	//}
	//ZeroMemory( optindexbuf, sizeof( int ) * oldpleng );


	optpno2oldpno = (int*)malloc( sizeof( int ) * optpleng );
	if( !optpno2oldpno ){
		_ASSERT( 0 );
		return 1;
	}
	int optpno;
	for( optpno = 0; optpno < optpleng; optpno++ ){
		*( optpno2oldpno + optpno ) = -1;
	}

	optsamepointbuf = (int*)malloc( sizeof( int ) * optpleng );
	if( !optsamepointbuf ){
		_ASSERT( 0 );
		return 1;
	}
	for( optpno = 0; optpno < optpleng; optpno++ ){
		*( optsamepointbuf + optpno ) = -1;
	}


///////////////

// data set
	int chkleng = 0;
	ret = SetOptData( 1, &chkleng, samepointbuf, srcfacet );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( optpleng != chkleng ){
		_ASSERT( 0 );
		return 1;
	}

//////////////
	//optindexbufのセット
	optindexbuf = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !optindexbuf ){
		DbgOut( "Polymesh2 : CreateOptData : optindexbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( optindexbuf, sizeof( int ) * meshinfo->n * 3 );


	int faceno;
	int p0, p1, p2;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		p0 = faceno * 3;
		p1 = faceno * 3 + 1;
		p2 = faceno * 3 + 2;

		*(optindexbuf + faceno * 3) = *(oldpno2optpno + p0);
		*(optindexbuf + faceno * 3 + 1) = *(oldpno2optpno + p1);
		*(optindexbuf + faceno * 3 + 2) = *(oldpno2optpno + p2);
	}


	DbgOut( "Polymesh2 : CreateOptData : oldpleng %d, optpleng %d\n", oldpleng, optpleng );

///////////////////
	//optpno2oldpno のセット
		//同じ座標を持つ、座標のうちで、一番大きいoldpnoをセットする。
	int oldpno;

	for( optpno = 0; optpno < optpleng; optpno++ ){
		for( oldpno = 0; oldpno < meshinfo->n * 3; oldpno++ ){	
			if( *( oldpno2optpno + oldpno ) == optpno ){
				*( optpno2oldpno + optpno ) = oldpno;
			}
		}
	}

	int chkoldno;
	int findsame = 1;
	for( optpno = 0; optpno < optpleng; optpno++ ){
		oldpno = *( optpno2oldpno + optpno );
		
		while( findsame == 1 ){
			findsame = 0;
			for( chkoldno = meshinfo->n * 3 - 1; chkoldno >= 0; chkoldno-- ){
				if( *( samepointbuf + chkoldno ) == oldpno ){
					oldpno = chkoldno;
					findsame = 1;
					break;
				}
			}
		}

		*( optpno2oldpno + optpno ) = oldpno;

	}

	ret = SetOptSamePointBuf();
	_ASSERT( !ret );


//for( optpno = 0; optpno < optpleng; optpno++ ){
//	DbgOut( "polymesh2 : CreateOptData : check optpno2oldpno : optpno %d --> oldpno %d\n",
//		optpno, *( optpno2oldpno + optpno ) );
//}


///////////////////

	optflag = 1;
	createoptflag = 1;


	return 0;
}
int CPolyMesh2::CreateOptDataBySamePointBufLegacy( int srcadjustuvflag, float srcfacet )
{

	//!!!!!!!!! 修正するときは、CreateOptDataもいっしょに！！！！！！！！！！
	
	
	//!!!!!!! ２度目は、作らない　!!!!!!!!!!!!!!!!
	if( createoptflag != 0 ){
		DbgOut( "polymesh2 : CreateOptData : optdata already exist exit !!!\n" );
		return 0;
	}


	if( !samepointbuf ){
		DbgOut( "polymesh2 : CreateOptDataBySamePointBuf : samepointbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DestroyOptData();

	int oldpleng;
	oldpleng = meshinfo->n * 3;

	if( srcadjustuvflag || adjustuvflag ){
		int aduv = max( srcadjustuvflag, adjustuvflag );
		AdjustUVBuf( aduv );
	}

// normal計算
	int ret;
	ret = CalcOrgNormal();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

// smoothbuf
	ret = SetSmoothBuf( srcfacet );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	smface = (SMFACE**)malloc( sizeof( SMFACE* ) * oldpleng );
	if( !smface ){
		DbgOut( "pm2 : CreateOptDataBySamePointBuf : smface alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( smface, sizeof( SMFACE* ) * oldpleng );



	ret = SetSMFace( srcfacet );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


// data長チェック

	optpleng = 0;
	ret = SetOptDataLegacy( 0, &optpleng, samepointbuf, srcfacet );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

// data	allocate
	opttlv = new D3DTLVERTEX [ optpleng ];
	if( !optpleng ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( opttlv, sizeof( D3DTLVERTEX ) * optpleng );

	optambient = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
	if( !optambient ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( optambient, sizeof( ARGBF ) * optpleng );

	optpowerbuf = (float*)malloc( sizeof( float ) * optpleng );
	if( !optpowerbuf ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( optpowerbuf, sizeof( float ) * optpleng );

	optemissivebuf = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
	if( !optemissivebuf ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( optemissivebuf, sizeof( ARGBF ) * optpleng );


	if( colorbuf ){
		optcolorbuf = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
		if( !optcolorbuf ){
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( optcolorbuf, sizeof( ARGBF ) * optpleng );
	}

//	oldpno2optpno = (int*)malloc( sizeof( int ) * oldpleng );
//	if( !oldpno2optpno ){
//		_ASSERT( 0 );
//		return 1;
//	}
//	ZeroMemory( oldpno2optpno, sizeof( int ) * oldpleng );


	//optindexbuf = (int*)malloc( sizeof( int ) * oldpleng );
	//if( !optindexbuf ){
	//	_ASSERT( 0 );
	//	return 1;
	//}
	//ZeroMemory( optindexbuf, sizeof( int ) * oldpleng );


	optpno2oldpno = (int*)malloc( sizeof( int ) * optpleng );
	if( !optpno2oldpno ){
		_ASSERT( 0 );
		return 1;
	}
	int optpno;
	for( optpno = 0; optpno < optpleng; optpno++ ){
		*( optpno2oldpno + optpno ) = -1;
	}

	optsamepointbuf = (int*)malloc( sizeof( int ) * optpleng );
	if( !optsamepointbuf ){
		_ASSERT( 0 );
		return 1;
	}
	for( optpno = 0; optpno < optpleng; optpno++ ){
		*( optsamepointbuf + optpno ) = -1;
	}


///////////////

// data set
	int chkleng = 0;
	ret = SetOptDataLegacy( 1, &chkleng, samepointbuf, srcfacet );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( optpleng != chkleng ){
		_ASSERT( 0 );
		return 1;
	}

//////////////
	//optindexbufのセット
	optindexbuf = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !optindexbuf ){
		DbgOut( "Polymesh2 : CreateOptData : optindexbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( optindexbuf, sizeof( int ) * meshinfo->n * 3 );


	int faceno;
	int p0, p1, p2;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		p0 = faceno * 3;
		p1 = faceno * 3 + 1;
		p2 = faceno * 3 + 2;

		*(optindexbuf + faceno * 3) = *(oldpno2optpno + p0);
		*(optindexbuf + faceno * 3 + 1) = *(oldpno2optpno + p1);
		*(optindexbuf + faceno * 3 + 2) = *(oldpno2optpno + p2);
	}


	DbgOut( "Polymesh2 : CreateOptData : oldpleng %d, optpleng %d\n", oldpleng, optpleng );

///////////////////
	//optpno2oldpno のセット
		//同じ座標を持つ、座標のうちで、一番大きいoldpnoをセットする。
	int oldpno;

	for( optpno = 0; optpno < optpleng; optpno++ ){
		for( oldpno = 0; oldpno < meshinfo->n * 3; oldpno++ ){	
			if( *( oldpno2optpno + oldpno ) == optpno ){
				*( optpno2oldpno + optpno ) = oldpno;
			}
		}
	}

	int chkoldno;
	int findsame = 1;
	for( optpno = 0; optpno < optpleng; optpno++ ){
		oldpno = *( optpno2oldpno + optpno );
		
		while( findsame == 1 ){
			findsame = 0;
			for( chkoldno = meshinfo->n * 3 - 1; chkoldno >= 0; chkoldno-- ){
				if( *( samepointbuf + chkoldno ) == oldpno ){
					oldpno = chkoldno;
					findsame = 1;
					break;
				}
			}
		}

		*( optpno2oldpno + optpno ) = oldpno;

	}

	ret = SetOptSamePointBuf();
	_ASSERT( !ret );


//for( optpno = 0; optpno < optpleng; optpno++ ){
//	DbgOut( "polymesh2 : CreateOptData : check optpno2oldpno : optpno %d --> oldpno %d\n",
//		optpno, *( optpno2oldpno + optpno ) );
//}


///////////////////

	optflag = 1;
	createoptflag = 1;


	return 0;
}

int CPolyMesh2::CreateOptDataByOld2Opt( int srcadjustuvflag, float srcfacet, int calcsmooth )
{
	// sig, gndからのoptdata作成。

	//!!!!!!! ２度目は、作らない　!!!!!!!!!!!!!!!!
	if( createoptflag != 0 ){
		DbgOut( "polymesh2 : CreateOptData : optdata already exist exit !!!\n" );
		return 0;
	}


	if( !samepointbuf || !oldpno2optpno ){
		DbgOut( "polymesh2 : CreateOptData : samepointbuf oldpno2optpno NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DestroyOptData();

	int oldpleng;
	oldpleng = meshinfo->n * 3;

	if( srcadjustuvflag || adjustuvflag ){
		int aduv = max( srcadjustuvflag, adjustuvflag );
		AdjustUVBuf( aduv );
	}

// normal計算
	int ret;
	ret = CalcOrgNormal();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

// smoothbuf
	if( calcsmooth ){
		ret = SetSmoothBuf( srcfacet );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	smface = (SMFACE**)malloc( sizeof( SMFACE* ) * oldpleng );
	if( !smface ){
		DbgOut( "pm2 : CreateOptDataByOld2Opt : smface alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( smface, sizeof( SMFACE* ) * oldpleng );



	ret = SetSMFace( srcfacet );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


// data長チェック
	int* chkflag;
	chkflag = (int*)malloc( sizeof( int ) * oldpleng );
	if( !chkflag ){
		DbgOut( "pm2 : CreateOptData : chkflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( chkflag, sizeof( int ) * oldpleng );

	optpleng = 0;
	int curno, optno;
	for( curno = 0; curno < oldpleng; curno++ ){
		optno = *( oldpno2optpno + curno );
		if( *( chkflag + optno ) == 0 ){
			optpleng++;
			*( chkflag + optno ) = 1;
		}
	}



// data	allocate
	opttlv = new D3DTLVERTEX [ optpleng ];
	if( !optpleng ){
		_ASSERT( 0 );
		free( chkflag );
		chkflag = 0;
		return 1;
	}
	ZeroMemory( opttlv, sizeof( D3DTLVERTEX ) * optpleng );

	optambient = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
	if( !optambient ){
		_ASSERT( 0 );
		free( chkflag );
		chkflag = 0;
		return 1;
	}
	ZeroMemory( optambient, sizeof( ARGBF ) * optpleng );

	optpowerbuf = (float*)malloc( sizeof( float ) * optpleng );
	if( !optpowerbuf ){
		_ASSERT( 0 );
		free( chkflag );
		chkflag = 0;
		return 1;
	}
	ZeroMemory( optpowerbuf, sizeof( float ) * optpleng );

	optemissivebuf = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
	if( !optemissivebuf ){
		_ASSERT( 0 );
		free( chkflag );
		chkflag = 0;
		return 1;
	}
	ZeroMemory( optemissivebuf, sizeof( ARGBF ) * optpleng );


	if( colorbuf ){
		optcolorbuf = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
		if( !optcolorbuf ){
			_ASSERT( 0 );
			free( chkflag );
			chkflag = 0;
			return 1;
		}
		ZeroMemory( optcolorbuf, sizeof( ARGBF ) * optpleng );
	}

//	oldpno2optpno = (int*)malloc( sizeof( int ) * oldpleng );
//	if( !oldpno2optpno ){
//		_ASSERT( 0 );
//		return 1;
//	}
//	ZeroMemory( oldpno2optpno, sizeof( int ) * oldpleng );


	//optindexbuf = (int*)malloc( sizeof( int ) * oldpleng );
	//if( !optindexbuf ){
	//	_ASSERT( 0 );
	//	return 1;
	//}
	//ZeroMemory( optindexbuf, sizeof( int ) * oldpleng );


	optpno2oldpno = (int*)malloc( sizeof( int ) * optpleng );
	if( !optpno2oldpno ){
		_ASSERT( 0 );
		free( chkflag );
		chkflag = 0;
		return 1;
	}
	int optpno;
	for( optpno = 0; optpno < optpleng; optpno++ ){
		*( optpno2oldpno + optpno ) = -1;
	}

	optsamepointbuf = (int*)malloc( sizeof( int ) * optpleng );
	if( !optsamepointbuf ){
		_ASSERT( 0 );
		free( chkflag );
		chkflag = 0;
		return 1;
	}
	for( optpno = 0; optpno < optpleng; optpno++ ){
		*( optsamepointbuf + optpno ) = -1;
	}


///////////////

// data set
	ZeroMemory( chkflag, sizeof( int ) * oldpleng );
	int chkleng = 0;
	ret = SetOptDataByOld2Opt( &chkleng, chkflag );
	if( ret ){
		_ASSERT( 0 );
		free( chkflag );
		chkflag = 0;
		return 1;
	}

	free( chkflag );//!!!!!!!!!!!!
	chkflag = 0;


	if( optpleng != chkleng ){
		DbgOut( "pm2 : CreateOptData : optpleng != chkleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////////////
	//optindexbufのセット
	optindexbuf = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !optindexbuf ){
		DbgOut( "Polymesh2 : CreateOptData : optindexbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( optindexbuf, sizeof( int ) * meshinfo->n * 3 );


	int faceno;
	int p0, p1, p2;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		p0 = faceno * 3;
		p1 = faceno * 3 + 1;
		p2 = faceno * 3 + 2;

		*(optindexbuf + faceno * 3) = *(oldpno2optpno + p0);
		*(optindexbuf + faceno * 3 + 1) = *(oldpno2optpno + p1);
		*(optindexbuf + faceno * 3 + 2) = *(oldpno2optpno + p2);
	}


	DbgOut( "Polymesh2 : CreateOptData : oldpleng %d, optpleng %d\n", oldpleng, optpleng );

///////////////////
	//optpno2oldpno のセット
		//同じ座標を持つ、座標のうちで、一番大きいoldpnoをセットする。
	int oldpno;

	for( optpno = 0; optpno < optpleng; optpno++ ){
		for( oldpno = 0; oldpno < meshinfo->n * 3; oldpno++ ){	
			if( *( oldpno2optpno + oldpno ) == optpno ){
				*( optpno2oldpno + optpno ) = oldpno;
			}
		}
	}

	int chkoldno;
	int findsame = 1;
	for( optpno = 0; optpno < optpleng; optpno++ ){
		oldpno = *( optpno2oldpno + optpno );
		
		while( findsame == 1 ){
			findsame = 0;
			for( chkoldno = meshinfo->n * 3 - 1; chkoldno >= 0; chkoldno-- ){
				if( *( samepointbuf + chkoldno ) == oldpno ){
					oldpno = chkoldno;
					findsame = 1;
					break;
				}
			}
		}

		*( optpno2oldpno + optpno ) = oldpno;

	}

	ret = SetOptSamePointBuf();
	_ASSERT( !ret );


//for( optpno = 0; optpno < optpleng; optpno++ ){
//	DbgOut( "polymesh2 : CreateOptData : check optpno2oldpno : optpno %d --> oldpno %d\n",
//		optpno, *( optpno2oldpno + optpno ) );
//}


///////////////////

	optflag = 1;
	createoptflag = 1;

	return 0;

}

int CPolyMesh2::SetOptData( int setflag, int* lengptr, int* srcsamepoint, float facet )
{
	int pno, chkpno;
	int newno = 0;
	int findno, chkcnt;
	int curfaceno, chkfaceno;
	
	int oldpleng;
	oldpleng = meshinfo->n * 3;
	
	curfaceno = 0;
	chkfaceno = 0;

	float cosfacet;
	int forcesmooth;

	cosfacet = (float)cos( facet * (float)DEG2PAI );
	if( facet == 180.0f ){
		forcesmooth = 1;
	}else{
		forcesmooth = 0;
	}

	for( pno = 0; pno < oldpleng; pno++ ){
		findno = -1;
		curfaceno = pno / 3;
		
		if( !srcsamepoint ){
			for( chkpno = 0; chkpno < pno; chkpno++ ){
		
				chkcnt = 0;//!!!!!!!!!
				chkfaceno = chkpno / 3;

				//material
				_ASSERT( m_attrib0 );
				//int curmatno = *( m_attrib0 + pno );
				//int chkmatno = *( m_attrib0 + chkpno );
				int curmatno = *( m_attrib0 + curfaceno );
				int chkmatno = *( m_attrib0 + chkfaceno );
				if( curmatno == chkmatno ){
					chkcnt++;
				}else{
					continue;
				}

				//座標
				VEC3F* curvec = pointbuf + pno;
				VEC3F* chkvec = pointbuf + chkpno;
				if( (curvec->x == chkvec->x) && (curvec->y == chkvec->y) && (curvec->z == chkvec->z) ){
					chkcnt++;
				}else{
					continue;
				}


				COORDINATE* curuv = uvbuf + pno;
				COORDINATE* chkuv = uvbuf + chkpno;
				if( (curuv->u == chkuv->u) && (curuv->v == chkuv->v) ){
					chkcnt++;
				}else{
					continue;
				}
				

				//diffuse
				ARGBF* curdiff = diffusebuf + curfaceno;
				float curd = *(diffbuf + curfaceno);
				ARGBF* chkdiff = diffusebuf + chkfaceno;
				float chkd = *(diffbuf + chkfaceno);
				if( (curdiff->a == chkdiff->a) && ((curdiff->r * curd) == (chkdiff->r * chkd)) && ((curdiff->g * curd) == (chkdiff->g * chkd)) && ((curdiff->b * curd) == (chkdiff->b * chkd)) ){
					chkcnt++;
				}else{
					continue;
				}

				//ambient
				ARGBF* curamb = ambientbuf + curfaceno;
				ARGBF* chkamb = ambientbuf + chkfaceno;
				if( (curamb->a == chkamb->a) && (curamb->r == chkamb->r) && (curamb->g == chkamb->g) && (curamb->b == chkamb->b) ){
					chkcnt++;
				}else{
					continue;
				}


				//specular
				float* curspec = specularbuf + curfaceno;
				float* chkspec = specularbuf + chkfaceno;
				if( *curspec == *chkspec ){
					chkcnt++;
				}else{
					continue;
				}


				//power
				float* curpow = powerbuf + curfaceno;
				float* chkpow = powerbuf + chkfaceno;
				if( *curpow == *chkpow ){
					chkcnt++;
				}else{
					continue;
				}

				//emissive
				float* curemi = emissivebuf + curfaceno;
				float* chkemi = emissivebuf + chkfaceno;
				if( *curemi == *chkemi ){
					chkcnt++;
				}else{
					continue;
				}


				//colorbuf
				if( colorbuf ){
					ARGBF* curcol = colorbuf + pno;
					ARGBF* chkcol = colorbuf + chkpno;
					if( (curcol->a == chkcol->a) && (curcol->r == chkcol->r) && (curcol->g == chkcol->g) && (curcol->b == chkcol->b) ){
						chkcnt++;
					}else{
						continue;
					}
				}

				//normal
				VEC3F* curnormal;
				VEC3F* chknormal;
				curnormal = orgnormal + pno;
				chknormal = orgnormal + chkpno;
				float dot;
				dot = curnormal->x * chknormal->x + curnormal->y * chknormal->y + curnormal->z * chknormal->z;
				if( (forcesmooth == 1) || (dot > cosfacet) ){
					chkcnt++;
				}else{
					continue;
				}
				

				if( !vcoldata ){
					chkcnt++;
				}else{
					__int64 curvcol;
					__int64 chkvcol;
					curvcol = GetVCol( pno );
					chkvcol = GetVCol( chkpno );
					if( curvcol == chkvcol ){
						chkcnt++;
					}else{
						continue;
					}
				}

				//ここまで、実行されたら、同じ点がすでに、登録されている。
				findno = chkpno;
				break;

			}
		}else{


			chkpno = *( srcsamepoint + pno );
			
			while( (chkpno >= 0) && (findno < 0) ){

				chkcnt = 0;
				chkfaceno = chkpno / 3;

				//material
				//int curmatno = *( m_attrib0 + pno );
				//int chkmatno = *( m_attrib0 + chkpno );
				int curmatno = *( m_attrib0 + curfaceno );
				int chkmatno = *( m_attrib0 + chkfaceno );

				if( curmatno == chkmatno ){
					chkcnt++;
				}


				COORDINATE* curuv = uvbuf + pno;
				COORDINATE* chkuv = uvbuf + chkpno;
				if( (curuv->u == chkuv->u) && (curuv->v == chkuv->v) ){
					chkcnt++;
				}
						

				//diffuse
				ARGBF* curdiff = diffusebuf + curfaceno;
				float curd = *(diffbuf + curfaceno);
				ARGBF* chkdiff = diffusebuf + chkfaceno;
				float chkd = *(diffbuf + chkfaceno);
				if( (curdiff->a == chkdiff->a) && ((curdiff->r * curd) == (chkdiff->r * chkd)) && ((curdiff->g * curd) == (chkdiff->g * chkd)) && ((curdiff->b * curd) == (chkdiff->b * chkd)) ){
					chkcnt++;
				}

				//ambient
				ARGBF* curamb = ambientbuf + curfaceno;
				ARGBF* chkamb = ambientbuf + chkfaceno;
				if( (curamb->a == chkamb->a) && (curamb->r == chkamb->r) && (curamb->g == chkamb->g) && (curamb->b == chkamb->b) ){
					chkcnt++;
				}


				//specular
				float* curspec = specularbuf + curfaceno;
				float* chkspec = specularbuf + chkfaceno;
				if( *curspec == *chkspec ){
					chkcnt++;
				}


				//power
				float* curpow = powerbuf + curfaceno;
				float* chkpow = powerbuf + chkfaceno;
				if( *curpow == *chkpow ){
					chkcnt++;
				}

				//emissive
				float* curemi = emissivebuf + curfaceno;
				float* chkemi = emissivebuf + chkfaceno;
				if( *curemi == *chkemi ){
					chkcnt++;
				}

				//colorbuf
				if( colorbuf ){
					ARGBF* curcol = colorbuf + pno;
					ARGBF* chkcol = colorbuf + chkpno;
					if( (curcol->a == chkcol->a) && (curcol->r == chkcol->r) && (curcol->g == chkcol->g) && (curcol->b == chkcol->b) ){
						chkcnt++;
					}
				}else{
					chkcnt++;
				}

				//normal
				VEC3F* curnormal;
				VEC3F* chknormal;
				curnormal = orgnormal + pno;
				chknormal = orgnormal + chkpno;
				float dot;
				dot = curnormal->x * chknormal->x + curnormal->y * chknormal->y + curnormal->z * chknormal->z;
				if( (forcesmooth == 1) || (dot > cosfacet) ){
					chkcnt++;
				}

				

				if( !vcoldata ){
					chkcnt++;
				}else{
					__int64 curvcol;
					__int64 chkvcol;
					curvcol = GetVCol( pno );
					chkvcol = GetVCol( chkpno );
					if( curvcol == chkvcol ){
						chkcnt++;
					}
				}


				if( chkcnt >= 10 ){
					findno = chkpno;
					break;
				}

				chkpno = *( srcsamepoint + chkpno );
			}
		}


		if( findno < 0 ){
			//同じ点がない場合
			if( setflag != 0 ){
				*(oldpno2optpno + pno) = newno;
			
				(opttlv + newno)->sx = (pointbuf + pno)->x;
				(opttlv + newno)->sy = (pointbuf + pno)->y;
				(opttlv + newno)->sz = (pointbuf + pno)->z;

				(opttlv + newno)->tu = (uvbuf + pno)->u;
				(opttlv + newno)->tv = (uvbuf + pno)->v;

				if( !vcoldata ){

					D3DCOLOR curdiff;
					curdiff = D3DCOLOR_ARGB( 
						(unsigned char)((diffusebuf + curfaceno)->a * 255.0f),
						(unsigned char)((diffusebuf + curfaceno)->r * *(diffbuf + curfaceno) * 255.0f), 
						(unsigned char)((diffusebuf + curfaceno)->g * *(diffbuf + curfaceno) * 255.0f), 
						(unsigned char)((diffusebuf + curfaceno)->b * *(diffbuf + curfaceno) * 255.0f) 
					);
					(opttlv + newno)->color = curdiff;


					(optemissivebuf + newno)->r = (diffusebuf + curfaceno)->r * *(emissivebuf + curfaceno);
					(optemissivebuf + newno)->g = (diffusebuf + curfaceno)->g * *(emissivebuf + curfaceno);
					(optemissivebuf + newno)->b = (diffusebuf + curfaceno)->b * *(emissivebuf + curfaceno);
				
				}else{
					__int64 curvc;
					curvc = GetVCol( pno );

					D3DCOLOR curdiff;
					curdiff = D3DCOLOR_ARGB(
						(unsigned char)((diffusebuf + curfaceno)->a * 255.0f),
						(unsigned char)( (float)(curvc & 0x000000FF) * *(diffbuf + curfaceno) ),
						(unsigned char)( (float)((curvc & 0x0000FF00) >> 8) * *(diffbuf + curfaceno) ),
						(unsigned char)( (float)((curvc & 0x00FF0000) >> 16) * *(diffbuf + curfaceno) )
					);
					(opttlv + newno)->color = curdiff;


					(optemissivebuf + newno)->r = (float)(curvc & 0x000000FF) / 255.0f * *(emissivebuf + curfaceno);
					(optemissivebuf + newno)->g = (float)((curvc & 0x0000FF00) >> 8) / 255.0f * *(emissivebuf + curfaceno);
					(optemissivebuf + newno)->b = (float)((curvc & 0x00FF0000) >> 16) / 255.0f * *(emissivebuf + curfaceno);

				}

				D3DCOLOR curspec;
				curspec = D3DCOLOR_ARGB( 
					255,
					(unsigned char)(*(specularbuf + curfaceno) * 255.0f), 
					(unsigned char)(*(specularbuf + curfaceno) * 255.0f), 
					(unsigned char)(*(specularbuf + curfaceno) * 255.0f) 
				);
				(opttlv + newno)->specular = curspec;


				*(optambient + newno) = *(ambientbuf + curfaceno);
				
				*(optpowerbuf + newno) = *(powerbuf + curfaceno);


//if( ((optambient + newno)->r = 0.0f) && ((optambient + newno)->g = 0.0f) && ((optambient + newno)->b = 0.0f) ){
//DbgOut( "pm2 : SetOptData : optambient zero %d!!!\r\n", curfaceno );
//}
				if( optcolorbuf ){
					*(optcolorbuf + newno) = *(colorbuf + curfaceno);
				}
			}

			newno++;//!!!!!
		}else{
			//同じ点は、findno
			if( setflag != 0 ){

				_ASSERT( pno > findno );
				//if( pno > findno ){
				*(oldpno2optpno + pno) = *(oldpno2optpno + findno);
				//}

			}

		}
	}

	*lengptr = newno;//!!!!!

	return 0;
}

int CPolyMesh2::SetOptDataLegacy( int setflag, int* lengptr, int* srcsamepoint, float facet )
{
	int pno, chkpno;
	int newno = 0;
	int findno, chkcnt;
	int curfaceno, chkfaceno;
	
	int oldpleng;
	oldpleng = meshinfo->n * 3;
	
	curfaceno = 0;
	chkfaceno = 0;

	float cosfacet;
	int forcesmooth;

	cosfacet = (float)cos( facet * (float)DEG2PAI );
	if( facet == 180.0f ){
		forcesmooth = 1;
	}else{
		forcesmooth = 0;
	}

	for( pno = 0; pno < oldpleng; pno++ ){
		findno = -1;
		curfaceno = pno / 3;
		
		if( !srcsamepoint ){
			for( chkpno = 0; chkpno < pno; chkpno++ ){
		
				chkcnt = 0;//!!!!!!!!!
				chkfaceno = chkpno / 3;

				//座標
				VEC3F* curvec = pointbuf + pno;
				VEC3F* chkvec = pointbuf + chkpno;
				if( (curvec->x == chkvec->x) && (curvec->y == chkvec->y) && (curvec->z == chkvec->z) ){
					chkcnt++;
				}else{
					continue;
				}


				COORDINATE* curuv = uvbuf + pno;
				COORDINATE* chkuv = uvbuf + chkpno;
				if( (curuv->u == chkuv->u) && (curuv->v == chkuv->v) ){
					chkcnt++;
				}else{
					continue;
				}
				

				//diffuse
				ARGBF* curdiff = diffusebuf + curfaceno;
				float curd = *(diffbuf + curfaceno);
				ARGBF* chkdiff = diffusebuf + chkfaceno;
				float chkd = *(diffbuf + chkfaceno);
				if( (curdiff->a == chkdiff->a) && ((curdiff->r * curd) == (chkdiff->r * chkd)) && ((curdiff->g * curd) == (chkdiff->g * chkd)) && ((curdiff->b * curd) == (chkdiff->b * chkd)) ){
					chkcnt++;
				}else{
					continue;
				}

				//ambient
				ARGBF* curamb = ambientbuf + curfaceno;
				ARGBF* chkamb = ambientbuf + chkfaceno;
				if( (curamb->a == chkamb->a) && (curamb->r == chkamb->r) && (curamb->g == chkamb->g) && (curamb->b == chkamb->b) ){
					chkcnt++;
				}else{
					continue;
				}


				//specular
				float* curspec = specularbuf + curfaceno;
				float* chkspec = specularbuf + chkfaceno;
				if( *curspec == *chkspec ){
					chkcnt++;
				}else{
					continue;
				}


				//power
				float* curpow = powerbuf + curfaceno;
				float* chkpow = powerbuf + chkfaceno;
				if( *curpow == *chkpow ){
					chkcnt++;
				}else{
					continue;
				}

				//emissive
				float* curemi = emissivebuf + curfaceno;
				float* chkemi = emissivebuf + chkfaceno;
				if( *curemi == *chkemi ){
					chkcnt++;
				}else{
					continue;
				}


				//colorbuf
				if( colorbuf ){
					ARGBF* curcol = colorbuf + pno;
					ARGBF* chkcol = colorbuf + chkpno;
					if( (curcol->a == chkcol->a) && (curcol->r == chkcol->r) && (curcol->g == chkcol->g) && (curcol->b == chkcol->b) ){
						chkcnt++;
					}else{
						continue;
					}
				}

				//normal
				VEC3F* curnormal;
				VEC3F* chknormal;
				curnormal = orgnormal + pno;
				chknormal = orgnormal + chkpno;
				float dot;
				dot = curnormal->x * chknormal->x + curnormal->y * chknormal->y + curnormal->z * chknormal->z;
				if( (forcesmooth == 1) || (dot > cosfacet) ){
					chkcnt++;
				}else{
					continue;
				}
				

				if( !vcoldata ){
					chkcnt++;
				}else{
					__int64 curvcol;
					__int64 chkvcol;
					curvcol = GetVCol( pno );
					chkvcol = GetVCol( chkpno );
					if( curvcol == chkvcol ){
						chkcnt++;
					}else{
						continue;
					}
				}

				//ここまで、実行されたら、同じ点がすでに、登録されている。
				findno = chkpno;
				break;

			}
		}else{


			chkpno = *( srcsamepoint + pno );
			
			while( (chkpno >= 0) && (findno < 0) ){

				chkcnt = 0;
				chkfaceno = chkpno / 3;

				COORDINATE* curuv = uvbuf + pno;
				COORDINATE* chkuv = uvbuf + chkpno;
				if( (curuv->u == chkuv->u) && (curuv->v == chkuv->v) ){
					chkcnt++;
				}
						

				//diffuse
				ARGBF* curdiff = diffusebuf + curfaceno;
				float curd = *(diffbuf + curfaceno);
				ARGBF* chkdiff = diffusebuf + chkfaceno;
				float chkd = *(diffbuf + chkfaceno);
				if( (curdiff->a == chkdiff->a) && ((curdiff->r * curd) == (chkdiff->r * chkd)) && ((curdiff->g * curd) == (chkdiff->g * chkd)) && ((curdiff->b * curd) == (chkdiff->b * chkd)) ){
					chkcnt++;
				}

				//ambient
				ARGBF* curamb = ambientbuf + curfaceno;
				ARGBF* chkamb = ambientbuf + chkfaceno;
				if( (curamb->a == chkamb->a) && (curamb->r == chkamb->r) && (curamb->g == chkamb->g) && (curamb->b == chkamb->b) ){
					chkcnt++;
				}


				//specular
				float* curspec = specularbuf + curfaceno;
				float* chkspec = specularbuf + chkfaceno;
				if( *curspec == *chkspec ){
					chkcnt++;
				}


				//power
				float* curpow = powerbuf + curfaceno;
				float* chkpow = powerbuf + chkfaceno;
				if( *curpow == *chkpow ){
					chkcnt++;
				}

				//emissive
				float* curemi = emissivebuf + curfaceno;
				float* chkemi = emissivebuf + chkfaceno;
				if( *curemi == *chkemi ){
					chkcnt++;
				}

				//colorbuf
				if( colorbuf ){
					ARGBF* curcol = colorbuf + pno;
					ARGBF* chkcol = colorbuf + chkpno;
					if( (curcol->a == chkcol->a) && (curcol->r == chkcol->r) && (curcol->g == chkcol->g) && (curcol->b == chkcol->b) ){
						chkcnt++;
					}
				}else{
					chkcnt++;
				}

				//normal
				VEC3F* curnormal;
				VEC3F* chknormal;
				curnormal = orgnormal + pno;
				chknormal = orgnormal + chkpno;
				float dot;
				dot = curnormal->x * chknormal->x + curnormal->y * chknormal->y + curnormal->z * chknormal->z;
				if( (forcesmooth == 1) || (dot > cosfacet) ){
					chkcnt++;
				}

				

				if( !vcoldata ){
					chkcnt++;
				}else{
					__int64 curvcol;
					__int64 chkvcol;
					curvcol = GetVCol( pno );
					chkvcol = GetVCol( chkpno );
					if( curvcol == chkvcol ){
						chkcnt++;
					}
				}


				if( chkcnt >= 9 ){
					findno = chkpno;
					break;
				}

				chkpno = *( srcsamepoint + chkpno );
			}
		}


		if( findno < 0 ){
			//同じ点がない場合
			if( setflag != 0 ){
				*(oldpno2optpno + pno) = newno;
			
				(opttlv + newno)->sx = (pointbuf + pno)->x;
				(opttlv + newno)->sy = (pointbuf + pno)->y;
				(opttlv + newno)->sz = (pointbuf + pno)->z;

				(opttlv + newno)->tu = (uvbuf + pno)->u;
				(opttlv + newno)->tv = (uvbuf + pno)->v;

				if( !vcoldata ){

					D3DCOLOR curdiff;
					curdiff = D3DCOLOR_ARGB( 
						(unsigned char)((diffusebuf + curfaceno)->a * 255.0f),
						(unsigned char)((diffusebuf + curfaceno)->r * *(diffbuf + curfaceno) * 255.0f), 
						(unsigned char)((diffusebuf + curfaceno)->g * *(diffbuf + curfaceno) * 255.0f), 
						(unsigned char)((diffusebuf + curfaceno)->b * *(diffbuf + curfaceno) * 255.0f) 
					);
					(opttlv + newno)->color = curdiff;


					(optemissivebuf + newno)->r = (diffusebuf + curfaceno)->r * *(emissivebuf + curfaceno);
					(optemissivebuf + newno)->g = (diffusebuf + curfaceno)->g * *(emissivebuf + curfaceno);
					(optemissivebuf + newno)->b = (diffusebuf + curfaceno)->b * *(emissivebuf + curfaceno);
				
				}else{
					__int64 curvc;
					curvc = GetVCol( pno );

					D3DCOLOR curdiff;
					curdiff = D3DCOLOR_ARGB(
						(unsigned char)((diffusebuf + curfaceno)->a * 255.0f),
						(unsigned char)( (float)(curvc & 0x000000FF) * *(diffbuf + curfaceno) ),
						(unsigned char)( (float)((curvc & 0x0000FF00) >> 8) * *(diffbuf + curfaceno) ),
						(unsigned char)( (float)((curvc & 0x00FF0000) >> 16) * *(diffbuf + curfaceno) )
					);
					(opttlv + newno)->color = curdiff;


					(optemissivebuf + newno)->r = (float)(curvc & 0x000000FF) / 255.0f * *(emissivebuf + curfaceno);
					(optemissivebuf + newno)->g = (float)((curvc & 0x0000FF00) >> 8) / 255.0f * *(emissivebuf + curfaceno);
					(optemissivebuf + newno)->b = (float)((curvc & 0x00FF0000) >> 16) / 255.0f * *(emissivebuf + curfaceno);

				}

				D3DCOLOR curspec;
				curspec = D3DCOLOR_ARGB( 
					255,
					(unsigned char)(*(specularbuf + curfaceno) * 255.0f), 
					(unsigned char)(*(specularbuf + curfaceno) * 255.0f), 
					(unsigned char)(*(specularbuf + curfaceno) * 255.0f) 
				);
				(opttlv + newno)->specular = curspec;


				*(optambient + newno) = *(ambientbuf + curfaceno);
				
				*(optpowerbuf + newno) = *(powerbuf + curfaceno);


//if( ((optambient + newno)->r = 0.0f) && ((optambient + newno)->g = 0.0f) && ((optambient + newno)->b = 0.0f) ){
//DbgOut( "pm2 : SetOptData : optambient zero %d!!!\r\n", curfaceno );
//}
				if( optcolorbuf ){
					*(optcolorbuf + newno) = *(colorbuf + curfaceno);
				}
			}

			newno++;//!!!!!
		}else{
			//同じ点は、findno
			if( setflag != 0 ){

				_ASSERT( pno > findno );
				//if( pno > findno ){
				*(oldpno2optpno + pno) = *(oldpno2optpno + findno);
				//}

			}

		}
	}

	*lengptr = newno;//!!!!!

	return 0;
}


int CPolyMesh2::SetOptDataByOld2Opt( int* lengptr, int* chkflag )
{
	int newno, pno, curfaceno;
	int oldpno = meshinfo->n * 3;
	
	int setcnt = 0;

	for( pno = 0; pno < oldpno; pno++ ){
		curfaceno = pno / 3;

		newno = *( oldpno2optpno + pno );

		if( *( chkflag + newno ) == 0 ){

			*( chkflag + newno ) = 1;//!!!!!

			//*(oldpno2optpno + pno) = newno;
			
			(opttlv + newno)->sx = (pointbuf + pno)->x;
			(opttlv + newno)->sy = (pointbuf + pno)->y;
			(opttlv + newno)->sz = (pointbuf + pno)->z;

			(opttlv + newno)->tu = (uvbuf + pno)->u;
			(opttlv + newno)->tv = (uvbuf + pno)->v;

			if( !vcoldata ){

				D3DCOLOR curdiff;
				curdiff = D3DCOLOR_ARGB( 
					(unsigned char)((diffusebuf + curfaceno)->a * 255.0f),
					(unsigned char)((diffusebuf + curfaceno)->r * *(diffbuf + curfaceno) * 255.0f), 
					(unsigned char)((diffusebuf + curfaceno)->g * *(diffbuf + curfaceno) * 255.0f), 
					(unsigned char)((diffusebuf + curfaceno)->b * *(diffbuf + curfaceno) * 255.0f) 
				);
				(opttlv + newno)->color = curdiff;


				(optemissivebuf + newno)->r = (diffusebuf + curfaceno)->r * *(emissivebuf + curfaceno);
				(optemissivebuf + newno)->g = (diffusebuf + curfaceno)->g * *(emissivebuf + curfaceno);
				(optemissivebuf + newno)->b = (diffusebuf + curfaceno)->b * *(emissivebuf + curfaceno);
				
			}else{
				__int64 curvc;
				curvc = GetVCol( pno );

				D3DCOLOR curdiff;
				curdiff = D3DCOLOR_ARGB(
					(unsigned char)((diffusebuf + curfaceno)->a * 255.0f),
					(unsigned char)( (float)(curvc & 0x000000FF) * *(diffbuf + curfaceno) ),
					(unsigned char)( (float)((curvc & 0x0000FF00) >> 8) * *(diffbuf + curfaceno) ),
					(unsigned char)( (float)((curvc & 0x00FF0000) >> 16) * *(diffbuf + curfaceno) )
				);
				(opttlv + newno)->color = curdiff;


				(optemissivebuf + newno)->r = (float)(curvc & 0x000000FF) / 255.0f * *(emissivebuf + curfaceno);
				(optemissivebuf + newno)->g = (float)((curvc & 0x0000FF00) >> 8) / 255.0f * *(emissivebuf + curfaceno);
				(optemissivebuf + newno)->b = (float)((curvc & 0x00FF0000) >> 16) / 255.0f * *(emissivebuf + curfaceno);

			}

			D3DCOLOR curspec;
			curspec = D3DCOLOR_ARGB( 
				255,
				(unsigned char)(*(specularbuf + curfaceno) * 255.0f), 
				(unsigned char)(*(specularbuf + curfaceno) * 255.0f), 
				(unsigned char)(*(specularbuf + curfaceno) * 255.0f) 
			);
			(opttlv + newno)->specular = curspec;


			*(optambient + newno) = *(ambientbuf + curfaceno);
				
			*(optpowerbuf + newno) = *(powerbuf + curfaceno);


//if( ((optambient + newno)->r = 0.0f) && ((optambient + newno)->g = 0.0f) && ((optambient + newno)->b = 0.0f) ){
//DbgOut( "pm2 : SetOptData : optambient zero %d!!!\r\n", curfaceno );
//}
			if( optcolorbuf ){
				*(optcolorbuf + newno) = *(colorbuf + curfaceno);
			}

			setcnt++;
		}
	}

	*lengptr = setcnt;

	return 0;
}

/***
int CPolyMesh2::AdjustUVBuf()
{
	int pno;
	COORDINATE* curuv;
	for( pno = 0; pno < meshinfo->n * 3; pno++ ){
		curuv = uvbuf + pno;

		if( curuv->u < 0.0f ){
			while( curuv->u < 0.0f )
				curuv->u += 1.0f;
		}
		if( curuv->v < 0.0f ){
			while( curuv->v < 0.0f )
				curuv->v += 1.0f;
		}

		if( curuv->u > 1.0f ){
			while( curuv->u > 1.0f )
				curuv->u -= 1.0f;
		}
		if( curuv->v > 1.0f ){
			while( curuv->v > 1.0f )
				curuv->v -= 1.0f;
		}
	}

	return 0;
}
***/


int CPolyMesh2::AdjustUVBuf( int aduv )
{
	int pno;
	COORDINATE* curuv;

	if( aduv == 1 ){

		float minu, maxu, minv, maxv;
		minu = 1e6;
		maxu = -1e6;
		minv = 1e6;
		maxv = -1e6;

		for( pno = 0; pno < meshinfo->n * 3; pno++ ){
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
			for( pno = 0; pno < meshinfo->n * 3; pno++ ){
				curuv = uvbuf + pno;
				
				curuv->u = (curuv->u - minu) / urange;
			}
		}
		if( (minv < 0.0f) || (maxv > 1.0f)){
			float vrange;
			vrange = maxv - minv;

			for( pno = 0; pno < meshinfo->n * 3; pno++ ){
				curuv = uvbuf + pno;
				
				curuv->v = (curuv->v - minv) / vrange;

			}
		}

	}else if( aduv == 2 ){
		for( pno = 0; pno < meshinfo->n * 3; pno++ ){
			curuv = uvbuf + pno;
			
			if( curuv->u < 0.0f )
				curuv->u = 0.0f;
			if( curuv->u > 1.0f )
				curuv->u = 1.0f;

			if( curuv->v < 0.0f )
				curuv->v = 0.0f;
			if( curuv->v > 1.0f )
				curuv->v = 1.0f;
		}
	}
	return 0;
}

//int CPolyMesh2::SetGroundFlag( int srcflag )
//{
//	groundflag = srcflag;
//
//	return 0;
//}


int CPolyMesh2::GetOptPos( int vertno, D3DXVECTOR3* vpos )
{
	if( (vertno < 0) || (vertno >= optpleng) ){
		DbgOut( "polymesh2 : GetOptPos : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !opttlv ){
		DbgOut( "polymesh2 : GetOptPos : opttlv not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DTLVERTEX* curtlv;
	curtlv = opttlv + vertno;

	vpos->x = curtlv->sx;
	vpos->y = curtlv->sy;
	vpos->z = curtlv->sz;

	return 0;
}

int CPolyMesh2::SetOptPos( int vertno, D3DXVECTOR3 vpos )
{
	if( (vertno < 0) || (vertno >= optpleng) ){
		DbgOut( "polymesh2 : GetOptPos : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !opttlv ){
		DbgOut( "polymesh2 : GetOptPos : opttlv not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DTLVERTEX* curtlv;
	curtlv = opttlv + vertno;

	curtlv->sx = vpos.x;
	curtlv->sy = vpos.y;
	curtlv->sz = vpos.z;



// old point buf もセット。
	int oldpno;
	VEC3F* dstold;
	int dbgcnt = 0;

	oldpno = *( optpno2oldpno + vertno );

	while( oldpno >= 0 ){
		dstold = pointbuf + oldpno;

		dstold->x = vpos.x;
		dstold->y = vpos.y;
		dstold->z = vpos.z;

		oldpno = *( samepointbuf + oldpno );

		dbgcnt++;

		if( dbgcnt > 10000 ){
			DbgOut( "polymesh2 : SetOptPos : oldpno loop error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	return 0;
}

int CPolyMesh2::MakeFaceno2Materialno( ARGBF* sceneamb, int srcshader, int sigmagicno, int** arrayptr, CMQOMaterial* newmathead, CMQOMaterial* shmathead, int srcseri )
{
	int faceno;

	int* faceno2matno;
	faceno2matno = (int*)malloc( sizeof( int ) * meshinfo->n );
	if( !faceno2matno ){
		DbgOut( "polymesh2 : MakeFaceno2Materialno : faceno2matno alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		*( faceno2matno + faceno ) = -1;
	}


	int newfileflag = 0;
	if( (sigmagicno >= SIGFILEMAGICNO_4) && (sigmagicno <= (SIGFILEMAGICNO_4 + 100)) ){
		newfileflag = 1;
	}
	if( (sigmagicno >= GNDFILEMAGICNO_4) && (sigmagicno <= (GNDFILEMAGICNO_4 + 100)) ){
		newfileflag = 1;
	}


	int topindex;
	D3DTLVERTEX* curtlv;
	ARGBF* curamb;
	ARGBF* curemi;
	float curpow;
	CMQOMaterial curmaterial;
	ARGBF* curorgdif;

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

		topindex = *(optindexbuf + faceno * 3);
		curtlv = opttlv + topindex;
		curorgdif = diffusebuf + faceno;
///////

		sprintf_s( curmaterial.name, 256, "mat_polymesh2_%d_%d", srcseri, faceno );

		curamb = optambient + topindex;
		curemi = optemissivebuf + topindex;
		curpow = *( optpowerbuf + topindex );


		int colr, colg, colb, specr;
		curmaterial.shader = srcshader;

		if( newfileflag == 0 ){

			colr = (curtlv->color & 0x00FF0000) >> 16;
			colg = (curtlv->color & 0x0000FF00) >> 8;
			colb = (curtlv->color & 0x000000FF);

			specr = (curtlv->specular & 0x000000FF);

			curmaterial.col.a = curalpha;

			if( (curtlv->color & 0x00FFFFFF) != 0 ){
				curmaterial.col.r = (float)colr / 255.0f;
				curmaterial.col.g = (float)colg / 255.0f;
				curmaterial.col.b = (float)colb / 255.0f;
				curmaterial.dif = 1.0f;
			}else{
				curmaterial.col.r = curorgdif->r;
				curmaterial.col.g = curorgdif->g;
				curmaterial.col.b = curorgdif->b;
				curmaterial.dif = 1.0f;
			}

			curmaterial.dif = 1.0f;
			//curmaterial.dif = 0.5f;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			curmaterial.spc = (float)specr / 255.0f;

			if( curmaterial.col.r != 0.0f )
				curmaterial.amb = curamb->r / curmaterial.col.r;
			else
				curmaterial.amb = curamb->r;

			curmaterial.emi = 0.0f;
			//curmaterial.power = 5.0f;
			curmaterial.power = 0.0f;

		}else{
			colr = (curtlv->color & 0x00FF0000) >> 16;
			colg = (curtlv->color & 0x0000FF00) >> 8;
			colb = (curtlv->color & 0x000000FF);

			specr = (curtlv->specular & 0x000000FF);

			curmaterial.col.a = curalpha;

			if( (curtlv->color & 0x00FFFFFF) != 0 ){
				curmaterial.col.r = (float)colr / 255.0f;
				curmaterial.col.g = (float)colg / 255.0f;
				curmaterial.col.b = (float)colb / 255.0f;
				curmaterial.dif = 1.0f;
			}else{
				curmaterial.col.r = curorgdif->r;
				curmaterial.col.g = curorgdif->g;
				curmaterial.col.b = curorgdif->b;
				curmaterial.dif = 0.0f;
			}
			
			curmaterial.spc = (float)specr / 255.0f;

			if( sceneamb->r != 0.0f )
				curmaterial.amb = curamb->r / sceneamb->r;
			else
				curmaterial.amb = curamb->r;
			
			if( curmaterial.col.r != 0.0f ){
				curmaterial.emi = curemi->r / curmaterial.col.r;
			}else if( curorgdif->r != 0.0f ){
				curmaterial.emi = curemi->r / curorgdif->r;
			}else{
				curmaterial.emi = curemi->r;
			}

			curmaterial.power = curpow;
		}


		if( curtexname && *curtexname ){
			int nameleng;
			nameleng = (int)strlen( curtexname );
			if( nameleng >= 255 ){
				DbgOut( "polymesh2 : MakeFaceno2Materialno : nameleng error !!!\n" );
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
				DbgOut( "polymesh2 : MakeFaceno2Materialno : befmat error !!!\n" );
				_ASSERT( 0 );
				free( faceno2matno );
				return 1;
			}


			CMQOMaterial* newmat;
			newmat = new CMQOMaterial();
			if( !newmat ){
				DbgOut( "polymesh2 : MakeFaceno2Materialno : newmat alloc error !!!\n" );
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

int CPolyMesh2::SetCurrentPose2OrgData( CMotHandler* srclpmh, CShdElem* selem, CQuaternion* multq )
{
	int ret;
	if( !opttlv ){
		DbgOut( "pm2 : SetCurrentPose2OrgData : optdata not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3* wvec;
	wvec = new D3DXVECTOR3[ optpleng ];
	if( !wvec ){
		DbgOut( "pm2 : SetCurrentPose2OrgData : wvec alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXMATRIX iniwmat;
	D3DXMatrixIdentity( &iniwmat );
	ret = selem->TransformOnlyWorld3( srclpmh, iniwmat, wvec );
	if( ret ){
		DbgOut( "pm2 : SetCurrentPose2OrgData : selem TransformOnlyWorld3 error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}

	int vno;
	D3DTLVERTEX* curdst;
	D3DXVECTOR3* cursrc;
	for( vno = 0; vno < optpleng; vno++ ){
		curdst = opttlv + vno;
		cursrc = wvec + vno;

		if( multq ){
			multq->Rotate( cursrc, *cursrc );
		}

		curdst->sx = cursrc->x;
		curdst->sy = cursrc->y;
		curdst->sz = cursrc->z;
	}


	delete [] wvec;
/////////

	int ono;
	VEC3F* dstorg;
	D3DTLVERTEX* srctlv;
	int opno;
	for( ono = 0; ono < meshinfo->n * 3; ono++ ){
		dstorg = pointbuf + ono;
		
		opno = *( oldpno2optpno + ono );
		if( opno >= 0 ){
			srctlv = opttlv + opno;

			dstorg->x = srctlv->sx;
			dstorg->y = srctlv->sy;
			dstorg->z = srctlv->sz;
		}
	}

	return 0;
}

int CPolyMesh2::ConvSymmXShape()
{
	//int ret;
	if( !opttlv ){
		DbgOut( "pm2 : ConvSymmXShape : optdata not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int vno;
	D3DTLVERTEX* curdst;
	for( vno = 0; vno < optpleng; vno++ ){
		curdst = opttlv + vno;

		curdst->sx = -curdst->sx;
		curdst->sy = curdst->sy;
		curdst->sz = curdst->sz;
	}
/////////

	int ono;
	VEC3F* dstorg;
	D3DTLVERTEX* srctlv;
	int opno;
	for( ono = 0; ono < meshinfo->n * 3; ono++ ){
		dstorg = pointbuf + ono;
		
		opno = *( oldpno2optpno + ono );
		if( opno >= 0 ){
			srctlv = opttlv + opno;

			dstorg->x = srctlv->sx;
			dstorg->y = srctlv->sy;
			dstorg->z = srctlv->sz;
		}
	}

	return 0;
}
int CPolyMesh2::WriteMQOObjectOnFrameMorph( HANDLE hfile, int* matnoindex, char* srctexname, 
	int motid, int frameno, int serialno, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult, int writeRevFlag )
{
	int ret;
	char tempchar[LINECHARLENG];

// vertex
	//	vertex 9 {
	//		15.0621 87.7051 -57.7583
	//		...
	//	}

	CShdElem* selem;
	selem = (*lpsh)( serialno );

	if( selem->clockwise != 3 ){
		sprintf_s( tempchar, LINECHARLENG, "\tvertex %d {", optpleng );
	}else{
		sprintf_s( tempchar, LINECHARLENG, "\tvertex %d {", optpleng * 2 );
	}
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObjectMorph : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	TEXV* wv;
	wv = (TEXV*)malloc( sizeof( TEXV ) * optpleng );
	if( !wv ){
		DbgOut( "pm2 : WriteMQOObjectOnFrameMorph : wvec alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( wv, sizeof( TEXV ) * optpleng );


	if( !selem ){
		DbgOut( "pm2 : WriteMQOObjectOnFrameMorph : selem NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = lpmh->SetCurrentMotion( lpsh, motid, frameno );
	if( ret ){
		DbgOut( "pm2 : WriteMQOObjectOnFrameMorph : mh SetCurrentMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = selem->TransformDMorphPM2();
	if( ret ){
		DbgOut( "pm2 : WriteMQOObjectOnFrameMorph : se TransformDMorphPM2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}




	D3DXMATRIX iniwmat;
	D3DXMatrixIdentity( &iniwmat );
	
	ret = selem->d3ddisp->TransformOnlyWorldMorph( lpmh, iniwmat, wv, m_IE, oldpno2optpno );
	if( ret ){
		DbgOut( "pm2 : WriteMQOObjectOnFrameMorph : d3ddisp TransformOnlyWorldMorph error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int vertno;
	TEXV* curv;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		curv = wv + vertno;

		//!!!!!!!
		// Z の符号を反転する。
		//!!!!!!!
		sprintf_s( tempchar, LINECHARLENG, "\t\t%.4f %.4f %.4f", curv->pos.x * srcmult, curv->pos.y * srcmult, -curv->pos.z * srcmult );
		//sprintf( tempchar, "\t\t%.4f %.4f %.4f", curtlv->sz, curtlv->sy, curtlv->sx );
		ret = WriteChar( hfile, tempchar, 1 );
		if( ret ){
			DbgOut( "polymesh2 : WriteMQOObjectMorph : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( selem->clockwise == 3 ){
		for( vertno = 0; vertno < optpleng; vertno++ ){
			curv = wv + vertno;

			//!!!!!!!
			// Z の符号を反転する。
			//!!!!!!!
			sprintf_s( tempchar, LINECHARLENG, "\t\t%.4f %.4f %.4f", curv->pos.x * srcmult, curv->pos.y * srcmult, -curv->pos.z * srcmult );
			//sprintf( tempchar, "\t\t%.4f %.4f %.4f", curtlv->sz, curtlv->sy, curtlv->sx );
			ret = WriteChar( hfile, tempchar, 1 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObjectMorph : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObjectMorph : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// face
	//	face 511 {
	//		4 V(487 19 4 482) M(0) UV(0.73213 0.24561 0.72100 0.24600 0.72100 0.26500 0.73217 0.26610)
	//		...	
	//	}

	int outputfacenum;
//	outputfacenum = meshinfo->n;

	if( selem->clockwise == 3 ){
		outputfacenum = meshinfo->n * 2;
	}else{
		outputfacenum = meshinfo->n;
	}

	sprintf_s( tempchar, LINECHARLENG, "\tface %d {", outputfacenum );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObjectMorph : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno;
	int i1, i2, i3;
	int curmatno;
//	D3DTLVERTEX* tlv1;
//	D3DTLVERTEX* tlv2;
//	D3DTLVERTEX* tlv3;
//	SKINVERTEX* sv1;
//	SKINVERTEX* sv2;
//	SKINVERTEX* sv3;
	TEXV* tv1;
	TEXV* tv2;
	TEXV* tv3;

	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		if( selem->clockwise == 1 ){
			i1 = *( optindexbuf + faceno * 3 );
			i2 = *( optindexbuf + faceno * 3 + 2 );
			i3 = *( optindexbuf + faceno * 3 + 1 );
		}else if( selem->clockwise == 2 ){
			i1 = *( optindexbuf + faceno * 3 );
			i2 = *( optindexbuf + faceno * 3 + 1 );
			i3 = *( optindexbuf + faceno * 3 + 2 );
		}else{
			i1 = *( optindexbuf + faceno * 3 );
			i2 = *( optindexbuf + faceno * 3 + 1 );
			i3 = *( optindexbuf + faceno * 3 + 2 );
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
			DbgOut( "polymesh2 : WriteMQOObjectMorph : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		// materialno
		if( curmatno >= 0 ){
			sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObjectMorph : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			//if( srctexname && *srctexname ){
				// UV
				
				tv1 = wv + i1;
				tv2 = wv + i2;
				tv3 = wv + i3;
				sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
					tv1->uv.x, tv1->uv.y,
					tv2->uv.x, tv2->uv.y,
					tv3->uv.x, tv3->uv.y
				);

				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh2 : WriteMQOObjectMorph : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			//}
		}

		if( vcoldata ){
			int setflag1, setflag2, setflag3;
			__int64 vc1;
			__int64 vc2;
			__int64 vc3;

			GetVCol2( (faceno * 3), &vc1, &setflag1 );
			GetVCol2( (faceno * 3 + 1), &vc2, &setflag2 );
			GetVCol2( (faceno * 3 + 2), &vc3, &setflag3 );
	
			if( setflag1 && setflag2 && setflag3 ){
				sprintf_s( tempchar, LINECHARLENG, " COL(%u", vc1 );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "pm2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				sprintf_s( tempchar, LINECHARLENG, " %u", vc2 );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "pm2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				sprintf_s( tempchar, LINECHARLENG, " %u)", vc3 );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "pm2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}


		// 改行
		strcpy_s( tempchar, LINECHARLENG, "\r\n" );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( selem->clockwise == 3 ){
		//裏面の出力
		for( faceno = 0; faceno < meshinfo->n; faceno++ ){
			if( selem->clockwise == 1 ){
				i1 = *( optindexbuf + faceno * 3 );
				i2 = *( optindexbuf + faceno * 3 + 1 );
				i3 = *( optindexbuf + faceno * 3 + 2 );
			}else if( selem->clockwise == 2 ){
				i1 = *( optindexbuf + faceno * 3 );
				i2 = *( optindexbuf + faceno * 3 + 2 );
				i3 = *( optindexbuf + faceno * 3 + 1 );
			}else{
				i1 = *( optindexbuf + faceno * 3 );
				i2 = *( optindexbuf + faceno * 3 + 2 );
				i3 = *( optindexbuf + faceno * 3 + 1 );
			}
	
			if( matnoindex ){
				curmatno = *( matnoindex + faceno );
			}else{
				curmatno = *( m_attrib0 + faceno );
			}
			
			// faceno
			sprintf_s( tempchar, LINECHARLENG, "\t\t3 V(%d %d %d)", i1 + optpleng, i2 + optpleng, i3 + optpleng );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			// materialno
			if( curmatno >= 0 ){
				sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				//if( srctexname && *srctexname ){
					// UV
					tv1 = wv + i1;
					tv2 = wv + i2;
					tv3 = wv + i3;
					sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
						tv1->uv.x, tv1->uv.y,
						tv2->uv.x, tv2->uv.y,
						tv3->uv.x, tv3->uv.y
					);

					ret = WriteChar( hfile, tempchar, 0 );
					if( ret ){
						DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				//}
			}

			// 改行
			strcpy_s( tempchar, LINECHARLENG, "\r\n" );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}


	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	free( wv );

	return 0;
}


int CPolyMesh2::WriteMQOObjectOnFrame( HANDLE hfile, int* matnoindex, char* srctexname, int motid, int frameno, int serialno, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult, int writeRevFlag )
{
	int ret;
	char tempchar[LINECHARLENG];

// vertex
	//	vertex 9 {
	//		15.0621 87.7051 -57.7583
	//		...
	//	}

	CShdElem* selem;
	selem = (*lpsh)( serialno );

	if( selem->clockwise != 3 ){
		sprintf_s( tempchar, LINECHARLENG, "\tvertex %d {", optpleng );
	}else{
		sprintf_s( tempchar, LINECHARLENG, "\tvertex %d {", optpleng * 2 );
	}
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3* wvec;
	wvec = new D3DXVECTOR3[ optpleng ];
	if( !wvec ){
		DbgOut( "pm2 : WriteMQOObjectOnFrame : wvec alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !selem ){
		DbgOut( "pm2 : WriteMQOObjectOnFrame : selem NULL error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}

	ret = lpmh->SetCurrentMotion( lpsh, motid, frameno );
	if( ret ){
		DbgOut( "pm2 : WriteMQOObjectOnFrame : mh SetCurrentMotion error !!!\n" );
		_ASSERT( 0 );
		delete [] wvec;
		return 1;
	}

	D3DXMATRIX iniwmat;
	D3DXMatrixIdentity( &iniwmat );
	if( selem->m_mtype == M_NONE ){
		ret = selem->TransformOnlyWorld3( lpmh, iniwmat, wvec );
		if( ret ){
			DbgOut( "pm2 : WriteMQOObjectOnFrame : selem TransformOnlyWorld3 error !!!\n" );
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




	int vertno;
	D3DXVECTOR3* curv;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		curv = wvec + vertno;

		//!!!!!!!
		// Z の符号を反転する。
		//!!!!!!!
		sprintf_s( tempchar, LINECHARLENG, "\t\t%.4f %.4f %.4f", curv->x * srcmult, curv->y * srcmult, -curv->z * srcmult );
		//sprintf( tempchar, "\t\t%.4f %.4f %.4f", curtlv->sz, curtlv->sy, curtlv->sx );
		ret = WriteChar( hfile, tempchar, 1 );
		if( ret ){
			DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( selem->clockwise == 3 ){
		for( vertno = 0; vertno < optpleng; vertno++ ){
			curv = wvec + vertno;

			//!!!!!!!
			// Z の符号を反転する。
			//!!!!!!!
			sprintf_s( tempchar, LINECHARLENG, "\t\t%.4f %.4f %.4f", curv->x * srcmult, curv->y * srcmult, -curv->z * srcmult );
			//sprintf( tempchar, "\t\t%.4f %.4f %.4f", curtlv->sz, curtlv->sy, curtlv->sx );
			ret = WriteChar( hfile, tempchar, 1 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	delete [] wvec;


	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// face
	//	face 511 {
	//		4 V(487 19 4 482) M(0) UV(0.73213 0.24561 0.72100 0.24600 0.72100 0.26500 0.73217 0.26610)
	//		...	
	//	}

	int outputfacenum;
//	outputfacenum = meshinfo->n;

	if( selem->clockwise == 3 ){
		outputfacenum = meshinfo->n * 2;
	}else{
		outputfacenum = meshinfo->n;
	}

	sprintf_s( tempchar, LINECHARLENG, "\tface %d {", outputfacenum );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno;
	int i1, i2, i3;
	int curmatno;
	D3DTLVERTEX* tlv1;
	D3DTLVERTEX* tlv2;
	D3DTLVERTEX* tlv3;

	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		if( selem->clockwise == 1 ){
			i1 = *( optindexbuf + faceno * 3 );
			i2 = *( optindexbuf + faceno * 3 + 2 );
			i3 = *( optindexbuf + faceno * 3 + 1 );
		}else if( selem->clockwise == 2 ){
			i1 = *( optindexbuf + faceno * 3 );
			i2 = *( optindexbuf + faceno * 3 + 1 );
			i3 = *( optindexbuf + faceno * 3 + 2 );
		}else{
			i1 = *( optindexbuf + faceno * 3 );
			i2 = *( optindexbuf + faceno * 3 + 1 );
			i3 = *( optindexbuf + faceno * 3 + 2 );
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
			DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		// materialno
		if( curmatno >= 0 ){
			sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			//if( srctexname && *srctexname ){
				// UV
				tlv1 = opttlv + i1;
				tlv2 = opttlv + i2;
				tlv3 = opttlv + i3;

				sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
					tlv1->tu, tlv1->tv,
					tlv2->tu, tlv2->tv,
					tlv3->tu, tlv3->tv			
				);

				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			//}
		}

		if( vcoldata ){
			int setflag1, setflag2, setflag3;
			__int64 vc1;
			__int64 vc2;
			__int64 vc3;

			GetVCol2( (faceno * 3), &vc1, &setflag1 );
			GetVCol2( (faceno * 3 + 1), &vc2, &setflag2 );
			GetVCol2( (faceno * 3 + 2), &vc3, &setflag3 );
	
			if( setflag1 && setflag2 && setflag3 ){
				sprintf_s( tempchar, LINECHARLENG, " COL(%u", vc1 );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "pm2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				sprintf_s( tempchar, LINECHARLENG, " %u", vc2 );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "pm2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				sprintf_s( tempchar, LINECHARLENG, " %u)", vc3 );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "pm2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}


		// 改行
		strcpy_s( tempchar, LINECHARLENG, "\r\n" );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( selem->clockwise == 3 ){
		//裏面の出力
		for( faceno = 0; faceno < meshinfo->n; faceno++ ){
			if( selem->clockwise == 1 ){
				i1 = *( optindexbuf + faceno * 3 );
				i2 = *( optindexbuf + faceno * 3 + 1 );
				i3 = *( optindexbuf + faceno * 3 + 2 );
			}else if( selem->clockwise == 2 ){
				i1 = *( optindexbuf + faceno * 3 );
				i2 = *( optindexbuf + faceno * 3 + 2 );
				i3 = *( optindexbuf + faceno * 3 + 1 );
			}else{
				i1 = *( optindexbuf + faceno * 3 );
				i2 = *( optindexbuf + faceno * 3 + 2 );
				i3 = *( optindexbuf + faceno * 3 + 1 );
			}
	
			if( matnoindex ){
				curmatno = *( matnoindex + faceno );
			}else{
				curmatno = *( m_attrib0 + faceno );
			}
			
			// faceno
			sprintf_s( tempchar, LINECHARLENG, "\t\t3 V(%d %d %d)", i1 + optpleng, i2 + optpleng, i3 + optpleng );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			// materialno
			if( curmatno >= 0 ){
				sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				//if( srctexname && *srctexname ){
					// UV
					tlv1 = opttlv + i1;
					tlv2 = opttlv + i2;
					tlv3 = opttlv + i3;

					sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
						tlv1->tu, tlv1->tv,
						tlv2->tu, tlv2->tv,
						tlv3->tu, tlv3->tv			
					);

					ret = WriteChar( hfile, tempchar, 0 );
					if( ret ){
						DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				//}
			}

			// 改行
			strcpy_s( tempchar, LINECHARLENG, "\r\n" );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}


	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CPolyMesh2::WriteMQOObject( HANDLE hfile, int* matnoindex, char* srctexname, CShdElem* selem )
{

	int ret;
	char tempchar[LINECHARLENG];

// vertex
	//	vertex 9 {
	//		15.0621 87.7051 -57.7583
	//		...
	//	}	
	sprintf_s( tempchar, LINECHARLENG, "\tvertex %d {", optpleng );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int vertno;
	D3DTLVERTEX* curv;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		curv = opttlv + vertno;

		//!!!!!!!
		// Z の符号を反転する。
		//!!!!!!!


		sprintf_s( tempchar, LINECHARLENG, "\t\t%.4f %.4f %.4f", curv->sx, curv->sy, -curv->sz );
		//sprintf( tempchar, "\t\t%.4f %.4f %.4f", curtlv->sz, curtlv->sy, curtlv->sx );
		ret = WriteChar( hfile, tempchar, 1 );
		if( ret ){
			DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// face
	//	face 511 {
	//		4 V(487 19 4 482) M(0) UV(0.73213 0.24561 0.72100 0.24600 0.72100 0.26500 0.73217 0.26610)
	//		...	
	//	}

	int outputfacenum;
	outputfacenum = meshinfo->n;

//	if( selem->clockwise == 3 ){
//		outputfacenum = meshinfo->n * 2;
//	}else{
//		outputfacenum = meshinfo->n;
//	}

	sprintf_s( tempchar, LINECHARLENG, "\tface %d {", outputfacenum );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno;
	int i1, i2, i3;
	int curmatno;
	D3DTLVERTEX* tlv1;
	D3DTLVERTEX* tlv2;
	D3DTLVERTEX* tlv3;

	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		if( selem->clockwise == 1 ){
			i1 = *( optindexbuf + faceno * 3 );
			i2 = *( optindexbuf + faceno * 3 + 2 );
			i3 = *( optindexbuf + faceno * 3 + 1 );
		}else if( selem->clockwise == 2 ){
			i1 = *( optindexbuf + faceno * 3 );
			i2 = *( optindexbuf + faceno * 3 + 1 );
			i3 = *( optindexbuf + faceno * 3 + 2 );
		}else{
			i1 = *( optindexbuf + faceno * 3 );
			i2 = *( optindexbuf + faceno * 3 + 1 );
			i3 = *( optindexbuf + faceno * 3 + 2 );
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
			DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		// materialno
		if( curmatno >= 0 ){
			sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			//if( srctexname && *srctexname ){
				// UV
				tlv1 = opttlv + i1;
				tlv2 = opttlv + i2;
				tlv3 = opttlv + i3;

				sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
					tlv1->tu, tlv1->tv,
					tlv2->tu, tlv2->tv,
					tlv3->tu, tlv3->tv			
				);

				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			//}
		}

		if( vcoldata ){
			int setflag1, setflag2, setflag3;
			__int64 vc1;
			__int64 vc2;
			__int64 vc3;

			GetVCol2( (faceno * 3), &vc1, &setflag1 );
			GetVCol2( (faceno * 3 + 1), &vc2, &setflag2 );
			GetVCol2( (faceno * 3 + 2), &vc3, &setflag3 );
	
			if( setflag1 && setflag2 && setflag3 ){
				sprintf_s( tempchar, LINECHARLENG, " COL(%u", vc1 );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "pm2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				sprintf_s( tempchar, LINECHARLENG, " %u", vc2 );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "pm2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				sprintf_s( tempchar, LINECHARLENG, " %u)", vc3 );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "pm2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}



		// 改行
		strcpy_s( tempchar, LINECHARLENG, "\r\n" );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	/***
	if( selem->clockwise == 3 ){
		//裏面の出力
		for( faceno = 0; faceno < meshinfo->n; faceno++ ){
			if( selem->clockwise == 1 ){
				i1 = *( optindexbuf + faceno * 3 );
				i2 = *( optindexbuf + faceno * 3 + 1 );
				i3 = *( optindexbuf + faceno * 3 + 2 );
			}else if( selem->clockwise == 2 ){
				i1 = *( optindexbuf + faceno * 3 );
				i2 = *( optindexbuf + faceno * 3 + 2 );
				i3 = *( optindexbuf + faceno * 3 + 1 );
			}else{
				i1 = *( optindexbuf + faceno * 3 );
				i2 = *( optindexbuf + faceno * 3 + 2 );
				i3 = *( optindexbuf + faceno * 3 + 1 );
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
				DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			// materialno
			if( curmatno >= 0 ){
				sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( srctexname && *srctexname ){
					// UV
					tlv1 = opttlv + i1;
					tlv2 = opttlv + i2;
					tlv3 = opttlv + i3;

					sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
						tlv1->tu, tlv1->tv,
						tlv2->tu, tlv2->tv,
						tlv3->tu, tlv3->tv			
					);

					ret = WriteChar( hfile, tempchar, 0 );
					if( ret ){
						DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}

			// 改行
			strcpy_s( tempchar, LINECHARLENG, "\r\n" );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}


	}
	***/


	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

/***
int CPolyMesh2::WriteMQOObject( HANDLE hfile, int* matnoindex, char* srctexname )
{
	int ret;
	char tempchar[LINECHARLENG];

// vertex
	//	vertex 9 {
	//		15.0621 87.7051 -57.7583
	//		...
	//	}	
	sprintf_s( tempchar, LINECHARLENG, "\tvertex %d {", optpleng );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int vertno;
	D3DTLVERTEX* curtlv;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		curtlv = opttlv + vertno;

		//!!!!!!!
		// Z の符号を反転する。
		//!!!!!!!


		sprintf_s( tempchar, LINECHARLENG, "\t\t%.4f %.4f %.4f", curtlv->sx, curtlv->sy, -curtlv->sz );
		//sprintf( tempchar, "\t\t%.4f %.4f %.4f", curtlv->sz, curtlv->sy, curtlv->sx );
		ret = WriteChar( hfile, tempchar, 1 );
		if( ret ){
			DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	
	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
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
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno;
	int i1, i2, i3;
	int curmatno;
	D3DTLVERTEX* tlv1;
	D3DTLVERTEX* tlv2;
	D3DTLVERTEX* tlv3;

	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		i1 = *( optindexbuf + faceno * 3 );
		i2 = *( optindexbuf + faceno * 3 + 1 );
		i3 = *( optindexbuf + faceno * 3 + 2 );
		//i1 = *( optindexbuf + faceno * 3 );
		//i2 = *( optindexbuf + faceno * 3 + 2 );
		//i3 = *( optindexbuf + faceno * 3 + 1 );
		
		curmatno = *( matnoindex + faceno );
		
		// faceno
		sprintf_s( tempchar, LINECHARLENG, "\t\t3 V(%d %d %d)", i1, i2, i3 );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		// materialno
		if( curmatno >= 0 ){
			sprintf_s( tempchar, LINECHARLENG, " M(%d)", curmatno );
			ret = WriteChar( hfile, tempchar, 0 );
			if( ret ){
				DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( srctexname && *srctexname ){
				// UV
				tlv1 = opttlv + i1;
				tlv2 = opttlv + i2;
				tlv3 = opttlv + i3;

				sprintf_s( tempchar, LINECHARLENG, " UV(%.5f %.5f %.5f %.5f %.5f %.5f)",
					tlv1->tu, tlv1->tv,
					tlv2->tu, tlv2->tv,
					tlv3->tu, tlv3->tv			
				);

				ret = WriteChar( hfile, tempchar, 0 );
				if( ret ){
					DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

		// 改行
		strcpy_s( tempchar, LINECHARLENG, "\r\n" );
		ret = WriteChar( hfile, tempchar, 0 );
		if( ret ){
			DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	strcpy_s( tempchar, LINECHARLENG, "\t}" );
	ret = WriteChar( hfile, tempchar, 1 );
	if( ret ){
		DbgOut( "polymesh2 : WriteMQOObject : WriteChar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/


int CPolyMesh2::WriteChar( HANDLE hfile, char* writechar, int addreturn )
{
	unsigned long wleng, writeleng;

	if( addreturn )
		strcat_s( writechar, LINECHARLENG, "\r\n" );

	wleng = (unsigned long)strlen( writechar );
	if( wleng >= LINECHARLENG ){
		DbgOut( "polymesh2 : WriteChar : wleng error !!!\n" );
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




int CPolyMesh2::GetDiffuse( int optvertno, ARGBF* dstcol )
{
	if( (optvertno < 0) || (optvertno >= optpleng) ){
		DbgOut( "polymesh2 : GetDiffuse : optvertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD dwcol;
	dwcol = (opttlv + optvertno)->color;
	int cola, colr, colg, colb;
	cola = (dwcol & 0xFF000000) >> 24;
	colr = (dwcol & 0x00FF0000) >> 16;
	colg = (dwcol & 0x0000FF00) >> 8;
	colb = (dwcol & 0x000000FF);

	dstcol->a = cola / 255.0f;
	dstcol->r = colr / 255.0f;
	dstcol->g = colg / 255.0f;
	dstcol->b = colb / 255.0f;


	return 0;
}
int CPolyMesh2::GetAmbient( int optvertno, ARGBF* dstcol )
{
	if( (optvertno < 0) || (optvertno >= optpleng) ){
		DbgOut( "polymesh2 : GetAmbient : optvertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstcol = *( optambient + optvertno );

	return 0;
}
int CPolyMesh2::GetEmissive( int optvertno, ARGBF* dstcol )
{
	if( (optvertno < 0) || (optvertno >= optpleng) ){
		DbgOut( "polymesh2 : GetEmissive : optvertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstcol = *( optemissivebuf + optvertno );
	return 0;
}

int CPolyMesh2::GetSpecularPower( int optvertno, float* powerptr )
{
	if( (optvertno < 0) || (optvertno >= optpleng) ){
		DbgOut( "polymesh2 : GetSpecularPower : optvertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*powerptr = *( optpowerbuf + optvertno );
	return 0;

}


int CPolyMesh2::GetSpecular( int optvertno, ARGBF* dstcol )
{
	if( (optvertno < 0) || (optvertno >= optpleng) ){
		DbgOut( "polymesh2 : GetSpecular : optvertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD dwcol;
	dwcol = (opttlv + optvertno)->specular;
	int cola, colr, colg, colb;
	cola = (dwcol & 0xFF000000) >> 24;
	colr = (dwcol & 0x00FF0000) >> 16;
	colg = (dwcol & 0x0000FF00) >> 8;
	colb = (dwcol & 0x000000FF);

	dstcol->a = cola / 255.0f;
	dstcol->r = colr / 255.0f;
	dstcol->g = colg / 255.0f;
	dstcol->b = colb / 255.0f;

	return 0;
}

int CPolyMesh2::GetMaterialDiffuse( int matno, ARGBF* dstcol )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int vertno;
	int firstv = 0;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){
			firstv = vertno;
			break;
		}
	}

	DWORD dwcol;
	dwcol = (opttlv + firstv)->color;
	int cola, colr, colg, colb;
	cola = (dwcol & 0xFF000000) >> 24;
	colr = (dwcol & 0x00FF0000) >> 16;
	colg = (dwcol & 0x0000FF00) >> 8;
	colb = (dwcol & 0x000000FF);

	dstcol->a = cola / 255.0f;
	dstcol->r = colr / 255.0f;
	dstcol->g = colg / 255.0f;
	dstcol->b = colb / 255.0f;
	
	return 0;
}
int CPolyMesh2::GetMaterialSpecular( int matno, ARGBF* dstcol )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int vertno;
	int firstv = 0;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){
			firstv = vertno;
			break;
		}
	}

	DWORD dwcol;
	dwcol = (opttlv + firstv)->specular;
	int cola, colr, colg, colb;
	cola = (dwcol & 0xFF000000) >> 24;
	colr = (dwcol & 0x00FF0000) >> 16;
	colg = (dwcol & 0x0000FF00) >> 8;
	colb = (dwcol & 0x000000FF);

	dstcol->a = cola / 255.0f;
	dstcol->r = colr / 255.0f;
	dstcol->g = colg / 255.0f;
	dstcol->b = colb / 255.0f;

	return 0;
}
int CPolyMesh2::GetMaterialAmbient( int matno, ARGBF* dstcol )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int vertno;
	int firstv = 0;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){
			firstv = vertno;
			break;
		}
	}

	*dstcol = *( optambient + firstv );


	return 0;
}
int CPolyMesh2::GetMaterialEmissive( int matno, ARGBF* dstcol )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int vertno;
	int firstv = 0;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){
			firstv = vertno;
			break;
		}
	}

	*dstcol = *( optemissivebuf + firstv );

	return 0;
}
int CPolyMesh2::GetMaterialPower( int matno, float* dstpower )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int vertno;
	int firstv = 0;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){
			firstv = vertno;
			break;
		}
	}

	*dstpower = *( optpowerbuf + firstv );

	return 0;
}

int CPolyMesh2::SetDiffuse( int optvertno, int setflag, ARGBF srccol, CD3DDisp* d3dptr, int tlmode )
{
	int ret;
	if( optvertno < 0 ){
		int vno;
		for( vno = 0; vno < optpleng; vno++ ){
			ret = SetDiffuse( vno, setflag, srccol, d3dptr, tlmode );
			if( ret ){
				DbgOut( "polymesh2 : SetDiffuse vno %d error !!!\n", vno );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		if( optvertno >= optpleng ){
			DbgOut( "polymesh2 : SetDiffuse : optvertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		float fr, fg, fb;
		int ir, ig, ib;
		fr = srccol.r;
		fg = srccol.g;
		fb = srccol.b;
		ir = (int)( fr * 255.0f );
		ig = (int)( fg * 255.0f );
		ib = (int)( fb * 255.0f );


		DWORD basecol = (opttlv + optvertno)->color;
		int basea, baser, baseg, baseb;
		basea = (basecol & 0xFF000000) >> 24;
		baser = (basecol & 0x00FF0000) >> 16;
		baseg = (basecol & 0x0000FF00) >> 8;
		baseb = (basecol & 0x000000FF);
		

		int newr, newg, newb;

		switch( setflag ){
		case 0://そのままセット
			newr = ir;
			newg = ig;
			newb = ib;
			break;
		case 1://乗算
			newr = (int)( baser * fr );
			newg = (int)( baseg * fg );
			newb = (int)( baseb * fb );
			break;
		case 2://足し算
			newr = baser + ir;
			if( newr > 255 )
				newr = 255;

			newg = baseg + ig;
			if( newg > 255 )
				newg = 255;

			newb = baseb + ib;
			if( newb > 255 )
				newb = 255;

			break;
		case 3://引き算
			newr = baser - ir;
			if( newr < 0 )
				newr = 0;

			newg = baseg - ig;
			if( newg < 0 )
				newg = 0;

			newb = baseb - ib;
			if( newb < 0 )
				newb = 0;
			break;
		default:
			newr = 0;
			newg = 0;
			newb = 0;
			break;
		}

		//opttlvのセット
		DWORD setcol;
		setcol = D3DCOLOR_ARGB( basea, newr, newg, newb );
		(opttlv + optvertno)->color = setcol;

		
		// disp データの更新
		if( d3dptr && (tlmode == TLMODE_D3D) ){
			ret = d3dptr->SetOrgTlvColor( optvertno, setcol, MAT_DIFFUSE );
			if( ret ){
				DbgOut( "polymesh2 : SetDiffuse : d3dptr SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		
	}

	return 0;
}

int CPolyMesh2::SetAmbient( int optvertno, int setflag, ARGBF srccol )
{
	int ret;
	if( optvertno < 0 ){
		int vno;
		for( vno = 0; vno < optpleng; vno++ ){
			ret = SetAmbient( vno, setflag, srccol );
			if( ret ){
				DbgOut( "polymesh2 : SetAmbient vno %d error !!!\n", vno );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		if( optvertno >= optpleng ){
			DbgOut( "polymesh2 : SetAmbient : optvertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float fr, fg, fb;
		fr = srccol.r;
		fg = srccol.g;
		fb = srccol.b;

		ARGBF basecol = *(optambient + optvertno);

		float basea, baser, baseg, baseb;
		basea = basecol.a;
		baser = basecol.r;
		baseg = basecol.g;
		baseb = basecol.b;
		
		ARGBF newargb;
		newargb.a = basea;

		switch( setflag ){
		case 0://そのままセット
			newargb.r = fr;
			newargb.g = fg;
			newargb.b = fb;
			break;
		case 1://乗算
			newargb.r = baser * fr;
			newargb.g = baseg * fg;
			newargb.b = baseb * fb;
			break;
		case 2://足し算
			newargb.r = baser + fr;
			if( newargb.r > 1.0f )
				newargb.r = 1.0f;

			newargb.g = baseg + fg;
			if( newargb.g > 1.0f )
				newargb.g = 1.0f;

			newargb.b = baseb + fb;
			if( newargb.b > 1.0f )
				newargb.b = 1.0f;

			break;
		case 3://引き算
			newargb.r = baser - fr;
			if( newargb.r < 0.0f )
				newargb.r = 0.0f;

			newargb.g = baseg - fg;
			if( newargb.g < 0.0f )
				newargb.g = 0.0f;

			newargb.b = baseb - fb;
			if( newargb.b < 0.0f )
				newargb.b = 0.0f;
			
			break;
		default:
			break;
		}

		//optambientのセット
		*(optambient + optvertno) = newargb;

	}

	return 0;
}

int CPolyMesh2::SetEmissive( int optvertno, int setflag, ARGBF srccol, CD3DDisp* d3dptr, int tlmode )
{
	int ret;
	if( optvertno < 0 ){
		int vno;
		for( vno = 0; vno < optpleng; vno++ ){
			ret = SetEmissive( vno, setflag, srccol, d3dptr, tlmode );
			if( ret ){
				DbgOut( "polymesh2 : SetEmissive vno %d error !!!\n", vno );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		if( optvertno >= optpleng ){
			DbgOut( "polymesh2 : SetEmissive : optvertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		float fr, fg, fb;
		fr = srccol.r;
		fg = srccol.g;
		fb = srccol.b;

		float basea, baser, baseg, baseb;
		basea = (optemissivebuf + optvertno)->a;
		baser = (optemissivebuf + optvertno)->r;
		baseg = (optemissivebuf + optvertno)->g;
		baseb = (optemissivebuf + optvertno)->b;

		float newr, newg, newb;

		switch( setflag ){
		case 0://そのままセット
			newr = fr;
			newg = fg;
			newb = fb;
			break;
		case 1://乗算
			newr = baser * fr;
			newg = baseg * fg;
			newb = baseb * fb;
			break;
		case 2://足し算
			newr = baser + fr;
			if( newr > 1.0f )
				newr = 1.0f;

			newg = baseg + fg;
			if( newg > 1.0f )
				newg = 1.0f;

			newb = baseb + fb;
			if( newb > 1.0f )
				newb = 1.0f;

			break;
		case 3://引き算
			newr = baser - fr;
			if( newr < 0.0f )
				newr = 0.0f;

			newg = baseg - fg;
			if( newg < 0.0f )
				newg = 0.0f;

			newb = baseb - fb;
			if( newb < 0.0f )
				newb = 0.0f;
			break;
		default:
			newr = 0.0f;
			newg = 0.0f;
			newb = 0.0f;
			break;
		}

		(optemissivebuf + optvertno)->a = basea;
		(optemissivebuf + optvertno)->r = newr;
		(optemissivebuf + optvertno)->g = newg;
		(optemissivebuf + optvertno)->b = newb;


		/***		
		// disp データの更新
		if( d3dptr && (tlmode == TLMODE_D3D) ){
			ret = d3dptr->SetOrgTlvColor( optvertno, setcol, MAT_SPECULAR );
			if( ret ){
				DbgOut( "polymesh2 : SetSpecular : d3dptr SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		***/

	}

	return 0;

}
int CPolyMesh2::SetSpecularPower( int optvertno, int setflag, float srcpow, CD3DDisp* d3dptr, int tlmode )
{
	int ret;
	if( optvertno < 0 ){
		int vno;
		for( vno = 0; vno < optpleng; vno++ ){
			ret = SetSpecularPower( vno, setflag, srcpow, d3dptr, tlmode );
			if( ret ){
				DbgOut( "polymesh2 : SetSpecularPower vno %d error !!!\n", vno );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		if( optvertno >= optpleng ){
			DbgOut( "polymesh2 : SetSpecularPower : optvertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}



		float basepow = *( optpowerbuf + optvertno );
		float newpow;

		switch( setflag ){
		case 0://そのままセット
			newpow = srcpow;
			break;
		case 1://乗算
			newpow = basepow * srcpow;
			break;
		case 2://足し算
			newpow = basepow + srcpow;
			if( newpow > 100.0f )
				newpow = 100.0f;
			break;
		case 3://引き算
			newpow = basepow - srcpow;
			if( newpow < 0.0f )
				newpow = 0.0f;
			break;
		default:
			newpow = 0.0f;
			break;
		}

		*( optpowerbuf + optvertno ) = newpow;

		/***		
		// disp データの更新
		if( d3dptr && (tlmode == TLMODE_D3D) ){
			ret = d3dptr->SetOrgTlvColor( optvertno, setcol, MAT_SPECULAR );
			if( ret ){
				DbgOut( "polymesh2 : SetSpecular : d3dptr SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		***/

	}

	return 0;

}


int CPolyMesh2::SetSpecular( int optvertno, int setflag, ARGBF srccol, CD3DDisp* d3dptr, int tlmode )
{
	int ret;
	if( optvertno < 0 ){
		int vno;
		for( vno = 0; vno < optpleng; vno++ ){
			ret = SetSpecular( vno, setflag, srccol, d3dptr, tlmode );
			if( ret ){
				DbgOut( "polymesh2 : SetSpecular vno %d error !!!\n", vno );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		if( optvertno >= optpleng ){
			DbgOut( "polymesh2 : SetSpecular : optvertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		float fr, fg, fb;
		int ir, ig, ib;
		fr = srccol.r;
		fg = srccol.g;
		fb = srccol.b;
		ir = (int)( fr * 255.0f );
		ig = (int)( fg * 255.0f );
		ib = (int)( fb * 255.0f );


		DWORD basecol = (opttlv + optvertno)->specular;
		int basea, baser, baseg, baseb;
		basea = (basecol & 0xFF000000) >> 24;
		baser = (basecol & 0x00FF0000) >> 16;
		baseg = (basecol & 0x0000FF00) >> 8;
		baseb = (basecol & 0x000000FF);
		

		int newr, newg, newb;

		switch( setflag ){
		case 0://そのままセット
			newr = ir;
			newg = ig;
			newb = ib;
			break;
		case 1://乗算
			newr = (int)( baser * fr );
			newg = (int)( baseg * fg );
			newb = (int)( baseb * fb );
			break;
		case 2://足し算
			newr = baser + ir;
			if( newr > 255 )
				newr = 255;

			newg = baseg + ig;
			if( newg > 255 )
				newg = 255;

			newb = baseb + ib;
			if( newb > 255 )
				newb = 255;

			break;
		case 3://引き算
			newr = baser - ir;
			if( newr < 0 )
				newr = 0;

			newg = baseg - ig;
			if( newg < 0 )
				newg = 0;

			newb = baseb - ib;
			if( newb < 0 )
				newb = 0;
			break;
		default:
			newr = 0;
			newg = 0;
			newb = 0;
			break;
		}

		//opttlvのセット
		DWORD setcol;
		setcol = D3DCOLOR_ARGB( basea, newr, newg, newb );
		(opttlv + optvertno)->specular = setcol;

		
		// disp データの更新
		if( d3dptr && (tlmode == TLMODE_D3D) ){
			ret = d3dptr->SetOrgTlvColor( optvertno, setcol, MAT_SPECULAR );
			if( ret ){
				DbgOut( "polymesh2 : SetSpecular : d3dptr SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int CPolyMesh2::SetMaterialDiffuse( int matno, int setflag, ARGBF srccol )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	float fr, fg, fb;
	int ir, ig, ib;
	fr = srccol.r;
	fg = srccol.g;
	fb = srccol.b;
	ir = (int)( fr * 255.0f );
	ig = (int)( fg * 255.0f );
	ib = (int)( fb * 255.0f );

	int vertno;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		
		if( *( dirtyptr + vertno ) != 0 ){

			DWORD basecol = (opttlv + vertno)->color;
			int basea, baser, baseg, baseb;
			basea = (basecol & 0xFF000000) >> 24;
			baser = (basecol & 0x00FF0000) >> 16;
			baseg = (basecol & 0x0000FF00) >> 8;
			baseb = (basecol & 0x000000FF);
			

			int newr, newg, newb;

			switch( setflag ){
			case 0://そのままセット
				newr = ir;
				newg = ig;
				newb = ib;
				break;
			case 1://乗算
				newr = (int)( baser * fr );
				newg = (int)( baseg * fg );
				newb = (int)( baseb * fb );
				break;
			case 2://足し算
				newr = baser + ir;
				if( newr > 255 )
					newr = 255;

				newg = baseg + ig;
				if( newg > 255 )
					newg = 255;

				newb = baseb + ib;
				if( newb > 255 )
					newb = 255;

				break;
			case 3://引き算
				newr = baser - ir;
				if( newr < 0 )
					newr = 0;

				newg = baseg - ig;
				if( newg < 0 )
					newg = 0;

				newb = baseb - ib;
				if( newb < 0 )
					newb = 0;
				break;
			default:
				newr = 0;
				newg = 0;
				newb = 0;
				break;
			}

			//opttlvのセット
			DWORD setcol;
			setcol = D3DCOLOR_ARGB( basea, newr, newg, newb );
			(opttlv + vertno)->color = setcol;
		}

	}
	return 0;
}
int CPolyMesh2::SetMaterialSpecular( int matno, int setflag, ARGBF srccol )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	float fr, fg, fb;
	int ir, ig, ib;
	fr = srccol.r;
	fg = srccol.g;
	fb = srccol.b;
	ir = (int)( fr * 255.0f );
	ig = (int)( fg * 255.0f );
	ib = (int)( fb * 255.0f );


	int vertno;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		
		if( *( dirtyptr + vertno ) != 0 ){

			DWORD basecol = (opttlv + vertno)->specular;
			int basea, baser, baseg, baseb;
			basea = (basecol & 0xFF000000) >> 24;
			baser = (basecol & 0x00FF0000) >> 16;
			baseg = (basecol & 0x0000FF00) >> 8;
			baseb = (basecol & 0x000000FF);
			

			int newr, newg, newb;

			switch( setflag ){
			case 0://そのままセット
				newr = ir;
				newg = ig;
				newb = ib;
				break;
			case 1://乗算
				newr = (int)( baser * fr );
				newg = (int)( baseg * fg );
				newb = (int)( baseb * fb );
				break;
			case 2://足し算
				newr = baser + ir;
				if( newr > 255 )
					newr = 255;

				newg = baseg + ig;
				if( newg > 255 )
					newg = 255;

				newb = baseb + ib;
				if( newb > 255 )
					newb = 255;

				break;
			case 3://引き算
				newr = baser - ir;
				if( newr < 0 )
					newr = 0;

				newg = baseg - ig;
				if( newg < 0 )
					newg = 0;

				newb = baseb - ib;
				if( newb < 0 )
					newb = 0;
				break;
			default:
				newr = 0;
				newg = 0;
				newb = 0;
				break;
			}

			//opttlvのセット
			DWORD setcol;
			setcol = D3DCOLOR_ARGB( basea, newr, newg, newb );
			(opttlv + vertno)->specular = setcol;

		}

	}

	return 0;
}
int CPolyMesh2::SetMaterialAmbient( int matno, int setflag, ARGBF srccol )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	float fr, fg, fb;
	fr = srccol.r;
	fg = srccol.g;
	fb = srccol.b;


	int vertno;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		
		if( *( dirtyptr + vertno ) != 0 ){

			ARGBF basecol = *(optambient + vertno);

			float basea, baser, baseg, baseb;
			basea = basecol.a;
			baser = basecol.r;
			baseg = basecol.g;
			baseb = basecol.b;
			
			ARGBF newargb;
			newargb.a = basea;

			switch( setflag ){
			case 0://そのままセット
				newargb.r = fr;
				newargb.g = fg;
				newargb.b = fb;
				break;
			case 1://乗算
				newargb.r = baser * fr;
				newargb.g = baseg * fg;
				newargb.b = baseb * fb;
				break;
			case 2://足し算
				newargb.r = baser + fr;
				if( newargb.r > 1.0f )
					newargb.r = 1.0f;

				newargb.g = baseg + fg;
				if( newargb.g > 1.0f )
					newargb.g = 1.0f;

				newargb.b = baseb + fb;
				if( newargb.b > 1.0f )
					newargb.b = 1.0f;

				break;
			case 3://引き算
				newargb.r = baser - fr;
				if( newargb.r < 0.0f )
					newargb.r = 0.0f;

				newargb.g = baseg - fg;
				if( newargb.g < 0.0f )
					newargb.g = 0.0f;

				newargb.b = baseb - fb;
				if( newargb.b < 0.0f )
					newargb.b = 0.0f;
				
				break;
			default:
				break;
			}

			//optambientのセット
			*(optambient + vertno) = newargb;

		}

	}

	return 0;
}
int CPolyMesh2::SetMaterialEmissive( int matno, int setflag, ARGBF srccol )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	float fr, fg, fb;
	fr = srccol.r;
	fg = srccol.g;
	fb = srccol.b;

	int vertno;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		
		if( *( dirtyptr + vertno ) != 0 ){

			float basea, baser, baseg, baseb;
			basea = (optemissivebuf + vertno)->a;
			baser = (optemissivebuf + vertno)->r;
			baseg = (optemissivebuf + vertno)->g;
			baseb = (optemissivebuf + vertno)->b;

			float newr, newg, newb;

			switch( setflag ){
			case 0://そのままセット
				newr = fr;
				newg = fg;
				newb = fb;
				break;
			case 1://乗算
				newr = baser * fr;
				newg = baseg * fg;
				newb = baseb * fb;
				break;
			case 2://足し算
				newr = baser + fr;
				if( newr > 1.0f )
					newr = 1.0f;

				newg = baseg + fg;
				if( newg > 1.0f )
					newg = 1.0f;

				newb = baseb + fb;
				if( newb > 1.0f )
					newb = 1.0f;

				break;
			case 3://引き算
				newr = baser - fr;
				if( newr < 0.0f )
					newr = 0.0f;

				newg = baseg - fg;
				if( newg < 0.0f )
					newg = 0.0f;

				newb = baseb - fb;
				if( newb < 0.0f )
					newb = 0.0f;
				break;
			default:
				newr = 0.0f;
				newg = 0.0f;
				newb = 0.0f;
				break;
			}

			(optemissivebuf + vertno)->a = basea;
			(optemissivebuf + vertno)->r = newr;
			(optemissivebuf + vertno)->g = newg;
			(optemissivebuf + vertno)->b = newb;
		}

	}

	return 0;
}
int CPolyMesh2::SetMaterialPower( int matno, int setflag, float srcpow )
{
	int* dirtyptr;
	dirtyptr = GetDirtyMaterial( matno );
	if( !dirtyptr ){
		return 0;
	}

	int vertno;
	for( vertno = 0; vertno < optpleng; vertno++ ){
		
		if( *( dirtyptr + vertno ) != 0 ){

			float basepow = *( optpowerbuf + vertno );
			float newpow;

			switch( setflag ){
			case 0://そのままセット
				newpow = srcpow;
				break;
			case 1://乗算
				newpow = basepow * srcpow;
				break;
			case 2://足し算
				newpow = basepow + srcpow;
				if( newpow > 100.0f )
					newpow = 100.0f;
				break;
			case 3://引き算
				newpow = basepow - srcpow;
				if( newpow < 0.0f )
					newpow = 0.0f;
				break;
			default:
				newpow = 0.0f;
				break;
			}

			*( optpowerbuf + vertno ) = newpow;

		}

	}

	return 0;
}




/***
int CPolyMesh2::SetOptDiffuse( int srcr, int srcg, int srcb, int setflag, CD3DDisp* d3dptr, int tlmode )
{
	if( !opttlv ){
		DbgOut( "polymesh2 : SetOptDiffuse : opttlv NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( srcr < 0 )
		srcr = 0;
	if( srcr > 255 )
		srcr = 255;
	if( srcg < 0 )
		srcg = 0;
	if( srcg > 255 )
		srcg = 255;
	if( srcb < 0 )
		srcb = 0;
	if( srcb > 255 )
		srcb = 255;

	D3DTLVERTEX* tlvptr;
	unsigned int orgalpha;
	unsigned int orgcol, orgr, orgg, orgb;
	int newr, newg, newb;

	float rater, rateg, rateb;

	int tlvno;
	for( tlvno = 0; tlvno < optpleng; tlvno++ ){
		tlvptr = opttlv + tlvno;
		_ASSERT( tlvptr );

		orgalpha = (tlvptr->color & 0xFF000000) >> 24;

		switch( setflag ){
		case 0://そのままセット
			newr = srcr;
			newg = srcg;
			newb = srcb;
			tlvptr->color = D3DCOLOR_ARGB( orgalpha, srcr, srcg, srcb );
			break;
		case 1://乗算
			rater = (float)srcr / 255.0f;
			rateg = (float)srcg / 255.0f;
			rateb = (float)srcb / 255.0f;
				
			orgcol = tlvptr->color;
			orgr = (orgcol & 0x00FF0000) >> 16;
			orgg = (orgcol & 0x0000FF00) >> 8;
			orgb = (orgcol & 0x000000FF);

			newr = (unsigned int)((float)orgr * rater);
			newg = (unsigned int)((float)orgg * rateg);
			newb = (unsigned int)((float)orgb * rateb);

			tlvptr->color = D3DCOLOR_ARGB( orgalpha, newr, newg, newb );

			break;
		case 2://足し算
			orgcol = tlvptr->color;
			orgr = (orgcol & 0x00FF0000) >> 16;
			orgg = (orgcol & 0x0000FF00) >> 8;
			orgb = (orgcol & 0x000000FF);

			newr = orgr + srcr;
			if( newr > 255 )
				newr = 255;

			newg = orgg + srcg;
			if( newg > 255 )
				newg = 255;

			newb = orgb + srcb;
			if( newb > 255 )
				newb = 255;

			tlvptr->color = D3DCOLOR_ARGB( orgalpha, newr, newg, newb );

			break;
		case 3://引き算
			orgcol = tlvptr->color;
			orgr = (orgcol & 0x00FF0000) >> 16;
			orgg = (orgcol & 0x0000FF00) >> 8;
			orgb = (orgcol & 0x000000FF);

			newr = orgr - srcr;
			if( newr < 0 )
				newr = 0;

			newg = orgg - srcg;
			if( newg < 0 )
				newg = 0;
			
			newb = orgb - srcb;
			if( newb < 0 )
				newb = 0;


			tlvptr->color = D3DCOLOR_ARGB( orgalpha, newr, newg, newb );
				
			break;
		default:
			break;
		}
	}

	int ret;
	if( tlmode == TLMODE_D3D ){
		_ASSERT( d3dptr );

		CVec3f newrgb;
		newrgb.x = newr / 255.0f;
		newrgb.y = newg / 255.0f;
		newrgb.z = newb / 255.0f;

		ret = d3dptr->SetD3DVERTEX_RGB( newrgb, MAT_DIFFUSE );
		if( ret ){
			DbgOut( "polymesh2 : SetOptDiffuse : d3ddisp SetD3DVERTEX_RGB error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CPolyMesh2::SetOptSpecular( int srcr, int srcg, int srcb, int setflag, CD3DDisp* d3dptr, int tlmode )
{
	if( !opttlv ){
		DbgOut( "polymesh2 : SetOptSpecular : opttlv NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( srcr < 0 )
		srcr = 0;
	if( srcr > 255 )
		srcr = 255;
	if( srcg < 0 )
		srcg = 0;
	if( srcg > 255 )
		srcg = 255;
	if( srcb < 0 )
		srcb = 0;
	if( srcb > 255 )
		srcb = 255;

	D3DTLVERTEX* tlvptr;
	unsigned int orgalpha;
	unsigned int orgcol, orgr, orgg, orgb;
	int newr, newg, newb;

	float rater, rateg, rateb;

	int tlvno;
	for( tlvno = 0; tlvno < optpleng; tlvno++ ){
		tlvptr = opttlv + tlvno;
		_ASSERT( tlvptr );

		orgalpha = (tlvptr->specular & 0xFF000000) >> 24;

		switch( setflag ){
		case 0://そのままセット
			newr = srcr;
			newg = srcg;
			newb = srcb;
			tlvptr->specular = D3DCOLOR_ARGB( orgalpha, srcr, srcg, srcb );
			break;
		case 1://乗算
			rater = (float)srcr / 255.0f;
			rateg = (float)srcg / 255.0f;
			rateb = (float)srcb / 255.0f;
				
			orgcol = tlvptr->specular;
			orgr = (orgcol & 0x00FF0000) >> 16;
			orgg = (orgcol & 0x0000FF00) >> 8;
			orgb = (orgcol & 0x000000FF);

			newr = (unsigned int)((float)orgr * rater);
			newg = (unsigned int)((float)orgg * rateg);
			newb = (unsigned int)((float)orgb * rateb);

			tlvptr->specular = D3DCOLOR_ARGB( orgalpha, newr, newg, newb );

			break;
		case 2://足し算
			orgcol = tlvptr->specular;
			orgr = (orgcol & 0x00FF0000) >> 16;
			orgg = (orgcol & 0x0000FF00) >> 8;
			orgb = (orgcol & 0x000000FF);

			newr = orgr + srcr;
			if( newr > 255 )
				newr = 255;

			newg = orgg + srcg;
			if( newg > 255 )
				newg = 255;

			newb = orgb + srcb;
			if( newb > 255 )
				newb = 255;

			tlvptr->specular = D3DCOLOR_ARGB( orgalpha, newr, newg, newb );

			break;
		case 3://引き算
			orgcol = tlvptr->specular;
			orgr = (orgcol & 0x00FF0000) >> 16;
			orgg = (orgcol & 0x0000FF00) >> 8;
			orgb = (orgcol & 0x000000FF);

			newr = orgr - srcr;
			if( newr < 0 )
				newr = 0;

			newg = orgg - srcg;
			if( newg < 0 )
				newg = 0;
			
			newb = orgb - srcb;
			if( newb < 0 )
				newb = 0;

			tlvptr->specular = D3DCOLOR_ARGB( orgalpha, newr, newg, newb );
				
			break;
		default:
			break;
		}
	}
	int ret;
	if( tlmode == TLMODE_D3D ){
		_ASSERT( d3dptr );

		CVec3f newrgb;
		newrgb.x = newr / 255.0f;
		newrgb.y = newg / 255.0f;
		newrgb.z = newb / 255.0f;

		ret = d3dptr->SetD3DVERTEX_RGB( newrgb, MAT_SPECULAR );
		if( ret ){
			DbgOut( "polymesh2 : SetOptSpecular : d3ddisp SetD3DVERTEX_RGB error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CPolyMesh2::SetOptAmbient( int srcr, int srcg, int srcb, int setflag )
{

	if( !optambient ){
		DbgOut( "polymesh2 : SetOptAmbient : optambient NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fr, fg, fb;
	
	fr = (float)srcr / 255.0f;
	if( fr < 0.0f )
		fr = 0.0f;
	if( fr > 1.0f )
		fr = 1.0f;

	fg = (float)srcg / 255.0f;
	if( fg < 0.0f )
		fg = 0.0f;
	if( fg > 1.0f )
		fg = 1.0f;

	fb = (float)srcb / 255.0f;
	if( fb < 0.0f )
		fb = 0.0f;
	if( fb > 1.0f )
		fb = 1.0f;


	ARGBF* ambptr;

	float newr, newg, newb;

	int pno;
	for( pno = 0; pno < optpleng; pno++ ){
		ambptr = optambient + pno;
		_ASSERT( ambptr );

		switch( setflag ){
		case 0://そのままセット
			ambptr->a = 0.0f;
			ambptr->r = fr;
			ambptr->g = fg;
			ambptr->b = fb;

			break;
		case 1://乗算
			newr = ambptr->r * fr;
			newg = ambptr->g * fg;
			newb = ambptr->b * fb;

			ambptr->a = 0.0f;
			ambptr->r = newr;
			ambptr->g = newg;
			ambptr->b = newb;

			break;
		case 2://足し算
			newr = ambptr->r + fr;
			if( newr > 1.0f )
				newr = 1.0f;

			newg = ambptr->g + fg;
			if( newg > 1.0f )
				newg = 1.0f;

			newb = ambptr->b + fb;
			if( newb > 1.0f )
				newb = 1.0f;

			ambptr->a = 0.0f;
			ambptr->r = newr;
			ambptr->g = newg;
			ambptr->b = newb;

			break;
		case 3://引き算
			newr = ambptr->r - fr;
			if( newr < 0.0f )
				newr = 0.0f;

			newg = ambptr->g - fg;
			if( newg < 0.0f )
				newg = 0.0f;

			newb = ambptr->b - fb;
			if( newb < 0.0f )
				newb = 0.0f;

			ambptr->a = 0.0f;
			ambptr->r = newr;
			ambptr->g = newg;
			ambptr->b = newb;

			break;
		default:
			break;
		}

	}

	return 0;
}
***/


int CPolyMesh2::CreateInfElemIfNot( int leng )
{
	if( m_IE )
		return 0;

	m_IE = new CInfElem[ leng ];
	if( !m_IE ){
		DbgOut( "polymesh2 : CreateInfElemIfNot : m_IE alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CPolyMesh2::GetOrgVertNo( int vertno, int* orgnoarray, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;

	if( (vertno < 0) || (vertno >= optpleng) ){
		DbgOut( "polymesh2 : GetOrgVertNo : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int orgno;

	int getcnt = 0;
	int oldpno;
	oldpno = *( optpno2oldpno + vertno );
	while( oldpno >= 0 ){
		if( getcnt >= arrayleng ){
			DbgOut( "polymesh2 : GetOrgVertNo : arrayleng too short warning !!!\n" );
			_ASSERT( 0 );
			return 0;//!!!!!!!!!!!!!
		}


		//
		//*( orgnoarray + *getnumptr ) = oldpno;



	//!!!!!!! 2004/09/15
		_ASSERT( oldpno < (meshinfo->n * 3) );
		orgno = *( orgnobuf + oldpno );

		//orgnoが、まだ、セットされていない番号の場合のみ、セットする。
		int isfind = 0;
		int i;
		for( i = 0; i <= (getcnt - 1); i++ ){
			if( *( orgnoarray + i ) == orgno ){
				isfind = 1;
				break;
			}
		}
		if( isfind == 0 ){
			*( orgnoarray + *getnumptr ) = orgno;

			getcnt++;			
			(*getnumptr)++;

		}
	//

		oldpno = *( samepointbuf + oldpno );

	}


	return 0;
}

int CPolyMesh2::CalcAnchorCenter()
{
	int pnum = meshinfo->n * 3;

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

/***
int CPolyMesh2::CheckVertInMikoAnchor( D3DXVECTOR3 srcv, int* insideptr )
{
	*insideptr = 0;

	if( !opttlv ){
		_ASSERT( 0 );
		return 1;
	}


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
		D3DTLVERTEX* p0;
		D3DTLVERTEX* p1;
		D3DTLVERTEX* p2;

		p0 = opttlv + *( optindexbuf + faceno * 3 );
		p1 = opttlv + *( optindexbuf + faceno * 3 + 1 );
		p2 = opttlv + *( optindexbuf + faceno * 3 + 2 );

		vec1.x = p1->sx - p0->sx;
		vec1.y = p1->sy - p0->sy;
		vec1.z = p1->sz - p0->sz;

		vec2.x = p2->sx - p0->sx;
		vec2.y = p2->sy - p0->sy;
		vec2.z = p2->sz - p0->sz;

		D3DXVec3Cross( &facenormal, &vec1, &vec2 );

		facecenter.x = (p0->sx + p1->sx + p2->sx) / 3.0f;
		facecenter.y = (p0->sy + p1->sy + p2->sy) / 3.0f;
		facecenter.z = (p0->sz + p1->sz + p2->sz) / 3.0f;

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
***/


int CPolyMesh2::CheckVertInShape( D3DXVECTOR3 srcv, int clockwise, int* insideptr )
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

		if( clockwise == 2 ){
			p0 = pointbuf + faceno * 3;
			p1 = pointbuf + faceno * 3 + 1;
			p2 = pointbuf + faceno * 3 + 2;
		}else{
			p0 = pointbuf + faceno * 3;
			p1 = pointbuf + faceno * 3 + 2;
			p2 = pointbuf + faceno * 3 + 1;
		}

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

int CPolyMesh2::SetUVTile( int texrule, int unum, int vnum, int texno )
{

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
			DbgOut( "polymesh2 : SetUVTile : SetUV_X error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case TEXRULE_Y:
		ret = SetUV_Y( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh2 : SetUVTile : SetUV_Y error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case TEXRULE_Z:
		ret = SetUV_Z( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh2 : SetUVTile : SetUV_Z error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case TEXRULE_CYLINDER:
		ret = SetUV_Cylinder( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh2 : SetUVTile : SetUV_CYLINDER error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case TEXRULE_SPHERE:
		ret = SetUV_Sphere( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh2 : SetUVTile : SetUV_SPHERE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		DbgOut( "polymesh2 : SetUVTile : unknown texrule : Set TEXRULE_Z warning !!!\n" );
		ret = SetUV_Z( startu, endu, startv, endv );
		if( ret ){
			DbgOut( "polymesh2 : SetUVTile : SetUV_Z error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	}


	return 0;
}


int CPolyMesh2::SetUV( int vertno, float fu, float fv, int setflag, int clampflag )
{
	
	if( (vertno < 0) || (vertno >= optpleng) ){
		DbgOut( "polymesh2 : SetUV : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !opttlv ){
		DbgOut( "polymesh2 : SetUV : opttlv not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DTLVERTEX* curtlv;
	curtlv = opttlv + vertno;

	if( setflag == 0 ){
		curtlv->tu = fu;
		curtlv->tv = fv;
	}else{
		curtlv->tu += fu;
		curtlv->tv += fv;
	}

	if( clampflag == 1 ){
		curtlv->tu = max( 0.0f, curtlv->tu );
		curtlv->tu = min( 1.0f, curtlv->tu );

		curtlv->tv = max( 0.0f, curtlv->tv );
		curtlv->tv = min( 1.0f, curtlv->tv );
	}


//////////

	
	int oldvnum;
	oldvnum = meshinfo->n * 3;
	int oldvno;
	int optvno;
	COORDINATE* curuv;

	ZeroMemory( dirtypoint, sizeof( char ) * oldvnum );
	
	for( oldvno = 0; oldvno < oldvnum; oldvno++ ){
		optvno = *( oldpno2optpno + oldvno );
		if( (optvno == vertno) && (*(dirtypoint + oldvno) == 0) ){
			curuv = uvbuf + oldvno;

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

			*( dirtypoint + oldvno ) = 1;
		}
	}

	return 0;
}

int CPolyMesh2::SetUV_X( float startu, float endu, float startv, float endv )
{

	int pno;
	D3DTLVERTEX* curtlv = opttlv;
	float minz = 1e6;
	float maxz = -1e6;
	float miny = 1e6;
	float maxy = -1e6;
	for( pno = 0; pno < optpleng; pno++ )
	{
		if( curtlv->sz > maxz )
			maxz = curtlv->sz;

		if( curtlv->sz < minz )
			minz = curtlv->sz;

		if( curtlv->sy > maxy )
			maxy = curtlv->sy;

		if( curtlv->sy < miny )
			miny = curtlv->sy;

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

	curtlv = opttlv;
	for( pno = 0; pno < optpleng; pno++ ){
		curtlv->tu = startu + (curtlv->sz - minz) * zstep;
		curtlv->tv = endv - (curtlv->sy - miny) * ystep;
		curtlv++;
	}

////////
	int oldvnum;
	oldvnum = meshinfo->n * 3;
	int oldvno;
	int optvno;
	COORDINATE* curuv;

	
	//ZeroMemory( dirtypoint, sizeof( char ) * oldvnum );

	for( oldvno = 0; oldvno < oldvnum; oldvno++ ){
		optvno = *( oldpno2optpno + oldvno );
		//if( (optvno == vertno) && (*(dirtypoint + oldvno) == 0) ){
			curuv = uvbuf + oldvno;
			curtlv = opttlv + optvno;

			curuv->u = curtlv->tu;
			curuv->v = curtlv->tv;

		//	*( dirtypoint + oldvno ) = 1;
		//}
	}

	return 0;
}
int CPolyMesh2::SetUV_Y( float startu, float endu, float startv, float endv )
{
	// U : X, V : Z

	int pno;
	D3DTLVERTEX* curtlv = opttlv;
	float minz = 1e6;
	float maxz = -1e6;
	float minx = 1e6;
	float maxx = -1e6;
	for( pno = 0; pno < optpleng; pno++ )
	{
		if( curtlv->sz > maxz )
			maxz = curtlv->sz;

		if( curtlv->sz < minz )
			minz = curtlv->sz;

		if( curtlv->sx > maxx )
			maxx = curtlv->sx;

		if( curtlv->sx < minx )
			minx = curtlv->sx;

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

	curtlv = opttlv;
	for( pno = 0; pno < optpleng; pno++ ){
		curtlv->tu = startu + (curtlv->sx - minx) * xstep;
		curtlv->tv = endv - (curtlv->sz - minz) * zstep;
		curtlv++;
	}
////////
	int oldvnum;
	oldvnum = meshinfo->n * 3;
	int oldvno;
	int optvno;
	COORDINATE* curuv;

	//ZeroMemory( dirtypoint, sizeof( char ) * oldvnum );
	
	for( oldvno = 0; oldvno < oldvnum; oldvno++ ){
		optvno = *( oldpno2optpno + oldvno );
		//if( (optvno == vertno) && (*(dirtypoint + oldvno) == 0) ){
			curuv = uvbuf + oldvno;
			curtlv = opttlv + optvno;

			curuv->u = curtlv->tu;
			curuv->v = curtlv->tv;

		//	*( dirtypoint + oldvno ) = 1;
		//}
	}

	return 0;
}
int CPolyMesh2::SetUV_Z( float startu, float endu, float startv, float endv )
{
	// U : X, V : Y
	int pno;
	D3DTLVERTEX* curtlv = opttlv;
	float minx = 1e6;
	float maxx = -1e6;
	float miny = 1e6;
	float maxy = -1e6;
	for( pno = 0; pno < optpleng; pno++ )
	{
		if( curtlv->sx > maxx )
			maxx = curtlv->sx;

		if( curtlv->sx < minx )
			minx = curtlv->sx;

		if( curtlv->sy > maxy )
			maxy = curtlv->sy;

		if( curtlv->sy < miny )
			miny = curtlv->sy;

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

	curtlv = opttlv;
	for( pno = 0; pno < optpleng; pno++ ){
		curtlv->tu = startu + (curtlv->sx - minx) * xstep;
		curtlv->tv = endv - (curtlv->sy - miny) * ystep;
		curtlv++;
	}
////////
	int oldvnum;
	oldvnum = meshinfo->n * 3;
	int oldvno;
	int optvno;
	COORDINATE* curuv;

	//ZeroMemory( dirtypoint, sizeof( char ) * oldvnum );
	
	for( oldvno = 0; oldvno < oldvnum; oldvno++ ){
		optvno = *( oldpno2optpno + oldvno );
		//if( (optvno == vertno) && (*(dirtypoint + oldvno) == 0) ){
			curuv = uvbuf + oldvno;
			curtlv = opttlv + optvno;

			curuv->u = curtlv->tu;
			curuv->v = curtlv->tv;

		//	*( dirtypoint + oldvno ) = 1;
		//}
	}

	return 0;
}
int CPolyMesh2::SetUV_Cylinder( float startu, float endu, float startv, float endv )
{
	int pno;
	D3DXVECTOR3 center;
	center.x = 0.0f; center.y = 0.0f; center.z = 0.0f;
	D3DTLVERTEX* curtlv = opttlv;
	float miny = 1e6;
	float maxy = -1e6;
	for( pno = 0; pno < optpleng; pno++ )
	{
		center.x += curtlv->sx;
		center.y += curtlv->sy;
		center.z += curtlv->sz;

		if( curtlv->sy > maxy )
			maxy = curtlv->sy;

		if( curtlv->sy < miny )
			miny = curtlv->sy;

		curtlv++;
	}
	center.x /= (float)optpleng;
	center.y /= (float)optpleng;
	center.z /= (float)optpleng;
	float ystep;
	if( maxy != miny )
		ystep = (endv - startv) / ( maxy - miny );
	else
		ystep = 0.0f;

	D3DXVECTOR2 base;
	base.x = 0.0f; base.y = -1.0f;// y にはz座標！！！


	curtlv = opttlv;
	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;
	float dot, kaku, flccw;
	float xstep = (endu - startu) / ( 2.0f * PI );

	for( pno = 0; pno < optpleng; pno++ ){
		curtlv->tv = endv - (curtlv->sy - miny) * ystep;
			
		vec.x = curtlv->sx - center.x;
		vec.y = curtlv->sz - center.z;
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

		curtlv->tu = startu + kaku * xstep;

		curtlv++;
	}
////////
	int oldvnum;
	oldvnum = meshinfo->n * 3;
	int oldvno;
	int optvno;
	COORDINATE* curuv;

	//ZeroMemory( dirtypoint, sizeof( char ) * oldvnum );
	
	for( oldvno = 0; oldvno < oldvnum; oldvno++ ){
		optvno = *( oldpno2optpno + oldvno );
		//if( (optvno == vertno) && (*(dirtypoint + oldvno) == 0) ){
			curuv = uvbuf + oldvno;
			curtlv = opttlv + optvno;

			curuv->u = curtlv->tu;
			curuv->v = curtlv->tv;

		//	*( dirtypoint + oldvno ) = 1;
		//}
	}

	return 0;
}
int CPolyMesh2::SetUV_Sphere( float startu, float endu, float startv, float endv )
{
	int pno;
	D3DXVECTOR3 center;
	center.x = 0.0f; center.y = 0.0f; center.z = 0.0f;
	D3DTLVERTEX* curtlv = opttlv;
	for( pno = 0; pno < optpleng; pno++ )
	{
		center.x += curtlv->sx;
		center.y += curtlv->sy;
		center.z += curtlv->sz;

		curtlv++;
	}
	center.x /= (float)optpleng;
	center.y /= (float)optpleng;
	center.z /= (float)optpleng;

	D3DXVECTOR2 base_xz;
	base_xz.x = 0.0f; base_xz.y = -1.0f;// y にはz座標！！！
	D3DXVECTOR2 base_xy;
	base_xy.x = 0.0f; base_xy.y = 1.0f;


	curtlv = opttlv;
	D3DXVECTOR2 vec_xz;
	D3DXVECTOR2 nvec_xz;
	D3DXVECTOR2 vec_xy;
	D3DXVECTOR2 nvec_xy;
	float dot_xz, kaku_xz, flccw_xz;
	float dot_xy, kaku_xy;//, flccw_xy;
	float stepxz = (endu - startu) / ( 2.0f * PI );
	float stepxy = (endv - startv) / (float)PI;

	for( pno = 0; pno < optpleng; pno++ ){
			
		vec_xz.x = curtlv->sx - center.x;
		vec_xz.y = curtlv->sz - center.z;
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

		curtlv->tu = startu + kaku_xz * stepxz;
		///////////
		vec_xy.x = curtlv->sx - center.x;
		vec_xy.y = curtlv->sy - center.y;
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

		curtlv->tv = startv + kaku_xy * stepxy;

		curtlv++;
	}
////////
	int oldvnum;
	oldvnum = meshinfo->n * 3;
	int oldvno;
	int optvno;
	COORDINATE* curuv;

	//ZeroMemory( dirtypoint, sizeof( char ) * oldvnum );
	
	for( oldvno = 0; oldvno < oldvnum; oldvno++ ){
		optvno = *( oldpno2optpno + oldvno );
		//if( (optvno == vertno) && (*(dirtypoint + oldvno) == 0) ){
			curuv = uvbuf + oldvno;
			curtlv = opttlv + optvno;

			curuv->u = curtlv->tu;
			curuv->v = curtlv->tv;

		//	*( dirtypoint + oldvno ) = 1;
		//}
	}

	return 0;
}


int CPolyMesh2::MultMat( D3DXMATRIX multmat )
{

	int bufleng;
	bufleng = meshinfo->n * 3;

	/***
	if( pointbuf ){
		int vno;
		VEC3F* curv;
		for( vno = 0; vno < bufleng; vno++ ){
			curv = pointbuf + vno;

			D3DXVECTOR3 befv, aftv;
			befv.x = curv->x;
			befv.y = curv->y;
			befv.z = curv->z;

			D3DXVec3TransformCoord( &aftv, &befv, &multmat );
			curv->x = aftv.x;
			curv->y = aftv.y;
			curv->z = aftv.z;

		}
	}


	if( opttlv ){
		int vno;
		D3DTLVERTEX* curv;
		for( vno = 0; vno < optpleng; vno++ ){
			curv = opttlv + vno;

			D3DXVECTOR3 befv, aftv;
			befv.x = curv->sx;
			befv.y = curv->sy;
			befv.z = curv->sz;

			D3DXVec3TransformCoord( &aftv, &befv, &multmat );
			curv->sx = aftv.x;
			curv->sy = aftv.y;
			curv->sz = aftv.z;
		}
	}
	***/
	if( pointbuf ){
		int vno;
		VEC3F* curv;
		for( vno = 0; vno < bufleng; vno++ ){
			curv = pointbuf + vno;

			float x, y, z;
			x = curv->x;
			y = curv->y;
			z = curv->z;

			curv->x = multmat._11*x + multmat._21*y + multmat._31*z + multmat._41;
			curv->y = multmat._12*x + multmat._22*y + multmat._32*z + multmat._42;
			curv->z = multmat._13*x + multmat._23*y + multmat._33*z + multmat._43;

		}
	}


	if( opttlv ){
		int vno;
		D3DTLVERTEX* curv;
		for( vno = 0; vno < optpleng; vno++ ){
			curv = opttlv + vno;

			float x, y, z;
			x = curv->sx;
			y = curv->sy;
			z = curv->sz;

			curv->sx = multmat._11*x + multmat._21*y + multmat._31*z + multmat._41;
			curv->sy = multmat._12*x + multmat._22*y + multmat._32*z + multmat._42;
			curv->sz = multmat._13*x + multmat._23*y + multmat._33*z + multmat._43;
		}
	}


	return 0;
}

/***
int CPolyMesh2::AdjustSamePoint()
{
	if( !pointbuf || !opttlv ){
		DbgOut( "pm2 : AdjustSamePoint : pointbuf NULL return !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	int vertno;
	int oldpno;
	VEC3F* dstold;
	VEC3F* srcpos;
	int dbgcnt;

	for( vertno = 0; vertno < optpleng; vertno++ ){

		dbgcnt = 0;

		oldpno = *( optpno2oldpno + vertno );
		srcpos = pointbuf + oldpno;

		while( oldpno >= 0 ){
			dstold = pointbuf + oldpno;

			dstold->x = srcpos->x;
			dstold->y = srcpos->y;
			dstold->z = srcpos->z;

			oldpno = *( samepointbuf + oldpno );

			dbgcnt++;

			if( dbgcnt > 10000 ){
				DbgOut( "polymesh2 : AdjustSamePoint : oldpno loop error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}
***/

/***
int CPolyMesh2::ChangeColor( int seri, CPolyMesh2* srcpm2 )
{
	if( optpleng != srcpm2->optpleng ){
		DbgOut( "pm2 : ChangeColor : optpleng not equal return %d\r\n", seri );
		return 2;
	}

	if( !opttlv || !optambient || !optpowerbuf || !optemissivebuf ){
		DbgOut( "pm2 : ChangeColor : opt buffer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !srcpm2->opttlv || !srcpm2->optambient || !srcpm2->optpowerbuf || !srcpm2->optemissivebuf ){
		DbgOut( "pm2 : ChangeColor : src opt buffer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int optvno;
	D3DTLVERTEX* srctlv;
	D3DTLVERTEX* dsttlv;

	for( optvno = 0; optvno < optpleng; optvno++ ){
		dsttlv = opttlv + optvno;
		srctlv = srcpm2->opttlv + optvno;

		dsttlv->color = srctlv->color;
		dsttlv->specular = srctlv->specular;
	}

	memcpy( optambient, srcpm2->optambient, sizeof( ARGBF ) * optpleng );
	memcpy( optpowerbuf, srcpm2->optpowerbuf, sizeof( float ) * optpleng );
	memcpy( optemissivebuf, srcpm2->optemissivebuf, sizeof( ARGBF ) * optpleng );

	return 0;
}
***/
int CPolyMesh2::ChangeColor( int seri, CPolyMesh2* srcpm2 )
{
	int vertnum, facenum;

	vertnum = meshinfo->m;
	facenum = meshinfo->n;

	if( vertnum != srcpm2->meshinfo->m ){
		DbgOut( "pm2 : ChangeColor : vertnum not equal return %d\r\n", seri );
		return 2;
	}
	if( facenum != srcpm2->meshinfo->n ){
		DbgOut( "pm2 : ChangeColor : facenum not equal return %d\r\n", seri );
		return 2;
	}

	if( !diffusebuf || !ambientbuf || !specularbuf || !powerbuf || !emissivebuf || !diffbuf ){
		DbgOut( "pm2 : ChangeColor : color buf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !srcpm2->diffusebuf || !srcpm2->ambientbuf || !srcpm2->specularbuf || !srcpm2->powerbuf || !srcpm2->emissivebuf || !srcpm2->diffbuf ){
		DbgOut( "pm2 : ChangeColor : src color buf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( !opttlv || !optambient || !optpowerbuf || !optemissivebuf ){
		DbgOut( "pm2 : ChangeColor : opt buffer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !srcpm2->opttlv || !srcpm2->optambient || !srcpm2->optpowerbuf || !srcpm2->optemissivebuf ){
		DbgOut( "pm2 : ChangeColor : src opt buffer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//////////

	memcpy( diffusebuf, srcpm2->diffusebuf, sizeof( ARGBF ) * facenum );
	memcpy( ambientbuf, srcpm2->ambientbuf, sizeof( ARGBF ) * facenum );
	memcpy( specularbuf, srcpm2->specularbuf, sizeof( float ) * facenum );
	memcpy( powerbuf, srcpm2->powerbuf, sizeof( float ) * facenum );
	memcpy( emissivebuf, srcpm2->emissivebuf, sizeof( float ) * facenum );
	memcpy( diffbuf, srcpm2->diffbuf, sizeof( float ) * facenum );


///////////
	int optvno;
	D3DTLVERTEX* dsttlv;
	for( optvno = 0; optvno < optpleng; optvno++ ){
		dsttlv = opttlv + optvno;
		
		int oldno;
		oldno = *( optpno2oldpno + optvno );
		int curfaceno;
		curfaceno = oldno / 3;

		if( oldno >= 0 ){
			
			D3DCOLOR curdiff;
			curdiff = D3DCOLOR_ARGB( 
				(unsigned char)((diffusebuf + curfaceno)->a * 255.0f),
				(unsigned char)((diffusebuf + curfaceno)->r * *(diffbuf + curfaceno) * 255.0f), 				
				(unsigned char)((diffusebuf + curfaceno)->g * *(diffbuf + curfaceno) * 255.0f), 
				(unsigned char)((diffusebuf + curfaceno)->b * *(diffbuf + curfaceno) * 255.0f) 
			);
			dsttlv->color = curdiff;

			D3DCOLOR curspec;
			curspec = D3DCOLOR_ARGB( 
				255,
				(unsigned char)(*(specularbuf + curfaceno) * 255.0f), 				
				(unsigned char)(*(specularbuf + curfaceno) * 255.0f), 
				(unsigned char)(*(specularbuf + curfaceno) * 255.0f) 
			);
			dsttlv->specular = curspec;

			*(optambient + optvno) = *(ambientbuf + curfaceno);
				
			*(optpowerbuf + optvno) = *(powerbuf + curfaceno);

			(optemissivebuf + optvno)->r = (diffusebuf + curfaceno)->r * *(emissivebuf + curfaceno);
			(optemissivebuf + optvno)->g = (diffusebuf + curfaceno)->g * *(emissivebuf + curfaceno);
			(optemissivebuf + optvno)->b = (diffusebuf + curfaceno)->b * *(emissivebuf + curfaceno);

		}
	}

	return 0;
}

int CPolyMesh2::CalcOrgNormal()
{
	if( (!pointbuf) || (!orgnormal) ){
		DbgOut( "pm2 : CalcOrgNormal : buffer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_loadorgnormalflag != 0 )
		return 0;


	VEC3F* v1;
	VEC3F* v2;
	VEC3F* v3;

	VEC3F* n1;
	VEC3F* n2;
	VEC3F* n3;

	VEC3F curn;

	int fno;
	for( fno = 0; fno < meshinfo->n; fno++ ){
		v1 = pointbuf + fno * 3;
		v2 = pointbuf + fno * 3 + 1;
		v3 = pointbuf + fno * 3 + 2;

		n1 = orgnormal + fno * 3;
		n2 = orgnormal + fno * 3 + 1;
		n3 = orgnormal + fno * 3 + 2;

		CalcNormal( &curn, v1, v2, v3 );

		n1->x = curn.x;
		n1->y = curn.y;
		n1->z = curn.z;

		n2->x = curn.x;
		n2->y = curn.y;
		n2->z = curn.z;
		
		n3->x = curn.x;
		n3->y = curn.y;
		n3->z = curn.z;
	}

	return 0;
}

int CPolyMesh2::CalcNormal( VEC3F* newn, VEC3F* curp, VEC3F* aftp1, VEC3F* aftp2 )
{
	VEC3F vec1, vec2, crossvec;

	vec1.x = aftp1->x - curp->x;
	vec1.y = aftp1->y - curp->y;
	vec1.z = aftp1->z - curp->z;

	vec2.x = aftp2->x - curp->x;
	vec2.y = aftp2->y - curp->y;
	vec2.z = aftp2->z - curp->z;

	//D3DXVec3Cross( &crossvec, &vec1, &vec2 );
	//D3DXVec3Normalize( newn, &crossvec );

	Vec3Cross( &crossvec, &vec1, &vec2 );
	Vec3Normalize( newn, &crossvec );

	return 0;
}

int CPolyMesh2::Vec3Cross( VEC3F* pOut, VEC3F* pV1, VEC3F* pV2 )
{
	//D3DXVECTOR3 v;
	
	float x1, y1, z1, x2, y2, z2;
	x1 = pV1->x; y1 = pV1->y; z1 = pV1->z;
	x2 = pV2->x; y2 = pV2->y; z2 = pV2->z;

	pOut->x = y1 * z2 - z1 * y2;
	pOut->y = z1 * x2 - x1 * z2;
	pOut->z = x1 * y2 - y1 * x2;

	//v.x = pV1->y * pV2->z - pV1->z * pV2->y;
	//v.y = pV1->z * pV2->x - pV1->x * pV2->z;
	//v.z = pV1->x * pV2->y - pV1->y * pV2->x;
	 
	//*pOut = v;

	return 0;
}

int CPolyMesh2::Vec3Normalize( VEC3F* retvec, VEC3F* srcvec )
{
	float mag;
	float srcx, srcy, srcz;
	srcx = srcvec->x; srcy = srcvec->y; srcz = srcvec->z;

	mag = srcx * srcx + srcy * srcy + srcz * srcz;
	float leng;
	leng = (float)sqrtf( mag );

	if( leng > 0.0f ){
		float divleng;
		divleng = 1.0f / leng;
		retvec->x = srcx * divleng;
		retvec->y = srcy * divleng;
		retvec->z = srcz * divleng;
	}else{
		retvec->x = 0.0f;
		retvec->y = 0.0f;
		retvec->z = 0.0f;
	}

	return 0;
}

int CPolyMesh2::SetSmoothBuf( float facet )
{

	if( !pointbuf || !samepointbuf || !smoothbuf ){
		_ASSERT( 0 );
		return 1;
	}

	float cosfacet;
	int forcesmooth;
	cosfacet = (float)cos( facet * (float)DEG2PAI );
	if( facet == 180.0f ){
		forcesmooth = 1;
	}else{
		forcesmooth = 0;
	}

//!!!!!!!!!!!!!!!!!
	//forcesmooth = 1;


	int pnum = meshinfo->n * 3;
	int pno;

	//-1で初期化。
	//一個前の同じ座標の番号を保持する。
	//同じ座標を持つ、先頭のsmoothbufは、-2

	//init
	for( pno = 0; pno < pnum; pno++ ){
		*(smoothbuf + pno) = -1;
	}

	//search and set
	int chkno, befno;
	VEC3F curv, chkv;
	for( pno = 0; pno < pnum; pno++ ){
		curv = *(pointbuf + pno);
		befno = pno;

		if( *(smoothbuf + pno) == -1 ){
			for( chkno = 0; chkno < pnum; chkno++ ){
				chkv = *(pointbuf + chkno);

				int issamepos;
				issamepos = (curv.x == chkv.x) && (curv.y == chkv.y) && (curv.z == chkv.z);

				int issmooth;
				VEC3F* curnormal;
				VEC3F* chknormal;
				curnormal = orgnormal + befno;
				chknormal = orgnormal + chkno;
				float dot;
				dot = curnormal->x * chknormal->x + curnormal->y * chknormal->y + curnormal->z * chknormal->z;
				if( (forcesmooth == 1) || (dot > cosfacet) ){
					issmooth = 1;
				}else{
					issmooth = 0;
				}

				if( (pno != chkno) && (*(smoothbuf + chkno) == -1) && issamepos && issmooth ){
					
					*(smoothbuf + chkno) = befno;
					if( *(smoothbuf + befno) == -1 )
						*(smoothbuf + befno) = -2;

					befno = chkno;
				}
			}
		}
	}

	return 0;
}


int CPolyMesh2::SetOptSamePointBuf()
{
	if( !opttlv || !optsamepointbuf ){
		_ASSERT( 0 );
		return 1;
	}

	int pnum = optpleng;
	int pno;

	//-1で初期化。
	//一個前の同じ座標の番号を保持する。
	//同じ座標を持つ、先頭のsamepointbufは、-2

	//init
	for( pno = 0; pno < pnum; pno++ ){
		*(optsamepointbuf + pno) = -1;
	}

	//search and set
	int chkno, befno;
	D3DTLVERTEX curv, chkv;
	for( pno = 0; pno < pnum; pno++ ){
		curv = *(opttlv + pno);
		befno = pno;

		if( *(optsamepointbuf + pno) == -1 ){
			for( chkno = 0; chkno < pnum; chkno++ ){
				chkv = *(opttlv + chkno);

				if( (pno != chkno) && (*(optsamepointbuf + chkno) == -1) && 
					(curv.sx == chkv.sx) && (curv.sy == chkv.sy) && (curv.sz == chkv.sz)
				){
					
					*(optsamepointbuf + chkno) = befno;
					if( *(optsamepointbuf + befno) == -1 )
						*(optsamepointbuf + befno) = -2;

					befno = chkno;
				}
			}
		}
	}

	return 0;
}

int CPolyMesh2::CalcInitialInfElem( int srcmatno, int srcchildno )
{
	if( !m_IE ){
		DbgOut( "pm2 : CalcInitialInfElem : IE NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	int pno;
	CInfElem* curie;
	for( pno = 0; pno < optpleng; pno++ ){
		curie = m_IE + pno;

		//ret = curie->SetInfElemInitial();
		ret = curie->SetInfElemDefaultNoSkin( srcmatno, srcchildno );
		if( ret ){
			DbgOut( "pm2 : CalcInitialInfElem : ie SetInfElemDefaultNoSkin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CPolyMesh2::CreateAndSaveTempMaterial()
{
	DestroyTempMaterial();

	tempdiffuse = (DWORD*)malloc( sizeof( DWORD ) * optpleng );
	if( !tempdiffuse ){
		DbgOut( "pm2 : CreateAndSaveTempMaterial : tempdiffuse alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	tempspecular = (DWORD*)malloc( sizeof( DWORD ) * optpleng );
	if( !tempspecular ){
		DbgOut( "pm2 : CreateAndSaveTempMaterial : tempspecular alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	tempambient = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
	if( !tempdiffuse ){
		DbgOut( "pm2 : CreateAndSaveTempMaterial : tempambient alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	tempemissive = (ARGBF*)malloc( sizeof( ARGBF ) * optpleng );
	if( !tempemissive ){
		DbgOut( "pm2 : CreateAndSaveTempMaterial : tempemissive alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	temppower = (float*)malloc( sizeof( float ) * optpleng );
	if( !temppower ){
		DbgOut( "pm2 : CreateAndSaveTempMaterial : temppower alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int optpno;
	D3DTLVERTEX* curtlv;
	for( optpno = 0; optpno < optpleng; optpno++ ){
		curtlv = opttlv + optpno;

		*( tempdiffuse + optpno ) = curtlv->color;
		*( tempspecular + optpno ) = curtlv->specular;

	}

	MoveMemory( tempambient, optambient, sizeof( ARGBF ) * optpleng );
	MoveMemory( tempemissive, optemissivebuf, sizeof( ARGBF ) * optpleng );
	MoveMemory( temppower, optpowerbuf, sizeof( float ) * optpleng );

	return 0;
}
int CPolyMesh2::RestoreAndDestroyTempMaterial()
{
	if( (!tempdiffuse) || (!tempspecular) || (!tempambient) || (!tempemissive) || (!temppower) ){
		return 0;//!!!!!!!
	}

	int optpno;
	D3DTLVERTEX* curtlv;
	for( optpno = 0; optpno < optpleng; optpno++ ){
		curtlv = opttlv + optpno;

		curtlv->color = *( tempdiffuse + optpno );
		curtlv->specular = *( tempspecular + optpno );

	}
	MoveMemory( optambient, tempambient, sizeof( ARGBF ) * optpleng );
	MoveMemory( optemissivebuf, tempemissive, sizeof( ARGBF ) * optpleng );
	MoveMemory( optpowerbuf, temppower, sizeof( float ) * optpleng );

	DestroyTempMaterial();

	return 0;
}

int CPolyMesh2::DestroyTempMaterial()
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

int CPolyMesh2::SetMaterialFromInfElem( int srcchildno, int srcvert )
{

	int ret;
	if( srcvert < 0 ){
		int vert;
		for( vert = 0; vert < optpleng; vert++ ){
			ret = SetMaterialFromInfElem( srcchildno, vert );
			if( ret ){
				DbgOut( "pm2 : SetMaterialFromInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		return 0;
	}

	if( (srcvert < 0) || (srcvert >= optpleng) ){
		DbgOut( "pm2 : SetMaterialFromInfElem : srcvert error !!!\n" );
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

	_ASSERT( opttlv );

	D3DTLVERTEX* curtlv;
	curtlv = opttlv + srcvert;

	int alpha;
	alpha = (curtlv->color & 0xFF000000) >> 24;

	if( srcchildno >= 0 ){
		if( findIE ){
			E3DCOLOR3UC calccol;
			MakeColor( 0.0f, 1.0f, findIE->dispinf, infcol[findIE->kind], &calccol );

			curtlv->color = D3DCOLOR_RGBA( calccol.r, calccol.g, calccol.b, alpha );
			curtlv->specular = D3DCOLOR_RGBA( 0, 0, 0, alpha );
			
			float ambr, ambg, ambb;
			ambr = (float)calccol.r / 255.0f * 0.25f;
			ambr = min( 1.0f, ambr );
			ambr = max( 0.0f, ambr );
			ambg = (float)calccol.g / 255.0f * 0.25f;
			ambg = min( 1.0f, ambg );
			ambg = max( 0.0f, ambg );
			ambb = (float)calccol.b / 255.0f * 0.25f;
			ambb = min( 1.0f, ambb );
			ambb = max( 0.0f, ambb );

			( optambient + srcvert )->r = ambr;
			( optambient + srcvert )->g = ambg;
			( optambient + srcvert )->b = ambb;

			( optemissivebuf + srcvert )->r = 0.0f;
			( optemissivebuf + srcvert )->g = 0.0f;
			( optemissivebuf + srcvert )->b = 0.0f;

			*( optpowerbuf + srcvert ) = 0.0f;

		}else{
			curtlv->color = D3DCOLOR_RGBA( 255, 255, 255, alpha );
			curtlv->specular = D3DCOLOR_RGBA( 0, 0, 0, alpha );

			( optambient + srcvert )->r = 0.25f;
			( optambient + srcvert )->g = 0.25f;
			( optambient + srcvert )->b = 0.25f;

			( optemissivebuf + srcvert )->r = 0.0f;
			( optemissivebuf + srcvert )->g = 0.0f;
			( optemissivebuf + srcvert )->b = 0.0f;

			*( optpowerbuf + srcvert ) = 0.0f;

		}
	}else{
		// 対称コピーの頂点のみに、色をつける。

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
/***
			curtlv->color = D3DCOLOR_RGBA( 0, 255, 255, alpha );
			curtlv->specular = D3DCOLOR_RGBA( 0, 0, 0, alpha );

			( optambient + srcvert )->r = 0.0f;
			( optambient + srcvert )->g = 0.25f;
			( optambient + srcvert )->b = 0.25f;
***/
			curtlv->color = D3DCOLOR_RGBA( infcol[CALCMODE_SYM].r, infcol[CALCMODE_SYM].g, infcol[CALCMODE_SYM].b, alpha );
			curtlv->specular = D3DCOLOR_RGBA( 0, 0, 0, alpha );
			
			float ambr, ambg, ambb;
			ambr = (float)infcol[CALCMODE_SYM].r / 255.0f * 0.25f;
			ambr = min( 1.0f, ambr );
			ambr = max( 0.0f, ambr );
			ambg = (float)infcol[CALCMODE_SYM].g / 255.0f * 0.25f;
			ambg = min( 1.0f, ambg );
			ambg = max( 0.0f, ambg );
			ambb = (float)infcol[CALCMODE_SYM].b / 255.0f * 0.25f;
			ambb = min( 1.0f, ambb );
			ambb = max( 0.0f, ambb );

			( optambient + srcvert )->r = ambr;
			( optambient + srcvert )->g = ambg;
			( optambient + srcvert )->b = ambb;

			( optemissivebuf + srcvert )->r = 0.0f;
			( optemissivebuf + srcvert )->g = 0.0f;
			( optemissivebuf + srcvert )->b = 0.0f;

			*( optpowerbuf + srcvert ) = 0.0f;

		}else{

			curtlv->color = D3DCOLOR_RGBA( 255, 255, 255, alpha );
			curtlv->specular = D3DCOLOR_RGBA( 0, 0, 0, alpha );

			( optambient + srcvert )->r = 0.25f;
			( optambient + srcvert )->g = 0.25f;
			( optambient + srcvert )->b = 0.25f;

			( optemissivebuf + srcvert )->r = 0.0f;
			( optemissivebuf + srcvert )->g = 0.0f;
			( optemissivebuf + srcvert )->b = 0.0f;

			*( optpowerbuf + srcvert ) = 0.0f;
		}

	}

	//ひとつのボーンの影響も受けていない頂点は、黒
	if( (curie->infnum == 1) && (curie->ie->bonematno <= 0) ){
		curtlv->color = D3DCOLOR_RGBA( 0, 0, 0, alpha );
		curtlv->specular = D3DCOLOR_RGBA( 0, 0, 0, alpha );

		( optambient + srcvert )->r = 0.0f;
		( optambient + srcvert )->g = 0.0f;
		( optambient + srcvert )->b = 0.0f;

		( optemissivebuf + srcvert )->r = 0.0f;
		( optemissivebuf + srcvert )->g = 0.0f;
		( optemissivebuf + srcvert )->b = 0.0f;

		*( optpowerbuf + srcvert ) = 0.0f;
	}



	return 0;
}


int CPolyMesh2::GetNearVert( D3DXVECTOR3 findpos, float symdist, int* findvert, float* finddist )
{
	*findvert = -1;
	*finddist = 1e8;

	int vno;
	D3DTLVERTEX* curp;
	D3DXVECTOR3 diffv;
	for( vno = 0; vno < optpleng; vno++ ){
		curp = opttlv + vno;

		diffv.x = curp->sx - findpos.x;
		diffv.y = curp->sy - findpos.y;
		diffv.z = curp->sz - findpos.z;

		float tempdist;
		tempdist = D3DXVec3Length( &diffv );
		if( (tempdist <= symdist) && (tempdist < *finddist) ){
			*findvert = vno;
			*finddist = tempdist;
		}
	}

	return 0;
}

__int64 CPolyMesh2::GetVCol( int vertno )
{
	__int64 findvcol = 0xFFFFFFFF;

	int vcno;
	for( vcno = 0; vcno < vcolnum; vcno++ ){
		VCOLDATA* curvc;
		curvc = *( vcoldata + vcno );

		if( curvc && (curvc->vertno == vertno) ){
			findvcol = curvc->vcol;
			break;
		}
	}

	return findvcol;
}
VCOLDATA* CPolyMesh2::GetFreeVColData()
{
	VCOLDATA* retvc = 0;

	int vcno;
	for( vcno = 0; vcno < vcolnum; vcno++ ){
		VCOLDATA* curvc;
		curvc = *( vcoldata + vcno );
		if( curvc && (curvc->vertno < 0) ){
			retvc = curvc;
			break;
		}
	}

	return retvc;
}


int CPolyMesh2::GetValidVColNum()
{

	if( !vcoldata )
		return 0;

	int retnum = 0;
	int vcno;
	for( vcno = 0; vcno < vcolnum; vcno++ ){
		VCOLDATA* curvc;
		curvc = *( vcoldata + vcno );

		if( curvc && (curvc->vertno >= 0) ){
			retnum++;
		}
	}

	return retnum;
}

/***
int CPolyMesh2::ConvColor2XMaterial( float srcalpha, char* srctexname, D3DXMATERIAL** ppmat, DWORD** ppattr, int* matnumptr )
{
	//int ret;

	if( !opttlv ){
		DbgOut( "pm2 : ConvColor2XMaterial : optdata not exist error !!!\n" );
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
		DbgOut( "pm2 : ConvColor2XMaterial : ppattr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( attrib, sizeof( DWORD ) * meshinfo->n );


	int topindex;
	D3DTLVERTEX* curtlv;
	ARGBF* curamb;
	float curpow;
	ARGBF* curemi;

	D3DMATERIAL9 curmat;
	int faceno;
	for( faceno = 0; faceno < meshinfo->n; faceno++ ){
		topindex = *(optindexbuf + faceno * 3);
		curtlv = opttlv + topindex;
		curamb = optambient + topindex;
		curpow = *(optpowerbuf + topindex);
		curemi = optemissivebuf + topindex;
/////////
		
		float diffr, diffg, diffb;
		diffr = (float)((curtlv->color & 0x00FF0000) >> 16) / 255.0f;
		diffg = (float)((curtlv->color & 0x0000FF00) >> 8) / 255.0f;
		diffb = (float)(curtlv->color & 0x000000FF) / 255.0f;

		curmat.Diffuse.r = diffr;
		curmat.Diffuse.g = diffg;
		curmat.Diffuse.b = diffb;
		curmat.Diffuse.a = srcalpha;

		curmat.Ambient.r = min( 1.0f, curamb->r );
		curmat.Ambient.g = min( 1.0f, curamb->g );
		curmat.Ambient.b = min( 1.0f, curamb->b );
		curmat.Ambient.a = srcalpha;
//if( (curmat.Ambient.r == 0.0f) && (curmat.Ambient.g == 0.0f) && (curmat.Ambient.b == 0.0f) ){
//	DbgOut( "pm2 : ConvColor2XMaterial : checkamb0 : amb0 %f %f %f!!!\r\n", curamb->r, curamb->g, curamb->b );
//}


		float specr, specg, specb;
		specr = (float)((curtlv->specular & 0x00FF0000) >> 16) / 255.0f;
		specg = (float)((curtlv->specular & 0x0000FF00) >> 8) / 255.0f;
		specb = (float)(curtlv->specular & 0x000000FF) / 255.0f;

		curmat.Specular.r = specr;
		curmat.Specular.g = specg;
		curmat.Specular.b = specb;
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
			if( curxmat ){
				issame = IsSameMaterial( &(curxmat->MatD3D), &curmat );
				if( issame ){
					findmatno = matno;
					break;
				}
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
				DbgOut( "pm2 : ConvColor2XMaterial : mathead alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			
			( mathead + matnum - 1 )->MatD3D = curmat;

//if( ((mathead + matnum - 1)->MatD3D.Ambient.r == 0.0f) && ((mathead + matnum - 1)->MatD3D.Ambient.b == 0.0f) && ((mathead + matnum - 1)->MatD3D.Ambient.b == 0.0f) ){
//	DbgOut( "pm2 : ConvColor2XMaterial : Checkamb0 : amb0 !!!\r\n" );
//}

			if( srctexname && *srctexname ){
				int tnameleng;
				tnameleng = (int)strlen( srctexname );
				char* tnameptr;
				tnameptr = new CHAR[ tnameleng + 1 ];
				if( !tnameptr ){
					DbgOut( "pm2 : ConvColor2XMaterial : tnameptr alloc error !!!\n" );
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
			DbgOut( "pm2 : ConvColor2XMaterial : ppmat alloc error !!!\n" );
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

int CPolyMesh2::ConvColor2XMaterial( CMQOMaterial* mqomat, D3DXMATERIAL** ppmat, DWORD** ppattr, int* matnumptr )
{
	//int ret;

	if( !opttlv ){
		DbgOut( "pm2 : ConvColor2XMaterial : optdata not exist error !!!\n" );
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
		DbgOut( "pm2 : ConvColor2XMaterial : ppattr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( attrib, sizeof( DWORD ) * meshinfo->n );


	int topindex;
	D3DTLVERTEX* curtlv;
	ARGBF* curamb;
	float curpow;
	ARGBF* curemi;

	int curmqomatno;
	CMQOMaterial* curmqomat;
	float curalpha;
	char* curtexname;

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

		topindex = *(optindexbuf + faceno * 3);
		curtlv = opttlv + topindex;
		curamb = optambient + topindex;
		curpow = *(optpowerbuf + topindex);
		curemi = optemissivebuf + topindex;
/////////
		
		float diffr, diffg, diffb;
		diffr = (float)((curtlv->color & 0x00FF0000) >> 16) / 255.0f;
		diffg = (float)((curtlv->color & 0x0000FF00) >> 8) / 255.0f;
		diffb = (float)(curtlv->color & 0x000000FF) / 255.0f;

		curmat.Diffuse.r = diffr;
		curmat.Diffuse.g = diffg;
		curmat.Diffuse.b = diffb;
		curmat.Diffuse.a = curalpha;

		curmat.Ambient.r = min( 1.0f, curamb->r );
		curmat.Ambient.g = min( 1.0f, curamb->g );
		curmat.Ambient.b = min( 1.0f, curamb->b );
		curmat.Ambient.a = curalpha;
//if( (curmat.Ambient.r == 0.0f) && (curmat.Ambient.g == 0.0f) && (curmat.Ambient.b == 0.0f) ){
//	DbgOut( "pm2 : ConvColor2XMaterial : checkamb0 : amb0 %f %f %f!!!\r\n", curamb->r, curamb->g, curamb->b );
//}


		float specr, specg, specb;
		specr = (float)((curtlv->specular & 0x00FF0000) >> 16) / 255.0f;
		specg = (float)((curtlv->specular & 0x0000FF00) >> 8) / 255.0f;
		specb = (float)(curtlv->specular & 0x000000FF) / 255.0f;

		curmat.Specular.r = specr;
		curmat.Specular.g = specg;
		curmat.Specular.b = specb;
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
			if( curxmat ){
				issame = IsSameXMaterial( curxmat, &curmat, curtexname );
				if( issame ){
					findmatno = matno;
					break;
				}
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
				DbgOut( "pm2 : ConvColor2XMaterial : mathead alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			
			( mathead + matnum - 1 )->MatD3D = curmat;

//if( ((mathead + matnum - 1)->MatD3D.Ambient.r == 0.0f) && ((mathead + matnum - 1)->MatD3D.Ambient.b == 0.0f) && ((mathead + matnum - 1)->MatD3D.Ambient.b == 0.0f) ){
//	DbgOut( "pm2 : ConvColor2XMaterial : Checkamb0 : amb0 !!!\r\n" );
//}

			if( *curtexname ){
				int tnameleng;
				tnameleng = (int)strlen( curtexname );
				char* tnameptr;
				tnameptr = new CHAR[ tnameleng + 1 ];
				if( !tnameptr ){
					DbgOut( "pm2 : ConvColor2XMaterial : tnameptr alloc error !!!\n" );
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
			DbgOut( "pm2 : ConvColor2XMaterial : ppmat alloc error !!!\n" );
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
int CPolyMesh2::IsSameXMaterial( D3DXMATERIAL* xmat1, D3DMATERIAL9* mat2, char* texname2 )
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

int CPolyMesh2::IsSameMaterial( D3DMATERIAL9* mat1, D3DMATERIAL9* mat2 )
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
int CPolyMesh2::IsSameD3DColorValue( D3DCOLORVALUE* col1, D3DCOLORVALUE* col2 )
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


int CPolyMesh2::CreateToon1Buffer( LPDIRECT3DDEVICE9 pdev, D3DXMATERIAL* pmat, DWORD* pattrib, int matnum, CMQOMaterial* mqomat )
{
	int ret;
	DestroyToon1Buffer();

	if( !pmat || !pattrib || !matnum ){
		DbgOut( "pm2 : CreateToon1Buffer : parameter error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_material = pmat;
	m_attrib = pattrib;
	m_materialnum = matnum;

	if( !optindexbuf || !opttlv ){
		DbgOut( "pm2 : CreateToon1Buffer : opt buffer not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


/////////
	m_optindexbuf2 = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !m_optindexbuf2 ){
		DbgOut( "pm2 : CreateToon1Buffer : optindexbuf2 alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_optindexbuf2, sizeof( int ) * meshinfo->n * 3 );

	m_optindexbuf2m = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !m_optindexbuf2m ){
		DbgOut( "pm2 : CreateToon1Buffer : optindexbuf2 alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_optindexbuf2m, sizeof( int ) * meshinfo->n * 3 );



	m_materialblock = (MATERIALBLOCK*)malloc( sizeof( MATERIALBLOCK ) * matnum );
	if( !m_materialblock ){
		DbgOut( "pm2 : CreateToon1Buffer : materialblock alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_materialblock, sizeof( MATERIALBLOCK ) * matnum );


	m_toon1 = new CToon1Params[ matnum ];
	if( !m_toon1 ){
		DbgOut( "pm2 : CreateToon1Buffer : toon1params alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_toonface2oldface = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !m_toonface2oldface ){
		DbgOut( "pm2 : CreateToon1Buffer : toonface2oldface alloc error !!!\n" );
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
				*(m_optindexbuf2 + setfaceno * 3) = *(optindexbuf + faceno * 3);
				*(m_optindexbuf2 + setfaceno * 3 + 1) = *(optindexbuf + faceno * 3 + 1);
				*(m_optindexbuf2 + setfaceno * 3 + 2) = *(optindexbuf + faceno * 3 + 2);

				*(m_optindexbuf2m + setfaceno * 3) = faceno * 3;
				*(m_optindexbuf2m + setfaceno * 3 + 1) = faceno * 3 + 1;
				*(m_optindexbuf2m + setfaceno * 3 + 2) = faceno * 3 + 2;

				*(m_toonface2oldface + setfaceno) = faceno;//!!!!!!!!

				if( isfirst == 1 ){
					(m_materialblock + matno)->startface = setfaceno;
					(m_materialblock + matno)->mqomat = curmqomat;//!!!!!!!!!!!!!
					isfirst = 0;
				}

				setfaceno++;
			}
		}
		(m_materialblock + matno)->endface = setfaceno;//次の最初の番号
	}

	if( setfaceno != meshinfo->n ){
		DbgOut( "pm2 : CreateToon1Buffer : setfaceno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/***
//// for dbg
	for( matno = 0; matno < matnum; matno++ ){
		MATERIALBLOCK* curmb;
		curmb = m_materialblock + matno;

		DbgOut( "check materialblock : no %d : materialno %d, startface %d, endface %d\r\n",
			matno, curmb->materialno, curmb->startface, curmb->endface );
	}
	DbgOut( "\r\n" );
***/

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

	//DbgOut( "pm2 : check matd3d : texno %d : diffuse %f, %f, %f, %f\r\n", texno, curdiffuse.a, curdiffuse.r, curdiffuse.g, curdiffuse.b );

		}
	}


	return 0;
}

int CPolyMesh2::CreateToon1Buffer2( LPDIRECT3DDEVICE9 pdev, CMQOMaterial* mqomat )
{
	int ret;
	int matnum = m_materialnum;

//free
	if( m_optindexbuf2 ){
		free( m_optindexbuf2 );
		m_optindexbuf2 = 0;
	}
	if( m_optindexbuf2m ){
		free( m_optindexbuf2m );
		m_optindexbuf2m = 0;
	}
	if( m_materialblock ){
		free( m_materialblock );
		m_materialblock = 0;
	}

	if( m_toonface2oldface ){
		free( m_toonface2oldface );
		m_toonface2oldface = 0;
	}

	if( !optindexbuf || !opttlv){
		DbgOut( "pm2 : CreateToon1Buffer2 : opt buffer not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !m_attrib || !m_toon1 ){
		DbgOut( "pm2 : CreateToon1Buffer2 : toon1 buf not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	m_optindexbuf2 = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !m_optindexbuf2 ){
		DbgOut( "pm2 : CreateToon1Buffer2 : optindexbuf2 alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_optindexbuf2, sizeof( int ) * meshinfo->n * 3 );

	m_optindexbuf2m = (int*)malloc( sizeof( int ) * meshinfo->n * 3 );
	if( !m_optindexbuf2m ){
		DbgOut( "pm2 : CreateToon1Buffer2 : optindexbuf2 alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_optindexbuf2m, sizeof( int ) * meshinfo->n * 3 );


	m_materialblock = (MATERIALBLOCK*)malloc( sizeof( MATERIALBLOCK ) * matnum );
	if( !m_materialblock ){
		DbgOut( "pm2 : CreateToon1Buffer2 : materialblock alloc error !!!\n" );
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
				*(m_optindexbuf2 + setfaceno * 3) = *(optindexbuf + faceno * 3);
				*(m_optindexbuf2 + setfaceno * 3 + 1) = *(optindexbuf + faceno * 3 + 1);
				*(m_optindexbuf2 + setfaceno * 3 + 2) = *(optindexbuf + faceno * 3 + 2);

				*(m_optindexbuf2m + setfaceno * 3) = faceno * 3;
				*(m_optindexbuf2m + setfaceno * 3 + 1) = faceno * 3 + 1;
				*(m_optindexbuf2m + setfaceno * 3 + 2) = faceno * 3 + 2;

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
		DbgOut( "pm2 : CreateToon1Buffer2 : setfaceno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/***
//// for dbg
	for( matno = 0; matno < matnum; matno++ ){
		MATERIALBLOCK* curmb;
		curmb = m_materialblock + matno;

		DbgOut( "check materialblock : no %d : materialno %d, startface %d, endface %d\r\n",
			matno, curmb->materialno, curmb->startface, curmb->endface );
	}
	DbgOut( "\r\n" );
***/

//// texture
	int texno;
	for( texno = 0; texno < m_materialnum; texno++ ){

		ret = (m_toon1 + texno)->CreateTexture( pdev );
		if( ret ){
			DbgOut( "pm2 : CreateToon1Buffer : toon1 CreateTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//DbgOut( "pm2 : check matd3d : texno %d : diffuse %f, %f, %f, %f\r\n", texno, curdiffuse.a, curdiffuse.r, curdiffuse.g, curdiffuse.b );

	}


	return 0;
}

int CPolyMesh2::GetInfluenceVertNo( int srcjointno, int* vertarray, int* vertnum )
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
	for( vno = 0; vno < optpleng; vno++ ){
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

int CPolyMesh2::SetSMFace( float srcfacet )
{
	float cosfacet;
	int forcesmooth;
	cosfacet = (float)cos( srcfacet * (float)DEG2PAI );
	if( srcfacet == 180.0f ){
		forcesmooth = 1;
	}else{
		forcesmooth = 0;
	}

	int vno;
	int oldpleng;
	oldpleng = meshinfo->n * 3;

	int samev, samef;
	int srcf;
	int tmpsm[ SHAREFACEMAX + 1 ];
	int setno;

	for( vno = 0; vno < oldpleng; vno++ ){
		srcf = vno / 3;
		setno = 0;

		//samepointbufを小さいほうへ検索
		samev = *( samepointbuf + vno );
		while( samev > 0 ){
			samef = samev / 3;
			if( (setno <= SHAREFACEMAX) && (samef != srcf) ){
				tmpsm[setno] = samef;
				setno++;
			}
			samev = *( samepointbuf + samev );
		}

		//samepointbufを大きいほうへ検索
		samev = SearchSamePointBuf( vno );
		while( samev > 0 ){
			samef = samev / 3;
			if( (setno <= SHAREFACEMAX) && (samef != srcf) ){
				tmpsm[setno] = samef;
				setno++;
			}
			samev = SearchSamePointBuf( samev );
		}
		
		if( setno > 0 ){
			SMFACE* newsmf;
			newsmf = (SMFACE*)malloc( sizeof( SMFACE ) * ( setno + 1 ) );
			if( !newsmf ){
				DbgOut( "pm2 : SetSMFace : newsmf alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( newsmf, sizeof( SMFACE ) * ( setno + 1 ) );

			int smno;
			for( smno = 0; smno < setno; smno++ ){
				( newsmf + smno )->faceno = tmpsm[ smno ];
				
				VEC3F* norm1 = orgnormal + vno;
				VEC3F* norm2 = orgnormal + tmpsm[ smno ] * 3;

				float dotnorm;
				dotnorm = norm1->x * norm2->x + norm1->y * norm2->y + norm1->z * norm2->z;
				if( (dotnorm > cosfacet) || forcesmooth ){
					( newsmf + smno )->smoothflag = 1;
				}else{
					( newsmf + smno )->smoothflag = 0;
				}
			}
			( newsmf + setno )->faceno = -1;
			( newsmf + setno )->smoothflag = -1;

			*( smface + vno ) = newsmf;//!!!!!!!!!
		}

	}


	return 0;
}

int CPolyMesh2::SearchSamePointBuf( int srcvno )
{
	if( !samepointbuf ){
		return -1;
	}

	int findv = -1;

	int vno;
	for( vno = 0; vno < meshinfo->n * 3; vno++ ){
		if( srcvno == *( samepointbuf + vno ) ){
			findv = vno;
			break;
		}
	}
	return findv;
}


int CPolyMesh2::CopyNearestInfElem( CPolyMesh2* srcpm2, SERICONV* sericonv, int serinum )
{
	int ret;
	
	//dstが自分、srcが相手。

	int dstvno, dstvnum;
	dstvnum = optpleng;

	int srcvnum;
	srcvnum = srcpm2->optpleng;

	for( dstvno = 0; dstvno < dstvnum; dstvno++ ){
		D3DTLVERTEX* dstv;
		dstv = opttlv + dstvno;
		D3DXVECTOR3 findpos;
		findpos.x = dstv->sx;
		findpos.y = dstv->sy;
		findpos.z = dstv->sz;

		int nearv = 0;
		float neardist = 1e8;
		ret = srcpm2->GetNearVert( findpos, 100.0f, &nearv, &neardist );
		if( ret ){
			DbgOut( "pm2 : CopyNearestInfElem : GetNearVert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		if( (nearv >= 0) && (nearv < srcvnum) ){
			CInfElem* srcie;
			srcie = srcpm2->m_IE + nearv;
			_ASSERT( srcie );

			CInfElem* dstie;
			dstie = m_IE + dstvno;
			_ASSERT( dstie );

			ret = dstie->CopyIE( srcie, sericonv, serinum );
			if( ret ){
				DbgOut( "pm2 : CopyNearestInfElem : ie CopyIE error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CPolyMesh2::CreateSortTexBuffer( CMQOMaterial* mathead )
{
	//int* m_attrib0;//meshinfo->n mqo読み込み時にsh->m_matheadのマテリアル番号を面ごとに保持する。
	//SORTTEXTURE* m_sorttex;// meshinfo->n 面ごとのテクスチャ名、テクスチャ名でソートする
	//int* m_optindexbuftex;// meshinfo->n * 3, sorttexの順番で並び替えたインデックス
	//int m_texblocknum;
	//TEXTUREBLOCK* m_texblock;// texblocknum個のレンダー単位

	_ASSERT( mathead );
	_ASSERT( m_attrib0 );

	DestroySortTexBuffer();

	int facenum = meshinfo->n;
	
	m_sorttex = (SORTTEXTURE*)malloc( sizeof( SORTTEXTURE ) * facenum );
	if( !m_sorttex ){
		DbgOut( "pm2 : CreateSortTexBuffer : m_sorttex alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_sorttex, sizeof( SORTTEXTURE ) * facenum );

	m_optindexbuftex = (int*)malloc( sizeof( int ) * facenum * 3 );
	if( !m_optindexbuftex ){
		DbgOut( "pm2 : CreateSortTexBuffer : m_optindexbuftex alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_optindexbuftexm = (int*)malloc( sizeof( int ) * facenum * 3 );
	if( !m_optindexbuftexm ){
		DbgOut( "pm2 : CreateSortTexBuffer : m_optindexbuftexm alloc error !!!\n" );
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
			DbgOut( "pm2 : CreateSortTexBuffer : curmat NULL error !!!\n" );
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

		*( m_optindexbuftex + faceno * 3 ) = *( optindexbuf + curfaceno * 3 );
		*( m_optindexbuftex + faceno * 3 + 1 ) = *( optindexbuf + curfaceno * 3 + 1 );
		*( m_optindexbuftex + faceno * 3 + 2 ) = *( optindexbuf + curfaceno * 3 + 2 );

		*( m_optindexbuftexm + faceno * 3 ) = curfaceno * 3;
		*( m_optindexbuftexm + faceno * 3 + 1 ) = curfaceno * 3 + 1;
		*( m_optindexbuftexm + faceno * 3 + 2 ) = curfaceno * 3 + 2;

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
		DbgOut( "pm2 : CreateSortTexBuffer : m_texblock alloc error !!!\n" );
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
		DbgOut( "pm2 : CreateSortTexBuffer : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/****
	//!!!!for debug
	DbgOut( "pm2 : CreateSortTexBuffer\r\n!!!!check textureblock  facenum %d!!!!\r\n", facenum );
	int tbno;
	for( tbno = 0; tbno < m_texblocknum; tbno++ ){
		TEXTUREBLOCK* curtb;
		curtb = m_texblock + tbno;

		DbgOut( "materialno %d, texname %s, startface %d, endface %d\r\n", curtb->mqomat->materialno, curtb->mqomat->tex, curtb->startface, curtb->endface );
	}
	****/


	m_dirtymat = (DIRTYMAT*)malloc( sizeof( DIRTYMAT ) * m_texblocknum );
	if( !m_dirtymat ){
		DbgOut( "pm2 : CreateSortTexBuf : dirtymat alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_dirtymat, sizeof( DIRTYMAT ) * m_texblocknum );

	int tbno;
	int* dirtyflag;
	DIRTYMAT* curdm;
	TEXTUREBLOCK* curtb;
	for( tbno = 0; tbno < m_texblocknum; tbno++ ){
		curdm = m_dirtymat + tbno;
		curtb = m_texblock + tbno;

		_ASSERT( curtb->mqomat );
		curdm->materialno = curtb->mqomat->materialno;


		dirtyflag = (int*)malloc( sizeof( int ) * optpleng );
		if( !dirtyflag ){
			DbgOut( "pm2 : CreateSortTexBuf : dirtyflag alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( dirtyflag, sizeof( int ) * optpleng );


		int fno;
		for( fno = 0; fno < meshinfo->n; fno++ ){
			int curattr;
			curattr = *( m_attrib0 + fno );
			if( curattr == curdm->materialno ){
				int p1, p2, p3;
				p1 = *( optindexbuf + fno * 3 );
				p2 = *( optindexbuf + fno * 3 + 1 );
				p3 = *( optindexbuf + fno * 3 + 2 );

				_ASSERT( p1 < optpleng );
				_ASSERT( p2 < optpleng );
				_ASSERT( p3 < optpleng );

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

	/***
	if( *sot1->texname == 0 ){
		if( *sot2->texname == 0 ){
			return ( sot1->faceno - sot2->faceno );
		}else{
			return -1;//sot1が前
		}
	}else{
		if( *sot2->texname == 0 ){
			return 1;//sot1が後
		}else{
			int leng1, leng2;
			leng1 = (int)strlen( sot1->texname );
			leng2 = (int)strlen( sot2->texname );

			int cmpleng = min( leng1, leng2 );
			int cmpno;
			char c1, c2;
			int diff = 0;
			for( cmpno = 0; cmpno < cmpleng; cmpno++ ){
				c1 = *( sot1->texname + cmpno );
				c2 = *( sot2->texname + cmpno );

				diff = c1 - c2;
				if( diff != 0 ){
					return diff;//
				}
			}

			_ASSERT( diff == 0 );
			return ( leng1 - leng2 );//diff == 0

		}
	}
	***/
}


CMQOMaterial* CPolyMesh2::GetMaterialFromNo( CMQOMaterial* mathead, int matno )
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

int CPolyMesh2::ChkAlphaNum( CMQOMaterial* mathead )
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
			DbgOut( "pm2 : ChkAlphaNum : curmat NULL error !!!\n" );
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

int* CPolyMesh2::GetDirtyMaterial( int matno )
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
int CPolyMesh2::SetInfCol( E3DCOLOR3UC* srcinfcol )
{
	if( !srcinfcol ){
		return 0;
	}
	MoveMemory( infcol, srcinfcol, sizeof( E3DCOLOR3UC ) * CALCMODE_MAX );
	return 0;
}

int CPolyMesh2::MakeColor( float minval, float maxval, float val, E3DCOLOR3UC target_col, E3DCOLOR3UC* dstcol )
{
    //min = 比重の最小値
    //max = 比重の最大値
    //val = 比重の値
	float rate = (1.0f - ((val - minval) / (maxval - minval))) * 0.8F;

	int tmpr, tmpg, tmpb;
	tmpr = (int)( (float)target_col.r + ((255.0f - (float)target_col.r) * rate ) );
	tmpr = min( 255, tmpr );
	tmpr = max( 0, tmpr );

	tmpg = (int)( (float)target_col.g + ((255.0f - (float)target_col.g) * rate ) );
	tmpg = min( 255, tmpg );
	tmpg = max( 0, tmpg );

	tmpb = (int)( (float)target_col.b + ((255.0f - (float)target_col.b) * rate ) );
	tmpb = min( 255, tmpb );
	tmpb = max( 0, tmpb );

	dstcol->r = tmpr;
	dstcol->g = tmpg;
	dstcol->b = tmpb;

	return 0;
}

int CPolyMesh2::CloneDispObj( CPolyMesh2* srcpm2, float srcfacet )
{
	int ret;
	ret = CreateBuffers( srcpm2->meshinfo );
	if( ret ){
		DbgOut( "pm2 : CloneDispObj : CreateBuffers error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int bufleng;
	bufleng = meshinfo->n * 3;
	MoveMemory( pointbuf, srcpm2->pointbuf, sizeof( VEC3F ) * bufleng );
	MoveMemory( orgnormal, srcpm2->orgnormal, sizeof( VEC3F ) * bufleng );
	MoveMemory( smoothbuf, srcpm2->smoothbuf, sizeof( int ) * bufleng );
	MoveMemory( samepointbuf, srcpm2->samepointbuf, sizeof( int ) * bufleng );
	MoveMemory( oldpno2optpno, srcpm2->oldpno2optpno, sizeof( int ) * bufleng );
	MoveMemory( uvbuf, srcpm2->uvbuf, sizeof( COORDINATE ) * bufleng );
	MoveMemory( orgnobuf, srcpm2->orgnobuf, sizeof( int ) * bufleng );

	int colbufleng = meshinfo->n;
	MoveMemory( diffusebuf, srcpm2->diffusebuf, sizeof( ARGBF ) * colbufleng );
	MoveMemory( ambientbuf, srcpm2->ambientbuf, sizeof( ARGBF ) * colbufleng );
	MoveMemory( specularbuf, srcpm2->specularbuf, sizeof( float ) * colbufleng );
	MoveMemory( powerbuf, srcpm2->powerbuf, sizeof( float ) * colbufleng );
	MoveMemory( emissivebuf, srcpm2->emissivebuf, sizeof( float ) * colbufleng );
	MoveMemory( diffbuf, srcpm2->diffbuf, sizeof( float ) * colbufleng );
	MoveMemory( m_attrib0, srcpm2->m_attrib0, sizeof( int ) * colbufleng );
	MoveMemory( dirtypoint, srcpm2->dirtypoint, sizeof( char ) * bufleng );
///////////
	ret = CreateOptDataBySamePointBuf( srcpm2->adjustuvflag, srcfacet );
	if( ret ){
		DbgOut( "pm2 : CloneDispObj : CreateOptDataBySamePointBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////////
	ret = CreateInfElemIfNot( optpleng );
	if( ret ){
		DbgOut( "pm2 : CloneDispObj : CreateInfElemIfNot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int infno;
	for( infno = 0; infno < optpleng; infno++ ){
		CInfElem* srcie = srcpm2->m_IE + infno;
		CInfElem* dstie = m_IE + infno;
		ret = dstie->CopyIE( srcie, 0, 0 );
		if( ret ){
			DbgOut( "pm2 : CloneDispObj : ie CopyIE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
