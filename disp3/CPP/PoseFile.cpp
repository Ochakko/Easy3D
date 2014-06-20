#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <windows.h>

#include <PoseFile.h>
#include <crtdbg.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <shdelem.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>


#define DBGH
#include <dbg.h>

CPoseFile::CPoseFile( int srcmode )
{
	InitParams();
	m_mode = srcmode; 
}

CPoseFile::~CPoseFile()
{
	DestroyObjs();
}

int CPoseFile::InitParams()
{
	m_thandler = 0;
	m_shandler = 0;
	m_mhandler = 0;

	m_hfile = INVALID_HANDLE_VALUE;

	return 0;

}

int CPoseFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == 0 ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	return 0;
}

int CPoseFile::WritePoseHeader()
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( int );

	int magicno = POSEFILEMAGICNO;//!!!!!

	WriteFile( m_hfile, (void*)(&magicno), wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "CPoseFile::WritePoseHeader : writefile error %d %d\n", wleng, writeleng );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CPoseFile::WritePoseData( POSEDATA* srcpose )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( POSEDATA );

	WriteFile( m_hfile, (void*)srcpose, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "CPoseFile::WritePoseData : writefile error %d %d\n", wleng, writeleng );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CPoseFile::SavePoseFile( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, 
	char* filename, int srcmotid, int srcframeno, int* cplist, int createkeyflag )
{
	m_thandler = lpth;
	m_shandler = lpsh;
	m_mhandler = lpmh;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CPoseFile::SavePoseFile : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	int ret;
	ret = WritePoseHeader();
	if( ret ){
		DbgOut( "posefile : SavePoseFile : WritePoseHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int seri;
	CMotionCtrl* mcptr;
	for( seri = 0; seri < m_mhandler->s2mot_leng; seri++ ){
		mcptr = (*m_mhandler)( seri );
		_ASSERT( mcptr );

		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
			if( *( cplist + seri ) == 1 ){
				CMotionPoint2* existptr = 0;
				ret = m_mhandler->IsExistMotionPoint( srcmotid, seri, srcframeno, &existptr );
				if( ret ){
					DbgOut( "posefile : SavePoseFile : IsExistMotionPoint error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( existptr ){
					CMotionPoint2 mp;
					mp = *existptr;
					mp.m_spp = 0;
					ret = WriteMotionPoint( seri, &mp );
					if( ret ){
						DbgOut( "posefile : SavePoseFile : WriteMotionPoint error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					if( createkeyflag ){
						CShdElem* selem;
						selem = (*m_shandler)( seri );
						_ASSERT( selem );
						CMotionPoint2 mp;
						int hasmpflag = 0;
						ret = mcptr->CalcMotionPointOnFrame( selem, &mp, srcmotid, srcframeno, &hasmpflag, m_shandler, m_mhandler );
						if( ret ){
							DbgOut( "posefile : SavePoseFile : CalcMotionPointOnFrame error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
						mp.m_spp = 0;

						ret = WriteMotionPoint( seri, &mp );
						if( ret ){
							DbgOut( "posefile : SavePoseFile : WriteMotionPoint error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}
			}
		}
	}

	POSEDATA posedata;
	ZeroMemory( &posedata, sizeof( POSEDATA ) );
	posedata.serialno = -1;
	ret = WritePoseData( &posedata );
	if( ret ){
		DbgOut( "posefile : SavePoseFile : WritePoseData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CPoseFile::WriteMotionPoint( int seri, CMotionPoint2* mpptr )
{
	POSEDATA posedata;
	ZeroMemory( &posedata, sizeof( POSEDATA ) );

	posedata.serialno = seri;
	CTreeElem2* te;
	te = (*m_thandler)( seri );
	_ASSERT( te );
	strcpy_s( posedata.name, 256, te->name );

	posedata.m_rot.x = mpptr->m_rotx;
	posedata.m_rot.y = mpptr->m_roty;
	posedata.m_rot.z = mpptr->m_rotz;

	posedata.m_mv.x = mpptr->m_mvx;
	posedata.m_mv.y = mpptr->m_mvy;
	posedata.m_mv.z = mpptr->m_mvz;

	posedata.m_q = mpptr->m_q;

	posedata.interp = mpptr->interp;

	posedata.m_scale.x = mpptr->m_scalex;
	posedata.m_scale.y = mpptr->m_scaley;
	posedata.m_scale.z = mpptr->m_scalez;

	posedata.m_userint1 = mpptr->m_userint1;

	posedata.m_eul = mpptr->m_eul;


	int ret;
	ret = WritePoseData( &posedata );
	if( ret ){
		DbgOut( "posefile : WriteMotionPoint : WritePoseData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CPoseFile::CheckFileVersion()
{
	int magicno = 0;
	int ret;

	ret = ReadUCharaData( (unsigned char*)&magicno, sizeof( int ) );
	if( ret ){
		DbgOut( "posefile : CheckFileVersion : ReadUCharaData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( magicno != POSEFILEMAGICNO ){
		DbgOut( "posefile : CheckFileVersion : this file is not pose file error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CPoseFile::ReadUCharaData( unsigned char* dataptr, int insize )
{

	DWORD rleng, readleng;
	
	rleng = insize;

	ReadFile( m_hfile, (void*)dataptr, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "CPoseFile::ReadUCharaData : ReadFile error %d %d\n", rleng, readleng );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CPoseFile::LoadPoseFile( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, char* fname, 
	int srcmotid, int srcframeno )
{
	m_thandler = lpth;
	m_shandler = lpsh;
	m_mhandler = lpmh;

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

	m_hfile = CreateFile( (LPCTSTR)pathname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "PoseFile : LoadPoseFile : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	int ret;
	ret = CheckFileVersion();
	if( ret ){
		DbgOut( "posefile : LoadPoseFile : CheckFileVersion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int isend = 0;
	while( isend == 0 ){
		POSEDATA posedata;
		ZeroMemory( &posedata, sizeof( POSEDATA ) );

		ret = ReadUCharaData( (unsigned char*)&posedata, sizeof( POSEDATA ) );
		if( ret ){
			DbgOut( "posefile : LoadPoseFile : Read posedata error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( posedata.serialno <= 0 ){
			isend = 1;
		}else{
			ret = SetPoseData( srcmotid, srcframeno, &posedata );
			if( ret ){
				DbgOut( "posefile : LoadPoseFile : SetPoseData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}

int CPoseFile::SetPoseData( int srcmotid, int srcframeno, POSEDATA* poseptr )
{
	int ret;
	int seri = 0;
	ret = m_thandler->GetBoneNoByName( poseptr->name, &seri, m_shandler, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!
	}

	if( seri <= 0 ){
		return 0;//!!!!!!!!
	}


	CMotionPoint2* existptr = 0;
	ret = m_mhandler->IsExistMotionPoint( srcmotid, seri, srcframeno, &existptr );
	if( ret ){
		DbgOut( "posefile : SetPoseData : IsExistMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( existptr ){
		existptr->m_rotx = poseptr->m_rot.x;
		existptr->m_roty = poseptr->m_rot.y;
		existptr->m_rotz = poseptr->m_rot.z;

		existptr->m_mvx = poseptr->m_mv.x;
		existptr->m_mvy = poseptr->m_mv.y;
		existptr->m_mvz = poseptr->m_mv.z;

		existptr->m_q = poseptr->m_q;
		existptr->interp = poseptr->interp;

		existptr->m_scalex = poseptr->m_scale.x;
		existptr->m_scaley = poseptr->m_scale.y;
		existptr->m_scalez = poseptr->m_scale.z;

		existptr->m_userint1 = poseptr->m_userint1;
		existptr->m_eul = poseptr->m_eul;
	}else{
		CMotionPoint2 setmp;
		setmp.m_frameno = srcframeno;//!!!!!!!!!!!
		setmp.m_rotx = poseptr->m_rot.x;
		setmp.m_roty = poseptr->m_rot.y;
		setmp.m_rotz = poseptr->m_rot.z;

		setmp.m_mvx = poseptr->m_mv.x;
		setmp.m_mvy = poseptr->m_mv.y;
		setmp.m_mvz = poseptr->m_mv.z;

		setmp.m_q = poseptr->m_q;
		setmp.interp = poseptr->interp;

		setmp.m_scalex = poseptr->m_scale.x;
		setmp.m_scaley = poseptr->m_scale.y;
		setmp.m_scalez = poseptr->m_scale.z;

		setmp.m_userint1 = poseptr->m_userint1;
		setmp.m_eul = poseptr->m_eul;

		setmp.m_spp = 0;

		CMotionCtrl* mcptr;
		mcptr = (*m_mhandler)( seri );
		_ASSERT( mcptr );

		CMotionPoint2* retmp = 0;
		retmp = mcptr->AddMotionPoint( srcmotid, &setmp, setmp.interp );
		if( !retmp ){
			DbgOut( "PoseFile : SetPoseData : AddMotionPoint error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}


