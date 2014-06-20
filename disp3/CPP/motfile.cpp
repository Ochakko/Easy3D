#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#define	DBGH
#include <dbg.h>
#include <crtdbg.h>

#include <motfile.h>
#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>
#include <matrix2.h>



CMotFile::CMotFile()
{
	ZeroMemory( (void*)&header, sizeof( MOTFILEHDR ) );
	matptr = 0;
	hfile = INVALID_HANDLE_VALUE;
	m_writemode = 0;

	m_frombuf = 0;
	m_buf.buf = 0;
	m_buf.bufsize = 0;
	m_buf.pos = 0;
}

CMotFile::~CMotFile()
{
	DestroyObjs();
}

void CMotFile::DestroyObjs()
{
	if( matptr ){
		delete [] matptr;
		matptr = 0;
	}

	if( hfile != INVALID_HANDLE_VALUE ){
		if( m_writemode ){
			FlushFileBuffers( hfile );
			SetEndOfFile( hfile );
		}
		CloseHandle( hfile );
		hfile = INVALID_HANDLE_VALUE;
	}

	if( m_buf.buf ){
		free( m_buf.buf );
		m_buf.buf = 0;
	}
	m_buf.bufsize = 0;
	m_buf.pos = 0;

}

int CMotFile::SaveMotFile( CMotHandler* lpmh, int mcookie, char* fname )
{
	char* mname;
	int framenum, mottype, leng;
	CMotionCtrl* mcptr = (*lpmh)( 1 );
	_ASSERT( mcptr );
	CMotionInfo* miptr = mcptr->motinfo;
	_ASSERT( miptr );

	_ASSERT( miptr->motname );
	mname = *(miptr->motname + mcookie);
	_ASSERT( mname );
	leng = (int)strlen( mname );
	if( leng > 255 ){
		DbgOut( "CMotFile : SaveMotFile : name leng error %d !!!\n", leng );
		_ASSERT( 0 );
		return 1;
	}
	_ASSERT( miptr->motnum );
	framenum = *(miptr->motnum + mcookie);
	_ASSERT( miptr->mottype );
	mottype = *(miptr->mottype + mcookie);
	
	int ret;
	ret = CreateMotFile( fname, mname, mcookie, framenum, mottype, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	// bone
	int serino;
	for( serino = 0; serino < lpmh->s2mot_leng; serino++ ){
		mcptr = (*lpmh)( serino );
		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
			if( mcptr->boneno >= 0 ){
				MOTFILEMARKER mmarker;
				ZeroMemory( &mmarker, sizeof( MOTFILEMARKER ) );
				mmarker.magicno = MOTMARKER_BONE;
				mmarker.serialno = mcptr->boneno;
				mmarker.valnum = framenum;
				ret = WriteMarker( &mmarker );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
				miptr = mcptr->motinfo;
				int frameno;
				for( frameno = 0; frameno < framenum; frameno++ ){
					CMatrix2* curmat;
					curmat = (*miptr)( mcookie, frameno );
					ret = WriteData( curmat );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}
				}

			}
		}
	}

	// morph
	for( serino = 0; serino < lpmh->s2mot_leng; serino++ ){
		mcptr = (*lpmh)( serino );
		if( mcptr->type == SHDMORPH ){
			if( mcptr->morphno >= 0 ){
				MOTFILEMARKER mmarker;
				ZeroMemory( &mmarker, sizeof( MOTFILEMARKER ) );
				mmarker.magicno = MOTMARKER_MORPH;
				mmarker.serialno = mcptr->morphno;
				mmarker.valnum = framenum;
				ret = WriteMarker( &mmarker );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
				miptr = mcptr->motinfo;
					
				float* curval;
				curval = miptr->GetMorphVal( mcookie );
				if( !curval ){
					_ASSERT( 0 );
					return 1;
				}

				ret = WriteData( curval, framenum );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	//end
	MOTFILEMARKER endmarker;
	ZeroMemory( &endmarker, sizeof( MOTFILEMARKER ) );
	endmarker.magicno = MOTMARKER_END;
	endmarker.serialno = -1;
	endmarker.valnum = -1;
	ret = WriteMarker( &endmarker );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMotFile::CreateMotFile( char* fname, char* mname, int mcookie, int framenum, int mottype, int elemnum )
{
	int leng;
	DWORD wleng, writeleng;

	hfile = CreateFile( (LPCTSTR)fname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CMotFile : CreateMotFile : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	
	m_writemode = 1;

	header.magicno = MOTFILEMAGICNO;
	leng = (int)strlen( mname );
	if( leng > 255 ){
		DbgOut( "CMotFile : CreateMotFile : name leng error %d !!!\n", leng );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( header.motname, 256 );
	strncpy_s( header.motname, 256, mname, leng );
	header.framenum = framenum;
	header.mottype = mottype;
	header.elemnum = elemnum;
	wleng = sizeof( MOTFILEHDR );
	WriteFile( hfile, (void*)&header, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "CMotFile : CreateMotFile : writeheader error %d %d\n", wleng, writeleng );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMotFile::WriteData( CMatrix2* wmat )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( CMatrix2 );
	WriteFile( hfile, (void*)wmat, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "CMotFile : WriteData : writefile error %d %d\n", wleng, writeleng );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CMotFile::LoadMotData( CMotHandler* lpmh, char* fname, char* motname, int* framenum, int* mottype )
{

	int ret;
	ret = OpenMotFile( fname );
	if( ret ){
		DbgOut( "MotFile : LoadMotData : mfile->OpenMotFile error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	
	m_frombuf = 0;//!!!

	ret = LoadMotData_aft( lpmh, motname, framenum, mottype );
	if( ret ){
		DbgOut( "motfile : LoadMotData : LoadMotData_aft error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}



	return 0;
}

int CMotFile::LoadMotDataFromBuf( CMotHandler* lpmh, char* bufptr, int bufsize, char* motname, int* framenum, int* mottype )
{
	int ret;

	m_buf.buf = (char*)malloc( sizeof( char ) * bufsize );
	if( !(m_buf.buf) ){
		DbgOut( "quafile : LoadMotDataFromBuf : buf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	MoveMemory( m_buf.buf, bufptr, sizeof( char ) * bufsize );

	m_buf.bufsize = bufsize;
	m_buf.pos = 0;
	

	
	m_frombuf = 1;//!!!

	ret = LoadMotData_aft( lpmh, motname, framenum, mottype );
	if( ret ){
		DbgOut( "motfile : LoadMotDataFromBuf : LoadMotData_aft error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}


	return 0;
}

int CMotFile::LoadMotData_aft( CMotHandler* lpmh, char* motname, int* framenum, int* mottype )
{
	MOTFILEHDR header;
	int ret;
	int retcookie = -1;

	MOTFILEMARKER mmarker;
	int findend = 0;

	ret = ReadHeader( &header );
	if( ret ){
		DbgOut( "MotFile : LoadMotData : mfile->ReadHeader error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	if( header.magicno != MOTFILEMAGICNO ){
		DbgOut( "MotFile : LoadMotData : magicno error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	*framenum = header.framenum;
	*mottype = header.mottype;
	strcpy_s( motname, MAX_PATH, header.motname );

	retcookie = lpmh->AddMotion( header.motname, header.mottype, header.framenum, INTERPOLATION_SQUAD, 0 );
	if( retcookie < 0 ){
		DbgOut( "MotFile : LoadMotData : AddMotion error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}


	//！！！boneno2serinoは、ShdHandler::ConvertInfluenceListと同期すること。
	int* boneno2serino;
	int* morphno2serino;
	boneno2serino = (int*)malloc( sizeof( int ) * lpmh->s2mot_leng );
	morphno2serino = (int*)malloc( sizeof( int ) * lpmh->s2mot_leng );
	if( !boneno2serino || !morphno2serino ){
		DbgOut( "MotFile : LoadMotData : boneno2serino alloc error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	int elemno;
	for( elemno = 0; elemno < lpmh->s2mot_leng; elemno++ ){
		*(boneno2serino + elemno) = -1;
		*(morphno2serino + elemno) = -1;
	}
	for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
		CMotionCtrl* mc = (*lpmh)( elemno );
		int bno = mc->boneno;
		if( bno >= 0 ){
			if( bno < lpmh->s2mot_leng ){
				*(boneno2serino + bno) = mc->serialno;
			}else{
				DbgOut( "MotFile : LoadMotData : boneno2serino : bno out of range !!!\n" );
				_ASSERT( 0 );
				free( boneno2serino );
				free( morphno2serino );
				return -1;
			}
		}
		int mno = mc->morphno;
		if( mno >= 0 ){
			if( mno < lpmh->s2mot_leng ){
				*(morphno2serino + mno) = mc->serialno;
			}else{
				DbgOut( "MotFile : LoadMotData : morphno2serino : mno out of range !!!\n" );
				_ASSERT( 0 );
				free( boneno2serino );
				free( morphno2serino );
				return -1;
			}
		}
	}


	while( !findend ){
		ZeroMemory( &mmarker, sizeof( MOTFILEMARKER ) );
		ret = ReadMarker( &mmarker );
		if( ret ){
			_ASSERT( 0 );
				free( boneno2serino );
				free( morphno2serino );
			return -1;
		}
		switch( mmarker.magicno ){
		case MOTMARKER_END:
			findend = 1;
			break;
		case MOTMARKER_BONE:
			ret = ReadBoneMatrix( lpmh, retcookie, mmarker, header.framenum, boneno2serino );
			if( ret ){
				_ASSERT( 0 );
				free( boneno2serino );
				free( morphno2serino );
				return -1;
			}
			break;
		case MOTMARKER_MORPH:
			ret = ReadMorphVal( lpmh, retcookie, mmarker, header.framenum, morphno2serino );
			if( ret ){
				_ASSERT( 0 );
				free( boneno2serino );
				free( morphno2serino );
				return -1;
			}
			break;
		default:
			_ASSERT( 0 );
			free( boneno2serino );
			free( morphno2serino );
			return -1;
			break;
		}
	}


	free( boneno2serino );
	free( morphno2serino );

	return 0;
}

int CMotFile::ReadBoneMatrix( CMotHandler* lpmh, int mcookie, MOTFILEMARKER mmarker, int framenum, int* bno2serino )
{
	int ret;
	if( framenum != mmarker.valnum ){
		_ASSERT( 0 );
		return 1;
	}
	CMatrix2* matptr;
	matptr = new CMatrix2[framenum];
	if( !matptr ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( matptr, sizeof( CMatrix2 ) * framenum );
	ret = ReadData( matptr, framenum );
	if( ret ){
		DbgOut( "MotFile : ReadBoneMatrix : mfile->ReadData error !!!\n" );
		_ASSERT( 0 );
		delete [] matptr;
		return 1;
	}
	int boneno = mmarker.serialno;
	if( (boneno < 0) || (boneno >= lpmh->s2mot_leng) ){
		//skip
		delete [] matptr;
		return 0;
	}
	int serino = *(bno2serino + boneno);
	if( (serino < 0) || (serino >= lpmh->s2mot_leng) ){
		//skip
		delete [] matptr;
		return 0;
	}

	CMotionCtrl* mc;
	mc = (*lpmh)( serino );
	ret = mc->CopyMotionMatrix( matptr, mcookie, framenum );
	if( ret ){
		DbgOut( "MotFile : ReadBoneMatrix : mctrl->CopyMotionMatrix error !!!\n" );
		_ASSERT( 0 );
		delete [] matptr;
		return 1;
	}

	delete [] matptr;

	return 0;
}
int CMotFile::ReadMorphVal( CMotHandler* lpmh, int mcookie, MOTFILEMARKER mmarker, int framenum, int* mno2serino )
{
	int ret;
	if( framenum != mmarker.valnum ){
		_ASSERT( 0 );
		return 1;
	}
	float* valptr;
	valptr = (float*)malloc( sizeof( float ) * framenum );
	if( !valptr ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( valptr, sizeof( float ) * framenum );
	ret = ReadData( valptr, framenum );
	if( ret ){
		_ASSERT( 0 );
		free( valptr );
		return 1;
	}
	int mno = mmarker.serialno;
	if( (mno < 0) || (mno >= lpmh->s2mot_leng) ){
		//skip
		_ASSERT( 0 );
		free( valptr );
		return 0;
	}
	int serino = *(mno2serino + mno);
	if( (serino < 0) || (serino >=lpmh->s2mot_leng) ){
		//skip
		_ASSERT( 0 );
		free( valptr );
		return 0;
	}
	CMotionCtrl* mc = (*lpmh)( serino );
	_ASSERT( mc );
	ret = mc->CopyJointVal( valptr, mcookie, framenum );
	if( ret ){
		_ASSERT( 0 );
		free( valptr );
		return 1;
	}

	free( valptr );
	return 0;
}

int CMotFile::OpenMotFile( char* fname )
{

	char pathname[2048];
	int ch = '\\';
	char* enptr = 0;
	enptr = strrchr( fname, ch );
	if( enptr ){
		strcpy_s( pathname, 2048, fname );
	}else{
		ZeroMemory( pathname, 2048 );
		int resdirleng;
		resdirleng = GetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPTSTR)pathname, 2048 );
		_ASSERT( resdirleng );
		strcat_s( pathname, 2048, fname );
	}

	hfile = CreateFile( (LPCTSTR)pathname, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CMotFile : OpenMotFile : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	m_writemode = 0;
	return 0;
}


int CMotFile::ReadHeader( MOTFILEHDR* hdr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( MOTFILEHDR );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)hdr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CMotFile : ReadHeader : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)hdr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}

int CMotFile::ReadData( CMatrix2* rmat, int matnum )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( CMatrix2 ) * matnum;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)rmat, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CMotFile : ReadData : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)rmat, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}

int CMotFile::WriteMarker( MOTFILEMARKER* wmarker )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( MOTFILEMARKER );
	WriteFile( hfile, (void*)wmarker, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "CMotFile : WriteMarker : writefile error %d %d\n", wleng, writeleng );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CMotFile::WriteData( float* wval, int valnum )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( float ) * valnum;
	WriteFile( hfile, (void*)wval, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "CMotFile : WriteData : writefile error %d %d\n", wleng, writeleng );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CMotFile::ReadMarker( MOTFILEMARKER* rmarker )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( MOTFILEMARKER );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)rmarker, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CMotFile : ReadMarker : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)rmarker, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;

}
int CMotFile::ReadData( float* rval, int valnum )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( float ) * valnum;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)rval, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CMotFile : ReadData : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)rval, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}
