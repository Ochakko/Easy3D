#include "stdafx.h"
#include <windows.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <crtdbg.h>
#include <coef.h>

#define DBGH
#include <dbg.h>

#include "IncludeManager.h"

CIncludeManager::CIncludeManager()
{

}
CIncludeManager::~CIncludeManager()
{

}


HRESULT CIncludeManager::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{


	char strFileFull[4098];

	char moduledir[2048];
	int leng;
	ZeroMemory( moduledir, 2048 );
	leng = GetEnvironmentVariable( (LPCTSTR)"MODULEDIR", (LPTSTR)moduledir, 2048 );

	if( ((int)strlen(pName) + (int)strlen(moduledir)) >= 4098 ){
		DbgOut( "IncludeManager : Open : filename too long error !!!\n" );
		_ASSERT( 0 );
		return E_FAIL;
	}

	sprintf( strFileFull, "%s%s", moduledir, pName );

	/***
	if( !f.Open( strFileFull, CFile::modeRead ) ) 
        return E_FAIL;

    INT_PTR size = (INT_PTR)f.GetLength();

    BYTE* pData = new BYTE[size];
    if( pData == NULL )
        return E_OUTOFMEMORY;

    f.Read( pData, size );
	***/
	HANDLE hfile;
	hfile = CreateFile( strFileFull, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "IncludeManager : Open : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return E_FAIL;
	}	
	
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( hfile, &sizehigh );

	char* pData = (char*)malloc( sizeof( char ) * bufleng );
	if( !pData ){
		DbgOut( "IncludeManager : Open : pData alloc error !!!\n" );
		_ASSERT( 0 );
		return E_FAIL;
	}

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( hfile, (void*)pData, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "IncludeManager : Open :  ReadFile error !!!\n" );
		_ASSERT( 0 );
		return E_FAIL;
	}
	

    *ppData = pData;
    *pBytes = bufleng;

    return S_OK;
}


HRESULT CIncludeManager::Close(LPCVOID pData)
{
	/***
    BYTE* pData2 = (BYTE*)pData;
    SAFE_DELETE_ARRAY( pData2 );
	***/

	char* deldata = (char*)pData;
	if( pData ){
		free( deldata );
		pData = 0;
	}

    return S_OK;
}

