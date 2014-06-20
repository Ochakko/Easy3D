#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>


#include <LWOfile.h>

#include <LWOtags.h>
#include <LWOlayr.h>
#include <LWOsurf.h>
#include <LWOclip.h>

#include <LWOvmap.h>
#include <LWOvmad.h>
#include <LWOpols.h>
#include <LWOptag.h>

#include <byteorder.h>

#include <crtdbg.h>


CLWOfile::CLWOfile()
{
	InitParams();
}
CLWOfile::~CLWOfile()
{
	DestroyObjs();
}

int CLWOfile::InitParams()
{
	InitLoadParams();

	return 0;
}
int CLWOfile::InitLoadParams()
{
	ZeroMemory( &lwobuf, sizeof( LWOBUF ) );
	lwobuf.hfile = INVALID_HANDLE_VALUE;


	m_multiple = 1.0f;

	m_cnktype = LWOCNK_NOTSUPPORT;
	m_subcnktype = LWOSUBCNK_NOTSUPPORT;
	m_blktype = LWOBLK_NOTSUPPORT;
	m_subblktype = LWOSUBBLK_NOTSUPPORT;

	tagsnum = 0;
	pptags = 0;

	layrnum = 0;
	pplayr = 0;

	surfnum = 0;
	ppsurf = 0;

	clipnum = 0;
	ppclip = 0;

	return 0;
}

int CLWOfile::DestroyObjs()
{
	if( lwobuf.hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( lwobuf.hfile );
		lwobuf.hfile = INVALID_HANDLE_VALUE;
	}

	if( tagsnum > 0 ){
		int tagsno;
		for( tagsno = 0; tagsno < tagsnum; tagsno++ ){
			CLWOtags* deltags;
			deltags = *( pptags + tagsno );
			if( deltags )
				delete deltags;
		}
	}
	tagsnum = 0;
	if( pptags ){
		free( pptags );
		pptags = 0;
	}

	if( layrnum > 0 ){
		int layrno;
		for( layrno = 0; layrno < layrnum; layrno++ ){
			CLWOlayr* dellayr;
			dellayr = *( pplayr + layrno );
			if( dellayr )
				delete dellayr;
		}
	}
	layrnum = 0;
	if( pplayr ){
		free( pplayr );
		pplayr = 0;
	}

	if( surfnum > 0 ){
		int surfno;
		for( surfno = 0; surfno < surfnum; surfno++ ){
			CLWOsurf* delsurf;
			delsurf = *( ppsurf + surfno );
			if( delsurf )
				delete delsurf;
		}
	}
	surfnum = 0;
	if( ppsurf ){
		free( ppsurf );
		ppsurf = 0;
	}

	if( clipnum > 0 ){
		int clipno;
		for( clipno = 0; clipno < clipnum; clipno++ ){
			CLWOclip* delclip;
			delclip = *( ppclip + clipno );
			if( delclip )
				delete delclip;
		}
	}
	clipnum = 0;
	if( ppclip ){
		free( ppclip );
		ppclip = 0;
	}


	return 0;
}

/***
private:
	LWOBUF lwobuf;

	float m_multiple;


	LWOCNKTYPE m_cnktype;
	LWOSUBCNKTYPE m_subcnktype;
	LWOBLKTYPE m_blktype;
	LWOSUBBLKTYPE m_subblktype;


	int tagsnum;
	CLWOtags**	pptags;
	
	int layrnum;
	CLWOlayr**	pplayr;

	int surfnum;
	CLWOsurf**	ppsurf;

	int clipnum;
	CLWOclip** ppclip;
***/

CLWOtags* CLWOfile::AddLWOtags()
{
	CLWOtags* newtags = 0;

	pptags = (CLWOtags**)realloc( pptags, sizeof( CLWOtags* ) * (tagsnum + 1) );
	if( !pptags ){
		DbgOut( "LWOfile : AddLWOtags : pptags alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	newtags = new CLWOtags();
	if( !newtags ){
		DbgOut( "LWOfile : AddLWOtags : newtags alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( pptags + tagsnum ) = newtags;
	tagsnum++;

	return newtags;
}

CLWOlayr* CLWOfile::GetCurrentLAYR()
{
	if( layrnum != 0 ){
		return *( pplayr + layrnum - 1 );
	}else{

		CLWOlayr* newlayr = 0;
		
		newlayr = AddLWOlayr();
		if( !newlayr ){
			DbgOut( "LWOfile : CreateDefaultLAYR : AddLWOlayr error !!!\n" );
			_ASSERT( 0 );
			return 0;
		}
		int ret;
		D3DXVECTOR3 zerovec( 0.0f, 0.0f, 0.0f );
		ret = newlayr->SetLayerParams( 0xFFFFFFFF, 0, zerovec, "defaultCreate", 0xFFFFFFFF );
		if( ret ){
			DbgOut( "LWOfile : CreateDefaultLAYR : SetLayerParams error !!!\n" );
			_ASSERT( 0 );
			return 0;
		}
		return newlayr;
	}
}

CLWOlayr* CLWOfile::AddLWOlayr()
{
	CLWOlayr* newlayr = 0;

	pplayr = (CLWOlayr**)realloc( pplayr, sizeof( CLWOlayr* ) * (layrnum + 1) );
	if( !pplayr ){
		DbgOut( "LWOfile : AddLWOlayr : pplayr alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	newlayr = new CLWOlayr();
	if( !newlayr ){
		DbgOut( "LWOfile : AddLWOlayr : newlayr alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( pplayr + layrnum ) = newlayr;
	layrnum++;

	return newlayr;
}
CLWOsurf* CLWOfile::AddLWOsurf()
{
	CLWOsurf* newsurf = 0;

	ppsurf = (CLWOsurf**)realloc( ppsurf, sizeof( CLWOsurf* ) * (surfnum + 1) );
	if( !ppsurf ){
		DbgOut( "LWOfile : AddLWOsurf : ppsurf alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	newsurf = new CLWOsurf();
	if( !newsurf ){
		DbgOut( "LWOfile : AddLWOsurf : newsurf alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( ppsurf + surfnum ) = newsurf;
	surfnum++;

	return newsurf;
}
CLWOclip* CLWOfile::AddLWOclip()
{
	CLWOclip* newclip = 0;

	ppclip = (CLWOclip**)realloc( ppclip, sizeof( CLWOclip* ) * (clipnum + 1) );
	if( !ppclip ){
		DbgOut( "LWOfile : AddLWOclip : ppclip alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	newclip = new CLWOclip();
	if( !newclip ){
		DbgOut( "LWOfile : AddLWOclip : newclip alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( ppclip + clipnum ) = newclip;
	clipnum++;

	return newclip;
}

//////

int CLWOfile::LoadLWOFile( float multiple, char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, int groundflag )
{
	int ret = 0;

	DestroyObjs();
	InitLoadParams();

	ret = CheckDataSize();// byteorder.cpp
	if( ret != 1 ){
		ret = 2;//!!!
		DbgOut( "LWOfile : LoadLWOFile : CheckDataSize error !!!\n" );
		_ASSERT( 0 );
		goto ldlwoexit;
	}

	ret = InitLWOBuf( filename );
	if( ret ){
		DbgOut( "LWOfile : LoadLWOFile : InitLWOBuf error !!!\n" );
		_ASSERT( 0 );
		goto ldlwoexit;
	}

	ret = CheckFileHeader();
	if( ret ){
		DbgOut( "LWOfile : LoadLWOFile : CheckFileHeader error !!!\n" );
		_ASSERT( 0 );
		goto ldlwoexit;
	}


	unsigned int cnkleng;

	while( 1 ){
		ret = GetCnkType( &cnkleng );
		if( (m_cnktype == LWOCNK_FINISH) || (ret != 0) ){
			if( ret != 0 ){
				DbgOut( "LWOfile : LoadLWOFile : GetCnkType error !!!\n" );
				ret = 1;
			}
			break;
		}

		switch( m_cnktype ){
		case LWOCNK_LAYR:
			ret = LoadChunkLAYR( cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOFile : LoadChunkLAYR error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;
		case LWOCNK_PNTS:
			ret = LoadChunkPNTS( cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOFile : LoadChunkPNTS error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;
		case LWOCNK_VMAP:
			ret = LoadChunkVMAP( cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOfile : LoadChunkVMAP error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;
		case LWOCNK_POLS:
			ret = LoadChunkPOLS( cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOFile : LoadChunkPOLS error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;
		case LWOCNK_TAGS:
			ret = LoadChunkTAGS( cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOFile : LoadChunkTAGS error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;
		case LWOCNK_PTAG:
			ret = LoadChunkPTAG( cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOFile : LoadChunkPTAG error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;
		case LWOCNK_VMAD:
			ret = LoadChunkVMAD( cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOfile : LoadChunkVMAD error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;
		case LWOCNK_CLIP:
			ret = LoadChunkCLIP( cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOfile : LoadChunkCLIP error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;
		case LWOCNK_SURF:
			ret = LoadChunkSURF( cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOfile : LoadChunkSURF error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;
		case LWOCNK_BBOX:
			ret = LoadChunkBBOX( cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOfile : LoadChunkBBOX error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;
		case LWOCNK_NOTSUPPORT:
			ret = SkipLoad( 0, cnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadLWOFile : SkipLoad error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto ldlwoexit;
			}
			break;

		default:
			DbgOut( "LWOfile : LoadLWOFile : illegal chunk type error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto ldlwoexit;
			break;

		//LWOCNK_FINISH,
		//LWOCNK_MAX
		}
	}



	goto ldlwoexit;
ldlwoexit:
	if( lwobuf.hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( lwobuf.hfile );
		lwobuf.hfile = INVALID_HANDLE_VALUE;
	}
	
	if( lwobuf.buf ){
		free( lwobuf.buf );
		lwobuf.buf = 0;
	}
	return ret;
}


int CLWOfile::InitLWOBuf( char* srcname )
{

	lwobuf.hfile = CreateFile( (LPCTSTR)srcname, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( lwobuf.hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "LWOfile : InitLWOBuf : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
	}	
	

	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( lwobuf.hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "LWOfile : InitLWOBuf :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "LWOfile : InitLWOBuf :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned char* newbuf;
	newbuf = (unsigned char*)malloc( sizeof( unsigned char ) * bufleng );
	if( !newbuf ){
		DbgOut( "LWOfile : InitLWOBuf :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( lwobuf.hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "LWOfile : InitLWOBuf :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}
	
	lwobuf.buf = newbuf;
	lwobuf.bufleng = bufleng;
	lwobuf.pos = 0;
	lwobuf.isend = 0;

	return 0;
}

int CLWOfile::CheckFileHeader()
{
	int ret;
	char id4[5];
	int offset = 0;
	int stepnum;

	ret = ReadID4( id4, offset, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : CheckFileHeader : ReadID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( strcmp( id4, "FORM" ) != 0 ){
		DbgOut( "LWOfile : CheckFileHeader : id4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned int datalen;
	ret = ReadU4( &datalen, offset, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : CheckFileHeader : ReadU4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned int calclen;
	calclen = lwobuf.bufleng - 8;

	if( datalen != calclen ){
		if( datalen > calclen ){
			DbgOut( "LWOfile : CheckFileHeader : datalen error %d %d !!!\n", datalen, calclen );
			_ASSERT( 0 );
			return 1;
		}else{
			DbgOut( "LWOfile : CheckFileHeader : datalen warning %d %d!!!\n", datalen, calclen ); 			
		}
	}
	lwobuf.bufleng = datalen + 8;//!!!	


	return 0;
}

int CLWOfile::GetCnkType( unsigned int* lengptr )
{

	// file の終わりをチェック
	if( (lwobuf.pos + 4) > lwobuf.bufleng ){
		m_cnktype = LWOCNK_FINISH;
		*lengptr = 0;
		lwobuf.isend = 1;
		return 0;
	}


	//
	int ret;
	char id4[5];
	int offset = 0;
	int stepnum;


	ret = ReadID4( id4, offset, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : GetCnkType : ReadID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LWOCNKTYPE findcnk = LWOCNK_MAX;
	int typeno;
	for( typeno = 0; typeno <= LWOCNK_BBOX; typeno++ ){
		if( strcmp( id4, strcnktype[typeno] ) == 0 ){
			findcnk = (LWOCNKTYPE)typeno;
			break;
		}
	}

	if( findcnk != LWOCNK_MAX ){
		m_cnktype = findcnk;
	}else{
		m_cnktype = LWOCNK_NOTSUPPORT;
	}

	ret = ReadU4( lengptr, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : GetCnkType : ReadU4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (lwobuf.pos + *lengptr) > lwobuf.bufleng ){
		DbgOut( "LWOfile : GetCnkType : chunk leng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CLWOfile::GetSubCnkType( unsigned int* lengptr )
{

	// file の終わりをチェック
	if( (lwobuf.pos + 4) > lwobuf.bufleng ){
		m_subcnktype = LWOSUBCNK_FINISH;
		*lengptr = 0;
		lwobuf.isend = 1;
		return 0;
	}


	//
	int ret;
	char id4[5];
	int offset = 0;
	int stepnum;


	ret = ReadID4( id4, offset, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : GetSubCnkType : ReadID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LWOSUBCNKTYPE findsubcnk = LWOSUBCNK_MAX;
	int typeno;
	for( typeno = 0; typeno <= LWOSUBCNK_SURFBLOK; typeno++ ){
		if( strcmp( id4, strsubcnktype[typeno] ) == 0 ){
			findsubcnk = (LWOSUBCNKTYPE)typeno;
			break;
		}
	}

	if( findsubcnk != LWOSUBCNK_MAX ){
		m_subcnktype = findsubcnk;
	}else{
		m_subcnktype = LWOSUBCNK_NOTSUPPORT;
	}

	ret = ReadU2( lengptr, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : GetSubCnkType : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (lwobuf.pos + *lengptr) > lwobuf.bufleng ){
		DbgOut( "LWOfile : GetSubCnkType : subchunk leng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::GetBlkType( unsigned int* lengptr )
{

	// file の終わりをチェック
	if( (lwobuf.pos + 4) > lwobuf.bufleng ){
		m_blktype = LWOBLK_FINISH;
		*lengptr = 0;
		lwobuf.isend = 1;
		return 0;
	}


	//
	int ret;
	char id4[5];
	int offset = 0;
	int stepnum;


	ret = ReadID4( id4, offset, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : GetBlkType : ReadID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LWOBLKTYPE findblk = LWOBLK_MAX;
	int typeno;
	for( typeno = 0; typeno <= LWOBLK_VMAP; typeno++ ){
		if( strcmp( id4, strblktype[typeno] ) == 0 ){
			findblk = (LWOBLKTYPE)typeno;
			break;
		}
	}

	if( findblk != LWOBLK_MAX ){
		m_blktype = findblk;
	}else{
		m_blktype = LWOBLK_NOTSUPPORT;
	}

	ret = ReadU2( lengptr, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : GetBlkType : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (lwobuf.pos + *lengptr) > lwobuf.bufleng ){
		DbgOut( "LWOfile : GetBlkType : blk leng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::GetSubBlkType( unsigned int* lengptr )
{

	// file の終わりをチェック
	if( (lwobuf.pos + 4) > lwobuf.bufleng ){
		m_subblktype = LWOSUBBLK_FINISH;
		*lengptr = 0;
		lwobuf.isend = 1;
		return 0;
	}


	//
	int ret;
	char id4[5];
	int offset = 0;
	int stepnum;


	ret = ReadID4( id4, offset, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : GetSubBlkType : ReadID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LWOSUBBLKTYPE findsubblk = LWOSUBBLK_MAX;
	int typeno;
	for( typeno = 0; typeno <= LWOSUBBLK_TMCSYS; typeno++ ){
		if( strcmp( id4, strsubcnktype[typeno] ) == 0 ){
			findsubblk = (LWOSUBBLKTYPE)typeno;
			break;
		}
	}

	if( findsubblk != LWOSUBBLK_MAX ){
		m_subblktype = findsubblk;
	}else{
		m_subblktype = LWOSUBBLK_NOTSUPPORT;
	}

	ret = ReadU2( lengptr, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : GetSubBlkType : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (lwobuf.pos + *lengptr) > lwobuf.bufleng ){
		DbgOut( "LWOfile : GetSubBlkType : subblk leng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::SkipLoad( int offset, unsigned int skipleng )
{
	if( (lwobuf.pos + offset + skipleng) > lwobuf.bufleng ){
		DbgOut( "LWOfile : SkipLoad : pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	lwobuf.pos += skipleng;

	return 0;
}

int CLWOfile::LoadChunkLAYR( unsigned int cnkleng )
{
	int ret;
	//unsigned int cnkleng;
	unsigned int totalstep = 0;
	int stepnum;


	// !!! chunk の length は、GetCnkType ですでに読み込み済

	//ret = ReadU4( &cnkleng, 0, &stepnum );
	//if( ret ){
	//	DbgOut( "LWOfile : LoadChunkLAYR : ReadU4 error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}

	unsigned int number;
	unsigned int flags;
	unsigned int parent;
	D3DXVECTOR3 pivot;
	char name[1024];

	ret = ReadU2( &number, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkLAYR : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadU2( &flags, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkLAYR : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	totalstep += stepnum;

	ret = ReadVEC12( &pivot.x, &pivot.y, &pivot.z, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkLAYR : ReadVEC12 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	ret = ReadS0( name, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkLAYR : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	if( (cnkleng - totalstep) >= 2 ){
		ret = ReadU2( &parent, 0, &stepnum );
		totalstep += stepnum;
	}else{
		parent = 0xFFFFFFFF;
	}


	if( cnkleng != totalstep ){
		DbgOut( "LWOfile : LoadChunkLAYR : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	// layrの作成
	CLWOlayr* newlayr;
	newlayr = AddLWOlayr();
	if( !newlayr ){
		DbgOut( "LWOfile : LoadChunkLAYR : AddLWOlayr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	// layrのセット
	ret = newlayr->SetLayerParams( number, flags, pivot, name, parent );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkLAYR : SetLayerParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CLWOfile::LoadChunkPNTS( unsigned int cnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;
	
	unsigned int pointnum;
	pointnum = cnkleng / 12;

	if( (pointnum * 12) != cnkleng ){
		//　チャンクの長さが、VEC12 で割り切れなかった場合。
		DbgOut( "LWOfile : LoadChunkPNTS : cnkleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CLWOlayr* curlayr;
	curlayr = GetCurrentLAYR();
	if( !curlayr ){
		DbgOut( "LWOfile : LoadChunkPNTS : GetCurrentLAYR error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curlayr->CreatePNTS( (int)pointnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkPNTS : CreatePNTS error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned int pointno;
	for( pointno = 0; pointno < pointnum; pointno++ ){
		D3DXVECTOR3 vec;
		ret = ReadVEC12( &vec.x, &vec.y, &vec.z, 0, &stepnum );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkPNTS : ReadVEC12 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		totalstep += stepnum;

		ret = curlayr->SetPNTS( (int)pointno, vec );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkPNTS : layr SetPNTS error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	

	if( totalstep != cnkleng ){
		DbgOut( "LWOfile : LoadChunkPNTS : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadChunkVMAP( unsigned int cnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	char id4[5];
	unsigned int dimension;
	char name[1024];


	CLWOlayr* curlayr;
	curlayr = GetCurrentLAYR();
	if( !curlayr ){
		DbgOut( "LWOfile : LoadChunkVMAP : GetCurrentLAYR error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/////
	id4[0] = 0;
	ret = ReadID4( id4, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAP : ReadID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;
	
	ret = ReadU2( &dimension, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAP : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadS0( name, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAP : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = CheckVMAPdimension( id4, (int)dimension );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAP : CheckVMAPdimension error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//////
	int datanum;
	ret = CheckVMAPDataNum( cnkleng - totalstep, (int)dimension, &datanum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAP : CheckVMAPDataNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////
	CLWOvmap* newvmap;
	newvmap = curlayr->AddVmap();
	if( !newvmap ){
		DbgOut( "LWOfile : layr AddVmap error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newvmap->SetParams( name, curlayr->vmapnum - 1, id4, (int)dimension, datanum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAP : vmap SetParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	int datano;
	unsigned int dimno;
	unsigned int vertindex;
	float* valueptr;

	valueptr = (float*)malloc( sizeof( float ) * dimension );
	if( !valueptr ){
		DbgOut( "LWOfile : LoadChunkVMAP : valueptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	for( datano = 0; datano < datanum; datano++ ){
		ret = ReadVX( &vertindex, 0, &stepnum );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkVMAP : ReadVX error !!!\n" );
			_ASSERT( 0 );
			free( valueptr );
			return 1;
		}
		totalstep += stepnum;

		for( dimno = 0; dimno < dimension; dimno++ ){
			ret = ReadF4( valueptr + dimno, 0, &stepnum );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkVMAP : ReadF4 error !!!\n" );
				_ASSERT( 0 );
				free( valueptr );
				return 1;
			}
			totalstep += stepnum;
		}

		ret = newvmap->SetData( datano, (int)vertindex, valueptr, dimension );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkVMAP : vmap SetData error !!!\n" );
			_ASSERT( 0 );
			free( valueptr );
			return 1;
		}

	}

	free( valueptr );//!!!!!!


	if( totalstep != cnkleng ){
		DbgOut( "LWOfile : LoadChunkVMAP : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadChunkVMAD( unsigned int cnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	char id4[5];
	unsigned int dimension;
	char name[1024];


	CLWOlayr* curlayr;
	curlayr = GetCurrentLAYR();
	if( !curlayr ){
		DbgOut( "LWOfile : LoadChunkVMAD : GetCurrentLAYR error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/////
	id4[0] = 0;
	ret = ReadID4( id4, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAD : ReadID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;
	
	ret = ReadU2( &dimension, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAD : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadS0( name, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAD : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = CheckVMAPdimension( id4, (int)dimension );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAD : CheckVMADdimension error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//////
	int datanum;
	ret = CheckVMADDataNum( cnkleng - totalstep, (int)dimension, &datanum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAD : CheckVMADDataNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////
	CLWOvmad* newvmad;
	newvmad = curlayr->AddVmad();
	if( !newvmad ){
		DbgOut( "LWOfile : layr AddVmad error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newvmad->SetParams( name, curlayr->vmadnum - 1, id4, (int)dimension, datanum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkVMAD : vmad SetParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	int datano;
	unsigned int dimno;
	unsigned int vertindex;
	unsigned int polyindex;
	float* valueptr;

	valueptr = (float*)malloc( sizeof( float ) * dimension );
	if( !valueptr ){
		DbgOut( "LWOfile : LoadChunkVMAD : valueptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	for( datano = 0; datano < datanum; datano++ ){
		ret = ReadVX( &vertindex, 0, &stepnum );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkVMAD : ReadVX error !!!\n" );
			_ASSERT( 0 );
			free( valueptr );
			return 1;
		}
		totalstep += stepnum;

		ret = ReadVX( &polyindex, 0, &stepnum );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkVMAD : ReadVX error !!!\n" );
			_ASSERT( 0 );
			free( valueptr );
			return 1;
		}
		totalstep += stepnum;


		for( dimno = 0; dimno < dimension; dimno++ ){
			ret = ReadF4( valueptr + dimno, 0, &stepnum );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkVMAD : ReadF4 error !!!\n" );
				_ASSERT( 0 );
				free( valueptr );
				return 1;
			}
			totalstep += stepnum;
		}

		ret = newvmad->SetData( datano, (int)vertindex, (int)polyindex, valueptr, dimension );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkVMAD : vmad SetData error !!!\n" );
			_ASSERT( 0 );
			free( valueptr );
			return 1;
		}

	}

	free( valueptr );//!!!!!!


	if( totalstep != cnkleng ){
		DbgOut( "LWOfile : LoadChunkVMAD : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadChunkPOLS( unsigned int cnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	char id4[5];
	
	CLWOlayr* curlayr;
	curlayr = GetCurrentLAYR();
	if( !curlayr ){
		DbgOut( "LWOfile : LoadChunkPOLS : GetCurrentLAYR error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	////
	ret = ReadID4( id4, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkPOLS : ReadID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	char polspat[5][5] = {
		"FACE",
		"CURV",
		"PTCH",
		"MBAL",
		"BONE"
	};
	int patno;
	int findpatno = -1;
	for( patno = 0; patno < 5; patno++ ){
		if( strcmp( id4, polspat[patno] ) == 0 ){
			findpatno = patno;
			break;
		}
	}

	if( (patno != 0) && (patno != 4) ){
		// ver1001では、対応しない、読み飛ばす。
		ret = SkipLoad( 0, cnkleng - totalstep );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkPOLS : SkipLoad error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		int datanum;
		ret = CheckPOLSDataNum( cnkleng - totalstep, &datanum );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkPOLS : CheckPOLSDataNum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CLWOpols* newpols;
		newpols = curlayr->AddPols();
		if( !newpols ){
			DbgOut( "LWOfile : LoadChunkPOLS : layr AddPols error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = newpols->CreateData( datanum, id4 );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkPOLS : pols CreateData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		unsigned int datano;
		unsigned int numvert;
		unsigned int* vertptr;

		vertptr = (unsigned int*)malloc( sizeof( unsigned int ) * 1024 );// numvert の最大値は１０２３！！！
		if( !vertptr ){
			DbgOut( "LWOfile : LoadChunkPOLS : vertptr alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		for( datano = 0; datano < (unsigned int)datanum; datano++ ){
			ret = ReadU2( &numvert, 0, &stepnum );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkPOLS : ReadU2 error !!!\n" );
				_ASSERT( 0 );
				free( vertptr );
				return 1;
			}
			numvert &= 0x03FF;//!!!!!! 上位６ビットを取り除く。
			totalstep += stepnum;

			unsigned int vertno;
			for( vertno = 0; vertno < numvert; vertno++ ){
				ret = ReadVX( vertptr + vertno, 0, &stepnum );
				if( ret ){
					DbgOut( "LWOfile : LoadChunkPOLS : ReadVX error !!!\n" );
					_ASSERT( 0 );
					free( vertptr );
					return 1;
				}
				totalstep += stepnum;
			}

			ret = newpols->SetPols( datano, vertptr, numvert );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkPOLS : pols SetPols error !!!\n" );
				_ASSERT( 0 );
				free( vertptr );
				return 1;
			}
		}

		free( vertptr );//!!!!!

		if( totalstep != cnkleng ){
			DbgOut( "LWOfile : LoadChunkPOLS : totalstep error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CLWOfile::LoadChunkTAGS( unsigned int cnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;


	/////

	char tagstr[1024];
	while( totalstep < cnkleng ){
		ret = ReadS0( tagstr, 1024, 0, &stepnum );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkTAGS : ReadS0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		totalstep += stepnum;

		CLWOtags* newtags;
		newtags = AddLWOtags();
		if( !newtags ){
			DbgOut( "LWOfile : LoadChunkTAGS : AddLWOtags error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = newtags->SetParams( tagstr, tagsnum - 1 );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkTAGS : tags SetParams error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	/////

	if( totalstep != cnkleng ){
		DbgOut( "LWOfile : LoadChunkTAGS : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadChunkPTAG( unsigned int cnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	/////
	CLWOlayr* curlayr;
	curlayr = GetCurrentLAYR();
	if( !curlayr ){
		DbgOut( "LWOfile : LoadChunkPTAG : GetCurrentLAYR error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CLWOptag* newptag;
	newptag = curlayr->AddPtag();
	if( !newptag ){
		DbgOut( "LWOfile : LoadChunkPTAG : layr AddPtag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/////
	
	char id4[5];
	ret = ReadID4( id4, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkPTAG : ReadaID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	int datanum;
	ret = CheckPTAGDataNum( cnkleng - totalstep, &datanum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkPTAG : CheckPTAGDataNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newptag->CreateData( id4, datanum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkPTAG : ptag CreateData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int datano;
	unsigned int polyindex;
	unsigned int tagno;
	for( datano = 0; datano < datanum; datano++ ){
		ret = ReadVX( &polyindex, 0, &stepnum );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkPTAG : ReadVX error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		totalstep += stepnum;

		ret = ReadU2( &tagno, 0, &stepnum );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkPTAG : ReadU2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		totalstep += stepnum;

		ret = newptag->SetPtag( datano, (int)polyindex, (int)tagno );
		if( ret ){
			DbgOut( "LWOfile : LoadChunkPTAG : ptag SetPtag error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	/////

	if( totalstep != cnkleng ){
		DbgOut( "LWOfile : LoadChunkTAGS : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadChunkBBOX( unsigned int cnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	/////
	CLWOlayr* curlayr;
	curlayr = GetCurrentLAYR();
	if( !curlayr ){
		DbgOut( "LWOfile : LoadChunkBBOX : GetCurrentLAYR error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/////
	D3DXVECTOR3 minvec, maxvec;
	ret = ReadVEC12( &minvec.x, &minvec.y, &minvec.z, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkBBOX : ReadVEC12 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadVEC12( &maxvec.x, &maxvec.y, &maxvec.z, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkBBOX : ReadVEC12 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	/////
	ret = curlayr->SetBBOX( minvec, maxvec );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkBBOX : layr SetBBOX error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	/////

	if( totalstep != cnkleng ){
		DbgOut( "LWOfile : LoadChunkBBOX : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadChunkCLIP( unsigned int cnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	/////
	CLWOlayr* curlayr;
	curlayr = GetCurrentLAYR();
	if( !curlayr ){
		DbgOut( "LWOfile : LoadChunkCLIP : GetCurrentLAYR error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CLWOclip* newclip;
	newclip = AddLWOclip();
	if( !newclip ){
		DbgOut( "LWOfile : LoadChunkCLIP : AddLWOclip error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////
	
	unsigned int clipindex;
	ret = ReadU4( &clipindex, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkCLIP : ReadU4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = newclip->SetIndex( clipindex );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkClip : clip SetIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	unsigned int subcnkleng;
	while( totalstep < cnkleng ){

		ret = GetSubCnkType( &subcnkleng );
		if( (m_subcnktype == LWOSUBCNK_FINISH) || (ret != 0) ){
			DbgOut( "LWOfile : LWOfile : LoadChunkClip : GetSubCnkType error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		switch( m_subcnktype ){

		case LWOSUBCNK_CLIPSTIL:
			ret = LoadSubChunkCLIPSTIL( newclip, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkClip : LoadSubChunkCLIPSTIL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBCNK_CLIPISEQ:
			ret = LoadSubChunkCLIPISEQ( newclip, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkClip : LoadSubChunkCLIPISEQ error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBCNK_CLIPXREF:
			ret = LoadSubChunkCLIPXREF( newclip, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkClip : LoadSubChunkCLIPXREF error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;

		case LWOSUBCNK_SURFCOLR:
		case LWOSUBCNK_SURFDIFF:
		case LWOSUBCNK_SURFSPEC:
		case LWOSUBCNK_SURFTRAN:
		case LWOSUBCNK_SURFSIDE:
		case LWOSUBCNK_SURFVCOL:
		case LWOSUBCNK_SURFBLOK:

		case LWOSUBCNK_NOTSUPPORT:
			ret = SkipLoad( 0, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkCLIP : SkipLoad error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;

		default:
			DbgOut( "LWOfile : LoadChunkCLIP : illegal subchunk type error !!!\n" );
			_ASSERT( 0 );
			return 1;
			break;
		}

		totalstep += subcnkleng;//!!!!
	}


	//////

	if( totalstep != cnkleng ){
		DbgOut( "LWOfile : LoadChunkBBOX : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CLWOfile::LoadSubChunkCLIPSTIL( CLWOclip* curclip, unsigned int subcnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////

	char name[1024];
	ret = ReadS0( name, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPSTIL : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = curclip->SetStil( name );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPSTIL : clip SetStil error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//////

	if( totalstep != subcnkleng ){
		DbgOut( "LWOfile : LoadSubChunkCLIPSTIL : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadSubChunkCLIPISEQ( CLWOclip* curclip, unsigned int subcnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;


	//////

	unsigned int numdigits;
	unsigned int flags;
	int offset;
	unsigned int reserved;
	int start;
	int end;
	char prefix[1024];
	char suffix[1024];

	ret = ReadU1( &numdigits, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPISEQ : ReadU1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadU1( &flags, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPISEQ : ReadU1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadI2( &offset, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPISEQ : ReadI2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadU2( &reserved, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPISEQ : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadI2( &start, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPISEQ : ReadI2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadI2( &end, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPISEQ : ReadI2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadS0( prefix, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPISEQ : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadS0( suffix, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPISEQ : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	//////

	ret = curclip->SetIseq( (unsigned char)numdigits, (unsigned int)flags, (short)offset, (unsigned short)reserved,
		(short)start, (short)end, 
		prefix, suffix );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPISEQ : clip SetIseq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != subcnkleng ){
		DbgOut( "LWOfile : LoadSubChunkCLIPISEQ : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadSubChunkCLIPXREF( CLWOclip* curclip, unsigned int subcnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////

	unsigned int xrefindex;
	char xrefstr[1024];

	ret = ReadU4( &xrefindex, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPXREF : ReadU4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadS0( xrefstr, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkCLIPXREF : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	//////

	if( totalstep != subcnkleng ){
		DbgOut( "LWOfile : LoadSubChunkCLIPXREF : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadChunkSURF( unsigned int cnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	/////
	CLWOlayr* curlayr;
	curlayr = GetCurrentLAYR();
	if( !curlayr ){
		DbgOut( "LWOfile : LoadChunkSURF : GetCurrentLAYR error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CLWOsurf* newsurf;
	newsurf = AddLWOsurf();
	if( !newsurf ){
		DbgOut( "LWOfile : LoadChunkSURF : AddLWOsurf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////	

	char name[1024];
	char sourcename[1024];

	ret = ReadS0( name, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkSURF : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	ret = ReadS0( sourcename, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkSURF : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	ret = newsurf->SetName( name );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkSURF : surf SetName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newsurf->SetSourceName( sourcename );
	if( ret ){
		DbgOut( "LWOfile : LoadChunkSURF : surf SetSourceName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	unsigned int subcnkleng;
	while( totalstep < cnkleng ){

		ret = GetSubCnkType( &subcnkleng );
		if( (m_subcnktype == LWOSUBCNK_FINISH) || (ret != 0) ){
			DbgOut( "LWOfile : LWOfile : LoadChunkSURF : GetSubCnkType error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		switch( m_subcnktype ){

		case LWOSUBCNK_SURFCOLR:
			ret = LoadSubChunkSURFCOLR( newsurf, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkSURF : LoadSubChunkSURFCOLR error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBCNK_SURFDIFF:
			ret = LoadSubChunkSURFShading( LWOSUBCNK_SURFDIFF, newsurf, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkSURF : LoadSubChunkSURFShading DIFF error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBCNK_SURFSPEC:
			ret = LoadSubChunkSURFShading( LWOSUBCNK_SURFSPEC, newsurf, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkSURF : LoadSubChunkSURFShading SPEC error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBCNK_SURFTRAN:
			ret = LoadSubChunkSURFShading( LWOSUBCNK_SURFTRAN, newsurf, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkSURF : LoadSubChunkSURFShading TRAN error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBCNK_SURFSIDE:
			ret = LoadSubChunkSURFSIDE( newsurf, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkSURF : LoadSubChunkSURFSIDE error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBCNK_SURFVCOL:
			ret = LoadSubChunkSURFVCOL( newsurf, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkSURF : LoadSubChunkSURFVCOL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBCNK_SURFBLOK:

			ret = LoadSubChunkSURFBLOK( newsurf, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkSURF : LoadSubChunkSURFBLOK error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;

		case LWOSUBCNK_CLIPSTIL:
		case LWOSUBCNK_CLIPISEQ:
		case LWOSUBCNK_CLIPXREF:

		case LWOSUBCNK_NOTSUPPORT:
			ret = SkipLoad( 0, subcnkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadChunkSURF : SkipLoad error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;

		default:
			DbgOut( "LWOfile : LoadChunkSURF : illegal subchunk type error !!!\n" );
			_ASSERT( 0 );
			return 1;
			break;
		}

		totalstep += subcnkleng;//!!!!
	}


	//////

	if( totalstep != cnkleng ){
		DbgOut( "LWOfile : LoadChunkBBOX : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CLWOfile::LoadSubChunkSURFCOLR( CLWOsurf* cursurf, unsigned int subcnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;


	//////

	D3DXVECTOR3 col;
	unsigned int envno;

	ret = ReadVEC12( &col.x, &col.y, &col.z, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFCOLR : ReadVEC12 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadVX( &envno, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFCOLR : ReadVX error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	//////

	if( totalstep != subcnkleng ){
		DbgOut( "LWOfile : LoadSubChunkSURFCOLR : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadSubChunkSURFShading( LWOSUBCNKTYPE type, CLWOsurf* cursurf, unsigned int subcnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////
	float floatval;
	unsigned int envno;

	ret = ReadF4( &floatval, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFShading : ReadF4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	ret = ReadVX( &envno, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFShading : ReadVX error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	switch( type ){
	case LWOSUBCNK_SURFDIFF:
		ret = cursurf->SetDiff( floatval );
		if( ret ){
			DbgOut( "LWOfile : LoadSubChunkSURFShading : surf SetDiff error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case LWOSUBCNK_SURFSPEC:
		ret = cursurf->SetSpec( floatval );
		if( ret ){
			DbgOut( "LWOfile : LoadSubChunkSURFShading : surf SetSpec error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case LWOSUBCNK_SURFTRAN:
		ret = cursurf->SetTran( floatval );
		if( ret ){
			DbgOut( "LWOfile : LoadSubChunkSURFShading : surf SetTran error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		DbgOut( "LWOfile : LoadSubChunkSURFShading : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
		break;
	}


	//////

	if( totalstep != subcnkleng ){
		DbgOut( "LWOfile : LoadSubChunkSURFShading : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CLWOfile::LoadSubChunkSURFSIDE( CLWOsurf* cursurf, unsigned int subcnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;


	//////

	unsigned int side;
	ret = ReadU2( &side, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFSIDE : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = cursurf->SetSide( side );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFSIDE : surf SetSide error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != subcnkleng ){
		DbgOut( "LWOfile : LoadSubChunkSURFSIDE : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadSubChunkSURFVCOL( CLWOsurf* cursurf, unsigned int subcnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;


	//////

	float intensity;
	unsigned int envno;
	char typestr[5];
	char name[1024];

	ret = ReadF4( &intensity, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFVCOL : ReadF4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadVX( &envno, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFVCOL : ReadVX error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadID4( typestr, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFVCOL : ReadID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = ReadS0( name, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFVCOL : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	//////

	ret = cursurf->SetVcol( typestr, name );
	if( ret ){
		DbgOut( "LWOfile : LoadSubChunkSURFVCOL : surf SetVcol error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != subcnkleng ){
		DbgOut( "LWOfile : LoadSubChunkSURFVCOL : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadSubChunkSURFBLOK( CLWOsurf* cursurf, unsigned int subcnkleng )
{
	int ret;
	unsigned int totalstep = 0;
	//int stepnum;

	/////

	CLWOsurfblok* newblok;
	newblok = cursurf->AddSurfBlock();
	if( !newblok ){
		DbgOut( "LWOfile : LoadSubChunkSURFBLOK : surf AddSurfBlock error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//////

	unsigned int blkleng;
	while( totalstep < subcnkleng ){

		ret = GetBlkType( &blkleng );
		if( (m_blktype == LWOBLK_FINISH) || (ret != 0) ){
			DbgOut( "LWOfile : LoadSubChunkSURFBLOK : GetBlkType error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		switch( m_blktype ){

		case LWOBLK_IMAP:
			//orderstr を忘れずに！！！
			ret = LoadBlkIMAP( newblok, blkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadSubChunkSURFBLOK : LoadBlkIMAP error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOBLK_TMAP:
			ret = LoadBlkTMAP( newblok, blkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadSubChunkSURFBLOK : LoadBlkTMAP error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;

		case LWOBLK_PROJ:
			ret = LoadBlkPROJ( newblok, blkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadSubChunkSURFBLOK : LoadBlkPROJ error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOBLK_AXIS:
			ret = LoadBlkAXIS( newblok, blkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadSubChunkSURFBLOK : LoadBlkAXIS error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOBLK_IMAG:
			ret = LoadBlkIMAG( newblok, blkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadSubChunkSURFBLOK : LoadBlkIMAG error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOBLK_WRAP:
			ret = LoadBlkWRAP( newblok, blkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadSubChunkSURFBLOK : LoadBlkWRAP error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOBLK_WRPW:
			ret = LoadBlkWrapAmount( LWOBLK_WRPW, newblok, blkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadSubChunkSURFBLOK : LoadBlkWrapAmount error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOBLK_WRPH:
			ret = LoadBlkWrapAmount( LWOBLK_WRPH, newblok, blkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadSubChunkSURFBLOK : LoadBlkWrapAmount error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOBLK_VMAP:
			ret = LoadBlkVMAP( newblok, blkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadSubChunkSURFBLOK : LoadBlkVMAP error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		
		case LWOBLK_NOTSUPPORT:
			ret = SkipLoad( 0, blkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadSubChunkSURFBLOK : SkipLoad error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;

		default:
			DbgOut( "LWOfile : LoadSubChunkSURFBLOK : illegal block type error !!!\n" );
			_ASSERT( 0 );
			return 1;
			break;
		}

		totalstep += blkleng;//!!!!
	}


	//////

	if( totalstep != subcnkleng ){
		DbgOut( "LWOfile : LoadSubChunkSURFBLOK : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadBlkPROJ( CLWOsurfblok* curblk, unsigned int blkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////
	unsigned int projmode;
	ret = ReadU2( &projmode, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkPROJ : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	//////

	ret = curblk->SetProj( (int)projmode );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkPROJ : blk SetProj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != blkleng ){
		DbgOut( "LWOfile : LoadBlkPROJ : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadBlkAXIS( CLWOsurfblok* curblk, unsigned int blkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////
	unsigned int axis;
	ret = ReadU2( &axis, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkAXIS : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	//////

	ret = curblk->SetAxis( (int)axis );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkAXIS : blk SetAxis error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != blkleng ){
		DbgOut( "LWOfile : LoadBlkAXIS : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadBlkIMAG( CLWOsurfblok* curblk, unsigned int blkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////
	unsigned int imagno;
	ret = ReadVX( &imagno, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkIMAG : ReadVX error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	//////

	ret = curblk->SetImag( (int)imagno );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkIMAG : blk SetImag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != blkleng ){
		DbgOut( "LWOfile : LoadBlkIMAG : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadBlkWRAP( CLWOsurfblok* curblk, unsigned int blkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////
	unsigned int wwrap, hwrap;
	ret = ReadU2( &wwrap, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkWRAP : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	ret = ReadU2( &hwrap, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkWRAP : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	//////

	ret = curblk->SetWidthWrap( (int)wwrap );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkWRAP : blk SetWidthWrap error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curblk->SetHeightWrap( (int)hwrap );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkWRAP : blk SetHeightWrap error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != blkleng ){
		DbgOut( "LWOfile : LoadBlkIMAG : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
int CLWOfile::LoadBlkWrapAmount( LWOBLKTYPE blktype, CLWOsurfblok* curblk, unsigned int blkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////
	float cycles;
	unsigned int envno;

	ret = ReadF4( &cycles, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkWrapAmount : ReadF4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	ret = ReadVX( &envno, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkWrapAmount : ReadVX error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	//////
	
	if( blktype == LWOBLK_WRPW ){
		ret = curblk->SetWrapW( cycles );
		if( ret ){
			DbgOut( "LWOfile : LoadBlkWrapAmount : blk SetWrapW error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( blktype == LWOBLK_WRPH ){
		ret = curblk->SetWrapH( cycles );
		if( ret ){
			DbgOut( "LWOfile : LoadBlkWrapAmount : blk SetWrapH error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		DbgOut( "LWOfile : LoadBlkWrapAmount : blktype error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != blkleng ){
		DbgOut( "LWOfile : LoadBlkIMAG : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
int CLWOfile::LoadBlkVMAP( CLWOsurfblok* curblk, unsigned int blkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////
	char mapname[1024];
	ret = ReadS0( mapname, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkVMAP : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	//////

	ret = curblk->SetVmapname( mapname );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkVMAP : blk SetVmapname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != blkleng ){
		DbgOut( "LWOfile : LoadBlkIMAG : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadBlkIMAP( CLWOsurfblok* curblk, unsigned int blkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	/////

	char orderstr[1024];
	ret = ReadS0( orderstr, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkIMAP : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	ret = curblk->SetImOrderStr( orderstr );
	if( ret ){
		DbgOut( "LWOfile : LoadBlkIMAP : blk SetImOrderStr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	unsigned int subblkleng;
	while( totalstep < blkleng ){

		ret = GetSubBlkType( &subblkleng );
		if( (m_subblktype == LWOSUBBLK_FINISH) || (ret != 0) ){
			DbgOut( "LWOfile : LoadBlkIMAP : GetSubBlkType error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		switch( m_subblktype ){

		case LWOSUBBLK_IMCHAN:
			ret = LoadSubBlkIMCHAN( curblk, subblkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadBlkIMAP : LoadSubBlkIMCHAN error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBBLK_IMENAB:
			ret = LoadSubBlkIMENAB( curblk, subblkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadBlkIMAP : LoadSubBlkIMENAB error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;

		case LWOSUBBLK_TMCNTR:
		case LWOSUBBLK_TMSIZE:
		case LWOSUBBLK_TMROTA:
		case LWOSUBBLK_TMOREF:
		case LWOSUBBLK_TMCSYS:

		case LWOBLK_NOTSUPPORT:
			ret = SkipLoad( 0, subblkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadBlkIMAP : SkipLoad error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;

		default:
			DbgOut( "LWOfile : LoadBlkIMAP : illegal subblock type error !!!\n" );
			_ASSERT( 0 );
			return 1;
			break;
		}

		totalstep += subblkleng;//!!!!
	}


	//////

	if( totalstep != blkleng ){
		DbgOut( "LWOfile : LoadBlkIMAP : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadSubBlkIMCHAN( CLWOsurfblok* curblk, unsigned int subblkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////
	char id4[5];
	ret = ReadID4( id4, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubBlkIMCHAN : ReadID4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	//////

	ret = curblk->SetImChan( id4 );
	if( ret ){
		DbgOut( "LWOfile : LoadSubBlkIMCHAN : blk SetImChan error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != subblkleng ){
		DbgOut( "LWOfile : LoadSubBlkIMCHAN : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadSubBlkIMENAB( CLWOsurfblok* curblk, unsigned int subblkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////
	unsigned int enab;
	ret = ReadU2( &enab, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubBlkIMENAB : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;

	//////

	ret = curblk->SetImEnab( enab );
	if( ret ){
		DbgOut( "LWOfile : LoadSubBlkIMENAB : blk SetImEnab error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != subblkleng ){
		DbgOut( "LWOfile : LoadSubBlkIMENAB : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadBlkTMAP( CLWOsurfblok* curblk, unsigned int blkleng )
{

	int ret;
	unsigned int totalstep = 0;
	//int stepnum;

	/////

	unsigned int subblkleng;
	while( totalstep < blkleng ){

		ret = GetSubBlkType( &subblkleng );
		if( (m_subblktype == LWOSUBBLK_FINISH) || (ret != 0) ){
			DbgOut( "LWOfile : LoadBlkIMAP : GetSubBlkType error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		switch( m_subblktype ){

		case LWOSUBBLK_TMCNTR:
		case LWOSUBBLK_TMSIZE:
		case LWOSUBBLK_TMROTA:
			ret = LoadSubBlkTMCSR( m_subblktype, curblk, subblkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadBlkTMAP : LoadSubBlkCSR error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBBLK_TMOREF:
			ret = LoadSubBlkTMOREF( curblk, subblkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadBlkTMAP : LoadSubBlkTMOREF error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case LWOSUBBLK_TMCSYS:
			ret = LoadSubBlkTMCSYS( curblk, subblkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadBlkTMAP : LoadSubBlkTMCSYS error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;

		case LWOSUBBLK_IMCHAN:
		case LWOSUBBLK_IMENAB:

		case LWOBLK_NOTSUPPORT:
			ret = SkipLoad( 0, subblkleng );
			if( ret ){
				DbgOut( "LWOfile : LoadBlkTMAP : SkipLoad error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;

		default:
			DbgOut( "LWOfile : LoadBlkTMAP : illegal subblock type error !!!\n" );
			_ASSERT( 0 );
			return 1;
			break;
		}

		totalstep += subblkleng;//!!!!
	}


	//////

	if( totalstep != blkleng ){
		DbgOut( "LWOfile : LoadBlkIMAP : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadSubBlkTMCSR( LWOSUBBLKTYPE subblktype, CLWOsurfblok* curblk, unsigned int subblkleng )
{

	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////
	D3DXVECTOR3 vec;
	unsigned int envno;

	ret = ReadVEC12( &vec.x, &vec.y, &vec.z, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubBlkTMCSR : ReadVEC12 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	ret = ReadVX( &envno, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubBlkTMCSR : ReadVX error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	//////

	switch( subblktype ){
	case LWOSUBBLK_TMCNTR:
		ret = curblk->SetTmCntr( vec );
		if( ret ){
			DbgOut( "LWOfile : LoadSubBlkTMCSR : blk SetTmCntr error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case LWOSUBBLK_TMSIZE:
		ret = curblk->SetTmSize( vec );
		if( ret ){
			DbgOut( "LWOfile : LoadSubBlkTMCSR : blk SetTmSize error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case LWOSUBBLK_TMROTA:
		ret = curblk->SetTmRota( vec );
		if( ret ){
			DbgOut( "LWOfile : LoadSubBlkTMCSR : blk SetTmRota error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		DbgOut( "LWOfile : LoadSubBlkTMCSR : subblocktype error !!!\n" );
		_ASSERT( 0 );
		return 1;
		break;
	}

	//////

	if( totalstep != subblkleng ){
		DbgOut( "LWOfile : LoadSubBlkTMCSR : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadSubBlkTMOREF( CLWOsurfblok* curblk, unsigned int subblkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////

	char objname[1024];
	ret = ReadS0( objname, 1024, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubBlkTMOREF : ReadS0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	//////

	ret = curblk->SetTmOref( objname );
	if( ret ){
		DbgOut( "LWOfile : LoadSubBlkTMOREF : blk SetTmOref error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//////

	if( totalstep != subblkleng ){
		DbgOut( "LWOfile : LoadSubBlkTMOREF : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::LoadSubBlkTMCSYS( CLWOsurfblok* curblk, unsigned int subblkleng )
{
	int ret;
	unsigned int totalstep = 0;
	int stepnum;

	//////

	unsigned int systype;
	ret = ReadU2( &systype, 0, &stepnum );
	if( ret ){
		DbgOut( "LWOfile : LoadSubBlkTMCSYS : ReadU2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	totalstep += stepnum;


	//////

	ret = curblk->SetTmCsys( systype );
	if( ret ){
		DbgOut( "LWOfile : LoadSubBlkTMCSYS : blk SetTmCsys error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////

	if( totalstep != subblkleng ){
		DbgOut( "LWOfile : LoadSubBlkTMCSYS : totalstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CLWOfile::ReadID4( char* id4ptr, int offset, int* stepnum )
{
	if( (lwobuf.pos + offset + 4) > lwobuf.bufleng ){
		DbgOut( "LWOfile : ReadID4 : pos error !!!\n" );
		_ASSERT( 0 );
		*id4ptr = 0;
		return 1;
	}

	strncpy( id4ptr, (char*)(lwobuf.buf + lwobuf.pos + offset), 4 );
	*( id4ptr + 4 ) = 0;

	*stepnum = 4;
	lwobuf.pos += 4;//!!!!


	return 0;
}


int CLWOfile::ReadI1( int* iptr, int offset, int* stepnum )
{
	if( (lwobuf.pos + offset + 1) > lwobuf.bufleng ){
		DbgOut( "LWOfile : ReadI1: pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	char* tempcptr;
	tempcptr = (char*)(lwobuf.buf + lwobuf.pos + offset);

	*iptr = (int)(*tempcptr);

	*stepnum = 1;
	lwobuf.pos += 1;//!!!

	return 0;
}
int CLWOfile::ReadI2( int* iptr, int offset, int* stepnum )
{
	if( (lwobuf.pos + offset + 2) > lwobuf.bufleng ){
		DbgOut( "LWOfile : ReadI2: pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	short int* tempsptr;
	tempsptr = (short int*)(lwobuf.buf + lwobuf.pos + offset);

	*iptr = (int)(N2HS( *tempsptr ));

	*stepnum = 2;
	lwobuf.pos += 2;//!!!

	return 0;
}
int CLWOfile::ReadI4( int* iptr, int offset, int* stepnum )
{
	if( (lwobuf.pos + offset + 4) > lwobuf.bufleng ){
		DbgOut( "LWOfile : ReadI4: pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int* tempiptr;
	tempiptr = (int*)(lwobuf.buf + lwobuf.pos + offset);

	*iptr = N2HI( *tempiptr );

	*stepnum = 4;
	lwobuf.pos += 4;//!!!

	return 0;
}

int CLWOfile::ReadU1( unsigned int* uiptr, int offset, int* stepnum )
{
	if( (lwobuf.pos + offset + 1) > lwobuf.bufleng ){
		DbgOut( "LWOfile : ReadU1: pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	unsigned char* tempucptr;
	tempucptr = (unsigned char*)(lwobuf.buf + lwobuf.pos + offset);

	*uiptr = (int)(*tempucptr);

	*stepnum = 1;
	lwobuf.pos += 1;//!!!

	return 0;
}
int CLWOfile::ReadU2( unsigned int* uiptr, int offset, int* stepnum, int notposflag )
{
	if( (lwobuf.pos + offset + 2) > lwobuf.bufleng ){
		DbgOut( "LWOfile : ReadU2: pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	unsigned short* tempusptr;
	tempusptr = (unsigned short*)(lwobuf.buf + lwobuf.pos + offset);

	*uiptr = (unsigned int)(N2HUI( *tempusptr ));

	*stepnum = 2;

	if( notposflag == 0 )
		lwobuf.pos += 2;//!!!

	return 0;
}

int CLWOfile::ReadU4( unsigned int* uiptr, int offset, int* stepnum )
{
	if( (lwobuf.pos + offset + 4) > lwobuf.bufleng ){
		DbgOut( "LWOfile : ReadU4: pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	unsigned int* tempuiptr;
	tempuiptr = (unsigned int*)(lwobuf.buf + lwobuf.pos + offset);

	*uiptr = N2HUI( *tempuiptr );

	*stepnum = 4;
	lwobuf.pos += 4;//!!!

	return 0;
}

int CLWOfile::ReadVX( unsigned int* uiptr, int offset, int* stepnum )
{
	int ret;
	unsigned char topuc;
	topuc = *( lwobuf.buf + lwobuf.pos + offset );

	if( topuc != 0xFF ){
		// U2 データ
		ret = ReadU2( uiptr, offset, stepnum );
		if( ret ){
			DbgOut( "LWOfile : ReadVX : ReadU2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		// U4 データ（0xFF + 3byte data）
		ret = ReadU4( uiptr, offset, stepnum );
		if( ret ){
			DbgOut( "LWOfile : ReadVX : ReadU4 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		*uiptr &= 0x00FFFFFF;//!!!!!
	}

	return 0;
}


int CLWOfile::ReadF4( float* flptr, int offset, int* stepnum )
{
	if( (lwobuf.pos + offset + 4) > lwobuf.bufleng ){
		DbgOut( "LWOfile : ReadF4: pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	float* tempflptr;
	tempflptr = (float*)(lwobuf.buf + lwobuf.pos + offset);

	*flptr = N2HF( *tempflptr );

	*stepnum = 4;
	lwobuf.pos += 4;//!!!

	return 0;
}

int CLWOfile::ReadS0( char* cptr, int dstleng, int offset, int* stepnum )
{
	int sleng;
	int oddplus = 0;
	sleng = (int)strlen( (char*)(lwobuf.buf + lwobuf.pos + offset) );

	if( (sleng + 1) > dstleng ){
		DbgOut( "LWOfile : ReadS0 : dst leng too short error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( ((sleng + 1) % 2) != 0 ){
		oddplus = 1;
	}

	if( (lwobuf.pos + offset + sleng + 1 + oddplus ) > lwobuf.bufleng ){
		DbgOut( "LWOfile : ReadS0: pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( *(lwobuf.buf + lwobuf.pos + offset + sleng + 1 + oddplus ) != 0 ){
		DbgOut( "LWOfile : ReadS0 : not found NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sleng > 0 ){
		strncpy( cptr, (char*)( lwobuf.buf + lwobuf.pos + offset ), sleng );
	}
	*( cptr + sleng ) = 0;


	*stepnum = sleng + oddplus + 1;//!!!!
	lwobuf.pos += sleng + oddplus + 1;

	return 0;
}
int CLWOfile::ReadVEC12( float* flptr1, float* flptr2, float* flptr3, int offset, int* stepnum )
{
	int ret;
	int tempstep1, tempstep2, tempstep3;

	ret = ReadF4( flptr1, offset, &tempstep1 );
	if( ret ){
		DbgOut( "LWOfile : ReadVEC12 : ReadF4 : 1 : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadF4( flptr2, offset, &tempstep2 );
	if( ret ){
		DbgOut( "LWOfile : ReadVEC12 : ReadF4 : 2 : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadF4( flptr3, offset, &tempstep3 );
	if( ret ){
		DbgOut( "LWOfile : ReadVEC12 : ReadF4 : 3 : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (tempstep1 + tempstep2 + tempstep3) != 12 ){
		DbgOut( "LWOfile : ReadVec12 : tempstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*stepnum = 12;//!!!

	// lwobuf.pos は、ReadF4内で足されているので、いじらない。

	return 0;
}

int CLWOfile::CheckVMAPdimension( char* srcid4, int srcdim )
{
	char vmapstr[8][5] = {
		"PICK",//0
		"WGHT",//1
		"MNVW",//2
		"TXUV",//3
		"RGB ",//4
		"RGBA",//5
		"MORF",//6
		"SPOT"//7
	};

	int findtype = -1;
	int patno;
	for( patno = 0; patno < 8; patno++ ){
		if( strcmp( srcid4, vmapstr[patno] ) == 0 ){
			findtype = patno;
			break;
		}
	}

	if( patno < 0 ){
		DbgOut( "LWOfile : CheckVMAPdimension : id4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkflag = 0;
	switch( patno ){
	case 0://PICK
		if( srcdim != 0 )
			chkflag = 1;
		break;
	case 1://WGHT
		if( srcdim != 1 )
			chkflag = 1;
		break;
	case 3://TXUV
		if( srcdim != 2 )
			chkflag = 1;
		break;
	case 4://RGB 
		if( srcdim != 3 )
			chkflag = 1;
	case 5://RGBA
		if( srcdim != 4 )
			chkflag = 1;

	case 2://MNVW
	case 6://MORF
	case 7://SPOT
	default:
		break;
	}

	if( chkflag != 0 ){
		DbgOut( "LWOfile : CheckVMAPdimension : dimension error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOfile::CheckVMAPDataNum( unsigned int dataleng, int srcdim, int* numptr )
{
	unsigned int datapos = 0;	
	int datacnt = 0;

	while( datapos < dataleng ){
		unsigned char topuc;
		topuc = *( lwobuf.buf + lwobuf.pos + datapos );
		
		if( topuc != 0xFF ){
			datapos += 2;
		}else{
			datapos += 4;
		}

		datapos += (srcdim * 4);

		datacnt++;
	}

	if( datapos != dataleng ){
		DbgOut( "LWOfile : CheckVMAPDataNum : datapos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = datacnt;

	return 0;
}

int CLWOfile::CheckVMADDataNum( unsigned int dataleng, int srcdim, int* numptr )
{
	unsigned int datapos = 0;	
	int datacnt = 0;

	while( datapos < dataleng ){
		unsigned char topuc;
		topuc = *( lwobuf.buf + lwobuf.pos + datapos );
		
		if( topuc != 0xFF ){
			datapos += 2;
		}else{
			datapos += 4;
		}


		topuc = *( lwobuf.buf + lwobuf.pos + datapos );
		
		if( topuc != 0xFF ){
			datapos += 2;
		}else{
			datapos += 4;
		}


		datapos += (srcdim * 4);

		datacnt++;
	}

	if( datapos != dataleng ){
		DbgOut( "LWOfile : CheckVMADDataNum : datapos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = datacnt;

	return 0;
}

int CLWOfile::CheckPOLSDataNum( unsigned int dataleng, int* numptr )
{
	int ret;
	unsigned int datapos = 0;	
	int datacnt = 0;

	while( datapos < dataleng ){

		int stepnum;
		unsigned int vertnum;
		ret = ReadU2( &vertnum, 0, &stepnum, 1 );//最後の引数で、lwobuf.posの変更をキャンセル！！！
		if( ret ){
			DbgOut( "LWOfile : CheckPOLSDataNum : ReadU2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		vertnum &= 0x03FF;//!!!!!
		datapos += 2;
		

		unsigned int vertno;
		for( vertno = 0; vertno < vertnum; vertno++ ){
			unsigned char vxuc;
			vxuc = *( lwobuf.buf + lwobuf.pos + datapos );
			if( vxuc != 0xFF ){
				datapos += 2;
			}else{
				datapos += 4;
			}
		}
		
		datacnt++;
	}

	if( datapos != dataleng ){
		DbgOut( "LWOfile : CheckPOLSDataNum : datapos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = datacnt;

	return 0;
}

int CLWOfile::CheckPTAGDataNum( unsigned int dataleng, int* numptr )
{

	unsigned int datapos = 0;	
	int datacnt = 0;

	while( datapos < dataleng ){
		unsigned char vxuc;
		vxuc = *( lwobuf.buf + lwobuf.pos + datapos );
		if( vxuc != 0xFF ){
			datapos += 2;
		}else{
			datapos += 4;
		}

		datapos += 2;

		datacnt++;
	}

	if( datapos != dataleng ){
		DbgOut( "LWOfile : CheckPOLSDataNum : datapos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = datacnt;

	return 0;
}


int CLWOfile::DumpLWOObjects()
{
	int ret = 0;

	m_dumpfile = CreateFile( (LPCTSTR)"lwodump.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_dumpfile == INVALID_HANDLE_VALUE ){
		return 1;
	}

	ret = DumpTAGS();
	if( ret ){
		DbgOut( "LWOfile : DumpLWOObjects : DumpTAGS error !!!\n" );
		_ASSERT( 0 );
		goto lwodumpexit;
	}

	ret = DumpCLIP();
	if( ret ){
		DbgOut( "LWOfile : DumpLWOObjects : DumpCLIP error !!!\n" );
		_ASSERT( 0 );
		goto lwodumpexit;
	}

	ret = DumpLAYR();
	if( ret ){
		DbgOut( "LWOfile : DumpLWOObjects : DumpLAYR error !!!\n" );
		_ASSERT( 0 );
		goto lwodumpexit;
	}

	ret = DumpSURF();
	if( ret ){
		DbgOut( "LWOfile : DumpLWOObjects : DumpSURF error !!!\n" );
		_ASSERT( 0 );
		goto lwodumpexit;
	}


	goto lwodumpexit;
lwodumpexit:
	if( m_dumpfile ){
		FlushFileBuffers( m_dumpfile );
		SetEndOfFile( m_dumpfile );
		CloseHandle( m_dumpfile );
		m_dumpfile = INVALID_HANDLE_VALUE;
	}
	return ret;
}

int CLWOfile::DumpTAGS()
{
	DumpLine( "\n\nstart TAGS dump\n" );

	int tagsno;
	for( tagsno = 0; tagsno < tagsnum; tagsno++ ){
		CLWOtags* tagsptr;
		tagsptr = *( pptags + tagsno );

		DumpLine( "TAGS index %d : name %s\n", tagsptr->index, tagsptr->nameptr );
	}

	return 0;
}
int CLWOfile::DumpCLIP()
{
	DumpLine( "\n\nstart CLIP dump\n" );

	int clipno;
	for( clipno = 0; clipno < clipnum; clipno++ ){
		CLWOclip* clipptr;
		clipptr = *( ppclip + clipno );

		DumpLine( "CLIP %d\n", clipno );
		DumpLine( "\tindex %d\n", clipptr->index );
		DumpLine( "\ttype %s\n", clipptr->typestr );
		DumpLine( "\tstil name %s\n", clipptr->nameptr );
		DumpLine( "\tnumdigits %d\n", clipptr->numdigits );
		DumpLine( "\tflag %d, offset %d, reserved %d\n", clipptr->flag, clipptr->offset, clipptr->reserved );
		DumpLine( "\tstart %d, end %d\n", clipptr->start, clipptr->end );
		DumpLine( "\tprefix %s\n", clipptr->prefixptr );
		DumpLine( "\tsuffix %s\n", clipptr->suffixptr );
		DumpLine( "\txrefindex %d\n", clipptr->xrefindex );
		DumpLine( "\n" );
	}

	return 0;
}
int CLWOfile::DumpLAYR()
{

	DumpLine( "\n\nstart LAYR dump\n" );

	int layrno;
	for( layrno = 0; layrno < layrnum; layrno++ ){
		CLWOlayr* layrptr;
		layrptr = *( pplayr + layrno );
		
		DumpLine( "LAYR %d\n", layrno );

		DumpLine( "\tnumber %d\n", layrptr->number );
		DumpLine( "\tname %s\n", layrptr->name );
		DumpLine( "\tparent %d\n", layrptr->parent );
		DumpLine( "\tflag %d\n", layrptr->flag );
		DumpLine( "\tpivot %f %f %f\n", layrptr->pivot.x, layrptr->pivot.y, layrptr->pivot.z );
		DumpLine( "\tBBOX min %f %f %f\n", layrptr->BBOXmin.x, layrptr->BBOXmin.y, layrptr->BBOXmin.z );
		DumpLine( "\tBBOX max %f %f %f\n", layrptr->BBOXmax.x, layrptr->BBOXmax.y, layrptr->BBOXmax.z );

		DumpLine( "\tPNTS num %d\n", layrptr->pntsnum );
		int pntsno;
		for( pntsno = 0; pntsno < layrptr->pntsnum; pntsno++ ){
			D3DXVECTOR3* pntsvec;
			pntsvec = layrptr->PNTS + pntsno;

			DumpLine( "\t\t%d : %f %f %f\n", pntsno, pntsvec->x, pntsvec->y, pntsvec->z );
		}
		
		DumpVMAP( layrptr );
		DumpVMAD( layrptr );
		DumpPOLS( layrptr );
		DumpPTAG( layrptr );

		DumpLine( "\n" );
		
	}

	return 0;
}
int CLWOfile::DumpSURF()
{
	DumpLine( "\n\nstart SURF dump\n" );

	int surfno;
	for( surfno = 0; surfno < surfnum; surfno++ ){
		CLWOsurf* surfptr;
		surfptr = *( ppsurf + surfno );

		DumpLine( "SURF %d\n", surfno );

		DumpLine( "\tname %s\n", surfptr->name );
		DumpLine( "\tsource %s\n", surfptr->source );
		DumpLine( "\tcolor %f %f %f\n", surfptr->color.x, surfptr->color.y, surfptr->color.z );
		DumpLine( "\tdiff %f, spec %f, tran %f\n", surfptr->diff, surfptr->spec, surfptr->tran );
		DumpLine( "\tside %d\n", surfptr->side );
		DumpLine( "\tvcoltype %s, vcolname %s\n", surfptr->vcoltypestr, surfptr->vcolname );

		DumpLine( "\tBLOCK num %d\n", surfptr->sbnum );
		
		DumpSURFBLOK( surfptr );

		DumpLine( "\n" );
	
	}

	return 0;
}

int CLWOfile::DumpVMAP( CLWOlayr* layrptr )
{
	int vmapno;
	int vmapnum;


	vmapnum = layrptr->vmapnum;
	DumpLine( "\tVMAP num %d\n", vmapnum );

	
	for( vmapno = 0; vmapno < vmapnum; vmapno++ ){
		CLWOvmap* vmapptr;
		vmapptr = *( layrptr->ppvmap + vmapno );

		DumpLine( "\t\tVMAP no %d\n", vmapno );

		DumpLine( "\t\tindex %d, name %s\n", vmapptr->index, vmapptr->nameptr );
		DumpLine( "\t\ttype %s\n", vmapptr->typestr );
		DumpLine( "\t\tdimension %d\n", vmapptr->dimension );

		DumpLine( "\t\tdatanum %d\n", vmapptr->datanum );
		int datano;
		for( datano = 0; datano < vmapptr->datanum; datano++ ){
			CLWOvmapelem* elemptr;
			elemptr = vmapptr->data + datano;
			
			DumpLine( "\t\t\tvmapelem dimention %d, vertindex %d\n", elemptr->dimension, elemptr->vertindex );

			int dimno;
			for( dimno = 0; dimno < elemptr->dimension; dimno++ ){
				DumpLine( "\t\t\t\tdimno %d, value %f\n", dimno, *( elemptr->valueptr + dimno ) );
			}
		}

	}

	return 0;
}
int CLWOfile::DumpVMAD( CLWOlayr* layrptr )
{
	int vmadno;
	int vmadnum;

	vmadnum = layrptr->vmadnum;
	DumpLine( "\tVMAD num %d\n", vmadnum );

	for( vmadno = 0; vmadno < vmadnum; vmadno++ ){
		CLWOvmad* vmadptr;
		vmadptr = *( layrptr->ppvmad + vmadno );

		DumpLine( "\t\tVMAD no %d\n", vmadno );

		DumpLine( "\t\tindex %d, name %s\n", vmadptr->index, vmadptr->nameptr );
		DumpLine( "\t\ttype %s\n", vmadptr->typestr );
		DumpLine( "\t\tdimension %d\n", vmadptr->dimension );

		DumpLine( "\t\tdatanum %d\n", vmadptr->datanum );

		int datano;
		for( datano = 0; datano < vmadptr->datanum; datano++ ){
			CLWOvmadelem* elemptr;
			elemptr = vmadptr->data + datano;
			
			DumpLine( "\t\t\tvmadelem dimention %d, vertindex %d, polyindex %d\n", elemptr->dimension, elemptr->vertindex, elemptr->polyindex );

			int dimno;
			for( dimno = 0; dimno < elemptr->dimension; dimno++ ){
				DumpLine( "\t\t\t\tdimno %d, value %f\n", dimno, *( elemptr->valueptr + dimno ) );
			}
		}
	}


	return 0;
}
int CLWOfile::DumpPOLS( CLWOlayr* layrptr )
{
	int polsno;
	int polsnum;

	polsnum = layrptr->polsnum;
	DumpLine( "\tPOLS num %d\n", polsnum );

	for( polsno = 0; polsno < polsnum; polsno++ ){
		CLWOpols* polsptr;
		polsptr = *( layrptr->pppols + polsno );

		DumpLine( "\t\tPOLS no %d\n", polsno );

		DumpLine( "\t\ttype %s\n", polsptr->typestr );

		DumpLine( "\t\tdatanum %d\n", polsptr->datanum );

		int datano;
		for( datano = 0; datano < polsptr->datanum; datano++ ){
			CLWOpolselem* elemptr;
			elemptr = polsptr->data + datano;

			DumpLine( "\t\t\tpolselem vertnum %d\n", elemptr->vertnum );

			int vertno;
			for( vertno = 0; vertno < elemptr->vertnum; vertno++ ){
				DumpLine( "\t\t\t\tvertno %d, vert %d\n", vertno, *( elemptr->vertptr + vertno ) );
			}
		}
	}

	return 0;
}
int CLWOfile::DumpPTAG( CLWOlayr* layrptr )
{
	int ptagno;
	int ptagnum;

	ptagnum = layrptr->ptagnum;
	DumpLine( "\tPTAG num %d\n", ptagnum );

	for( ptagno = 0; ptagno < ptagnum; ptagno++ ){
		CLWOptag* ptagptr;
		ptagptr = *( layrptr->ppptag + ptagno );

		DumpLine( "\t\tPTAG no %d\n", ptagno );

		DumpLine( "\t\ttype %s\n", ptagptr->typestr );

		DumpLine( "\t\tdatanum %d\n", ptagptr->datanum );

		int datano;
		for( datano = 0; datano < ptagptr->datanum; datano++ ){
			LWOPTAG* elemptr;
			elemptr = ptagptr->dataptr + datano;

			DumpLine( "\t\t\tpolyindex %d, tag %d\n", elemptr->polyindex, elemptr->tag );

		}
	}

	return 0;
}

int CLWOfile::DumpSURFBLOK( CLWOsurf* surfptr )
{
	int sbno;
	int sbnum;

	sbnum = surfptr->sbnum;
	DumpLine( "\tSURFBLOK num %d\n", sbnum );

	for( sbno = 0; sbno < sbnum; sbno++ ){
		CLWOsurfblok* sbptr;
		sbptr = *( surfptr->ppsb + sbno );

		DumpLine( "\t\tSURFBLOK no %d\n", sbno );

		DumpLine( "\t\tim_order %s\n", sbptr->im_orderstr );
		DumpLine( "\t\tim_chan %s\n", sbptr->im_chan );
		DumpLine( "\t\tim_enab %d\n", sbptr->im_enab );

		DumpLine( "\t\ttm_cntr %f %f %f\n", sbptr->tm_cntr.x, sbptr->tm_cntr.y, sbptr->tm_cntr.z );
		DumpLine( "\t\ttm_size %f %f %f\n", sbptr->tm_size.x, sbptr->tm_size.y, sbptr->tm_size.z );
		DumpLine( "\t\ttm_rota %f %f %f\n", sbptr->tm_rota.x, sbptr->tm_rota.y, sbptr->tm_rota.z );
		DumpLine( "\t\ttm_oref %s\n", sbptr->tm_oref );
		DumpLine( "\t\ttm_csys %d\n", sbptr->tm_csys );

		DumpLine( "\t\tproj %d\n", sbptr->proj );
		DumpLine( "\t\taxis %d\n", sbptr->axis );
		DumpLine( "\t\timag %d\n", sbptr->imag );
		DumpLine( "\t\twidth_wrap %d\n", sbptr->width_wrap );
		DumpLine( "\t\theight_wrap %d\n", sbptr->width_wrap );
		DumpLine( "\t\twrpw %f, wrph %f\n", sbptr->wrpw, sbptr->wrph );
		DumpLine( "\t\tvmapname %s\n", sbptr->vmapname );
		
	}

	return 0;
}

int CLWOfile::DumpLine( char* lpFormat, ... )
{
	if( !m_dumpfile ){
		//_ASSERT( 0 );
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[2048];
			
	ZeroMemory( outchar, 2048 );

	va_start( Marker, lpFormat );
	ret = vsprintf( outchar, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 )
		return 1;

	wleng = (unsigned long)strlen( outchar );
	WriteFile( m_dumpfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		return 1;
	}

	return 0;
}
