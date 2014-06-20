#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <MkmRot.h>

#include <QElem.h>

#include <treehandler2.h>
#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>
#include <motionpoint2.h>
#include <quaternion.h>

#include <BoneProp.h>

#define DBGH
#include <dbg.h>


CMkmRot::CMkmRot()
{

	InitParams();
}

CMkmRot::~CMkmRot()
{
	DestroyObjs();
}

int CMkmRot::InitParams()
{
	rotstart = 0;//“Ç‚Ýž‚Ýpos
	rotend = 0;

	ZeroMemory( attach, sizeof( char ) * 256 );
	endframe = 0;
	qarray = 0;
	elemnum = 0;

	ZeroMemory( m_linechar, sizeof( char ) * MKMLINELENG );

	return 0;
}

int CMkmRot::DestroyObjs()
{

	if( qarray ){
		delete [] qarray;
		qarray = 0;
	}
	elemnum = 0;

	return 0;
}

int CMkmRot::LoadRot( MKMBUF* mkmbuf )
{

	DestroyObjs();

	mkmbuf->pos = rotstart;
	mkmbuf->isend = 0;

	int ret;
//attach
	ret = GetLine( mkmbuf );
	if( ret ){
		DbgOut( "mkmrot : LoadRot : GetLine 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = GetAttachName();
	if( ret ){
		DbgOut( "mkmrot : LoadRot : GetAttachName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( mkmbuf->isend ){
		DbgOut( "mkmrot : LoadRot : isend 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
/////
	elemnum = 0;
	ret = GetQNum( mkmbuf, &elemnum );
	if( ret ){
		DbgOut( "mkmrot : LoadRot : GetQNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( elemnum > 0 ){
		qarray = new CQElem[ elemnum ];
		if( !qarray ){
			DbgOut( "mkmrot : LoadRot : qarray alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = GetLine( mkmbuf );
		if( ret ){
			DbgOut( "mkmrot : LoadRot : GetLine 01 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = GetLine( mkmbuf );
		if( ret ){
			DbgOut( "mkmrot : LoadRot : GetLine 02 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = GetLine( mkmbuf );
		if( ret ){
			DbgOut( "mkmrot : LoadRot : GetLine 03 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		int qno;
		for( qno = 0; qno < elemnum; qno++ ){
			if( mkmbuf->isend ){
				DbgOut( "mkmrot : LoadRot : isend 2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = GetLine( mkmbuf );
			if( ret ){
				DbgOut( "mkmrot : LoadRot : GetLine 2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = GetQElem( qarray + qno );
			if( ret ){
				DbgOut( "mkmrot : LoadRot : GetQElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}

int CMkmRot::GetLine( MKMBUF* mkmbuf )
{
	ZeroMemory( m_linechar, sizeof( char ) * MKMLINELENG );

	char* retptr;
	int leng;
	int curpos;
	retptr = strstr( mkmbuf->buf + mkmbuf->pos, "\r\n" );
	if( retptr ){
		curpos = (int)( retptr - mkmbuf->buf );
		if( curpos > rotend ){
			curpos = rotend;
		}
		leng = curpos - mkmbuf->pos + 2;//‰üs‚àŠÜ‚ß‚é
	}else{
		curpos = rotend;
		leng = curpos - mkmbuf->pos;
	}


	if( leng >= MKMLINELENG ){
		DbgOut( "mkmrot :GetLine : line leng too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strncpy_s( m_linechar, MKMLINELENG, mkmbuf->buf + mkmbuf->pos, leng );

	mkmbuf->pos += leng;
	if( mkmbuf->pos >= rotend ){
		mkmbuf->isend = 1;
	}

	return 0;
}

int CMkmRot::GetAttachName()
{
	char startpat[15] = "name = \"";
	char endpat[15] = "\"\r\n";

	int startpatleng;
	startpatleng = (int)strlen( startpat );


	char* startptr;
	startptr = strstr( m_linechar, startpat );
	if( !startptr ){
		DbgOut( "mkmrot :GetAttachName : startpat not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* endptr;
	endptr = strstr( startptr + startpatleng, endpat );
	if( !endptr ){
		DbgOut( "mkmrot :GetAttachName : endpat not found error !!!\n" );
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

int CMkmRot::GetEndFrame()
{
	char startpat[15] = "endframe = ";
	char endpat[15] = "\r\n";

	int startpatleng;
	startpatleng = (int)strlen( startpat );


	char* startptr;
	startptr = strstr( m_linechar, startpat );
	if( !startptr ){
		DbgOut( "mkmrot :GetEndFrame : startpat not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* endptr;
	endptr = strstr( startptr + startpatleng, endpat );
	if( !endptr ){
		DbgOut( "mkmrot :GetEndFrame : endpat not found error !!!\n" );
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

int CMkmRot::GetQNum( MKMBUF* mkmbuf, int* numptr )
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
			if( curpos < rotend ){
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

int CMkmRot::GetQElem( CQElem* dstq )
{
	int ret;
	int pos = 0;
	int stepnum = 0;

	int lineleng = (int)strlen( m_linechar );

	ret = GetInt( &dstq->frameno, m_linechar, pos, lineleng, &stepnum );
	if( ret ){
		DbgOut( "mkmrot : GetQElem : GetInt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

	CQuaternion tmpq;

	ret = GetFloat( &tmpq.x, m_linechar, pos, lineleng, &stepnum );
	if( ret ){
		DbgOut( "mkmrot : GetQElem : GetFloat x error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

	ret = GetFloat( &tmpq.y, m_linechar, pos, lineleng, &stepnum );
	if( ret ){
		DbgOut( "mkmrot : GetQElem : GetFloat y error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

	ret = GetFloat( &tmpq.z, m_linechar, pos, lineleng, &stepnum );
	if( ret ){
		DbgOut( "mkmrot : GetQElem : GetFloat z error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

	ret = GetFloat( &tmpq.w, m_linechar, pos, lineleng, &stepnum );
	if( ret ){
		DbgOut( "mkmrot : GetQElem : GetFloat w error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

//	tmpq.inv( &dstq->q );

	tmpq.normalize();

	dstq->q = tmpq;


	return 0;
}

int CMkmRot::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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
int CMkmRot::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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

int CMkmRot::SetMotion( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, int motid, int curkey,
	CQuaternion* invq, CQuaternion* accumq, CQuaternion* accuminvq )
{
	int ret;

	if( serialno <= 0 ){
		DbgOut( "mkmrot : SetMotion : serialno <= 0 skip !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!
	}


	CMotionPoint2* mp;
	ret = lpmh->IsExistMotionPoint( motid, serialno, curkey, &mp );
	if( ret || !mp ){
		DbgOut( "mkmrot : SetMotion : mh IsExistMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int keyno;
	CQElem* findq = 0;
	D3DXVECTOR3 befeul;
	for( keyno = 0; keyno < elemnum; keyno++ ){
		CQElem* curq = qarray + keyno;
		if( curkey == curq->frameno ){
			findq = curq;
			if( keyno != 0 ){
				befeul = (qarray + keyno - 1)->eul;
			}else{
				befeul.x = 0.0f;
				befeul.y = 0.0f;
				befeul.z = 0.0f;
			}
			break;
		}
	}

	if( !findq ){
		DbgOut( "mkmrot : SetMotion : findq NULL skip !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!
	}

	/***
	D3DXVECTOR3 neweul;
	ret =  qToEuler( 0, &findq->q, &neweul );
	_ASSERT( !ret );
	ret = modifyEuler( &neweul, &befeul );
	_ASSERT( !ret );
	findq->eul = neweul;

	CQuaternion q, qx, qy, qz;
	D3DXVECTOR3 axisX( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 axisY( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 axisZ( 0.0f, 0.0f, 1.0f );
	qx.SetAxisAndRot( axisX, neweul.x * (float)DEG2PAI );
	qy.SetAxisAndRot( axisY, neweul.y * (float)DEG2PAI );
	qz.SetAxisAndRot( axisZ, neweul.z * (float)DEG2PAI );

	q = qy * qx * qz;
	mp->m_q = q;
	***/

	/***
	mp->m_q.x = findq->q.x;	
	mp->m_q.y = findq->q.y;	
	mp->m_q.z = -findq->q.z;	
	mp->m_q.w = -findq->q.w;	
	***/

	//mp->m_q = findq->q;

	//mp->m_q = *accuminvq * *invq * findq->q * *accumq;
	mp->m_q = *accumq * findq->q * *invq * *accuminvq;
	mp->m_q.z *= -1.0f;
	mp->m_q.w *= -1.0f;


	return 0;
}

int CMkmRot::SetSerialNo( CTreeHandler2* lpth, CShdHandler* lpsh )
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

	int ret;
	ret = lpth->GetBoneNoByName( attach, &serialno, lpsh, 0 );
	if( ret || (serialno <= 0) ){
		DbgOut( "mkmrot : SetSerialNo : th GetBoneNoByName error skip !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!
	}



	return 0;
}
