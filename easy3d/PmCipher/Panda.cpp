#include "stdafx.h"
#include "Panda.h"

#include <windows.h>
#include <stdio.h>
#include <imagehlp.h>
#include <string>
#include "PmCipher.h"

#include <crtdbg.h>

using namespace std;

static int s_alloccnt = 0;

CPanda::CPanda()
{
	m_cipher.clear();
	return;
}

CPanda::~CPanda()
{

	map<int,_PmCipher*>::iterator itrpm;
	for( itrpm = m_cipher.begin(); itrpm != m_cipher.end(); itrpm++ ){
		_PmCipher* curpm = itrpm->second;
		if( curpm ){
			delete curpm;
		}
	}
	m_cipher.clear();

	return;
}

_PmCipher* CPanda::GetCipher( int pndid )
{
	if( pndid < 0 ){
		_ASSERT( 0 );
		return 0;
	}
	return m_cipher[ pndid ];
}


int CPanda::Init( unsigned char* keyptr, unsigned int keyleng, int* pndidptr )
{
	*pndidptr = -1;

	_PmCipher* newpm;
	newpm = new _PmCipher();
	if( !newpm ){
		_ASSERT( 0 );
		return 1;
	}

	CIPHER_RESULT cr;
	cr = newpm->Init( keyptr, keyleng );
	if( cr != CIPHER_OK ){
		_ASSERT( 0 );
		delete newpm;
		return 1;
	}

	s_alloccnt++;
	m_cipher[ s_alloccnt ] = newpm;

	*pndidptr = s_alloccnt;

	return 0;
}

int CPanda::Destroy( int pndid )
{
	map<int,_PmCipher*>::iterator itrpm;
	itrpm = m_cipher.find( pndid );

	if( itrpm != m_cipher.end() ){
		delete itrpm->second;
		m_cipher.erase( itrpm );
	}

	return 0;
}


int CPanda::Encrypt( int pndid, const char* orgdirectory, const char* pndpath )
{
	_PmCipher* curpm = GetCipher( pndid );
	if( !curpm ){
		_ASSERT( 0 );
		return 1;
	}

	CIPHER_RESULT cr;
	cr = curpm->Encrypt( orgdirectory, pndpath, true, true );
	_ASSERT( cr == CIPHER_OK );
	return 0;
}

int CPanda::ParseCipherFile( int pndid, const char* pndpath )
{
	_PmCipher* curpm = GetCipher( pndid );
	if( !curpm ){
		_ASSERT( 0 );
		return 1;
	}

	CIPHER_RESULT cr;
	cr = curpm->ParseCipherFile( pndpath );

	return 0;
}


int CPanda::GetRootPath( int pndid, char* dstpath, int dstleng )
{
	_PmCipher* curpm = GetCipher( pndid );
	if( !curpm ){
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( dstpath, dstleng, curpm->GetRootPath().c_str() );

	return 0;
}

int CPanda::GetPropertyNum( int pndid )
{
	_PmCipher* curpm = GetCipher( pndid );
	if( !curpm ){
		_ASSERT( 0 );
		return 1;
	}

	PmCipherPropertyList* list = curpm->GetPropertyList();
	if( !list ){
		_ASSERT( 0 );
		return 0;
	}else{
		return list->GetPropertySize();
	}
}

int CPanda::GetProperty( int pndid, char* sourcepath, PNDPROP* dstprop )
{

	_PmCipher* curpm = GetCipher( pndid );
	if( !curpm ){
		_ASSERT( 0 );
		return 1;
	}

	ZeroMemory( dstprop, sizeof( PNDPROP ) );

	int findflag = 0;
	PmCipherPropertyList* list = curpm->GetPropertyList();
	if( list ){
		int propnum = list->GetPropertySize();
		int propno;
		for( propno = 0; propno < propnum; propno++ ){
			int cmp = strcmp( list->GetProperty( propno )->GetPath().c_str(), sourcepath );
			if( cmp == 0 ){
				strcpy_s( dstprop->path, MAX_PATH, list->GetProperty( propno )->GetPath().c_str() );
				strcpy_s( dstprop->directory, MAX_PATH, list->GetProperty( propno )->GetDirectory().c_str() );
				strcpy_s( dstprop->filename, MAX_PATH, list->GetProperty( propno )->GetFileName().c_str() );
				dstprop->sourcesize = list->GetProperty( propno )->GetSourceSize();
				findflag = 1;
				break;
			}
		}
	}


	if( findflag == 1 ){
		return 0;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CPanda::GetProperty( int pndid, int propno, PNDPROP* dstprop )
{
	_PmCipher* curpm = GetCipher( pndid );
	if( !curpm ){
		_ASSERT( 0 );
		return 1;
	}

	PmCipherPropertyList* list = curpm->GetPropertyList();
	if( list ){
		strcpy_s( dstprop->path, MAX_PATH, list->GetProperty( propno )->GetPath().c_str() );
		strcpy_s( dstprop->directory, MAX_PATH, list->GetProperty( propno )->GetDirectory().c_str() );
		strcpy_s( dstprop->filename, MAX_PATH, list->GetProperty( propno )->GetFileName().c_str() );

		dstprop->sourcesize = list->GetProperty( propno )->GetSourceSize();
	}else{
		ZeroMemory( dstprop, sizeof( PNDPROP ) );
		return 1;
	}

	return 0;
}

int CPanda::Decrypt( int pndid, const char* pndpath, unsigned char* dstbuf, int dstsize, int* getsize )
{
	*getsize = 0;

	_PmCipher* curpm = GetCipher( pndid );
	if( !curpm ){
		_ASSERT( 0 );
		return 1;
	}

	if( !dstbuf ){
		_ASSERT( 0 );
		return 1;
	}

	curpm->DecryptInit( pndpath );

	CIPHER_RESULT cr;
	unsigned int totalsize = 0;
	unsigned int size;
	char *buf;
	while(( (cr = curpm->Decrypt(&buf, &size)) != CIPHER_DEC_FINISH ) && (size > 0) )
	{
		if( totalsize >= (unsigned int)dstsize ){
			_ASSERT( 0 );
			return 1;
		}
		memcpy( dstbuf + totalsize, buf, size );
		totalsize += size;
	}
	//最後のバッファを出力
	if(size > 0)
	{
		if( totalsize >= (unsigned int)dstsize ){
			_ASSERT( 0 );
			return 1;
		}
		memcpy( dstbuf + totalsize, buf, size );
		totalsize += size;
	}

	//後始末（暗号化ファイルのクローズなど
	curpm->DecryptEnd();

	*getsize = totalsize;

	return 0;
}


