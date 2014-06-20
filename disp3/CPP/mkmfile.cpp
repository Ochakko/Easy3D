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

#include <mkmfile.h>

#include <MkmMot.h>

#include <MkmRot.h>
#include <QElem.h>

#include <MkmTra.h>
#include <TraElem.h>

#define DBGH
#include <dbg.h>

static char s_mkmheader[256] = "Mikoto Motion Ver 2\r\n";
static char s_motheader[20] = "Motion {\r\n";


CMKMFile::CMKMFile()
{
	InitParams();
}

CMKMFile::~CMKMFile()
{
	DestroyObjs();
}

int CMKMFile::InitParams()
{
	ZeroMemory( &m_mkmbuf, sizeof( MKMBUF ) );
	m_mkmbuf.hfile = INVALID_HANDLE_VALUE;


	m_motnum = 0;
	m_mkmmot = 0;

	return 0;
}

int CMKMFile::DestroyObjs()
{
	if( m_mkmbuf.hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( m_mkmbuf.hfile );
		m_mkmbuf.hfile = INVALID_HANDLE_VALUE;
	}

	if( m_mkmbuf.buf ){
		free( m_mkmbuf.buf );
		m_mkmbuf.buf = 0;
	}
	m_mkmbuf.bufleng = 0;
	m_mkmbuf.pos = 0;
	m_mkmbuf.isend = 1;

	if( m_mkmmot ){
		delete [] m_mkmmot;
		m_mkmmot = 0;
	}
	m_motnum = 0;
	

	return 0;
}

int CMKMFile::LoadMKMFile( char* filename )
{
	int ret = 0;

	ret = SetBuffer( filename );
	if( ret ){
		DbgOut( "mkmfile : LoadMKMFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		goto ldmkmexit;
	}

	ret = CheckFileHeader();
	if( ret ){
		DbgOut( "mkmfile : LoadMKMFile : CheckFileHeader error !!!\n" );
		_ASSERT( 0 );
		goto ldmkmexit;
	}

	m_motnum = 0;
	ret = FindMotionBlock( 0, &m_motnum );
	if( ret ){
		DbgOut( "mkmfile : LoadMKMFile : FindMotionBlock 0 error !!!\n" );
		_ASSERT( 0 );
		goto ldmkmexit;
	}


	m_mkmmot = new CMkmMot[ m_motnum ];
	if( !m_mkmmot ){
		DbgOut( "mkmfile : LoadMKMFile : mkmmot alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto ldmkmexit;
	}

	int tmpnum = 0;
	ret = FindMotionBlock( 1, &tmpnum );
	if( ret ){
		DbgOut( "mkmfile : LoadMKMFile : FindMotionBlock 1 error !!!\n" );
		_ASSERT( 0 );
		goto ldmkmexit;
	}
	if( tmpnum != m_motnum ){
		DbgOut( "mkmfile : LoadMKMFile : m_motnum error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto ldmkmexit;
	}
//	DbgOut( "check!!!: mkmfile : motnum %d, bufleng %d\r\n", m_motnum, m_mkmbuf.bufleng );
//	int mno;
//	for( mno = 0; mno < m_motnum; mno++ ){
//		DbgOut( "check!!!: mkmfile : motno %d, motstart %d, motend %d\r\n", mno, (m_mkmmot + mno)->motstart, (m_mkmmot + mno)->motend );
//	}

	int motno;
	for( motno = 0; motno < m_motnum; motno++ ){
		CMkmMot* curmot = m_mkmmot + motno;
		ret = curmot->LoadMotion( &m_mkmbuf );
		if( ret ){
			DbgOut( "mkmfile : LoadMKMFile : mkmmot LoadMotion error !!!\n" );
			_ASSERT( 0 );
			goto ldmkmexit;
		}

		ret = curmot->SetKeyFrame();
		if( ret ){
			DbgOut( "mkmfile : LoadMKMFile : mkmmot SetKeyFrame error !!!\n" );
			_ASSERT( 0 );
			goto ldmkmexit;
		}

	}


//	int mno;
//	for( mno = 0; mno < m_motnum; mno++ ){
//		DbgOut( "check!!!: mkmfile : motno %d, motname %s, endframe %d\r\n", mno, (m_mkmmot + mno)->motname, (m_mkmmot + mno)->endframe );
//	}
//	for( motno = 0; motno < m_motnum; motno++ ){
//		CMkmMot* curmot = m_mkmmot + motno;
//
//		DbgOut( "check!!!: motno %d, rotnum %d, tranum %d, motstart %d, motend %d, keynum %d, frameleng %d\r\n", 
//			motno, curmot->rotnum, curmot->tranum, curmot->motstart, curmot->motend, curmot->keynum, curmot->frameleng );
//		
//		int keyno;
//		for( keyno = 0; keyno < curmot->keynum; keyno++ ){
//			DbgOut( "check !!!:    key %d --- %d\r\n", keyno, *( curmot->keyframe + keyno ) );
//		}
//
//
//		int rotno;
//		for( rotno = 0; rotno < curmot->rotnum; rotno++ ){
//			CMkmRot* currot = curmot->mkmrot + rotno;
//			DbgOut( "check !!!: rotno %d, rotstart %d, rotend %d\r\n", rotno, currot->rotstart, currot->rotend );
//			DbgOut( "check !!!: rotno %d, attach %s, elemnum %d\r\n", rotno, currot->attach, currot->elemnum );
//
//			int qno;
//			for( qno = 0; qno < currot->elemnum; qno++ ){
//				CQElem* curq = currot->qarray + qno;
//				DbgOut( "check !!!:     frameno %d, q( %f %f %f %f )\r\n",
//					curq->frameno, curq->q.x, curq->q.y, curq->q.z, curq->q.w );
//			}
//
//		}
//
//		int trano;
//		for( trano = 0; trano < curmot->tranum; trano++ ){
//			CMkmTra* curtra = curmot->mkmtra + trano;
//			DbgOut( "check !!!: trano %d, trastart %d, traend %d\r\n", trano, curtra->trastart, curtra->traend );
//			DbgOut( "check !!!: trano %d, attach %s, elemnum %d\r\n", trano, curtra->attach, curtra->elemnum );
//
//			int tno;
//			for( tno = 0; tno < curtra->elemnum; tno++ ){
//				CTraElem* curt = curtra->traarray + tno;
//				DbgOut( "check !!!:     frameno %d, tra( %f %f %f )\r\n",
//					curt->frameno, curt->tra.x, curt->tra.y, curt->tra.z );
//			}
//		}
//	}



	goto ldmkmexit;
ldmkmexit:
	if( m_mkmbuf.hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( m_mkmbuf.hfile );
		m_mkmbuf.hfile = INVALID_HANDLE_VALUE;
	}
	if( m_mkmbuf.buf ){
		free( m_mkmbuf.buf );
		m_mkmbuf.buf = 0;
	}

	return ret;
}

int CMKMFile::SetBuffer( char* filename )
{
	DestroyObjs();

	HANDLE hfile;
	hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	
	m_mkmbuf.hfile = hfile;


	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_mkmbuf.hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "MKMFile : SetBuffer :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "MKMFile : SetBuffer :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );//bufleng + 1
	if( !newbuf ){
		DbgOut( "MKMFile : SetBuffer :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_mkmbuf.hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "MKMFile : SetBuffer :  ReadFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_mkmbuf.buf = newbuf;
	*(m_mkmbuf.buf + bufleng) = 0;//!!!!!!!!
	m_mkmbuf.bufleng = bufleng;
	m_mkmbuf.pos = 0;
	m_mkmbuf.isend = 0;

	return 0;
}

int CMKMFile::CheckFileHeader()
{
	int patleng;
	patleng = (int)strlen( s_mkmheader );

	int cmp;
	cmp = strncmp( m_mkmbuf.buf, s_mkmheader, patleng );

	if( cmp == 0 ){
		m_mkmbuf.pos = patleng;
	}

	return cmp;
}

int CMKMFile::FindMotionBlock( int setflag, int* numptr )
{
	//int ret;

	int patleng;
	patleng = (int)strlen( s_motheader );

	int setno = 0;
	int curpos = m_mkmbuf.pos;
	char* patptr = m_mkmbuf.buf;
	while( patptr ){
		patptr = strstr( m_mkmbuf.buf + curpos, s_motheader );
		if( patptr ){

			curpos = (int)( patptr - m_mkmbuf.buf + patleng );

			if( setflag ){
				if( setno != 0 ){
					(m_mkmmot + setno - 1)->motend = (int)( patptr - m_mkmbuf.buf );
				}
				(m_mkmmot + setno)->motstart = curpos;
			}
			setno++;
		}
	}

	if( setflag ){
		if( setno != 0 ){
			(m_mkmmot + setno - 1)->motend = m_mkmbuf.bufleng - 1;
		}
	}

	*numptr = setno;

	return 0;
}

int CMKMFile::SetMotion( CMKMFile* inimkm, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, int motno, float tramult, LONG* motidptr )
{
	if( (motno < 0) || (motno >= m_motnum) ){
		DbgOut( "mkmfile : SetMotion : motno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( inimkm->m_motnum <= 0 ){
		DbgOut( "mkmfile : SetMotion : inimkm not include mkmmot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CMkmMot* curmot;
	curmot = m_mkmmot + motno;

	int ret;

	ret = curmot->SetMotion( inimkm->m_mkmmot, lpth, lpsh, lpmh, tramult, motidptr );
	if( ret ){
		DbgOut( "mkmfile : SetMotion : mkmmot SetMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
