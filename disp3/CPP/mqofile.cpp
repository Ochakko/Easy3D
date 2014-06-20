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
#include <shdelem.h>
#include <part.h>

#include <crtdbg.h>

#include <mqofile.h>
#include <mqomaterial.h>
#include <mqoobject.h>

#include <mqoface.h>
#include <bvhelem.h>

#include <morph.h>

//#include "MQODlg.h"


// global
extern int g_toon1matcnt;



#define BUFBLOCKLEN	2048

typedef enum tag_linekind {
	LINE_NORMAL,
	LINE_STARTCHUNK,
	LINE_ENDCHUNK,
	LINE_MAX
} LINEKIND;


CMQOFile::CMQOFile( HWND apphwnd )
{

	InitLoadParams( apphwnd );

}
CMQOFile::~CMQOFile()
{
	if( mqobuf.buf ){
		free( mqobuf.buf );
		mqobuf.buf = 0;
	}

	CMQOObject* nextobj;
	CMQOObject* delobj = objhead;
	while( delobj ){
		nextobj = delobj->next;
		delete delobj;
		delobj = nextobj;
	}
	objhead = 0;

}

int CMQOFile::LoadMQOFileFromBuf( float multiple, char* bufptr, int bufsize, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, int groundflag, int bonetype, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot )
{

	int ret;

	mqobuf.buf = (unsigned char*)realloc( mqobuf.buf, bufsize );
	if( !mqobuf.buf ){
		DbgOut( "MQOFile : LoadMQOFileFromBuf : buf alloc error !!!" );
		_ASSERT( 0 );
		return 1;
	}

	memcpy( mqobuf.buf, bufptr, sizeof( char ) * bufsize );
	mqobuf.bufleng = bufsize;
	
	
	
	ret = LoadMQOFile_aft( multiple, srclpth, srclpsh, srclpmh, offset, groundflag, bonetype, adjustuvflag, offsetpos, rot );
	if( ret ){
		DbgOut( "mqofile : LoadMQOFile : LoadMQOFile_aft error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMQOFile::LoadMQOFile_aft( float multiple, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, int groundflag, int bonetype, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot )
{

	g_toon1matcnt = 0;//!!!!!!!!!!!!!!!!!!!!


	int ret = 0;
	int no_pm2orgno = 0;

	if( !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "mqofile : LoadMQOFile : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_bonetype = bonetype;
	m_multiple = multiple;
	m_groundflag = groundflag;

	m_adjustuvflag = adjustuvflag;

	//m_offsetpos = offsetpos;

	thandler = srclpth;
	shandler = srclpsh;
	mhandler = srclpmh;
	
	if( offset == 0 ){
		ret = shandler->InitMQOMaterial();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		m_materialoffset = 0;
	}else{
		int maxmatno = 0;
		int chkmatcnt;
		CMQOMaterial* curchkmat = shandler->m_mathead;
		for( chkmatcnt = 0; chkmatcnt < shandler->m_materialcnt; chkmatcnt++ ){
			if( maxmatno < curchkmat->materialno ){
				maxmatno = curchkmat->materialno;
			}
			curchkmat = curchkmat->next;
		}
		m_materialoffset =  maxmatno + 1;
	}

	ppmathead = &( srclpsh->m_mathead );

	shandler->m_bonetype = m_bonetype;//!!!!!!!!!!!!!!!!!!


	D3DXMATRIX scalemat, rotxmat, rotymat, rotzmat, shiftmat;
	D3DXMatrixIdentity( &scalemat );
	D3DXMatrixIdentity( &rotxmat );
	D3DXMatrixIdentity( &rotymat );
	D3DXMatrixIdentity( &rotzmat );
	D3DXMatrixIdentity( &shiftmat );

	scalemat._11 = m_multiple;
	scalemat._22 = m_multiple;
	scalemat._33 = -m_multiple;//!!!!!!!! 反転

	shiftmat._41 = offsetpos.x;
	shiftmat._42 = offsetpos.y;
	shiftmat._43 = offsetpos.z;

	D3DXMatrixRotationX( &rotxmat, rot.x * (float)DEG2PAI );
	D3DXMatrixRotationY( &rotymat, rot.y * (float)DEG2PAI );
	D3DXMatrixRotationZ( &rotzmat, rot.z * (float)DEG2PAI );

	m_offsetmat = scalemat * rotzmat * rotymat * rotxmat * shiftmat;


	//if( offset > 0 )
	//	m_offset = offset - 1;//import dataの先頭データもSHDROOTで、無効データなので。
	//else
	//	m_offset = offset;

	m_offset = offset;// MQOFileには、SHDROOTが無い。
//////////
	ret = CheckFileVersion();
	if( ret ){
		DbgOut( "mqofile : LoadMQOFile : file version error!!!\n" );
		goto loadmqoexit;
	}
	m_state = BEGIN_FINDCHUNK;


	//char chunkname[256];

	while( m_state != BEGIN_FINISH ){

		switch( m_state ){
		case BEGIN_FINDCHUNK:
			ret = FindChunk( &m_state );
			if( ret ){
				_ASSERT( 0 );
				goto loadmqoexit;
			}
			break;
		case BEGIN_SCENE:
			ret = ReadScene( &m_state );
			if( ret ){
				_ASSERT( 0 );
				goto loadmqoexit;
			}
			break;
		case BEGIN_BACKIMAGE:
			ret = SkipChunk();
			if( ret ){
				_ASSERT( 0 );
				goto loadmqoexit;
			}
			m_state = BEGIN_FINDCHUNK;
			break;
		case BEGIN_MATERIAL:
			ret = ReadMaterial( &m_state );
			if( ret ){
				_ASSERT( 0 );
				goto loadmqoexit;
			}
			break;

		case BEGIN_OBJECT://子チャンクをもつ
			ret = ReadObject( &m_state );
			if( ret ){
				_ASSERT( 0 );
				goto loadmqoexit;
			}
			break;

		case BEGIN_VERTEX:
			if( currentobj ){
				ret = ReadVertex( &m_state );
				if( ret ){
					_ASSERT( 0 );
					goto loadmqoexit;
				}
			}else{
				DbgOut( "MQOFile : LoadMQOFile : BEGIN_VERTEX : currentobj error !!!" );
				_ASSERT( 0 );

				ret = SkipChunk();
				if( ret ){
					_ASSERT( 0 );
					goto loadmqoexit;
				}
				m_state = BEGIN_FINDCHUNK;
			}
			break;
		case BEGIN_FACE:
			if( currentobj ){
				ret = ReadFace( &m_state );
				if( ret ){
					_ASSERT( 0 );
					goto loadmqoexit;
				}
			}else{
				DbgOut( "MQOFile : LoadMQOFile : BEGIN_FACE : currentobj error !!!" );
				_ASSERT( 0 );

				ret = SkipChunk();
				if( ret ){
					_ASSERT( 0 );
					goto loadmqoexit;
				}
				m_state = BEGIN_FINDCHUNK;
			}
			break;
		case BEGIN_BVERTEX://子チャンクをもつ
			if( currentobj ){
				ret = ReadBVertex( &m_state );
				if( ret ){
					_ASSERT( 0 );
					goto loadmqoexit;
				}
			}else{
				DbgOut( "MQOFile : LoadMQOFile : BEGIN_BVERTEX : currentobj error !!!" );
				_ASSERT( 0 );

				ret = SkipChunk();
				if( ret ){
					_ASSERT( 0 );
					goto loadmqoexit;
				}
				m_state = BEGIN_FINDCHUNK;
			}			
			break;
		case BEGIN_COLOR:
			if( currentobj ){
				ret = ReadColor( &m_state );
				if( ret ){
					_ASSERT( 0 );
					goto loadmqoexit;
				}
			}else{
				DbgOut( "MQOFile : LoadMQOFile : BEGIN_COLOR : currentobj error !!!" );
				_ASSERT( 0 );

				ret = SkipChunk();
				if( ret ){
					_ASSERT( 0 );
					goto loadmqoexit;
				}
				m_state = BEGIN_FINDCHUNK;
			}			
			break;
		case BEGIN_VERTEXATTR://子チャンクをもつ
			m_state = BEGIN_FINDCHUNK;
			break;

		case BEGIN_VECTOR:
		case BEGIN_WEIT:
		case BEGIN_BLOB:
		case BEGIN_UNKCHUNK:
			ret = SkipChunk();
			if( ret ){
				_ASSERT( 0 );
				goto loadmqoexit;
			}
			m_state = BEGIN_FINDCHUNK;
			break;

		case BEGIN_TRIALNOISE:
		case BEGIN_FINISH:
		case BEGIN_NONE:
		default:
			m_state = BEGIN_FINISH;
			break;
		}
	}

	ret = Multiple();
	if( ret ){
		DbgOut( "MQOFile : LoadMQOFile : Multiple error !!!\n" );
		_ASSERT( 0 );
		goto loadmqoexit;
	}

	


	ret = MakeTree();
	if( ret ){
		DbgOut( "MQOFile : LoadMQOFile : MakeTree error !!!\n" );
		_ASSERT( 0 );
		goto loadmqoexit;
	}

	if( m_bonetype == BONETYPE_MIKO ){

		ret = SetMikoAnchorName();
		if( ret ){
			DbgOut( "mqofile : LoadMQOFile : SetMikoAnchorName error !!!\n" );
			_ASSERT( 0 );
			goto loadmqoexit;
		}

		ret = ReplaceLRName();
		if( ret ){
			DbgOut( "mqofile : LoadMQOFile : ReplaceLRName error !!!\n" );
			_ASSERT( 0 );
			goto loadmqoexit;
		}

		ret = ReplaceBdefSdef();
		if( ret ){
			DbgOut( "mqofile : LoadMQOFile : ReplaceBdefSdef error !!!\n" );
			_ASSERT( 0 );
			goto loadmqoexit;
		}

	}

	ret = shandler->SetMikoAnchorApply( thandler );
	if( ret ){
		DbgOut( "mqofile : LoadMQOFile : sh SetMikoAnchorApply error !!!\n" );
		_ASSERT( 0 );
		goto loadmqoexit;
	}

	ret = shandler->Anchor2InfScope( thandler, mhandler );
	if( ret ){
		DbgOut( "mqofile : LoadMQOFile : sh Anchor2InfScope error !!!\n" );
		_ASSERT( 0 );
		goto loadmqoexit;
	}

	ret = shandler->CreateShdBBoxIfNot( thandler, mhandler );
	if( ret ){
		DbgOut( "mqofile : LoadMQOFile : sh CreateShdBBoxIfNot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/***
	if( mathead ){
		CMQOMaterial* dumpmat;
		dumpmat = mathead;
		while( dumpmat ){
			ret = dumpmat->Dump();
			if( ret ){
				_ASSERT( 0 );
				goto loadmqoexit;
			}
			dumpmat = dumpmat->next;
		}
	}

	if( objhead ){
		CMQOObject* dumpobj;
		dumpobj = objhead;
		while( dumpobj ){
			ret = dumpobj->Dump();
			if( ret ){
				_ASSERT( 0 );
				goto loadmqoexit;
			}
			dumpobj = dumpobj->next;
		}
	}
	***/

	ret = shandler->CheckOrgNoBuf( &no_pm2orgno );
	if( ret ){
		DbgOut( "mqofile : LoadMQOFile : shandler : CheckOrgNoBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( no_pm2orgno ){
		shandler->m_im2enableflag = 0;
	}else{
		shandler->m_im2enableflag = 1;
	}


	goto loadmqoexit;

loadmqoexit:
	if( mqobuf.hfile != INVALID_HANDLE_VALUE ){
		//FlushFileBuffers( hfile );
		//SetEndOfFile( hfile );
		CloseHandle( mqobuf.hfile );
		mqobuf.hfile = INVALID_HANDLE_VALUE;
	}

	return ret;
}

int CMQOFile::LoadMQOFile( float multiple, char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, int groundflag, int bonetype, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot )
{

	int ret;

	if( !filename ){
		_ASSERT( 0 );
		return 1;
	}

	char pathname[2048];
	int ch = '\\';
	char* enptr = 0;
	enptr = strrchr( filename, ch );
	if( enptr ){
		strcpy_s( pathname, 2048, filename );
	}else{
		ZeroMemory( pathname, 2048 );
		int resdirleng;
		resdirleng = GetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPTSTR)pathname, 2048 );
		_ASSERT( resdirleng );
		strcat_s( pathname, 2048, filename );
	}

	SetLastError( 0 );
	mqobuf.hfile = CreateFile( (LPCTSTR)pathname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( mqobuf.hfile == INVALID_HANDLE_VALUE ){
		DWORD dwer;
		dwer = GetLastError();


		LPVOID lpMsgBuf = 0;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
		//	| FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwer,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
		);
		
		char mes[4098];
		sprintf_s( mes, 4098, "ファイルを開けませんでした。ファイル名があっているか確認してください。\n%s", pathname );
		::MessageBox( NULL, mes, "エラー", MB_OK );
		DbgOut( "mqofile : LoadMQOFile cant open : %s error !!!\n", pathname );

		if( lpMsgBuf ){
			::MessageBox( NULL, (LPCTSTR)lpMsgBuf, "GetLastError", MB_OK );
			DbgOut( "mqofile : LoadMQOFile : %s : error !!!\n", lpMsgBuf );
			LocalFree(lpMsgBuf);
		}

		ret = D3DAPPERR_MEDIANOTFOUND;
		return ret;
	}	


	ret = LoadMQOFile_aft( multiple, srclpth, srclpsh, srclpmh, offset, groundflag, bonetype, adjustuvflag, offsetpos, rot );
	if( ret ){
		DbgOut( "mqofile : LoadMQOFile : LoadMQOFile_aft error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


void CMQOFile::InitLoadParams( HWND apphwnd )
{
	int  ret;

	m_objcnt = 0;

	ZeroMemory( m_linechar, LINECHARLENG );

	m_state = BEGIN_NONE;

	m_apphwnd = apphwnd;

	ppmathead = 0;
	objhead = 0;
	currentobj = 0;

	thandler = 0;
	shandler = 0;
	mhandler = 0;


	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	cur_seri = 0;
	depth = 0;
	shdtype = 0;

	mqobuf.hfile = INVALID_HANDLE_VALUE;
	mqobuf.buf = 0;
	mqobuf.bufleng = 0;
	mqobuf.pos = 0;
	mqobuf.isend = 0;

	ZeroMemory( tempname, NAMELENG + 1 );

	m_multiple = 1.0f;
	m_groundflag = 0;

	m_includeextline = 0;

	m_adjustuvflag = 0;

	D3DXMatrixIdentity( &m_offsetmat );

	m_legacymatflag = 0;
	m_tempmathead = 0;

	m_materialoffset = 0;

}


//int CMQOFile::AddShape2Tree( SIGHEADER sh )


int CMQOFile::Init3DObj()
{

	return 0;
}


int CMQOFile::CheckFileVersion()
{
	int ret;
	int leng1, leng2;

	ret = GetLine( &leng1 );
	DbgOut( "mqofile : CheckFileVersion : filekind : %s\n", m_linechar );
	if( ret ){
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 0, 0 );
		::MessageBox( m_apphwnd, "Metasequoia Documentではありません。\n読み込めません。", "読み込みエラー", MB_OK );
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 1, 0 );

		return 1;
	}
	
	char pat1[] = "Metasequoia Document";
	int pat1leng = (int)strlen( pat1 );
	int cmp1 = 1;//!!!

	if( pat1leng <= leng1 )
		cmp1 = strncmp( pat1, m_linechar, pat1leng );

	if( cmp1 ){
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 0, 0 );
		::MessageBox( m_apphwnd, "Metasequoia Documentではありません。\n読み込めません。", "読み込みエラー", MB_OK );
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 1, 0 );
		return 1;
	}

////////	
	ret = GetLine( &leng2 );
	DbgOut( "mqofile : CheckFileVersion : fileversion : %s\n", m_linechar );
	if( ret ){
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 0, 0 );
		::MessageBox( m_apphwnd, "このfile versionには、対応していません。\n読み込めません。", "読み込みエラー", MB_OK );
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 1, 0 );
		return 1;
	}

	char pat2[] = "Format Text Ver 1.0";
	int pat2leng = (int)strlen( pat2 );
	int cmp2 = 1;//!!!

	if( pat2leng <= leng2 )
		cmp2 = strncmp( pat2, m_linechar, pat2leng );

	if( cmp2 ){
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 0, 0 );
		::MessageBox( m_apphwnd, "このfile versionには、対応していません。\n読み込めません。", "読み込みエラー", MB_OK );
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 1, 0 );
		return 1;
	}


	return 0;
}

int CMQOFile::GrowUpMQOBuf()
{
	if( mqobuf.isend == 1 )
		return 1;

	mqobuf.buf = (unsigned char*)realloc( mqobuf.buf, mqobuf.bufleng + BUFBLOCKLEN );
	if( !mqobuf.buf ){
		DbgOut( "MQOFile : GrowUpMQOFile : buf alloc error !!!" );
		return 1;
	}

	DWORD rleng, readleng;
	rleng = BUFBLOCKLEN;
	ReadFile( mqobuf.hfile, (void*)(mqobuf.buf + mqobuf.bufleng), rleng, &readleng, NULL );
	
	if( rleng != readleng ){
		mqobuf.isend = 1;
	}

	mqobuf.bufleng += BUFBLOCKLEN;

	return 0;
}

int CMQOFile::GetLine( int* getlen )
{
	int ret;

	if( mqobuf.buf == 0 ){
		ret = GrowUpMQOBuf();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		if( mqobuf.bufleng == 0 ){
			_ASSERT( 0 );
			DbgOut( "MQOFile : GetLine : empty file : warning !!!" );
			return 1;
		}
	}else if( (mqobuf.pos >= mqobuf.bufleng) && (mqobuf.isend == 0) ){
		ret = GrowUpMQOBuf();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	int notfound = 1;
	int stepno = 0;
	while( (mqobuf.pos + stepno < mqobuf.bufleng) && (notfound = ( *(mqobuf.buf + mqobuf.pos + stepno) != '\n' )) ){
		stepno++;

		if( mqobuf.pos + stepno >= mqobuf.bufleng ){
			if( notfound == 1 ){
				if( mqobuf.isend == 0 ){
					ret = GrowUpMQOBuf();
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}
				}else{
					_ASSERT( 0 );
					DbgOut( "MQOFile : GetLine : file is end warning !!!" );
					break;
				}
			}
		}
	}

	if( notfound == 0 ){
		stepno++; //\nの分
	}else{
		if( mqobuf.isend == 0 ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( LINECHARLENG > stepno ){
		strncpy_s( m_linechar, LINECHARLENG, (const char*)(mqobuf.buf + mqobuf.pos), stepno );
		*getlen = stepno;
		mqobuf.pos += stepno;

		m_linechar[stepno] = 0;

	}else{
		_ASSERT( 0 );
		DbgOut( "MQOFile : GetLine : dstlen short error !!!" );
		return 1;
	}

	if( (notfound == 1) && (mqobuf.isend == 1) ){
		m_state = BEGIN_FINISH;
		return 0;
	}else{

		if( ((*getlen == 2) && (strcmp( m_linechar, "\r\n" ) == 0)) || ((*getlen == 1) && (*m_linechar == '\n')) ){
			int retgl;			
			retgl = GetLine( getlen );	
			return retgl;
		}else{
			return notfound;
		}
	}

}

int CMQOFile::GetBytes( unsigned char** dstuc, int getlen )
{
	int ret;

	if( mqobuf.buf == 0 ){
		ret = GrowUpMQOBuf();
		if( ret ){
			return 1;
		}
		if( mqobuf.bufleng == 0 ){
			DbgOut( "MQOFile : GetBytes : empty file : warning !!!" );
			return 1;
		}
	}
	
	while( mqobuf.pos + getlen >= mqobuf.bufleng ){
		if( mqobuf.isend == 0 ){
			ret = GrowUpMQOBuf();
			if( ret ){
				return 1;
			}
		}else{
			DbgOut( "MQOFile : GetBytes : file is end warning !!!" );
			return 1;
		}
	}

	*dstuc = mqobuf.buf + mqobuf.pos;
	mqobuf.pos += getlen;

	return 0;
}

int CMQOFile::FindChunk( MQOSTATE* nextstate )
{
	int isfind = 0;
	int ret = 0;
	int getleng;
	while( isfind == 0 ){
		ret = GetLine( &getleng );
		if( ret ){
			_ASSERT( 0 );
			*nextstate = BEGIN_FINISH;
			break;
		}
		if( (getleng >= 3) && (strstr( m_linechar, "{\r\n" ) != NULL) ){
			isfind = 1;
			ret = GetChunkType( nextstate, m_linechar, getleng );
			if( ret ){
				_ASSERT( 0 );
				*nextstate = BEGIN_FINISH;
				break;
			}
		}else if( (getleng >= 5) && (strstr( m_linechar, "Eof\r\n" ) != NULL) ){
			*nextstate = BEGIN_FINISH;
			break;
		}
	}


	return ret;
}

int CMQOFile::GetChunkType( MQOSTATE* type, char* chunkname, int nameleng )
{
	char* namehead = chunkname;

	while( (*namehead == ' ') || (*namehead == '\t') ){
		namehead++;
	}

	char chunkpat[13][20] =
	{
		"TrialNoise ",
		"Scene ",
		"BackImage ",
		"Material ",
		"Object ",
		"vertexattr ",
		"vertex ", //vertexattrより後。
		"weit ",
		"color ",
		"BVertex ",
		"Vector ",
		"face ",
		"Blob "
	};

	int isfind = 0;
	int patno;
	for( patno = 0; patno < 13; patno++ ){
		if( isfind == 1 )
			break;

		int patleng;
		patleng = (int)strlen( chunkpat[patno] );
		int cmp = 1;
		if( nameleng >= patleng ){
			cmp = strncmp( chunkpat[patno], namehead, patleng );
			if( cmp == 0 ){
				isfind = 1;
				switch( patno ){
				case 0:
					*type = BEGIN_TRIALNOISE;
					break;
				case 1:
					*type = BEGIN_SCENE;
					break;
				case 2:
					*type = BEGIN_BACKIMAGE;
					break;
				case 3:
					*type = BEGIN_MATERIAL;
					break;
				case 4:
					*type = BEGIN_OBJECT;
					break;
				case 5:
					*type = BEGIN_VERTEXATTR;
					break;
				case 6:
					*type = BEGIN_VERTEX;
					break;
				case 7:
					*type = BEGIN_WEIT;
					break;
				case 8:
					*type = BEGIN_COLOR;
					break;
				case 9:
					*type = BEGIN_BVERTEX;
					break;
				case 10:
					*type = BEGIN_VECTOR;
					break;
				case 11:
					*type = BEGIN_FACE;
					break;
				case 12:
					*type = BEGIN_BLOB;
					break;
				default:
					break;
				}
			}
		}

	}

	if( isfind == 0 ){
		_ASSERT( 0 );
		*type = BEGIN_UNKCHUNK;
	}

	return 0;
}

int CMQOFile::SkipChunk()
{
	int depth = 1;
	int findend = 0;
	int ret;
	//char linechar[512];

	while( (findend == 0) || (depth != 0) ){
		
		findend = 0;

		int getleng;
		ret = GetLine( &getleng );
		if( ret ){
			_ASSERT( 0 );
			return ret;
		}

		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
			depth--;
		}else if( (getleng >= 3) && (strstr( m_linechar, "{\r\n" ) != NULL) ){
			depth++;
		}

	}

	

	return 0;
}

int CMQOFile::ReadColor( MQOSTATE* nextstate )
{
	int ret;
	int findend = 0;
	int getleng;

	_ASSERT( currentobj );
	ret = currentobj->CreateColor();
	if( ret ){
		DbgOut( "MQOFile : ReadColor : CreateColor error !!!" );
		_ASSERT( 0 );
		*nextstate = BEGIN_FINISH;
		return 1;
	}			
	
	int matno = 0;
	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;
		
		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
		}else{
			ret = currentobj->SetColor( m_linechar, (int)strlen( m_linechar ) );
			if( ret ){
				DbgOut( "MQOFile : ReadColor : SetColor error !!!" );
				_ASSERT( 0 );
				*nextstate = BEGIN_FINISH;
				return 1;
			}			
		}
	}
	
DbgOut( "mqofile : ReadColor : %s\r\n", m_linechar );

	*nextstate = BEGIN_FINDCHUNK;

	return 0;



}

int CMQOFile::ReadScene( MQOSTATE* nextstate )
{
	int ret;
	int findend = 0;
	int getleng;

	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;

		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
			//*nextstate = BEGIN_FINISH;
		}else{
			char ambpat[10] = "amb ";
			char* ambptr = 0;

			ambptr = strstr( m_linechar, ambpat );
			if( ambptr ){
				ARGBF ambient;
				ambient.a = 1.0f;
				ambient.r = 0.25f;
				ambient.g = 0.25f;
				ambient.b = 0.25f;
				
				int leng;
				leng = (int)strlen( m_linechar );

				int step;
				int cnt;
				float dstfloat;
				int stepnum;
				stepnum = 4;
				for( cnt = 0; cnt < 3; cnt++ ){		
					ret = GetFloat( &dstfloat, m_linechar, stepnum, leng, &step ); 
					if( ret )
						return ret;

					switch( cnt ){
					case 0:
						ambient.r = dstfloat;
						break;
					case 1:
						ambient.g = dstfloat;
						break;
					case 2:
						ambient.b = dstfloat;
						break;
					default:
						break;
					}

					stepnum += step;
				}
		
				shandler->m_scene_ambient = ambient;
				DbgOut( "mqofile : Scene : ambient %f %f %f\r\n", ambient.r, ambient.g, ambient.b );

			}


		}
	}

	*nextstate = BEGIN_FINDCHUNK;

	return 0;
}

int CMQOFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int flstart;
	int flend;
	int step = 0;

	while( (pos + step < srcleng) && 
	( (*(srcchar + pos + step) == ' ') || (*(srcchar + pos + step) == '\t') )
	//( *(srcchar + pos + step) ) 
	){
		step++;
	}

	flstart = pos+ step;

	while( (pos + step < srcleng) && 
		( isdigit( *(srcchar + pos + step) ) || (*(srcchar + pos + step) == '.') || (*(srcchar + pos + step) == '-') )
	){
		step++;
	}
	flend = pos + step;

	char tempchar[256];
	if( flend - flstart < 256 ){
		strncpy_s( tempchar, 256, srcchar + flstart, flend - flstart );
		tempchar[flend - flstart] = 0;
		*dstfloat = (float)atof( tempchar );
	}else{
		_ASSERT( 0 );
		*dstfloat = 0.0f;
	}

	*stepnum = step;

	return 0;

}


int CMQOFile::ReadMaterial( MQOSTATE* nextstate )
{

	m_newmatindex.clear();
	int newmatindex = -1;

	int ret;
	int findend = 0;
	//char linechar[1024];
	int getleng;

	int matno = 0;
	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;
		
		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
			//*nextstate = BEGIN_FINISH;
		}else{
			newmatindex++;

			CMQOMaterial* newmat = 0;
			char matname[256] = {0};
			char* startname = 0;
			char* endname = 0;
			int matno1 = -1;
			

			startname = strchr( m_linechar, '\"' );
			if( startname ){
				endname = strchr( startname + 1, '\"' );
				if( endname ){
					int nameleng = (int)( endname - startname - 1 );
					strncpy_s( matname, startname + 1, nameleng );
					matname[ nameleng ] = 0;

					shandler->GetMaterialNoByName( matname, &matno1 );
					if( matno1 >= 0 ){
						newmat = GetMaterialFromNo( *ppmathead, matno1 );
//DbgOut( "mqo!!! : ReadMaterial : matname %s, newmatindex %d, matno1 %d\r\n", matname, newmatindex, matno1 );

					}
				}
			}

			if( newmat ){
				m_newmatindex[ newmatindex ] = matno1;

			}else{
				newmat = new CMQOMaterial();
				if( !newmat ){
					DbgOut( "MQOFile : ReadMaterial : newmat alloc error !!!" );
					*nextstate = BEGIN_FINISH;
					return 1;
				}
				newmat->next = *ppmathead;
				*ppmathead = newmat;

	//DbgOut( "MQOFile : ReadMaterial : SetParams : %s\n", m_linechar );
				//matno = shandler->m_materialcnt - 1;
				matno = m_materialoffset + newmatindex;
				ret = newmat->SetParams( matno, shandler->m_scene_ambient, m_linechar, (int)strlen( m_linechar ) );
				if( ret ){
					DbgOut( "MQOFile : ReadMaterial : newmat SetParams error !!!" );
					_ASSERT( 0 );
					*nextstate = BEGIN_FINISH;
					return 1;
				}
				shandler->m_materialcnt++;


				if( matno == 0 ){
					switch( newmat->shader ){
					case 0:
						shandler->m_shader = COL_MQCLASSIC;
						break;
					case 1:
						shandler->m_shader = COL_BLINN;
						break;
					case 2:
						shandler->m_shader = COL_BLINN;
						break;
					case 3:
						shandler->m_shader = COL_PHONG;
						break;
					case 4:
						shandler->m_shader = COL_BLINN;
						break;
					default:
						shandler->m_shader = COL_BLINN;
						break;
					}
				}
			}
		}
	}
	
	*nextstate = BEGIN_FINDCHUNK;

	return 0;
}


int CMQOFile::ReadObject( MQOSTATE* nextstate )
{
	int ret;
	int findend = 0;
	//char linechar[1024];
	int getleng;

	m_objcnt++;

	CMQOObject* newobj;
	newobj = new CMQOObject();
	if( !newobj ){
		DbgOut( "MQOFile : ReadObject : newobj alloc error !!!" );
		*nextstate = BEGIN_FINISH;
		return 1;
	}
	
	
	//newobj->next = objhead;
	//objhead = newobj;

	//チェインの最後に追加するように変更。ver1031 2004/4/19

	CMQOObject* lastobj = objhead;
	CMQOObject* befobj = 0;
	while( lastobj ){
		befobj = lastobj;
		lastobj = lastobj->next;
	}

	if( befobj ){
		//befobjの次にチェイン
		befobj->next = newobj;
		newobj->next = 0;
	}else{
		//objheadにチェイン
		objhead = newobj;
		newobj->next = 0;
	}



	currentobj = newobj;//!!!!

	//nameのセット
	ret = newobj->SetParams( m_linechar, (int)strlen( m_linechar ) );
	if( ret ){
		DbgOut( "MQOFile : ReadObject : newobj SetParams error !!!" );
		_ASSERT( 0 );
		*nextstate = BEGIN_FINISH;
		return 1;
	}			


	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;
		
		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
			currentobj = 0;

		}else if( (getleng >= 3) && (strstr( m_linechar, "{\r\n" ) != NULL) ){
			//子チャンクをチェック
			ret = GetChunkType( nextstate, m_linechar, (int)strlen( m_linechar ) );
			_ASSERT( !ret );
			return 0;
		}else{
			//DbgOut( "MQOFile : ReadObject : SetParams : %s\n", m_linechar );
			ret = newobj->SetParams( m_linechar, (int)strlen( m_linechar ) );
			if( ret ){
				DbgOut( "MQOFile : ReadObject : newobj SetParams error !!!" );
				_ASSERT( 0 );
				*nextstate = BEGIN_FINISH;
				return 1;
			}			
		}
	}
	
	*nextstate = BEGIN_FINDCHUNK;

	return 0;

}

int CMQOFile::ReadBVertex( MQOSTATE* nextstate )
{
	int ret;
	int getleng;
	int vertnum;

	_ASSERT( currentobj );

	ret = GetLine( &getleng );
	if( ret ){
		_ASSERT( 0 );
		return ret;
	}

	ret = currentobj->SetVertex( &vertnum, m_linechar, (int)strlen( m_linechar ) );
	if( ret ){
		DbgOut( "MQOFile : ReadBVertex : SetVertex error !!!" );
		_ASSERT( 0 );
		*nextstate = BEGIN_FINISH;
		return 1;
	}

	unsigned char* dataptr;
	ret = GetBytes( &dataptr, vertnum * sizeof( VEC3F ) );
	if( ret ){
		DbgOut( "MQOFile : ReadBVertex : GetBytes error !!!" );
		_ASSERT( 0 );
		*nextstate = BEGIN_FINISH;
		return 1;
	}

	ret = currentobj->SetPointBuf( dataptr, vertnum );
	if( ret ){
		DbgOut( "MQOFile : ReadBVertex : SetPointBuf error !!!" );
		_ASSERT( 0 );
		*nextstate = BEGIN_FINISH;
		return 1;
	}


	*nextstate = BEGIN_FINDCHUNK;

	return 0;
}

int CMQOFile::ReadVertex( MQOSTATE* nextstate )
{
	int ret;
	int vertnum;

	_ASSERT( currentobj );

	ret = currentobj->SetVertex( &vertnum, m_linechar, (int)strlen( m_linechar ) );
	if( ret ){
		DbgOut( "MQOFile : ReadVertex : SetVertex error !!!" );
		_ASSERT( 0 );
		*nextstate = BEGIN_FINISH;
		return 1;
	}

	int findend = 0;
	int getleng;
	int vertno = 0;
	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;
		
		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
		}else{
			if( !currentobj || (vertno >= vertnum) ){
				DbgOut( "MQOFile : ReadVertex : vertno error !!!" );
				_ASSERT( 0 );
				*nextstate = BEGIN_FINISH;
				return 1;
			}

			ret = currentobj->SetPointBuf( vertno, m_linechar, getleng );
			if( ret ){
				DbgOut( "MQOFile : ReadVertex : SetPointBuf error !!!" );
				_ASSERT( 0 );
				*nextstate = BEGIN_FINISH;
				return 1;
			}
			
			vertno++;
		}
	}


	*nextstate = BEGIN_FINDCHUNK;

	return 0;
}


int CMQOFile::ReadFace( MQOSTATE* nextstate )
{
	int ret;
	int facenum;

	_ASSERT( currentobj );
	ret = currentobj->SetFace( &facenum, m_linechar, (int)strlen( m_linechar ) );
	if( ret ){
		DbgOut( "MQOFile : ReadFace : SetFace error !!!" );
		_ASSERT( 0 );
		*nextstate = BEGIN_FINISH;
		return 1;
	}


	int findend = 0;
	int getleng;
	int faceno = 0;
	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;
		
		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
		}else{
			if( !currentobj || (faceno >= facenum) ){
				DbgOut( "MQOFile : ReadFace : faceno error !!!" );
				_ASSERT( 0 );
				*nextstate = BEGIN_FINISH;
				return 1;
			}

			char* mnostart = 0;
			char* mnoend = 0;
			char strmno[256] = {0};
			int orgmno = -1;
			mnostart = strstr( m_linechar, "M(" );
			if( mnostart ){
				mnoend = strchr( mnostart + 2, ')' );
				if( mnoend ){
					int strleng = mnoend - (mnostart + 2);
					strncpy_s( strmno, 256, mnostart + 2, strleng );
					strmno[ strleng ] = 0;
					orgmno = atoi( strmno );
				}else{
					_ASSERT( 0 );
				}
			}else{
				//_ASSERT( 0 );
			}

			int setmatno = 0;

			std::map<int, int>::iterator finditr;
			finditr = m_newmatindex.find( orgmno );
			if( finditr == m_newmatindex.end() ){
				setmatno = orgmno + m_materialoffset;
			}else{
				setmatno = finditr->second;
			}

//int cmp0;
//cmp0 = strcmp( currentobj->name, "kao" );
//if( cmp0 == 0 ){
//	DbgOut( "mqo!!!: objno %d, name %s, orgmno %d, setmatno %d\r\n", currentobj->objectno, currentobj->name, orgmno, setmatno );
//}

			ret = currentobj->SetFaceBuf( faceno, m_linechar, getleng, setmatno );
			if( ret ){
				DbgOut( "MQOFile : ReadFace : SetFaceBuf error !!!" );
				_ASSERT( 0 );
				*nextstate = BEGIN_FINISH;
				return 1;
			}
			
			faceno++;
		}
	}



	*nextstate = BEGIN_FINDCHUNK;

	return 0;
}

int CMQOFile::MakeTree()
{
	int ret;

	if( m_offset == 0 )
		cur_seri = 0;
	else
		cur_seri = m_offset - 1;
	

	ret = start();
	if( ret ){
		DbgOut( "MQOFile : MakeTree : start error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = make_first_part();
	if( ret ){
		DbgOut( "MQOFile : MakeTree : make_first_part error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = make_polymesh2();
	if( ret ){
		DbgOut( "MQOFile : MakeTree : make_polymesh2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	ret = end_first_part();
	if( ret ){
		DbgOut( "MQOFile : MakeTree : end_first_part error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_bonetype == BONETYPE_RDB2 ){
		ret = make_balljoints();
		if( ret ){
			DbgOut( "MQOFile : MakeTree : make_balljoints error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( m_bonetype == BONETYPE_MIKO ){
		ret = make_mikobone();
		if( ret ){
			DbgOut( "MQOFile : MakeTree : make_mikobone error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	ret = make_extline();
	if( ret ){
		DbgOut( "MQOFile : MakeTree : make_extline error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = finish();
	if( ret ){
		DbgOut( "MQOFile : MakeTree : finish error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CMQOFile::start()
{
	int ret;
	depth = 0;

	ret = thandler->Start( m_offset );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	totalpoly = 0;

	return 0;
}

int CMQOFile::begin()
{
	int ret;
	ret = thandler->Begin();
	if( ret ){
		DbgOut( "MQOFile : begin error!!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	depth = thandler->GetCurDepth(); //!!! thandler->Beginするたびに必ず呼ぶ。

	ret = AddShape2Tree();
	if( ret ){
		DbgOut( "MQOFile : begin : AddShape2Tree error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CMQOFile::end()
{
	int ret;
	ret = thandler->End();
	if( ret ){
		DbgOut( "MQOFile : end error!!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CMQOFile::AddShape2Tree()
{
	int ret = 0;	
	
	cur_seri++;

	ret = tempinfo.ResetParams();
	if( ret ){
		DbgOut( "MQOFile : AddShape2Tree : ResetParams error!!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	thandler->AddTree( tempname, cur_seri );
	//if( cur_seri <= 0 ) // 0 : root は不可。
	//{
	//	DbgOut( "MQOFile : AddShape2Tree : thandler->AddTree error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}

	ret = thandler->SetElemType( cur_seri, shdtype );
	if( ret ){
		DbgOut( "MQOFile : AddShape2Tree : thandler->SetElemType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

		
	// shdhandler
	ret = shandler->AddShdElem( cur_seri, depth );
	if( ret ){
		DbgOut( "MQOFile : AddShape2Tree : shandler->AddShdElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = tempinfo.SetMem( shdtype, BASE_TYPE );
	if( ret ){
		DbgOut( "MQOFile : AddShape2Tree : tempinfo.SetMem : BASE_TYPE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	/***
	bool isinvface;
	isinvface = current_shape->get_flip_face();
	ret = shandler->SetInvFace( cur_seri, isinvface );
	_ASSERT( !ret );
	***/

	// beziersurf以外のpart, joint は、ここで作成
	//if( (shdtype == SHDPART) || IsJoint() ){
	//	do_part( current_shape );
	//}

	// mothandler
	do_transform_mat();


	// texture
	//do_texture( current_shape );

	return 0;
}


int CMQOFile::make_first_part()
{
	int ret;

	if( m_groundflag == 0 ){

		shdtype = SHDPART;
		strcpy_s( tempname, NAMELENG + 1, "パート" );

		ret = begin();
		if( ret ){
			DbgOut( "MQOFile : make_first_part : begin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = shandler->Init3DObj( cur_seri, &tempinfo );
		if( ret ){
			DbgOut( "MQOFile : make_first_part : Init3DObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{

	// polymesh2 の数をチェックする。
		int haspolygon;
		CMQOObject* curobj = objhead;
		int pm2num = 0;
		while( curobj ){
			haspolygon = curobj->HasPolygon();
			if( haspolygon > 0 ){
				pm2num++;
			}
			curobj = curobj->next;
		}

	// SHDGROUNDDATAを作る。
		if( pm2num > 0 ){
			shdtype = SHDGROUNDDATA;
			strcpy_s( tempname, NAMELENG + 1, "GroundData" );

			ret = begin();
			if( ret ){
				DbgOut( "MQOFile : make_first_part : SHDGROUNDDATA : begin error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			tempinfo.m = 0; // ！！！<--- 0 は、n にpm2の個数が入っていることを示す。
			tempinfo.n = pm2num;
			ret = shandler->Init3DObj( cur_seri, &tempinfo );
			if( ret ){
				DbgOut( "MQOFile : make_first_part : SHDGROUNDDATA : Init3DObj error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			shdtype = SHDPART;
			strcpy_s( tempname, NAMELENG + 1, "パート" );

			ret = begin();
			if( ret ){
				DbgOut( "MQOFile : make_first_part : begin error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = shandler->Init3DObj( cur_seri, &tempinfo );
			if( ret ){
				DbgOut( "MQOFile : make_first_part : Init3DObj error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	/***
	//RokDeBoneとメタセコで、clockwiseが逆なので、先頭パートで、反転しておく。
	ret = shandler->SetInvFace( cur_seri, 1 );
	if( ret ){
		DbgOut( "MQOFile : make_first_part : SetInvFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/

	//end();<--- 全ての形状を作ってから。end_first_part()で行う。

	return ret;
}
int CMQOFile::end_first_part()
{
	int ret;
	ret = end();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}


int CMQOFile::do_transform_mat () 
{
	// shapetype == part 以外でも、partにキャスト可能ならtransformを出力

	CMatrix2 tempcmat;
	int ret;

	ret = mhandler->AddMotionCtrl( cur_seri, depth );
	if( ret ){
		DbgOut( "MQOFile : do_transform_mat : AddMotionCtrl error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//hasmotionのセット
		//skip jointの際に使う。	
	if( IsJoint() && (shdtype != SHDMORPH) ){
		ret = mhandler->SetHasMotion( cur_seri, 1 );
		if( ret ){
			DbgOut( "MQOFile : do_transform_mat : SetHasMotion error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = mhandler->SetHasMotion( cur_seri, 0 );
		if( ret ){
			DbgOut( "MQOFile : do_transform_mat : SetHasMotion error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	// R5 では、使用しないので、基本行列をセット。
	tempcmat.Identity();
	ret = mhandler->SetTransMat( cur_seri, &tempcmat );
	if( ret ){
		DbgOut( "MQOFile : do_transform_mat : SetTransMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = mhandler->SetPartMat( cur_seri, &tempcmat );
	if( ret ){
		DbgOut( "MQOFile : do_transform_mat : SetPartMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CMQOFile::make_polymesh2()
{
	int ret, haspolygon;
	CMQOObject* curobj = objhead;

	int ismikobone;

	while( curobj ){

		haspolygon = curobj->HasPolygon();
		ismikobone = curobj->IsMikoBone();

		if( (haspolygon > 0) && ((m_bonetype != BONETYPE_MIKO) || (ismikobone == 0)) ){
			strcpy_s( tempname, NAMELENG + 1, curobj->name );
			shdtype = SHDPOLYMESH2;
			ret = begin();
			if( ret ){
				DbgOut( "MQOFile : make_polymesh2 : begin error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int cmpanc;
			cmpanc = strncmp( curobj->name, "anchor", 6 );
			if( cmpanc == 0 ){
				curobj->mirror = 0;//!!!!!!!!  アンカーは、ミラー禁止。
			}


			ret = curobj->MakePolymesh2( shandler, cur_seri, shandler->m_mathead, m_groundflag, m_adjustuvflag );
			if( ret ){
				DbgOut( "MQOFile : make_polymesh2 : curobj MakePolymesh2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			CShdElem* selem;
			selem = (*shandler)( cur_seri );
			_ASSERT( selem );
			selem->facet = curobj->facet;
DbgOut( "mqofile : make_polymesh2 : facet %f, %s\r\n", selem->facet, curobj->name );

			ret = end();
			if( ret ){
				DbgOut( "MQOFile : make_polymesh2 : end error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		curobj = curobj->next;
	}

	return 0;
}

int CMQOFile::make_extline()
{
	int ret, hasline;
	CMQOObject* curobj = objhead;

	while( curobj ){
		char* linename;
		linename = strstr( curobj->name, "EXTLINE_" );//名前に、EXTLINE_が含まれていた場合は、表示用ラインとする。
		if( linename != NULL ){

			hasline = curobj->HasLine();

			if( hasline > 0 ){
				strcpy_s( tempname, NAMELENG + 1, curobj->name );
				shdtype = SHDEXTLINE;
				ret = begin();
				if( ret ){
					DbgOut( "MQOFile : make_extline : begin error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = curobj->MakeExtLine( shandler, cur_seri, shandler->m_mathead );
				if( ret ){
					DbgOut( "MQOFile : make_extline : curobj MakeExtLine error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				mhandler->m_curbs.visibleflag = 1;//!!!!!!!!!!!!

				ret = end();
				if( ret ){
					DbgOut( "MQOFile : make_extline : end error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			m_includeextline++;

		}

		curobj = curobj->next;
	}

	return 0;
}

int CMQOFile::make_mikobone()
{

	int ret;
	int ismikobone;
	CMQOObject* curobj = objhead;

	int totaljointnum = 0;

	while( curobj ){
		ismikobone = curobj->IsMikoBone();
		if( ismikobone ){
			
			ret = curobj->SetMikoBoneIndex3();
			if( ret ){
				DbgOut( "mqofile : make_mikobone : obj SetMikoBoneIndex3 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = curobj->SetMikoBoneIndex2();
			if( ret ){
				DbgOut( "mqofile : make_mikobone : obj SetMikoBoneIndex2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = curobj->CheckSameMikoBone();
			if( ret ){
				DbgOut( "mqofile : make_mikobone : obj CheckSameMikoBone error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			CMQOFace* toplevelface[ MAXBONENUM ];
			int toplevelnum = 0;

			ret = curobj->GetTopLevelMikoBone( toplevelface, &toplevelnum, MAXBONENUM );
			if( ret ){
				DbgOut( "mqofile : make_mikobone : obj GetTopLevelMikoBone error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

DbgOut( "mqofile : make_mikobone : toplevelnum %d\r\n", toplevelnum );

			ret = curobj->InitFaceDirtyFlag();//!!!!!!!!!!!!
			if( ret ){
				DbgOut( "mqofile : make_mikobone : obj InitFaceDirtyFlag error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int topno;
			for( topno = 0; topno < toplevelnum; topno++ ){
				ret = curobj->SetTreeMikoBone( toplevelface[ topno ], shandler->m_mathead );
				if( ret ){
					DbgOut( "mqofile : make_mikobone : obj SetTreeMikoBone error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

		//////////////
			ret = curobj->InitFaceDirtyFlag();//!!!!!!!!!!!!
			if( ret ){
				DbgOut( "mqofile : make_mikobone : obj InitFaceDirtyFlag error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int islooped = 0;
			int jointnum = 0;

			for( topno = 0; topno < toplevelnum; topno++ ){
				ret = curobj->CheckLoopedMikoBoneReq( shandler->m_mathead, toplevelface[ topno ], &islooped, &jointnum );
				if( ret ){
					DbgOut( "mqofile : make_mikobone : obj CheckLoopedMikoBone error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				
				if( islooped ){
					DbgOut( "mqofile : make_mikobone : looped bone is found error !!!\n" );
					::MessageBox( NULL, "ループ又は重複しているボーン構造が見つかりました。(誤判定かも（汗）)読み込めません。", "エラー", MB_OK );
					_ASSERT( 0 );
					return 1;
				}

				totaljointnum += jointnum;
				if( totaljointnum >= MAXBONENUM ){
					DbgOut( "mqofile : make_mikobone : totaljointnum overflow error !!!\n" );
					::MessageBox( NULL, "ボーンの数が多すぎます。2048以下にしてください。", "エラー", MB_OK );
					_ASSERT( 0 );
					return 1;
				}
			}

		//////////////////
			ret = curobj->SetMikoBoneName( shandler->m_mathead );
			if( ret ){
				DbgOut( "mqofile : make_mikobone : obj SetMikoBoneNameNormal error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = curobj->SetMikoFloatBoneName();
			if( ret ){
				DbgOut( "mqofile : make_mikobone : obj SetMikoFloatBoneName error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		///////////////////
			for( topno = 0; topno < toplevelnum; topno++ ){
				curobj->DumpMikoBoneReq( toplevelface[ topno ], 0 );
			}
			
		///////////////////
			shdtype = SHDPART;
			sprintf_s( tempname, NAMELENG + 1, "Joint Part%d", curobj->objectno ); 
		
			begin();
			ret = shandler->Init3DObj( cur_seri, &tempinfo );
			_ASSERT( !ret );

			//////

			int errorcnt = 0;
			for( topno = 0; topno < toplevelnum; topno++ ){
				make_balljoints_from_mikoboneReq( curobj, toplevelface[ topno ], &errorcnt );
				if( errorcnt > 0 ){
					DbgOut( "mqofile : make_mikobone : make_balljoints_from_mikoboneReq error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				end();// TopOf%s のためのend
			}

			////////
			end();//partのためのend


		}
	
		curobj = curobj->next;
	}

	return 0;
}

void CMQOFile::make_balljoints_from_mikoboneReq( CMQOObject* srcobj, CMQOFace* srcface, int* errorcnt )
{
	int ret;
	
	if( !srcface->parent ){
		shdtype = SHDBALLJOINT;
		sprintf_s( tempname, NAMELENG + 1, "TopOf%s", srcface->bonename );// cur_seri は　begin 内で、++されて、値が更新される！！！
		begin();

		ret = shandler->Init3DObj( cur_seri, &tempinfo );
		if( ret ){
			DbgOut( "mqofile : make_balljoints_from_mikoboneReq : sh Init3DObj error !!!\n" );
			_ASSERT( 0 );
			(*errorcnt)++;
			return;
		}
						
		CVec3f jointloc;
		ret = srcobj->GetParentLock( srcface, &jointloc );
		if( ret ){
			DbgOut( "mqofile : make_balljoints_from_mikoboneReq : obj GetParentLock error !!!\n" );
			_ASSERT( 0 );
			(*errorcnt)++;
			return;
		}

		CShdElem* curselem;
		curselem = (*shandler)(cur_seri);
		ret = curselem->SetJointLoc( &jointloc );
		if( ret ){
			DbgOut( "mqofile : make_balljoints_from_mikoboneReq : selem SetJointLoc error !!!\n" );
			_ASSERT( 0 );
			(*errorcnt)++;
			return;
		}
	}


	shdtype = SHDBALLJOINT;
	strcpy_s( tempname, NAMELENG + 1, srcface->bonename );// cur_seri は　begin 内で、++されて、値が更新される！！！
	begin();

	ret = shandler->Init3DObj( cur_seri, &tempinfo );
	if( ret ){
		DbgOut( "mqofile : make_balljoints_from_mikoboneReq : sh Init3DObj error !!!\n" );
		_ASSERT( 0 );
		(*errorcnt)++;
		return;
	}
					
	CVec3f jointloc;
	ret = srcobj->GetChildLock( srcface, &jointloc );
	if( ret ){
		DbgOut( "mqofile : make_balljoints_from_mikoboneReq : obj GetParentLock error !!!\n" );
		_ASSERT( 0 );
		(*errorcnt)++;
		return;
	}

	CShdElem* curselem;
	curselem = (*shandler)(cur_seri);
	ret = curselem->SetJointLoc( &jointloc );
	if( ret ){
		DbgOut( "mqofile : make_balljoints_from_mikoboneReq : selem SetJointLoc error !!!\n" );
		_ASSERT( 0 );
		(*errorcnt)++;
		return;
	}

///////
	if( srcface->mikobonetype == MIKOBONE_FLOAT ){
		curselem->m_notransik = 1;
		curselem->m_notsel = 1;
		curselem->m_setmikobonepos = 0;
	}else{
		CVec3f hloc;
		ret = srcobj->GetHLock( srcface, &hloc );
		if( ret ){
			DbgOut( "mqofile : make_balljoints_from_mikoboneReq : mqoobj GetHLock error !!!\n" );
			_ASSERT( 0 );
			(*errorcnt)++;
			return;
		}
		curselem->m_mikobonepos = D3DXVECTOR3( hloc.x, hloc.y, hloc.z );//!!!!!!!!!!!!!!!!!!!!!!!!!!
		curselem->m_setmikobonepos = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	CMQOFace* chilface;
	chilface = srcface->child;
	if( chilface && (chilface->mikobonetype == MIKOBONE_FLOAT) ){
		curselem->m_notransik = 1;
		//curselem->m_notsel = 1;
		//_ASSERT( 0 );
	}

/////////////
	if( srcface->child ){
		make_balljoints_from_mikoboneReq( srcobj, srcface->child, errorcnt );
	}

	end();

	if( srcface->brother ){
		make_balljoints_from_mikoboneReq( srcobj, srcface->brother, errorcnt );
	}

}



int CMQOFile::make_balljoints()
{
	shdtype = SHDPART;
	strcpy_s( tempname, NAMELENG + 1, "Joint Part" ); 
	begin();

	int ret = 0;

	ret = shandler->Init3DObj( cur_seri, &tempinfo );
	_ASSERT( !ret );

////////
	int* jointflag;
	int objno = 0;
	CMQOObject* curobj = objhead;
	while( curobj ){
		
		//オブジェクト名にEXTLINE_が含まれていた場合には、ジョイントの処理をしない。
		char* linename;
		linename = strstr( curobj->name, "EXTLINE_" );
		if( linename == NULL ){

			int face_count = curobj->face;
			int vert_count = curobj->vertex;

			
			VEC3F* pts = curobj->pointbuf;

			jointflag = (int*)malloc( sizeof( int ) * vert_count );
			if( !jointflag ){
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( jointflag, sizeof( int ) * vert_count );
			
			int face_index;
			for( face_index = 0; face_index < face_count; face_index++ )
			{
				CMQOFace* curface = curobj->facebuf + face_index;
				if( curobj->lathe == 0 ){
					switch( curface->pointnum ){
					case 2:
						*(jointflag + curface->index[0]) = 1;
						*(jointflag + curface->index[1]) = 1;
						break;
					default:
						break;
					}
				}
			}

			int vertno;
			int jointno = 0;
			for( vertno = 0; vertno < vert_count; vertno++ ){
				if( *(jointflag + vertno) == 1 ){
			
					shdtype = SHDBALLJOINT;
					//sprintf( tempname, "BallJoint_%d_%d", objno, jointno );
					sprintf_s( tempname, NAMELENG + 1, "ジョイント_シリアル%d", cur_seri + 1 );// cur_seri は　begin 内で、++されて、値が更新される！！！
					begin();

					ret = shandler->Init3DObj( cur_seri, &tempinfo );
					_ASSERT( !ret );
					
					CVec3f jointloc;
					jointloc.x = (pts + vertno)->x;
					jointloc.y = (pts + vertno)->y;
					jointloc.z = (pts + vertno)->z;
					
					CShdElem* curselem;
					curselem = (*shandler)(cur_seri);
					ret = curselem->SetJointLoc( &jointloc );
					if( ret ){
						DbgOut( "MQOFile : make_balljoints : SetJointLoc error !!!\n" );
						return 1;
					}


					end();
					jointno++;
				}
			}

			free( jointflag );
			objno++;
		}

		curobj = curobj->next;
	}

////////
	end();//partのためのend

	return 0;
}

int CMQOFile::finish() {
	int ret = 0;

	depth--;

	ret = shandler->SetChain( m_offset );// treehandlerのchainが出来あがってから。
	if( ret ){
		DbgOut( "finish : shandler->SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//ret = shandler->SetColors();
	//if( ret ){
	//	DbgOut( "finish : shandler->SetColors error !!!\n" );
	//	_ASSERT( 0 );
	//}

	//ret = shandler->SetTexName();
	//if( ret ){
	//	DbgOut( "finish : shandler->SetTexName error !!!\n" );
	//	_ASSERT( 0 );
	//}

	ret = shandler->SetClockwise();
	if( ret ){
		DbgOut( "finish : shandler->SetClockwise error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = mhandler->SetChain( m_offset );
	if( ret ){
		DbgOut( "finish : mhandler->SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

		
	if( m_includeextline > 0 )
		shandler->m_TLmode = TLMODE_D3D;


//DbgOut( "sig exporter : finish : shandler %x, s2shd_leng %d\n",
//	   shandler, shandler->s2shd_leng );


	return 0;
}

int CMQOFile::IsJoint()
{
	if( (shdtype >= SHDROTATE) && (shdtype <= SHDBALLJOINT) && (shdtype != SHDMORPH) ){
		return 1;
	}else{
		return 0;
	}

}

int CMQOFile::Multiple()
{
	int ret;

	CMQOObject* curobj = objhead;

	while( curobj ){

		ret = curobj->MultMat( m_offsetmat );
		if( ret ){
			DbgOut( "MQOFile : Multiple : obj MultMat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		/***
		ret = curobj->Multiple( m_multiple );
		if( ret ){
			DbgOut( "MQOFile : Multiple error !!!\n" );
			return 1;
		}

		ret = curobj->Shift( m_offsetpos );
		if( ret ){
			DbgOut( "MQOFile : Multiple : obj Shfit error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		***/

		curobj = curobj->next;
	}

	return 0;
}

/////////////////////////
/////////////////////////
/////////////////////////

int CMQOFile::SaveMQOFileOnFrame( int chkmorph, int motid, int frameno, char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, float srcmult, int srcoutbone, HANDLE srchfile )
{
	if( (srchfile == INVALID_HANDLE_VALUE) && !filename ){
		_ASSERT( 0 );
		return 1;
	}

	//InitLoadParams();<--- コンストラクタで呼ばれる。

	int** matnoarray = 0;// faceno -> materialno の変換表を配列の要素とする、配列。
	int serino;

	HANDLE hfile = INVALID_HANDLE_VALUE;
	int ret = 0;

	if( !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "mqofile : SaveMQOFile : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	thandler = srclpth;
	shandler = srclpsh;
	mhandler = srclpmh;


	if( srchfile == INVALID_HANDLE_VALUE ){
		hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
		if( hfile == INVALID_HANDLE_VALUE ){
			DbgOut( "mqofile : SaveMQOFile : CreateFile error !!! %s\n", filename );
			_ASSERT( 0 );
			ret = 1;
			goto savemqoexit;
		}	
	}else{
		hfile = srchfile;
	}


// header
	ret = WriteMQOHeader( hfile );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFile : WriteMQOHeader error !!!\n" );
		ret = 1;
		goto savemqoexit;
	}

// material
	CMQOMaterial* firstmat;
	firstmat = GetMaterialFromNo( shandler->m_mathead, 0 );
	if( !firstmat ){
		m_legacymatflag = 1;
	}else{
		if( firstmat->legacyflag != 0 ){
			m_legacymatflag = 1;
		}else{
			m_legacymatflag = 0;
		}
	}

	if( m_legacymatflag != 0 ){
		matnoarray = (int**)malloc( sizeof( int ) * shandler->s2shd_leng );
		if( !matnoarray ){
			DbgOut( "mqofile : SaveMQOFile : matnoarray alloc error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto savemqoexit;
		}
		ZeroMemory( matnoarray, sizeof( int* ) * shandler->s2shd_leng );

		ret = CreateMaterial( matnoarray );
		if( ret ){
			DbgOut( "mqofile : SaveMQOFile : CreateMaterial error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto savemqoexit;
		}
		ret = WriteMQOMaterial( hfile, m_tempmathead );
		if( ret ){
			DbgOut( "mqofile : SaveMQOFile : WriteMQOMaterial error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto savemqoexit;
		}

	}else{
		ret = WriteMQOMaterial( hfile, shandler->m_mathead );
		if( ret ){
			DbgOut( "mqofile : SaveMQOFile : WriteMQOMaterial error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto savemqoexit;
		}
	}
// object

	ret = WriteMQOObjectOnFrame( hfile, matnoarray, motid, frameno, srcmult, chkmorph );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFile : WriteMQOObject error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savemqoexit;
	}

	if( srcoutbone ){

		ret = WriteMQOBoneOnFrame( hfile, motid, frameno, srcmult );
		if( ret ){
			DbgOut( "mqofile : SaveMQOFile : WriteMQOBoneOnFrame error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto savemqoexit;
		}
	}


// Eof
	strcpy_s( m_linechar, LINECHARLENG, "Eof" );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFile : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savemqoexit;
	}

	goto savemqoexit;
savemqoexit:
	if( (srchfile == INVALID_HANDLE_VALUE) && (hfile != INVALID_HANDLE_VALUE) ){
		FlushFileBuffers( hfile );
		SetEndOfFile( hfile );
		CloseHandle( hfile );
	}
	hfile = INVALID_HANDLE_VALUE;

	if( m_tempmathead ){
		CMQOMaterial* delmat = m_tempmathead;
		CMQOMaterial* nextmat;
		while( delmat ){
			nextmat = delmat->next;
			delete delmat;
			delmat = nextmat;
		}
		m_tempmathead = 0;
	}

	if( matnoarray ){
		int* delelem;

		for( serino = 0; serino < shandler->s2shd_leng; serino++ ){
			delelem = *(matnoarray + serino);
			if( delelem ){
				free( delelem );
			}
		}
		
		free( matnoarray );

		matnoarray = 0;
	}


	return ret;
}


int CMQOFile::SaveMQOFileFromBVH( int srcjointnum, int srcbonenum, CBVHElem* srcbe, char* filename )
{
	if( !filename ){
		_ASSERT( 0 );
		return 1;
	}

	//InitLoadParams();<--- コンストラクタで呼ばれる。

	//int serino;

	HANDLE hfile = INVALID_HANDLE_VALUE;
	int ret = 0;
	int mqono = 0;

	CMQOMaterial newmat;
	newmat.materialno = 0;
	strcpy_s( newmat.name, 256, "DummyMaterial" );
	newmat.next = 0;


	hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "mqofile : SaveMQOFileFromBVH : CreateFile error !!! %s\n", filename );
		_ASSERT( 0 );
		ret = 1;
		goto savemqoexit;
	}	

// header
	ret = WriteMQOHeader( hfile );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFileFromBVH : WriteMQOHeader error !!!\n" );
		ret = 1;
		goto savemqoexit;
	}

// material

	m_legacymatflag = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	ret = WriteMQOMaterial( hfile, &newmat );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFileFromBVH : WriteMQOMaterial error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savemqoexit;
	}

// object
	ret = WriteObjectHeader( hfile, "BoneLines", 59.5f );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFileFromBVH : WriteObjectHeader error !!!\n" );
		_ASSERT( 0 );
		goto savemqoexit;
	}

	sprintf_s( m_linechar, LINECHARLENG, "\tvertex %d {", srcjointnum );
	ret = WriteLinechar( hfile, 1 );
	_ASSERT( !ret );
	mqono = 0;
	WriteMQOVertexFromBVHReq( hfile, srcbe, &mqono );
	sprintf_s( m_linechar, LINECHARLENG, "\t}" );
	ret = WriteLinechar( hfile, 1 );
	_ASSERT( !ret );



	sprintf_s( m_linechar, LINECHARLENG, "\tface %d {", srcbonenum );
	ret = WriteLinechar( hfile, 1 );
	_ASSERT( !ret );
	WriteMQOFaceFromBVHReq( hfile, srcbe );
	sprintf_s( m_linechar, LINECHARLENG, "\t}" );
	ret = WriteLinechar( hfile, 1 );
	_ASSERT( !ret );



	strcpy_s( m_linechar, LINECHARLENG, "}" );//objectの終わり
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFileFromBVH : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		goto savemqoexit;
	}


// Eof
	strcpy_s( m_linechar, LINECHARLENG, "Eof" );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFile : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savemqoexit;
	}

	goto savemqoexit;
savemqoexit:
	if( hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( hfile );
		SetEndOfFile( hfile );
		CloseHandle( hfile );
		hfile = 0;
	}

	return ret;

}


int CMQOFile::SaveMQOFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh )
{
	if( !filename ){
		_ASSERT( 0 );
		return 1;
	}

	//InitLoadParams();<--- コンストラクタで呼ばれる。

	int** matnoarray = 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 削除する
	int serino;

	HANDLE hfile = INVALID_HANDLE_VALUE;
	int ret = 0;

	if( !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "mqofile : SaveMQOFile : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	thandler = srclpth;
	shandler = srclpsh;
	mhandler = srclpmh;

	hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "mqofile : SaveMQOFile : CreateFile error !!! %s\n", filename );
		_ASSERT( 0 );
		ret = 1;
		goto savemqoexit;
	}	

// header
	ret = WriteMQOHeader( hfile );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFile : WriteMQOHeader error !!!\n" );
		ret = 1;
		goto savemqoexit;
	}

// material
	CMQOMaterial* firstmat;
	firstmat = GetMaterialFromNo( shandler->m_mathead, 0 );
	if( !firstmat ){
		m_legacymatflag = 1;
	}else{
		if( firstmat->legacyflag != 0 ){
			m_legacymatflag = 1;
		}else{
			m_legacymatflag = 0;
		}
	}

	if( m_legacymatflag != 0 ){
		matnoarray = (int**)malloc( sizeof( int ) * shandler->s2shd_leng );
		if( !matnoarray ){
			DbgOut( "mqofile : SaveMQOFile : matnoarray alloc error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto savemqoexit;
		}
		ZeroMemory( matnoarray, sizeof( int* ) * shandler->s2shd_leng );

		ret = CreateMaterial( matnoarray );
		if( ret ){
			DbgOut( "mqofile : SaveMQOFile : CreateMaterial error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto savemqoexit;
		}
		ret = WriteMQOMaterial( hfile, m_tempmathead );
		if( ret ){
			DbgOut( "mqofile : SaveMQOFile : WriteMQOMaterial error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto savemqoexit;
		}

	}else{
		ret = WriteMQOMaterial( hfile, shandler->m_mathead );
		if( ret ){
			DbgOut( "mqofile : SaveMQOFile : WriteMQOMaterial error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto savemqoexit;
		}
	}

// object
	ret = WriteMQOObject( hfile, matnoarray );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFile : WriteMQOObject error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savemqoexit;
	}

// Eof
	strcpy_s( m_linechar, LINECHARLENG, "Eof" );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : SaveMQOFile : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savemqoexit;
	}

	goto savemqoexit;
savemqoexit:
	if( hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( hfile );
		SetEndOfFile( hfile );
		CloseHandle( hfile );
		hfile = INVALID_HANDLE_VALUE;
	}
	if( m_tempmathead ){
		CMQOMaterial* delmat = m_tempmathead;
		CMQOMaterial* nextmat;
		while( delmat ){
			nextmat = delmat->next;
			delete delmat;
			delmat = nextmat;
		}
		m_tempmathead = 0;
	}


	if( matnoarray ){
		int* delelem;

		for( serino = 0; serino < shandler->s2shd_leng; serino++ ){
			delelem = *(matnoarray + serino);
			if( delelem ){
				free( delelem );
			}
		}
		
		free( matnoarray );

		matnoarray = 0;
	}


	return ret;

}

int CMQOFile::WriteMQOHeader( HANDLE hfile )
{
	int ret;
//Metasequoia Document
//Format Text Ver 1.0
//
//Scene {
//	pos 0.0000 0.0000 1500.0000
//	lookat 0.0000 0.0000 0.0000
//	head 1.7364
//	pich 0.0808
//	ortho 0
//	zoom2 5.0000
//	amb 0.250 0.250 0.250
//}

	strcpy_s( m_linechar, LINECHARLENG, "Metasequoia Document\r\nFormat Text Ver 1.0\r\n" );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( m_linechar, LINECHARLENG, "Scene {\r\n\tpos 0.0000 0.0000 1500.0000\r\n\tlookat 0.0000 0.0000 0.0000\r\n\thead 1.7364\r\n\tpich 0.0808" );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//strcpy( m_linechar, "\tortho 0\r\n\tzoom2 5.0000\r\n\tamb 0.250 0.250 0.250\r\n}" );
	if( shandler ){
		sprintf_s( m_linechar, LINECHARLENG, "\tortho 0\r\n\tzoom2 5.0000\r\n\tamb %f %f %f\r\n}", 
			shandler->m_scene_ambient.r, shandler->m_scene_ambient.g, shandler->m_scene_ambient.b );
	}else{
		strcpy_s( m_linechar, LINECHARLENG, "\tortho 0\r\n\tzoom2 5.0000\r\n\tamb 0.250 0.250 0.250\r\n}" );
	}
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int CMQOFile::CreateMaterial( int** matnoarray )
{
	int serino;
	CShdElem* selem;
	int ret;

	//materialの先頭要素を作成する。
	CMQOMaterial* topmat;
	topmat = new CMQOMaterial();
	if( !topmat ){
		DbgOut( "mqofile : CreateMaterial : topmat alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	strcpy_s( topmat->name, 256, "mat_dummy" );
	topmat->materialno = 0;
	topmat->next = 0;

	m_tempmathead = topmat;//!!!!!!

	if( matnoarray ){

		////
		for( serino = 0; serino < shandler->s2shd_leng; serino++ ){
			selem = (*shandler)( serino );

			if( (selem->notuse == 0) && ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2)) ){
				ret = selem->MakeFaceno2Materialno( &(shandler->m_scene_ambient), shandler->m_shader, shandler->m_sigmagicno, 
					matnoarray + serino, m_tempmathead, shandler->m_mathead );
				if( ret ){
					DbgOut( "mqofile : CreateMaterial : selem MakeFaceno2Materialno error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

	}
/// for dbg	
	//{
	//	CMQOMaterial* dumpmat = mathead;
	//	while( dumpmat ){
	//		DbgOut( "mqofile : CreateMaterial : dumpmat %s %d\n", dumpmat->name, dumpmat->materialno );
	//		dumpmat = dumpmat->next;
	//	}
	//}


	return 0;
}

int CMQOFile::WriteMQOMaterial( HANDLE hfile, CMQOMaterial* mathead )
{

//Material 16 {
//	"nas1" col(0.341 0.000 0.475 1.000) dif(1.000) amb(0.250) emi(0.250) spc(0.000) power(5.00)
//	"mat1" col(0.776 0.776 0.776 1.000) dif(0.180) amb(0.470) emi(0.910) spc(0.060) power(5.00) tex("sakana.jpg")
//}
	int ret;
	CMQOMaterial* curmat;
	char tempchar[MAX_PATH];

	if( m_legacymatflag != 0 ){

		int matcnt = 0;
		CMQOMaterial* befmat = 0;
		CMQOMaterial* lastmat = mathead;
		while( lastmat ){
			befmat = lastmat;
			lastmat = lastmat->next;
			matcnt++;
		}
		

		if( befmat ){
			//if( befmat->materialno != (matcnt - 1) ){
			//	DbgOut( "mqofile : WriteMQOMaterial : matcnt error !!!\n" );
			//	_ASSERT( 0 );
			//	return 1;
			//}


			sprintf_s( m_linechar, LINECHARLENG, "Material %d {", matcnt );
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
			_ASSERT( 0 );
				return 1;
			}

			int matno;
			curmat = mathead;
			for( matno = 0; matno < matcnt; matno++ ){

				sprintf_s( m_linechar, LINECHARLENG, "\t\"%s\" col(%.3f %.3f %.3f %.3f) dif(%.3f) amb(%.3f) emi(%.3f) spc(%.3f) power(%.2f)",
					curmat->name, curmat->col.r, curmat->col.g, curmat->col.b, curmat->col.a,
					curmat->dif, curmat->amb, curmat->emi, curmat->spc, curmat->power
					);
				if( curmat->tex[0] ){

					//拡張子の後の「_TEXID」をとる
					//フルパスはそのままver5032

					char writename[2048];
					ZeroMemory( writename, sizeof( char ) * 2048 );

					strcpy_s( writename, 2048, curmat->tex );

					char* periptr = 0;
					periptr = strrchr( writename, '.' );
					if( periptr ){
						char* underptr = 0;
						underptr = strchr( periptr, '_' );
						if( underptr ){
							*underptr = 0;
						}
					}

					sprintf_s( tempchar, MAX_PATH, " tex(\"%s\")", writename );
					strcat_s( m_linechar, LINECHARLENG, tempchar );
				}
				ret = WriteLinechar( hfile, 1 );
				if( ret ){
				_ASSERT( 0 );
					return 1;
				}
				curmat = curmat->next;
			}
			
			strcpy_s( m_linechar, LINECHARLENG, "}" );
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
			_ASSERT( 0 );
				return 1;
			}
		}
	}else{
		sprintf_s( m_linechar, LINECHARLENG, "Material %d {", shandler->m_materialcnt - 1 );
		ret = WriteLinechar( hfile, 1 );
		if( ret ){
		_ASSERT( 0 );
			return 1;
		}

		int matno;
		//curmat = mathead;
		for( matno = 0; matno < ( shandler->m_materialcnt - 1 ); matno++ ){

			curmat = GetMaterialFromNo( mathead, matno );//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if( !curmat ){
				DbgOut( "mqofile : WriteMQOMaterial : curmat NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			sprintf_s( m_linechar, LINECHARLENG, "\t\"%s\" ", curmat->name );
			
			if( curmat->vcolflag != 0 ){
				strcpy_s( tempchar, MAX_PATH, "vcol(1) " );
				strcat_s( m_linechar, LINECHARLENG, tempchar );
			}
			
			sprintf_s( tempchar, MAX_PATH, "col(%.3f %.3f %.3f %.3f) dif(%.3f) amb(%.3f) emi(%.3f) spc(%.3f) power(%.2f)",
				curmat->col.r, curmat->col.g, curmat->col.b, curmat->col.a,
				curmat->dif, curmat->amb, curmat->emi, curmat->spc, curmat->power
			);
			strcat_s( m_linechar, LINECHARLENG, tempchar );

			if( curmat->tex[0] ){

				//拡張子の後の「_TEXID」をとる
				//フルパスはそのままver5032

				char writename[2048];
				ZeroMemory( writename, sizeof( char ) * 2048 );

				strcpy_s( writename, 2048, curmat->tex );
				
				char* periptr = 0;
				periptr = strrchr( writename, '.' );
				if( periptr ){
					char* underptr = 0;
					underptr = strchr( periptr, '_' );
					if( underptr ){
						*underptr = 0;
					}
				}

				sprintf_s( tempchar, MAX_PATH, " tex(\"%s\")", writename );
				strcat_s( m_linechar, LINECHARLENG, tempchar );
			}
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
			_ASSERT( 0 );
				return 1;
			}

		}
			
		strcpy_s( m_linechar, LINECHARLENG, "}" );
		ret = WriteLinechar( hfile, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CMQOFile::WriteMQOBoneOnFrame( HANDLE hfile, int motid, int frameno, float srcmult )
{
	int ret;

	int jointnum = 0;
	int bonenum = 0;
	int onlyjointnum = 0;
	CShdElem* firstse;
	firstse = (*shandler)( 1 );
	ret = CountJointNum( &jointnum, &bonenum, &onlyjointnum );
	if( ret ){
		DbgOut( "mqofile : WriteMQOBoneOnFrame : CountJointNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (jointnum <= 0) || (bonenum <= 0) ){
		DbgOut( "mqofile : WriteMQOBoneOnFrame : bone not exist return !!!\n" );
		return 0;
	}


	ret = WriteObjectHeader( hfile, "BoneObject", 59.5f );
	if( ret ){
		DbgOut( "mqofile : WriteMQOBoneOnFrame : WriteObjectHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////// vertex
	//vertex 23 {
	//	0.000000 0.000000 0.000000
	//	0.000000 26.049999 0.000000
	//	...
	//}
	sprintf_s( m_linechar, LINECHARLENG, "\tvertex %d {", jointnum );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : WriteMQOBoneOnFrame : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = mhandler->SetCurrentMotion( shandler, motid, frameno );
	if( ret ){
		DbgOut( "mqofile : WriteMQOBoneOnFrame : mh SetCurrentMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	D3DXMATRIX iniwmat;
	D3DXMatrixIdentity( &iniwmat );


	int serino;
	CShdElem* selem;
	int mqono = 0;
	for( serino = 0; serino < shandler->s2shd_leng; serino++ ){
		selem = (*shandler)( serino );
		_ASSERT( selem );

		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			D3DXVECTOR3 trav;

			ret = selem->TransformOnlyWorld3( mhandler, iniwmat, &trav );
			if( ret ){
				DbgOut( "mqofile : WriteMQOBoneOnFrame : selem TransformOnlyWorld3 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			sprintf_s( m_linechar, LINECHARLENG, "\t\t%f %f %f", trav.x * srcmult, trav.y * srcmult, -trav.z * srcmult );
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
				DbgOut( "mqofile : WriteMQOBoneOnFrame : WriteLinechar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			selem->m_mqono = mqono;//!!!!!!!!!!!!!!!!!!!!
			mqono++;
		}
	}

	strcpy_s( m_linechar, LINECHARLENG, "}" );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : WriteMQOBoneOnFrame : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////// face
	//face 22 {
	//	2 V(0 1)
	//	2 V(1 2)
	//	...
	//}
	sprintf_s( m_linechar, LINECHARLENG, "\tface %d {", bonenum + onlyjointnum );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : WriteMQOBoneOnFrame : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	selem = (*shandler)( 0 );
	WriteMQOFaceFromSelemReq( hfile, selem );

	/***
	for( serino = 0; serino < shandler->s2shd_leng; serino++ ){
		selem = (*shandler)( serino );
		_ASSERT( selem );

		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			CPart* partptr;
			partptr = selem->part;
			if( !partptr ){
				DbgOut( "mqofile : WriteMQOBoneOnFrame : partptr NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( partptr->bonenum > 0 ){
				int bno;
				CShdElem* childelem;
				for( bno = 0; bno < partptr->bonenum; bno++ ){
					childelem = partptr->GetChildSelem( shandler, bno );
					if( !childelem ){
						DbgOut( "mqofile : WriteMQOBoneOnFrame : part GetChildSelem ; child NULL error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					
					sprintf( m_linechar, "\t\t2 V(%d %d)", selem->m_mqono, childelem->m_mqono );
					ret = WriteLinechar( hfile, 1 );
					if( ret ){
						DbgOut( "mqofile : WriteMQOBoneOnFrame : WriteLinechar error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}else{
				//ボーンをなさない、ジョイントも書き出す。

				sprintf( m_linechar, "\t\t2 V(0 %d)", selem->m_mqono );
				ret = WriteLinechar( hfile, 1 );
				if( ret ){
					DbgOut( "mqofile : WriteMQOBoneOnFrame : WriteLinechar error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}
	}
	***/

	strcpy_s( m_linechar, LINECHARLENG, "}" );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : WriteMQOObject : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//////

	strcpy_s( m_linechar, LINECHARLENG, "}" );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : WriteMQOObject : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CMQOFile::WriteMQOObjectOnFrame( HANDLE hfile, int** matnoarray, int motid, int frameno, float srcmult, int chkmorph )
{
	int ret;
	int serino;
	CShdElem* selem;
	CTreeElem2* telem;

	for( serino = 0; serino < shandler->s2shd_leng; serino++ ){
		selem = (*shandler)( serino );

		int pmwrite = 0;
		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			if( chkmorph == 0 ){
				pmwrite = 1;
			}else{
				if( selem->m_mtype == M_NONE ){
					pmwrite = 1;
				}else{
					pmwrite = 0;
				}
			}
		}

		if( (selem->notuse == 0) && (pmwrite == 1) ){
			
			telem = (*thandler)( serino );
			ret = WriteObjectHeader( hfile, telem->name, selem->facet );
			if( ret ){
				DbgOut( "mqofile : WriteMQOObject : WriteObjectHeader error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int writerevflag;
			if( selem->clockwise == 3 ){
				writerevflag = 1;
			}else{
				writerevflag = 0;
			}
			if( matnoarray ){
				ret = selem->WriteMQOObjectOnFrame( hfile, *(matnoarray + serino), motid, frameno, shandler, mhandler, srcmult, writerevflag );
			}else{
				ret = selem->WriteMQOObjectOnFrame( hfile, 0, motid, frameno, shandler, mhandler, srcmult, writerevflag );
			}
			if( ret ){
				DbgOut( "mqofile : WriteMQOObject : WriteMQOObject error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			

			strcpy_s( m_linechar, LINECHARLENG, "}" );
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
				DbgOut( "mqofile : WriteMQOObject : WriteLinechar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else if( (selem->notuse == 0) && (chkmorph == 1) && (selem->type == SHDMORPH) ){
			telem = (*thandler)( serino );
			ret = WriteObjectHeader( hfile, telem->name, selem->facet );
			if( ret ){
				DbgOut( "mqofile : WriteMQOObject : WriteObjectHeader error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			CMorph* morph;
			morph = selem->morph;
			if( !morph ){
				_ASSERT( 0 );
				return 1;
			}

			int writerevflag;
			if( selem->clockwise == 3 ){
				writerevflag = 1;
			}else{
				writerevflag = 0;
			}
			if( matnoarray ){
				ret = selem->WriteMQOObjectOnFrame( hfile, *(matnoarray + morph->m_baseelem->serialno), motid, frameno, shandler, mhandler, srcmult, writerevflag );
			}else{
				ret = selem->WriteMQOObjectOnFrame( hfile, 0, motid, frameno, shandler, mhandler, srcmult, writerevflag );
			}
			if( ret ){
				DbgOut( "mqofile : WriteMQOObject : WriteMQOObject error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			

			strcpy_s( m_linechar, LINECHARLENG, "}" );
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
				DbgOut( "mqofile : WriteMQOObject : WriteLinechar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


		}
	}


	return 0;
}

void CMQOFile::WriteMQOVertexFromBVHReq( HANDLE hfile, CBVHElem* srcbe, int* mqonoptr )
{
	int ret;

	srcbe->mqono = *mqonoptr;

//DbgOut( "mqofile : WriteMQOVertexFromBVHReq : %s, %d\r\n", srcbe->name, srcbe->mqono );

	sprintf_s( m_linechar, LINECHARLENG, "\t\t%f %f %f", srcbe->position.x, srcbe->position.y, -srcbe->position.z );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : WriteMQOVertexFromBVHReq : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		return;
	}


////////
	if( srcbe->child ){
		(*mqonoptr)++;
		WriteMQOVertexFromBVHReq( hfile, srcbe->child, mqonoptr );
	}
	if( srcbe->brother ){
		(*mqonoptr)++;
		WriteMQOVertexFromBVHReq( hfile, srcbe->brother, mqonoptr );
	}
}

void CMQOFile::WriteMQOFaceFromSelemReq( HANDLE hfile, CShdElem* srcse )
{
	int ret;

	if( srcse->parent ){
		if( !(srcse->parent->IsJoint()) && srcse->IsJoint() && ( !(srcse->child) || ( srcse->child && !(srcse->child->IsJoint()) ) )  ){
			//onlyjoint
			sprintf_s( m_linechar, LINECHARLENG, "\t\t2 V(0 %d)", srcse->m_mqono );
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
				DbgOut( "mqofile : WriteMQOFaceFromSelemReq : WriteLinechar error !!!\n" );
				_ASSERT( 0 );
				return;
			}
		}

	}

////////
	if( srcse->child ){

		if( srcse->IsJoint() && srcse->child->IsJoint() ){
			sprintf_s( m_linechar, LINECHARLENG, "\t\t2 V(%d %d)", srcse->m_mqono, srcse->child->m_mqono );
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
				DbgOut( "mqofile : WriteMQOFaceFromSelemReq : WriteLinechar error !!!\n" );
				_ASSERT( 0 );
				return;
			}
		}
		WriteMQOFaceFromSelemReq( hfile, srcse->child );
	}
	if( srcse->brother ){

		//if( srcse->parent ){
		if( srcse->parent && srcse->parent->IsJoint() && srcse->brother->IsJoint() ){
			sprintf_s( m_linechar, LINECHARLENG, "\t\t2 V(%d %d)", srcse->parent->m_mqono, srcse->brother->m_mqono );
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
				DbgOut( "mqofile : WriteMQOFaceFromBVHReq : WriteLinechar error !!!\n" );
				_ASSERT( 0 );
				return;
			}
		}

		WriteMQOFaceFromSelemReq( hfile, srcse->brother );
	}
}



void CMQOFile::WriteMQOFaceFromBVHReq( HANDLE hfile, CBVHElem* srcbe )
{
	int ret;

		//2 V(0 1)

////////
	if( srcbe->child ){

		sprintf_s( m_linechar, LINECHARLENG, "\t\t2 V(%d %d)", srcbe->mqono, srcbe->child->mqono );
		ret = WriteLinechar( hfile, 1 );
		if( ret ){
			DbgOut( "mqofile : WriteMQOFaceFromBVHReq : WriteLinechar error !!!\n" );
			_ASSERT( 0 );
			return;
		}

		WriteMQOFaceFromBVHReq( hfile, srcbe->child );
	}
	if( srcbe->brother ){

		if( srcbe->parent ){
			sprintf_s( m_linechar, LINECHARLENG, "\t\t2 V(%d %d)", srcbe->parent->mqono, srcbe->brother->mqono );
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
				DbgOut( "mqofile : WriteMQOFaceFromBVHReq : WriteLinechar error !!!\n" );
				_ASSERT( 0 );
				return;
			}
		}

		WriteMQOFaceFromBVHReq( hfile, srcbe->brother );
	}
}


int CMQOFile::WriteMQOObject( HANDLE hfile, int** matnoarray )
{
	int ret;
	int serino;
	CShdElem* selem;
	CTreeElem2* telem;

	for( serino = 0; serino < shandler->s2shd_leng; serino++ ){
		selem = (*shandler)( serino );

		if( (selem->notuse == 0) && ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2)) ){
			
			telem = (*thandler)( serino );
			ret = WriteObjectHeader( hfile, telem->name, selem->facet );
			if( ret ){
				DbgOut( "mqofile : WriteMQOObject : WriteObjectHeader error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( matnoarray ){
				ret = selem->WriteMQOObject( hfile, *(matnoarray + serino) );
			}else{
				ret = selem->WriteMQOObject( hfile, 0 );
			}
			if( ret ){
				DbgOut( "mqofile : WriteMQOObject : WriteMQOObject error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			

			strcpy_s( m_linechar, LINECHARLENG, "}" );
			ret = WriteLinechar( hfile, 1 );
			if( ret ){
				DbgOut( "mqofile : WriteMQOObject : WriteLinechar error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}

int CMQOFile::WriteObjectHeader( HANDLE hfile, char* nameptr, float srcfacet )
{

	if( !nameptr || !*nameptr ){
		DbgOut( "mqofile : WriteObjectHeader : nameptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int nameleng, charno, setno;
	char tempname[MAX_PATH];

	// 名前の中に含まれる、" を取り除いて、コピーする。
	ZeroMemory( tempname, MAX_PATH );
	nameleng = (int)strlen( nameptr );

	if( nameleng >= MAX_PATH ){
		DbgOut( "mqofile : WriteObjectHeader : nameleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	setno = 0;
	for( charno = 0; charno < nameleng; charno++ ){
		char curchar;
		curchar = *( nameptr + charno );
		
		if( curchar != '\"' ){
			tempname[setno] = curchar;
			setno++;
		}
	}

//Object "obj1" {
//	visible 15
//	locking 0
//	shading 1
//	facet 59.5
//	color 0.898 0.498 0.698
//	color_type 0
	
	sprintf_s( m_linechar, LINECHARLENG, "Object \"%s\" {", tempname );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : WriteObjectHeader : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	sprintf_s( m_linechar, LINECHARLENG, "\tvisible 15\r\n\tlocking 0\r\n\tshading 1\r\n\tfacet %f\r\n\tcolor 0.898 0.498 0.698\r\n\tcolor_type 0", srcfacet );
	ret = WriteLinechar( hfile, 1 );
	if( ret ){
		DbgOut( "mqofile : WriteObjectHeader : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}



int CMQOFile::WriteLinechar( HANDLE hfile, int addreturn )
{
	unsigned long wleng, writeleng;

	if( addreturn )
		strcat_s( m_linechar, LINECHARLENG, "\r\n" );

	wleng = (int)strlen( m_linechar );
	if( wleng >= LINECHARLENG ){
		DbgOut( "mqofile : WriteLinechar : wleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	WriteFile( hfile, m_linechar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CMQOFile::CountJointNum( int* jointnumptr, int* bonenumptr, int* onlyjointnumptr )
{
	//int ret;

	*jointnumptr = 0;
	*bonenumptr = 0;
	*onlyjointnumptr = 0;

	int serino;
	CShdElem* selem;
	for( serino = 0; serino < shandler->s2shd_leng; serino++ ){
		selem = (*shandler)( serino );
		_ASSERT( selem );

		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			(*jointnumptr)++;//!!!!!!!!!!!!

			CPart* partptr;
			partptr = selem->part;

			if( !partptr ){
				DbgOut( "mqofile : CountJointNum : part NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			(*bonenumptr) += partptr->bonenum; 

			if( partptr->bonenum == 0 ){
				(*onlyjointnumptr)++;//!!!!!!!!!!!!!!!!!!!!!
			}

		}
	}

	
	return 0;
}

int CMQOFile::ReplaceBdefSdef()
{
	int ret;
	int serino;
	CTreeElem2* telem;
	CShdElem* selem;

	char tempchar[2048];
	int leng;

	char defpat[2][10] = {
		"sdef:",
		"bdef:"
	};
	int patno;
	int patleng;
	int cmp;
	


	for( serino = 0; serino < thandler->s2e_leng; serino++ ){
		telem = (*thandler)( serino );
		selem = (*shandler)( serino );

		selem->m_mikodef = MIKODEF_NODEF;

		for( patno = 0; patno < 2; patno++ ){
			patleng = (int)strlen( defpat[patno] );
			leng = (int)strlen( telem->name );

			cmp = strncmp( telem->name, defpat[patno], patleng );
			if( (cmp == 0) && (leng > patleng) ){

				int ch = '-';
				char* minusptr;
				minusptr = strchr( telem->name, ch );

				if( !minusptr ){
					strcpy_s( tempchar, 2048, telem->name );
					ret = telem->SetName( tempchar + patleng );
					if( ret ){
						DbgOut( "mqofile : ReplaceBdefSdef : te SetName error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					if( patno == 0 ){
						selem->m_mikodef = MIKODEF_SDEF;
					}else if( patno == 1 ){
						selem->m_mikodef = MIKODEF_BDEF;
					}
				}else{
					//sdef, bdefと、影響ボーン名が両方指定されていた場合は、影響ボーン名を優先し、sdef, bdefは無視する。

					strcpy_s( tempchar, 2048, telem->name );
					ret = telem->SetName( tempchar + patleng );
					if( ret ){
						DbgOut( "mqofile : ReplaceBdefSdef : te SetName error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					selem->m_mikodef = MIKODEF_NONE;//!!!!!!!!!

				}
			}
		}
	}

	return 0;
}



int CMQOFile::SetMikoAnchorName()
{
	
	int ret;
	int serino;
	CShdElem* selem;
	CTreeElem2* telem;
	CPolyMesh2* pm2;
	int isancname;

	char targetname[256];
	char applyname[256];

	char Lpat[20] = "[L]_X+";
	char Rpat[20] = "[R]_X-";

	char tempchar[2048];

	for( serino = 0; serino < shandler->s2shd_leng; serino++ ){
		telem = (*thandler)( serino );
		selem = (*shandler)( serino );

		if( selem->type == SHDPOLYMESH2 ){

			isancname = telem->IsAnchorName( targetname, applyname );

			if( isancname != 0 ){

				selem->m_anchorflag = 1;//!!!!!!!
				selem->notuse = 1;//!!!!!!!!!!!!!!!!
				

				pm2 = selem->polymesh2;
				_ASSERT( pm2 );

				ret = pm2->CalcAnchorCenter();
				if( ret ){
					DbgOut( "mqofile : SetMikoAnchorName : pm2 CalcAnchorCenter error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( isancname == 1 ){
					//[]でない適用ボーン
					
					//名前は、そのまま

				}else if( isancname == 2 ){
					//適用ボーンは、[]
				
					int leng;
					leng = (int)strlen( telem->name );
					*( telem->name + leng - 2 ) = 0;
				
					if( pm2->m_center.x >= 0.0f ){
						sprintf_s( tempchar, 2048, "%s%s", telem->name, Lpat );
						ret = telem->SetName( tempchar );
						if( ret ){
							DbgOut( "mqofile : SetMikoAnchorName : te SetName error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						sprintf_s( tempchar, 2048, "%s%s", telem->name, Rpat );
						ret = telem->SetName( tempchar );
						if( ret ){
							DbgOut( "mqofile : SetMikoAnchorName : te SetName error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}

				}else if( isancname == 3 ){
					//適用ボーン指定無し

					//材質名を見て、同じボーン名があれば、そのボーンを適用
					char* nameptr;
					shandler->m_mathead->GetName( pm2->m_anchormaterialno, &nameptr );
					int leng;
					leng = (int)strlen( nameptr );
					
					char bonename[1024];
					int cmp;
					if( leng > 2 )
						cmp = strcmp( nameptr + leng - 2, "[]" );
					else
						cmp = 1;

					if( cmp ){
						strcpy_s( bonename, 1024, nameptr );
					
					}else{
						
						strcpy_s( bonename, 1024, nameptr );
						*( bonename + leng - 2 ) = 0;
						
						if( pm2->m_center.x >= 0.0f ){
							strcat_s( bonename, 1024, Lpat );
						}else{
							strcat_s( bonename, 1024, Rpat );
						}

					}

					int boneno;
					ret = thandler->GetBoneNoByName( bonename, &boneno, shandler, 0 );
					if( ret ){
						DbgOut( "mqofile : SetMikoAnchorName : th GetBoneNoByName error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					if( boneno > 0 ){
						sprintf_s( tempchar, 2048, "%s-%s", telem->name, bonename );
						ret = telem->SetName( tempchar );
						if( ret ){
							DbgOut( "mqofile : SetMikoAnchorName : te SetName error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						//材質名とおなじボーン名が無かったら、一番近いボーン
						CShdElem* nearbone;
						ret = shandler->FindNearestBoneFromAnchor( selem, &nearbone );
						if( ret ){
							DbgOut( "mqofile : SetMikoAnchorName : sh FindNearestBoneFromAnchor error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}

						if( nearbone ){
							CTreeElem2* neartelem;
							neartelem = (*thandler)( nearbone->serialno );
							_ASSERT( neartelem );

							sprintf_s( tempchar, 2048, "%s-%s",	telem->name, neartelem->name );
							ret = telem->SetName( tempchar );
							if( ret ){
								DbgOut( "mqofile : SetMikoAnchorName : te SetName error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}

						}else{
							//ボーンが1個もなかったら、そのまま
						}
					}

				}else{
					DbgOut( "mqofile : SetMikoAnchorName : anchornametype error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}
	}

	return 0;
}

int CMQOFile::ReplaceLRName()
{
	int ret;

	char lrpat_bef[4][10] = {
		"[l]",
		"[L]",
		"[r]",
		"[R]"
	};

	char lrpat_aft[4][10] = {
		"[L]_X+",
		"[L]_X+",
		"[R]_X-",
		"[R]_X-"
	};

	char tempchar[2048];

	int serino;
	CTreeElem2* telem;
	int leng;

	for( serino = 0; serino < thandler->s2e_leng; serino++ ){
		telem = (*thandler)( serino );
		leng = (int)strlen( telem->name );

		if( leng > 3 ){
			int patno;
			int cmp;
			for( patno = 0; patno < 4; patno++ ){
				cmp = strcmp( telem->name + leng - 3, lrpat_bef[patno] );
				if( cmp == 0 ){
					strncpy_s( tempchar, 2048, telem->name, leng - 3 );
					*( tempchar + leng - 3 ) = 0;
					strcat_s( tempchar, 2048, lrpat_aft[patno] );

					ret = telem->SetName( tempchar );
					if( ret ){
						DbgOut( "mqofile : ReplaceLRName : te SetName error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}
	}

	return 0;
}

CMQOMaterial* CMQOFile::GetMaterialFromNo( CMQOMaterial* mathead, int matno )
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
