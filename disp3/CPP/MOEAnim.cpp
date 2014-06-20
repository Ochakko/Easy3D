#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <MOEAnim.h>

#include <shdhandler.h>
#include <shdelem.h>
#include <mothandler.h>
#include <MCHandler.h>

#include <MOEKey.h>
#include <MState.h>

#include <DSKey.h>
#include <DispSwitch.h>
#include <TexKey.h>
#include <AlpKey.h>
#include <mqomaterial.h>

#include <MOEEHelper.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

CMOEAnim::CMOEAnim( char* srcname, int srcmotkind )
{
	InitParams();
	if( srcname && *srcname ){
		strcpy_s( m_animname, 256, srcname );
	}else{
		ZeroMemory( m_animname, sizeof( char ) * 256 );
	}

	m_motkind = srcmotkind;
}

CMOEAnim::~CMOEAnim()
{
	DestroyObjs();
}

int CMOEAnim::InitParams()
{
	ZeroMemory( m_animname, sizeof( char ) * 256 );
	m_frameno = 0;
	m_curms = 0;

	m_isfirst = 1;
	m_maxframe = 0;
	m_animtype = MOTION_CLAMP;
	m_animstep = 1;
	m_stepdir = 1;
	m_motjump = 0;

	ZeroMemory( &m_nextmot, sizeof( NEXTMOTION ) );
	m_nextmot.mk = -1;
	m_nextmot.befframeno = -1;
	m_nextmot.aftframeno = -1;

	m_firstkey = 0;


	m_framedata = 0;

	m_bonenum = 0;
	m_boneno2seri = 0;
	m_seri2boneno = 0;

	m_mlleng = 0;
	m_mllist = 0;

	ZeroMemory( &m_defmoee, sizeof( MOEELEM ) );
	m_defmoee.eventno = 0;
	m_defmoee.listnum = 1;
	m_defmoee.list[0] = -1;
	m_defmoee.notlistnum = 0;

	m_dsfillupmode = 0;
	m_texfillupmode = 0;

	m_framedataleng = 0;

	InitStepFunc();
	return 0;
}

void CMOEAnim::InitStepFunc()
{
	int i;
	for( i = 0; i < MOTIONTYPEMAX; i++ ){

		switch( i ){
		case MOTION_STOP:
			StepFunc[i] = &CMOEAnim::StepStop;
			break;
		case MOTION_CLAMP:
			StepFunc[i] = &CMOEAnim::StepClamp;
			break;
		case MOTION_ROUND:
			StepFunc[i] = &CMOEAnim::StepRound;
			break;
		case MOTION_INV:
			StepFunc[i] = &CMOEAnim::StepInv;
			break;
		case MOTION_JUMP:
			StepFunc[i] = &CMOEAnim::StepJump;
			break;
		default:
			StepFunc[i] = &CMOEAnim::StepDummy;
			break;
		}

	}
}

int CMOEAnim::DestroyObjs()
{
/*
	CMOEKey* m_firstkey;
	CMState* m_framedata;//maxframe + 1フレーム分のCMStateの配列

	int m_bonenum;
	int* m_boneno2seri;
	int* m_seri2boneno;
*/

	DestroyAllKeys();

	DestroyFrameData();
	DestroyBoneData();

	if( m_mllist ){
		free( m_mllist );
		m_mllist = 0;
	}
	m_mlleng = 0;


	InitParams();

	return 0;
}

int CMOEAnim::DestroyAllKeys()
{
	CMOEKey* delmoek = m_firstkey;
	CMOEKey* nextmoek = 0;
	while( delmoek ){
		nextmoek = delmoek->next;
		delete delmoek;
		delmoek = nextmoek;
	}
	m_firstkey = 0;

	return 0;
}

int CMOEAnim::ExistMOEKey( int frameno, CMOEKey** ppmoek )
{
	*ppmoek = 0;

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "MOEAnim : ExistMOEKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//////////
	CMOEKey* curmoek = m_firstkey;
	while( curmoek && (curmoek->m_frameno <= frameno) ){
		if( curmoek->m_frameno == frameno ){
			*ppmoek = curmoek;
			break;
		}

		curmoek = curmoek->next;
	}

	return 0;
}

int CMOEAnim::ExistBefMOEKey( int frameno, CMOEKey** ppmoek )
{
	*ppmoek = 0;

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "MOEAnim : ExistBefMOEKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//////////
	CMOEKey* curmoek = m_firstkey;
	while( curmoek && (curmoek->m_frameno <= frameno) ){
		*ppmoek = curmoek;

		curmoek = curmoek->next;
	}

	return 0;
}


int CMOEAnim::CreateMOEKey( int frameno, CMOEKey** ppmoek )
{
	*ppmoek = 0;

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "MOEAnim : CreateMOEKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////////
	CMOEKey* newmoek = 0;
	newmoek = new CMOEKey();
	if( !newmoek ){
		DbgOut( "MOEAnim : CreateMOEKey : newmoek alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newmoek->m_frameno = frameno;


//////////////////
	int ret;
	CMOEKey* moekhead = m_firstkey;
	CMOEKey* curmoek = moekhead;
	CMOEKey* largermoek = 0;
	CMOEKey* smallermoek = 0;
	int cmp;
	while( curmoek ){
		cmp = newmoek->FramenoCmp( curmoek );
		if( cmp < 0 ){
			largermoek = curmoek;
			break;
		}else{
			smallermoek = curmoek;
		}
		curmoek = curmoek->next;
	}

	if( !moekhead ){
		//先頭に追加
		m_firstkey = newmoek;
	}else{
		if( largermoek ){
			//largermoekの前に追加。
			ret = largermoek->AddToPrev( newmoek );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( !newmoek->prev )
				m_firstkey = newmoek;
		}else{
			//最後に追加。(smallermoek の後に追加)
			_ASSERT( smallermoek );
			ret = smallermoek->AddToNext( newmoek );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}

	*ppmoek = newmoek;

	return 0;
}
CMOEKey* CMOEAnim::GetFirstMOEKey()
{
	return m_firstkey;
}
CMOEKey* CMOEAnim::LeaveFromChainMOEKey( int frameno )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "MOEAnim : LeaveFromChainMOEKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	int ret;
	CMOEKey* curmoek = 0;
	ret = ExistMOEKey( frameno, &curmoek );
	if( ret ){
		return 0;
	}

	if( curmoek ){
		CMOEKey* firstmoek;
		firstmoek = m_firstkey;
		if( firstmoek == curmoek ){
			m_firstkey = curmoek->next;
		}

		curmoek->LeaveFromChain();
	}

	return curmoek;
}
int CMOEAnim::ChainMOEKey( CMOEKey* addmoek )
{
	if( (addmoek->m_frameno < 0) || (addmoek->m_frameno > m_maxframe) ){
		DbgOut( "MOEAnim : ChainMOEKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEKey* moekhead = m_firstkey;
	CMOEKey* curmoek = moekhead;
	CMOEKey* largermoek = 0;
	CMOEKey* smallermoek = 0;
	int cmp;
	while( curmoek ){
		cmp = addmoek->FramenoCmp( curmoek );
		if( cmp < 0 ){
			largermoek = curmoek;
			break;
		}else{
			smallermoek = curmoek;
		}
		curmoek = curmoek->next;
	}

	if( !moekhead ){
		//先頭に追加
		m_firstkey = addmoek;
	}else{
		if( largermoek ){
			//largermoekの前に追加。
			ret = largermoek->AddToPrev( addmoek );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			if( !addmoek->prev )
				m_firstkey = addmoek;
		}else{
			//最後に追加。(smallermoek の後に追加)
			_ASSERT( smallermoek );
			ret = smallermoek->AddToNext( addmoek );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	
	return 0;
}
int CMOEAnim::DeleteMOEKeyOutOfRange( int srcmaxframe )
{
	int ret;
	CMOEKey* moekptr = m_firstkey;
	CMOEKey* nextmoek = 0;
	while( moekptr ){
		nextmoek = moekptr->next;
		int frameno = moekptr->m_frameno;
		if( frameno > srcmaxframe ){
			ret = DeleteMOEKey( frameno );
			if( ret ){
				DbgOut( "MOEAnim : DeleteMOEKeyOutOfRange : DeleteMOEKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		moekptr = nextmoek;
	}

	return 0;
}
int CMOEAnim::SetMOEAnim()
{
	int ret;
	ret = GetMOEAnim( &m_curms, m_frameno );
	if( ret ){
		DbgOut( "MOEanim : SetMOEAnim : GetMOEAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMOEAnim::SetKeyIdlingName( int frameno, char* idlingname )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "MOEAnim : SetKeyIdlingName : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	CMOEKey* pmoek = 0;
	ret = ExistMOEKey( frameno, &pmoek );
	if( ret ){
		DbgOut( "MOEAnim : SetKeyIdlingName : ExistMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !pmoek ){
		ret = CreateMOEKey( frameno, &pmoek );
		if( ret || !pmoek ){
			DbgOut( "MOEAnim : SetKeyIdlingName : CreateMOEKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	_ASSERT( pmoek );

	ret = pmoek->SetIdlingName( idlingname );
	if( ret ){
		DbgOut( "MOEAnim : SetKeyIdlingName : moek SetIdlingName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CMOEAnim::SetMOEKey( int frameno, CMOEKey* srcmoek )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "MOEAnim : SetMOEKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	CMOEKey* pmoek = 0;
	ret = ExistMOEKey( frameno, &pmoek );
	if( ret ){
		DbgOut( "MOEAnim : SetMOEKey : ExistMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !pmoek ){
		ret = CreateMOEKey( frameno, &pmoek );
		if( ret || !pmoek ){
			DbgOut( "MOEAnim : SetMOEKey : CreateMOEKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	_ASSERT( pmoek );

	ret = pmoek->CopyMOEElem( srcmoek );
	if( ret ){
		DbgOut( "MOEAnim : SetMOEKey : moek CopyMOEElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	pmoek->m_goonflag = srcmoek->m_goonflag;

	return 0;
}


int CMOEAnim::DeleteMOEKey( int frameno )
{
	if( frameno < 0 ){
		DestroyAllKeys();
	}else{
		CMOEKey* delmoek = 0;
		delmoek = LeaveFromChainMOEKey( frameno );
		if( delmoek ){
			delete delmoek;
		}
	}
	return 0;
}

int CMOEAnim::AddMOEE( int frameno, MOEELEM* srcmoee )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "MOEAnim : AddMOEE : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	CMOEKey* pmoek = 0;
	ret = ExistMOEKey( frameno, &pmoek );
	if( ret ){
		DbgOut( "MOEAnim : AddMOEE : ExistMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !pmoek ){
		ret = CreateMOEKey( frameno, &pmoek );
		if( ret || !pmoek ){
			DbgOut( "MOEAnim : AddMOEE : CreateMOEKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	_ASSERT( pmoek );

	ret = pmoek->AddMOEE( srcmoee );
	if( ret ){
		DbgOut( "MOEAnim : AddMOEE : moek AddMOEE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CMOEAnim::DeleteMOEE( int frameno, int delindex )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "MOEAnim : DeleteMOEE : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	CMOEKey* pmoek = 0;
	ret = ExistMOEKey( frameno, &pmoek );
	if( ret ){
		DbgOut( "MOEAnim : DeleteMOEE : ExistMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( !pmoek ){
		return 0;
	}

	ret = pmoek->DeleteMOEE( delindex );
	if( ret ){
		DbgOut( "MOEAnim : DeleteMOEE : moek DeleteMOEE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//	if( pmoek->m_moeenum <= 0 ){
//		ret = DeleteMOEKey( frameno );
//		if( ret ){
//			DbgOut( "MOEAnim : DeleteMOEE : DeleteMOEKey error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//	}

	return 0;
}

int CMOEAnim::CalcFrameData( CShdHandler* lpsh, CMotHandler* lpmh, CMCHandler* lpmch )
{
	int ret;

	int bonenum = 0;
	ret = SetBoneno2Seri( lpsh, 0, 0, 0, &bonenum );//必要な配列長を取得
	if( ret ){
		DbgOut( "moeanim : CalcFrameData : SetBoneno2Seri 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = CreateFrameData( lpsh );
	if( ret ){
		DbgOut( "moeanim : CalcFrameData : CreateFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameno;

	if( (bonenum <= 0) || !lpsh ){
		for( frameno = 0; frameno <= m_maxframe; frameno++ ){
			( m_framedata + frameno )->InitMatrix();
		}
		return 0;//!!!!!!!
	}

	//初期姿勢
	int idling0;
	idling0 = lpmch->GetIdlingMotID();
	if( idling0 >= 0 ){
		ret = lpmh->SetMotionFrameNo( lpsh, idling0, 0, 1 );
		if( ret ){
			DbgOut( "moeanim : CalcFrameData : mh SetNewPoseML error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	CMOEKey* findkey = 0;
	CMOEKey* findbefkey = 0;
	for( frameno = 0; frameno <= m_maxframe; frameno++ ){
		CMState* dstms = m_framedata + frameno;


		dstms->m_keyflag = 0;
		findkey = 0;
		findbefkey = 0;
		ExistMOEKey( frameno, &findkey );
		if( findkey ){
			dstms->m_keyflag = 1;
		}else{
			ExistBefMOEKey( frameno, &findbefkey );
			if( findbefkey && (findbefkey->m_goonflag == 1) ){
				findkey = findbefkey;
			}
		}

		if( findkey ){
			int eno;
			int idlingkey;
			idlingkey = lpmh->GetMotionIDByName( findkey->m_idlingname );
			if( idlingkey < 0 ){
				_ASSERT( 0 );
				idlingkey = 0;
			}
			int idlingmoa;
			idlingmoa = lpmch->GetIdlingMotID();
			if( idlingmoa < 0 ){
				//DbgOut( "moeanim : CalcFrameData : idlingid 0 error !!!\n" );
				_ASSERT( 0 );
				//return 1;
				idlingmoa = 0;
			}
			if( idlingkey != idlingmoa ){
				ret = lpmch->ChangeIdlingMotion( lpsh, idlingkey );
				if( ret ){
					DbgOut( "moeanim : CalcFrameData : mch ChangeIdlingMotion error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = lpmh->SetNewPoseML( lpsh );
				if( ret ){
					DbgOut( "moeanim : CalcFrameData : mh SetNewPoseML error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			for( eno = 0; eno < findkey->m_moeenum; eno++ ){
				MOEELEM* curmoee = findkey->m_pmoee + eno;
				_ASSERT( curmoee );

				ret = SetNewPoseByMOAML( curmoee, lpsh, lpmh, lpmch );
				if( ret ){
					DbgOut( "moeanim : CalcFrameData : SetNewPoseByMOAML 0 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}else{
			ret = SetNewPoseByMOAML( &m_defmoee, lpsh, lpmh, lpmch );
			if( ret ){
				DbgOut( "moeanim : CalcFrameData : SetNewPoseByMOAML 1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

		ret = lpmh->SetNewPoseML( lpsh );
		if( ret ){
			DbgOut( "moeanim : CalcFrameData : mh SetNewPoseML error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = lpmh->SetMState( lpsh, dstms, m_bonenum, m_boneno2seri );
		if( ret ){
			DbgOut( "moeanim : CalcFrameData : mh SetMState error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		int bno;
		for( bno = 0; bno < m_bonenum; bno++ ){
			int bid = *( m_boneno2seri + bno );
			int curmotid, curframeno;
			int nextmotid, nextframeno;

			ret = lpmh->GetMotionFrameNoML( bid, &curmotid, &curframeno );
			_ASSERT( !ret );
			ret = lpmh->GetNextMotionFrameNoML( bid, curmotid, &nextmotid, &nextframeno );
			_ASSERT( !ret );

			ret = dstms->SetMSLocalInf( bno, curmotid, curframeno, nextmotid, nextframeno );
			_ASSERT( !ret );
		}
	}

	return 0;
}
int CMOEAnim::GetMOEAnim( CMState** ppms, int srcframeno )
{
	if( (srcframeno < 0) || (srcframeno > m_maxframe) ){
		DbgOut( "MOEAnim : GetMOEAnim : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*ppms = (m_framedata + srcframeno);

	return 0;
}



int CMOEAnim::SetMOEAnimNo( int frameno, int srcisfirst )
{
	int ret;
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "MOEAnim : SetMOEAnimNo : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_frameno = frameno;
	m_isfirst = srcisfirst;

	ret = GetMOEAnim( &m_curms, m_frameno );
	if( ret ){
		DbgOut( "MOEAnim : SetMOEAnimNo : GetMOEAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

int CMOEAnim::GetNextMOE( NEXTMOTION* pnm, int getonly )
{
	if( m_motkind < 0 ){
		pnm->mk = -1;
		pnm->aftframeno = 0;
		return 0;
	}

	int ret;

	if( m_isfirst == 0 ){
		ret = (this->*StepFunc[m_animtype])( pnm );
		_ASSERT( !ret );
	}else{
		if( getonly == 0 ){
			m_isfirst = 0;
		}
		pnm->mk = m_motkind;
		pnm->aftframeno = m_frameno;
	}
	
	return 0;
}

int CMOEAnim::StepStop( NEXTMOTION* pnm )
{
	pnm->mk = m_motkind;
	pnm->aftframeno = m_frameno;
	return 0;
}
int CMOEAnim::StepClamp( NEXTMOTION* pnm )
{
	int nextno;	
	nextno = m_frameno + m_animstep;

	if( nextno > m_maxframe ){
		if( m_nextmot.mk == -1 ){
			pnm->mk = m_motkind;
			pnm->aftframeno = m_maxframe;
		}else{
			*pnm = m_nextmot;
		}
	}else if( nextno < 0 ){
		pnm->mk = m_motkind;
		pnm->aftframeno = 0;
	}else{
		if( (m_nextmot.mk != -1) && (m_nextmot.befframeno != -1) && (nextno > m_nextmot.befframeno) ){
			*pnm = m_nextmot;
		}else{
			pnm->mk = m_motkind;
			pnm->aftframeno = nextno;
		}
	}

	return 0;
}
int CMOEAnim::StepRound( NEXTMOTION* pnm )
{
	int nextno;
	nextno = m_frameno + m_animstep;

	if( nextno > m_maxframe ){
		if( m_nextmot.mk == -1 ){
			pnm->mk = m_motkind;
			pnm->aftframeno = 0;
		}else{
			*pnm = m_nextmot;
		}
	}else if( nextno < 0 ){
		pnm->mk = m_motkind;
		pnm->aftframeno = m_maxframe;
	}else{
		if( (m_nextmot.mk != -1) && (m_nextmot.befframeno != -1) && (nextno > m_nextmot.befframeno) ){
			*pnm = m_nextmot;
		}else{
			pnm->mk = m_motkind;
			pnm->aftframeno = nextno;
		}
	}
	
	return 0;
}
int CMOEAnim::StepInv( NEXTMOTION* pnm )
{
	int nextno;
		nextno = m_frameno + m_stepdir * m_animstep;

	if( nextno > m_maxframe ){
		if( m_nextmot.mk == -1 ){
			pnm->mk = m_motkind;
			pnm->aftframeno = m_maxframe;
		}else{
			*pnm = m_nextmot;
		}
		m_stepdir *= -1;
	}else if( nextno < 0 ){
		pnm->mk = m_motkind;
		pnm->aftframeno = 0;
		m_stepdir *= -1;
	}else{
		if( (m_nextmot.mk != -1) && (m_nextmot.befframeno != -1) && (nextno > m_nextmot.befframeno) ){
			*pnm = m_nextmot;
		}else{
			pnm->mk = m_motkind;
			pnm->aftframeno = nextno;
		}
	}

	return 0;
}
int CMOEAnim::StepJump( NEXTMOTION* pnm )
{
	int nextno;
	nextno = m_frameno + m_animstep;

	if( nextno > m_maxframe ){
		if( m_nextmot.mk == -1 ){
			pnm->mk = m_motkind;
			pnm->aftframeno = m_motjump;
		}else{
			*pnm = m_nextmot;
		}
	}else if( nextno < 0 ){
		pnm->mk = m_motkind;
		pnm->aftframeno = m_maxframe;
	}else{
		if( (m_nextmot.mk != -1) && (m_nextmot.befframeno != -1) && (nextno > m_nextmot.befframeno) ){
			*pnm = m_nextmot;
		}else{
			pnm->mk = m_motkind;
			pnm->aftframeno = nextno;
		}
	}
	
	return 0;
}

int CMOEAnim::StepDummy( NEXTMOTION* pnm )	
{
	DbgOut( "CMOEAnim : StepFunc called StepDummy warning !!!\n" );
	pnm->mk = m_motkind;
	pnm->aftframeno = m_frameno;
	return 0;
}


int CMOEAnim::GetMOEKeyframeNoRange( int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	*framenumptr = 0;

	int setno = 0;
	CMOEKey* curmoek = m_firstkey;
	while( curmoek ){
		if( (curmoek->m_frameno >= srcstart) && (curmoek->m_frameno <= srcend) ){
			if( framearray && (arrayleng > 0) ){
				if( setno >= arrayleng ){
					DbgOut( "MOEAnim : GetMOEKeyframeNoRange : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				*( framearray + setno ) = curmoek->m_frameno;
			}
			setno++;
		}
		curmoek = curmoek->next;
	}

	*framenumptr = setno;

	return 0;
}

int CMOEAnim::SetAnimName( char* srcname )
{
	if( srcname && *srcname ){
		strcpy_s( m_animname, 256, srcname );
	}else{
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CMOEAnim::GetAnimName( char* dstname, int arrayleng )
{
	if( !dstname || (arrayleng <= 0) ){
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( dstname, arrayleng, m_animname );
	return 0;
}

int CMOEAnim::StepMOE( int frameno )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "MOEAnim : StepMOE : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_frameno = frameno;

	int ret;
	ret = GetMOEAnim( &m_curms, m_frameno );
	if( ret ){
		DbgOut( "MOEAnim : StepMOE : GetMOEAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CMOEAnim::CreateFrameData( CShdHandler* lpsh )
{	
//	if( m_framedataleng != (m_maxframe + 1) ){
		DestroyFrameData();

		m_framedataleng = m_maxframe + 1;

		m_framedata = new CMState[ m_framedataleng ];
		if( !m_framedata ){
			DbgOut( "MOEAnim : CreateFrameData : framedata alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
//	}

	//boneno2seri, seri2boneno
	int ret;
	int bonenum = 0;
	ret = SetBoneno2Seri( lpsh, 0, 0, 0, &bonenum );//必要な配列長を取得
	if( ret ){
		DbgOut( "moeanim : CreateFrameData : SetBoneno2Seri 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( bonenum != m_bonenum ){
		DestroyBoneData();

		if( bonenum > 0 ){
			m_boneno2seri = (int*)malloc( sizeof( int ) * bonenum );
			if( !m_boneno2seri ){
				DbgOut( "moeanim : CreateFrameData : boneno2seri alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( m_boneno2seri, sizeof( int ) * bonenum );

			m_seri2boneno = (int*)malloc( sizeof( int ) * lpsh->s2shd_leng );
			if( !m_seri2boneno ){
				DbgOut( "moeanim : CreateFrameData : seri2boneno alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( m_seri2boneno, sizeof( int ) * lpsh->s2shd_leng );

			int getnum = 0;
			ret = SetBoneno2Seri( lpsh, m_boneno2seri, m_seri2boneno, bonenum, &getnum );
			if( ret || (getnum != bonenum ) ){
				DbgOut( "moeanim : CreateFrameData : SetBoneno2Seri 1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			m_bonenum = bonenum;

		}
	}

	ret = lpsh->CreateMorphTable();
	if( ret ){
		DbgOut( "moeanim : CreateFrameData : sh CreateMorphTable error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int frameno;
	for( frameno = 0; frameno < m_framedataleng; frameno++ ){
		ret = ( m_framedata + frameno )->CreateMatrix( lpsh, m_bonenum );
		if( ret ){
			DbgOut( "moeanim : CreateFrameData : ms CreateMatrix error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CMOEAnim::SetBoneno2Seri( CShdHandler* lpsh, int* boneno2seri, int* seri2boneno, int srcleng, int* getnumptr )
{
	*getnumptr = 0;

	if( !lpsh ){
		return 0;
	}

	int setno = 0;
	int seri;
	for( seri = 0; seri < lpsh->s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*lpsh)( seri );
		_ASSERT( selem );
		if( selem->IsJoint() ){
			if( boneno2seri ){
				if( setno >= srcleng ){
					DbgOut( "moeanim : SetBoneno2Seri : srcleng short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				*( boneno2seri + setno ) = seri;
				*( seri2boneno + seri ) = setno;
			}

			setno++;

		}else{
			if( seri2boneno ){
				*( seri2boneno + seri ) = -1;
			}
		}
	}

	if( boneno2seri ){
		if( setno != srcleng ){
			DbgOut( "moeanim : SetBoneno2Seri : setno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	*getnumptr = setno;

	return 0;
}

int CMOEAnim::DestroyFrameData()
{
	if( m_framedata ){
		delete [] m_framedata;
		m_framedata = 0;
	}
	m_framedataleng = 0;

	return 0;
}

int CMOEAnim::DestroyBoneData()
{
	if( m_boneno2seri ){
		free( m_boneno2seri );
		m_boneno2seri = 0;
	}
	if( m_seri2boneno ){
		free( m_seri2boneno );
		m_seri2boneno = 0;
	}
	m_bonenum = 0;

	return 0;
}



int CMOEAnim::SetNewPoseByMOAML( MOEELEM* pmoee, CShdHandler* shandler, CMotHandler* mhandler, CMCHandler* mch )
{

	int ret;
	int eventno = pmoee->eventno;
	ret = SetMLList( shandler, pmoee->listnum, pmoee->list, pmoee->notlistnum, pmoee->notlist );
	if( ret ){
		DbgOut( "moeanim : SetNewPoseByMOAML : SetMLList error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int idlingid;
	int idlingleng;
	int idlingtype;
	idlingid = mch->GetIdlingMotID();
	if( idlingid < 0 ){
		//DbgOut( "moeanim : SetNewPoseByMOAML : idlingid 1 error !!!\n" );
		_ASSERT( 0 );
		//return 1;
		idlingid = 0;
	}
	ret = mhandler->GetMotionFrameLength( idlingid, &idlingleng );
	if( ret ){
		DbgOut( "moeanim : SetNewPoseByMOAML : mh GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = mhandler->GetMotionType( idlingid, &idlingtype );
	if( ret ){
		DbgOut( "moeanim : SetNewPoseByMOAML : GetMotionType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int firstid;
	int firstframe;
	int syncroframe;
	int nextflag = 1;
	ret = mhandler->GetFirstTickJoint( nextflag, idlingid, &firstid, &firstframe );
	if( ret ){
		DbgOut( "moeanim : SetNewPoseByMOAML : mh GetFirstTickJoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( firstid > 0 ){
		syncroframe = firstframe + mch->m_fillupleng - 1;
		if( syncroframe >= idlingleng ){
			switch( idlingtype ){
			case MOTION_CLAMP:
				syncroframe = idlingleng - 1;
				break;
			case MOTION_ROUND:
				while( syncroframe >= idlingleng ){
					syncroframe -= idlingleng;
				}
				break;
			case MOTION_INV:
				syncroframe = idlingleng - 1 - (syncroframe - idlingleng);
				syncroframe = max( 0, syncroframe );
				syncroframe = min( idlingleng, syncroframe );
				break;
			default:
				syncroframe = idlingleng - 1;
				break;
			}
		}
	}else{
		syncroframe = 0;
	}


	DWORD curtick;
	curtick = GetTickCount();


	int seri;
	for( seri = 0; seri < shandler->s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*shandler)( seri );
		_ASSERT( selem );

		if( *( m_mllist + seri ) == 1 ){

			int curmotid = -1;
			int curframe = 0;
			int curframeleng = 0;

			ret = mhandler->GetMotionFrameNoML( seri, &curmotid, &curframe );
			if( ret || (curmotid < 0) ){
				DbgOut( "moeanim : SetNewPoseByMOAML : mh GetMotionFrameNoML error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = mhandler->GetMotionFrameLength( curmotid, &curframeleng );
			if( ret ){
				DbgOut( "moeanim : SetNewPoseByMOAML : mh GetMotionFrameLength error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int curmottype = -1;
			ret = mhandler->GetMotionType( curmotid, &curmottype );
			if( ret ){
				DbgOut( "moeanim : SetNewPoseByMOAML : GetMotionType error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


		//DbgOut( "check !!! : moeanim : SetNewPoseByMOA : curmotid %d, curframe %d, mhandler->m_fuid %d\r\n",
		//	curmotid, curframe, mhandler->m_fuid );

			int checkfillup = 0;


			int nextmotid = -1;
			int nextframe = 0;
			int nextframeleng = 0;

			int befmotid = -1;
			int befframe = 0;

			int fillupflag = 0;
			int notfu = 0;
			int ev0idle = 0;
			int nottoidle = 0;

			if( curmotid != mhandler->m_fuid ){
				int tmpnottoidle = 0;
				ret = mch->GetNextMotion( curmotid, curframe, eventno, &nextmotid, &nextframe, &notfu, &tmpnottoidle );
				if( ret ){
					DbgOut( "moeanim : SetNewPoseByMOAML : mch GetNextMotion error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( tmpnottoidle < 0 ){
					mhandler->GetNotToIdle( seri, &nottoidle );
				}else{
					nottoidle = tmpnottoidle;
					mhandler->SetNotToIdle( seri, nottoidle );
				}


				ret = mch->GetEv0Idle( curmotid, &ev0idle );
				if( ret ){
					DbgOut( "moeanim : SetNewPoseByMOAML : mch GetEv0Idle 0 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				
				if( (eventno == 0) && (ev0idle != 0) ){
					nextmotid = idlingid;
					nextframe = 0;
					notfu = 0;
				}else{
					if( curmotid == nextmotid ){
						//最終フレームだった場合は、アイドリングへ
						if( (curmottype == MOTION_CLAMP) && (curframe >= (curframeleng - 1)) && (nottoidle == 0) ){
							nextmotid = idlingid;
							nextframe = 0;
							notfu = 0;
						}
					}
				}

		//DbgOut( "check !!! : moeanim : SetNewPoseByMOA : curmotid != mhandler->m_fuid, nextmotid %d, nextframe %d\r\n",
		//	   nextmotid, nextframe );

			}else{
				//補間モーション中だった場合。
				checkfillup = 1;

				ret = mhandler->GetNextMotionFrameNoML( seri, mhandler->m_fuid, &befmotid, &befframe );
				if( ret || (befmotid < 0) ){
					DbgOut( "moeanim : SetNewPoseByMOAML : mh GetNextMotionFrameNo error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				int tmpframe;
				tmpframe = max( 0, (befframe - mch->m_fillupleng) );
				int tmpnottoidle = 0;
				ret = mch->GetNextMotion( befmotid, tmpframe + curframe, eventno, &nextmotid, &nextframe, &notfu, &tmpnottoidle );
				//ret = mch->GetNextMotion( befmotid, befframe + curframe, eventno, &nextmotid, &nextframe, &notfu );
				if( ret ){
					DbgOut( "moeanim : SetNewPoseByMOAML : mch GetNextMotion error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
		//DbgOut( "check !!! : moeanim : SetNewPoseByMOA : curmotid == mhandler->m_fuid, befmotid %d, befframe %d, nextmotid %d, nextframe %d\r\n",
		//	   befmotid, befframe, nextmotid, nextframe );

				if( tmpnottoidle < 0 ){
					mhandler->GetNotToIdle( seri, &nottoidle );
				}else{
					nottoidle = tmpnottoidle;
					mhandler->SetNotToIdle( seri, nottoidle );
				}
				
				ret = mch->GetEv0Idle( befmotid, &ev0idle );
				if( ret ){
					DbgOut( "moeanim : SetNewPoseByMOAML : mch GetEv0Idle 2 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				
				if( (eventno == 0) && (ev0idle != 0) ){
					nextmotid = idlingid;
					nextframe = 0;
					notfu = 0;
				}
				
				if( befmotid != nextmotid ){
					fillupflag = 1;
				}

			}


			if( ((checkfillup == 0) && (curmotid != nextmotid)) || fillupflag ){

//DbgOut( "moeanim : SetNewPoseByMOAML : fillup calc !!!\n" );

				if( notfu == 0 ){

					ret = mhandler->GetMotionFrameLength( nextmotid, &nextframeleng );
					if( ret ){
						DbgOut( "moeanim : SetNewPoseByMOAML : mh GetMotionFrameLength error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					int filluppoint;
					if( nextmotid != idlingid ){
						filluppoint = min( (nextframeleng - 1), (nextframe + mch->m_fillupleng - 1) );
					}else{
						filluppoint = syncroframe;
					}
					ret = mhandler->CopyMotionFrameML( shandler, seri, curmotid, curframe, mhandler->m_fuid, 0 );
					if( ret ){
						DbgOut( "moeanim : SetNewPoseByMOAML : mh CopyMotionFrameML fu0 error %d, %d!!!\n", curmotid, curframe );
						_ASSERT( 0 );
						return 1;
					}
					ret = mhandler->CopyMotionFrameML( shandler, seri, nextmotid, filluppoint, mhandler->m_fuid, mch->m_fillupleng - 1 );
					if( ret ){
						DbgOut( "moeanim : SetNewPoseByMOAML : mh CopyMotionFrameML fu last error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					//morph anim
					ret = mhandler->CopyMMotAnimFrame( seri, shandler, curmotid, curframe, mhandler->m_fuid, 0 );
					if( ret ){
						DbgOut( "moeanim : SetNewPoseByMOAML : mh CopyMMotAnimFrame 0 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					ret = mhandler->CopyMMotAnimFrame( seri, shandler, nextmotid, filluppoint, mhandler->m_fuid, mch->m_fillupleng - 1 );
					if( ret ){
						DbgOut( "moeanim : SetNewPoseByMOAML : mh CopyMMotAnimFrame 0 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}



					//dskey, texkey, alpの設定
					CShdElem* selem;
					selem = (*shandler)( seri );
					_ASSERT( selem );
					if( selem->IsJoint() && selem->type != SHDMORPH ){

						ret = shandler->InitTempDS();
						_ASSERT( !ret );

						ret = mhandler->InitTexAnim( shandler );
						_ASSERT( !ret );

						if( m_dsfillupmode == 0 ){
							ret = mhandler->GetDSAnim( shandler->m_tempds, seri, curmotid, curframe );
							_ASSERT( !ret );
						}else{
							ret = mhandler->GetDSAnim( shandler->m_tempds, seri, nextmotid, filluppoint );
							_ASSERT( !ret );
						}

						if( m_texfillupmode == 0 ){
							ret = mhandler->GetTexAnim( seri, curmotid, curframe );
							_ASSERT( !ret );
						}else{
							ret = mhandler->GetTexAnim( seri, nextmotid, filluppoint );
							_ASSERT( !ret );
						}

						int dirtyds = 0;
						int dsno;
						for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
							if( (shandler->m_tempds + dsno)->state == 0 ){
								dirtyds++;
								break;
							}
						}

						CMQOMaterial* curmat = shandler->m_mathead;
						int dirtytex = 0;
						while( curmat ){
							if( curmat->curtexname != NULL ){
								dirtytex++;
								break;
							}
							curmat = curmat->next;
						}

						ret = mhandler->DeleteDSKey( seri, mhandler->m_fuid, 0 );
						_ASSERT( !ret );

						ret = mhandler->DeleteTexKey( seri, mhandler->m_fuid, 0 );
						_ASSERT( !ret );

						ret = mhandler->DeleteAlpKey( seri, mhandler->m_fuid, 0 );
						_ASSERT( !ret );


						if( dirtyds ){
							CDSKey* dskptr;
							ret = mhandler->CreateDSKey( seri, mhandler->m_fuid, 0, &dskptr );
							_ASSERT( !ret );

							for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
								if( (shandler->m_tempds + dsno)->state == 0 ){
									ret = dskptr->SetDSChange( shandler->m_ds + dsno, 0 );
									_ASSERT( !ret );
								}
							}
						}

						if( dirtytex ){
							CTexKey* tkptr;
							ret = mhandler->CreateTexKey( seri, mhandler->m_fuid, 0, &tkptr );
							_ASSERT( !ret );

							curmat = shandler->m_mathead;
							while( curmat ){
								if( curmat->curtexname ){
									ret = tkptr->SetTexChange( curmat, curmat->curtexname );
									_ASSERT( !ret );
								}
								curmat = curmat->next;
							}
						}


						CAlpKey* alpkptr = 0;
						ret = mhandler->CreateAlpKey( seri, mhandler->m_fuid, 0, &alpkptr );
						_ASSERT( !ret );

						curmat = shandler->m_mathead;
						while( curmat ){
							float alphaval = 1.0f;
							int existflag = 0;
							ret = mhandler->GetAlpAnim( seri, curmotid, curframe, curmat, &alphaval, &existflag );
							_ASSERT( !ret );

							if( alphaval != curmat->orgalpha ){
								ret = alpkptr->SetAlpChange( curmat, alphaval );
								_ASSERT( !ret );
							}
							curmat = curmat->next;
						}
					}

					ret = mhandler->FillUpMotionML( shandler, mhandler->m_fuid, seri, 0, mch->m_fillupleng - 1 );
					if( ret ){
						DbgOut( "moeanim : SetNewPoseByMOAML : mh FillUpMotionML fu error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					int nextfirstframe;
					nextfirstframe = min( (filluppoint + 1), (nextframeleng - 1) );

					//ret = mhandler->SetNextMotionFrameNo( shandler, mhandler->m_fuid, nextmotid, filluppoint );//// filluppointへジャンプ
					//ret = mhandler->SetNextMotionFrameNoML( shandler, mhandler->m_fuid, nextmotid, nextfirstframe, -1 );//// filluppoint + 1へジャンプ
					ret = mhandler->SetNextMotionFrameNoML( seri, mhandler->m_fuid, nextmotid, nextfirstframe, -1 );//// filluppoint + 1へジャンプ
					if( ret ){
						DbgOut( "moeanim : SetNewPoseByMOAML : mh SetNextMotionFrameNoML fu error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					ret = mhandler->SetMotionKindML( seri, mhandler->m_fuid );
					if( ret ){
						DbgOut( "moeanim : SetNewPoseByMOAML : mh SetMotionKindML fu error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					if( nextmotid == idlingid ){
						ret = mhandler->SetTickCount( seri, curtick );
						_ASSERT( !ret );
					}


			//DbgOut( "check !!! : moeanim : SetNewPoseByMOA : !!!!!!!!!! FillUp !!!!!!!!!!!!, filluppoint %d, nextframeleng %d\r\n",
			//	   filluppoint, nextframeleng );

				}else{

					// notfu != 0  direct change
					if( nextmotid != idlingid ){
						ret = mhandler->SetMotionFrameNoML( shandler->m_mathead, seri, nextmotid, nextframe, 1 );
						if( ret ){
							DbgOut( "moeanim : SetNewPoseByMOAML : mh SetMotionFrameNoML error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						int frameno;
						frameno = min( idlingleng, (firstframe + 1) );
						ret = mhandler->SetMotionFrameNoML( shandler->m_mathead, seri, idlingid, frameno, 1 );
						if( ret ){
							DbgOut( "moeanim : SetNewPoseByMOAML : mh SetMotionFrameNoML error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}

						ret = mhandler->SetTickCount( seri, curtick );
						_ASSERT( !ret );

					}
				}
			}

		}
	}


	int chkid, chkframe;
	ret = mhandler->GetFirstTickJoint( 0, idlingid, &chkid, &chkframe );
	if( ret ){
		DbgOut( "moeanim : SetNewPoseByMOAML : mh GetFirstTickJoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( chkid > 0 ){
		int chkseri;
		for( chkseri = 0; chkseri < shandler->s2shd_leng; chkseri++ ){
			CShdElem* selem;
			selem = (*shandler)( chkseri );

			if( (chkseri != chkid) && selem->IsJoint() && (selem->type != SHDMORPH) ){
				int id, frame;
				ret = mhandler->GetMotionFrameNoML( chkseri, &id, &frame );
				if( ret ){
					DbgOut( "moeanim : SetNewPoseByMOAML : mh GetMotionFrameNoML chk error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( (id == idlingid) && (frame != chkframe) ){
					ret = mhandler->SetMotionFrameNoML( shandler->m_mathead, chkseri, idlingid, chkframe, 0 );
					if( ret ){
						DbgOut( "moeanim : SetNewPoseByMOAML : mh SetMotionFrameNoML error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}
	}



// SetNewPoseMLは、複数回E3DSetMOAEventNoMLを呼び出したのち、１回だけ呼び出す。

//	ret = mhandler->SetNewPoseML( shandler );
//	if( ret ){
//		DbgOut( "moeanim : SetNewPoseByMOAML : mh SetNewPoseML error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}


	return 0;
}

int CMOEAnim::SetMLList( CShdHandler* shandler, int listnum, int* listptr, int notlistnum, int* notlistptr )
{
	if( !m_mllist || (m_mlleng != shandler->s2shd_leng) ){
		if( m_mllist ){
			free( m_mllist );
			m_mllist = 0;
		}

		m_mllist = (int*)malloc( sizeof( int ) * shandler->s2shd_leng );
		if( !m_mllist ){
			DbgOut( "moeanim : SetMLList : mllist alloc error!!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		m_mlleng = shandler->s2shd_leng;
	}
	ZeroMemory( m_mllist, sizeof( int ) * m_mlleng );


	int nlcnt;
	for( nlcnt = 0; nlcnt < notlistnum; nlcnt++ ){
		SetMLNotListReq( shandler, *( notlistptr + nlcnt ), 0 );
	}



	int seri;
	CShdElem* selem;
	int no = 0;
	int curlist = *listptr;
	if( curlist < 0 ){
		
		//notlist以外のすべてのジョイントに１をセット

		for( seri = 0; seri < shandler->s2shd_leng; seri++ ){
			selem = (*shandler)( seri );
			if( selem->IsJoint() && (selem->type != SHDMORPH) ){
				int curval;
				curval = *( m_mllist + seri );
				if( curval == 0 ){
					*( m_mllist + seri ) = 1;
				}
			}
		}

	}else{
		int lcnt;
		for( lcnt = 0; lcnt < listnum; lcnt++ ){
			SetMLListReq( shandler, *( listptr + lcnt ), 0 );
		}
	}

	return 0;
}

void CMOEAnim::SetMLNotListReq( CShdHandler* shandler, int srcnot, int broflag )
{
	if( (srcnot >= 0) && (srcnot < shandler->s2shd_leng) ){
		CShdElem* selem;
		selem = (*shandler)( srcnot );
		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			*( m_mllist + srcnot ) = 2;//!!!!!!!!!!!!!!!!!!!!!!!!!
		}

		if( broflag && selem->brother ){
			SetMLNotListReq( shandler, selem->brother->serialno, 1 );
		}
		
		if( selem->child ){
			SetMLNotListReq( shandler, selem->child->serialno, 1 );
		}

	}
}
void CMOEAnim::SetMLListReq( CShdHandler* shandler, int srclist, int broflag )
{
	if( (srclist >= 0) && (srclist < shandler->s2shd_leng) ){
		CShdElem* selem;
		selem = (*shandler)( srclist );
		if( selem->IsJoint() && (selem->type != SHDMORPH) ){

			if( *( m_mllist + srclist ) == 0 ){
				*( m_mllist + srclist ) = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!
			}
		}

		if( broflag && selem->brother ){
			SetMLListReq( shandler, selem->brother->serialno, 1 );
		}
		
		if( selem->child ){
			SetMLListReq( shandler, selem->child->serialno, 1 );
		}

	}
}

int CMOEAnim::CheckAndDelInvalid( CShdHandler* lpsh, int frameno, CMOEKey** ppmoek )//seriチェック。不正データ削除。
{
	int ret;
	ret = ExistMOEKey( frameno, ppmoek );
	if( ret ){
		DbgOut( "moeanim : CheckAndDelInvalid : ExistMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !*ppmoek ){
		return 0;
	}

	int eno;
	for( eno = 0; eno < (*ppmoek)->m_moeenum; eno++ ){
		MOEELEM* moeeptr = (*ppmoek)->m_pmoee + eno;
		ret = CheckAndDelInvalidMOEE( lpsh, moeeptr );
		if( ret ){
			DbgOut( "moeanim : CheckAndDelInvalid : CheckAndDelInvalidMOEE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CMOEAnim::InitFrameData()
{
	if( !m_framedata ){
		_ASSERT( 0 );
		return 0;
	}

	int frameno;
	for( frameno = 0; frameno <= m_maxframe; frameno++ ){
		( m_framedata + frameno )->InitMatrix();
	}

	return 0;
}

int CMOEAnim::GetLocalMotionFrameNo( int moeframeno, int* lanimno, int* lframeno )
{
	if( !m_framedata ){
		DbgOut( "moeanim : GetLocalMotionFrameNo : m_framedata not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (moeframeno < 0) || (moeframeno > m_maxframe) ){
		DbgOut( "moeanim : GetLocalMotionFrameNo : moeframeno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMState* curms = m_framedata + moeframeno;
	int boneno = 0;
	*lanimno = (curms->m_mselem + boneno)->localinf[MSLOCAL_ANIM];
	*lframeno = (curms->m_mselem + boneno)->localinf[MSLOCAL_FRAME];

	return 0;
}
int CMOEAnim::GetLocalNextMotionFrameNo( int moeframeno, int* lanimno, int* lframeno )
{
	if( !m_framedata ){
		DbgOut( "moeanim : GetLocalNextMotionFrameNo : m_framedata not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (moeframeno < 0) || (moeframeno > m_maxframe) ){
		DbgOut( "moeanim : GetLocalNextMotionFrameNo : moeframeno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CMState* curms = m_framedata + moeframeno;
	int boneno = 0;
	*lanimno = (curms->m_mselem + boneno)->localinf[MSLOCAL_NEXTANIM];
	*lframeno = (curms->m_mselem + boneno)->localinf[MSLOCAL_NEXTFRAME];

	return 0;
}


int CMOEAnim::GetLocalMotionFrameNoML( CShdHandler* lpsh, int moeframeno, int boneid, int* lanimno, int* lframeno )
{
	if( !m_framedata ){
		DbgOut( "moeanim : GetLocalMotionFrameNoML : m_framedata not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (moeframeno < 0) || (moeframeno > m_maxframe) ){
		DbgOut( "moeanim : GetLocalMotionFrameNoML : moeframeno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneid <= 0) || (boneid >= lpsh->s2shd_leng) ){
		DbgOut( "moeanim : GetLocalMotinoFrameNoML : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMState* curms = m_framedata + moeframeno;
	int boneno = *( m_seri2boneno + boneid );
	if( (boneno <= 0) || (boneno >= m_bonenum) ){
		DbgOut( "moeanim : GetLocalMotionFrameNoML : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*lanimno = (curms->m_mselem + boneno)->localinf[MSLOCAL_ANIM];
	*lframeno = (curms->m_mselem + boneno)->localinf[MSLOCAL_FRAME];

	return 0;
}
int CMOEAnim::GetLocalNextMotionFrameNoML( CShdHandler* lpsh, int moeframeno, int boneid, int* lanimno, int* lframeno )
{
	if( !m_framedata ){
		DbgOut( "moeanim : GetLocalNextMotionFrameNoML : m_framedata not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (moeframeno < 0) || (moeframeno > m_maxframe) ){
		DbgOut( "moeanim : GetLocalNextMotionFrameNoML : moeframeno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneid <= 0) || (boneid >= lpsh->s2shd_leng) ){
		DbgOut( "moeanim : GetLocalNextMotinoFrameNoML : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMState* curms = m_framedata + moeframeno;
	int boneno = *( m_seri2boneno + boneid );
	if( (boneno <= 0) || (boneno >= m_bonenum) ){
		DbgOut( "moeanim : GetLocalMotionFrameNoML : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*lanimno = (curms->m_mselem + boneno)->localinf[MSLOCAL_NEXTANIM];
	*lframeno = (curms->m_mselem + boneno)->localinf[MSLOCAL_NEXTFRAME];

	return 0;
}
