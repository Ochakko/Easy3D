#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>

#include <crtdbg.h>

#include <MAFile.h>
#include <MCHandler.h>

#define DBGH
#include <dbg.h>


#include <forbidid.h>
#include <Panda.h>

#ifdef INEASY3D
#include "c:\pgfile9\e3dhsp3\handlerset.h"

#else
#include "c:\pgfile9\RokDeBone2DX\viewer.h" //!!!!!!!!!!!!!!!!!!!!!!!!
#include "c:\pgfile9\RokDeBone2DX\motdlg.h" //!!!!!!!!!!!!!!!!!!!!!!!!
#include "c:\pgfile9\RokDeBone2DX\MotChangeDlg.h" //!!!!!!!!!!!!!!!!!!

extern CMotDlg* g_motdlg;
#endif

static char s_maheader[256] = "Motion Accelerator File ver1.0 type01\r\n";
static char s_maheader2[256] = "Motion Accelerator File ver2.0 type01\r\n";
static char s_maheader3[256] = "Motion Accelerator File ver3.0 type01\r\n";
static char s_maheader4[256] = "Motion Accelerator File ver4.0 type01\r\n";
static char s_maheader5[256] = "Motion Accelerator File ver5.0 type01\r\n";
static char s_maheader6[256] = "Motion Accelerator File ver6.0 type01\r\n";


CMAFile::CMAFile()
{
	InitParams();
}
CMAFile::~CMAFile()
{
	DestroyObjs();
}


int CMAFile::InitParams()
{
	m_state = MA_FINDCHUNK;
	ZeroMemory( &mabuf, sizeof( MABUF ) );
	ZeroMemory( m_linechar, sizeof( char ) * MALINELENG );

	m_mh = 0;
	m_mch = 0;

	m_apphwnd = 0;
	m_papp = 0;

	m_loadtrunknum = 0;
	m_moaversion = 0;

	m_quamult = 1.0f;
	m_fuleng = 10;

	return 0;
}
int CMAFile::DestroyObjs()
{
	if( mabuf.hfile != INVALID_HANDLE_VALUE ){
		if( mabuf.buf == 0 ){
			//書き込み時だけ
			FlushFileBuffers( mabuf.hfile );
			SetEndOfFile( mabuf.hfile );
		}
		CloseHandle( mabuf.hfile );
		mabuf.hfile = INVALID_HANDLE_VALUE;
	}

	if( mabuf.buf ){
		free( mabuf.buf );
		mabuf.buf = 0;
	}

	mabuf.bufleng = 0;
	mabuf.pos = 0;
	mabuf.isend = 0;

	return 0;
}

int CMAFile::WriteMAHeader()
{
	int ret;
	strcpy_s( m_linechar, MALINELENG, s_maheader6 );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CMAFile::WriteFULeng()
{
	int ret;
	sprintf_s( m_linechar, MALINELENG, "#FULENG {\r\n\t%d\r\n}\r\n", m_mh->m_fuleng );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CMAFile::WriteEventKey()
{
	int ret;
	strcpy_s( m_linechar, MALINELENG, "#EVENTKEY {" );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	CEventKey* ekptr = &( m_mh->m_ek );

	int kindex;
	for( kindex = 0; kindex < ekptr->m_keynum; kindex++ ){
		sprintf_s( m_linechar, MALINELENG, "%d, %d, %d, %d", 
			ekptr->m_ekey[kindex].eventno, ekptr->m_ekey[kindex].key, ekptr->m_ekey[kindex].combono, ekptr->m_ekey[kindex].singleevent );
		ret = WriteLinechar( 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	strcpy_s( m_linechar, MALINELENG, "}\r\n" );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CMAFile::WriteLinechar( int addreturn )
{
	unsigned long wleng, writeleng;

	if( addreturn )
		strcat_s( m_linechar, MALINELENG, "\r\n" );

	wleng = (int)strlen( m_linechar );
	if( wleng >= MALINELENG ){
		DbgOut( "mafile : WriteLinechar : wleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	WriteFile( mabuf.hfile, m_linechar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMAFile::SaveMAFile( char* srcfilename, CMotHandler* srcmh, CMCHandler* srcmch, HWND srchwnd, int overwriteflag )

{

	int ret;

	m_mh = srcmh;
	m_mch = srcmch;

	ret = CheckSameMotionName();
	if( ret ){
		DbgOut( "mafile : SaveMAFile : CheckSameMotionName : same name found error !!!\n" );
		_ASSERT( 0 );
		return -1;//!!!!!!!!!!!
	}

	ret = CheckIdlingMotion( m_mch );
	if( ret ){
		DbgOut( "mafile : SaveMAFile : CheckIdlingMotion : idling setting error !!!\n" );
		_ASSERT( 0 );
		return -2;//!!!!!!!!!!!!
	}

	ret = CheckCommonid( srchwnd );
	if( ret ){
		DbgOut( "mafile : SaveMAFile : CheckCommonid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( overwriteflag == 0 ){
		DWORD fattr;
		fattr = GetFileAttributes( srcfilename );
		if( (fattr & FILE_ATTRIBUTE_DIRECTORY) == 0 ){//ファイルが見つかった場合
			char messtr[1024];
			sprintf_s( messtr, 1024, "%sは、既に存在します。\n上書きしますか？", srcfilename );

			int dlgret;
			dlgret = (int)MessageBox( srchwnd, messtr, "上書き確認", MB_OKCANCEL );
			if( dlgret != IDOK ){
				return 0;//!!!!!!!!!!!!!!!!!!
			}
		}
	}

	mabuf.hfile = CreateFile( (LPCTSTR)srcfilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( mabuf.hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "mafile : SaveMAFile : CreateFile error !!! %s\n", srcfilename );
		_ASSERT( 0 );
		return 1;
	}	
	
	ret = WriteMAHeader();
	if( ret ){
		DbgOut( "mafile : SaveMAFile : WriteMAHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteFULeng();
	if( ret ){
		DbgOut( "mafile : SaveMAFile : WriteFULeng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteEventKey();
	if( ret ){
		DbgOut( "mafile : SaveMAFile : WriteEventKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = WriteTrunk();
	if( ret ){
		DbgOut( "mafile : SaveMAFile : WriteTrunk error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteBranch();
	if( ret ){
		DbgOut( "mafile : SaveMAFile : WriteBranch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMAFile::CheckSameMotionName()
{
	if( m_mh->s2mot_leng <= 1 ){
		return 0;//!!!!!!!!!!!!!!!!!!
	}

	CMotionCtrl* mcptr = (*m_mh)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return 1;
	}
	if( !(miptr->motname) ){
		_ASSERT( 0 );
		return 1;
	}
	//szName = *(miptr->motname + cookie);
	
	int findsame = 0;

	int mcno;
	for( mcno = 0; mcno < m_mch->m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mch->m_mcarray + mcno;

		char* curname;
		curname = *( miptr->motname + curmce->id );

		int chkno;
		for( chkno = 0; chkno < m_mch->m_mcnum; chkno++ ){
			MCELEM* chkmce;
			chkmce = m_mch->m_mcarray + chkno;

			char* chkname;
			chkname = *( miptr->motname + chkmce->id );

			if( curmce != chkmce ){
				int cmp0;
				cmp0 = strcmp( curname, chkname );
				if( cmp0 == 0 ){
					findsame = 1;
					break;
				}
			}
		}
		if( findsame ){
			break;
		}
	}


	return findsame;

}

int CMAFile::CheckIdlingMotion( CMCHandler* mch )
{
	int idlingnum = 0;

	int mcno;
	for( mcno = 0; mcno < mch->m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = mch->m_mcarray + mcno;

		if( curmce->idling == 1 ){
			DbgOut( "mafile : CheckIdlingMotion : find idling %d %d\r\n", mcno, curmce->idling );
			idlingnum++;
		}
	}

	if( idlingnum == 1 ){
		return 0;
	}else{
		DbgOut( "mafile : CheckIdlingMotin : idlingnum error !!! %d\r\n", idlingnum );
		return 1;
	}
}


int CMAFile::CheckCommonid( HWND srchwnd )
{

	if( m_mh->s2mot_leng <= 1 ){
		return 0;//!!!!!!!!!!!!!!!!!!
	}

	CMotionCtrl* mcptr = (*m_mh)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return 1;
	}
	if( !(miptr->motname) ){
		_ASSERT( 0 );
		return 1;
	}
	//szName = *(miptr->motname + cookie);
	
	int mcno;
	for( mcno = 0; mcno < m_mch->m_mcnum; mcno++ ){
		MCELEM* parmce;
		parmce = m_mch->m_mcarray + mcno;

		int childno;
		for( childno = 0; childno < parmce->childnum; childno++ ){
			MCELEM* childmce;
			childmce = parmce->childmc + childno;

			int chkno;
			for( chkno = 0; chkno < m_mch->m_mcnum; chkno++ ){
				MCELEM* chkmce;
				chkmce = m_mch->m_mcarray + chkno;

				if( (HitTestForbidID( parmce, chkmce->commonid ) == 0) && (childmce->eventno1 == chkmce->commonid) ){
					char tmpline[2048];
					sprintf_s( tmpline, 2048, "分岐モーションの中に、共通分岐IDと同じIDが見つかりました。\n親項目 %s\n子項目 %s\nイベントID %d",
						*( miptr->motname + parmce->id ), *( miptr->motname + childmce->id ), chkmce->commonid );
					::MessageBox( srchwnd, tmpline, "警告", MB_OK );
				}
			}
		}
	}


	return 0;
}

int CMAFile::WriteTrunk()
{

//#TRUNK {
//"MotionName1", "filename1.qua", "Idling0", ev0idle, commonid, forbidcommonid, notfu 
//"MotionName2", "filename2.qua", "Normal0", ev0idle, commonid, forbidcommonid, notfu
//}
	int ret;

	if( m_mh->s2mot_leng <= 1 ){
		return 0;//!!!!!!!!!!!!!!!!!!
	}

	CMotionCtrl* mcptr = (*m_mh)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return 1;
	}
	if( !(miptr->motname) ){
		_ASSERT( 0 );
		return 1;
	}
	//szName = *(miptr->motname + cookie);

	strcpy_s( m_linechar, MALINELENG, "#TRUNK {" );
	ret = WriteLinechar( 1 );
	_ASSERT( !ret );


	int mcno;
	for( mcno = 0; mcno < m_mch->m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mch->m_mcarray + mcno;

		char* mname;
		mname = *( miptr->motname + curmce->id );

		sprintf_s( m_linechar, MALINELENG, "\t\"%s\", \"%s.qua\", ", mname, mname );
		
		if( curmce->idling == 1 ){
			strcat_s( m_linechar, MALINELENG, "\"Idling0\", " );
		}else{
			strcat_s( m_linechar, MALINELENG, "\"Normal0\", " );
		}

		char tmpline[1024];
		sprintf_s( tmpline, 1024, "%d, %d, %d", curmce->ev0idle, curmce->commonid, curmce->forbidnum );
		strcat_s( m_linechar, MALINELENG, tmpline );

		int fno;
		for( fno = 0; fno < curmce->forbidnum; fno++ ){
			sprintf_s( tmpline, 1024, ", %d", *( curmce->forbidid + fno ) );
			strcat_s( m_linechar, MALINELENG, tmpline );
		}

		sprintf_s( tmpline, 1024, ", %d", curmce->notfu );
		strcat_s( m_linechar, MALINELENG, tmpline );


		ret = WriteLinechar( 1 );
		_ASSERT( !ret );
	}

	strcpy_s( m_linechar, MALINELENG, "}\r\n" );
	ret = WriteLinechar( 1 );
	_ASSERT( !ret );


	return 0;
}

int CMAFile::WriteBranch()
{

//#BRANCH "MotionName1(parentname)"{
//    "MotionName2(childname)", eventno, frameno1, frameno2, notfu, nottoidle
//    "MotionName3(childname)", eventno, frameno1, frameno2, notfu, nottoidle
//    "MotionName4(childname)", eventno, frameno1, frameno2, notfu, nottoidle
//}

	int ret;

	if( m_mh->s2mot_leng <= 1 ){
		return 0;//!!!!!!!!!!!!!!!!!!
	}

	CMotionCtrl* mcptr = (*m_mh)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return 1;
	}
	if( !(miptr->motname) ){
		_ASSERT( 0 );
		return 1;
	}
	//szName = *(miptr->motname + cookie);


	int mcno;
	for( mcno = 0; mcno < m_mch->m_mcnum; mcno++ ){
		MCELEM* parmce;
		parmce = m_mch->m_mcarray + mcno;

		char* parname;
		parname = *( miptr->motname + parmce->id );

		if( parmce->childnum >= 1 ){

			sprintf_s( m_linechar, MALINELENG, "#BRANCH \"%s\" {", parname );
			ret = WriteLinechar( 1 );
			_ASSERT( !ret );


			int chilno;
			for( chilno = 0; chilno < parmce->childnum; chilno++ ){
				MCELEM* chilmce;
				chilmce = parmce->childmc + chilno;

				char* chilname;
				chilname = *( miptr->motname + chilmce->id );

				sprintf_s( m_linechar, "\t\"%s\", %d, %d, %d, %d, %d", chilname, chilmce->eventno1, chilmce->frameno1, chilmce->frameno2, chilmce->notfu, chilmce->nottoidle );
				ret = WriteLinechar( 1 );
				_ASSERT( !ret );

			}

			strcpy_s( m_linechar, MALINELENG, "}\r\n" );
			ret = WriteLinechar( 1 );
			_ASSERT( !ret );

		}
	}

	return 0;
}

int CMAFile::SetBuffer( CPanda* panda, int pndid, char* srcfilename )
{
	DestroyObjs();

	PNDPROP prop;
	ZeroMemory( &prop, sizeof( PNDPROP ) );
	int ret;
	ret = panda->GetProperty( pndid, srcfilename, &prop );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int bufleng = prop.sourcesize;

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );//bufleng + 1
	if( !newbuf ){
		DbgOut( "mafile : SetBuffer :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	int getsize = 0;
	ret = panda->Decrypt( pndid, srcfilename, (unsigned char*)newbuf, bufleng, &getsize );
	if( ret || (getsize != bufleng) ){
		_ASSERT( 0 );
		return 1;
	}
	
	mabuf.buf = newbuf;
	*(mabuf.buf + bufleng) = 0;//!!!!!!!!
	mabuf.bufleng = bufleng;
	mabuf.pos = 0;	
	

//DbgOut( "check !!! mafile : SetBuffer : filename %s, bufleng %d\r\n", srcfilename, bufleng );

	/////////////

	char* lasten;
	lasten = strrchr( srcfilename, '\\' );
	if( lasten ){
		int cpleng;
		cpleng = (int)( lasten - srcfilename );
		if( cpleng >= MAX_PATH ){
			DbgOut( "mafile : SetBuffer : mediadir path too long error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		strncpy_s( m_mediadir, MAX_PATH, srcfilename, cpleng );
		m_mediadir[cpleng] = 0;

	}else{
		m_mediadir[0] = 0;
	}


	return 0;
}


int CMAFile::SetBuffer( char* srcfilename )
{
	DestroyObjs();

	mabuf.hfile = CreateFile( (LPCTSTR)srcfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( mabuf.hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	

	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( mabuf.hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "mafile : SetBuffer :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "mafile : SetBuffer :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );//bufleng + 1
	if( !newbuf ){
		DbgOut( "mafile : SetBuffer :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( mabuf.hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "mafile : SetBuffer :  ReadFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	mabuf.buf = newbuf;
	*(mabuf.buf + bufleng) = 0;//!!!!!!!!
	mabuf.bufleng = bufleng;
	mabuf.pos = 0;	
	

//DbgOut( "check !!! mafile : SetBuffer : filename %s, bufleng %d\r\n", srcfilename, bufleng );

	/////////////

	char* lasten;
	lasten = strrchr( srcfilename, '\\' );
	if( lasten ){
		int cpleng;
		cpleng = (int)( lasten - srcfilename );
		if( cpleng >= MAX_PATH ){
			DbgOut( "mafile : SetBuffer : mediadir path too long error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		strncpy_s( m_mediadir, MAX_PATH, srcfilename, cpleng );
		m_mediadir[cpleng] = 0;

	}else{
		m_mediadir[0] = 0;
	}


	return 0;
}

int CMAFile::GetLine( int* getlen )
{
	//int ret;

	int notfound = 1;
	int stepno = 0;
	while( (mabuf.pos + stepno < mabuf.bufleng) && (notfound = ( *(mabuf.buf + mabuf.pos + stepno) != '\n' )) ){
		stepno++;

		if( (mabuf.pos + stepno) >= mabuf.bufleng ){
			//mabuf.isend = 1;//!!!!!!!!!!
			//_ASSERT( 0 );
			break;
		}
	}

	if( notfound == 0 ){
		stepno++; //\nの分
	}

	if( MALINELENG > stepno ){
		if( (stepno > 0) && ((mabuf.pos + stepno) <= mabuf.bufleng) )
			strncpy_s( m_linechar, MALINELENG, (const char*)(mabuf.buf + mabuf.pos), stepno );
		*getlen = stepno;
		mabuf.pos += stepno;

		m_linechar[stepno] = 0;

	}else{
		_ASSERT( 0 );
		DbgOut( "MAFile : GetLine : dstlen short error !!!" );
		return 1;
	}

//	if( mabuf.pos + stepno >= mabuf.bufleng ){
	if( mabuf.pos >= mabuf.bufleng ){

//DbgOut( "check !!! mafile : GetLine : pos + stepno >= bufleng, %d + %d >= %d, %s\r\n", mabuf.pos, stepno, mabuf.bufleng, m_linechar );
		mabuf.isend = 1;
		m_state = MA_FINISH;
		return 0;
	}else{

//DbgOut( "check !!! mafile : GetLine : return notfound\r\n" );
//			return notfound;


		if( ((*getlen == 2) && (strcmp( m_linechar, "\r\n" ) == 0)) || ((*getlen == 1) && (*m_linechar == '\n')) ){
//DbgOut( "check !!! mafile : GetLine : only return : call GetLine\r\n" );
			int retgl;			
			retgl = GetLine( getlen );	
			return retgl;
		}else{

//DbgOut( "check !!! mafile : GetLine : return notfound\r\n" );

			return notfound;
		}

	}

/***
	//if( (notfound == 1) && (mabuf.isend == 1) ){
	if( mabuf.isend == 1 ){
		m_state = MA_FINISH;
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
***/

}


int CMAFile::CheckFileVersion()
{
	int ret;
	int leng1;

	ret = GetLine( &leng1 );
	DbgOut( "MAFile : CheckFileVersion : filekind : %s\n", m_linechar );
	if( ret ){
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 0, 0 );
		::MessageBox( m_apphwnd, "moaファイルではありません。\n読み込めません。", "読み込みエラー", MB_OK );
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 1, 0 );

		return 0;
	}

	int cmp1, cmp2, cmp3, cmp4, cmp5, cmp6;
	cmp1 = strcmp( s_maheader, m_linechar );
	cmp2 = strcmp( s_maheader2, m_linechar );
	cmp3 = strcmp( s_maheader3, m_linechar );
	cmp4 = strcmp( s_maheader4, m_linechar );
	cmp5 = strcmp( s_maheader5, m_linechar );
	cmp6 = strcmp( s_maheader6, m_linechar );
	if( cmp1 && cmp2 && cmp3 && cmp4 && cmp5 && cmp6 ){
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 0, 0 );
		::MessageBox( m_apphwnd, "moaファイルではありません。\n読み込めません。", "読み込みエラー", MB_OK );
		if( m_apphwnd )
			::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 1, 0 );
		return 0;
	}

	if( cmp1 == 0 ){
		return 10;
	}else if( cmp2 == 0 ){
		return 20;
	}else if( cmp3 == 0 ){
		return 30;
	}else if( cmp4 == 0 ){
		return 40;
	}else if( cmp5 == 0 ){
		return 50;
	}else if( cmp6 == 0 ){
		return 60;
	}else{
		_ASSERT( 0 );
		return 0;
	}

}

int CMAFile::FindChunk()
{
	int isfind = 0;
	int ret = 0;
	int getleng;
	while( (isfind == 0) && (mabuf.isend == 0) ){
		ret = GetLine( &getleng );

//DbgOut( "check !!! mafile : FindChunk %d, %s\r\n", ret, m_linechar );

		if( ret ){
			//_ASSERT( 0 );
			m_state = MA_FINISH;
			if( mabuf.isend == 1 ){
				ret = 0;//!!!!
			}
			break;
		}
		if( (getleng >= 3) && (strstr( m_linechar, "{\r\n" ) != NULL) ){
			isfind = 1;
			ret = GetChunkType( m_linechar, getleng );

//DbgOut( "check !!! mafile : FindChunk : aft GetChunkType %d, m_state %d\r\n", ret, m_state );

			if( ret ){
				_ASSERT( 0 );
				m_state = MA_FINISH;
				break;
			}
		}else if( (getleng >= 5) && (strstr( m_linechar, "Eof\r\n" ) != NULL) ){

//DbgOut( "check !!! mafile : FindChunk :find Eof\r\n" );

			m_state = MA_FINISH;
			break;
		}

		if( mabuf.isend == 1 ){

//DbgOut( "check !!! mafile : FindChunk : mabuf.isend == 1\r\n" );

			m_state = MA_FINISH;
			break;
		}

	}

	return ret;
}

int CMAFile::GetChunkType( char* chunkname, int nameleng )
{
	char* namehead = chunkname;

	while( (*namehead == ' ') || (*namehead == '\t') ){
		namehead++;
	}

	char chunkpat[4][20] =
	{
		"#TRUNK",
		"#BRANCH",
		"#FULENG",
		"#EVENTKEY"
	};

	int isfind = 0;
	int patno;
	for( patno = 0; patno < 4; patno++ ){
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
					m_state = MA_TRUNK;
					break;
				case 1:
					m_state = MA_BRANCH;
					break;
				case 2:
					m_state = MA_FULENG;
					break;
				case 3:
					m_state = MA_EVENTKEY;
					break;
				default:
					break;
				}
			}
		}

	}

	if( isfind == 0 ){
		_ASSERT( 0 );
		m_state = MA_UNKCHUNK;
	}

	return 0;
}

int CMAFile::SkipChunk()
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

#ifdef INEASY3D

int CMAFile::LoadMAFileFromPnd( CPanda* panda, int pndid, char* srcfilename, HWND srchwnd, CHandlerSet* srchs, float mvmult )
{
	int ret;

	m_loadtrunknum = 0;
	m_apphwnd = srchwnd;
	m_papp = 0;
	m_hs = srchs;
	m_mh = m_hs->m_mhandler;
	m_mch = m_hs->m_mch;
	m_quamult = mvmult;

	ret = SetBuffer( panda, pndid, srcfilename );
	if( ret ){
		DbgOut( "mafile : LoadMAFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	///////

	m_moaversion = CheckFileVersion();
	if( m_moaversion == 0 ){
		DbgOut( "mafile : Loadmafile : file version error!!!\n" );
		return 1;
	}

	m_state = MA_FINDCHUNK;


	//char chunkname[256];

	while( (m_state != MA_FINISH) && (mabuf.isend == 0) ){

		switch( m_state ){
		case MA_FINDCHUNK:
			ret = FindChunk();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;
		case MA_UNKCHUNK:
			ret = SkipChunk();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			m_state = MA_FINDCHUNK;
			break;
		case MA_FULENG:
			ret = ReadFULeng();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;
		case MA_EVENTKEY:
			ret = ReadEventKey();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;

		case MA_TRUNK:
			ret = ReadTrunk( panda, pndid );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;
		case MA_BRANCH:
			ret = ReadBranch();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;

		case MA_FINISH:
		case MA_NONE:
		default:
			m_state = MA_FINISH;
			break;
		}
	}


	return 0;

}
#endif


#ifdef INEASY3D
	int CMAFile::LoadMAFile( char* srcfilename, HWND srchwnd, CHandlerSet* srchs, float mvmult )
	{
		int ret;

		m_loadtrunknum = 0;

		m_apphwnd = srchwnd;
		m_papp = 0;
		m_hs = srchs;
		m_mh = m_hs->m_mhandler;
		m_mch = m_hs->m_mch;

		m_quamult = mvmult;

		ret = LoadMAFile_aft( srcfilename );
		if( ret ){
			DbgOut( "mafile : LoadMAFile : LoadMAFile_aft error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		return 0;
	}

#else
	int CMAFile::LoadMAFile( char* srcfilename, CMyD3DApplication* srcpapp )
	{
		int ret;

		m_loadtrunknum = 0;

		m_apphwnd = srcpapp->m_hWnd;
		m_papp = srcpapp;
		m_mh = m_papp->m_mhandler;
		m_mch = m_papp->m_mcdlg->m_mch;
		m_hs = 0;

		ret = LoadMAFile_aft( srcfilename );
		if( ret ){
			DbgOut( "mafile : LoadMAFile : LoadMAFile_aft error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		return 0;
	}

#endif

int CMAFile::LoadMAFile_aft( char* srcfilename )
{
	int ret;

	ret = SetBuffer( srcfilename );
	if( ret ){
		DbgOut( "mafile : LoadMAFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	///////

	m_moaversion = CheckFileVersion();
	if( m_moaversion == 0 ){
		DbgOut( "mafile : Loadmafile : file version error!!!\n" );
		return 1;
	}

	m_state = MA_FINDCHUNK;


	//char chunkname[256];

	while( (m_state != MA_FINISH) && (mabuf.isend == 0) ){

		switch( m_state ){
		case MA_FINDCHUNK:
			ret = FindChunk();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;
		case MA_UNKCHUNK:
			ret = SkipChunk();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			m_state = MA_FINDCHUNK;
			break;
		case MA_FULENG:
			ret = ReadFULeng();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;
		case MA_EVENTKEY:
			ret = ReadEventKey();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;

		case MA_TRUNK:
			ret = ReadTrunk();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;
		case MA_BRANCH:
			ret = ReadBranch();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;

		case MA_FINISH:
		case MA_NONE:
		default:
			m_state = MA_FINISH;
			break;
		}
	}


	return 0;
}
int CMAFile::ReadFULeng()
{
	int ret;
	int findend = 0;
	int getleng;
	int pos, stepnum;
	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;
		
		pos = 0;
		stepnum = 0;

		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
		}else{
			m_fuleng = 10;
			ret = GetInt( &m_fuleng, m_linechar, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut( "mafile : ReadFULeng : GetInt fuleng error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
#ifndef INEASY3D
			if( m_fuleng > 0 ){
				m_mh->m_fuleng = m_fuleng;
				m_mch->m_fillupleng = m_fuleng;
			}else{
				m_mh->m_fuleng = 10;
				m_mch->m_fillupleng = 10;
			}
#endif
		}
	}
	m_state = MA_FINDCHUNK;

	return 0;
}

int CMAFile::ReadEventKey()
{
	int ret;
	int findend = 0;
	int getleng;
	int pos, stepnum;

	ret = m_mh->m_ek.DelEKeyByIndex( -1 );
	if( ret ){
		DbgOut( "mafile : ReadEventKey : ek DelEKeyByIndex all error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;
		
		pos = 0;
		stepnum = 0;

		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
		}else{
			EKEY ek;
			ZeroMemory( &ek, sizeof( EKEY ) );

			ret = GetInt( &ek.eventno, m_linechar, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut( "mafile : ReadEventKey : GetInt eventno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

			ret = GetInt( &ek.key, m_linechar, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut( "mafile : ReadEventKey : GetInt key error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

			ret = GetInt( &ek.combono, m_linechar, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut( "mafile : ReadEventKey : GetInt combono error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

			if( m_moaversion >= 60 ){
				int tmpsingle = 1;
				ret = GetInt( &tmpsingle, m_linechar, pos, MALINELENG, &stepnum );
				if( ret ){
					DbgOut( "mafile : ReadEventKey : GetInt singleevent error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				pos += stepnum;

				if( tmpsingle == 0 ){
					ek.singleevent = 0;
				}else{	
					ek.singleevent = 1;
				}
			}else{
				ek.singleevent = 1;
			}

			ret = m_mh->m_ek.AddEKey( ek );
			if( ret ){
				DbgOut( "mafile : ReadEventKey : ek AddEKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	m_state = MA_FINDCHUNK;

	return 0;
}


int CMAFile::ReadTrunk( CPanda* panda, int pndid )
{
	int ret;

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
			MATRUNK matrunk;
			ZeroMemory( &matrunk, sizeof( MATRUNK ) );

			ret = SetMATrunk( m_linechar, &matrunk );
			if( ret ){
				DbgOut( "mafile : ReadTrunk : SetMATrunk error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			char fullname[MAX_PATH];
			int chkleng;
			chkleng = (int)strlen( m_mediadir ) + (int)strlen( matrunk.filename ) + 1;
			if( chkleng >= MAX_PATH ){
				DbgOut( "mafile : ReadTrunk : fullname path too long error %s!!!\n", matrunk.filename );
				_ASSERT( 0 );
				return 1;
			}
			sprintf_s( fullname, MAX_PATH, "%s\\%s", m_mediadir, matrunk.filename );


	DbgOut( "check !!! mafile : ReadTrunk : fullname %s\r\n", fullname );

#ifdef INEASY3D
			int curcookie = -1;
			ret = m_hs->LoadMAFMotion( fullname, matrunk.idling, matrunk.ev0idle, matrunk.commonid, matrunk.forbidnum, matrunk.forbidid, &curcookie, m_quamult, matrunk.notfu, panda, pndid );
			if( ret ){
				DbgOut( "mafile : ReadTrunk : hs LoadMAFMotion error %s !!!\n", fullname );
				_ASSERT( 0 );
				return 1;
			}

	DbgOut( "check !!! mafile : ReadTrunk : curcookie %d\r\n", curcookie );

			ret = m_hs->m_mhandler->SetMotionName( curcookie, matrunk.motname );
			if( ret ){
				DbgOut( "mafile : ReadTrunk : mh SetMotionName error %s !!!\n", matrunk.motname );
				_ASSERT( 0 );
				return 1;
			}
#else

//DbgOut( "check !!! mafile : ReadTrunk : name %s, idling %d, notfu %d\r\n",
//	matrunk.motname, matrunk.idling, matrunk.notfu );

			int callmoa = 1;
			D3DXVECTOR3 quamult = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
			ret = (int)m_papp->LoadMotion( fullname, quamult, matrunk.idling, matrunk.ev0idle, matrunk.commonid, matrunk.forbidnum, matrunk.forbidid, matrunk.notfu, callmoa );
			if( ret ){
				DbgOut( "mafile : ReadTrunk : LoadMotion error %s !!!\n", fullname );
				_ASSERT( 0 );
				return 1;
			}

			int curcookie;
			curcookie = g_motdlg->GetMotCookie();


	DbgOut( "check !!! mafile : ReadTrunk : curcookie %d\r\n", curcookie );


			ret = m_papp->m_mhandler->SetMotionName( curcookie, matrunk.motname );
			if( ret ){
				DbgOut( "mafile : ReadTrunk : mh SetMotionName error %s !!!\n", matrunk.motname );
				_ASSERT( 0 );
				return 1;
			}

	//DbgOut( "check : mafile : ReadTrunk : %s, %s, %d\r\n", matrunk.motname, matrunk.filename, matrunk.idling );			


#endif

		


	//DbgOut( "check : mafile : ReadTrunk : %s, %s, %d\r\n", matrunk.motname, matrunk.filename, matrunk.idling );			

			m_loadtrunknum++;//!!!!!!!!!!!!!

			if( matrunk.forbidid ){
				free( matrunk.forbidid );
				matrunk.forbidid = 0;
			}
			matrunk.forbidnum = 0;

		}
	}
	m_state = MA_FINDCHUNK;

	return 0;
}

int CMAFile::ReadBranch()
{
	int ret;

	int pos = 0;
	int stepnum;

	char parentname[MAX_PATH];
	char* startpoint;

	stepnum = 0;
	startpoint = strchr( m_linechar + pos, '\"' );
	if( !startpoint ){
		_ASSERT( 0 );
		return 1;
	}
	pos = (int)(startpoint - m_linechar);
	ret = GetName( parentname, MAX_PATH, m_linechar, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut( "mafile : ReadBranch : GetName parentname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		


//DbgOut( "check !!! mafile : ReadBranch : parentname %s\r\n", parentname );


	int parentcookie;

#ifdef INEASY3D
	parentcookie = m_hs->m_mhandler->GetMotionIDByName( parentname );
	if( parentcookie < 0 ){
		DbgOut( "mafile : ReadBranch : mh GetMotionIDByName %s error !!!\n", parentname );
		_ASSERT( 0 );
		return 1;
	}
#else
	parentcookie = m_papp->m_mhandler->GetMotionIDByName( parentname );
	if( parentcookie < 0 ){
		DbgOut( "mafile : ReadBranch : mh GetMotionIDByName %s error !!!\n", parentname );
		_ASSERT( 0 );
		return 1;
	}
#endif

	//DbgOut( "check !!! : mafile : ReadBranch : parentname %s\r\n", parentname );



	int findend = 0;
	int getleng;
	int vertno = 0;
	while( findend == 0 ){
		ret = GetLine( &getleng );

//DbgOut( "check !!! mafile : ReadBranch : aft GetLine %d, %s\r\n", ret, m_linechar );

		if( ret )
			return ret;
			
		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
		}else{
			MABRANCH mabranch;
			ret = SetMABranch( m_linechar, &mabranch );

//DbgOut( "check !!! mafile : ReadBranch : aft SetBranch %d, %s\r\n", ret, m_linechar );

			if( ret ){
				DbgOut( "mafile : ReadBranch : SetMABranch error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int childcookie;

#ifdef INEASY3D
			childcookie = m_hs->m_mhandler->GetMotionIDByName( mabranch.motname );
			if( childcookie < 0 ){
				DbgOut( "mafile : ReadBranch : mh GetMotionIDByName child %s error !!!\n", mabranch.motname );
				_ASSERT( 0 );
				return 1;
			}
#else
			childcookie = m_papp->m_mhandler->GetMotionIDByName( mabranch.motname );
			if( childcookie < 0 ){
				DbgOut( "mafile : ReadBranch : mh GetMotionIDByName child %s error !!!\n", mabranch.motname );
				_ASSERT( 0 );
				return 1;
			}
#endif				
			MCELEM childmc;
			ZeroMemory( &childmc, sizeof( MCELEM ) );
			childmc.id = childcookie;
			childmc.eventno1 = mabranch.eventno;
			childmc.frameno1 = mabranch.frameno1;
			childmc.frameno2 = mabranch.frameno2;
			childmc.notfu = mabranch.notfu;
			childmc.nottoidle = mabranch.nottoidle;

#ifdef INEASY3D
			ret = m_hs->AddChildMC( parentcookie, childmc );
			if( ret ){
				DbgOut( "mafile : ReadBranch : hs AddChildMC error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
#else
			ret = m_papp->m_mcdlg->AddChildMC( parentcookie, childmc );
			if( ret ){
				DbgOut( "mafile : ReadBranch : mcdlg AddChildMC error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
#endif

	//DbgOut( "check !!! : mafile : ReadBranch : %s, %d, %d, %d\r\n", mabranch.motname, mabranch.eventno, mabranch.frameno1, mabranch.frameno2 );

		}
	}

	m_state = MA_FINDCHUNK;

	return 0;
}





int CMAFile::SetMATrunk( char* srcline, MATRUNK* srctrunk )
{
	int ret;
	int pos = 0;
	int stepnum;

	stepnum = 0;
	char* startpoint;
	startpoint = strchr( srcline, '\"' );
	if( !startpoint ){
		_ASSERT( 0 );
		return 1;
	}
	pos = (int)(startpoint - srcline);
	ret = GetName( srctrunk->motname, MAX_PATH, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut( "mafile : SetMATrunk : GetName motname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;



	stepnum = 0;
	startpoint = strchr( srcline + pos, ',' );
	if( !startpoint ){
		_ASSERT( 0 );
		return 1;
	}
	pos = (int)(startpoint - srcline + 1);
	ret = GetName( srctrunk->filename, MAX_PATH, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut( "mafile : SetMATrunk : GetName filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;


	char tempchar[MAX_PATH];
	stepnum = 0;
	startpoint = strchr( srcline + pos, ',' );
	if( !startpoint ){
		_ASSERT( 0 );
		return 1;
	}
	pos = (int)(startpoint - srcline + 1);
	ret = GetName( tempchar, MAX_PATH, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut( "mafile : SetMATrunk : GetName tempchar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

	int cmp0;
	cmp0 = strcmp( tempchar, "Idling0" );
	if( cmp0 == 0 ){
		srctrunk->idling = 1;
	}else{
		srctrunk->idling = 0;
	}

	if( m_moaversion >= 20 ){

		stepnum = 0;
		ret = GetInt( &srctrunk->ev0idle, srcline, pos, MALINELENG, &stepnum );
		if( ret ){
			DbgOut( "mafile : SetMATrunk : GetInt ev0idle error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pos += stepnum;
		

		stepnum = 0;
		ret = GetInt( &srctrunk->commonid, srcline, pos, MALINELENG, &stepnum );
		if( ret ){
			DbgOut( "mafile : SetMATrunk : GetInt commonid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pos += stepnum;

		if( m_moaversion < 30 ){
			int forbidtotal = 0;
			stepnum = 0;
			ret = GetInt( &forbidtotal, srcline, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut( "mafile : SetMATrunk : GetInt forbidcommonid error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

			srctrunk->forbidnum = 0;
			_ASSERT( !srctrunk->forbidid );
			srctrunk->forbidid = 0;

			int bno;
			int curval = 1;
			int setno = 0;
			for( bno = 0; bno < 30; bno++ ){
				int andval;
				andval = forbidtotal & curval;
				if( andval ){
					srctrunk->forbidid = (int*)realloc( srctrunk->forbidid, sizeof( int ) * ( setno + 1 ) );
					if( !srctrunk->forbidid ){
						DbgOut( "mafile : SetMATrunk : forbidid alloc error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					*( srctrunk->forbidid + setno ) = andval;
					setno++;
				}
				curval = curval << 1;
			}

			srctrunk->forbidnum = setno;
		}else{

			stepnum = 0;
			ret = GetInt( &srctrunk->forbidnum, srcline, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut( "mafile : SetMATrunk : GetInt forbidnum error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

			if( srctrunk->forbidid ){
				free( srctrunk->forbidid );
				srctrunk->forbidid = 0;
			}
			
			if( srctrunk->forbidnum > 0 ){
				srctrunk->forbidid = (int*)malloc( sizeof( int ) * srctrunk->forbidnum );
				if( !srctrunk->forbidid ){
					DbgOut( "mafile : SetMATrunk : forbidid alloc error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ZeroMemory( srctrunk->forbidid, sizeof( int ) * srctrunk->forbidnum );

				int fno;
				for( fno = 0; fno < srctrunk->forbidnum; fno++ ){

					stepnum = 0;
					ret = GetInt( srctrunk->forbidid + fno, srcline, pos, MALINELENG, &stepnum );
					if( ret ){
						DbgOut( "mafile : SetMATrunk : GetInt forbidid error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					pos += stepnum;
				}
			}
		}

		if( m_moaversion >= 40 ){
			stepnum = 0;
			ret = GetInt( &srctrunk->notfu, srcline, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut( "mafile : SetMATrunk : GetInt notfu error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

//			_ASSERT( 0 );

		}else{
			srctrunk->notfu = 0;
		}

	}else{

		srctrunk->ev0idle = 0;
		srctrunk->commonid = 0;
		srctrunk->forbidnum = 0;
		srctrunk->forbidid = 0;
		srctrunk->notfu = 0;
	}

	return 0;
}

int CMAFile::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, endpos - startpos );
		tempchar[endpos - startpos] = 0;

		*dstint = atoi( tempchar );

		*stepnum = endpos - pos;
	}else{
		_ASSERT( 0 );
		*stepnum = endpos - pos;
		return 1;
	}


	return 0;
}
int CMAFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') && (*(srcchar + startpos) != '.') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) || (*(srcchar + endpos) == '.') )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, endpos - startpos );
		tempchar[endpos - startpos] = 0;

		*dstfloat = (float)atof( tempchar );

		*stepnum = endpos - pos;
	}else{
		_ASSERT( 0 );
		*stepnum = endpos - pos;
		return 1;
	}


	return 0;
}

int CMAFile::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) && 
		( ( *(srcchar + startpos) == ' ' ) || ( *(srcchar + startpos) == '\t' ) || ( *(srcchar + startpos) == '\"' ) ) 
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( ( *(srcchar + endpos) != ' ' ) && ( *(srcchar + endpos) != '\t' ) && (*(srcchar + endpos) != '\r') && (*(srcchar + endpos) != '\n') && (*(srcchar + endpos) != '\"') )
	){
		endpos++;
	}

	if( (endpos - startpos < dstleng) && (endpos - startpos > 0) ){
		strncpy_s( dstchar, dstleng, srcchar + startpos, endpos - startpos );
		*(dstchar + endpos - startpos) = 0;

		*stepnum = endpos - pos;

	}else{
		_ASSERT( 0 );
		*stepnum = endpos - pos;
		return 1;
	}

	return 0;
}


int CMAFile::SetMABranch( char* srcline, MABRANCH* srcbranch )
{

	int ret;
	int pos = 0;
	int stepnum;

	char* startpoint;

	stepnum = 0;
	startpoint = strchr( srcline + pos, '\"' );
	if( !startpoint ){
		_ASSERT( 0 );
		return 1;
	}
	pos = (int)(startpoint - srcline);
	ret = GetName( srcbranch->motname, MAX_PATH, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut( "mafile : SetMABranch : GetName motname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;


	stepnum = 0;
	ret = GetInt( &srcbranch->eventno, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut( "mafile : SetMABranch : GetInt eventno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;


	stepnum = 0;
	ret = GetInt( &srcbranch->frameno1, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut( "mafile : SetMABranch : GetInt frameno1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;


	stepnum = 0;
	ret = GetInt( &srcbranch->frameno2, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut( "mafile : SetMABranch : GetInt frameno2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;


	if( m_moaversion >= 20 ){
		stepnum = 0;
		ret = GetInt( &srcbranch->notfu, srcline, pos, MALINELENG, &stepnum );
		if( ret ){
			DbgOut( "mafile : SetMABranch : GetInt notfu error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pos += stepnum;
	}else{
		srcbranch->notfu = 0;
	}

	if( m_moaversion >= 60 ){
		stepnum = 0;
		ret = GetInt( &srcbranch->nottoidle, srcline, pos, MALINELENG, &stepnum );
		if( ret ){
			DbgOut( "mafile : SetMABranch : GetInt nottoidle error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pos += stepnum;

	}else{
		srcbranch->nottoidle = 0;
	}

	return 0;
}
