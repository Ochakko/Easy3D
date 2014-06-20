#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <D3DX9.h>
#include <coef.h>

#define DBGH
#include "dbg.h"

#include <treehandler2.h>
#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>
#include <d3ddisp.h>
#include <polymesh2.h>
#include <namebank.h>

#include "BIMFile.h"
#include <crtdbg.h>

static char fileheaderstr[256] = "BoneInfluenceMapFile : ver1001\r\n";
static char partstartstr[256] = "PARTSTART\r\n";
static char partendstr[256] = "PARTEND\r\n";

//static char skinmodestr0[256] = "SKINNING_MODE\r\n";
//static char skinmodestr1[256] = "NO_SKINNING_MODE\r\n";

static char calcstr0[256] = "CALC_NOSKIN0";
static char calcstr1[256] = "CALC_ONESKIN0";


CBIMFile::CBIMFile()
{

	InitParams();

}
CBIMFile::~CBIMFile()
{
	if( bimbuf.buf ){
		free( bimbuf.buf );
		bimbuf.buf = 0;
	}

}

int CBIMFile::InitParams()
{
	m_hfile = INVALID_HANDLE_VALUE;
	m_lpth = 0;
	m_lpsh = 0;
	m_lpmh = 0;

	bimbuf.buf = 0;
	bimbuf.bufleng = 0;
	bimbuf.pos = 0;
	bimbuf.isend = 0;

	BIMELEMInit( 1 );

	m_firstseri = 0;

	m_calcmode = CALCMODE_ONESKIN0;

	return 0;
}

int CBIMFile::BIMELEMInit( int inimode )
{
	if( inimode == 1 ){
		curbimelem.partname[0] = 0;
		curbimelem.serialno = -1;
	}

	curbimelem.vertno = -1;
	curbimelem.boneno1 = -1;
	curbimelem.boneno2 = -1;
	curbimelem.calcmode = CALCMODE_ONESKIN0;

	return 0;
}

int CBIMFile::WriteBIMFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh )
{
	int ret = 0;

	if( !lpth || !lpsh ){
		DbgOut( "BIMFile : WriteBIMFile : handler NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_lpth = lpth;
	m_lpsh = lpsh;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "BIMFile : WriteBIMFile : file open error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto bimfexit;
	}

	ret = GetFirstJointNo( &m_firstseri );
	if( ret ){
		DbgOut( "BIMFile : WriteBIMFile : GetFirstJointNo error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto bimfexit;
	}

	int sameno;
	sameno = CheckSameName();
	if( sameno >= 0 ){
		DbgOut( "bimfile : WriteBIMFile : CheckSameName error !!!\n" );
		_ASSERT( 0 );

		char messtr1[1024];
		strcpy( messtr1, "同じオブジェクト名が複数あると、\n正しく、ファイルを作れません。\n名前を変えてから、再試行してください。\n" );

		CTreeElem2* sametelem;
		sametelem = (*m_lpth)( sameno );
		_ASSERT( sametelem );

		::MessageBox( NULL, messtr1, sametelem->name, MB_OK );

		ret = 1;
		goto bimfexit;
	}


	Write2File( fileheaderstr );


	int serino;
	CShdElem* selem;
	CTreeElem2* telem;
	int dispflag, isbone;
	for( serino = 0; serino < m_lpsh->s2shd_leng; serino++ ){

		selem = (*m_lpsh)( serino );
		telem = (*m_lpth)( serino );

		if( !selem || !telem ){
			DbgOut( "BIMFile : WriteBIMFile : elem error %x %x !!!\n", selem, telem );
			_ASSERT( 0 );
			ret = 1;
			goto bimfexit;
		}


		dispflag = selem->dispflag;
		if( selem->IsJoint() ){
			isbone = 1;
		}else{
			isbone = 0;
		}
		

		if( dispflag && (isbone == 0) ){
			ret = WriteBIM( selem, telem );
			if( ret ){
				DbgOut( "BIMFile : WriteBIMFile : WriteBIM error %d!!!\n", serino );
				ret = 1;
				goto bimfexit;
			}
		}
	}

	goto bimfexit;
bimfexit:
	if( m_hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hfile );
		SetEndOfFile( m_hfile );
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	return ret;
}

int CBIMFile::GetFirstJointNo( int* firstno )
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
int CBIMFile::Write2File( char* lpFormat, ... )
{
	if( !m_hfile ){
		//_ASSERT( 0 );
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[BIMLINELEN];
			
	ZeroMemory( outchar, BIMLINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf( outchar, lpFormat, Marker );
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

int CBIMFile::WriteBIM( CShdElem* selem, CTreeElem2* telem )
{
	int ret;
	
	ret = Write2File( "\r\n" );
	if( ret ){
		DbgOut( "BIMfile : WriteBIM : Write2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = Write2File( partstartstr );
	if( ret ){
		DbgOut( "BIMfile : WriteBIM : Write2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
/////////
	BIMELEMInit( 1 );

	if( telem->name ){
		int nameleng;
		nameleng = (int)strlen( telem->name );
		if( nameleng >= BIMLINELEN ){
			DbgOut( "BIMfile : WriteBIM : name length too long error %s!!!\n", telem->name );
			_ASSERT( 0 );
			return 1;
		}
		strcpy( curbimelem.partname, telem->name );
	}else{
		DbgOut( "BIMfile : WriteBIM : name is NULL warning !!! skip\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!
	}

	curbimelem.serialno = selem->serialno;

	ret = Write2File( "%s\r\n", curbimelem.partname );
	if( ret ){
		DbgOut( "BIMfile : WriteBIM : Write2File partname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////
	switch( selem->type ){
	case SHDPOLYMESH:
		ret = WritePolyMeshBIM( selem );
		if( ret ){
			DbgOut( "BIMfile : WriteBIM : WritePolyMeshBIM error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SHDPOLYMESH2:
		ret = WritePolyMesh2BIM( selem );
		if( ret ){
			DbgOut( "BIMfile : WriteBIM : WritePolyMeshBIM2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		break;
	}

////////
	ret = Write2File( partendstr );
	if( ret ){
		DbgOut( "BIMfile : WriteBIM : Write2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBIMFile::WritePolyMeshBIM( CShdElem* selem )
{
	DWORD vertno;
	DWORD vertnum;

	CD3DDisp* d3dptr;
	d3dptr = selem->d3ddisp;
	if( !d3dptr ){
		DbgOut( "BIMfile : WritePolyMeshBIM : d3ddisp not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBoneElem* beptr;
	beptr = d3dptr->m_BEptr;
	if( !beptr ){
		DbgOut( "BIMfile : WritePolyMeshBIM : boneelem not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	vertnum = d3dptr->m_numTLV;
	CBoneElem* curbe = beptr;
	for( vertno = 0; vertno < vertnum; vertno++ ){
		ret = Write2File( "%d, %d, %d, ", vertno, curbe->childno - m_firstseri, curbe->bonematno - m_firstseri );
		if( ret ){
			DbgOut( "BIMfile : WritePolyMeshBIM : Write2File error %d !!!\n", vertno );
			_ASSERT( 0 );
			return 1;
		}

		switch( curbe->calcmode ){
		case CALCMODE_ONESKIN0:
			ret = Write2File( "%s\r\n", calcstr1 );
			if( ret ){
				DbgOut( "BIMfile : WritePolyMeshBIM : Write2File error %d !!!\n", vertno );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case CALCMODE_NOSKIN0:
			ret = Write2File( "%s\r\n", calcstr0 );
			if( ret ){
				DbgOut( "BIMfile : WritePolyMeshBIM : Write2File error %d !!!\n", vertno );
				_ASSERT( 0 );
				return 1;
			}
			break;
		default:
			DbgOut( "bimfile : WritePolyMeshBIM : invalid BoneElem calcmode warning !!!\n" );
			_ASSERT( 0 );

			ret = Write2File( "%s\r\n", calcstr1 );
			if( ret ){
				DbgOut( "BIMfile : WritePolyMeshBIM : Write2File error %d !!!\n", vertno );
				_ASSERT( 0 );
				return 1;
			}
			break;
		}


		curbe++;
	}

	return 0;
}
int CBIMFile::WritePolyMesh2BIM( CShdElem* selem )
{
	DWORD vertno;
	DWORD vertnum;

	CPolyMesh2* pm2;
	pm2 = selem->polymesh2;
	if( !pm2 ){
		DbgOut( "BIMfile : WritePolyMesh2BIM : polymesh2 NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CD3DDisp* d3dptr;
	d3dptr = selem->d3ddisp;
	if( !d3dptr ){
		DbgOut( "BIMfile : WritePolyMesh2BIM : d3ddisp not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBoneElem* beptr;
	beptr = d3dptr->m_BEptr;
	if( !beptr ){
		DbgOut( "BIMfile : WritePolyMesh2BIM : boneelem not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	vertnum = d3dptr->m_numTLV;
	CBoneElem* curbe = beptr;
	int oldpno;
	for( vertno = 0; vertno < vertnum; vertno++ ){
		
		oldpno = *( pm2->optpno2oldpno + vertno );
		while( oldpno >= 0 ){

			ret = Write2File( "%d, %d, %d, ", oldpno, curbe->childno - m_firstseri, curbe->bonematno - m_firstseri );
			if( ret ){
				DbgOut( "BIMfile : WritePolyMesh2BIM : Write2File error %d %d!!!\n", vertno, oldpno );
				_ASSERT( 0 );
				return 1;
			}


			switch( curbe->calcmode ){
			case CALCMODE_ONESKIN0:
				ret = Write2File( "%s\r\n", calcstr1 );
				if( ret ){
					DbgOut( "BIMfile : WritePolyMeshBIM : Write2File error %d !!!\n", vertno );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case CALCMODE_NOSKIN0:
				ret = Write2File( "%s\r\n", calcstr0 );
				if( ret ){
					DbgOut( "BIMfile : WritePolyMeshBIM : Write2File error %d !!!\n", vertno );
					_ASSERT( 0 );
					return 1;
				}
				break;
			default:
				DbgOut( "bimfile : WritePolyMeshBIM : invalid BoneElem calcmode warning !!!\n" );
				_ASSERT( 0 );

				ret = Write2File( "%s\r\n", calcstr1 );
				if( ret ){
					DbgOut( "BIMfile : WritePolyMeshBIM : Write2File error %d !!!\n", vertno );
					_ASSERT( 0 );
					return 1;
				}
				break;
			}


			oldpno = *( pm2->samepointbuf + oldpno );
		}
		
		curbe++;
	}


	return 0;
}

int CBIMFile::LoadBIMFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh )
{

	int ret = 0;

	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "BIMFile : LoadBIMFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
	}	

	ret = InitBIMBUF();
	if( ret ){
		DbgOut( "BIMFile : LoadBIMFile : InitBIMBUF error !!!\n" );
		_ASSERT( 0 );
		goto ldbimexit;
	}
	
	ret = CheckFileHeader();
	if( ret ){
		DbgOut( "BIMFile : LoadBIMFile : CheckFileHeader error !!!\n" );
		_ASSERT( 0 );
		goto ldbimexit;
	}

	ret = GetFirstJointNo( &m_firstseri );//InitBIMBUFより後で。m_firstseri
	if( ret ){
		DbgOut( "BIMFile : LoadBIMFile : GetFirstJointNo error !!!\n" );
		_ASSERT( 0 );
		goto ldbimexit;
	}
	//BIM_NUTRAL,
	//BIM_LOADNAME,
	//BIM_LOADBIM,
	//BIM_FINISH,

	int loadmode;
	int loadflag;

	loadmode = BIM_NUTRAL;
	loadflag = 1;
	while( loadflag ){
		switch( loadmode ){
		case BIM_NUTRAL:
			ret = GetLoadMode( &loadmode );
			if( ret ){
				DbgOut( "BIMFile : LoadBIMFile : GetLoadMode error !!!\n" );
				_ASSERT( 0 );
				loadflag = 0;
			}
			break;

		case BIM_FINISH:
			loadflag = 0;
			break;
		
		case BIM_LOADNAME:
			ret = LoadPartName( &loadmode );
			if( ret ){
				DbgOut( "BIMFile : LoadBIMFile : LoadPartName error !!!\n" );
				_ASSERT( 0 );
				goto ldbimexit;
			}
			break;

		case BIM_LOADBIM:
			ret = LoadBIM( &loadmode );
			if( ret ){
				DbgOut( "BIMFile : LoadBIMFile : LoadBIM error !!!\n" );
				_ASSERT( 0 );
				goto ldbimexit;
			}
			break;
		default:
			DbgOut( "BIMFile : LoadBIMFile : loadmode error !!!\n" );
			_ASSERT( 0 );
			loadflag = 0;
			break;
		}
	}

	goto ldbimexit;
ldbimexit:

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
	
	if( bimbuf.buf ){
		free( bimbuf.buf );
		bimbuf.buf = 0;

		bimbuf.bufleng = 0;
		bimbuf.pos = 0;
		bimbuf.isend = 0;
	}

	return ret;
}
int CBIMFile::InitBIMBUF()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "BIMFile : InitBIMBUF :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "BIMFile : InitBIMBUF :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * bufleng );
	if( !newbuf ){
		DbgOut( "BIMFile : InitBIMBUF :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "BIMFile : InitBIMBUF :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}
	
	bimbuf.buf = newbuf;
	bimbuf.bufleng = bufleng;
	bimbuf.pos = 0;
	bimbuf.isend = 0;

	m_firstseri = 0;

	return 0;
}
int CBIMFile::CheckFileHeader()
{
	char* findptr = 0;

	findptr = strstr( bimbuf.buf, fileheaderstr );
	if( findptr ){
		return 0;
	}else{
		return 1;
	}
}

int CBIMFile::GetLoadMode( int* modeptr )
{
	int ret;
	int ldleng;

	ret = LoadLine( &ldleng );
	if( ret ){
		DbgOut( "BIMFile : GetLoadMode : LoadLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( bimbuf.isend != 0 ){
		*modeptr = BIM_FINISH;
		return 0;
	}
	
	int cmp1, cmp2;
	cmp1 = strcmp( m_line, partstartstr );
	if( cmp1 == 0 ){
		*modeptr = BIM_LOADNAME;
		return 0;
	}else{
		cmp2 = strcmp( m_line, partendstr );
		if( cmp2 == 0 ){
			*modeptr = BIM_LOADBIM;
			return 0;
		}else{
			*modeptr = BIM_NUTRAL;
			return 0;
		}
	}

	return 0;
}


int CBIMFile::LoadPartName( int* modeptr )
{

	BIMELEMInit( 1 );


	int ret;
	int ldleng;
	ret = LoadLine( &ldleng );
	if( ret ){
		DbgOut( "BIMFile : LoadPartName : LoadLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( bimbuf.isend != 0 ){
		*modeptr = BIM_FINISH;
		return 0;
	}

	int cmp;
	cmp = strcmp( m_line, partendstr );
	if( cmp == 0 ){
		*modeptr = BIM_NUTRAL;
		return 0;
	}

	char* endchar;
	endchar = strchr( m_line, '\r' );
	if( endchar ){
		int nameleng;
		nameleng = (int)( endchar - m_line );
		strncpy( curbimelem.partname, m_line, nameleng );
		curbimelem.partname[nameleng] = 0;
	}else{
		strcpy( curbimelem.partname, m_line );
	}


DbgOut( "bimfile : LoadPartName : name %s\n", curbimelem.partname );

/////////
	ret = m_lpth->GetPartNoByName( curbimelem.partname, &(curbimelem.serialno) );
	if( ret ){
		DbgOut( "BIMfile : LoadPartName : lpth GetPartNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curbimelem.serialno <= 0 ){
		DbgOut( "BIMfile : LoadPartName : invalid partname , serialno not found error %s!!!\n", curbimelem.partname );
		_ASSERT( 0 );
		return 1;
	}

//////////

	*modeptr = BIM_LOADBIM;


	return 0;
}
int CBIMFile::LoadBIM( int* modeptr )
{
	int ret;

	while( 1 ){

		BIMELEMInit( 0 );

		int ldleng;
		ret = LoadLine( &ldleng );
		if( ret ){
			DbgOut( "BIMFile : LoadBIM : LoadLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( bimbuf.isend != 0 ){
			*modeptr = BIM_FINISH;
			break;
		}

		int cmp;
		cmp = strcmp( m_line, partendstr );
		if( cmp == 0 ){
			*modeptr = BIM_NUTRAL;
			break;
		}

		ret = ParseBIMLine();
		if( ret ){
			DbgOut( "BIMFile : LoadBIM : ParseBIMLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}



int CBIMFile::LoadLine( int* lenptr )
{
	int stepnum = 0;
	unsigned int curpos = bimbuf.pos;
	char curc;

	while( curpos < bimbuf.bufleng ){
		curc = *( bimbuf.buf + curpos );	

		if( curc == '\n' ){
			stepnum++;
			break;
		}
		stepnum++;
		curpos++;
	}

	int ret = 0;
	if( stepnum >= BIMLINELEN ){
		// 行が長すぎ
		DbgOut( "BIMFile : LoadLine : stepnum overflow error !!!\n" );
		_ASSERT( 0 );
		m_line[0] = 0;
		ret = 1;
	}else if( stepnum > 0 ){
		// 正常
		strncpy( m_line, bimbuf.buf + bimbuf.pos, stepnum );
		m_line[ stepnum ] = 0;
		ret = 0;
	}else{
		// ファイルの終わり
		m_line[0] = 0;
		bimbuf.isend = 1;
		ret = 0;//!!!!!!!
	}

	*lenptr = stepnum;
	bimbuf.pos += stepnum;

	return ret;
}

int CBIMFile::ParseBIMLine()
{
	int ret;
	int ldleng;
	int pos = 0;

	int lineleng;
	lineleng = (int)strlen( m_line );

	int isend;

	BIMELEMInit( 0 );//!!!!


// 番号が、 , で区切られている。

	int* dstptrlist[3];
	dstptrlist[0] = &(curbimelem.vertno);
	dstptrlist[1] = &(curbimelem.boneno1);
	dstptrlist[2] = &(curbimelem.boneno2);

	char templine[BIMLINELEN];


	int getno;
	for( getno = 0; getno < 4; getno++ ){
		if( pos >= lineleng ){
			DbgOut( "BIMFile : ParseBIMLine : pos overflow error %d!!! return\n", getno );
			_ASSERT( 0 );
			return 1;
		}

		if( getno < 3 ){
			ret = GetIntFromLine( lineleng, pos, &ldleng, &isend, dstptrlist[getno] );
			if( ret ){
				DbgOut( "BIMFile : ParseBIMLine : GetIntFromLine error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( isend != 0 ){
				DbgOut( "BIMfile : ParseBIMLine : getint %d data is too short error !!!\n", getno );
				_ASSERT( 0 );
				return 1;
			}

		}else{
			templine[0] = 0;
			ret = GetAlnumFromLine( lineleng, pos, &ldleng, &isend, templine );
			if( ret ){
				DbgOut( "BIMFile : ParseBIMLine : GetFloatFromLine error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( isend != 0 ){
				DbgOut( "BIMfile : ParseBIMLine : get alnum data is too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			//m_calcmodeのセット
			int cmp0, cmp1;
			cmp0 = strcmp( calcstr0, templine );
			cmp1 = strcmp( calcstr1, templine );

			if( cmp0 == 0 ){
				curbimelem.calcmode = CALCMODE_NOSKIN0;
			}else{
				if( cmp1 == 0 ){
					curbimelem.calcmode = CALCMODE_ONESKIN0;
				}else{
					DbgOut( "bimfile : ParseBIMLine : invalid calcmode : set default value warning !!!\n" );
					_ASSERT( 0 );

					curbimelem.calcmode = CALCMODE_ONESKIN0;

				}
			}

		}
		if( isend != 0 ){
			break;
		}
		pos += ldleng;

	}


	curbimelem.boneno1 += m_firstseri;
	curbimelem.boneno2 += m_firstseri;


////////// boneelemのセット
	
	CShdElem* selem;
	selem = (*m_lpsh)( curbimelem.serialno );
	if( !selem ){
		DbgOut( "BIMfile : ParseBIMLine : selem NULL error %d !!!\n", curbimelem.serialno );
		_ASSERT( 0 );
		return 1;
	}

	if( selem->type == SHDPOLYMESH2 ){
		CPolyMesh2* pm2;
		pm2 = selem->polymesh2;
		if( !pm2 ){
			DbgOut( "BIMFile : ParseBIMLine : pm2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		CMeshInfo* miptr;
		miptr = selem->GetMeshInfo();
		if( !miptr ){
			DbgOut( "BIMFile : ParseBIMFile : meshinfo NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( ((curbimelem.vertno < 0) && (curbimelem.vertno != -1)) || (curbimelem.vertno >= miptr->n * 3) ){
			DbgOut( "BIMFile : ParseBIMFile : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( curbimelem.vertno != -1 ){
			curbimelem.vertno = *(pm2->oldpno2optpno + curbimelem.vertno);//!!!!!!!!!!!
		}

	}else if( selem->type == SHDPOLYMESH ){
		CMeshInfo* miptr;
		miptr = selem->GetMeshInfo();
		if( !miptr ){
			DbgOut( "BIMFile : ParseBIMFile : meshinfo NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( ((curbimelem.vertno < 0) && (curbimelem.vertno != -1)) || (curbimelem.vertno >= miptr->m) ){
			DbgOut( "BIMFile : ParseBIMFile : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
	}else{
		DbgOut( "BIMFile : PrseBIMFile : selem type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = selem->SetBIM( m_lpsh, curbimelem.vertno, curbimelem.boneno1, curbimelem.boneno2, curbimelem.calcmode );
	if( ret ){
		DbgOut( "BIMfile : ParseBIMLine : selem SetBIM error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	selem->m_loadbimflag = 1;


	return 0;	
}


int CBIMFile::GetIntFromLine( int lineleng, int pos, int* lengptr, int* isendptr, int* intptr )
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
		DbgOut( "BIMfile : GetIntData : dataleng toot long error %d!!!\n", dataleng );
		_ASSERT( 0 );
		return 1;
	}


// 文字列を番号に変換。
	char seristr[256];
	strncpy( seristr, m_line + startpos, dataleng );
	seristr[dataleng] = 0;

	
	if( minusflag == 1 ){
		*intptr = -atoi( seristr );
	}else{
		*intptr = atoi( seristr );
	}

	*lengptr = stepnum + dataleng;

	return 0;
}

int CBIMFile::GetFloatFromLine( int lineleng, int pos, int* lengptr, int* isendptr, float* floatptr )
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
		DbgOut( "BIMfile : GetFloatData : dataleng toot long error %d!!!\n", dataleng );
		_ASSERT( 0 );
		return 1;
	}


// 文字列を番号に変換。
	char seristr[256];
	strncpy( seristr, m_line + startpos, dataleng );
	seristr[dataleng] = 0;

	
	if( minusflag == 1 ){
		*floatptr = (float)(-atof( seristr ));
	}else{
		*floatptr = (float)(atof( seristr ));
	}

	*lengptr = stepnum + dataleng;

	return 0;
}

int CBIMFile::GetAlnumFromLine( int lineleng, int pos, int* lengptr, int* isendptr, char* strptr )
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
		DbgOut( "BIMfile : GetAlnumData : dataleng toot long error %d!!!\n", dataleng );
		_ASSERT( 0 );
		return 1;
	}


// 格納。

	strncpy( strptr, m_line + startpos, dataleng );
	*(strptr + dataleng) = 0;


	*lengptr = stepnum + dataleng;


	return 0;
}



int CBIMFile::CheckSameName()
{
	CNameBank nb;

	CTreeElem2* telem;
	int exist;
	int serino;
	for( serino = 1; serino < m_lpth->s2e_leng; serino++ ){
		telem = (*m_lpth)( serino );
		_ASSERT( telem );

		exist = nb.FindName( telem->name );
		if( exist >= 0 ){
			DbgOut( "bimfile : CheckSameName : %s name error !!!\n" );
			_ASSERT( 0 );

			return serino;
		}

		nb.AddName( telem->name );
	}

	return -1;
}



