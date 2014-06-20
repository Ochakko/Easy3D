#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <GPAnim.h>

#include <shdhandler.h>
#include <mothandler.h>
#include <GPKey.h>

#include <BoneProp.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

CGPAnim::CGPAnim( char* srcname, int srcmotkind, GPELEM* defgpeptr, int* definterptr, D3DXMATRIX* offmatptr, int* offflagptr )
{
	InitParams();
	if( srcname && *srcname ){
		strcpy_s( m_animname, 256, srcname );
	}else{
		ZeroMemory( m_animname, sizeof( char ) * 256 );
	}

	m_motkind = srcmotkind;
	m_defgpeptr = defgpeptr;
	m_definterptr = definterptr;
	m_offmatptr = offmatptr;
	m_offflagptr = offflagptr;
}

CGPAnim::~CGPAnim()
{
	DestroyObjs();
}

int CGPAnim::InitParams()
{
	ZeroMemory( m_animname, sizeof( char ) * 256 );
	m_frameno = 0;
	ZeroMemory( &m_curgpe, sizeof( GPELEM ) );
	m_curinterp = 0;
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

	m_defgpeptr = 0;
	m_definterptr = 0;
	m_framedata = 0;

	m_groundhsid = 0;

	m_offmatptr = 0;
	m_offflagptr = 0;

	InitStepFunc();

	return 0;
}

void CGPAnim::InitStepFunc()
{
	int i;
	for( i = 0; i < MOTIONTYPEMAX; i++ ){

		switch( i ){
		case MOTION_STOP:
			StepFunc[i] = &CGPAnim::StepStop;
			break;
		case MOTION_CLAMP:
			StepFunc[i] = &CGPAnim::StepClamp;
			break;
		case MOTION_ROUND:
			StepFunc[i] = &CGPAnim::StepRound;
			break;
		case MOTION_INV:
			StepFunc[i] = &CGPAnim::StepInv;
			break;
		case MOTION_JUMP:
			StepFunc[i] = &CGPAnim::StepJump;
			break;
		default:
			StepFunc[i] = &CGPAnim::StepDummy;
			break;
		}

	}
}

int CGPAnim::DestroyObjs()
{
	CGPKey* delgpk = m_firstkey;
	CGPKey* nextgpk = 0;
	while( delgpk ){
		nextgpk = delgpk->next;
		delete delgpk;
		delgpk = nextgpk;
	}
	m_firstkey = 0;

	DestroyFrameData();

	InitParams();

	return 0;
}

int CGPAnim::ExistGPKey( int frameno, CGPKey** ppgpk )
{
	*ppgpk = 0;

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "GPAnim : ExistGPKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//////////
	CGPKey* curgpk = m_firstkey;
	while( curgpk && (curgpk->m_frameno <= frameno) ){
		if( curgpk->m_frameno == frameno ){
			*ppgpk = curgpk;
			break;
		}

		curgpk = curgpk->next;
	}

	return 0;
}
int CGPAnim::CreateGPKey( int frameno, CGPKey** ppgpk )
{
	*ppgpk = 0;

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "GPAnim : CreateGPKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////////
	CGPKey* newgpk = 0;
	newgpk = new CGPKey();
	if( !newgpk ){
		DbgOut( "GPAnim : CreateGPKey : newgpk alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newgpk->m_frameno = frameno;


//////////////////
	int ret;
	CGPKey* gpkhead = m_firstkey;
	CGPKey* curgpk = gpkhead;
	CGPKey* largergpk = 0;
	CGPKey* smallergpk = 0;
	int cmp;
	while( curgpk ){
		cmp = newgpk->FramenoCmp( curgpk );
		if( cmp < 0 ){
			largergpk = curgpk;
			break;
		}else{
			smallergpk = curgpk;
		}
		curgpk = curgpk->next;
	}

	if( !gpkhead ){
		//先頭に追加
		m_firstkey = newgpk;
	}else{
		if( largergpk ){
			//largergpkの前に追加。
			ret = largergpk->AddToPrev( newgpk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( !newgpk->prev )
				m_firstkey = newgpk;
		}else{
			//最後に追加。(smallergpk の後に追加)
			_ASSERT( smallergpk );
			ret = smallergpk->AddToNext( newgpk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}

	*ppgpk = newgpk;

	return 0;
}
CGPKey* CGPAnim::GetFirstGPKey()
{
	return m_firstkey;
}
int CGPAnim::DeleteGPKey( int frameno )
{
	CGPKey* delgpk = 0;
	delgpk = LeaveFromChainGPKey( frameno );
	if( delgpk ){
		delete delgpk;
	}
	
	return 0;
}
CGPKey* CGPAnim::LeaveFromChainGPKey( int frameno )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "GPAnim : LeaveFromChainGPKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	int ret;
	CGPKey* curgpk = 0;
	ret = ExistGPKey( frameno, &curgpk );
	if( ret ){
		return 0;
	}

	if( curgpk ){
		CGPKey* firstgpk;
		firstgpk = m_firstkey;
		if( firstgpk == curgpk ){
			m_firstkey = curgpk->next;
		}

		curgpk->LeaveFromChain();
	}

	return curgpk;
}
int CGPAnim::ChainGPKey( CGPKey* addgpk )
{
	if( (addgpk->m_frameno < 0) || (addgpk->m_frameno > m_maxframe) ){
		DbgOut( "GPAnim : ChainGPKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CGPKey* gpkhead = m_firstkey;
	CGPKey* curgpk = gpkhead;
	CGPKey* largergpk = 0;
	CGPKey* smallergpk = 0;
	int cmp;
	while( curgpk ){
		cmp = addgpk->FramenoCmp( curgpk );
		if( cmp < 0 ){
			largergpk = curgpk;
			break;
		}else{
			smallergpk = curgpk;
		}
		curgpk = curgpk->next;
	}

	if( !gpkhead ){
		//先頭に追加
		m_firstkey = addgpk;
	}else{
		if( largergpk ){
			//largergpkの前に追加。
			ret = largergpk->AddToPrev( addgpk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			if( !addgpk->prev )
				m_firstkey = addgpk;
		}else{
			//最後に追加。(smallergpk の後に追加)
			_ASSERT( smallergpk );
			ret = smallergpk->AddToNext( addgpk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	
	return 0;
}
int CGPAnim::DeleteGPKeyOutOfRange( int srcmaxframe )
{
	int ret;
	CGPKey* gpkptr = m_firstkey;
	CGPKey* nextgpk = 0;
	while( gpkptr ){
		nextgpk = gpkptr->next;
		int frameno = gpkptr->m_frameno;
		if( frameno > srcmaxframe ){
			ret = DeleteGPKey( frameno );
			if( ret ){
				DbgOut( "GPAnim : DeleteGPKeyOutOfRange : DeleteGPKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		gpkptr = nextgpk;
	}

	return 0;
}
int CGPAnim::SetGPAnim()
{
	int ret;
	int existflag = 0;
	ret = GetGPAnim( &m_curgpe, &m_curinterp, m_frameno, &existflag );
	if( ret ){
		DbgOut( "GPanim : SetGPAnim : GetGPAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CGPAnim::SetGPKey( int frameno, GPELEM srcgpe, int srcinterp )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "GPAnim : SetGPKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	CGPKey* pgpk = 0;
	ret = ExistGPKey( frameno, &pgpk );
	if( ret ){
		DbgOut( "GPAnim : SetGPKey : ExistGPKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int setinterp;
	if( !pgpk ){
		ret = CreateGPKey( frameno, &pgpk );
		if( ret || !pgpk ){
			DbgOut( "GPAnim : SetGPKey : CreateGPKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( srcinterp < 0 ){
			setinterp = *m_definterptr;
		}else{
			setinterp = srcinterp;
		}
	}else{
		if( srcinterp < 0 ){
			setinterp = pgpk->m_interp;
		}else{
			setinterp = srcinterp;
		}
	}

	_ASSERT( pgpk );

	ret = pgpk->SetGPKey( &srcgpe );
	_ASSERT( !ret );
	ret = pgpk->SetInterp( setinterp );
	_ASSERT( !ret );

	return 0;
}
/***
			D3DXVECTOR3 newpos;
			D3DXVec3TransformCoord( &newpos, &(gpkptr->m_gpe.pos), m_offmatptr );
			gpkptr->m_gpe.e3dpos = newpos;

			CQuaternion srcq;
			CQuaternion qx, qy, qz;
			D3DXVECTOR3 axisX( 1.0f, 0.0f, 0.0f );
			D3DXVECTOR3 axisY( 0.0f, 1.0f, 0.0f );
			D3DXVECTOR3 axisZ( 0.0f, 0.0f, 1.0f );
			qx.SetAxisAndRot( axisX, gpkptr->m_gpe.rot.x * (float)DEG2PAI );
			qy.SetAxisAndRot( axisY, gpkptr->m_gpe.rot.y * (float)DEG2PAI );
			qz.SetAxisAndRot( axisZ, gpkptr->m_gpe.rot.z * (float)DEG2PAI );
			srcq = qy * qx * qz;

			D3DXQUATERNION mqx, invmqx;
			D3DXQuaternionRotationMatrix( &mqx, m_offmatptr );
			D3DXQuaternionInverse( &invmqx, &mqx );
			CQuaternion mq;
			mq.x = invmqx.x;
			mq.y = invmqx.y;
			mq.z = invmqx.z;
			mq.w = invmqx.w;

			CQuaternion newq;
			newq = mq * srcq;
			newq.normalize();

			D3DXVECTOR3 neweul;
			ret = qToEuler( 0, &newq, &neweul );
			ret = modifyEuler( &neweul, &gpkptr->m_gpe.rot );

			gpkptr->m_gpe.e3drot = neweul;
***/

int CGPAnim::CalcFrameData( CShdHandler* lpsh, CMotHandler* lpmh, D3DXMATRIX* matWorld )
{
	int ret;
	int frameno;

	if( !m_firstkey ){
		for( frameno = 0; frameno <= m_maxframe; frameno++ ){
			( m_framedata + frameno )->m_frameno = frameno;
			( m_framedata + frameno )->m_gpe = *m_defgpeptr;
			( m_framedata + frameno )->m_interp = *m_definterptr;
			( m_framedata + frameno )->m_keyflag = 0;
		}
		return 0;//!!!!!!!
	}

	CGPKey* gpkptr = m_firstkey;

#ifdef INEASY3D
	while( gpkptr ){
		CQuaternion orgq;
		CQuaternion qx, qy, qz;
		D3DXVECTOR3 axisX( 1.0f, 0.0f, 0.0f );
		D3DXVECTOR3 axisY( 0.0f, 1.0f, 0.0f );
		D3DXVECTOR3 axisZ( 0.0f, 0.0f, 1.0f );
		qx.SetAxisAndRot( axisX, gpkptr->m_gpe.rot.x * (float)DEG2PAI );
		qy.SetAxisAndRot( axisY, gpkptr->m_gpe.rot.y * (float)DEG2PAI );
		qz.SetAxisAndRot( axisZ, gpkptr->m_gpe.rot.z * (float)DEG2PAI );
		orgq = qy * qx * qz;

		D3DXMATRIX orgmat;
		orgmat = orgq.MakeRotMatX();
		orgmat._41 = gpkptr->m_gpe.pos.x;
		orgmat._42 = gpkptr->m_gpe.pos.y;
		orgmat._43 = gpkptr->m_gpe.pos.z;

	/////////
		D3DXMATRIX multmat;
		multmat = orgmat * *m_offmatptr;
		//multmat = *m_offmatptr * orgmat;

		gpkptr->m_gpe.e3dpos.x = multmat._41;
		gpkptr->m_gpe.e3dpos.y = multmat._42;
		gpkptr->m_gpe.e3dpos.z = multmat._43;

	//////////
		D3DXMATRIX rotmat;
		rotmat = multmat;
		rotmat._41 = 0.0f;
		rotmat._42 = 0.0f;
		rotmat._43 = 0.0f;

		D3DXQUATERNION mqx, invmqx;
		D3DXQuaternionRotationMatrix( &mqx, &rotmat );
		D3DXQuaternionInverse( &invmqx, &mqx );
		CQuaternion mq;
		mq.x = mqx.x;
		mq.y = mqx.y;
		mq.z = mqx.z;
		mq.w = mqx.w;

		D3DXVECTOR3 neweul;
		ret = qToEuler( 0, &mq, &neweul );
		ret = modifyEuler( &neweul, &gpkptr->m_gpe.e3drot );

		gpkptr->m_gpe.e3drot = neweul;


		gpkptr = gpkptr->next;
	}
#endif

//キーの接地計算
	//CGPKey* gpkptr = m_firstkey;
	gpkptr = m_firstkey;
	if( lpsh ){
		while( gpkptr ){
			if( gpkptr->m_gpe.ongmode != GROUND_NONE ){
				//D3DXMATRIX inimat;
				//D3DXMatrixIdentity( &inimat );
				D3DXVECTOR3 befpos, newpos;
#ifdef INEASY3D
				befpos = gpkptr->m_gpe.e3dpos;
				befpos.y = gpkptr->m_gpe.rayy + m_offmatptr->_42;
				newpos = gpkptr->m_gpe.e3dpos;
				newpos.y = gpkptr->m_gpe.rayy - gpkptr->m_gpe.rayleng;
				int result = 0;
				D3DXVECTOR3 adjustv, nv;
				ret = lpsh->ChkConfGround( 0, 0, matWorld, befpos, newpos, lpmh, 0, 200.0f, newpos.y - 100.0f, &result, &adjustv, &nv );
				if( ret ){
					DbgOut( "GPAnim : CalcFrameData : sh ChkConfGround error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( result ){
					gpkptr->m_gpe.e3dpos = adjustv;
					gpkptr->m_gpe.e3dpos.y += gpkptr->m_gpe.offsety;
				}
#else
				befpos = gpkptr->m_gpe.pos;
				befpos.y = gpkptr->m_gpe.rayy;
				newpos = gpkptr->m_gpe.pos;
				newpos.y = gpkptr->m_gpe.rayy - gpkptr->m_gpe.rayleng;

				int result = 0;
				D3DXVECTOR3 adjustv, nv;
				ret = lpsh->ChkConfGround( 0, 0, matWorld, befpos, newpos, lpmh, 0, 200.0f, newpos.y - 100.0f, &result, &adjustv, &nv );
				if( ret ){
					DbgOut( "GPAnim : CalcFrameData : sh ChkConfGround error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( result ){
					gpkptr->m_gpe.pos = adjustv;
					gpkptr->m_gpe.pos.y += gpkptr->m_gpe.offsety;
				}
#endif
			}
			gpkptr = gpkptr->next;
		}
	}


	CGPKey* prevkey = 0;
	CGPKey* nextkey = 0;
	for( frameno = 0; frameno <= m_maxframe; frameno++ ){
		CGPKey* dstkey = m_framedata + frameno;
		dstkey->m_frameno = frameno;

		int findkey = 0;
		CGPKey* tmpkey = 0;
		ExistGPKey( frameno, &tmpkey );
		if( tmpkey ){
			findkey = 1;
			prevkey = tmpkey;
		}
		if( prevkey ){
			nextkey = prevkey->next;
		}

		dstkey->m_keyflag = findkey;

		if( findkey ){
			dstkey->m_gpe = prevkey->m_gpe;
			dstkey->m_interp = prevkey->m_interp;
		}else if( !prevkey ){
			dstkey->m_gpe = *m_defgpeptr;
			dstkey->m_interp = *m_definterptr;
		}else if( !nextkey ){
			dstkey->m_gpe = prevkey->m_gpe;
			dstkey->m_interp = prevkey->m_interp;
		}else{
			dstkey->m_interp = prevkey->m_interp;
			int framenum;
			framenum = nextkey->m_frameno - prevkey->m_frameno;
			int framecnt;
			framecnt = frameno - prevkey->m_frameno;

//DbgOut( "check!!! : GPAnim : CalcFrameData : frameno %d, maxframe %d, framenum %d, framecnt %d\r\n",
//	frameno, m_maxframe, framenum, framecnt );

			ret = FillUpGPElem( prevkey, nextkey, &dstkey->m_gpe, framenum, framecnt, prevkey->m_interp, lpsh, lpmh, matWorld );
			if( ret ){
				DbgOut( "GPAnim : CalcFrameData : FillUpGPElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}
int CGPAnim::GetGPAnim( GPELEM* gpeptr, int* interpptr, int srcframeno, int* existflag )
{
	if( (srcframeno < 0) || (srcframeno > m_maxframe) ){
		DbgOut( "GPAnim : GetGPAnim : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*gpeptr = (m_framedata + srcframeno)->m_gpe;
	*interpptr = (m_framedata + srcframeno)->m_interp;
	*existflag = (m_framedata + srcframeno)->m_keyflag;

	return 0;
}

int CGPAnim::FillUpGPElem( CGPKey* startgpk, CGPKey* endgpk, GPELEM* dstgpe, int framenum, int framecnt, int interp,
	CShdHandler* lpsh, CMotHandler* lpmh, D3DXMATRIX* matWorld )
{
	int ret;
	GPELEM* startgpe;
	GPELEM* endgpe;
	startgpe = &( startgpk->m_gpe );
	endgpe = &( endgpk->m_gpe );

	dstgpe->ongmode = startgpe->ongmode;
	dstgpe->rayy = startgpe->rayy;
	dstgpe->rayleng = startgpe->rayleng;
	dstgpe->offsety = startgpe->offsety;

	if( interp == INTERPOLATION_SLERP ){

		D3DXVECTOR3 startpos, startrot;
		D3DXVECTOR3 steppos, steprot;
#ifdef INEASY3D
		startpos = startgpe->e3dpos;
		startrot = startgpe->e3drot;
		if( framenum != 0 ){
			steppos = ( endgpe->e3dpos - startpos ) / (float)framenum;
			steprot = ( endgpe->e3drot - startrot ) / (float)framenum;
		}else{
			steppos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			steprot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		}
		dstgpe->e3dpos = startpos + steppos * (float)framecnt;
		dstgpe->e3drot = startrot + steprot * (float)framecnt;

#else
		startpos = startgpe->pos;
		startrot = startgpe->rot;
		if( framenum != 0 ){
			steppos = ( endgpe->pos - startpos ) / (float)framenum;
			steprot = ( endgpe->rot - startrot ) / (float)framenum;
		}else{
			steppos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			steprot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		}
		dstgpe->pos = startpos + steppos * (float)framecnt;
		dstgpe->rot = startrot + steprot * (float)framecnt;
#endif

	}else{
		GPELEM* befgpe;
		GPELEM* aftgpe;
		
		if( startgpk->prev ){
			befgpe = &( startgpk->prev->m_gpe );
		}else{
			befgpe = startgpe;
		}

		if( endgpk->next ){
			aftgpe = &( endgpk->next->m_gpe );
		}else{
			aftgpe = endgpe;
		}
	
		float t;
		t = (float)framecnt / (float)framenum;

#ifdef INEASY3D
		ret = SplineVec3( befgpe->e3dpos, startgpe->e3dpos, endgpe->e3dpos, aftgpe->e3dpos, &( dstgpe->e3dpos ), t );
		_ASSERT( !ret );
		ret = SplineVec3( befgpe->e3drot, startgpe->e3drot, endgpe->e3drot, aftgpe->e3drot, &( dstgpe->e3drot ), t );
		_ASSERT( !ret );
#else
		ret = SplineVec3( befgpe->pos, startgpe->pos, endgpe->pos, aftgpe->pos, &( dstgpe->pos ), t );
		_ASSERT( !ret );
		ret = SplineVec3( befgpe->rot, startgpe->rot, endgpe->rot, aftgpe->rot, &( dstgpe->rot ), t );
		_ASSERT( !ret );
#endif

	}
	
	if( (startgpk->m_gpe.ongmode == GROUND_ON) && lpsh ){
		//D3DXMATRIX inimat;
		//D3DXMatrixIdentity( &inimat );
		D3DXVECTOR3 befpos, newpos;
#ifdef INEASY3D
		befpos = dstgpe->e3dpos;
		befpos.y = startgpk->m_gpe.rayy + m_offmatptr->_42;
		newpos = dstgpe->e3dpos;
		newpos.y = startgpk->m_gpe.rayy - startgpk->m_gpe.rayleng;
		int result = 0;
		D3DXVECTOR3 adjustv, nv;
		ret = lpsh->ChkConfGround( 0, 0, matWorld, befpos, newpos, lpmh, 0, 200.0f, newpos.y - 100.0f, &result, &adjustv, &nv );
		if( ret ){
			DbgOut( "GPAnim : FillUpGPElem : sh ChkConfGround error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( result ){
			dstgpe->e3dpos = adjustv;
			dstgpe->pos.y += dstgpe->offsety;
		}
#else
		befpos = dstgpe->pos;
		befpos.y = startgpk->m_gpe.rayy;
		//newpos = dstgpe->e3dpos;
		newpos = dstgpe->pos;
		newpos.y = startgpk->m_gpe.rayy - startgpk->m_gpe.rayleng;
		int result = 0;
		D3DXVECTOR3 adjustv, nv;
		ret = lpsh->ChkConfGround( 0, 0, matWorld, befpos, newpos, lpmh, 0, 200.0f, newpos.y - 100.0f, &result, &adjustv, &nv );
		if( ret ){
			DbgOut( "GPAnim : FillUpGPElem : sh ChkConfGround error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( result ){
			dstgpe->pos = adjustv;
			dstgpe->pos.y += dstgpe->offsety;
		}
#endif
	}

	return 0;
}

int CGPAnim::SplineF( float val1, float val2, float val3, float val4, float* dstptr, float t )
{
	const float alpha = 0.0f;
	float m0, m1;

	m0 = ( 1.0f - alpha ) / 2.0f * ( ( val2 - val1 ) + ( val3 - val2 ) );
	m1 = ( 1.0f - alpha ) / 2.0f * ( ( val3 - val2 ) + ( val4 - val3 ) );

	float t2 = t * t;
	float t3 = t2 * t;

	float resval;

	resval = (((2 * t3) - (3 * t2) + 1) * val2) +
                     ((t3 - (2 * t2) + t) * m0) +
                     ((t3 - t2) * m1) +
                     (((-2 * t3) + (3 * t2)) * val3);

	*dstptr = resval;
	return 0;
}

int CGPAnim::SplineVec3( D3DXVECTOR3 befv, D3DXVECTOR3 startv, D3DXVECTOR3 endv, D3DXVECTOR3 aftv, D3DXVECTOR3* dstv, float t )
{
	int ret;
	ret = SplineF( befv.x, startv.x, endv.x, aftv.x, &(dstv->x), t );
	_ASSERT( !ret );

	ret = SplineF( befv.y, startv.y, endv.y, aftv.y, &(dstv->y), t );
	_ASSERT( !ret );

	ret = SplineF( befv.z, startv.z, endv.z, aftv.z, &(dstv->z), t );
	_ASSERT( !ret );

	return 0;
}





int CGPAnim::SetGPAnimNo( int frameno, int srcisfirst )
{
	int ret;
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "GPAnim : SetGPAnimNo : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_frameno = frameno;
	m_isfirst = srcisfirst;

	int existflag = 0;
	ret = GetGPAnim( &m_curgpe, &m_curinterp, m_frameno, &existflag );
	if( ret ){
		DbgOut( "GPAnim : SetGPAnimNo : GetGPAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

int CGPAnim::GetNextGP( NEXTMOTION* pnm, int getonly )
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

int CGPAnim::StepStop( NEXTMOTION* pnm )
{
	pnm->mk = m_motkind;
	pnm->aftframeno = m_frameno;
	return 0;
}
int CGPAnim::StepClamp( NEXTMOTION* pnm )
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
int CGPAnim::StepRound( NEXTMOTION* pnm )
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
int CGPAnim::StepInv( NEXTMOTION* pnm )
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
int CGPAnim::StepJump( NEXTMOTION* pnm )
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

int CGPAnim::StepDummy( NEXTMOTION* pnm )	
{
	DbgOut( "CGPAnim : StepFunc called StepDummy warning !!!\n" );
	pnm->mk = m_motkind;
	pnm->aftframeno = m_frameno;
	return 0;
}


int CGPAnim::GetGPKeyframeNoRange( int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	*framenumptr = 0;

	int setno = 0;
	CGPKey* curgpk = m_firstkey;
	while( curgpk ){
		if( (curgpk->m_frameno >= srcstart) && (curgpk->m_frameno <= srcend) ){
			if( framearray && (arrayleng > 0) ){
				if( setno >= arrayleng ){
					DbgOut( "GPAnim : GetGPKeyframeNoRange : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				*( framearray + setno ) = curgpk->m_frameno;
			}
			setno++;
		}
		curgpk = curgpk->next;
	}

	*framenumptr = setno;

	return 0;
}

int CGPAnim::SetAnimName( char* srcname )
{
	if( srcname && *srcname ){
		strcpy_s( m_animname, 256, srcname );
	}else{
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CGPAnim::GetAnimName( char* dstname )
{
	strcpy_s( dstname, 256, m_animname );
	return 0;
}

int CGPAnim::StepGP( int frameno )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "GPAnim : StepGP : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_frameno = frameno;

	int ret;
	int existflag = 0;
	ret = GetGPAnim( &m_curgpe, &m_curinterp, m_frameno, &existflag );
	if( ret ){
		DbgOut( "GPAnim : StepGP : GetGPAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CGPAnim::SetFillUpMode( int frameno, int setfu )
{
	int ret;
	if( frameno >= 0 ){
		CGPKey* gpkptr = 0;
		ret = ExistGPKey( frameno, &gpkptr );
		if( ret ){
			DbgOut( "GPAnim : SetFillUpMode : ExistGPKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( gpkptr ){
			gpkptr->m_interp = setfu;
		}

	}else{
		CGPKey* gpkptr;
		gpkptr = GetFirstGPKey();
		while( gpkptr ){
			gpkptr->m_interp = setfu;
			gpkptr = gpkptr->next;
		}
	}

	return 0;
}

int CGPAnim::CreateFrameData( int framenum )
{
	DestroyFrameData();

	m_framedata = new CGPKey[ framenum ];
	if( !m_framedata ){
		DbgOut( "GPAnim : CreateFrameData : framedata alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CGPAnim::DestroyFrameData()
{
	if( m_framedata ){
		delete [] m_framedata;
		m_framedata = 0;
	}

	return 0;
}
