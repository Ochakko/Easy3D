
#include "stdafx.h"
#ifndef	FILEMAPH
	#include "filemap.h"
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <conio.h>
#include <winbase.h>




static HANDLE	s_hHeap = 0;

CFileMap::CFileMap()
{
	m_firstFileInfo = 0;

	if( !s_hHeap ){
		s_hHeap = HeapCreate( 0, 655360, 2031616 );
		if( !s_hHeap ){
			fprintf( stdout, "CFileMap : HeapCreate error!!!!!\n" );
		}
	}
}

CFileMap::~CFileMap()
{
	CUSFILEINFO*	pinfo = 0;
	CUSFILEINFO*	pnextinfo = 0;
	DWORD	dwcookie;
	BOOL	bret;

	pinfo = m_firstFileInfo;
	while( pinfo ){
		pnextinfo = pinfo->nextinfo;
		dwcookie = pinfo->dwcookie;
		bret = DelFileInfo( dwcookie );
		if( !bret ){
			fprintf( stdout, "CFileMap : Destruct : DelFileInfo error!!!\n" );
			break;
		}
		pinfo = pnextinfo;
	}
	
	
	if( s_hHeap ){
		HeapDestroy( s_hHeap );
		s_hHeap = 0;
	}
}

CUSFILEINFO*	CFileMap::AddNewFileInfo( LPCTSTR fname, 
	int acttype, DWORD sizehigh, DWORD sizelow )
{
	// write mode で　mapping するときは　filesize が　必須　！！！！
	
	static DWORD	newcounter = 0;
	
	HRESULT	hres;
	BOOL	bret;

	DWORD	hashval;

	CUSFILEINFO*	findinfo = 0;
	CUSFILEINFO*	pcurinfo = 0;
	CUSFILEINFO*	newinfo = 0;
	
	int nameleng, namesize;
	HANDLE	hfile = 0;
	HANDLE	hfilemapping = 0;
	PBYTE	pbyte = 0;

	DWORD	accessmode = 0;
	DWORD	fileshare = 0;
	DWORD	openmode = 0;
	DWORD	pagemode = 0;
	DWORD	filemapmode = 0;

	DWORD	mapsizehigh = 0;
	DWORD	mapsizelow = 0;

	DWORD	filesizehigh = 0;
	DWORD	filesizelow = 0;

// file name の　hash
	hashval = HashFName( fname );

// hash をもとに　file 検索、　cookie の決定
	findinfo = FindChain( hashval, fname );
	if( findinfo ){

		// すでに　fileinfo が存在する場合
		if( (acttype == ACT_READ) && (findinfo->acttype == ACT_READ) ){
			// 既存の　fileinfo を　使用
			return findinfo;

		}else{
			// アクセス違反 
			fprintf( stdout, "CFileMap : AddNewFileInfo : FindChain : Access mode error!!!\n" );		
			return 0;
		}
	}else{
		// 新規　
		newcounter++; // cookieの元
	}

// filemapping の　作成
	switch( acttype ){
	case ACT_READ:
		accessmode = GENERIC_READ;
		fileshare = FILE_SHARE_READ;
		openmode = OPEN_EXISTING;
		pagemode = PAGE_WRITECOPY;
		filemapmode = FILE_MAP_READ;
		mapsizehigh = 0;
		mapsizelow = 0;

		break;
	case ACT_WRITE:
		accessmode = GENERIC_READ | GENERIC_WRITE;
		fileshare = 0; // 書き込み中は　他のアクセスは　許さない
		openmode = OPEN_ALWAYS;
		pagemode = PAGE_READWRITE;
		filemapmode = FILE_MAP_WRITE;
		mapsizehigh = sizehigh;
		mapsizelow = sizelow;

		break;
	default:
		break;
	}

	hfile = CreateFile( fname, accessmode, fileshare, NULL, openmode,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		hres = GetLastError();
		fprintf( stdout, "component: AddNewFileInfo : CreateFile error!!\n" );
		goto newfileinfoerror;
	}
	
	hfilemapping = CreateFileMapping( hfile, NULL, pagemode, 
		mapsizehigh, mapsizelow, NULL );
	if( hfilemapping == INVALID_HANDLE_VALUE ){
		hres = GetLastError();
		fprintf( stdout, "component:AddNewFileInfo : CreateFileMapping error!!!!\n" );
		goto newfileinfoerror;
	}

	pbyte = (PBYTE)MapViewOfFile( hfilemapping, filemapmode, 0, 0, 0 );
	if( !pbyte ){
		hres = GetLastError();
		fprintf( stdout, "component:AddNewFileInfo : MapViewOfFile error!!!!\n" );
		goto newfileinfoerror;
	}


// fileinfo の　作成と　データセット
	newinfo = (CUSFILEINFO*)HeapAlloc( s_hHeap, 0, sizeof( CUSFILEINFO ) );
	if( !newinfo ){
		fprintf( stdout, "component : AddNewFileInfo : newinfo alloc error!!!\n" );
		goto newfileinfoerror;
	}

	newinfo->hashval = hashval;
	newinfo->dwcookie = newcounter;

	newinfo->hfile = hfile;
	newinfo->hfilemapping = hfilemapping;
	newinfo->acttype = acttype;
	newinfo->pbyte = pbyte;

	nameleng = _tcslen( fname ); // 文字数
	namesize = nameleng * sizeof( TCHAR ); // バイト数

	newinfo->filename = (LPTSTR)HeapAlloc( s_hHeap, 0, namesize + 2 );
	if( !(newinfo->filename) ){
		fprintf( stdout, "component : AddNewFileInfo : filename alloc error!!!\n" );
		goto newfileinfoerror;
	}
	ZeroMemory( newinfo->filename, namesize + 2 );
	_tcscpy( newinfo->filename, fname );


	if( acttype == ACT_READ ){
		filesizelow = GetFileSize( hfile, &filesizehigh );
		if( filesizelow == 0xFFFFFFFF ){
			hres = GetLastError();
			fprintf( stdout, "component: AddNewFileInfo : GetFileSize error!!!\n" );
			goto newfileinfoerror;
		}
		newinfo->sizehigh = filesizehigh;
		newinfo->sizelow = filesizelow;
	}else if( acttype == ACT_WRITE ){
		newinfo->sizehigh = mapsizehigh;
		newinfo->sizelow = mapsizelow;
	}

// hash 値の　小さい順に　チェインに　つなげる
	bret = AddChain( newinfo );
	if( !bret ){
		fprintf( stdout, "CFileMap : AddNewFileInfo : AddChain error!!!!\n" );
		goto newfileinfoerror;
	}

	return newinfo;


newfileinfoerror:
	// エラー時の　後処理
	if( newinfo && (newinfo->filename) ){
		HeapFree( s_hHeap, 0, newinfo->filename );
		newinfo->filename = 0;
	}
	if( newinfo ){
		HeapFree( s_hHeap, 0, newinfo );
		newinfo = 0;
	}
	if( (hfile != 0) || (hfile != INVALID_HANDLE_VALUE) ){
		CloseHandle( hfile );
		hfile = 0;
	}
	if( (hfilemapping != 0) || (hfilemapping != INVALID_HANDLE_VALUE) ){
		CloseHandle( hfilemapping );
		hfilemapping = 0;
	}
	if( pbyte ){
		UnmapViewOfFile( pbyte );
		pbyte = 0;
	}

	return 0;

}

CUSFILEINFO*	CFileMap::GetFileInfo( DWORD	dwcookie )
{
	CUSFILEINFO*	pcurinfo = 0;
	CUSFILEINFO*	findinfo = 0;

	pcurinfo = m_firstFileInfo;

	while( pcurinfo ){
		if( pcurinfo->dwcookie == dwcookie ){
			findinfo = pcurinfo;
			break;
		}
		pcurinfo = pcurinfo->nextinfo;
	}

	return findinfo;
}

BOOL	CFileMap::DelFileInfo( DWORD dwcookie )
{
	CUSFILEINFO*	curinfo = 0;
	CUSFILEINFO* befinfo = 0;
	CUSFILEINFO* findinfo = 0;

	befinfo = 0;
	curinfo = m_firstFileInfo;
	while( curinfo ){
		if( curinfo->dwcookie == dwcookie ){
			findinfo = curinfo;
			break;
		}
		befinfo = curinfo;
		curinfo = curinfo->nextinfo;
	}

	if( !findinfo ){
		// クッキーなし
		return FALSE;

	}else if( findinfo == m_firstFileInfo ){
		// 先頭を　削除
		m_firstFileInfo = findinfo->nextinfo;
		CloseHandle( findinfo->hfile );
		CloseHandle( findinfo->hfilemapping );
		UnmapViewOfFile( findinfo->pbyte );
		HeapFree( s_hHeap, 0, findinfo->filename );
		HeapFree( s_hHeap, 0, findinfo );
		return TRUE;
	}else{
		// 2　番目以降を削除
		befinfo->nextinfo = findinfo->nextinfo;
		CloseHandle( findinfo->hfile );
		CloseHandle( findinfo->hfilemapping );
		UnmapViewOfFile( findinfo->pbyte );
		HeapFree( s_hHeap, 0, findinfo->filename );
		HeapFree( s_hHeap, 0, findinfo );
		return TRUE;
	}

}


DWORD	CFileMap::HashFName( LPCTSTR fname )
{
	// 値が　ダブっても　可 　てきとー

	int	nameleng, cnt;
	LPCTSTR	curptr = 0;
	DWORD	val;
	DWORD	valmax =  4294967295;

	curptr = fname;
	nameleng = _tcslen( fname );
	for( cnt = 0; cnt < nameleng; cnt++ ){
		val += (*curptr * 3);
		curptr++;
	}

	
	val = val % valmax;
	fprintf( stdout, "CFileMap : hashval %d\n", val );

	return val;
}

CUSFILEINFO*	CFileMap::FindChain( DWORD hashval, LPCTSTR fname )
{
		// hashval, fname の順で　チェック
	DWORD			cmpval;
	CUSFILEINFO*	curinfo;
	CUSFILEINFO*	findinfo = 0;
	int cmpleng;


	cmpleng = _tcslen( fname ) * sizeof( TCHAR );
	curinfo = m_firstFileInfo;

	while( curinfo && ( (cmpval = curinfo->hashval) < hashval) ){
		if( (cmpval == hashval) && !_tcsncmp( (LPCTSTR)curinfo->filename, fname, cmpleng ) ){
			findinfo = curinfo;
			break;
		}
		curinfo = curinfo->nextinfo;
	}

	return findinfo;
}

BOOL	CFileMap::AddChain( CUSFILEINFO*	pinfo )
{
		// pinfo->hashval で　ソート
	CUSFILEINFO*	curinfo = 0;
	CUSFILEINFO*	befinfo = 0;
	DWORD	cmpval;
	//char*	cmpname;
	LPTSTR	cmpname;

	cmpval = pinfo->hashval;
	cmpname = pinfo->filename;
	curinfo = m_firstFileInfo;
	befinfo = 0;

	if( !curinfo ){
		// 他にデータがない
		// 先頭につける
		m_firstFileInfo = pinfo;
		pinfo->nextinfo = 0;
	}else{
		while( curinfo && (curinfo->hashval > cmpval) ){
			befinfo = curinfo->nextinfo;
			curinfo = curinfo->nextinfo;
		}
		if( !befinfo ){
			// hashval が　一番小さい
			// 先頭につける
			pinfo->nextinfo = curinfo;
			m_firstFileInfo = pinfo;
		}else{
			befinfo->nextinfo = pinfo;
			pinfo->nextinfo = curinfo;
		}
	}
	
	return TRUE;
}

BOOL	CFileMap::FlushAndSetEnd( DWORD	dwcookie, DWORD curpos )
{
	CUSFILEINFO*	pinfo = 0;
	HANDLE	hfile = 0;
	BYTE*	pbyte = 0;
	BOOL	bret1, bret2;
	DWORD	dwret;

	pinfo = GetFileInfo( dwcookie );
	if( !pinfo ){
		fprintf( stdout, "CFileMap : GetFileInfo error!!!\n" );
		return FALSE;
	}

	hfile = pinfo->hfile;
	pbyte = pinfo->pbyte;

	if( curpos > 0 ){
		bret1 = FlushViewOfFile( (void*)pbyte, curpos );
		if( !bret1 ){
			fprintf( stdout, "CFileMap : GetFileInfo error!!!\n" );
		}

		dwret = SetFilePointer( hfile, curpos, NULL, FILE_BEGIN );
		if( dwret == 0xFFFFFFFF ){
			fprintf( stdout,"CFileMap : SetFilePointer error!!!!\n" );
		}
	}
	

	bret2 = SetEndOfFile( hfile );
	if( !bret2 ){
		fprintf( stdout, "CFileMap : SetEndOfFile error!!!!!!\n" );
	}

	if( bret1 && bret2 && (dwret != 0xFFFFFFFF) )
		return TRUE;
	else
		return FALSE;
}	




