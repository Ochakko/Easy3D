#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <MkmTra.h>

#include <TraElem.h>

#include <treehandler2.h>
#include <mothandler.h>
#include <motionpoint2.h>

#define DBGH
#include <dbg.h>


CMkmTra::CMkmTra()
{

	InitParams();
}

CMkmTra::~CMkmTra()
{
	DestroyObjs();
}

int CMkmTra::InitParams()
{
	trastart = 0;//“Ç‚Ýž‚Ýpos
	traend = 0;

	ZeroMemory( attach, sizeof( char ) * 256 );
	endframe = 0;
	traarray = 0;
	elemnum = 0;

	ZeroMemory( m_linechar, sizeof( char ) * MKMLINELENG );
	return 0;
}

int CMkmTra::DestroyObjs()
{

	if( traarray ){
		delete [] traarray;
		traarray = 0;
	}
	elemnum = 0;

	return 0;
}

int CMkmTra::LoadTra( MKMBUF* mkmbuf )
{

	DestroyObjs();

	mkmbuf->pos = trastart;
	mkmbuf->isend = 0;

	int ret;
//attach
	ret = GetLine( mkmbuf );
	if( ret ){
		DbgOut( "MkmTra : LoadTra : GetLine 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = GetAttachName();
	if( ret ){
		DbgOut( "MkmTra : LoadTra : GetAttachName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( mkmbuf->isend ){
		DbgOut( "MkmTra : LoadTra : isend 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
/////
	elemnum = 0;
	ret = GetTraNum( mkmbuf, &elemnum );
	if( ret ){
		DbgOut( "MkmTra : LoadTra : GetQNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( elemnum > 0 ){
		traarray = new CTraElem[ elemnum ];
		if( !traarray ){
			DbgOut( "MkmTra : LoadTra : traarray alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = GetLine( mkmbuf );
		if( ret ){
			DbgOut( "mkmtra : LoadTra : GetLine 01 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = GetLine( mkmbuf );
		if( ret ){
			DbgOut( "mkmtra : LoadTra : GetLine 02 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = GetLine( mkmbuf );
		if( ret ){
			DbgOut( "mkmtra : LoadTra : GetLine 03 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int trano;
		for( trano = 0; trano < elemnum; trano++ ){
			if( mkmbuf->isend ){
				DbgOut( "MkmTra : LoadTra : isend 2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = GetLine( mkmbuf );
			if( ret ){
				DbgOut( "MkmTra : LoadTra : GetLine 2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = GetTraElem( traarray + trano );
			if( ret ){
				DbgOut( "MkmTra : LoadTra : GetTraElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}

int CMkmTra::GetLine( MKMBUF* mkmbuf )
{
	ZeroMemory( m_linechar, sizeof( char ) * MKMLINELENG );

	char* retptr;
	int leng;
	int curpos;
	retptr = strstr( mkmbuf->buf + mkmbuf->pos, "\r\n" );
	if( retptr ){
		curpos = (int)( retptr - mkmbuf->buf );
		if( curpos > traend ){
			curpos = traend;
		}
		leng = curpos - mkmbuf->pos + 2;//‰üs‚àŠÜ‚ß‚é
	}else{
		curpos = traend;
		leng = curpos - mkmbuf->pos;
	}


	if( leng >= MKMLINELENG ){
		DbgOut( "MkmTra :GetLine : line leng too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strncpy_s( m_linechar, MKMLINELENG, mkmbuf->buf + mkmbuf->pos, leng );

	mkmbuf->pos += leng;
	if( mkmbuf->pos >= traend ){
		mkmbuf->isend = 1;
	}

	return 0;
}

int CMkmTra::GetAttachName()
{
	char startpat[15] = "name = \"";
	char endpat[15] = "\"\r\n";

	int startpatleng;
	startpatleng = (int)strlen( startpat );


	char* startptr;
	startptr = strstr( m_linechar, startpat );
	if( !startptr ){
		DbgOut( "MkmTra :GetAttachName : startpat not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* endptr;
	endptr = strstr( startptr + startpatleng, endpat );
	if( !endptr ){
		DbgOut( "MkmTra :GetAttachName : endpat not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int leng;
	leng = (int)( endptr - startptr - startpatleng );

	if( leng > 255 ){
		leng = 255;
	}

	strncpy_s( attach, 256, startptr + startpatleng, leng );
	*(attach + leng) = 0;

	return 0;
}

int CMkmTra::GetEndFrame()
{
	char startpat[15] = "endframe = ";
	char endpat[15] = "\r\n";

	int startpatleng;
	startpatleng = (int)strlen( startpat );


	char* startptr;
	startptr = strstr( m_linechar, startpat );
	if( !startptr ){
		DbgOut( "MkmTra :GetEndFrame : startpat not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* endptr;
	endptr = strstr( startptr + startpatleng, endpat );
	if( !endptr ){
		DbgOut( "MkmTra :GetEndFrame : endpat not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int leng;
	leng = (int)( endptr - startptr - startpatleng );

	if( leng > 255 ){
		leng = 255;
	}

	char tempchar[256];

	strncpy_s( tempchar, 256, startptr + startpatleng, leng );
	*(tempchar + leng) = 0;


	endframe = atoi( tempchar );

	return 0;
}

int CMkmTra::GetTraNum( MKMBUF* mkmbuf, int* numptr )
{
	char pat[10] = ")\r\n";
	int patleng = (int)strlen( pat );

	int findno = 0;
	char* findptr = mkmbuf->buf;
	int curpos = mkmbuf->pos;
	int endflag = 0;

	while( endflag == 0 ){
		findptr = strstr( mkmbuf->buf + curpos, pat );
		if( findptr ){
			curpos = (int)( findptr - mkmbuf->buf + patleng );
			if( curpos < traend ){
				findno++;
			}else{
				endflag = 1;
			}
		}else{
			endflag = 1;
		}
	}

	*numptr = findno;

	return 0;
}

int CMkmTra::GetTraElem( CTraElem* dsttra )
{
	int ret;
	int pos = 0;
	int stepnum = 0;

	int lineleng = (int)strlen( m_linechar );

	ret = GetInt( &dsttra->frameno, m_linechar, pos, lineleng, &stepnum );
	if( ret ){
		DbgOut( "MkmTra : GetTraElem : GetInt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

	ret = GetFloat( &dsttra->tra.x, m_linechar, pos, lineleng, &stepnum );
	if( ret ){
		DbgOut( "MkmTra : GetTraElem : GetFloat x error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

	ret = GetFloat( &dsttra->tra.y, m_linechar, pos, lineleng, &stepnum );
	if( ret ){
		DbgOut( "MkmTra : GetTraElem : GetFloat y error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

	ret = GetFloat( &dsttra->tra.z, m_linechar, pos, lineleng, &stepnum );
	if( ret ){
		DbgOut( "MkmTra : GetTraElem : GetFloat z error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

	return 0;
}

int CMkmTra::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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
int CMkmTra::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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

int CMkmTra::SetMotion( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, int motid, int curkey, float tramult )
{
	int ret;

	if( serialno <= 0 ){
		DbgOut( "mkmtra : SetMotion : serialno <= 0 skip !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!
	}

	CMotionPoint2* mp;
	ret = lpmh->IsExistMotionPoint( motid, serialno, curkey, &mp );
	if( ret || !mp ){
		DbgOut( "mkmtra : SetMotion : mh IsExistMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int keyno;
	CTraElem* findtra = 0;
	for( keyno = 0; keyno < elemnum; keyno++ ){
		CTraElem* curtra = traarray + keyno;
		if( curkey == curtra->frameno ){
			findtra = curtra;
			break;
		}
	}

	if( !findtra ){
		DbgOut( "mkmrot : SetMotion : findtra NULL skip !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!
	}

	mp->m_mvx = findtra->tra.x * tramult;
	mp->m_mvy = findtra->tra.y * tramult;
	mp->m_mvz = findtra->tra.z * tramult;
	
	return 0;
}

int CMkmTra::SetSerialNo( CTreeHandler2* lpth, CShdHandler* lpsh )
{
	char lpat[10] = "[L]";
	char rpat[10] = "[R]";
	int lpatleng = (int)strlen( lpat );
	int rpatleng = (int)strlen( rpat );

	char* lptr;
	lptr = strstr( attach, lpat );
	if( lptr ){
		char* endptr;
		endptr = lptr + lpatleng;
		if( *endptr == 0 ){
			strcat_s( attach, 256, "_X+" );
		}
	}
	
	char* rptr;
	rptr = strstr( attach, rpat );
	if( rptr ){
		char* endptr;
		endptr = rptr + rpatleng;
		if( *endptr == 0 ){
			strcat_s( attach, 256, "_X-" );
		}
	}

	char jpat[10] = "j_";
	int jpatleng = (int)strlen( jpat );
	int cmp;
	cmp = strncmp( attach, jpat, jpatleng );
	if( cmp == 0 ){
		char tempchar[256];
		ZeroMemory( tempchar, sizeof( char ) * 256 );
		strcpy_s( tempchar, 256, attach + jpatleng );

		attach[0] = 0;
		strcpy_s( attach, 256, tempchar );
	}

	int ret;
	ret = lpth->GetBoneNoByName( attach, &serialno, lpsh, 0 );
	if( ret || (serialno <= 0) ){
		DbgOut( "mkmtra : SetSerialNo : th GetBoneNoByName error skip !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!
	}


	return 0;
}

