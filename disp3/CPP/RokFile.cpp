#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <windows.h>

#include "RokFile.h"
//#include "C:\program files\microsoft platform sdk\samples\multimedia\direct3d\rokdebone\LoadParamDlg.h"
//#include "C:\pgfile8\rokdebone\LoadParamDlg.h"
#include "c:\pgfile9\RokDeBone2DX\LoadParamDlg.h"


#include <treehandler2.h>
#include <shdhandler.h>
#include <mothandler.h>
#include <mqomaterial.h>

#include <shdelem.h>
#include <polymesh.h>

#include <crtdbg.h>


#define MULTIPLEELEM	650.0f


///// extern
extern int LoadRokFile( char* fname, CRokFile* rfptr );
extern int g_toon1matcnt;

/////////////

CRokFile::CRokFile()
{
	InitParams();

}
CRokFile::~CRokFile()
{
	DestroyObjs();
}

void CRokFile::DestroyObjs()
{
	if( pointbuf != NULL ){
		free( pointbuf );
		pointbuf = NULL;
	}
	if( linebuf != NULL ){
		free( linebuf );
		linebuf = NULL;
	}
	if( facebuf != NULL ){
		free( facebuf );
		facebuf = NULL;
	}
}
	
void CRokFile::InitParams()
{
	pointbuf = NULL;
	pointbufleng = 0;
	rokpointnum = 0;
	linebuf = NULL;
	linebufleng = 0;
	roklinenum = 0;
	facebuf = NULL;
	facebufleng = 0;
	rokfacenum = 0;
	ZeroMemory( palletbuf, sizeof( ROKPALLET ) * 13 );

	m_palletno = 0;

	m_thandler = 0;
	m_shandler = 0;
	m_mhandler = 0;

	curdepth = 0;
	curseri = 0;
	curtype = 0;
	int ret;
	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	m_multiple = 0.0f;
	m_swapxz = 0;
	m_centerx = 0.0f;
	m_centerz = 0.0f;

	m_invz = 0;
	m_divmode = 0;

	m_chkinv = 0;

	m_minvec.SetMem( 1e6, 1e6, 1e6 );
	m_maxvec.SetMem( -1e6, -1e6, -1e6 );

	m_materialoffset = 0;
}

int CRokFile::LoadRokFile( char* fname, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, HWND appwnd, int offset )
{
	g_toon1matcnt = 0;//!!!!!!!!!!!!!!!!


	DestroyObjs();
	InitParams();
	
	if( !lpth || !lpsh || !lpmh )
		return 1;
	m_thandler = lpth;
	m_shandler = lpsh;
	m_mhandler = lpmh;
	m_offset = offset;

	int ret;
	ret = ::LoadRokFile( fname, this );
	if( ret ){
		if( ret == D3DAPPERR_MEDIANOTFOUND ){
			return ret;
		}else{
			_ASSERT( 0 );
			return ret;
		}
	}

	DbgOut( "RokFile : LoadRokFile : rokpointnum %d, roklinenum %d, rokfacenum %d, rokpalletno %d\n",
		rokpointnum, roklinenum, rokfacenum, m_palletno );

	//pallet情報の無いrokには、defaultのpallet情報を付加する。
	if( m_palletno == 0 ){
		char moduledir[1024];
		int leng;
		ZeroMemory( moduledir, 1024 );
		leng = GetEnvironmentVariable( (LPCTSTR)"MEDIADIR", (LPTSTR)moduledir, 1024 );
		_ASSERT( leng );
		char palletname[1024];
		ZeroMemory( palletname, 1024 );
		strcpy_s( palletname, 1024, moduledir );
		strcat_s( palletname, 1024, s_defpalletrok );
		ret = ::LoadRokFile( palletname, this );
		if( ret ){
			DbgOut( "RokFile : LoadRokFile : load defpallet error !!! %d %s\n",
				ret, palletname );
			_ASSERT( 0 );
			return ret;
		}else{
			DbgOut( "RokFile : LoadRokFile : load default pallet %d\n", m_palletno );
		}
	}

	ret = CheckUpDataRange();
	if( ret ){
		_ASSERT( 0 );
		return ret;
	}

	CLoadParamDlg	dlg( m_minvec, m_maxvec );
	if( appwnd )
		::SendMessage( appwnd, WM_USER_ENABLE_MENU, 0, 0 );
	dlg.DoModal();
	if( appwnd )
		::SendMessage( appwnd, WM_USER_ENABLE_MENU, 1, 0 );
	float multiple;
	int swapxz;
	int centeringx;
	int centeringz;
	int invz;
	int divmode;
	int chkinv;
	multiple = dlg.m_multiple;
	swapxz = dlg.m_swapxz;
	centeringx = dlg.m_centerx;
	centeringz = dlg.m_centerz;
	invz = dlg.m_invz;
	divmode = dlg.m_divmode;
	chkinv = dlg.m_chkinv;

	ret = SetLoadParams( multiple, swapxz, centeringx, centeringz, invz, divmode, chkinv );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( offset == 0 ){
		ret = m_shandler->InitMQOMaterial();
		if( ret ){
			DbgOut( "rokfile : LoadRokFile : sh InitMQOMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	m_materialoffset = m_shandler->m_materialcnt - 1;

	ret = RokPallet2MQOMaterial();
	if( ret ){
		DbgOut( "rokfile : LoadRokFile : RokPallet2MQOMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = MakeShdTree();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
/////////


	ret = m_shandler->SetMikoAnchorApply( m_thandler );
	if( ret ){
		DbgOut( "rokfile : LoadRokFile : sh SetMikoAnchorApply error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->Anchor2InfScope( m_thandler, m_mhandler );
	if( ret ){
		DbgOut( "rokfile : LoadRokFile : sh Anchor2InfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->CreateShdBBoxIfNot( m_thandler, m_mhandler );
	if( ret ){
		DbgOut( "rokfile : LoadRokFile : sh CreateShdBBoxIfNot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//////////////////
	int no_pm2orgno;
	ret = m_shandler->CheckOrgNoBuf( &no_pm2orgno );
	if( ret ){
		DbgOut( "sigfile : LoadRokFile : shandler : CheckOrgNoBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( no_pm2orgno ){
		m_shandler->m_im2enableflag = 0;
	}else{
		m_shandler->m_im2enableflag = 1;
	}


	return 0;
}

int CRokFile::ReadRokHeader( char* buf )
{
	if( m_offset != 0 )
		return 0;
	
	//DbgOut( "RokFile : ReadRokHeader : %s\n", buf );
	char* bufptr = buf;
	int skipno;
	for( skipno = 0; skipno < 11; skipno++ ){
		bufptr = SkipOnLine( bufptr );
		if( !bufptr ){
			_ASSERT( 0 );
			return 1;
		}
	}

	float ratex, ratey;
	bufptr = GetFloatData( bufptr, &ratex );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}

	bufptr = GetFloatData( bufptr, &ratey );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}

	float aspect = ratex / ratey;
	m_shandler->m_aspect = aspect;
	DbgOut( "RokFile : ReadRokHeader : aspect %f\n", aspect );

	return 0;
}
int CRokFile::ReadPoint( char* buf )
{
	int ret;
	if( rokpointnum >= pointbufleng ){
		ret = GrowUpPointBuf();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( rokpointnum == 0 ){
		//pointno とbufの添え字を同じにするために、先頭データはダミー。
		pointbuf->pointno = 0;
		pointbuf->groupno = 0;
		pointbuf->dispflag = 0;
		pointbuf->dummy = 0;
		pointbuf->dimensionflag = 0;
		pointbuf->x = 0.0f;
		pointbuf->y = 0.0f;
		pointbuf->z = 0.0f;

		rokpointnum++;
	}


	rokpointnum++;
	int pointno = rokpointnum - 1;
	ROKPOINT* currp = pointbuf + pointno;
	if( !currp ){
		_ASSERT( 0 );
		return 1;
	}

	/**
    1               //　点の番号
    0               //　グループ化した時のグループ番号（グループは階層化不可）
    0               //　０なら表示。１なら隠す。
    0               //　ダミー
    2               //　０なら２次元の点。０以外なら左右対称な点の番号。
    -0.354941037    //　点のＸ座標
     0.270283061    //　点のＹ座標
     0.095652076    //　点のＺ座標
	**/
	char* bufptr = buf;
	bufptr = GetIntData( bufptr, &(currp->pointno) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	if( pointno != currp->pointno ){
		DbgOut( "RokFile : ReadPoint : pointno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	bufptr = GetIntData( bufptr, &(currp->groupno) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currp->dispflag) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currp->dummy) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currp->dimensionflag) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetFloatData( bufptr, &(currp->x) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetFloatData( bufptr, &(currp->y) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetFloatData( bufptr, &(currp->z) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CRokFile::ReadLine( char* buf )
{
	int ret = 0;
	if( roklinenum >= linebufleng ){
		ret = GrowUpLineBuf();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( roklinenum == 0 ){
		//lineno と bufの添え字を同じにするために、先頭データはダミー。
		linebuf->p1 = 0;
		linebuf->p2 = 0;
		linebuf->groupno = 0;
		linebuf->dispflag = 0;

		roklinenum++;
	}

	roklinenum++;
	int lineno = roklinenum - 1;
	ROKLINE* currl = linebuf + lineno;
	if( !currl ){
		_ASSERT( 0 );
		return 1;
	}

	/**
    3               //　つながる点（その１）の番号
    1               //　つながる点（その２）の番号
    0               //　グループの番号（０の時はグループ化されていない）
    0               //　０のとき表示　１のとき隠す
	**/

	char* bufptr = buf;
	bufptr = GetIntData( bufptr, &(currl->p1) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currl->p2) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currl->groupno) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currl->dispflag) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CRokFile::ReadFace( char* buf )
{
	int ret;
	if( rokfacenum >= facebufleng ){
		ret = GrowUpFaceBuf();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( rokfacenum == 0 ){
		//faceno と bufの添え字を同じにするために、先頭データはダミー。
		facebuf->pointnum = 0;
		facebuf->colorno = 0;
		facebuf->faceno = 0;
		facebuf->line[0] = 0;
		facebuf->line[1] = 0;
		facebuf->line[2] = 0;
		facebuf->line[3] = 0;

		rokfacenum++;
	}

	rokfacenum++;
	int faceno = rokfacenum - 1;
	ROKFACE* currf = facebuf + faceno;
	if( !currf ){
		_ASSERT( 0 );
		return 1;
	}

	/**
    3               //　面の画数
    2               //　色の番号
    1               //　面の番号
    5               //　面の輪郭線その１の番号
    3               //　面の輪郭線その２の番号
    4               //　面の輪郭線その３の番号
	**/
	char* bufptr = buf;
	bufptr = GetIntData( bufptr, &(currf->pointnum) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currf->colorno) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currf->faceno) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	if( faceno != currf->faceno ){
		DbgOut( "RokFile : ReadFace : faceno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	bufptr = GetIntData( bufptr, &(currf->line[0]) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currf->line[1]) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currf->line[2]) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}	
	if( (currf->pointnum) == 4 ){
		bufptr = GetIntData( bufptr, &(currf->line[3]) );
		if( !bufptr ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CRokFile::ReadPallet( char* buf )
{
	if( m_palletno >= 13 ){
		_ASSERT( 0 );
		return 1;
	}

	ROKPALLET* currp = palletbuf + m_palletno;
	if( !currp ){
		_ASSERT( 0 );
		return 1;
	}

	/**
	255                 //　１番目の色：明るい所の色(R) (0～255)
	255                 //　１番目の色：明るい所の色(G)
	255                 //　１番目の色：明るい所の色(B)
	128                 //　１番目の色：暗い所の色(R)
	128                 //　１番目の色：暗い所の色(G)
	128                 //　１番目の色：暗い所の色(B)
	0                   //　予約
	**/
	char* bufptr = buf;
	bufptr = GetIntData( bufptr, &(currp->diffuse[0]) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currp->diffuse[1]) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currp->diffuse[2]) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currp->ambient[0]) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currp->ambient[1]) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currp->ambient[2]) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}
	bufptr = GetIntData( bufptr, &(currp->reserved) );
	if( !bufptr ){
		_ASSERT( 0 );
		return 1;
	}

	m_palletno++;

	return 0;
}

int CRokFile::GrowUpPointBuf()
{
	int newleng = pointbufleng + BUF_BLOCK_LENG;
	pointbuf = (ROKPOINT*)realloc( pointbuf, sizeof(ROKPOINT) * newleng );
	if( pointbuf == NULL ){
		DbgOut( "CRokFile : GrowUpPointBuf : pointbuf realloc error !!!\n" );
		return 1;
	}
	pointbufleng = newleng;
	return 0;
}

int CRokFile::GrowUpLineBuf()
{
	int newleng = linebufleng + BUF_BLOCK_LENG;
	linebuf = (ROKLINE*)realloc( linebuf, sizeof(ROKLINE) * newleng );
	if( linebuf == NULL ){
		DbgOut( "CRokFile : GrowUpLineBuf : linebuf realloc error !!!\n" );
		return 1;
	}
	linebufleng = newleng;
	return 0;
}

int CRokFile::GrowUpFaceBuf()
{
	int newleng = facebufleng + BUF_BLOCK_LENG;
	facebuf = (ROKFACE*)realloc( facebuf, sizeof(ROKFACE) * newleng );
	if( facebuf == NULL ){
		DbgOut( "CRokFile : GrowUpFaceBuf : facebuf realloc error !!!\n" );
		return 1;
	}
	facebufleng = newleng;
	return 0;
}

char* CRokFile::GetIntData( char* buf, int* dstint )
{
	char* startptr = buf;
	while( isspace( *startptr ) )
		startptr++;

	int ch = '\r';
	char* endptr;
	endptr = strchr( startptr, ch );
	if( !endptr ){
		_ASSERT( 0 );
		return 0;
	}
	char tempchar[1024];
	ZeroMemory( tempchar, 1024 );
	int leng = (int)(endptr - startptr);
	if( leng >= 1024 ){
		_ASSERT( 0 );
		return 0;
	}
	strncpy_s( tempchar, 1024, startptr, leng );
	
	*dstint = atoi( tempchar );

	return (endptr + 1);
}

char* CRokFile::SkipOnLine( char* buf )
{
	int ch = '\r';
	char* endptr;
	endptr = strchr( buf, ch );
	if( !endptr ){
		_ASSERT( 0 );
		return 0;
	}
	return (endptr + 1);
}


char* CRokFile::GetFloatData( char* buf, float* dstfloat )
{
	char* startptr = buf;
	while( isspace( *startptr ) )
		startptr++;

	int ch = '\r';
	char* endptr;
	endptr = strchr( startptr, ch );
	if( !endptr ){
		_ASSERT( 0 );
		return 0;
	}
	char tempchar[1024];
	ZeroMemory( tempchar, 1024 );
	int leng = (int)(endptr - startptr);
	if( leng >= 1024 ){
		_ASSERT( 0 );
		return 0;
	}
	strncpy_s( tempchar, 1024, startptr, leng );
	
	*dstfloat = (float)atof( tempchar );

	return (endptr + 1);
}
/////////////
int CRokFile::MakeShdTree()
{
	int ret;

	if( m_divmode != 0 ){
		ret = SetGroupMax();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		m_groupmax = 0;
	}
	
	if( m_offset == 0 )
		curseri = 0;
	else
		curseri = m_offset - 1;

	m_thandler->Start( m_offset );

	// treeをまとめるpartの作成。
	ret = m_thandler->Begin();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = MakePart( "パート" );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( m_divmode == 0 ){
		// 色情報ごとのpolymeshの作成。
		int colorno;
		for( colorno = 0; colorno <= 12; colorno++ ){
			ret = AddPolyMesh( colorno, -1 );
			if( ret ){
				DbgOut( "RokFile : MakeShdTree : AddPolyMesh %d error !!!\n", colorno );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{
		int gno;
		for( gno = 0; gno <= m_groupmax; gno++ ){
			ret = m_thandler->Begin();
			_ASSERT( !ret );
			char partname[256];
			ZeroMemory( partname, 256 );
			sprintf_s( partname, 256, "グループ%d", gno );
			ret = MakePart( partname );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}

			int colorno;
			for( colorno = 0; colorno <= 12; colorno++ ){
				ret = AddPolyMesh( colorno, gno );
				if( ret ){
					DbgOut( "RokFile : MakeShdTree : AddPolyMesh %d %d error !!!\n", 
						gno, colorno );
					_ASSERT( 0 );
					return 1;
				}
			}
			
			ret = m_thandler->End();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	ret = AddBone();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	// treeをまとめるpartのend
	ret = m_thandler->End();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	// finish
	ret = m_shandler->SetChain( m_offset );// treehandlerのchainが出来あがってから。
	if( ret ){
		DbgOut( "RokFile : MakeShdTree : finish : shandler->SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->SetClockwise();
	if( ret ){
		DbgOut( "RokFile : MakeShdTree : finish : shandler->SetClockwise error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->SetChain( m_offset );
	if( ret ){
		DbgOut( "RokFile : MakeShdTree : finish : mhandler->SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}
int CRokFile::Add2Tree( char* nameptr, int hasmotion )
{
	int ret;
	ret = tempinfo.ResetParams();
	if( ret )
		return 1;

	curseri++; // !!!!!!!!!!!!

	// treehandler
	ret = m_thandler->AddTree( nameptr, curseri );
	if( curseri != ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_thandler->SetElemType( curseri, curtype );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	// shdhandler
	ret = m_shandler->AddShdElem( curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	

	ret = tempinfo.SetMem( curtype, BASE_TYPE );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}	

	// mothandler
	ret = m_mhandler->AddMotionCtrl( curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->SetHasMotion( curseri, hasmotion );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CRokFile::MakePart( char* name )
{
	curdepth = m_thandler->GetCurDepth();
	//char name[1024];
	//ZeroMemory( name, 1024 );
	//strcpy( name, "パート" );
	curtype = SHDPART;

	int ret;
	ret = Add2Tree( name, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = tempinfo.SetMem( 0, MESHI_HASCOLOR );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_shandler->Init3DObj( curseri, &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CRokFile::MakeBoneElem( ROKPOINT* pointptr, int boneno )
{
	int ret;
	ret = m_thandler->Begin();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	curdepth = m_thandler->GetCurDepth();
	char name[1024];
	ZeroMemory( name, 1024 );
	//sprintf( name, "ボールジョイント%d", boneno );
	sprintf_s( name, 1024, "ジョイント_シリアル%d", curseri + 1 );//!!!!! 後に続くAdd2Tree内で、curseri++ されて、curseriが更新される！！！
	curtype = SHDBALLJOINT;
	ret = Add2Tree( name, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = SetMaterials( 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->Init3DObj( curseri, &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* curselem;
	curselem = (*m_shandler)(curseri);
	CVec3f jointloc;
	float zelem;

	if( m_invz == 0 )
		zelem = 1.0f;
	else
		zelem = -1.0f;

	if( m_swapxz == 0 ){
		jointloc.x = (pointptr->x - m_centerx) * m_multiple;
		jointloc.y = pointptr->y * -m_multiple;
		jointloc.z = (pointptr->z - m_centerz) * zelem * m_multiple;
	}else{
		jointloc.x = (pointptr->z - m_centerz) * m_multiple;
		jointloc.y = pointptr->y * -m_multiple;
		jointloc.z = (pointptr->x - m_centerx) * zelem * m_multiple;
	}
	ret = curselem->SetJointLoc( &jointloc );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_mhandler->SetHasMotion( curseri, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_thandler->End();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CRokFile::AddPolyMesh( int colorno, int gno )
{
	int ret;
	int* dirtypoint;
	int* indexconv;// rokpointのpointnoから新しいvertexbufのpointnoへの変換表。
	int* dirtyface;
	
	dirtypoint = (int*)malloc( sizeof( int ) * rokpointnum );
	indexconv = (int*)malloc( sizeof( int ) * rokpointnum );
	dirtyface = (int*)malloc( sizeof( int ) * rokfacenum );
	if( !dirtypoint || !indexconv || !dirtyface ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( dirtypoint, sizeof( int ) * rokpointnum );
	ZeroMemory( indexconv, sizeof( int ) * rokpointnum );
	ZeroMemory( dirtyface, sizeof( int ) * rokfacenum );
	int faceno;
	int polygonnum = 0;
	ROKFACE* currf;
	for( faceno = 1; faceno < rokfacenum; faceno++ ){
		currf = facebuf + faceno;
		int chkgno;
		if( gno >= 0 ){
			chkgno = CheckGroupNo( currf, gno );
		}else{
			chkgno = 1;
		}

		if( chkgno ){
			if( currf->colorno == colorno ){
				*(dirtyface + faceno) = 1;//!!!
//DbgOut( "RokFile : AddPolyMesh : dirtyface %d\n", faceno );
				if( currf->pointnum == 3 ){
					polygonnum++;
				}else if( currf->pointnum == 4 ){
					polygonnum += 2;
				}else{
					DbgOut( "RokFile : AddPolyMesh : pointnum error !!!\n" );
					_ASSERT( 0 );
					free( dirtypoint );
					free( indexconv );
					free( dirtyface );
					return 1;
				}
					
				int linecnt;
				int lineno;
				for( linecnt = 0; linecnt < currf->pointnum; linecnt++ ){
					lineno = currf->line[linecnt];
					ROKLINE* currl = linebuf + lineno;
					*(dirtypoint + currl->p1) = 1;
					*(dirtypoint + currl->p2) = 1;
				}
			}
		}
	}
	if( polygonnum > 0 ){
		ret = m_thandler->Begin();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		curdepth = m_thandler->GetCurDepth();
		char name[1024];
		ZeroMemory( name, 1024 );
		sprintf_s( name, 1024, "色番号%dPolyMesh%d", colorno, m_thandler->curseri + 1 );//thandler->curseri + 1 == serialno
		curtype = SHDPOLYMESH;
		ret = Add2Tree( name, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		int indexno = 0;
		int pointno;
		for( pointno = 1; pointno < rokpointnum; pointno++ ){
			if( *(dirtypoint + pointno) ){
				*(indexconv + pointno) = indexno;
				indexno++;
			}
		}
		
		//int totalpoint = indexno + 1; // 間違いだ2002/7/5
		int totalpoint = indexno;//!!!!!!!!!!!!!!!!

		ret = begin_polymesh_vertex( colorno, totalpoint );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = polymesh_vertex( dirtypoint, indexconv );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = begin_polymesh_face( polygonnum, colorno );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = polymesh_face( dirtyface, indexconv );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}		

		if( m_chkinv ){
		//check inverse face
			int* chkindex;
			chkindex = (int*)malloc( sizeof( int ) * rokfacenum );
			if( !chkindex ){
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( chkindex, sizeof( int ) * rokfacenum );

			int errorflag = 0;
			int fno;
			for( fno = 1; fno < rokfacenum; fno++ ){
				if( *(dirtyface + fno) == 1 ){
					*(chkindex + fno) |= 0x0010;
					CheckInvFace( fno, chkindex, &errorflag );
					if( errorflag ){
						_ASSERT( 0 );
						return 1;
					}
				}
			}
			
			ret = InverseIndex( dirtyface, chkindex );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			free( chkindex );
		}

		ret = m_thandler->End();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	free( dirtypoint );
	free( indexconv );
	free( dirtyface );
	return 0;
}

int CRokFile::SetMaterials( int colorno )
{
	int ret;
	CVec3f tempvec;
	ROKPALLET currp = palletbuf[colorno];
	//diffuse
	ret = tempvec.SetMem( (float)(currp.diffuse[0]) / 255.0f,
		(float)(currp.diffuse[1]) / 255.0f,
		(float)(currp.diffuse[2]) / 255.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = tempinfo.SetMem( &tempvec, MAT_DIFFUSE );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//ambient
	ret = tempvec.SetMem( (float)(currp.ambient[0]) / 255.0f,
		(float)(currp.ambient[1]) / 255.0f,
		(float)(currp.ambient[2]) / 255.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = tempinfo.SetMem( &tempvec, MAT_AMBIENT );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//specular
	ret = tempvec.SetMem( 0.0f, 0.0f, 0.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = tempinfo.SetMem( &tempvec, MAT_SPECULAR );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CRokFile::begin_polymesh_vertex( int colorno, int vertnum )
{
	int ret;
	ret = SetMaterials( colorno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret += tempinfo.SetMem( vertnum, MESHI_M );
	_ASSERT( !ret );
	ret += tempinfo.SetMem( 0, MESHI_N );
	_ASSERT( !ret );
	ret += tempinfo.SetMem( 0, MESHI_TOTAL );
	_ASSERT( !ret );
	ret += tempinfo.SetMem( 1, MESHI_MCLOSED );
	_ASSERT( !ret );
	ret += tempinfo.SetMem( 0, MESHI_NCLOSED );
	_ASSERT( !ret );

	ret += tempinfo.SetMem( 1, MESHI_HASCOLOR );
	_ASSERT( !ret );

	ret += m_shandler->Init3DObj( curseri, &tempinfo );
	if( ret ){
		DbgOut( "RokFile : begin_polymesh_vertex : %d error !!!\n", ret );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CRokFile::polymesh_vertex( int* dirtypoint, int* indexconv )
{
	int ret;
	int pno;
	CShdElem* curselem;
	curselem = (*m_shandler)(curseri);
	_ASSERT( curselem );
	float zelem;

	if( m_invz == 0 )
		zelem = 1.0f;
	else
		zelem = -1.0f;

	for( pno = 1; pno < rokpointnum; pno++ ){
		if( *(dirtypoint + pno) ){
			int vno = *(indexconv + pno);
			ROKPOINT* currp = pointbuf + pno;

			//ret = curselem->SetPolyMeshPoint( vno, 
			//	currp->z * MULTIPLEELEM, 
			//	currp->y * -MULTIPLEELEM, 
			//	currp->x * MULTIPLEELEM );

			// ！！！この部分を変更する時は、Boneのjointlocも変更必要。
			if( m_swapxz == 0 ){
				ret = curselem->SetPolyMeshPoint( vno, 
					(currp->x - m_centerx) * m_multiple, 
					currp->y * -m_multiple, 
					(currp->z - m_centerz) * zelem * m_multiple,
					pno );
			}else{
				ret = curselem->SetPolyMeshPoint( vno, 
					(currp->z - m_centerz) * m_multiple, 
					currp->y * -m_multiple, 
					(currp->x - m_centerx) * zelem * m_multiple,
					pno );
			}

			if( ret ){
				DbgOut( "RokFile : polymesh_vertex : SetPolyMeshPoint error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CRokFile::begin_polymesh_face( int facenum, int colorno )
{
	CShdElem* curselem = (*m_shandler)( curseri );
	int ret;
	ret = curselem->CreatePolyMeshIndex( facenum );
	if( ret ){
		DbgOut( "RokFile : begin_polymesh_face : CreatePolyMeshIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


////////////// attrib0 のセット
	CPolyMesh* pm;
	pm = curselem->polymesh;
	_ASSERT( pm );

	int faceno;
	for( faceno = 0; faceno < facenum; faceno++ ){
		*( pm->m_attrib0 + faceno ) = colorno + m_materialoffset;
	}

//	DbgOut( "!!!check !!! polymesh : begin_polymesh_vertex : m_attrib0 : curseri %d, facenum %d, colorno %d, m_materialoffset %d\r\n",
//		curseri, facenum, colorno, m_materialoffset );



	return 0;
}

int CRokFile::polymesh_face( int* dirtyface, int* indexconv )
{
	CShdElem* curselem = (*m_shandler)( curseri );
	int ret;

	int faceno;
	ROKFACE* currf;
	int setfaceno = 0;
	for( faceno = 1; faceno < rokfacenum; faceno++ ){
		if( *(dirtyface + faceno) == 1 ){
			currf = facebuf + faceno;

			int vert[4];
			ZeroMemory( vert, sizeof( int ) * 4 );
			ret = SetVertNo( currf, vert );
			if( ret ){
				DbgOut( "RokFile : polymesh_face : SetVertNo error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int list[3];
			if( m_swapxz == m_invz ){
				list[0] = *(indexconv + vert[0]);
				list[1] = *(indexconv + vert[2]);
				list[2] = *(indexconv + vert[1]);
			}else{
				list[0] = *(indexconv + vert[0]);
				list[1] = *(indexconv + vert[1]);
				list[2] = *(indexconv + vert[2]);
			}
			ret = curselem->SetPolyMeshIndex( setfaceno, list );
			if( ret ){
				DbgOut( "RokFile : polymesh_face : SetPolyMeshIndex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			setfaceno++;//!!!!

			if( currf->pointnum == 4 ){
				if( m_swapxz == m_invz ){
					list[0] = *(indexconv + vert[0]);
					list[1] = *(indexconv + vert[3]);
					list[2] = *(indexconv + vert[2]);
				}else{
					list[0] = *(indexconv + vert[0]);
					list[1] = *(indexconv + vert[2]);
					list[2] = *(indexconv + vert[3]);
				}
				ret = curselem->SetPolyMeshIndex( setfaceno, list );
				if( ret ){
					DbgOut( "RokFile : polymesh_face : SetPolyMeshIndex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				setfaceno++;//!!!!
			}
		}
	}

	return 0;
}

int CRokFile::SetVertNo( ROKFACE* rfptr, int* dstvertno )
{
	int pointcnt;
	int pointcntmax = rfptr->pointnum;
	
	if( (pointcntmax != 3) && (pointcntmax != 4) ){
		DbgOut( "RokFile: SetVertNo : pointcntmax error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	for( pointcnt = 0; pointcnt < pointcntmax; pointcnt++ ){
		int curlineno;
		int nextlineno;
		curlineno = rfptr->line[pointcnt];
		if( pointcnt != (pointcntmax - 1) ){
			nextlineno = rfptr->line[pointcnt + 1];
		}else{
			nextlineno = rfptr->line[0];
		}

		ROKLINE* curline = linebuf + curlineno;
		ROKLINE* nextline = linebuf + nextlineno;
		int pattern;
		pattern = SearchSamePoint( curline, nextline );
		if( pattern <= 0 ){
			DbgOut( "RokFile : SetVertNo : SearchSamePoint error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		switch( pattern ){
		case 1:
			*(dstvertno + pointcnt) = curline->p2;
			break;
		case 2:
			*(dstvertno + pointcnt) = curline->p2;
			break;
		case 3:
			*(dstvertno + pointcnt) = curline->p1;
			break;
		case 4:
			*(dstvertno + pointcnt) = curline->p1;
			break;
		}
	}

	return 0;
}

int CRokFile::SearchSamePoint( ROKLINE* curline, ROKLINE* nextline )
{
	if( curline->p1 == nextline->p1 ){
		return 1;
	}else if( curline->p1 == nextline->p2 ){
		return 2;
	}else if( curline->p2 == nextline->p1 ){
		return 3;
	}else if( curline->p2 == nextline->p2 ){
		return 4;
	}else{
		return 0;
	}

}

int CRokFile::AddBone()
{
	int* dirtyline;
	int* bonepoint;

	dirtyline = (int*)malloc( sizeof( int ) * roklinenum );
	bonepoint = (int*)malloc( sizeof( int ) * rokpointnum );
	if( !dirtyline || !bonepoint ){
		DbgOut( "RokFile : AddBone : malloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( dirtyline, sizeof( int ) * roklinenum );
	ZeroMemory( bonepoint, sizeof( int ) * rokpointnum );

	int faceno;
	for( faceno = 1; faceno < rokfacenum; faceno++ ){
		ROKFACE* currf = facebuf + faceno;
		int linenum = currf->pointnum;
		int linecnt;
		for( linecnt = 0; linecnt < linenum; linecnt++ ){
			int lineno = currf->line[linecnt];
			*(dirtyline + lineno) = 1;
		}
	}

	int findno = 0;
	int lineno;
	for( lineno = 1; lineno < roklinenum; lineno++ ){
		if( *(dirtyline + lineno) == 0 ){
			ROKLINE* currl = linebuf + lineno;
			if( currl->dispflag == 0 ){
				int p1, p2;
				p1 = currl->p1;
				p2 = currl->p2;
				if( (pointbuf + p1)->dispflag == 0 ){
					*(bonepoint + currl->p1) = 1;
					findno++;
				}
				if( (pointbuf + p2)->dispflag == 0 ){
					*(bonepoint + currl->p2) = 1;
					findno++;
				}
			}
		}
	}

	int ret;
	if( findno ){		
		ret = m_thandler->Begin();
		_ASSERT( !ret );
		ret = MakePart( "ボーンパート" );
		if( ret ){
			_ASSERT( 0 );
			free( dirtyline );
			free( bonepoint );
			return 1;
		}

		
		int pointno;
		int boneno = 0;
		for( pointno = 1; pointno < rokpointnum; pointno++ ){
			if( *(bonepoint + pointno) == 1 ){
				
				ROKPOINT* currp = pointbuf + pointno;
				ret = MakeBoneElem( currp, boneno );
				if( ret ){
					DbgOut( "RokFile : AddBone : MakeBoneElem error !!!\n" );
					_ASSERT( 0 );
					free( dirtyline );
					free( bonepoint );
					return 1;
				}
				
				boneno++;
			}
		}
		
		DbgOut( "RokFile : AddBone : roklinenum %d findno %d boneno %d\n", roklinenum, findno, boneno );


		ret = m_thandler->End();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}

	free( dirtyline );
	free( bonepoint );

	return 0;
}

int CRokFile::CheckUpDataRange()
{
	int pno;
	ROKPOINT* currp;

	m_minvec.SetMem( 1e6, 1e6, 1e6 );
	m_maxvec.SetMem( -1e6, -1e6, -1e6 );

	for( pno = 1; pno < rokpointnum; pno++ ){
		currp = pointbuf + pno;
		if( currp->dispflag == 0 ){
			if( currp->x < m_minvec.x )
				m_minvec.x = currp->x;
			if( currp->y < m_minvec.y )
				m_minvec.y = currp->y;
			if( currp->z < m_minvec.z )
				m_minvec.z = currp->z;

			if( currp->x > m_maxvec.x )
				m_maxvec.x = currp->x;
			if( currp->y > m_maxvec.y )
				m_maxvec.y = currp->y;
			if( currp->z > m_maxvec.z )
				m_maxvec.z = currp->z;
		}
	}

	float tempy;
	tempy = m_minvec.y;
	m_minvec.y = -m_maxvec.y;
	m_maxvec.y = -tempy;

	return 0;
}
int CRokFile::SetLoadParams( float multiple, int swapxz, int centeringx, int centeringz, int invz, int divmode, int chkinv )
{
	m_multiple = multiple;
	m_swapxz = swapxz;
	m_invz = invz;
	m_divmode = divmode;
	m_chkinv = chkinv;

	float sumx = 0.0f;
	float sumz = 0.0f;
	int pno;
	int sumcnt = 0;
	if( centeringx || centeringz ){
		for( pno = 1; pno < rokpointnum; pno++ ){
			ROKPOINT* currp = pointbuf + pno;
			if( currp->dispflag == 0 ){
				sumx += currp->x;
				sumz += currp->z;
				sumcnt++;
			}
		}
	}

	if( centeringx ){
		if( sumcnt != 0 )
			m_centerx =	sumx / (float)sumcnt;
		else
			m_centerx = 0.0f;
	}else{
		m_centerx = 0.0f;
	}

	if( centeringz ){
		if( sumcnt != 0 )
			m_centerz = sumz / (float)sumcnt;
		else
			m_centerz = 0.0f;
	}else{
		m_centerz = 0.0f;
	}

	return 0;
}

int CRokFile::SetGroupMax()
{
	int gmax = 0;
	int lineno;
	for( lineno = 1; lineno < roklinenum; lineno++ ){
		ROKLINE* currl = linebuf + lineno;
		if( currl->dispflag == 0 ){
			int gno = currl->groupno;
			if( gno > gmax ){
				gmax = gno;
			}
		}
	}

	m_groupmax = gmax;

	return 0;
}

int CRokFile::CheckGroupNo( ROKFACE* rfptr, int gno )
{
	//rfptrの持つlineのうち、ひとつでもgnoに属していれば1を返す。

	int findflag = 0;
	int linecnt;
	int linecntmax = rfptr->pointnum;
	for( linecnt = 0; linecnt < linecntmax; linecnt++ ){
		int lineno = rfptr->line[linecnt];
		ROKLINE* currl = linebuf + lineno;
		if( currl->groupno == gno ){
			findflag = 1;
			break;
		}
	}

	return findflag;
}

int CRokFile::CalcNormal( ROKFACE* rf, CVec3f* dstn )
{
	int ret;
	int vert[4];
	ZeroMemory( vert, sizeof( int ) * 4 );
	ret = SetVertNo( rf, vert );
	if( ret ){
		DbgOut( "RokFile : CalcNormal : SetVertNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ROKPOINT* rp0 = pointbuf + vert[0];
	ROKPOINT* rp1 = pointbuf + vert[1];
	ROKPOINT* rp2 = pointbuf + vert[2];

	CVec3f vec1, vec2;
	vec1.x = rp1->x - rp0->x;
	vec1.y = rp1->y - rp0->y;
	vec1.z = rp1->z - rp0->z;

	vec2.x = rp2->x - rp0->x;
	vec2.y = rp2->y - rp0->y;
	vec2.z = rp2->z - rp0->z;

	CVec3f normal;
	normal.x = vec1.y * vec2.z - vec1.z * vec2.y;
    normal.y = vec1.z * vec2.x - vec1.x * vec2.z;
    normal.z = vec1.x * vec2.y - vec1.y * vec2.x;
	normal.Normalize();

	*dstn = normal;

	return 0;
}

void CRokFile::CheckInvFace( int fno, int* chkindex, int* errorflag )
{
	// chkindex : 
	//		自分のfnoでCheckInvFaceを呼び出し済なら	0x0100をadd
	//		Orderをチェック済なら					0x0010をadd
	//		裏面であることがわかったら				0x0001をadd
	

	//自分のchkindexが0x0100を持っていれば、リターンする。
	int callflag = *(chkindex + fno) & 0x0100;
	if( callflag )
		return;

	*(chkindex + fno) |= 0x0100;

//DbgOut( "Rokfile : CheckInvFace %d %d\n", fno, *errorflag );

	//共通するlineを持つfaceを見つける
	int ret;
	ROKFACE* currf = facebuf + fno;
	int findnum = 0;
	int findface[256];
	int commline[256];
	ZeroMemory( findface, sizeof( int ) * 256 );
	ZeroMemory( commline, sizeof( int ) * 256 );
	ret = FindCommonFace( currf, findface, commline, &findnum );
	if( ret ){
		_ASSERT( 0 );
		(*errorflag)++;
		return;
	}

	//見つけたfaceの共通辺のたどり方をチェックして裏面を探す。
	int findno;
	for( findno = 0; findno < findnum; findno++ ){
		int chkfaceno = findface[findno];
		int clineno = commline[findno];
		
		int chkflag = *(chkindex + chkfaceno) & 0x0010;
		if( chkflag == 0 ){
			ROKFACE* chkface = facebuf + chkfaceno;
			int chkorder;
			chkorder = CheckFaceDir( *(chkindex + fno), currf, chkface, clineno );
			if( chkorder == 0 ){
				*(chkindex + chkfaceno) |= 0x0001;
			}
			*(chkindex + chkfaceno) |= 0x0010;
		}
	}

	//見つけたfaceそれぞれについて、この関数を呼び出す。
	for( findno = 0; findno < findnum; findno++ ){
		int chkfaceno = findface[findno];
		CheckInvFace( chkfaceno, chkindex, errorflag );
	}

}

int CRokFile::CheckFaceDir( int chkflag, ROKFACE* rf, ROKFACE* chkrf, int clineno )
{
	int ret;
	int vert[4];
	ZeroMemory( vert, sizeof( int ) * 4 );
	ret = SetVertNo( rf, vert );
	if( ret ){
		DbgOut( "RokFile : CheckOrder : SetVertNo error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	int chkvert[4];
	ZeroMemory( chkvert, sizeof( int ) * 4 );
	ret = SetVertNo( chkrf, chkvert );
	if( ret ){
		DbgOut( "RokFile : CheckOrder : SetVertNo error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	int order1, order2;
	order1 = CheckOrder( vert, rf->pointnum, clineno );
	order2 = CheckOrder( chkvert, chkrf->pointnum, clineno );

	int invflag = chkflag & 0x0001;
	if( invflag == 0 ){
		if( order1 == order2 )
			return 0;
		else
			return 1;
	}else{
		if( order1 == order2 )
			return 1;
		else
			return 0;
	}
}

int CRokFile::CheckOrder( int* vlist, int vnum, int lineno )
{
	ROKLINE* currl = linebuf + lineno;
	int p1, p2;
	p1 = currl->p1;
	p2 = currl->p2;

	int vno;
	int findflag = 0;
	for( vno = 0; vno < vnum; vno++ ){
		int curno = *(vlist + vno);
		int nextno;
		if( vno != (vnum - 1) )
			nextno = *(vlist + vno + 1);
		else
			nextno = *vlist;

		if( (p1 == curno) && (p2 == nextno) ){
			findflag = 1;
			break;
		}
	}

	return findflag;
}


int CRokFile::FindCommonFace( ROKFACE* rf, int* findptr, int* commline, int* findnumptr )
{
	int linecnt, linecntmax;
	linecntmax = rf->pointnum;
	for( linecnt = 0; linecnt < linecntmax; linecnt++ ){
		int lineno = rf->line[linecnt];

		int faceno;
		for( faceno = 1; faceno < rokfacenum; faceno++ ){
			ROKFACE* chkrf = facebuf + faceno;
			int lcnt, lcntmax;
			lcntmax = chkrf->pointnum;
			for( lcnt = 0; lcnt < lcntmax; lcnt++ ){
				int chklineno = chkrf->line[lcnt];
				if( lineno == chklineno ){
					(*findnumptr)++;
					int setno = *findnumptr - 1;
					if( setno >= 256 ){
						DbgOut( "RokFile : FindCommonFace : findnum out of range error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					*(findptr + setno) = faceno;
					*(commline + setno) = lineno;
				}
			}
		}
	}

	//for dbg
	//if( linecntmax > *findnumptr ){
	//	DbgOut( "RokFile : FindCommonFace : faceno %d, linecntmax %d, findnum %d\n",
	//		rf->faceno, linecntmax, *findnumptr );
	//}


	return 0;
}


int CRokFile::InverseIndex( int* dirtyface, int* chkindex )
{
	CShdElem* curselem = (*m_shandler)( curseri );
	int ret;

	int faceno;
	ROKFACE* currf;
	int setfaceno = 0;
	for( faceno = 1; faceno < rokfacenum; faceno++ ){
		if( *(dirtyface + faceno) == 1 ){
			currf = facebuf + faceno;
			
			int invflag = *(chkindex + faceno) & 0x0001;

			if( invflag ){
				ret = curselem->InvPolyMeshIndex( setfaceno );
				if( ret ){
					DbgOut( "RokFile : InverseIndex : InvPolyMeshIndex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
			setfaceno++;//!!!!

			if( currf->pointnum == 4 ){
				if( invflag ){
					ret = curselem->InvPolyMeshIndex( setfaceno );
					if( ret ){
						DbgOut( "RokFile : InverseIndex : InvPolyMeshIndex error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				setfaceno++;//!!!!
			}
		}
	}

	return 0;
}

int CRokFile::RokPallet2MQOMaterial()
{
	CMQOMaterial* curmqomat;
	curmqomat = m_shandler->GetLastMaterial();
	if( !curmqomat ){
		DbgOut( "rokfile : RokPallet2MQOMaterial : curmqomat NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int colno;
	for( colno = 0; colno <= 12; colno++ ){
		CMQOMaterial* newmat;
		newmat = new CMQOMaterial();
		if( !newmat ){
			DbgOut( "rokfile : RokPallet2MQOMaterial : newmat alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		newmat->materialno = colno + m_materialoffset;
		(m_shandler->m_materialcnt)++;

		curmqomat->next = newmat;
		curmqomat = newmat;

		newmat->legacyflag = 1;

		char tmpname[256];
		sprintf_s( tmpname, 256, "rokpal_%d_%d", colno, m_materialoffset );
		strcpy_s( newmat->name, 256, tmpname );


		ROKPALLET* currp = palletbuf + colno;
		newmat->col.a = 1.0f;
		newmat->col.r = (float)(currp->diffuse[0]) / 255.0f;
		newmat->col.g = (float)(currp->diffuse[1]) / 255.0f;
		newmat->col.b = (float)(currp->diffuse[2]) / 255.0f;

		newmat->dif4f.a = 1.0f;
		newmat->dif4f.r = newmat->col.r;
		newmat->dif4f.g = newmat->col.g;
		newmat->dif4f.b = newmat->col.b;

		newmat->amb3f.r = (float)(currp->ambient[0]) / 255.0f;
		newmat->amb3f.g = (float)(currp->ambient[1]) / 255.0f;
		newmat->amb3f.b = (float)(currp->ambient[2]) / 255.0f;

		newmat->dif = 1.0f;
		if( newmat->col.r != 0.0f )
			newmat->amb = newmat->amb3f.r / newmat->col.r;
		else
			newmat->amb = newmat->amb3f.r;

		newmat->orgalpha = newmat->col.a;//!!!!!!!!
	}

	return 0;
}