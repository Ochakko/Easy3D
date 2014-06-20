#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <SndFullFrame.h>
#include <SndFrame.h>
#include <SndKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>



CSndFullFrame::CSndFullFrame()
{
	InitParams();
}
CSndFullFrame::~CSndFullFrame()
{
	DestroyObjs();
}

int CSndFullFrame::InitParams()
{
//	int m_framenum;
//	int m_range;
//	CSndFrame* m_pframe;

	m_framenum = 0;
	m_range = 0;
	m_pframe = 0;

	return 0;
}
int CSndFullFrame::DestroyObjs()
{
	if( m_pframe ){
		delete [] m_pframe;
		m_pframe = 0;
	}

	InitParams();


	return 0;
}

int CSndFullFrame::CreateFullFrame( CSndKey* firstkey, int framenum, int range )
{
	DestroyObjs();

	m_framenum = framenum;
	m_range = range;


	m_pframe = new CSndFrame[ m_framenum ];
	if( !m_pframe ){
		DbgOut( "sndfullframe : CreateFullFrame pframe alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !firstkey ){
		return 0;
	}

	int ret;
	CSndKey* curkey = firstkey;
	while( curkey ){
		int curframe;
		int rcnt;
		curframe = curkey->m_frameno;

		for( rcnt = 0; rcnt <= m_range; rcnt++ ){
			int addframeno = curframe + rcnt;
			if( addframeno < m_framenum ){
				CSndFrame* cursf;
				cursf = m_pframe + curframe + rcnt;

				ret = cursf->AddSndKey( curkey );
				if( ret ){
					DbgOut( "sndfullframe : CreateFullFrame : sf AddSndKey error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( rcnt == 0 ){
					cursf->m_keyflag = 1;
				}
			}
		}

		curkey = curkey->next;
	}

	return 0;
}
CSndFrame* CSndFullFrame::GetFrame( int frameno )
{
	if( !m_pframe || (frameno < 0) || (frameno >= m_framenum) ){
		return 0;
	}

	return ( m_pframe + frameno );
}

