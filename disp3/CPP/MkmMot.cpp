#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <MkmMot.h>

#include <MkmRot.h>
#include <QElem.h>

#include <MkmTra.h>
#include <TraElem.h>

#include <treehandler2.h>
#include <mothandler.h>
#include <shdhandler.h>
#include <shdelem.h>
#include <motionctrl.h>
#include <motioninfo.h>
#include <motionpoint2.h>
#include <quaternion.h>


#define DBGH
#include <dbg.h>


CMkmMot::CMkmMot()
{

	InitParams();
}

CMkmMot::~CMkmMot()
{
	DestroyObjs();
}

int CMkmMot::InitParams()
{
	motstart = 0;
	motend = 0;
	ZeroMemory( motname, sizeof( char ) * 256 );
	endframe = 0;
	keynum = 0;
	keyframe = 0;
	frameleng = 0;

	rotnum = 0;
	mkmrot = 0;

	tranum = 0;
	mkmtra = 0;

	ZeroMemory( m_linechar, sizeof( char ) * MKMLINELENG );


	return 0;
}

int CMkmMot::DestroyObjs()
{
	if( keyframe ){
		free( keyframe );
		keyframe = 0;
	}
	keynum = 0;

	if( mkmrot ){
		delete [] mkmrot;
		mkmrot = 0;
	}
	rotnum = 0;

	if( mkmtra ){
		delete [] mkmtra;
		mkmtra = 0;
	}
	tranum = 0;

	return 0;
}

int CMkmMot::LoadMotion( MKMBUF* mkmbuf )
{

	DestroyObjs();

	mkmbuf->pos = motstart;
	mkmbuf->isend = 0;

	int ret;
//motname
	ret = GetLine( mkmbuf );
	if( ret ){
		DbgOut( "mkmmot : LoadMotion : GetLine 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = GetMotionName();
	if( ret ){
		DbgOut( "mkmmot : LoadMotion : GetMotionName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( mkmbuf->isend ){
		DbgOut( "mkmmot : LoadMotion : isend 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//endframe
	ret = GetLine( mkmbuf );
	if( ret ){
		DbgOut( "mkmmot : LoadMotion : GetLine 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = GetEndFrame();
	if( ret ){
		DbgOut( "mkmmot : LoadMotion : GetEndFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( mkmbuf->isend ){
		DbgOut( "mkmmot : LoadMotion : isend 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
/////
	rotnum = 0;
	ret = FindRotation( mkmbuf, 0, &rotnum );
	if( ret ){
		DbgOut( "mkmmot : LoadMotion : FindRotation 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( rotnum > 0 ){
		mkmrot = new CMkmRot[ rotnum ];
		if( !mkmrot ){
			DbgOut( "mkmmot : LoadMotion : mkmrot alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int tempnum = 0;
		ret = FindRotation( mkmbuf, 1, &tempnum );
		if( ret ){
			DbgOut( "mkmmot : LoadMotion : FindRotation 1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( rotnum != tempnum ){
			DbgOut( "mkmmot : LoadMotion : rotnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
//////
	tranum = 0;
	ret = FindTrans( mkmbuf, 0, &tranum );
	if( ret ){
		DbgOut( "mkmmot : LoadMotion : FindTrans 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( tranum > 0 ){
		mkmtra = new CMkmTra[ tranum ];
		if( !mkmtra ){
			DbgOut( "mkmmot : LoadMotion : mkmtra alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int tempnum = 0;
		ret = FindTrans( mkmbuf, 1, &tempnum );
		if( ret ){
			DbgOut( "mkmmot : LoadMotion : FindTrans 1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( tranum != tempnum ){
			DbgOut( "mkmmot : LoadMotion : tranum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
/////////
	int rotno;
	for( rotno = 0; rotno < rotnum; rotno++ ){
		CMkmRot* currot;
		currot = mkmrot + rotno;
		ret = currot->LoadRot( mkmbuf );
		if( ret ){
			DbgOut( "mkmmot : LoadMotion : mkmrot LoadRot error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
/////////
	int trano;
	for( trano = 0; trano < tranum; trano++ ){
		CMkmTra* curtra;
		curtra = mkmtra + trano;
		ret = curtra->LoadTra( mkmbuf );
		if( ret ){
			DbgOut( "mkmmot : LoadMotion : mkmtra LoadTra error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}

int CMkmMot::GetLine( MKMBUF* mkmbuf )
{
	ZeroMemory( m_linechar, sizeof( char ) * MKMLINELENG );

	char* retptr;
	int leng;
	int curpos;
	retptr = strstr( mkmbuf->buf + mkmbuf->pos, "\r\n" );
	if( retptr ){
		curpos = (int)( retptr - mkmbuf->buf );
		if( curpos > motend ){
			curpos = motend;
		}
		leng = curpos - mkmbuf->pos + 2;//改行も含める
	}else{
		curpos = motend;
		leng = curpos - mkmbuf->pos;
	}


	if( leng >= MKMLINELENG ){
		DbgOut( "mkmmot : GetLine : line leng too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strncpy_s( m_linechar, MKMLINELENG, mkmbuf->buf + mkmbuf->pos, leng );

	mkmbuf->pos += leng;
	if( mkmbuf->pos >= motend ){
		mkmbuf->isend = 1;
	}

	return 0;
}

int CMkmMot::GetMotionName()
{
	char startpat[15] = "name = \"";
	char endpat[15] = "\"\r\n";

	int startpatleng;
	startpatleng = (int)strlen( startpat );


	char* startptr;
	startptr = strstr( m_linechar, startpat );
	if( !startptr ){
		DbgOut( "mkmmot : GetMotionName : startpat not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* endptr;
	endptr = strstr( startptr + startpatleng, endpat );
	if( !endptr ){
		DbgOut( "mkmmot : GetMotionName : endpat not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int leng;
	leng = (int)( endptr - startptr - startpatleng );

	if( leng > 255 ){
		leng = 255;
	}

	strncpy_s( motname, 256, startptr + startpatleng, leng );
	*(motname + leng) = 0;

	return 0;
}

int CMkmMot::GetEndFrame()
{
	char startpat[15] = "endframe = ";
	char endpat[15] = "\r\n";

	int startpatleng;
	startpatleng = (int)strlen( startpat );


	char* startptr;
	startptr = strstr( m_linechar, startpat );
	if( !startptr ){
		DbgOut( "mkmmot : GetEndFrame : startpat not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* endptr;
	endptr = strstr( startptr + startpatleng, endpat );
	if( !endptr ){
		DbgOut( "mkmmot : GetEndFrame : endpat not found error !!!\n" );
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

int CMkmMot::FindRotation( MKMBUF* mkmbuf, int setflag, int* numptr )
{
	char startpat[20] = "Quaternion {\r\n";
	char endpat[10] = "}\r\n";

	int startpatleng = (int)strlen( startpat );
	int endpatleng = (int)strlen( endpat );

	char* findstart = mkmbuf->buf;
	char* findend = mkmbuf->buf;
	int startpos = mkmbuf->pos;
	int endpos = mkmbuf->pos;
	int setno = 0;
	int endflag = 0;
	while( endflag == 0 ){
		findstart = strstr( mkmbuf->buf + endpos, startpat );		
		if( findstart ){
			startpos = (int)( findstart - mkmbuf->buf + startpatleng );
			if( startpos >= motend ){
				endflag = 1;//!!!!!!!!!!
			}else{
				findend = strstr( mkmbuf->buf + startpos, endpat );
				if( findend ){
					endpos = (int)( findend - mkmbuf->buf + endpatleng );
					if( endpos > motend ){
						endpos = motend;
						endflag = 1;//!!!!!!!!!!
					}
				}else{
					endpos = motend;
					endflag = 1;//!!!!!!!!!!
				}
					
				if( setflag ){
					(mkmrot + setno)->rotstart = startpos;
					(mkmrot + setno)->rotend = endpos;
				}
				setno++;
			}
		}else{
			endflag = 1;//!!!!!!!!!!
		}
	}

	*numptr = setno;

	return 0;
}

int CMkmMot::FindTrans( MKMBUF* mkmbuf, int setflag, int* numptr )
{
	char startpat[20] = "Vector {\r\n";
	char endpat[10] = "}\r\n";

	int startpatleng = (int)strlen( startpat );
	int endpatleng = (int)strlen( endpat );

	char* findstart = mkmbuf->buf;
	char* findend = mkmbuf->buf;
	int startpos = mkmbuf->pos;
	int endpos = mkmbuf->pos;
	int setno = 0;
	int endflag = 0;
	while( endflag == 0 ){
		findstart = strstr( mkmbuf->buf + endpos, startpat );		
		if( findstart ){
			startpos = (int)( findstart - mkmbuf->buf + startpatleng );
			if( startpos >= motend ){
				endflag = 1;//!!!!!!!!!!
			}else{
				findend = strstr( mkmbuf->buf + startpos, endpat );
				if( findend ){
					endpos = (int)( findend - mkmbuf->buf + endpatleng );
					if( endpos > motend ){
						endpos = motend;
						endflag = 1;//!!!!!!!!!!
					}
				}else{
					endpos = motend;
					endflag = 1;//!!!!!!!!!!
				}
					
				if( setflag ){
					(mkmtra + setno)->trastart = startpos;
					(mkmtra + setno)->traend = endpos;
				}
				setno++;
			}
		}else{
			endflag = 1;//!!!!!!!!!!
		}
	}

	*numptr = setno;

	return 0;
}

int CMkmMot::SetKeyFrame()
{
	if( keyframe ){
		free( keyframe );
		keyframe = 0;
	}
	frameleng = 0;
	keynum = 0;


	int maxframe;
	if( mkmrot && mkmrot->qarray ){
		maxframe = (mkmrot->qarray + mkmrot->elemnum - 1)->frameno;	
		frameleng = max( (maxframe + 1), (endframe + 1) );

		keynum = mkmrot->elemnum;

		keyframe = (int*)malloc( sizeof( int ) * keynum );
		if( !keyframe ){
			DbgOut( "mkmmot : SetKeyFrame : keyframe alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int keyno;
		for( keyno = 0; keyno < keynum; keyno++ ){
			CQElem* curq = mkmrot->qarray + keyno;
			*( keyframe + keyno ) = curq->frameno;
		}

	}else if( mkmtra && mkmtra->traarray ){
		maxframe = (mkmtra->traarray + mkmtra->elemnum - 1)->frameno;	
		frameleng = max( (maxframe + 1), (endframe + 1) );

		keynum = mkmtra->elemnum;

		keyframe = (int*)malloc( sizeof( int ) * keynum );
		if( !keyframe ){
			DbgOut( "mkmmot : SetKeyFrame : keyframe alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int keyno;
		for( keyno = 0; keyno < keynum; keyno++ ){
			CTraElem* curt = mkmtra->traarray + keyno;
			*( keyframe + keyno ) = curt->frameno;
		}
	}


	return 0;
}

int CMkmMot::SetMotion( CMkmMot* inimot, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, float tramult, LONG* motidptr )
{
	int ret;

	if( rotnum != inimot->rotnum ){
		DbgOut( "mkmmot : SetMotion : inimot rotnum not equal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int motid;
	motid = lpmh->AddMotion( ZA_1, motname, MOTION_CLAMP, frameleng, INTERPOLATION_SLERP, 0 );
	if( motid < 0 ){
		DbgOut( "mkmmot : SetMotion : mh AddMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*motidptr = motid;

	int rotno;
	for( rotno = 0; rotno < rotnum; rotno++ ){
		CMkmRot* currot;
		currot = mkmrot + rotno;
		ret = currot->SetSerialNo( lpth, lpsh );
		_ASSERT( !ret );


		CMkmRot* curinirot;
		curinirot = inimot->mkmrot + rotno;
		ret = curinirot->SetSerialNo( lpth, lpsh );
		_ASSERT( !ret );
	}



	int keyno;
	for( keyno = 0; keyno < keynum; keyno++ ){
		int curkey;
		curkey = *( keyframe + keyno );


		ret = CreateMotionPoints( lpsh, lpmh, motid, curkey );
		if( ret ){
			DbgOut( "mkmmot : SetMotion : CreateMotionPoints error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		for( rotno = 0; rotno < rotnum; rotno++ ){
			CMkmRot* currot;
			currot = mkmrot + rotno;

			CQuaternion invq, accuminvq, accumq;
			ret = inimot->GetInitInvQ( lpth, lpsh, rotno, &invq, &accumq, &accuminvq );
			if( ret == 0 ){
				ret = currot->SetMotion( lpth, lpsh, lpmh, motid, curkey, &invq, &accumq, &accuminvq );
				if( ret ){
					DbgOut( "mkmmot : SetMotion : mkmrot SetMotion error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				_ASSERT( 0 );
			}
		}

		int trano;
		for( trano = 0; trano < tranum; trano++ ){
			CMkmTra* curtra;
			curtra = mkmtra + trano;

			ret = curtra->SetSerialNo( lpth, lpsh );
			_ASSERT( !ret );

			ret = curtra->SetMotion( lpth, lpsh, lpmh, motid, curkey, tramult );
			if( ret ){
				DbgOut( "mkmmot : SetMotion : mkmtra SetMotion error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int CMkmMot::CreateMotionPoints( CShdHandler* lpsh, CMotHandler* lpmh, int motcookie, int frameno )
{

	int ret;
	int elemno, framenum;
	CMotionCtrl* mcptr;
	CMotionPoint2* curmp;
	CShdElem* selem;

	DWORD dispswitch = 0xFFFFFFFF;
	int interp = INTERPOLATION_SLERP;

	CQuaternion initq;
	initq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
		mcptr = (*lpmh)( elemno );
		if( !mcptr )
			break;

		selem = (*lpsh)( elemno );
		_ASSERT( selem );

		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH)){

			CQuaternion axisq;
			ret = selem->GetBoneAxisQ( &axisq );
			if( ret ){
				DbgOut( "mkmmot : CreateMotionPoints : se GetBoneAxisQ error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			curmp = mcptr->IsExistMotionPoint( motcookie, frameno );
			if( !curmp ){
				CMotionPoint2* newmp = mcptr->AddMotionPoint( motcookie, frameno,
					initq, 0.0f, 0.0f, 0.0f, dispswitch, interp, 1.0f, 1.0f, 1.0f, 0 );
				if( !newmp ){
					_ASSERT( 0 );
					DbgOut( "mkmmot : CreateMotionPoints : AddMotionPoint error" );
					return 1;
				}
							
				
				ret = mcptr->PutQuaternionInOrder( motcookie );
				if( ret ){
					_ASSERT( 0 );
					DbgOut( "mkmmot : CreateMotionPoints : PutQuaternionInOrder error" );
					return 1;
				}
				

				//前後のモーションポイントを補完した値を代入する。
				CMotionPoint2* prevmp;
				CMotionPoint2* nextmp;
				prevmp = newmp->prev;
				nextmp = newmp->next;

				if( prevmp ){
					if( nextmp ){
						framenum = nextmp->m_frameno - prevmp->m_frameno + 1;

						ret = newmp->FillUpMotionPoint( &axisq, prevmp, nextmp, framenum, frameno - prevmp->m_frameno, prevmp->interp );
						if( ret ){
							_ASSERT( 0 );
							return 1;
						}
					}else{
						ret = newmp->SetParams( frameno, prevmp->m_q, prevmp->m_mvx, prevmp->m_mvy, prevmp->m_mvz, 
							dispswitch, interp, 
							prevmp->m_scalex, prevmp->m_scaley, prevmp->m_scalez, prevmp->m_userint1 );
						_ASSERT( !ret );
					}
				}
			}		
		}

	}

		
	return 0;
}

int CMkmMot::GetInitInvQ( CTreeHandler2* lpth, CShdHandler* lpsh, int rotno, CQuaternion* invq, CQuaternion* accumq, CQuaternion* accuminvq )
{
	int ret;
	CMkmRot* currot;
	currot = mkmrot + rotno;

	currot->qarray->q.inv( invq );

	int seri;
	ret = lpth->GetBoneNoByName( currot->attach, &seri, lpsh, 0 );
	if( ret || (seri <= 0) ){
		DbgOut( "mkmmot : GetInitInvQ : th GetBoneNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*lpsh)( seri );
	_ASSERT( selem );

	CShdElem* parelem;
	parelem = lpsh->FindUpperJoint( selem );

	if( !parelem ){
		accumq->SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
		accuminvq->SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
	}else{


		CQuaternion acq( 1.0f, 0.0f, 0.0f, 0.0f );

		while( parelem ){
			int parseri;
			parseri = parelem->serialno;
			CMkmRot* findrot;
			FindRot( parseri, &findrot );
			if( findrot ){
				acq = findrot->qarray->q * acq;
			}

			parelem = lpsh->FindUpperJoint( parelem );
		}

		*accumq = acq;
		acq.inv( accuminvq );
	}

	return 0;
}

int CMkmMot::FindRot( int srcseri, CMkmRot** pprot )
{
	int rotno;
	*pprot = 0;
	for( rotno = 0; rotno < rotnum; rotno++ ){
		CMkmRot* currot = mkmrot + rotno;
		if( currot->serialno == srcseri ){
			*pprot = currot;
			break;
		}
	}
	return 0;
}

