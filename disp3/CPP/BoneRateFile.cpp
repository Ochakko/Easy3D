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

#include "BoneRateFile.h"
#include <crtdbg.h>

static char fileheaderstr[256] = "Bone Influence rate file : ver1001\r\n";
static char partstartstr[256] = "PARTSTART\r\n";
static char partendstr[256] = "PARTEND\r\n";


CBoneRateFile::CBoneRateFile()
{

	InitParams();

}
CBoneRateFile::~CBoneRateFile()
{
	if( ratbuf.buf ){
		free( ratbuf.buf );
		ratbuf.buf = 0;
	}

}

int CBoneRateFile::InitParams()
{
	m_hfile = INVALID_HANDLE_VALUE;
	m_lpth = 0;
	m_lpsh = 0;
	m_lpmh = 0;

	ratbuf.buf = 0;
	ratbuf.bufleng = 0;
	ratbuf.pos = 0;
	ratbuf.isend = 0;

	RATELEMInit( 1 );

	m_firstseri = 0;

	return 0;
}

int CBoneRateFile::RATELEMInit( int inimode )
{
	if( inimode == 1 ){
		curratelem.partname[0] = 0;
		curratelem.serialno = -1;
	}

	curratelem.vertno = -1;
	curratelem.boneno1 = -1;
	curratelem.boneno2 = -1;
	curratelem.bonerate1 = 1.0f;

	return 0;
}

int CBoneRateFile::WriteBoneRateFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh )
{
	int ret = 0;

	if( !lpth || !lpsh ){
		DbgOut( "BoneRateFile : WriteBoneRateFile : handler NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_lpth = lpth;
	m_lpsh = lpsh;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "BoneRateFile : WriteBoneRateFile : file open error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto ratfexit;
	}

	ret = GetFirstJointNo( &m_firstseri );
	if( ret ){
		DbgOut( "BoneRateFile : WriteBoneRateFile : GetFirstJointNo error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto ratfexit;
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
			DbgOut( "BoneRateFile : WriteBoneRateFile : elem error %x %x !!!\n", selem, telem );
			_ASSERT( 0 );
			ret = 1;
			goto ratfexit;
		}


		dispflag = selem->dispflag;
		if( selem->IsJoint() ){
			isbone = 1;
		}else{
			isbone = 0;
		}
		

		if( dispflag && (isbone == 0) ){
			ret = WriteRate( selem, telem );
			if( ret ){
				DbgOut( "BoneRateFile : WriteBoneRateFile : WriteRate error %d!!!\n", serino );
				ret = 1;
				goto ratfexit;
			}
		}
	}

	goto ratfexit;
ratfexit:
	if( m_hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hfile );
		SetEndOfFile( m_hfile );
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	return ret;
}

int CBoneRateFile::GetFirstJointNo( int* firstno )
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
int CBoneRateFile::Write2File( char* lpFormat, ... )
{
	if( !m_hfile ){
		//_ASSERT( 0 );
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[RATLINELEN];
			
	ZeroMemory( outchar, RATLINELEN );

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

int CBoneRateFile::WriteRate( CShdElem* selem, CTreeElem2* telem )
{
	int ret;
	
	ret = Write2File( "\r\n" );
	if( ret ){
		DbgOut( "boneratefile : WriteRate : Write2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = Write2File( partstartstr );
	if( ret ){
		DbgOut( "boneratefile : WriteRate : Write2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
/////////
	RATELEMInit( 1 );

	if( telem->name ){
		int nameleng;
		nameleng = (int)strlen( telem->name );
		if( nameleng >= RATLINELEN ){
			DbgOut( "boneratefile : WriteRate : name length too long error %s!!!\n", telem->name );
			_ASSERT( 0 );
			return 1;
		}
		strcpy( curratelem.partname, telem->name );
	}else{
		DbgOut( "boneratefile : WriteRate : name is NULL warning !!! skip\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!
	}

	curratelem.serialno = selem->serialno;

	ret = Write2File( "%s\r\n", curratelem.partname );
	if( ret ){
		DbgOut( "boneratefile : WriteRate : Write2File partname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////
	switch( selem->type ){
	case SHDPOLYMESH:
		ret = WritePolyMeshRate( selem );
		if( ret ){
			DbgOut( "boneratefile : WriteRate : WritePolyMeshRate error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SHDPOLYMESH2:
		ret = WritePolyMesh2Rate( selem );
		if( ret ){
			DbgOut( "boneratefile : WriteRate : WritePolyMeshRate2 error !!!\n" );
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
		DbgOut( "boneratefile : WriteRate : Write2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBoneRateFile::WritePolyMeshRate( CShdElem* selem )
{
	DWORD vertno;
	DWORD vertnum;

	CD3DDisp* d3dptr;
	d3dptr = selem->d3ddisp;
	if( !d3dptr ){
		DbgOut( "boneratefile : WritePolyMeshRate : d3ddisp not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBoneElem* beptr;
	beptr = d3dptr->m_BEptr;
	if( !beptr ){
		DbgOut( "boneratefile : WritePolyMeshRate : boneelem not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	vertnum = d3dptr->m_numTLV;
	CBoneElem* curbe = beptr;
	for( vertno = 0; vertno < vertnum; vertno++ ){
		ret = Write2File( "%d, %d, %d, %f\r\n", vertno, curbe->bonematno - m_firstseri, curbe->parmatno - m_firstseri, curbe->bonerate );
		if( ret ){
			DbgOut( "boneratefile : WritePolyMeshRate : Write2File error %d !!!\n", vertno );
			_ASSERT( 0 );
			return 1;
		}

		curbe++;
	}

	return 0;
}
int CBoneRateFile::WritePolyMesh2Rate( CShdElem* selem )
{
	DWORD vertno;
	DWORD vertnum;

	CPolyMesh2* pm2;
	pm2 = selem->polymesh2;
	if( !pm2 ){
		DbgOut( "boneratefile : WritePolyMesh2Rate : polymesh2 NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CD3DDisp* d3dptr;
	d3dptr = selem->d3ddisp;
	if( !d3dptr ){
		DbgOut( "boneratefile : WritePolyMesh2Rate : d3ddisp not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBoneElem* beptr;
	beptr = d3dptr->m_BEptr;
	if( !beptr ){
		DbgOut( "boneratefile : WritePolyMesh2Rate : boneelem not created error !!!\n" );
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

			ret = Write2File( "%d, %d, %d, %f\r\n", oldpno, curbe->bonematno - m_firstseri, curbe->parmatno - m_firstseri, curbe->bonerate );
			if( ret ){
				DbgOut( "boneratefile : WritePolyMesh2Rate : Write2File error %d %d!!!\n", vertno, oldpno );
				_ASSERT( 0 );
				return 1;
			}

			oldpno = *( pm2->samepointbuf + oldpno );
		}
		
		curbe++;
	}


	return 0;
}

int CBoneRateFile::LoadBoneRateFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh )
{

	int ret = 0;

	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "BoneRateFile : LoadBoneRateFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
	}	

	ret = InitRATBUF();
	if( ret ){
		DbgOut( "BoneRateFile : LoadBoneRateFile : InitRATBUF error !!!\n" );
		_ASSERT( 0 );
		goto ldratexit;
	}
	
	ret = CheckFileHeader();
	if( ret ){
		DbgOut( "BoneRateFile : LoadBoneRateFile : CheckFileHeader error !!!\n" );
		_ASSERT( 0 );
		goto ldratexit;
	}

	ret = GetFirstJointNo( &m_firstseri );//InitRATBUFより後で。m_firstseri
	if( ret ){
		DbgOut( "BoneRateFile : LoadBoneRateFile : GetFirstJointNo error !!!\n" );
		_ASSERT( 0 );
		goto ldratexit;
	}
	//RAT_NUTRAL,
	//RAT_LOADNAME,
	//RAT_LOADRAT,
	//RAT_FINISH,

	int loadmode = RAT_NUTRAL;
	int loadflag = 1;
	while( loadflag ){
		switch( loadmode ){
		case RAT_NUTRAL:
			ret = GetLoadMode( &loadmode );
			if( ret ){
				DbgOut( "BoneRateFile : LoadBoneRateFile : GetLoadMode error !!!\n" );
				_ASSERT( 0 );
				loadflag = 0;
			}
			break;

		case RAT_FINISH:
			loadflag = 0;
			break;
		
		case RAT_LOADNAME:
			ret = LoadPartName( &loadmode );
			if( ret ){
				DbgOut( "BoneRateFile : LoadBoneRateFile : LoadPartName error !!!\n" );
				_ASSERT( 0 );
				goto ldratexit;
			}
			break;

		case RAT_LOADRAT:
			ret = LoadRat( &loadmode );
			if( ret ){
				DbgOut( "BoneRateFile : LoadBoneRateFile : LoadRat error !!!\n" );
				_ASSERT( 0 );
				goto ldratexit;
			}
			break;
		default:
			DbgOut( "BoneRateFile : LoadBoneRateFile : loadmode error !!!\n" );
			_ASSERT( 0 );
			loadflag = 0;
			break;
		}
	}

	goto ldratexit;
ldratexit:

	// ボーン自体の影響度の計算を忘れずに！！！


	if( m_hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( ratbuf.buf ){
		free( ratbuf.buf );
		ratbuf.buf = 0;

		ratbuf.bufleng = 0;
		ratbuf.pos = 0;
		ratbuf.isend = 0;
	}

	return ret;
}
int CBoneRateFile::InitRATBUF()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "BoneRateFile : InitRATBUF :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "BoneRateFile : InitRATBUF :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "BoneRateFile : InitRATBUF :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "BoneRateFile : InitRATBUF :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}
	
	ratbuf.buf = newbuf;
	ratbuf.bufleng = bufleng;
	ratbuf.pos = 0;
	ratbuf.isend = 0;

	m_firstseri = 0;

	return 0;
}
int CBoneRateFile::CheckFileHeader()
{
	char* findptr = 0;

	findptr = strstr( ratbuf.buf, fileheaderstr );
	if( findptr ){
		return 0;
	}else{
		return 1;
	}
}

int CBoneRateFile::GetLoadMode( int* modeptr )
{
	int ret;
	int ldleng;

	ret = LoadLine( &ldleng );
	if( ret ){
		DbgOut( "BoneRateFile : GetLoadMode : LoadLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( ratbuf.isend != 0 ){
		*modeptr = RAT_FINISH;
		return 0;
	}
	
	int cmp1, cmp2;
	cmp1 = strcmp( m_line, partstartstr );
	if( cmp1 == 0 ){
		*modeptr = RAT_LOADNAME;
		return 0;
	}else{
		cmp2 = strcmp( m_line, partendstr );
		if( cmp2 == 0 ){
			*modeptr = RAT_LOADRAT;
			return 0;
		}else{
			*modeptr = RAT_NUTRAL;
			return 0;
		}
	}

	return 0;
}


int CBoneRateFile::LoadPartName( int* modeptr )
{

	RATELEMInit( 1 );


	int ret;
	int ldleng;
	ret = LoadLine( &ldleng );
	if( ret ){
		DbgOut( "BoneRateFile : LoadPartName : LoadLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( ratbuf.isend != 0 ){
		*modeptr = RAT_FINISH;
		return 0;
	}

	int cmp;
	cmp = strcmp( m_line, partendstr );
	if( cmp == 0 ){
		*modeptr = RAT_NUTRAL;
		return 0;
	}

	strcpy( curratelem.partname, m_line );

/////////
	ret = m_lpth->GetPartNoByName( curratelem.partname, &(curratelem.serialno) );
	if( ret ){
		DbgOut( "boneratefile : LoadPartName : lpth GetPartNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curratelem.serialno <= 0 ){
		DbgOut( "boneratefile : LoadPartName : invalid partname , serialno not found error %s!!!\n", curratelem.partname );
		_ASSERT( 0 );
		return 1;
	}

//////////

	*modeptr = RAT_LOADRAT;


	return 0;
}
int CBoneRateFile::LoadRat( int* modeptr )
{
	int ret;

	while( 1 ){

		RATELEMInit( 0 );

		int ldleng;
		ret = LoadLine( &ldleng );
		if( ret ){
			DbgOut( "BoneRateFile : LoadRat : LoadLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( ratbuf.isend != 0 ){
			*modeptr = RAT_FINISH;
			break;
		}

		int cmp;
		cmp = strcmp( m_line, partendstr );
		if( cmp == 0 ){
			*modeptr = RAT_NUTRAL;
			break;
		}

		ret = ParseRatLine();
		if( ret ){
			DbgOut( "BoneRateFile : LoadRat : ParseRatLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}



int CBoneRateFile::LoadLine( int* lenptr )
{
	int stepnum = 0;
	unsigned int curpos = ratbuf.pos;
	char curc;

	while( curpos < ratbuf.bufleng ){
		curc = *( ratbuf.buf + curpos );	

		if( curc == '\n' ){
			stepnum++;
			break;
		}
		stepnum++;
		curpos++;
	}

	int ret = 0;
	if( stepnum >= RATLINELEN ){
		// 行が長すぎ
		DbgOut( "BoneRateFile : LoadLine : stepnum overflow error !!!\n" );
		_ASSERT( 0 );
		m_line[0] = 0;
		ret = 1;
	}else if( stepnum > 0 ){
		// 正常
		strncpy( m_line, ratbuf.buf + ratbuf.pos, stepnum );
		m_line[ stepnum ] = 0;
		ret = 0;
	}else{
		// ファイルの終わり
		m_line[0] = 0;
		ratbuf.isend = 1;
		ret = 0;//!!!!!!!
	}

	*lenptr = stepnum;
	ratbuf.pos += stepnum;

	return ret;
}

int CBoneRateFile::ParseRatLine()
{
	int ret;
	int ldleng;
	int pos = 0;

	int lineleng;
	lineleng = (int)strlen( m_line );

	int isend;

	RATELEMInit( 0 );//!!!!


// 番号が、 , で区切られている。

	int* dstptrlist[3];
	dstptrlist[0] = &(curratelem.vertno);
	dstptrlist[1] = &(curratelem.boneno1);
	dstptrlist[2] = &(curratelem.boneno2);

	float* dstfloat;
	dstfloat = &(curratelem.bonerate1);

	int getno;
	for( getno = 0; getno < 4; getno++ ){
		if( pos < lineleng ){
			DbgOut( "BoneRateFile : ParseRatLine : pos overflow error %d!!! return\n", getno );
			_ASSERT( 0 );
			return 1;
		}

		if( getno < 3 ){
			ret = GetIntFromLine( lineleng, pos, &ldleng, &isend, dstptrlist[getno] );
			if( ret ){
				DbgOut( "BoneRateFile : ParseRatLine : GetIntFromLine error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( isend != 0 ){
				DbgOut( "boneratefile : ParseRatLine : getint %d data is too short error !!!\n", getno );
				_ASSERT( 0 );
				return 1;
			}

		}else{
			ret = GetFloatFromLine( lineleng, pos, &ldleng, &isend, dstfloat );
			if( ret ){
				DbgOut( "BoneRateFile : ParseRatLine : GetFloatFromLine error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( isend != 0 ){
				DbgOut( "boneratefile : ParseRatLine : get float data is too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		if( isend != 0 ){
			break;
		}
		pos += ldleng;

	}


	curratelem.boneno1 += m_firstseri;
	curratelem.boneno2 += m_firstseri;


////////// boneelemのセット
	/***
	CShdElem* selem;
	selem = (*m_lpsh)( curratelem.serialno );
	if( !selem ){
		DbgOut( "boneratefile : ParseRatLine : selem NULL error %d !!!\n", curratelem.serialno );
		_ASSERT( 0 );
		return 1;
	}

	int parentflag = 0;//!!!!!!!

	ret = selem->SetBoneRate( curratelem.boneno1, curratelem.boneno2, curratelem.bonerate1, parentflag );
	if( ret ){
		DbgOut( "boneratefile : ParseRatLine : selem SetBoneRate error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/

	return 0;	
}


int CBoneRateFile::GetIntFromLine( int lineleng, int pos, int* lengptr, int* isendptr, int* intptr )
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
		DbgOut( "boneratefile : GetIntData : dataleng toot long error %d!!!\n", dataleng );
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

int CBoneRateFile::GetFloatFromLine( int lineleng, int pos, int* lengptr, int* isendptr, float* floatptr )
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
		DbgOut( "boneratefile : GetFloatData : dataleng toot long error %d!!!\n", dataleng );
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




