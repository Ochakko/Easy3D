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
	pointnum = 0; // <--- create時にセットする
	npno = 0;// leng : pointnum, navilineのpointidを順番に格納。
	roundnum = 0;
	clearflag = 0;// leng : pointnum * roundnum, 
	
	lastround = 0;//最後に、E3DSetNaviPointClearFlagしたときの、周回数
	lastpointarno = 0;//最後に、E3DSetNaviPointClearFlagしたときの、クリアーポイント
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

// npnoのセット
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

// clearflagの初期化
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


// 前回最後にクリアーした点との距離を求める。
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

// 次の点があるかどうかチェックする。
	chkround = lastround;
	chkparno = lastpointarno + 1;

	if( chkparno >= pointnum ){
		if( (chkround + 1) < roundnum ){
			//次の周回へ進む
			chkround++;
			chkparno = 0;

		}else{
			//最後まで、クリアーしている

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
			// クリアーした点を、増やして、次を調べる。
			lastround = chkround;
			lastpointarno = chkparno;
			lastdist = dist;

			*( clearflag + pointnum * chkround + chkparno ) = 1;

		}else{
			// クリアーできない点が見つかったので、検索は、ここまで。
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
				//新しい周回を調べる。
				chkround++;
				chkparno = 0;
			}else{
				//データの最後まで調べたので、検索は、ここまで。

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




