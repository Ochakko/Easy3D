#include "stdafx.h"
#include <windows.h>
#include <math.h>

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <D3DX9.h>

#include <coef.h>
#define DBGH
#include "dbg.h"

#include <crtdbg.h>

#include <naviline.h>
#include <navipoint.h>

#include "npcleardata.h"


CNPClearData::CNPClearData()
{
	InitParams();
}
CNPClearData::~CNPClearData()
{
	DestroyObjs();
	InitParams();
}


void CNPClearData::InitParams()
{
	ownerhsid = 0;
	nlptr = 0;
	pointnum = 0; // <--- create���ɃZ�b�g����
	npno = 0;// leng : pointnum, naviline��pointid�����ԂɊi�[�B
	roundnum = 0;
	clearflag = 0;// leng : pointnum * roundnum, 
	
	lastround = 0;//�Ō�ɁAE3DSetNaviPointClearFlag�����Ƃ��́A����
	lastpointarno = 0;//�Ō�ɁAE3DSetNaviPointClearFlag�����Ƃ��́A�N���A�[�|�C���g
	lastdist = 0.0f;
}

int CNPClearData::DestroyData()
{
	DestroyObjs();
	InitParams();
	return 0;
}

void CNPClearData::DestroyObjs()
{
	if( npno ){
		free( npno );
		npno = 0;
	}

	if( clearflag ){
		free( clearflag );
		clearflag = 0;
	}
}



int CNPClearData::CreateData( int ownerid, CNaviLine* srcnl, int srcroundnum )
{
	int ret = 0;
	int setno;
	
	DestroyObjs();
	InitParams();

	ownerhsid = ownerid;
	nlptr = srcnl;
	roundnum = srcroundnum;
	pointnum = nlptr->pnum;

	CNaviPoint* curnp;

	npno = (int*)malloc( sizeof( int ) * pointnum );
	if( !npno ){
		DbgOut( "npcleardata : CreateData : npno alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto createdataexit;
	}

	clearflag = (char*)malloc( sizeof( char ) * pointnum * roundnum );
	if( !clearflag ){
		DbgOut( "npcleardata : CreateData : clearflag alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto createdataexit;
	}

// npno�̃Z�b�g
	setno = 0;
	curnp = nlptr->nphead;
	while( curnp ){
		if( setno >= pointnum ){
			DbgOut( "npcleardata : CreateData : setno error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto createdataexit;
		}
		*(npno + setno) = curnp->pointid;

		curnp = curnp->next;
		setno++;
	}

// clearflag�̏�����
	ret = InitFlag();	
	if( ret ){
		DbgOut( "npcleardata : CreateData : InitFlag error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto createdataexit;		
	}
	
//
	goto createdataexit;
createdataexit:
	if( ret ){
		DestroyObjs();
		InitParams();
	}
	return ret;

}


int CNPClearData::InitFlag()
{
	if( !clearflag || !npno ){
		DbgOut( "npcleardata : InitFlag : objs not exist warning !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!
	}

	
	ZeroMemory( clearflag, sizeof( char ) * pointnum * roundnum );
	lastround = 0;
	lastpointarno = 0;
	lastdist = 0.0f;

	return 0;
}


int CNPClearData::SetNaviPointClearFlag( D3DXVECTOR3 srcpos, float maxdist, int* pidptr, int* roundptr, float* distptr )
{
	int chkparno, chkround;
	CNaviPoint* chknp;
	D3DXVECTOR3 diffvec;
	float distmag, dist;

	if( !clearflag || !npno ){
		DbgOut( "npcleardata : SetNaviPointClearFlag : objs not exist warning !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!
	}


// �O��Ō�ɃN���A�[�����_�Ƃ̋��������߂�B
	chknp = nlptr->GetNaviPoint( *(npno + lastpointarno) );
	if( !chknp ){
		DbgOut( "npcleardata : SetNaviPointClearFlag : nlptr GetNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	diffvec = srcpos - chknp->point;
	distmag = diffvec.x * diffvec.x + diffvec.y * diffvec.y + diffvec.z * diffvec.z;
	if( distmag > 0.0f ){
		dist = sqrtf( distmag );
	}else{
		dist = 0.0f;
	}

// ���̓_�����邩�ǂ����`�F�b�N����B
	chkround = lastround;
	chkparno = lastpointarno + 1;

	if( chkparno >= pointnum ){
		if( (chkround + 1) < roundnum ){
			//���̎���֐i��
			chkround++;
			chkparno = 0;

		}else{
			//�Ō�܂ŁA�N���A�[���Ă���

			lastdist = dist;//!!!!!!!!!!

			*pidptr = chknp->pointid;
			*roundptr = lastround;
			*distptr = dist;
			return 0;
		}
	}


	int befround = lastround;
	int befparno = lastpointarno;
	float befdist = dist;//!!!!!!!!!

	while( 1 ){
		chknp = nlptr->GetNaviPoint( *(npno + chkparno) );
		_ASSERT( chknp );
		diffvec = srcpos - chknp->point;
		
		distmag = diffvec.x * diffvec.x + diffvec.y * diffvec.y + diffvec.z * diffvec.z;
		if( distmag > 0.0f ){
			dist = sqrtf( distmag );
		}else{
			dist = 0.0f;
		}
		
		if( dist < maxdist ){
			// �N���A�[�����_���A���₵�āA���𒲂ׂ�B
			lastround = chkround;
			lastpointarno = chkparno;
			lastdist = dist;

			*( clearflag + pointnum * chkround + chkparno ) = 1;

		}else{
			// �N���A�[�ł��Ȃ��_�����������̂ŁA�����́A�����܂ŁB
			lastround = befround;
			lastpointarno = befparno;
			lastdist = befdist;

			break;//!!!!!!!!!!!!
		}

	//
		befround = chkround;
		befparno = chkparno;
		befdist = dist;

		chkparno++;
		if( chkparno >= pointnum ){
			if( (chkround + 1) < roundnum ){
				//�V��������𒲂ׂ�B
				chkround++;
				chkparno = 0;
			}else{
				//�f�[�^�̍Ō�܂Œ��ׂ��̂ŁA�����́A�����܂ŁB

				break;//!!!!!!!!!
			}
		}
	}

	CNaviPoint* setnp = nlptr->GetNaviPoint( *(npno + lastpointarno) );
	_ASSERT( setnp );
	*pidptr = setnp->pointid;
	*roundptr = lastround;
	*distptr = befdist;			

	return 0;
}




