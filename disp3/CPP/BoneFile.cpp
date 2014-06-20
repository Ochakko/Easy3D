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
#include <bvhelem.h>

#include "BoneFile.h"


#include <crtdbg.h>

enum {
	MODE_CHIL,
	MODE_BRO,
	MODE_MAX
};

static char fileheaderstr1[256] = "BoneFile : type mixed : ver1001\r\n";
static char fileheaderstr2[256] = "BoneFile : type separated : ver1001\r\n";

static char namestartstr[256] = "NAMEPART_START\r\n";
static char nameendstr[256] = "NAMEPART_END\r\n";
static char treestartstr[256] = "TREEPART_START\r\n";
static char treeendstr[256] = "TREEPART_END\r\n";

static char relativestr[256] = "RELATIVE_BONENO_MODE\r\n";

enum {
	BF_LOADNAME,
	BF_LOADTREE,
	BF_LOADNUT,
	BF_LOADFINISH,
	BF_MAX
};



CBoneFile::CBoneFile()
{

	InitParams();

}
CBoneFile::~CBoneFile()
{
	if( bonbuf.buf ){
		free( bonbuf.buf );
		bonbuf.buf = 0;
	}
	if( m_bonrel ){
		free( m_bonrel );
		m_bonrel = 0;
	}

}

int CBoneFile::InitParams()
{
	m_hfile = INVALID_HANDLE_VALUE;
	m_lpth = 0;
	m_lpsh = 0;
	m_lpmh = 0;

	bonbuf.buf = 0;
	bonbuf.bufleng = 0;
	bonbuf.pos = 0;
	bonbuf.isend = 0;

	BFELEMInit();

	relativemode = 0;
	m_firstparentno = 0;

	m_mqoflag = 0;
	m_bonrel = 0;
	return 0;
}

int CBoneFile::WriteBoneFileFromBVH( char* filename, int srcoffset, CBVHElem* srcbe )
{
	int ret = 0;


	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "BoneFile : WriteBoneFileFromBVH : file open error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto wbfexit;
	}



	Write2File( fileheaderstr2 );
	Write2File( relativestr );

	if( srcbe ){

		Write2File( "\r\n%s", namestartstr );
		WriteBVHNameReq( srcbe, srcoffset );
		Write2File( "%s", nameendstr );


		Write2File( "\r\n\r\n%s", treestartstr );
		WriteBVHTreeReq( srcbe, srcoffset );
		Write2File( "\r\n%s", treeendstr );

	}

	goto wbfexit;
wbfexit:
	if( m_hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hfile );
		SetEndOfFile( m_hfile );
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	return ret;

}


int CBoneFile::WriteBoneFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, int filetype, int relativeflag, int mqoflag )
{
	int ret = 0;

	if( !lpth || !lpsh ){
		DbgOut( "BoneFile : WriteBoneFile : handler NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_lpth = lpth;
	m_lpsh = lpsh;
	m_mqoflag = mqoflag;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "BoneFile : WriteBoneFile : file open error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto wbfexit;
	}

	if( relativeflag == 1 ){
		ret = GetFirstJointNo( &relativemode );
		if( ret ){
			DbgOut( "bonefile : WriteBoneFile : GetFirstJointNo error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto wbfexit;
		}
	}else{
		relativemode = 0;
	}

	ret = MakeBonRel();
	if( ret ){
		DbgOut( "bonefile : WriteBoneFile : MakeBonRel error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( filetype == 0 ){
		Write2File( fileheaderstr1 );

		if( relativemode != 0 )
			Write2File( relativestr );

		WriteElemReq( 1, 0, MODE_BRO, 0 );
	}else{
		Write2File( fileheaderstr2 );
	
		if( relativemode != 0 )
			Write2File( relativestr );


		Write2File( "\r\n%s", namestartstr );
		ret = WriteElemName();
		if( ret ){
			DbgOut( "BoneFile : WriteBoneFile : WriteElemName error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto wbfexit;
		}
		Write2File( "%s", nameendstr );


		Write2File( "\r\n\r\n%s", treestartstr );
		WriteElemReq( 1, 0, MODE_BRO, 1 );
		Write2File( "\r\n%s", treeendstr );
	}

	goto wbfexit;
wbfexit:
	if( m_hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hfile );
		SetEndOfFile( m_hfile );
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	return ret;
}

void CBoneFile::WriteElemReq( int firstflag, int srcseri, int addmode, int filetype )
{
	int ret;
	//static int isfirst = 1;

	char* shdname;
	int parentno;
	CShdElem* curselem;
	CShdElem* parselem;

	char parseristr[6];
	char seristr[6];

	curselem = (*m_lpsh)( srcseri );
		
	// joint で、かつ、bonenumが１以上のものだけTreeにAddする。
	//----> bonenum が０のものもAddするように変更！！！！03/1/26

	if( curselem->IsJoint() && (curselem->type != SHDMORPH)){
		parselem = m_lpsh->FindUpperJoint( curselem, 1 );
		if( parselem ){
			parentno = parselem->serialno;
		}else{
			parselem = curselem->parent;
			if( parselem )
				parentno = parselem->serialno;
			else
				parentno = 1;//!!!!!!!!!!!!!!!!2004/4/19
				//parentno = 0;
		}
		shdname = (*m_lpth)( srcseri )->name;

		if( (firstflag == 1) || (addmode == MODE_BRO) ){
			//行の先頭なので、改行と、parentのserinoを書く
			//ret = Conv4Digits( parentno - relativemode, parseristr );//!!!!!!!!! 2004/4/19
			if( m_mqoflag == 1 ){
				ret = Conv4Digits( (m_bonrel + parentno)->contrel, parseristr );
			}else{
				if( relativemode == 0 ){
					ret = Conv4Digits( parentno, parseristr );
				}else{
					ret = Conv4Digits( (m_bonrel + parentno)->relseri, parseristr );
				}
			}
			if( ret ){
				DbgOut( "BoneFile : WriteElemReq : Conv4Digits error !!!\n" );
				_ASSERT( 0 );
				return;
			}

			if( filetype == 0 ){
				ret = Write2File( "\r\n%s:,", parseristr );
				if( ret ){
					DbgOut( "BoneFile : WriteElemReq : Write2File error !!!\n" );
					_ASSERT( 0 );
					return;
				}
			}else{
				ret = Write2File( "\r\n%s,", parseristr );
				if( ret ){
					DbgOut( "BoneFile : WriteElemReq : Write2File error !!!\n" );
					_ASSERT( 0 );
					return;
				}
			}
//DbgOut( "Bonefile : WriteElemReq : isfirst %d, addmode %d : parserisstr %s, shdnamae %s\n",
//	isfirst, addmode, parseristr, shdname );

			firstflag = 0;
		}

		//seri:名前,　を書く
//		ret = Conv4Digits( srcseri - relativemode, seristr );//!!!!!!!!!!!!!! 2004/4/19
		if( m_mqoflag == 1 ){
			ret = Conv4Digits( (m_bonrel + srcseri)->contrel, seristr );
		}else{
			ret = Conv4Digits( (m_bonrel + srcseri)->relseri, seristr );
		}
		if( ret ){
			DbgOut( "BoneFile : WriteElemReq : Conv4Digits error !!!\n" );
			_ASSERT( 0 );
			return;
		}

		if( filetype == 0 ){
			ret = Write2File( "%s:%s,", seristr, shdname );
			if( ret ){
				DbgOut( "BoneFile : WriteElemReq : Write2File error !!!\n" );
				_ASSERT( 0 );
				return;
			}
		}else{
			ret = Write2File( "%s,", seristr, shdname );
			if( ret ){
				DbgOut( "BoneFile : WriteElemReq : Write2File error !!!\n" );
				_ASSERT( 0 );
				return;
			}
		}

	}

	///////////
	CShdElem* childselem;
	int childno;
	//childselem = curselem->child;
	if( curselem->child ){
		childselem = m_lpsh->GetValidChild( curselem->child );
		if( childselem ){
			childno = childselem->serialno;
			WriteElemReq( firstflag, childno, MODE_CHIL, filetype );
		}
	}


	///////////
	CShdElem* broselem;
	int brono;
	//broselem = curselem->brother;
	if( curselem->brother ){
		broselem = m_lpsh->GetValidBrother( curselem->brother );
		if( broselem ){
			brono = broselem->serialno;
			WriteElemReq( firstflag, brono, MODE_BRO, filetype );
		}
	}

	return;
}

int CBoneFile::Write2File( char* lpFormat, ... )
{
	if( !m_hfile ){
		//_ASSERT( 0 );
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[BFLINELEN];
			
	ZeroMemory( outchar, BFLINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, BFLINELEN, lpFormat, Marker );
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

int CBoneFile::Conv4Digits( int srcseri, char* dstptr )
{
	//if( (srcseri < 0) || (srcseri > 9999) ){
	if( srcseri > 9999 ){
		DbgOut( "motparamdlg : Conv5Digits : srcseri range error !!!\n" );
		_ASSERT( 0 );
		*dstptr = 0;
		return 1;
	}
	
	int minusflag;
	if( srcseri < 0 ){
		minusflag = 1;
		srcseri *= -1;//!!!!!!!!
	}else{
		minusflag = 0;
	}

	int zerohead;
	if( srcseri < 10 ){
		zerohead = 3;
	}else if( srcseri < 100 ){
		zerohead = 2;
	}else if( srcseri < 1000 ){
		zerohead = 1;
	}else{
		zerohead = 0;
	}

	*dstptr = 0;//!!!!

	if( minusflag == 1 ){
		strcat_s( dstptr, 6, "-" );//!!!!!!!!!
	}

	int headno;
	for( headno = 0; headno < zerohead; headno++ ){
		strcat_s( dstptr, 6, "0" );
	}

	char numstr[6];
	sprintf_s( numstr, 6, "%d", srcseri );

	strcat_s( dstptr, 6, numstr );

	return 0;
}

///////////////////

int CBoneFile::LoadBoneFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh )
{
	int ret = 0;

	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "BoneFile : LoadBoneFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
	}	

	ret = InitBONBUF();
	if( ret ){
		DbgOut( "BoneFile : LoadBoneFile : InitBONBUF error !!!\n" );
		_ASSERT( 0 );
		goto ldbfexit;
	}
	
	int filetype;
	ret = GetFileVersion( &filetype );
	if( ret ){
		DbgOut( "BoneFile : LoadBoneFile : GetFileVersion error !!!\n" );
		_ASSERT( 0 );
		goto ldbfexit;
	}

	ret = CheckRelativeMode();
	if( ret ){
		DbgOut( "BoneFile : LoadBoneFile : CheckRelativeMode error !!!\n" );
		_ASSERT( 0 );
		goto ldbfexit;
	}

	if( filetype == 0 ){

		ret = MakeAllTreeFlat();
		if( ret ){
			DbgOut( "BoneFile : LoadBoneFile : MakeAllTreeFlat error !!!\n" );
			_ASSERT( 0 );
			goto ldbfexit;
		}

		while( 1 ){
			int ldleng;
			ret = LoadLine( &ldleng );
			if( ret ){
				DbgOut( "BoneFile : LoadBoneFile : LoadLine error !!!\n" );
				_ASSERT( 0 );
				goto ldbfexit;
			}

			if( bonbuf.isend != 0 ){
				break;
			}

			ret = ParseLine();
			if( ret ){
				DbgOut( "BoneFile : LoadBoneFile : ParseLine error !!!\n" );
				_ASSERT( 0 );
				goto ldbfexit;
			}

		}
	}else{


		int loadmode = BF_LOADNUT;
		int loadflag = 1;
		while( loadflag ){
			switch( loadmode ){
			case BF_LOADNAME:
				ret = LoadNamePart( &loadmode );
				if( ret ){
					DbgOut( "BoneFile : LoadBoneFile : LoadNamePart error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case BF_LOADTREE:


				ret = MakeAllTreeFlat();
				if( ret ){
					DbgOut( "BoneFile : LoadBoneFile : MakeAllTreeFlat error !!!\n" );
					_ASSERT( 0 );
					goto ldbfexit;
				}

				ret = LoadTreePart( &loadmode );
				if( ret ){
					DbgOut( "BoneFile : LoadBoneFile : LoadTreePart error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case BF_LOADFINISH:
				loadflag = 0;
				break;
			case BF_LOADNUT:
				ret = GetLoadMode( &loadmode );
				if( ret ){
					DbgOut( "BoneFile : LoadBoneFile : GetLoadMode error !!!\n" );
					_ASSERT( 0 );
					loadflag = 0;
				}
				break;
			default:
				DbgOut( "BoneFile : LoadBoneFile : loadmode error !!!\n" );
				_ASSERT( 0 );
				loadflag = 0;
				break;
			}
		}

	}

	goto ldbfexit;
ldbfexit:

	m_lpmh->CopyChainFromShd( m_lpsh );//!!!!!!!!!!
	m_lpmh->CalcDepth();


	if( m_hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( bonbuf.buf ){
		free( bonbuf.buf );
		bonbuf.buf = 0;

		bonbuf.bufleng = 0;
		bonbuf.pos = 0;
		bonbuf.isend = 0;
	}

	return ret;
}

int CBoneFile::InitBONBUF()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "BoneFile : InitBONBUF :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "BoneFile : InitBONBUF :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "BoneFile : InitBONBUF :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "BoneFile : InitBONBUF :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}
	
	bonbuf.buf = newbuf;
	bonbuf.bufleng = bufleng;
	bonbuf.pos = 0;
	bonbuf.isend = 0;

	relativemode = 0;

	return 0;
}

int CBoneFile::GetFileVersion( int* typeptr )
{
	int ldleng;
	int ret;

	ret = LoadLine( &ldleng );
	if( ret ){
		DbgOut( "BoneFile : GetFileVersion : LoadLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int headerlen1;
	headerlen1 = (int)strlen( fileheaderstr1 );

	int headerlen2;
	headerlen2 = (int)strlen( fileheaderstr2 );

	int cmp1, cmp2;
	cmp1 = strncmp( fileheaderstr1, m_line, headerlen1 );
	if( cmp1 == 0 ){
		*typeptr = 0;
	}else{

		cmp2 = strncmp( fileheaderstr2, m_line, headerlen2 );
		if( cmp2 == 0 ){
			*typeptr = 1;
		}else{
			// type 1 でも　2 でもないときは、エラーを返す。
			DbgOut( "BoneFile : GetFileVersion : File type error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CBoneFile::LoadLine( int* lenptr )
{
	int stepnum = 0;
	unsigned int curpos = bonbuf.pos;
	char curc;

	while( curpos < bonbuf.bufleng ){
		curc = *( bonbuf.buf + curpos );	

		if( curc == '\n' ){
			stepnum++;
			break;
		}
		stepnum++;
		curpos++;
	}

	int ret;
	if( stepnum >= BFLINELEN ){
		// 行が長すぎ
		DbgOut( "BoneFile : LoadLine : stepnum overflow error !!!\n" );
		_ASSERT( 0 );
		m_line[0] = 0;
		ret = 1;
	}else if( stepnum > 0 ){
		// 正常
		strncpy_s( m_line, BFLINELEN, bonbuf.buf + bonbuf.pos, stepnum );
		m_line[ stepnum ] = 0;
		ret = 0;
	}else{
		// ファイルの終わり
		m_line[0] = 0;
		bonbuf.isend = 1;
		ret = 0;//!!!!!!!
	}

	*lenptr = stepnum;
	bonbuf.pos += stepnum;

	return ret;
}

int CBoneFile::ParseLine()
{
	int ret;
	int ldleng;
	int pos = 0;

	int lineleng;
	lineleng = (int)strlen( m_line );

	int isend;

	BFELEMInit();//!!!!

// 行の最初の要素は、シリアル番号のみ
	ret = GetSerialFromLine( lineleng, pos, &ldleng, &isend );
	if( ret ){
		DbgOut( "BoneFile : ParseLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( isend != 0 ){
		return 0;
	}
	pos += ldleng;


//DbgOut( "Bonefile : ParseLine : parent serialno %d\n", curbfelem.serialno );

	Copy2BefBFELEM();//!!!!
	

// シリアル番号：要素名　が、 , で区切られている。

	while( pos < lineleng ){
		//isend は、要素の読み込み前に、終端０が現れたときに１にセットされる。

		ret = GetSerialFromLine( lineleng, pos, &ldleng, &isend );
		if( ret ){
			DbgOut( "BoneFile : ParseLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( isend != 0 ){
			break;
		}
		pos += ldleng;

//DbgOut( "BoneFile : ParseLine : curbfelem serialno %d\n", curbfelem.serialno );

		ret = GetNameFromLine( lineleng, pos, &ldleng, &isend );
		if( ret ){
			DbgOut( "BoneFile : ParseLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( isend != 0 ){
			break;
		}
		pos += ldleng;

//DbgOut( "BoneFile : ParseLine : curbfelem name %s\n", curbfelem.name );

		//// ここで、Treeを設定する
		CShdElem* curselem;
		curselem = (*m_lpsh)( curbfelem.serialno );
		if( !curselem ){
			DbgOut( "BoneFile : ParseLine : curselem NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = curselem->LeaveFromChain();
		if( ret ){
			DbgOut( "BoneFile : ParseLine : selem LeaveFromChain error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* befselem;
		if( befbfelem.serialno != 0 ){
			befselem = (*m_lpsh)( befbfelem.serialno );
		}else{
			// parent のserialno が０のときは、１の子供にする。！！！！！！
			befselem = (*m_lpsh)( 1 );
		}
		ret = curselem->PutToChain( befselem, 1 );
		if( ret ){
			DbgOut( "BoneFile : ParseLine : selem PutToChain error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}

		ret = m_lpsh->CalcDepth();
		if( ret ){
			DbgOut( "BoneFile : ParseLine : lpsh CalcDepth error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}

		// 名前を設定する
		CTreeElem2* telem;
		telem = (*m_lpth)( curbfelem.serialno );
		if( telem ){
			ret = telem->SetName( curbfelem.name );
			if( ret ){
				DbgOut( "BoneFile : ParseLine : telem SetName error !!!\n ");
				_ASSERT( 0 );
				return 1;
			}
		}

		////
		//befbfelem = curbfelem;
		Copy2BefBFELEM();


	}

	return 0;
}

int CBoneFile::ParseNameLine()
{
	int ret;
	int ldleng;
	int pos = 0;

	int lineleng;
	lineleng = (int)strlen( m_line );

	int isend;

	BFELEMInit();//!!!!
	

// シリアル番号：要素名　が、 1行になっている。

		//isend は、要素の読み込み前に、終端０が現れたときに１にセットされる。

	ret = GetSerialFromLine( lineleng, pos, &ldleng, &isend );
	if( ret ){
		DbgOut( "BoneFile : ParseNameLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( isend != 0 ){
		return 0;
	}
	pos += ldleng;

//DbgOut( "BoneFile : ParseNameLine : curbfelem serialno %d\n", curbfelem.serialno );

	ret = GetNameFromLine( lineleng, pos, &ldleng, &isend );
	if( ret ){
		DbgOut( "BoneFile : ParseNameLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( isend != 0 ){
		return 0;
	}
	pos += ldleng;

//DbgOut( "BoneFile : ParseNameLine : curbfelem name %s\n", curbfelem.name );

	// 名前を設定する
	CTreeElem2* telem;
	telem = (*m_lpth)( curbfelem.serialno );
	if( telem ){
		ret = telem->SetName( curbfelem.name );
		if( ret ){
			DbgOut( "BoneFile : ParseNameLine : telem SetName error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}
	}else{
		DbgOut( "BoneFile : ParseNameLine : curbfelem serialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBoneFile::ParseTreeLine()
{
	int ret;
	int ldleng;
	int pos = 0;

	int lineleng;
	lineleng = (int)strlen( m_line );

	int isend;

	BFELEMInit();//!!!!

// 行の最初の要素
	ret = GetSerialFromLine( lineleng, pos, &ldleng, &isend, 0 );
	if( ret ){
		DbgOut( "BoneFile : ParseTreeLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( isend != 0 ){
		return 0;
	}
	pos += ldleng;


//DbgOut( "Bonefile : ParseLine : parent serialno %d\n", curbfelem.serialno );

	Copy2BefBFELEM();//!!!!
	

// シリアル番号が、 , で区切られている。

	while( pos < lineleng ){
		//isend は、要素の読み込み前に、終端０が現れたときに１にセットされる。

		ret = GetSerialFromLine( lineleng, pos, &ldleng, &isend, 0 );
		if( ret ){
			DbgOut( "BoneFile : ParseTreeLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( isend != 0 ){
			break;
		}
		pos += ldleng;

//DbgOut( "BoneFile : ParseLine : curbfelem serialno %d\n", curbfelem.serialno );

		//// ここで、Treeを設定する
		CShdElem* curselem;
		curselem = (*m_lpsh)( curbfelem.serialno );
		if( !curselem ){
			DbgOut( "BoneFile : ParseTreeLine : curselem NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = curselem->LeaveFromChain();
		if( ret ){
			DbgOut( "BoneFile : ParseTreeLine : selem LeaveFromChain error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* befselem;
		if( befbfelem.serialno != 0 ){
			befselem = (*m_lpsh)( befbfelem.serialno );
		}else{
			// parent のserialno が０のときは、１の子供にする。！！！！！！
			befselem = (*m_lpsh)( 1 );
		}
		ret = curselem->PutToChain( befselem, 1 );
		if( ret ){
			DbgOut( "BoneFile : ParseTreeLine : selem PutToChain error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}

		ret = m_lpsh->CalcDepth();
		if( ret ){
			DbgOut( "BoneFile : ParseTreeLine : lpsh CalcDepth error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}

		////
		//befbfelem = curbfelem;
		Copy2BefBFELEM();


	}

	return 0;
}

int CBoneFile::BFELEMInit()
{
	befbfelem.serialno = 0;
	befbfelem.name[0] = 0;

	curbfelem.serialno = 0;
	curbfelem.name[0] = 0;

	return 0;
}
int CBoneFile::GetSerialFromLine( int lineleng, int pos, int* lengptr, int* isendptr, int collonflag )
{
	//isend は、要素の読み込み前に、終端０が現れたときに１にセットさする。
	
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

// 4桁分、データがあるかチェック
	int startpos;
	startpos = pos + stepnum;
	if( startpos + 3 >= lineleng ){
		*isendptr = 1;
		return 0;
	}
	
// 4桁分、数字が続いているかチェック
	int digino;
	for( digino = 0; digino < 4; digino++ ){
		curc = m_line[startpos + digino];
		if( isdigit( curc ) == 0 ){
			DbgOut( "BoneFile : GetSerialFromLine : not digit error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	
	if( collonflag == 1 ){
// 5桁目がコロンかどうかチェック
		char mark;
		mark = m_line[ startpos + 4 ];
		if( mark != ':' ){
			DbgOut( "BoneFile : GetSerialFromLine : mark not found error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

// 文字列をシリアル番号に変換。
	char seristr[5];
	strncpy_s( seristr, 5, m_line + startpos, 4 );
	seristr[4] = 0;

	
	if( minusflag == 1 ){
		curbfelem.serialno = -atoi( seristr ) + relativemode;//!!!!!!!!!!! 2004/4/19
	}else{
		curbfelem.serialno = atoi( seristr ) + relativemode;// !!!!! 2004/4/19
	}

	if( (curbfelem.serialno <= 0) || (curbfelem.serialno >= m_lpth->s2e_leng) ){
		DbgOut( "bonefile : GetSerialFromLine : serialno error %d %d %d !!!\n", minusflag, atoi( seristr ), relativemode );
		_ASSERT( 0 );
		return 1;
	}

	if( collonflag == 1 )
		*lengptr = stepnum + 5;
	else
		*lengptr = stepnum + 4;

	*isendptr = 0;

	return 0;
}
int CBoneFile::GetNameFromLine( int lineleng, int pos, int* lengptr, int* isendptr )
{
	//isend は、要素の読み込み前に、終端０が現れたときに１にセットさする。

	char curc;
	int stepnum = 0;
	int isfind = 0;
	while( (pos + stepnum) < lineleng ){
		curc = m_line[ pos + stepnum ];
		if( (curc == ',') || (curc == '\r') ){
			isfind = 1;
			break;
		}
		stepnum++;
	}

	// , が見つからなかった場合は、行末までを名前とする。

	strncpy_s( curbfelem.name, BFLINELEN, m_line + pos, stepnum );
	*( curbfelem.name + stepnum ) = 0;

	*lengptr = stepnum;
	*isendptr = 0;
	return 0;
}

int CBoneFile::Copy2BefBFELEM()
{
	befbfelem.serialno = curbfelem.serialno;

	strcpy_s( befbfelem.name, BFLINELEN, curbfelem.name );

	return 0;
}


int CBoneFile::WriteElemName()
{
	int ret;
	CShdElem* curselem;
	int elemno;
	int findnum = 0;

	char seristr[6];
	char* elemname;

	for( elemno = 1; elemno < m_lpsh->s2shd_leng; elemno++ ){
		curselem = (*m_lpsh)( elemno );
		_ASSERT( curselem );

		if( curselem->IsJoint() && (curselem->type != SHDMORPH)){
			
			//ret = Conv4Digits( elemno - relativemode, seristr );//!!!!!!!!!!!!!
			if( m_mqoflag == 1 ){
				ret = Conv4Digits( (m_bonrel + elemno)->contrel, seristr );
			}else{
				ret = Conv4Digits( (m_bonrel + elemno)->relseri, seristr );
			}
			if( ret ){
				DbgOut( "BoneFile : WriteElemName : Conv4Digits error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			_ASSERT( (*m_lpth)( elemno ) );
			elemname = (*m_lpth)( elemno )->name;

			Write2File( "%s:%s\r\n", seristr, elemname );

			findnum++;
		}
	}

	return 0;
}

int CBoneFile::GetLoadMode( int* modeptr )
{
	int ret;
	int ldleng;

	ret = LoadLine( &ldleng );
	if( ret ){
		DbgOut( "BoneFile : GetLoadMode : LoadLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( bonbuf.isend != 0 ){
		*modeptr = BF_LOADFINISH;
		return 0;
	}
	
	int cmp1, cmp2;
	cmp1 = strcmp( m_line, namestartstr );
	if( cmp1 == 0 ){
		*modeptr = BF_LOADNAME;
		return 0;
	}else{
		cmp2 = strcmp( m_line, treestartstr );
		if( cmp2 == 0 ){
			*modeptr = BF_LOADTREE;
			return 0;
		}else{
			*modeptr = BF_LOADNUT;
			return 0;
		}
	}


	return 0;
}

int CBoneFile::LoadNamePart( int* modeptr )
{
	int ret;

	while( 1 ){
		int ldleng;
		ret = LoadLine( &ldleng );
		if( ret ){
			DbgOut( "BoneFile : LoadNamePart : LoadLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( bonbuf.isend != 0 ){
			*modeptr = BF_LOADFINISH;
			break;
		}

		int cmp;
		cmp = strcmp( m_line, nameendstr );
		if( cmp == 0 ){
			*modeptr = BF_LOADNUT;
			break;
		}

		ret = ParseNameLine();
		if( ret ){
			DbgOut( "BoneFile : LoadNamePart : ParseNameLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int CBoneFile::LoadTreePart( int* modeptr )
{
	int ret;

	while( 1 ){
		int ldleng;
		ret = LoadLine( &ldleng );
		if( ret ){
			DbgOut( "BoneFile : LoadTreePart : LoadLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( bonbuf.isend != 0 ){
			*modeptr = BF_LOADFINISH;
			break;
		}

		int cmp;
		cmp = strcmp( m_line, treeendstr );
		if( cmp == 0 ){
			*modeptr = BF_LOADNUT;
			break;
		}

		ret = ParseTreeLine();
		if( ret ){
			DbgOut( "BoneFile : LoadTreePart : ParseTreeLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int CBoneFile::CheckRelativeMode()
{
	if( !(bonbuf.buf) ){
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int firstjointno;
	ret = GetFirstJointNo( &firstjointno );
	if( ret ){
		DbgOut( "bonefile : CheckRelativeMode : GetFirstJointNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//SHDPARTの中で、一番serialの大きいものを、m_firstparentnoにセット
	m_firstparentno = 1;//!!!!!
	int seri;
	for( seri = 0; seri < m_lpsh->s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*m_lpsh)( seri );
		if( selem->type == SHDPART ){
			if( seri > m_firstparentno ){
				m_firstparentno = seri;
			}
		}
	}

	char* findptr;
	findptr = strstr( bonbuf.buf, relativestr );
	if( findptr ){
		//boneの一番若いserial番号を、relativemodeに代入する。
		relativemode = firstjointno;

	}else{
		relativemode = 0;
	}

	return 0;
}

int CBoneFile::GetFirstJointNo( int* firstno )
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


int CBoneFile::MakeAllTreeFlat()
{

	int ret;

	if( (m_firstparentno <= 0) || (m_firstparentno >= m_lpsh->s2shd_leng) ){
		DbgOut( "bonefile : MakeAllTreeFlat : firstparentno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// newparentのtreeを初期化
	CShdElem* newparent;
	newparent = (*m_lpsh)( m_firstparentno );
	if( !newparent ){
		DbgOut( "bonefile : MakeAllTreeFlat : newparent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	//newparent->parent = 
//newparent->child = 0;
	//newparent->brother = 0;
	//newparent->sister = 0;



	int seri;
	CShdElem* curselem;

// jointのTreeを０にリセット
	for( seri = 1; seri < m_lpsh->s2shd_leng; seri++ ){
		curselem = (*m_lpsh)( seri );
		if( !curselem ){
			DbgOut( "bonefile : MakeAllTreeFlag : curselem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( curselem->IsJoint() ){
			curselem->parent = 0;
			curselem->child = 0;
			curselem->brother = 0;
			curselem->sister = 0;
		}else{
			//「追加ジョイント」　パート
			//if( (seri > m_firstparentno) && (curselem->type == SHDPART) ){
			if( curselem->type == SHDPART ){
				CShdElem* chkchild;
				chkchild = curselem->child;

				if( chkchild && (chkchild->IsJoint()) ){
					curselem->child = 0;
				}
			}
		}
	}


// 全てのjointをnewparentの子供にセットする
	for( seri = 1; seri < m_lpsh->s2shd_leng; seri++ ){
		curselem = (*m_lpsh)( seri );
		if( !curselem ){
			DbgOut( "bonefile : MakeAllTreeFlag : curselem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( curselem->IsJoint() ){
			ret = curselem->PutToChain( newparent, 1 );
			if( ret ){
				DbgOut( "bonefile : MakeAllTreeFlat : PutToChain error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}


void CBoneFile::WriteBVHNameReq( CBVHElem* srcbe, int srcoffset )
{
	int ret;

	if( srcbe ){
		char nostr[6];
		nostr[0] = 0;
		ret = Conv4Digits( srcoffset + srcbe->mqono, nostr );
		if( ret ){
			DbgOut( "bonefile : WriteBVHNameReq : Conv4Digits error !!!\n" );
			_ASSERT( 0 );
			return;
		}
		
		char sitestr[10] = "Site";
		int cmp;
		cmp = strncmp( srcbe->name, sitestr, (int)strlen( sitestr ) );
		if( cmp == 0 ){
			sprintf_s( srcbe->name, PATH_LENG, "Site%d", srcbe->mqono );
		}

		ret = Write2File( "%s:%s\r\n", nostr, srcbe->name );
		if( ret ){
			DbgOut( "bonefile : WriteBVHNameReq : Write2File error !!!\n" );
			_ASSERT( 0 );
			return;
		}

	/////
		if( srcbe->child ){
			WriteBVHNameReq( srcbe->child, srcoffset );
		}
		if( srcbe->brother ){
			WriteBVHNameReq( srcbe->brother, srcoffset );
		}

	}

}

/***
void CBoneFile::WriteBVHTreeReq( CBVHElem* srcbe, int srcoffset )
{
	int ret;

	char nostr[6];
	nostr[0] = 0;
	ret = Conv4Digits( srcoffset + srcbe->mqono, nostr );
	if( ret ){
		DbgOut( "bonefile : WriteBVHTreeReq : Conv4Digits error !!!\n" );
		_ASSERT( 0 );
		return;
	}
	Write2File( "%s,", nostr );

///////////
	if( srcbe->child ){
		WriteBVHTreeReq( srcbe->child, srcoffset );
	}else{
		Write2File( "\r\n" );
	}
	if( srcbe->brother ){
		if( srcbe->parent ){
			nostr[0] = 0;
			ret = Conv4Digits( srcoffset + srcbe->parent->mqono, nostr );
			if( ret ){
				DbgOut( "bonefile : WriteBVHTreeReq : Conv4Digits error !!!\n" );
				_ASSERT( 0 );
				return;
			}
			Write2File( "%s,", nostr );
		}

		WriteBVHTreeReq( srcbe->brother, srcoffset );
	}
}
***/
void CBoneFile::WriteBVHTreeReq( CBVHElem* srcbe, int srcoffset )
{
	int ret;

	char nostr[6];
	nostr[0] = 0;
	ret = Conv4Digits( srcoffset + srcbe->mqono, nostr );
	if( ret ){
		DbgOut( "bonefile : WriteBVHTreeReq : Conv4Digits error !!!\n" );
		_ASSERT( 0 );
		return;
	}
	Write2File( "%s,", nostr );

///////////
	if( srcbe->child ){
		WriteBVHTreeReq( srcbe->child, srcoffset );
	}else{
		Write2File( "\r\n" );
	}
	if( srcbe->brother ){
		if( srcbe->parent ){
			nostr[0] = 0;
			ret = Conv4Digits( srcoffset + srcbe->parent->mqono, nostr );
			if( ret ){
				DbgOut( "bonefile : WriteBVHTreeReq : Conv4Digits error !!!\n" );
				_ASSERT( 0 );
				return;
			}
			Write2File( "%s,", nostr );
		}

		WriteBVHTreeReq( srcbe->brother, srcoffset );
	}
}

int CBoneFile::MakeBonRel()
{
	m_bonrel = (BONREL*)malloc( sizeof( BONREL ) * m_lpsh->s2shd_leng );
	if( !m_bonrel ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_bonrel, sizeof( BONREL ) * m_lpsh->s2shd_leng );

	int setcnt = 0;
	int seri;
	for( seri = 0; seri < m_lpsh->s2shd_leng; seri++ ){
		BONREL* curbr = m_bonrel + seri;

		curbr->seri = seri;

		CShdElem* selem;
		selem = (*m_lpsh)( seri );

		if( selem->IsJoint() ){
			curbr->relseri = seri - relativemode;
			curbr->contrel = setcnt;
			setcnt++;
		}else{
			if( relativemode == 0 ){
				curbr->relseri = 1;//絶対値モード
			}else{
				curbr->relseri = -1;//相対値モード
			}
			curbr->contrel = -1;
		}
	}

	return 0;
}