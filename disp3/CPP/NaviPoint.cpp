#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <NaviPoint.h>

#define	DBGH
#include "dbg.h"


static int s_allocno = 0;

CNaviPoint::CNaviPoint()
{
	InitParams( 1 );
	s_allocno++;
}


CNaviPoint::CNaviPoint( int ctrlflag )
{
	InitParams( ctrlflag );
	s_allocno++;
}

CNaviPoint::~CNaviPoint()
{


}

void CNaviPoint::InitParams(int ctrlflag )
{
	pointid = s_allocno; // line中で、一意の番号
	controlflag = ctrlflag; // ユーザー指定の点の場合１、補間で作成した場合０
	
	point.x = 0.0f;
	point.y = 0.0f;
	point.z = 0.0f;

	ownerid = -1; // pointを占有する？？場合に使うかも
	next = 0;
	prev = 0;

	ishead = 0;
}


int CNaviPoint::AddToPrev( CNaviPoint* addnp )
{
	CNaviPoint *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addnp->prev = prev;
	addnp->next = this;

	prev = addnp;

	if( saveprev ){
		saveprev->next = addnp;
	}else{
		_ASSERT( ishead );
		addnp->ishead = 1;
		ishead = 0;
	}

	return 0;
}

int CNaviPoint::AddToNext( CNaviPoint* addnp )
{
	CNaviPoint *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addnp->prev = this;
	addnp->next = savenext;

	next = addnp;

	if( savenext ){
		savenext->prev = addnp;
	}

	addnp->ishead = 0;

	return 0;
}

int CNaviPoint::LeaveFromChain( CNaviPoint** pphead )
{
	CNaviPoint *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	prev = 0;
	next = 0;

	if( saveprev ){
		saveprev->next = savenext;
	}else{
		_ASSERT( ishead );
		if( savenext ){
			savenext->ishead = 1;
		}
		*pphead = savenext;
		ishead = 0;
	}

	if( savenext ){
		savenext->prev = saveprev;
	}

	return 0;
}
