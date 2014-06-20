#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <NaviPoint.h>
#include <NaviLine.h>

#define	DBGH
#include "dbg.h"


static int s_allocno = 0;


CNaviLine::CNaviLine()
{
	InitParams();
	s_allocno++;
}

CNaviLine::~CNaviLine()
{

	CNaviPoint* curnp = nphead;
	CNaviPoint* nextnp;
	while( curnp ){
		nextnp = curnp->next;

		delete curnp;
		curnp = nextnp;
	}
	nphead = 0;
	
	pnum = 0;

}

void CNaviLine::InitParams()
{
	lineid = s_allocno;//e3d内で、一意の番号
	pnum = 0;
	nphead = 0;

	next = 0;
}


int CNaviLine::AddNaviPoint( int befnpid, int ctrlflag, int* npidptr )
{
	// befpointidの次にチェインする。bef==-1のとき最後に、bef==-2のとき最初にチェインする

	*npidptr = -1;

	CNaviPoint* newnp;
	newnp = new CNaviPoint( ctrlflag );
	if( !newnp ){
		DbgOut( "NaviLine : AddNaviPoint : newnp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	if( befnpid == -1 ){
		//最後にチェイン
		CNaviPoint* curnp = nphead;
		CNaviPoint* lastnp = 0;
		while( curnp ){
			lastnp = curnp;
			curnp = curnp->next;
		}

		if( lastnp ){
			ret = lastnp->AddToNext( newnp );
			if( ret ){
				DbgOut( "NaviLine : AddNaviPoint : -1 : lastnp AddToNext error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			//まだ、pointが一つもないので、先頭に。
			_ASSERT( !nphead );
			nphead = newnp;
			newnp->ishead = 1;
		}

	}else if( befnpid == -2 ){
		//最初にチェイン

		ret = nphead->AddToPrev( newnp );
		if( ret ){
			DbgOut( "NaviLine : AddNaviPoint : -2 : nphead AddToPrev error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( newnp->ishead != 1 ){
			DbgOut ( "NaviLine : AddNaviPoint : -2 : newnp ishead error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		// befnpidの次にチェイン

		CNaviPoint* curnp = nphead;
		CNaviPoint* findnp = 0;
		while( curnp ){
			if( curnp->pointid == befnpid ){
				findnp = curnp;
				break;
			}
			curnp = curnp->next;
		}

		if( !findnp ){
			DbgOut( "NaviLine : AddNaviPoint : befnpid : findnp error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = findnp->AddToNext( newnp );
		if( ret ){
			DbgOut( "NaviLine : AddNaviPoint : befnpid : findnp AddToNext error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( newnp->ishead == 1 )
		nphead = newnp;

	pnum++;


	*npidptr = newnp->pointid;
	return 0;
}

CNaviPoint* CNaviLine::GetNaviPoint( int npid )
{
	CNaviPoint* curnp = nphead;
	CNaviPoint* findnp = 0;
	while( curnp ){
		if( curnp->pointid == npid ){
			findnp = curnp;
			break;
		}
		curnp = curnp->next;
	}

	return findnp;
}
CNaviPoint* CNaviLine::GetLastNaviPoint()
{
	CNaviPoint* curnp = nphead;
	CNaviPoint* lastnp = 0;
	while( curnp ){
		lastnp = curnp;

		curnp = curnp->next;
	}

	return lastnp;
}


CNaviPoint* CNaviLine::GetTargetNaviPoint( D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, int roundflag, int reverseflag, int ctrlmode )
{
	CNaviPoint* findnp = 0;

	if( !nphead )
		return 0;

	//if( ctrlmode & 0x01 ){
// srcposと同一点がある場合（キャストの誤差を考慮する）
		//同一点の次のポイントを返す。
		float zerodist = 4.0f;
		float curdist;
		float dx, dy, dz;
		CNaviPoint* curnp = nphead;
		while( curnp ){
			dx = curnp->point.x + offset.x - srcpos.x;
			dy = curnp->point.y + offset.y - srcpos.y;
			dz = curnp->point.z + offset.z - srcpos.z;
			curdist = dx * dx + dy * dy + dz * dz;

			if( curdist <= zerodist ){
				if( reverseflag == 0 ){
					findnp = curnp->next;
					if( !findnp ){
						if( roundflag == 0 ){
							findnp = curnp;	
						}else{
							findnp = nphead;
						}
					}
				}else{
					findnp = curnp->prev;
					if( !findnp ){
						if( roundflag == 0 ){
							findnp = curnp;
						}else{
							findnp = GetLastNaviPoint();
						}
					}
				}
//DbgOut( "CNaviLine::GetTargetNaviPoint : 1 : targetnp %d\n", findnp->pointid );

				break;
			}
			curnp = curnp->next;
		}
	//}

	if( !findnp ){
		// 内積と距離で判定する。
		if( reverseflag == 0 ){
			findnp = GetTargetNaviPoint_regular( srcpos, offset, roundflag, ctrlmode );
		}else{
			findnp = GetTargetNaviPoint_reverse( srcpos, offset, roundflag, ctrlmode );
		}
		
//DbgOut( "CNaviLine::GetTargetNaviPoint : 2 : targetnp %d\n", findnp->pointid );

	}



	return findnp;
}

CNaviPoint* CNaviLine::GetTargetNaviPoint_regular( D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, int roundflag, int ctrlmode )
{
	// prev->cur と src->curの内積、　src->curの距離で判定する。
	CNaviPoint* targetnp = 0;
	CNaviPoint* tempnp = 0;

	D3DXVECTOR3 prev2cur, src2cur;
	float distsrc2cur;
	float chkdot;
	float mindist = 1e10;
	float mindist2 = 1e10;

	CNaviPoint* prevnp;
	if( roundflag == 0 )
		prevnp = nphead;
	else
		prevnp = GetLastNaviPoint();

	CNaviPoint* curnp = nphead;

	if( !prevnp || !curnp ){
		return 0;
	}

	while( curnp ){	
		
		prev2cur = (curnp->point + offset) - (prevnp->point + offset);
		src2cur = (curnp->point + offset) - srcpos;
		
		chkdot = D3DXVec3Dot( &prev2cur, &src2cur );

		distsrc2cur = src2cur.x * src2cur.x + src2cur.y * src2cur.y + src2cur.z * src2cur.z;
		
		// 内積が正で、距離が最短の物を探す。
		if( chkdot >= 0.0f ){
			if( distsrc2cur < mindist ){
				targetnp = curnp;
				mindist = distsrc2cur;
			}
		}

		// 上の条件を満たす物が無かったときのために、距離だけで判定したものを、保存しておく。
		if( distsrc2cur < mindist2 ){
			tempnp = curnp;
			mindist2 = distsrc2cur;
		}


		curnp = curnp->next;
		if( curnp )
			prevnp = curnp->prev;
		else
			prevnp = 0;
	}

	if( targetnp == 0 )
		targetnp = tempnp;


	if( !(ctrlmode & 0x01) ){
		// 一番近いポイントの次のポイントを目指す
		CNaviPoint* nextnp;
		nextnp = targetnp->next;
		if( !nextnp ){
			if( roundflag == 0 ){
				nextnp = targetnp;
			}else{
				nextnp = nphead;
			}
		}
		targetnp = nextnp;
	}



	return targetnp;
}
CNaviPoint* CNaviLine::GetTargetNaviPoint_reverse( D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, int roundflag, int ctrlmode )
{
	// next->cur と src->curの内積、　src->curの距離で判定する。
	CNaviPoint* targetnp = 0;
	CNaviPoint* tempnp = 0;

	D3DXVECTOR3 next2cur, src2cur;
	float distsrc2cur;
	float chkdot;
	float mindist = 1e10;
	float mindist2 = 1e10;

	CNaviPoint* nextnp;
		
	CNaviPoint* curnp = nphead;

	if( !curnp ){
		return 0;
	}

	while( curnp ){
		nextnp = curnp->next;
		if( !nextnp ){
			if( roundflag == 0 ){
				nextnp = curnp;
			}else{
				nextnp = nphead;
			}
		}

		next2cur = (curnp->point + offset) - (nextnp->point + offset);
		src2cur = (curnp->point + offset) - srcpos;
		
		chkdot = D3DXVec3Dot( &next2cur, &src2cur );

		distsrc2cur = src2cur.x * src2cur.x + src2cur.y * src2cur.y + src2cur.z * src2cur.z;
		
		// 内積が正で、距離が最短の物を探す。
		if( chkdot >= 0.0f ){
			if( distsrc2cur < mindist ){
				targetnp = curnp;
				mindist = distsrc2cur;
			}
		}

		// 上の条件を満たす物が無かったときのために、距離だけで判定したものを、保存しておく。
		if( distsrc2cur < mindist2 ){
			tempnp = curnp;
			mindist2 = distsrc2cur;
		}


		curnp = curnp->next;
	}


	if( targetnp == 0 )
		targetnp = tempnp;

	if( !(ctrlmode & 0x01) ){
		// 一番近いポイントの前のポイントを目指す
		CNaviPoint* prevnp;
		prevnp = targetnp->prev;
		if( !prevnp ){
			if( roundflag == 0 ){
				prevnp = targetnp;
			}else{
				prevnp = GetLastNaviPoint();
			}
		}
		targetnp = prevnp;
	}

	return targetnp;
}

int CNaviLine::RemoveNaviPoint( int npid )
{
	CNaviPoint* curnp = nphead;
	CNaviPoint* rmnp = 0;
	while( curnp ){
		if( curnp->pointid == npid ){
			rmnp = curnp;
			break;
		}
		curnp = curnp->next;
	}

	if( !rmnp ){
		DbgOut( "NaviLine : RemoveNaviPoint : rmnp not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = rmnp->LeaveFromChain( &nphead );
	if( ret ){
		DbgOut( "NaviLine : RemoveNaviPoint : rmnp LeaveFromChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	delete rmnp;

	pnum--;

	return 0;
}

int CNaviLine::GetNaviPointPos( int npid, D3DXVECTOR3* posptr )
{
	CNaviPoint* curnp;
	curnp = GetNaviPoint( npid );
	if( !curnp ){
		DbgOut( "NaviLine : GetNaviPointPos : GetNaviPoint : npid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posptr = curnp->point;
	
	return 0;

}

int CNaviLine::SetNaviPointPos( int npid, D3DXVECTOR3* srcpos )
{
	CNaviPoint* curnp;
	curnp = GetNaviPoint( npid );
	if( !curnp ){
		DbgOut( "NaviLine : SetNaviPointPos : GetNaviPoint : npid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curnp->point = *srcpos;

	return 0;
}


int CNaviLine::GetNaviPointOwnerID( int npid, int* oidptr )
{
	CNaviPoint* curnp;
	curnp = GetNaviPoint( npid );
	if( !curnp ){
		DbgOut( "NaviLine : GetNaviPointOwnerID : GetNaviPoint : npid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*oidptr = curnp->ownerid;


	return 0;
}

int CNaviLine::SetNaviPointOwnerID( int npid, int oid )
{
	CNaviPoint* curnp;
	curnp = GetNaviPoint( npid );
	if( !curnp ){
		DbgOut( "NaviLine : SetNaviPointOwnerID : GetNaviPoint : npid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curnp->ownerid = oid;

	return 0;

}

int CNaviLine::GetNextNaviPoint( int npid, int* nextptr )
{

	if( npid >= 0 ){
		CNaviPoint* curnp;
		curnp = GetNaviPoint( npid );
		if( !curnp ){
			DbgOut( "NaviLine : GetNextNaviPoint : GetNaviPoint : npid not found error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( curnp->next ){
			*nextptr = curnp->next->pointid;
		}else{
			*nextptr = -1;
		}		
	}else{
		//先頭のNaviPointを返す
		if( nphead ){
			*nextptr = nphead->pointid;
		}else{
			*nextptr = -1;
		}
	}

	return 0;
}

int CNaviLine::GetPrevNaviPoint( int npid, int* prevptr )
{
	CNaviPoint* curnp;
	curnp = GetNaviPoint( npid );
	if( !curnp ){
		DbgOut( "NaviLine : GetPrevNaviPoint : GetNaviPoint : npid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curnp->prev ){
		*prevptr = curnp->prev->pointid;
	}else{
		*prevptr = -1;
	}
	return 0;
}



int CNaviLine::GetNearestNaviPoint( D3DXVECTOR3* srcpos, int* nearptr, int* prevptr, int* nextptr )
{
	CNaviPoint* nearnp = 0;
	float minmag = 1e10;
	float dx, dy, dz, mag;

	CNaviPoint* curnp = nphead;
	while( curnp ){
		dx = srcpos->x - curnp->point.x;
		dy = srcpos->y - curnp->point.y;
		dz = srcpos->z - curnp->point.z;

		mag = dx * dx + dy * dy + dz * dz;
		if( mag < minmag ){
			nearnp = curnp;
			minmag = mag;
		}

		curnp = curnp->next;
	}

	if( nearnp ){
		*nearptr = nearnp->pointid;
		
		if( nearnp->prev ){
			*prevptr = nearnp->prev->pointid;
		}else{
			*prevptr = -1;
		}

		if( nearnp->next ){
			*nextptr = nearnp->next->pointid;
		}else{
			*nextptr = -1;
		}

	}else{
		*nearptr = -1;
		*prevptr = -1;
		*nearptr = -1;
	}


	return 0;
}

int CNaviLine::ReorderNaviPoint( int* oldnoptr, int ptrlen, int* setlenptr )
{
	int setno = 0;
	CNaviPoint* curnp = nphead;
	while( curnp ){
		if( setno >= ptrlen ){
			DbgOut( "naviline : ReorderNaviPoint : setno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		*( oldnoptr + setno ) = curnp->pointid;
		curnp->pointid = setno;

		curnp = curnp->next;
		setno++;
	}

	*setlenptr = setno;

	return 0;
}


int CNaviLine::FillUpNaviLine( int div, int flag )
{
	if( pnum < 3 ){
		DbgOut( "NaviLine : FillUpNaviLine : pnum less than 3 : exit !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( div < 2 ){
		DbgOut( "NaviLine : FillUpNaviLine : div too small warning : exit !!!\n" );
		return 0;
	}

	CNaviPoint** saveptr;
	saveptr = (CNaviPoint**)malloc( sizeof( CNaviPoint* ) * pnum );
	if( !saveptr ){
		DbgOut( "NaviLine : FillUpNaviLine : saveptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	// ポイントを保存して処理しないと、新しく補間したポイント間を補間してしまう。
	int saveno = 0;
	CNaviPoint* topnp = nphead;
	while( topnp ){
		if( saveno >= pnum ){
			DbgOut( "NaviLine : FillUpNaviLine : saveno error %d %d\n", saveno, pnum );
			_ASSERT( 0 );
			free( saveptr );
			return 1;
		}

		*(saveptr + saveno) = topnp;

		topnp = topnp->next;
		saveno++;
	}


	CNaviPoint** ptr1;
	CNaviPoint** ptr2;
	CNaviPoint** ptr3;
	CNaviPoint** ptr4;

	CNaviPoint** topptr;
	topptr = saveptr;
	int ret;
	int fillno;
	for( fillno = 0; fillno < (pnum - 1); fillno++ ){
		
		ptr1 = topptr;

		if( fillno == 0 ){
			ptr2 = ptr1;		
		}else{
			ptr2 = ptr1 + 1;
		}

		ptr3 = ptr2 + 1;

		if( fillno == (pnum - 2) ){
			ptr4 = ptr3;
		}else{
			ptr4 = ptr3 + 1;
		}

		if( !*ptr1 || !*ptr2 || !*ptr3 || !*ptr4 ){
			DbgOut( "NaviLine : FillUpNaviLine : ptr NULL error %x %x %x %x !!!\n",
				ptr1, ptr2, ptr3, ptr4 );
			_ASSERT( 0 );
			free( saveptr );
			return 1;
		}

		ret = CreateCatmullRomPoint( *ptr1, *ptr2, *ptr3, *ptr4, div );
		if( ret ){
			DbgOut( "NaviLine : FillUpNaviLine : CreateCatmullRomPoint error !!!\n" );
			_ASSERT( 0 );
			free( saveptr );
			return 1;
		}

		topptr = ptr2;
	}

	free( saveptr );
	saveptr = 0;


	// pnum の更新
	int newpnum;
	newpnum = pnum + (div - 1) * (pnum - 1);

	// pnum のチェック
	int npcnt = 0;
	CNaviPoint* curnp = nphead;

	while( curnp ){
		npcnt++;
		curnp = curnp->next;
	}

	if( newpnum == npcnt ){
		pnum = newpnum;
	}else{
		pnum = npcnt;
		DbgOut( "NaviLine : FillUpNaviLine : newpnum != npcnt error %d %d!!!\n", newpnum, npcnt );
		_ASSERT( 0 );
		return 1;
	}

	// flag != 0 の時は、pointnoをふり直す。
	if( flag != 0 ){
		curnp = nphead;
		int curno = 0;
		while( curnp ){
			curnp->pointid = curno;
			
			curno++;
			curnp = curnp->next;
		}

		if( curno != pnum ){
			DbgOut( "NaviLine : FillUpNaviLine : curno != pnum error %d %d!!!\n", curno, pnum );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CNaviLine::CreateCatmullRomPoint( CNaviPoint* np1, CNaviPoint* np2, CNaviPoint* np3, CNaviPoint* np4, int div )
{
	float step;
	float u;
	float coef1, coef2, coef3, coef4;

	CNaviPoint* newnp;
	CNaviPoint* savenp = np2;

	step = 1.0f / (float)div;

	int ret;
	int divno;
	u = step;
	for( divno = 0; divno < div - 1; divno++ ){

		coef1 = -0.5f * u * u * u + u * u - 0.5f * u;
		coef2 = 1.5f * u * u * u - 2.5f * u * u + 1.0f;
		coef3 = -1.5f * u * u * u + 2.0f * u * u + 0.5f * u;
		coef4 = 0.5f * u * u * u - 0.5f * u * u;

		newnp = new CNaviPoint( 0 );
		if( !newnp ){
			DbgOut( "NaviLine : CreateCatmullRomPoint : newnp alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		newnp->point = np1->point * coef1 + np2->point * coef2 + np3->point * coef3 + np4->point * coef4;

		ret = savenp->AddToNext( newnp );
		if( ret ){
			DbgOut( "NaviLine : CreateCatmullRomPont : savenp AddToNext error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		u += step;
		savenp = newnp;
	}

	return 0;
}

int CNaviLine::ControlByNaviLine( D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, CQuaternion srcq, 
			int ctrlmode, int roundflag, int reverseflag,
			float maxdist,
			float posstep, float dirstep,
			D3DXVECTOR3* newposptr, CQuaternion* newqptr, int* tpidptr )
{
	int ret;
	

	D3DXVECTOR3 zerovec( 0.0f, 0.0f, 0.0f );
	CQuaternion zeroq( 1.0f, 0.0f, 0.0f, 0.0f );

	CNaviPoint* targetnp;
	targetnp = GetNaviPoint( *tpidptr );
	if( !targetnp ){
		//_ASSERT( 0 );
		//return 1;
		targetnp = GetTargetNaviPoint( srcpos, offset, roundflag, reverseflag, ctrlmode );
		if( !targetnp ){
			DbgOut( "NaviLine : ControlByNaviLine : GetNearNaviPoint not found error !!!\n" );
			_ASSERT( 0 );
			*newposptr = zerovec;
			*newqptr = zeroq;
			return 1;
		}
	}

	int xzflag;
	if( ctrlmode & 0x04 ){
		xzflag = 1;
	}else{
		xzflag = 0;
	}

	if( ctrlmode & 0x1 ){
		ret = ControlByNL_Pos( targetnp, xzflag, srcpos, offset, srcq, roundflag, reverseflag, posstep, dirstep, newposptr, newqptr, tpidptr );
		if( ret ){
			DbgOut( "NaviLine : ControlByNaviLine : ControlByNL_Pos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = ControlByNL_Dir( targetnp, xzflag, srcpos, offset, srcq, roundflag, reverseflag, maxdist, posstep, dirstep, newposptr, newqptr, tpidptr );
		if( ret ){
			DbgOut( "NaviLine : ControlByNaviLine : ControlByNL_Dir error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}



int CNaviLine::ControlByNL_Pos( CNaviPoint* targetnp, int xzflag, D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, CQuaternion srcq, 
		int roundflag, int reverseflag,
		float posstep, float dirstep,
		D3DXVECTOR3* newposptr, CQuaternion* newqptr, int* tpidptr )
{
	
// 位置を設定
	D3DXVECTOR3 src2targ;
	float dist;
	CNaviPoint* newtarget = 0;

	src2targ = (targetnp->point + offset) - srcpos;
	dist = src2targ.x * src2targ.x + src2targ.y * src2targ.y + src2targ.z * src2targ.z;

	if( dist < (posstep * posstep) ){
		// 目標点に設定
		*newposptr = targetnp->point + offset;
		
		if( reverseflag == 0 ){
			newtarget = targetnp->next;
			if( !newtarget ){
				if( roundflag == 0 ){
					newtarget = targetnp;
				}else{
					newtarget = nphead;
				}
			}
		}else{
			newtarget = targetnp->prev;
			if( !newtarget ){
				if( roundflag == 0 ){
					newtarget = targetnp;
				}else{
					newtarget = GetLastNaviPoint();
				}
			}
		}
		
		_ASSERT( newtarget );
		*tpidptr = newtarget->pointid;// 次のポイントを目指す。

	}else{
		// 目標点にposstepだけ近づける。

		D3DXVECTOR3 dir;
		D3DXVec3Normalize( &dir, &src2targ );

		*newposptr = srcpos + posstep * dir;

		//*tpidptrはそのまま
		*tpidptr = targetnp->pointid;
	}

// 向きを設定
	int ret;

	if( xzflag == 0 ){
		ret = DirTo( src2targ, newqptr );
		if( ret ){
			DbgOut( "NaviLine : ControlByNL_Pos : DirTo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = DirToXZ( src2targ, newqptr );
		if( ret ){
			DbgOut( "NaviLine : ControlByNL_Pos : DirToXZ error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}

int CNaviLine::ControlByNL_Dir( CNaviPoint* targetnp, int xzflag, D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, CQuaternion srcq, 
		int roundflag, int reverseflag,
		float maxdist,
		float posstep, float dirstep,
		D3DXVECTOR3* newposptr, CQuaternion* newqptr, int* tpidptr )
{
	int ret;

	CNaviPoint* fromv;
	
	if( reverseflag == 0 ){
		fromv = targetnp->prev;
		if( !fromv ){
			if( roundflag == 0 ){
				fromv = targetnp;
			}else{
				fromv = GetLastNaviPoint();
			}
		}
	}else{
		fromv = targetnp->next;
		if( !fromv ){
			if( roundflag == 0 ){
				fromv = targetnp;
			}else{
				fromv = nphead;
			}
		}
	}

//////
	// srcpos から、navilineへの距離をチェックし、maxdistより遠ければ、ControlByNL_Pos に処理を任せる。
	// fromv == targetのときも、ControlByNL_Pos に処理を任せる。

	float distfromline;
	D3DXVECTOR3 vertp;

	

	if( fromv->point != targetnp->point ){
		D3DXVECTOR3 pos1, pos2, pos3;
		pos1 = fromv->point + offset;
		pos2 = targetnp->point + offset;
		pos3 = srcpos;
		ret = GetVerticalPos( &pos1, &pos2, &pos3, 0, &vertp );
		if( ret ){
			DbgOut( "naviline : ControlByNL_Dir : GetVerticalPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		/// !!!!
//DbgOut( "NaviLine::ControlByNL_Dir : Call ControlByNL_Pos 1\n" );

		ret = ControlByNL_Pos( targetnp, xzflag, srcpos, offset, srcq, roundflag, reverseflag, posstep, dirstep, newposptr, newqptr, tpidptr );
		return ret;
	}
	float dx, dy, dz;
	dx = vertp.x - srcpos.x;
	dy = vertp.y - srcpos.y;
	dz = vertp.z - srcpos.z;

	distfromline = dx * dx + dy * dy + dz * dz;
	if( distfromline > (maxdist * maxdist) ){
		/// !!!!
//DbgOut( "NaviLine::ControlByNL_Dir : Call ControlByNL_Pos 2\n" );

		ret = ControlByNL_Pos( targetnp, xzflag, srcpos, offset, srcq, roundflag, reverseflag, posstep, dirstep, newposptr, newqptr, tpidptr );
		return ret;
	}

/////向きの設定
	D3DXVECTOR3 from2cur;
	from2cur = (targetnp->point + offset) - (fromv->point + offset);
	D3DXVec3Normalize( &from2cur, &from2cur );

	
	D3DXVECTOR3 setdir;
	if( xzflag == 0 ){
		ret = LimitedDirTo( srcq, dirstep, from2cur, newqptr, &setdir );
		if( ret ){
			DbgOut( "NaviLine : ControlByNL_Pos : LimitedDirTo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = LimitedDirToXZ( srcq, dirstep, from2cur, newqptr, &setdir );
		if( ret ){
			DbgOut( "NaviLine : ControlByNL_Pos : LimitedDirToXZ error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


/////位置の設定
	_ASSERT( fromv->point != targetnp->point );//これを満たさない場合は、上で、return している　はず！！。
	

	D3DXVECTOR3 new0;
	new0 = srcpos + posstep * setdir;

	D3DXVECTOR3 src2new0, new02cur;
	float chkdot;

	src2new0 = new0 - srcpos;
	new02cur = (targetnp->point + offset) - new0;
	chkdot = D3DXVec3Dot( &src2new0, &new02cur );
	if( chkdot < 0.0f ){
		// targetを通り過ぎている。targetからsrc2new0へおろした垂線の位置にclampし、次の目標点を目指す。

		D3DXVECTOR3 new1;
		D3DXVECTOR3 pos1, pos2, pos3;
		pos1 = srcpos;
		pos2 = new0;
		pos3 = targetnp->point + offset;
		ret = GetVerticalPos( &pos1, &pos2, &pos3, 1, &new1 );
		if( ret ){
			DbgOut( "naviline : ControlByNL_Dir : GetVertialPos 2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*newposptr = new1;//!!!!!!!!

		CNaviPoint* newtarget = 0;
		if( reverseflag == 0 ){
			newtarget = targetnp->next;
			if( !newtarget ){
				if( roundflag == 0 ){
					newtarget = targetnp;
				}else{
					newtarget = nphead;
				}
			}
		}else{
			newtarget = targetnp->prev;
			if( !newtarget ){
				if( roundflag == 0 ){
					newtarget = targetnp;
				}else{
					newtarget = GetLastNaviPoint();
				}
			}
		}
		
		_ASSERT( newtarget );
		*tpidptr = newtarget->pointid;// 次のポイントを目指す。

//DbgOut( "NaviLine::ControlByNL_Dir : chkdot < 0 : nexttarget %d\n", *tpidptr );

	}else{
		// new0 を新しい位置とする。目標点は、そのまま。

		*newposptr = new0;
		*tpidptr = targetnp->pointid;

//DbgOut( "NaviLine::ControlByNL_Dir : chkdot >= 0 : nexttarget %d\n", *tpidptr );
	
	}



	return 0;
}

int CNaviLine::GetVerticalPos( D3DXVECTOR3* pos1, D3DXVECTOR3* pos2, D3DXVECTOR3* pos3, int clampflag, D3DXVECTOR3* dstp )
{
	//！！！pos1 != pos2 が保証された状態で、呼ばれる！！！
	_ASSERT( pos1 != pos2 );

	/***
//pos3 が　直線pos1-->pos2 上の点であるかどうかをチェックする
	float t0;
	t0 = ( pos3->x - pos2->x ) / ( pos1->x - pos2->x );
	float chky, chkz;
	chky = t0 * pos1->y + ( 1.0f - t0 ) * pos2->y;
	chkz = t0 * pos1->z + ( 1.0f - t0 ) * pos2->z;

	if( (chky == pos3->y) && (chkz == pos3->z) ){
		// pos3 が　直線pos1-->pos2 上の点である

		if( (t0 >= 0.0f) && (t0 <= 1.0f) ){
			*dstp = *pos3;
		}else if( t0 < 0.0f ){
			*dstp = *pos2;
		}else if( t0 > 1.0f ){
			*dstp = *pos1;
		}else{
			_ASSERT( 0 );
			*dstp = *pos3;
		}
		return 0;
	}
////
	***/

	D3DXVECTOR3 vec2to1, vec2to3;

	vec2to1 = *pos1 - *pos2;
	vec2to3 = *pos3 - *pos2;

	float t, tunder, tupper;
	tunder = vec2to1.x * vec2to1.x + vec2to1.y * vec2to1.y + vec2to1.z * vec2to1.z;
	tupper = vec2to1.x * vec2to3.x + vec2to1.y * vec2to3.y + vec2to1.z * vec2to3.z;

	_ASSERT( tupper != 0.0f );
	t = tupper / tunder;

	if( clampflag == 0 ){
		*dstp = t * *pos1 + ( 1.0f - t ) * *pos2;
	}else{
		if( (t >= 0.0f) && (t <= 1.0f) ){
			*dstp = t * *pos1 + ( 1.0f - t ) * *pos2;
		}else if( t < 0.0f ){
			*dstp = *pos2;
		}else if( t > 1.0f ){
			*dstp = *pos1;
		}else{
			_ASSERT( 0 );
			*dstp = *pos1;
		}
	}

	return 0;
}

int CNaviLine::LimitedDirToXZ( CQuaternion srcq, float maxdeg, D3DXVECTOR3 newdir, CQuaternion* dstq, D3DXVECTOR3* setdir )
{
	int ret;
	float maxrad;
	maxrad = maxdeg * (float)DEG2PAI;

// 制限回転角度内で、新しい向きsetdir を決める。
	D3DXVECTOR3 olddir3;
	D3DXVECTOR3 basevec3( 0.0f, 0.0f, -1.0f );
	srcq.Rotate( &olddir3, basevec3 );

	D3DXVECTOR2 olddir2;
	olddir2.x = olddir3.x;
	olddir2.y = olddir3.z;
	D3DXVec2Normalize( &olddir2, &olddir2 );

	D3DXVECTOR2 newdir2;
	newdir2.x = newdir.x;
	newdir2.y = newdir.z;
	D3DXVec2Normalize( &newdir2, &newdir2 );


	float dot0;
	dot0 = D3DXVec2Dot( &olddir2, &newdir2 );
	if( dot0 > 1.0f )
		dot0 = 1.0f;
	else if( dot0 < -1.0f )
		dot0 = -1.0f;
	float radold2new;
	radold2new = (float)acos( dot0 );

	float ccw0;
	ccw0 = D3DXVec2CCW( &olddir2, &newdir2 );

	if( radold2new <= maxrad ){
		setdir->x = newdir.x;
		setdir->y = 0.0f;
		setdir->z = newdir.z;

		D3DXVec3Normalize( setdir, setdir );//!!!

	}else{
		
		float roty0;
		if( ccw0 >= 0.0f ){
			roty0 = -maxdeg;
		}else{
			roty0 = maxdeg;
		}
		CQuaternion q0;
		ret = q0.SetRotation( 0.0f, roty0, 0.0f );
		_ASSERT( !ret );
		D3DXVECTOR3 tempdir3;
		tempdir3.x = olddir2.x;
		tempdir3.y = 0.0f;
		tempdir3.z = olddir2.y;
		q0.Rotate( setdir, tempdir3 );

		setdir->y = 0.0f;
		D3DXVec3Normalize( setdir, setdir );//!!!
	}	
	
	
////　basevecを　setdir に 向かせるための　dstq を求める。
	D3DXVECTOR2 basevec( 0.0f, -1.0f );
	
	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;
	

	vec.x = setdir->x;
	vec.y = setdir->z;

	D3DXVec2Normalize( &nvec, &vec );
	float dot;
	dot = D3DXVec2Dot( &basevec, &nvec );
	float rad;
	if( dot <= -1.0f )
		dot = -1.0f;
	else if( dot >= 1.0f )
		dot = 1.0f;

	rad = (float)acos( dot );
	float ccw;
	ccw = D3DXVec2CCW( &basevec, &nvec );

	float roty;
	if( ccw >= 0.0f ){
		roty = -rad * (float)PAI2DEG;
	}else{
		roty = rad * (float)PAI2DEG;
	}

	ret = dstq->SetRotation( 0.0f, roty, 0.0f );
	_ASSERT( !ret );


	return 0;
}

int CNaviLine::LimitedDirTo( CQuaternion srcq, float maxdeg, D3DXVECTOR3 newdir, CQuaternion* dstq, D3DXVECTOR3* setdir )
{
	float maxrad;
	maxrad = maxdeg * (float)DEG2PAI;

// 制限回転角度内で、新しい向きsetdir を決める。
	D3DXVECTOR3 basevec( 0.0f, 0.0f, -1.0f );

	D3DXVECTOR3 olddir;
	srcq.Rotate( &olddir, basevec );

	float dot0;
	dot0 = D3DXVec3Dot( &olddir, &newdir );
	if( dot0 > 1.0f )
		dot0 = 1.0f;
	else if( dot0 < -1.0f )
		dot0 = -1.0f;
	float radold2new;
	radold2new = (float)acos( dot0 );


	if( radold2new <= maxrad ){
		*setdir = newdir;

	}else{
		D3DXVECTOR3 axis0;
		D3DXVec3Cross( &axis0, &olddir, &newdir );
		D3DXVec3Normalize( &axis0, &axis0 );

		CQuaternion qreg, qrev;
		qreg.SetAxisAndRot( axis0, maxrad );
		qrev.SetAxisAndRot( axis0, -maxrad );

		D3DXVECTOR3 testreg, testrev;
		qreg.Rotate( &testreg, olddir );
		qrev.Rotate( &testrev, olddir );

		float dotreg, dotrev;
		dotreg = D3DXVec3Dot( &newdir, &testreg );
		dotrev = D3DXVec3Dot( &newdir, &testrev );
		if( dotreg >= dotrev )
			*setdir = testreg;
		else
			*setdir = testrev;
	}


////　basevecを　setdir に 向かせるための　dstq を求める。

	D3DXVECTOR3 axis;

	D3DXVec3Cross( &axis, &basevec, setdir );
	D3DXVec3Normalize( &axis, &axis );

	float dot, rad;
	dot = D3DXVec3Dot( &basevec, setdir );
	if( dot > 1.0f )
		dot = 1.0f;
	else if( dot < -1.0f )
		dot = -1.0f;
	rad = (float)acos( dot );

	CQuaternion q0, q1;
	q0.SetAxisAndRot( axis, rad );
	q1.SetAxisAndRot( axis, -rad );

	D3DXVECTOR3 testvec0, testvec1;
	q0.Rotate( &testvec0, basevec );
	q1.Rotate( &testvec1, basevec );

	float testdot0, testdot1;
	testdot0 = D3DXVec3Dot( setdir, &testvec0 );
	testdot1 = D3DXVec3Dot( setdir, &testvec1 );

	if( testdot0 >= testdot1 )
		*dstq = q0;
	else
		*dstq = q1;

	return 0;
}

int CNaviLine::DirToXZ( D3DXVECTOR3 newdir, CQuaternion* dstq )
{
	D3DXVECTOR2 basevec;
	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;
	
	basevec.x = 0.0f;
	basevec.y = -1.0f;

	vec.x = newdir.x;
	vec.y = newdir.z;

	D3DXVec2Normalize( &nvec, &vec );
	float dot;
	dot = D3DXVec2Dot( &basevec, &nvec );
	float rad;
	if( dot <= -1.0f )
		dot = -1.0f;
	else if( dot >= 1.0f )
		dot = 1.0f;

	rad = (float)acos( dot );
	float ccw;
	ccw = D3DXVec2CCW( &basevec, &nvec );

	float roty;
	if( ccw >= 0.0f ){
		roty = -rad * (float)PAI2DEG;
	}else{
		roty = rad * (float)PAI2DEG;
	}

	int ret;
	ret = dstq->SetRotation( 0.0f, roty, 0.0f );
	_ASSERT( !ret );
	

	return ret;
}

int CNaviLine::DirTo( D3DXVECTOR3 newdir, CQuaternion* dstq )
{
	D3DXVECTOR3 basevec;
	D3DXVECTOR3 vec;
	
	basevec.x = 0.0f;
	basevec.y = 0.0f;
	basevec.z = -1.0f;

	D3DXVec3Normalize( &vec, &newdir );

	D3DXVECTOR3 axis;

	D3DXVec3Cross( &axis, &basevec, &vec );
	D3DXVec3Normalize( &axis, &axis );

	float dot, rad;
	dot = D3DXVec3Dot( &basevec, &vec );
	if( dot > 1.0f )
		dot = 1.0f;
	else if( dot < -1.0f )
		dot = -1.0f;
	rad = (float)acos( dot );

	CQuaternion q0, q1;
	q0.SetAxisAndRot( axis, rad );
	q1.SetAxisAndRot( axis, -rad );

	D3DXVECTOR3 testvec0, testvec1;
	q0.Rotate( &testvec0, basevec );
	q1.Rotate( &testvec1, basevec );

	float testdot0, testdot1;
	testdot0 = D3DXVec3Dot( &vec, &testvec0 );
	testdot1 = D3DXVec3Dot( &vec, &testvec1 );

	CQuaternion q;
	if( testdot0 >= testdot1 )
		*dstq = q0;
	else
		*dstq = q1;

	return 0;
}


