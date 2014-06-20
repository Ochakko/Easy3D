#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <D3DX9.h>
#include <coef.h>

#define DBGH
#include <dbg.h>

#include <treehandler2.h>
#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>
#include <d3ddisp.h>
#include <polymesh2.h>
#include <namebank.h>

#include <infelem.h>
#include <shdelem.h>

#include "IM2File.h"
#include <crtdbg.h>

static char fileheaderstr[256] = "InfluenceMapFile2 : ver1001\r\n";
static char fileheaderstr2[256] = "InfluenceMapFile2 : ver1002\r\n";
static char fileheaderstr3[256] = "InfluenceMapFile2 : ver1003\r\n";
static char fileheaderstr4[256] = "InfluenceMapFile2 : ver1004\r\n";

static char partstartstr[256] = "PARTSTART\r\n";
static char partendstr[256] = "PARTEND\r\n";

//static char skinmodestr0[256] = "SKINNING_MODE\r\n";
//static char skinmodestr1[256] = "NO_SKINNING_MODE\r\n";

static char calcstr0[256] = "CALC_NOSKIN0";
static char calcstr1[256] = "CALC_ONESKIN0";

static char im2endmark[256] = "!!!im2file end mark!!!";


CIM2File::CIM2File()
{

	InitParams();

}
CIM2File::~CIM2File()
{
	if( im2buf.buf ){
		free( im2buf.buf );
		im2buf.buf = 0;
	}

}

int CIM2File::InitParams()
{
	m_hfile = INVALID_HANDLE_VALUE;
	m_lpth = 0;
	m_lpsh = 0;
	m_lpmh = 0;

	im2buf.buf = 0;
	im2buf.bufleng = 0;
	im2buf.pos = 0;
	im2buf.isend = 0;

	IM2ELEMInit( 1 );

	m_firstseri = 0;

	m_calcmode = CALCMODE_ONESKIN0;
	m_formqo = 0;

	m_invalidnameflag = 0;

	return 0;
}

int CIM2File::IM2ELEMInit( int inimode )
{
	if( inimode == 1 ){
		curim2elem.partname[0] = 0;
		curim2elem.serialno = -1;
	}

	curim2elem.vertno = -1;
	curim2elem.boneno1 = -1;
	curim2elem.boneno2 = -1;
	curim2elem.calcmode = CALCMODE_ONESKIN0;

	return 0;
}

int CIM2File::WriteIM2File( char* filename, int srcformqo, CTreeHandler2* lpth, CShdHandler* lpsh )
{
	int ret = 0;

	if( !lpth || !lpsh ){
		DbgOut( "IM2File : WriteIM2File : handler NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_lpth = lpth;
	m_lpsh = lpsh;

	m_formqo = srcformqo;

	//////////////////

	//ボーンが1個も無いときは、出力しない。
	int bonenum = 0;
	ret = lpsh->CheckBoneNum( &bonenum );
	if( ret ){
		DbgOut( "im2file : WriteIM2File : sh CheckBoneNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( bonenum == 0 ){
		DbgOut( "im2file : WriteIM2File : bonenum == 0 : return 0 !!!\n" );

		MessageBox( NULL, "階層構造を持ったジョイントが一つもありません。\nジョイントに階層構造を作ってから、再試行してください。", "階層構造がありません", MB_OK );

		//_ASSERT( 0 );
		return 0;
	}

	///////////////////

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "IM2File : WriteIM2File : file open error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto im2fexit;
	}

	int sameno;
	sameno = CheckSameName();
	if( sameno >= 0 ){
		DbgOut( "im2file : WriteIM2File : CheckSameName error !!!\n" );
		_ASSERT( 0 );

		char messtr1[1024];
		strcpy_s( messtr1, 1024, "同じオブジェクト名が複数あると、\n正しく、ファイルを作れません。\n名前を変えてから、再試行してください。\n" );

		CTreeElem2* sametelem;
		sametelem = (*m_lpth)( sameno );
		_ASSERT( sametelem );

		::MessageBox( NULL, messtr1, sametelem->name, MB_OK );

		ret = 1;
		goto im2fexit;
	}


	Write2File( fileheaderstr4 );


	int serino;
	CShdElem* selem;
	CTreeElem2* telem;
	//int dispflag, isbone;
	for( serino = 0; serino < m_lpsh->s2shd_leng; serino++ ){

		selem = (*m_lpsh)( serino );
		telem = (*m_lpth)( serino );

		if( !selem || !telem ){
			DbgOut( "IM2File : WriteIM2File : elem error %x %x !!!\n", selem, telem );
			_ASSERT( 0 );
			ret = 1;
			goto im2fexit;
		}		

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = WriteIM2( selem, telem );
			if( ret ){
				DbgOut( "IM2File : WriteIM2File : WriteIM2 error %d!!!\n", serino );
				ret = 1;
				goto im2fexit;
			}
		}
	}

	//終わりの目印
	IM2HEADER im2h;
	ZeroMemory( &im2h, sizeof( IM2HEADER ) );
	strcpy_s( im2h.name, 256, im2endmark );
	im2h.vertnum = 0;
	ret = WriteUCharData( (unsigned char*)&im2h, sizeof( IM2HEADER ) );
	if( ret ){
		DbgOut( "im2file : WriteIM2File : endmark write error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto im2fexit;
	}


	goto im2fexit;
im2fexit:
	if( m_hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hfile );
		SetEndOfFile( m_hfile );
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	return ret;
}
int CIM2File::GetFirstJointNo( int* firstno )
{
	//boneの一番若いserial番号を、取得する。
	*firstno = 0;

	int maxno = (int)1e8;
	int minno = maxno;
	int serino;
	for( serino = 0; serino < m_lpsh->s2shd_leng; serino++ ){
		CShdElem* selem;
		selem = (*m_lpsh)( serino );
		int isjoint;
		isjoint = selem->IsJoint();
		if( isjoint ){
			if( selem->serialno < minno ){
				minno = selem->serialno;
			}
		}
	}

	if( minno != maxno ){
		*firstno = minno;
	}else{
		*firstno = 0;
	}

	return 0;
}

int CIM2File::Write2File( char* lpFormat, ... )
{
	if( !m_hfile ){
		//_ASSERT( 0 );
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[IM2LINELEN];
			
	ZeroMemory( outchar, IM2LINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, IM2LINELEN, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 )
		return 1;

	wleng = (unsigned long)strlen( outchar );
	WriteFile( m_hfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		return 1;
	}

	return 0;
}

int CIM2File::WriteIM2( CShdElem* selem, CTreeElem2* telem )
{
	int ret;
	
	switch( selem->type ){
	case SHDPOLYMESH:
		ret = WritePolyMeshIM2( selem, telem->name );
		if( ret ){
			DbgOut( "IM2file : WriteIM2 : WritePolyMeshIM2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SHDPOLYMESH2:
		ret = WritePolyMesh2IM2( selem, telem->name );
		if( ret ){
			DbgOut( "IM2file : WriteIM2 : WritePolyMeshIM22 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		break;
	}

	return 0;
}

int CIM2File::WritePolyMeshIM2( CShdElem* selem, char* name )
{
	int ret;
	IM2HEADER im2h;
	ZeroMemory( &im2h, sizeof( IM2HEADER ) );
	
	if( !name ){
		DbgOut( "im2file : WritePolyMeshIM2 : name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int leng;
	leng = (int)strlen( name );
	if( (leng <= 0) || (leng >= 256) ){
		DbgOut( "im2file : WritePolyMeshIM2 : name leng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( im2h.name, 256, name );

	_ASSERT( selem->polymesh );
	im2h.vertnum = selem->polymesh->meshinfo->m;

	ret = WriteUCharData( (unsigned char*)&im2h, sizeof( IM2HEADER ) );
	if( ret ){
		DbgOut( "im2file : WritePolyMeshIM2 : im2header write error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	_ASSERT( selem->polymesh->m_IE );
	INFELEMHEADER ieh;
//	SIGINFELEM sie;
	INFIM2 infim2;
	int vno;
	CInfElem* curie;
	int infno, infnum;
	INFELEM* pIE;
	for( vno = 0; vno < im2h.vertnum; vno++ ){
		curie = selem->polymesh->m_IE + vno;
		infnum = curie->infnum;

		ZeroMemory( &ieh, sizeof( INFELEMHEADER ) );
		ieh.infnum = infnum;
		ieh.normalizeflag = curie->normalizeflag;

		ret = WriteUCharData( (unsigned char*)&ieh, sizeof( INFELEMHEADER ) );
		if( ret ){
			DbgOut( "im2file : WritePolyMeshIM2 : ieh write error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		for( infno = 0; infno < infnum; infno++ ){
			ZeroMemory( &infim2, sizeof( INFIM2 ) );

			ret = m_lpsh->GetInfElem( selem->serialno, vno, infno, &pIE );
			if( ret || !pIE ){
				DbgOut( "im2file : WritePolyMeshIM2 : sh GetInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( pIE->childno <= 0 ){
				_ASSERT( 0 );
				return 1;
			}
			CTreeElem2* te = (*m_lpth)( pIE->childno );
			_ASSERT( te );
			strcpy_s( infim2.bonename, 256, te->name );
			infim2.kind = pIE->kind;
			infim2.userrate = pIE->userrate;
			infim2.orginf = pIE->orginf;
			infim2.dispinf = pIE->dispinf;

			ret = WriteUCharData( (unsigned char*)&infim2, sizeof( INFIM2 ) );
			if( ret ){
				DbgOut( "im2file : WritePolyMeshIM2 : infim2 write error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CIM2File::WritePolyMesh2IM2( CShdElem* selem, char* name )
{
	int ret;
	IM2HEADER im2h;
	ZeroMemory( &im2h, sizeof( IM2HEADER ) );
	
	if( !name ){
		DbgOut( "im2file : WritePolyMesh2IM2 : name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int leng;
	leng = (int)strlen( name );
	if( (leng <= 0) || (leng >= 256) ){
		DbgOut( "im2file : WritePolyMesh2IM2 : name leng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( im2h.name, 256, name );

	_ASSERT( selem->polymesh2 );
	im2h.vertnum = selem->polymesh2->optpleng;

	ret = WriteUCharData( (unsigned char*)&im2h, sizeof( IM2HEADER ) );
	if( ret ){
		DbgOut( "im2file : WritePolyMesh2IM2 : im2header write error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	_ASSERT( selem->polymesh2->m_IE );
	INFELEMHEADER ieh;
	//SIGINFELEM sie;
	INFIM2 infim2;
	int vno;
	CInfElem* curie;
	int infno, infnum;
	INFELEM* pIE;
	for( vno = 0; vno < im2h.vertnum; vno++ ){
		curie = selem->polymesh2->m_IE + vno;
		infnum = curie->infnum;

		ZeroMemory( &ieh, sizeof( INFELEMHEADER ) );
		ieh.infnum = infnum;
		ieh.normalizeflag = curie->normalizeflag;

		ret = WriteUCharData( (unsigned char*)&ieh, sizeof( INFELEMHEADER ) );
		if( ret ){
			DbgOut( "im2file : WritePolyMesh2IM2 : ieh write error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		for( infno = 0; infno < infnum; infno++ ){
			ZeroMemory( &infim2, sizeof( INFIM2 ) );

			ret = m_lpsh->GetInfElem( selem->serialno, vno, infno, &pIE );
			if( ret || !pIE ){
				DbgOut( "im2file : WritePolyMesh2IM2 : sh GetInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( pIE->childno <= 0 ){
				_ASSERT( 0 );
				return 1;
			}
			CTreeElem2* te = (*m_lpth)( pIE->childno );
			_ASSERT( te );
			strcpy_s( infim2.bonename, 256, te->name );
			infim2.kind = pIE->kind;
			infim2.userrate = pIE->userrate;
			infim2.orginf = pIE->orginf;
			infim2.dispinf = pIE->dispinf;

			ret = WriteUCharData( (unsigned char*)&infim2, sizeof( INFIM2 ) );
			if( ret ){
				DbgOut( "im2file : WritePolyMesh2IM2 : sie write error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}
int CIM2File::LoadIM2File( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh )
{

	int ret = 0;

	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;


	////////////////////////////////////


	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "IM2File : LoadIM2File : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
	}	

	ret = InitIM2BUF();
	if( ret ){
		DbgOut( "IM2File : LoadIM2File : InitIM2BUF error !!!\n" );
		_ASSERT( 0 );
		goto ldim2exit;
	}
	
	m_version = CheckFileHeader();
	if( m_version == 0 ){
		DbgOut( "IM2File : LoadIM2File : CheckFileHeader error !!!\n" );
		_ASSERT( 0 );
		goto ldim2exit;
	}

	////////////////////////////////////

	if( (m_version == 1) && (lpsh->m_im2enableflag == 0) ){
		DbgOut( "im2file : LoadIM2File : not support error !!!\n" );
		DbgOut( "see : http://www5d.biglobe.ne.jp/~ochikko/rdb2_im2file.htm !!!\n" );
		_ASSERT( 0 );
		goto ldim2exit;
	}

	////////////////////////////////////
	if( (m_version == 2) || (m_version == 3) || (m_version == 4) ){
		int ldleng;
		LoadLine( &ldleng );//fileheader読み込み

		ret = LoadIM2Binary();
		if( ret ){
			DbgOut( "im2file : LoadIM2File : LoadIM2Binary error !!!\n" );
			_ASSERT( 0 );
		}

		goto ldim2exit;

	}


////////////////////////	
	///////  以下、旧バージョン　
//////////
	ret = GetFirstJointNo( &m_firstseri );//InitIM2BUFより後で。m_firstseri
	if( ret ){
		DbgOut( "IM2File : LoadIM2File : GetFirstJointNo error !!!\n" );
		_ASSERT( 0 );
		goto ldim2exit;
	}
	//IM2_NUTRAL,
	//IM2_LOADNAME,
	//IM2_LOADIM2,
	//IM2_FINISH,

	int loadmode;
	int loadflag;

	loadmode = IM2_NUTRAL;
	loadflag = 1;
	while( loadflag ){
		switch( loadmode ){
		case IM2_NUTRAL:
			ret = GetLoadMode( &loadmode );
			if( ret ){
				DbgOut( "IM2File : LoadIM2File : GetLoadMode error !!!\n" );
				_ASSERT( 0 );
				loadflag = 0;
			}
			break;

		case IM2_FINISH:
			loadflag = 0;
			break;
		
		case IM2_LOADNAME:
			ret = LoadPartName( &loadmode );
			if( ret ){
				DbgOut( "IM2File : LoadIM2File : LoadPartName error !!!\n" );
				_ASSERT( 0 );
				goto ldim2exit;
			}
			break;

		case IM2_LOADIM2:
			ret = LoadIM2( &loadmode );
			if( ret ){
				DbgOut( "IM2File : LoadIM2File : LoadIM2 error !!!\n" );
				_ASSERT( 0 );
				goto ldim2exit;
			}
			break;
		default:
			DbgOut( "IM2File : LoadIM2File : loadmode error !!!\n" );
			_ASSERT( 0 );
			loadflag = 0;
			break;
		}
	}

	ret = lpsh->CreateSkinMat( -1, 1 );
	if( ret ){
		DbgOut( "IM2File : LoadIM2File : sh CreateSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	goto ldim2exit;
ldim2exit:

	// selem m_loadbimflagの処理
	if( ret == 0 ){
		//本セット
		int seri;
		CShdElem* se;
		for( seri = 0; seri < m_lpsh->s2shd_leng; seri++ ){
			se = (*m_lpsh)( seri );
			if( se->m_loadbimflag == 1 )
				se->m_loadbimflag = 2;//!!!!!!!
		}

	}else{
		//セット取り消し
		int seri;
		CShdElem* se;
		for( seri = 0; seri < m_lpsh->s2shd_leng; seri++ ){
			se = (*m_lpsh)( seri );
			if( se->m_loadbimflag == 1 )
				se->m_loadbimflag = 0;//!!!!!!!
		}

	}


	if( m_hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( im2buf.buf ){
		free( im2buf.buf );
		im2buf.buf = 0;

		im2buf.bufleng = 0;
		im2buf.pos = 0;
		im2buf.isend = 0;
	}

	return ret;
}
int CIM2File::InitIM2BUF()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "IM2File : InitIM2BUF :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "IM2File : InitIM2BUF :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "IM2File : InitIM2BUF :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "IM2File : InitIM2BUF :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}
	
	im2buf.buf = newbuf;
	im2buf.bufleng = bufleng;
	im2buf.pos = 0;
	im2buf.isend = 0;

	m_firstseri = 0;

	return 0;
}
int CIM2File::CheckFileHeader()
{
	int cmp1, cmp2, cmp3, cmp4;


	//findptr = strstr( im2buf.buf, fileheaderstr );
	cmp1 = strncmp( im2buf.buf, fileheaderstr, (int)strlen( fileheaderstr ) );
	cmp2 = strncmp( im2buf.buf, fileheaderstr2, (int)strlen( fileheaderstr2 ) );
	cmp3 = strncmp( im2buf.buf, fileheaderstr3, (int)strlen( fileheaderstr3 ) );
	cmp4 = strncmp( im2buf.buf, fileheaderstr4, (int)strlen( fileheaderstr4 ) );
	
	if( cmp4 == 0 ){
		return 4;
	}else if( cmp3 == 0 ){
		return 3;
	}else if( cmp2 == 0 ){
		return 2;
	}else if( cmp1 == 0 ){
		return 1;
	}else{
		return 0;
	}
}

int CIM2File::GetLoadMode( int* modeptr )
{
	int ret;
	int ldleng;

	ret = LoadLine( &ldleng );
	if( ret ){
		DbgOut( "IM2File : GetLoadMode : LoadLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( im2buf.isend != 0 ){
		*modeptr = IM2_FINISH;
		return 0;
	}
	
	int cmp1, cmp2;
	cmp1 = strcmp( m_line, partstartstr );
	if( cmp1 == 0 ){
		*modeptr = IM2_LOADNAME;
		return 0;
	}else{
		cmp2 = strcmp( m_line, partendstr );
		if( cmp2 == 0 ){
			*modeptr = IM2_LOADIM2;
			return 0;
		}else{
			*modeptr = IM2_NUTRAL;
			return 0;
		}
	}

	return 0;
}


int CIM2File::LoadPartName( int* modeptr )
{

	IM2ELEMInit( 1 );


	int ret;
	int ldleng;
	ret = LoadLine( &ldleng );
	if( ret ){
		DbgOut( "IM2File : LoadPartName : LoadLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( im2buf.isend != 0 ){
		*modeptr = IM2_FINISH;
		return 0;
	}

	int cmp;
	cmp = strcmp( m_line, partendstr );
	if( cmp == 0 ){
		*modeptr = IM2_NUTRAL;
		return 0;
	}

	char* endchar;
	endchar = strchr( m_line, '\r' );
	if( endchar ){
		int nameleng;
		nameleng = (int)( endchar - m_line );
		strncpy_s( curim2elem.partname, IM2LINELEN, m_line, nameleng );
		curim2elem.partname[nameleng] = 0;
	}else{
		strcpy_s( curim2elem.partname, IM2LINELEN, m_line );
	}


DbgOut( "im2file : LoadPartName : name %s\n", curim2elem.partname );

/////////
	//ret = m_lpth->GetPartNoByName( curim2elem.partname, &(curim2elem.serialno) );
	ret = m_lpth->GetDispObjNoByName( curim2elem.partname, &(curim2elem.serialno), m_lpsh, 1 );
	if( ret ){
		DbgOut( "IM2file : LoadPartName : lpth GetDispObjNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curim2elem.serialno <= 0 ){
		DbgOut( "IM2file : LoadPartName : invalid partname , serialno not found error %s!!!\n", curim2elem.partname );
		_ASSERT( 0 );
		//return 1;
	
		m_invalidnameflag = 1;//!!!!!!!!!!!!!!!!!!
	}else{
		m_invalidnameflag = 0;
	}

//////////

	*modeptr = IM2_LOADIM2;


	return 0;
}
int CIM2File::LoadIM2( int* modeptr )
{
	int ret;

	while( 1 ){

		IM2ELEMInit( 0 );

		int ldleng;
		ret = LoadLine( &ldleng );
		if( ret ){
			DbgOut( "IM2File : LoadIM2 : LoadLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( im2buf.isend != 0 ){
			*modeptr = IM2_FINISH;
			break;
		}

		int cmp;
		cmp = strcmp( m_line, partendstr );
		if( cmp == 0 ){
			*modeptr = IM2_NUTRAL;
			break;
		}

		if( m_invalidnameflag == 0 ){
			ret = ParseIM2Line();
			if( ret ){
				DbgOut( "IM2File : LoadIM2 : ParseIM2Line error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}



int CIM2File::LoadLine( int* lenptr )
{
	int stepnum = 0;
	unsigned int curpos = im2buf.pos;
	char curc;

	while( curpos < im2buf.bufleng ){
		curc = *( im2buf.buf + curpos );	

		if( curc == '\n' ){
			stepnum++;
			break;
		}
		stepnum++;
		curpos++;
	}

	int ret = 0;
	if( stepnum >= IM2LINELEN ){
		// 行が長すぎ
		DbgOut( "IM2File : LoadLine : stepnum overflow error !!!\n" );
		_ASSERT( 0 );
		m_line[0] = 0;
		ret = 1;
	}else if( stepnum > 0 ){
		// 正常
		strncpy_s( m_line, IM2LINELEN, im2buf.buf + im2buf.pos, stepnum );
		m_line[ stepnum ] = 0;
		ret = 0;
	}else{
		// ファイルの終わり
		m_line[0] = 0;
		im2buf.isend = 1;
		ret = 0;//!!!!!!!
	}

	*lenptr = stepnum;
	im2buf.pos += stepnum;

	return ret;
}

int CIM2File::ParseIM2Line()
{
	int ret;
	int ldleng;
	int pos = 0;

	int lineleng;
	lineleng = (int)strlen( m_line );

	int isend;

	IM2ELEMInit( 0 );//!!!!


// 番号が、 , で区切られている。

	int* dstptrlist[3];
	dstptrlist[0] = &(curim2elem.vertno);
	dstptrlist[1] = &(curim2elem.boneno1);
	dstptrlist[2] = &(curim2elem.boneno2);

	char templine[IM2LINELEN];


	int getno;
	for( getno = 0; getno < 4; getno++ ){
		if( pos >= lineleng ){
			DbgOut( "IM2File : ParseIM2Line : pos overflow error %d!!! return\n", getno );
			_ASSERT( 0 );
			return 1;
		}

		if( getno < 3 ){
			ret = GetIntFromLine( lineleng, pos, &ldleng, &isend, dstptrlist[getno] );
			if( ret ){
				DbgOut( "IM2File : ParseIM2Line : GetIntFromLine error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( isend != 0 ){
				DbgOut( "IM2file : ParseIM2Line : getint %d data is too short error !!!\n", getno );
				_ASSERT( 0 );
				return 1;
			}

		}else{
			templine[0] = 0;
			ret = GetAlnumFromLine( lineleng, pos, &ldleng, &isend, templine );
			if( ret ){
				DbgOut( "IM2File : ParseIM2Line : GetFloatFromLine error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( isend != 0 ){
				DbgOut( "IM2file : ParseIM2Line : get alnum data is too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			//m_calcmodeのセット
			int cmp0, cmp1;
			cmp0 = strcmp( calcstr0, templine );
			cmp1 = strcmp( calcstr1, templine );

			if( cmp0 == 0 ){
				curim2elem.calcmode = CALCMODE_NOSKIN0;
			}else{
				if( cmp1 == 0 ){
					curim2elem.calcmode = CALCMODE_ONESKIN0;
				}else{
					DbgOut( "im2file : ParseIM2Line : invalid calcmode : set default value warning !!!\n" );
					_ASSERT( 0 );

					curim2elem.calcmode = CALCMODE_ONESKIN0;

				}
			}

		}
		if( isend != 0 ){
			break;
		}
		pos += ldleng;

	}


	curim2elem.boneno1 += m_firstseri;
	curim2elem.boneno2 += m_firstseri;


////////// boneelemのセット
	
	CShdElem* selem;
	selem = (*m_lpsh)( curim2elem.serialno );
	if( !selem ){
		DbgOut( "IM2file : ParseIM2Line : selem NULL error %d !!!\n", curim2elem.serialno );
		_ASSERT( 0 );
		return 1;
	}

	if( selem->type == SHDPOLYMESH2 ){
		CPolyMesh2* pm2;
		pm2 = selem->polymesh2;
		if( !pm2 ){
			DbgOut( "IM2File : ParseIM2Line : pm2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		CMeshInfo* miptr;
		miptr = selem->GetMeshInfo();
		if( !miptr ){
			DbgOut( "IM2File : ParseIM2File : meshinfo NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( ((curim2elem.vertno < 0) && (curim2elem.vertno != -1)) || (curim2elem.vertno >= miptr->n * 3) ){
			DbgOut( "IM2File : ParseIM2File : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//if( curim2elem.vertno != -1 ){
		//	curim2elem.vertno = *(pm2->oldpno2optpno + curim2elem.vertno);//!!!!!!!!!!!
		//}

	
		int oldno;
		int orgno;
		int optno;

		int bufleng;
		bufleng = pm2->meshinfo->n * 3;
		orgno = curim2elem.vertno;

		int* orgnobuf;
		orgnobuf = pm2->orgnobuf;
		if( !orgnobuf ){
			DbgOut( "im2file : ParseIM2File : pm2 : orgnobuf not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		for( oldno = 0; oldno < bufleng; oldno++ ){
			if( *( orgnobuf + oldno ) == orgno ){
				optno = *( pm2->oldpno2optpno + oldno );

				ret = selem->SetBIM( m_lpsh, optno, curim2elem.boneno1, curim2elem.boneno2, curim2elem.calcmode );
				if( ret ){
					DbgOut( "IM2file : ParseIM2Line : selem SetBIM error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}

	}else if( selem->type == SHDPOLYMESH ){
		CMeshInfo* miptr;
		miptr = selem->GetMeshInfo();
		if( !miptr ){
			DbgOut( "IM2File : ParseIM2File : meshinfo NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//if( ((curim2elem.vertno < 0) && (curim2elem.vertno != -1)) || (curim2elem.vertno >= miptr->m) ){
		//	DbgOut( "IM2File : ParseIM2File : vertno error !!!\n" );
		//	_ASSERT( 0 );
		//	return 1;
		//}

		if( (curim2elem.vertno < 0) && (curim2elem.vertno != -1) ){
			DbgOut( "IM2File : ParseIM2File : pm vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;			
		}

		CPolyMesh* pm;
		pm = selem->polymesh;
		if( !pm ){
			DbgOut( "IM2File : ParseIM2File : pm not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int* orgnobuf;
		orgnobuf = pm->orgnobuf;
		if( !orgnobuf ){
			DbgOut( "IM2File : ParseIM2File : orgnobuf not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		int optno = -999;
		int oldno;
		for( oldno = 0; oldno < miptr->m; oldno++ ){
			if( *( orgnobuf + oldno ) == curim2elem.vertno ){
				optno = oldno;
				break;
			}
		}

		if( optno != -999 ){
			ret = selem->SetBIM( m_lpsh, optno, curim2elem.boneno1, curim2elem.boneno2, curim2elem.calcmode );
			if( ret ){
				DbgOut( "IM2file : ParseIM2Line : selem SetBIM error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		DbgOut( "IM2File : PrseIM2File : selem type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	selem->m_loadbimflag = 1;


	return 0;	
}


int CIM2File::GetIntFromLine( int lineleng, int pos, int* lengptr, int* isendptr, int* intptr )
{
	//isend は、要素の読み込み前に、終端０が現れたときに１にセットさする。
	
	*isendptr = 0;


//非数字部分を読み飛ばす。
	int stepnum = 0;
	char curc = m_line[pos];
	int minusflag = 0;//数字の直前に'-'があるかどうかのフラグ。

	while( isdigit( curc ) == 0 ){
		stepnum++;
		
		if( (pos + stepnum) >= lineleng ){
			*isendptr = 1;
			return 0;
		}
		
		if( curc == '-' ){
			minusflag = 1;
		}else{
			minusflag = 0;
		}

		curc = m_line[ pos + stepnum ];
	}

// データ長を求める
	int startpos;
	startpos = pos + stepnum;

	int endpos = startpos;
	while( (endpos < lineleng) && (isdigit( curc ) != 0) ){
		endpos++;

		curc = m_line[ endpos ];
	}

	int dataleng;
	dataleng = endpos - startpos;
	if( dataleng >= 256 ){
		DbgOut( "IM2file : GetIntData : dataleng toot long error %d!!!\n", dataleng );
		_ASSERT( 0 );
		return 1;
	}


// 文字列を番号に変換。
	char seristr[256];
	strncpy_s( seristr, 256, m_line + startpos, dataleng );
	seristr[dataleng] = 0;

	
	if( minusflag == 1 ){
		*intptr = -atoi( seristr );
	}else{
		*intptr = atoi( seristr );
	}

	*lengptr = stepnum + dataleng;

	return 0;
}

int CIM2File::GetFloatFromLine( int lineleng, int pos, int* lengptr, int* isendptr, float* floatptr )
{
	//isend は、要素の読み込み前に、終端０が現れたときに１にセットさする。
	
	*isendptr = 0;


//非数字部分を読み飛ばす。
	int stepnum = 0;
	char curc = m_line[pos];
	int minusflag = 0;//数字の直前に'-'があるかどうかのフラグ。

	while( isdigit( curc ) == 0 ){
		stepnum++;
		
		if( (pos + stepnum) >= lineleng ){
			*isendptr = 1;
			return 0;
		}
		
		if( curc == '-' ){
			minusflag = 1;
		}else{
			minusflag = 0;
		}

		curc = m_line[ pos + stepnum ];
	}

// データ長を求める
	int startpos;
	startpos = pos + stepnum;

	int endpos = startpos;
	while( (endpos < lineleng) && ( (isdigit( curc ) != 0) ||  (curc == '.') ) ){
		endpos++;

		curc = m_line[ endpos ];
	}

	int dataleng;
	dataleng = endpos - startpos;
	if( dataleng >= 256 ){
		DbgOut( "IM2file : GetFloatData : dataleng toot long error %d!!!\n", dataleng );
		_ASSERT( 0 );
		return 1;
	}


// 文字列を番号に変換。
	char seristr[256];
	strncpy_s( seristr, 256, m_line + startpos, dataleng );
	seristr[dataleng] = 0;

	
	if( minusflag == 1 ){
		*floatptr = (float)(-atof( seristr ));
	}else{
		*floatptr = (float)(atof( seristr ));
	}

	*lengptr = stepnum + dataleng;

	return 0;
}

int CIM2File::GetAlnumFromLine( int lineleng, int pos, int* lengptr, int* isendptr, char* strptr )
{
	//isend は、要素の読み込み前に、終端０が現れたときに１にセットさする。
	
	*isendptr = 0;


//非数字部分を読み飛ばす。
	int stepnum = 0;
	char curc = m_line[pos];

	//while( (isalnum( curc ) == 0) || (curc != '_') ){
	while( (isspace( curc ) != 0) || (isalnum( curc ) == 0) ){
		stepnum++;
		
		if( (pos + stepnum) >= lineleng ){
			*isendptr = 1;
			return 0;
		}
		
		curc = m_line[ pos + stepnum ];
	}

// データ長を求める
	int startpos;
	startpos = pos + stepnum;

	int endpos = startpos;
	//while( (endpos < lineleng) && ( (isalnum( curc ) != 0) || (curc == '_') ) ){
	while( (endpos < lineleng) && ( (isspace( curc ) == 0) || (curc == '_') ) ){
		endpos++;

		curc = m_line[ endpos ];
	}

	int dataleng;
	dataleng = endpos - startpos;
	if( dataleng >= 256 ){
		DbgOut( "IM2file : GetAlnumData : dataleng toot long error %d!!!\n", dataleng );
		_ASSERT( 0 );
		return 1;
	}


// 格納。

	strncpy_s( strptr, 256, m_line + startpos, dataleng );
	*(strptr + dataleng) = 0;


	*lengptr = stepnum + dataleng;


	return 0;
}



int CIM2File::CheckSameName()
{
	
	// dispobj に、ボーンと同じ名前を付けるのは、許す。


	CNameBank nb;

	CTreeElem2* telem;
	CShdElem* selem;
	int exist;
	int serino;
	int elemkind;//1-->disp obj, 2-->joint, 3-->other
	int chkkind;
	for( serino = 1; serino < m_lpth->s2e_leng; serino++ ){
		telem = (*m_lpth)( serino );
		_ASSERT( telem );

		selem = (*m_lpsh)( serino );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			elemkind = 1;
		}else if( selem->IsJoint() ){
			elemkind = 2;
		}else{
			elemkind = 3;
		}

		int permittype;
		permittype = ( telem->type == SHDPART ) || ( telem->type == SHDINFSCOPE ) || (telem->type == SHDDESTROYED);

		exist = nb.FindName( telem->name );
		if( (exist >= 0) && (permittype == 0) ){
			chkkind = nb.GetIntData( exist );
			if( chkkind == elemkind ){

				DbgOut( "im2file : CheckSameName : %s name error !!!\n", telem->name );
				_ASSERT( 0 );
				return serino;
			}
		}

		nb.AddName( telem->name, elemkind );
	}

	return -1;
}

int CIM2File::SetIM2Params( CShdHandler* lpsh, int partno, int optvno, int childjoint, int parentjoint, int calcmode )
{
	m_lpsh = lpsh;


	if( (partno <= 0) || (partno >= m_lpsh->s2shd_leng) ){
		DbgOut( "mi2file : SetIM2Params : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_lpsh)( partno );
	_ASSERT( selem );

	if( (selem->type != SHDPOLYMESH) && (selem->type != SHDPOLYMESH2) ){
		DbgOut( "mi2file : SetIM2Params : this part is not disp obj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	if( (calcmode != CALCMODE_NOSKIN0) && (calcmode != CALCMODE_ONESKIN0) ){
		DbgOut( "mi2file : SetIM2Params : calcmode error %d !!!\n", calcmode );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = selem->SetBIM( m_lpsh, optvno, childjoint, parentjoint, calcmode );
	if( ret ){
		DbgOut( "IM2file : SetIM2Params : selem SetBIM error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	selem->m_loadbimflag = 2;//!!!!!!!!!!


	return 0;
}

/***
int CIM2File::GetIM2Params( CShdHandler* lpsh, int partno, int optvno, int* childjointptr, int* parentjointptr, int* calcmodeptr )
{

	m_lpsh = lpsh;


	if( (partno <= 0) || (partno >= m_lpsh->s2shd_leng) ){
		DbgOut( "mi2file : GetIM2Params : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_lpsh)( partno );
	_ASSERT( selem );

	if( (selem->type != SHDPOLYMESH) && (selem->type != SHDPOLYMESH2) ){
		DbgOut( "mi2file : GetIM2Params : this part is not disp obj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		

	CD3DDisp* d3dptr;
	d3dptr = selem->d3ddisp;
	if( !d3dptr ){
		DbgOut( "IM2file : GetIM2Params : d3ddisp not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfElem* ieptr;
	ieptr = d3dptr->m_IEptr;
	if( !ieptr ){
		DbgOut( "IM2file : GetIM2Params : infelem not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int vertnum;

	vertnum = d3dptr->m_numTLV;
	if( (optvno < 0) || (optvno >= vertnum) ){
		DbgOut( "im2file : GetIM2Params : optvno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfElem* curie = ieptr + optvno;
	_ASSERT( curie );

	if( curie->infnum != 2 ){
		DbgOut( "im2file : GetIM2Prams : infnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	*childjointptr = curie->ie->childno;
	*parentjointptr = curie->ie->bonematno;
	*calcmodeptr = curie->ie->kind;


	return 0;
}
***/

int CIM2File::WriteUCharData( unsigned char* srcuchar, int num )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcuchar );

	wleng = num;

	WriteFile( m_hfile, srcuchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "im2file : WriteUCharData error %d %d\n",
			wleng, writeleng );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CIM2File::LoadUCharData( unsigned char* dstuc, int ucnum )
{
	DWORD rleng;

	rleng = ucnum;

	if( im2buf.pos + (int)rleng > im2buf.bufleng ){
		_ASSERT( 0 );
		return 1;
	}
	MoveMemory( (void*)dstuc, im2buf.buf + im2buf.pos, rleng );
	im2buf.pos += rleng;

	return 0;
}


int CIM2File::LoadIM2Binary()
{
	int ret;
	int isend = 0;

	while( !isend && (im2buf.pos < im2buf.bufleng) ){

		IM2HEADER im2h;
		ZeroMemory( &im2h, sizeof( IM2HEADER ) );
		ret = LoadUCharData( (unsigned char*)&im2h, sizeof( IM2HEADER ) );
		if( ret ){
			DbgOut( "im2file : LoadIM2Binary : im2h load error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int cmp0;
		cmp0 = strcmp( im2h.name, im2endmark );
		if( cmp0 == 0 ){
			isend = 1;
			break;
		}
		
		int dispseri = 0;
		ret = m_lpth->GetDispObjNoByName( im2h.name, &dispseri, m_lpsh, 0 );
		
		CShdElem* selem = 0;
		CInfElem* ieptr = 0;
		if( dispseri > 0 ){
			selem = (*m_lpsh)( dispseri );
			_ASSERT( selem );

			switch( selem->type ){
			case SHDPOLYMESH:
				if( selem->polymesh && (im2h.vertnum <= selem->polymesh->meshinfo->m) )
					ieptr = selem->polymesh->m_IE;
				break;
			case SHDPOLYMESH2:
				if( selem->polymesh2 && (im2h.vertnum <= selem->polymesh2->optpleng) )
					ieptr = selem->polymesh2->m_IE;
				break;
			default:
				ieptr = 0;
				break;
			}

		}

		if( ieptr ){
			selem->m_loadbimflag = 1;//!!!!!!!!!!!!!!!!!!!!!
		}


		int vertno;
		for( vertno = 0; vertno < im2h.vertnum; vertno++ ){
			INFELEMHEADER ieh;
			ZeroMemory( &ieh, sizeof( INFELEMHEADER ) );
			ret = LoadUCharData( (unsigned char*)&ieh, sizeof( INFELEMHEADER ) );
			if( ret ){
				DbgOut( "im2file : LoadIM2Binary : ieh load error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( ieptr ){
				(ieptr + vertno)->normalizeflag = ieh.normalizeflag;
				(ieptr + vertno)->DestroyIE();
			}

			if( m_version == 4 ){
				int infno;
				for( infno = 0; infno < ieh.infnum; infno++ ){
					INFIM2 infim2;
					ZeroMemory( &infim2, sizeof( INFIM2 ) );
					ret = LoadUCharData( (unsigned char*)&infim2, sizeof( INFIM2 ) );
					if( ret ){
						DbgOut( "im2file : LoadIM2Binary : sie load error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					int boneseri = 0;
					m_lpth->GetBoneNoByName( infim2.bonename, &boneseri, m_lpsh, 0 );


					//ジョイントの番号ではなかった場合は、AddInfしない。！！！！！！！！！！！
					if( ieptr && (boneseri > 0) ){
						INFELEM setIE;

						setIE.bonematno = boneseri;
						setIE.childno = boneseri;
						setIE.kind = infim2.kind;
						setIE.userrate = infim2.userrate;
						setIE.orginf = infim2.orginf;
						setIE.dispinf = infim2.dispinf;

						ret = (ieptr + vertno)->AddInfElem( setIE );
						if( ret ){
							DbgOut( "im2file : LoadIM2Binary : ie AddInfElem error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}

				}
			}else{

				int infno;
				for( infno = 0; infno < ieh.infnum; infno++ ){
					SIGINFELEM sie;
					ZeroMemory( &sie, sizeof( SIGINFELEM ) );
					ret = LoadUCharData( (unsigned char*)&sie, sizeof( SIGINFELEM ) );
					if( ret ){
						DbgOut( "im2file : LoadIM2Binary : sie load error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					int sericheck;
					if( (sie.childno < m_lpsh->s2shd_leng) && (sie.bonematno < m_lpsh->s2shd_leng) ){
						//0以下はダミーもあるためＯＫとする。
						sericheck = 1;
					}else{
						sericheck = 0;
					}

					int childcheck = 0;
					if( sericheck == 1 ){
						if( sie.childno > 0 ){
							CShdElem* childelem;
							childelem = (*m_lpsh)( sie.childno );
							if( childelem->IsJoint() && (childelem->type != SHDMORPH) ){
								childcheck = 1;
							}else{
								childcheck = 0;
							}
						}else{
							childcheck = 1;// ダミーもＯＫ
						}
					}
					int bonecheck = 0;
					if( m_version == 2 ){
						if( sericheck == 1 ){
							if( sie.bonematno > 0 ){
								CShdElem* boneelem;
								boneelem = (*m_lpsh)( sie.bonematno );
								if( boneelem->IsJoint() && (boneelem->type != SHDMORPH) ){
									bonecheck = 1;
								}else{
									bonecheck = 0;
								}
							}else{
								bonecheck = 1;// ダミーもＯＫ
							}
						}
					}else{
						bonecheck = 1;
					}
					//ジョイントの番号ではなかった場合は、AddInfしない。！！！！！！！！！！！
					if( ieptr && (childcheck == 1) && (bonecheck == 1) ){
						INFELEM setIE;

						if( m_version == 2 ){
							if( sie.childno > 0 ){
								setIE.bonematno = sie.childno;//!!!!!!!
							}else{
								setIE.bonematno = sie.bonematno;
							}
						}else{
							setIE.bonematno = sie.bonematno;
						}

						setIE.childno = sie.childno;
						//setIE.bonematno = sie.bonematno;
						setIE.kind = sie.kind;
						setIE.userrate = sie.userrate;
						setIE.orginf = sie.orginf;
						setIE.dispinf = sie.dispinf;

						ret = (ieptr + vertno)->AddInfElem( setIE );
						if( ret ){
							DbgOut( "im2file : LoadIM2Binary : ie AddInfElem error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}

				}
			}
		}

	}

	return 0;
}
