
#include "StdAfx.h"
#ifndef SHDTREEVIEWH
#include <windows.h>
#endif

#include <coef.h>
#include <InfElem.h>

#include <boneinfo.h>

#include <shdhandler.h>
#include <shdelem.h>


/***
typedef struct tag_infelem
{
	int childno;//影響ボーン、子供の番号。エンドジョイントもあり。
	int bonematno;//マトリックス格納ボーンの番号、親の番号。bonematno, parmatno。子供を持つジョイントの番号しか入らない。
	int kind;//CALC_*
	float userrate;//％
	float orginf;//CALC_*で計算した値。
	float dispinf;//　orginf[] * userrate[]、normalizeflagが１のときは、正規化する。
}INFELEM;
***/

CInfElem::CInfElem()
{
	InitParams();
}

CInfElem::~CInfElem()
{
	DestroyObjs();
}

int CInfElem::InitIE( INFELEM* dstie )
{
	dstie->childno = -1;
	dstie->bonematno = -1;
	dstie->kind = CALCMODE_ONESKIN0;
	dstie->userrate = 1.0f;
	dstie->orginf = 0.0f;
	dstie->dispinf = 0.0f;

	return 0;
}

int CInfElem::InitParams()
{
	infnum = 0;
	normalizeflag = 1;
	symaxis = SYMAXIS_X;
	symdist = 100.0f;

	ie = 0;

	//////// for miko
	mikoapchildnum = 0;
	mikoapchildseri = 0;
	nearestbi = 0;
	nearestbipar = 0;

	return 0;
}
int CInfElem::DestroyObjs()
{
	ClearMikoParams();
	DestroyIE();

	return 0;
}

int CInfElem::DestroyIE()
{
	if( ie ){
		free( ie );
		ie = 0;
	}
	infnum = 0;

	return 0;
}

int CInfElem::ClearMikoParams()
{
	if( mikoapchildseri ){
		free( mikoapchildseri );
		mikoapchildseri = 0;
	}
	mikoapchildnum = 0;
	nearestbi = 0;
	nearestbipar = 0;

	return 0;
}


int CInfElem::AddMikoApChild( int srcchildseri )
{
	mikoapchildnum++;

	mikoapchildseri = (int*)realloc( mikoapchildseri, sizeof( int ) * mikoapchildnum );
	if( !mikoapchildseri ){
		DbgOut( "bi : AddMikoApChild : mikoapchildseri alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( mikoapchildseri + mikoapchildnum - 1 ) = srcchildseri;

	return 0;
}

int CInfElem::SetNearestBI( CShdHandler* srclpsh, D3DXVECTOR3 srcv )
{
	int ret;

	/***
	ret = Replace2FloatBone( srclpsh );
	if( ret ){
		DbgOut( "InfElem : SetNearestBI : Replace2FloatBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/

	int i;
	int curapchil;
	CShdElem* curapchilelem;
	CPart* chilpart;

	CShdElem* parelem;
	CPart* parpart;

	D3DXVECTOR3 bonecenter;
	D3DXVECTOR3 diff;
	float curdist;

	float mindist = 1e6;
	CShdElem* nearapchilelem = 0;
	CPart* nearparpart = 0;

	for( i = 0; i < mikoapchildnum; i++ ){
		curapchil = *( mikoapchildseri + i );
		curapchilelem = (*srclpsh)( curapchil );
		if( !curapchilelem ){
			_ASSERT( 0 );
			return 1;
		}
		chilpart = curapchilelem->part;
		if( !chilpart ){
			DbgOut( "InfElem : SetNearestBI : chilpart NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		parelem = srclpsh->FindUpperJoint( curapchilelem, 0 );
		if( parelem ){
			parpart = parelem->part;
			if( !parpart ){
				_ASSERT( 0 );
				return 1;
			}
			
			/***
			bonecenter.x = ( chilpart->jointloc.x + parpart->jointloc.x ) * 0.5f;
			bonecenter.y = ( chilpart->jointloc.y + parpart->jointloc.y ) * 0.5f;
			bonecenter.z = ( chilpart->jointloc.z + parpart->jointloc.z ) * 0.5f;

			diff = srcv - bonecenter;
			***/
			diff.x = srcv.x - parpart->jointloc.x;
			diff.y = srcv.y - parpart->jointloc.y;
			diff.z = srcv.z - parpart->jointloc.z;


			curdist = D3DXVec3Length( &diff );

			if( curdist <= mindist ){
				nearapchilelem = curapchilelem;
				nearparpart = parpart;
				mindist = curdist;
			}

		}
	}

	if( nearapchilelem && nearparpart ){
		ret = nearparpart->GetBoneInfo( nearapchilelem->serialno, &nearestbi );
		if( ret ){
			DbgOut( "InfElem : SetNearestBI : part GetBoneInfo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int parno;
		if( nearestbi )
			parno = nearestbi->parentjoint;
		else
			parno = -1;

		if( parno > 0 ){
			nearestbipar = (*srclpsh)( parno );
		}else{
			nearestbipar = 0;
		}
		
	}

	return 0;
}

int CInfElem::Replace2FloatBone( CShdHandler* srclpsh )
{
	if( mikoapchildnum != 2 )
		return 0;

	CShdElem* selem1;
	CShdElem* selem2;

	selem1 = (*srclpsh)( *mikoapchildseri );
	selem2 = (*srclpsh)( *(mikoapchildseri + 1) );

	if( !selem1 || !selem2 ){
		DbgOut( "InfElem : Replace2FloatBone : selem NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* par1;
	CShdElem* par2;

	par1 = srclpsh->FindUpperJoint( selem1, 0 );
	par2 = srclpsh->FindUpperJoint( selem2, 0 );

	if( (par1 == selem2) || (par2 == selem1) ){
		return 0;
	}

	CShdElem* gpa1 = 0;
	CShdElem* gpa2 = 0;
	
	if( par1 )
		gpa1 = srclpsh->FindUpperJoint( par1, 0 );

	if( par2 )
		gpa2 = srclpsh->FindUpperJoint( par2, 0 );


	if( gpa1 && (gpa1 == selem2) ){
		*( mikoapchildseri + 1 ) = par1->serialno;
	}else if( gpa2 && (gpa2 == selem1) ){
		*mikoapchildseri = par2->serialno;
	}


	return 0;
}


int CInfElem::SetInfElemDefault( CBoneInfo* srcbi, CShdElem* parselem, D3DXVECTOR3* srcxv, int mikoflag )
{

	if( parselem != 0 ){

		//スキニング有り

		if( infnum != 2 ){

			DestroyIE();

			ie = (INFELEM*)malloc( sizeof( INFELEM ) * 2 );
			if( !ie ){
				DbgOut( "ie : SetInfElemDefault : ie alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			infnum = 2;
		}
		
		INFELEM* curie = ie;
		INFELEM* parie = ie + 1;

		InitIE( curie );
		InitIE( parie );


		if( !srcbi ){
			curie->orginf = 1.0f;
			curie->dispinf = 1.0f;
			parie->orginf = 0.0f;
			parie->dispinf = 0.0f;

			curie->kind = CALCMODE_NOSKIN0;
			parie->kind = CALCMODE_NOSKIN0;

			return 0;
		}


		// dot
//		curie->bonematno = srcbi->jointno;
//		curie->childno = srcbi->childno;
//		parie->bonematno = srcbi->parentjoint;
//		parie->childno = srcbi->jointno;
		curie->bonematno = srcbi->childno;
		curie->childno = srcbi->childno;
		parie->bonematno = srcbi->jointno;
		parie->childno = srcbi->jointno;



		int ret;
		float brate;
		ret = CalcOneSkin0( srcbi, parselem, mikoflag, srcxv, &brate );
		if( ret ){
			DbgOut( "infelem : SetInfElemDefault : CalcOneSkin0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		curie->orginf = brate;
		curie->dispinf = brate;

		parie->orginf = 1.0f - brate;
		parie->dispinf = 1.0f - brate;

		curie->kind = CALCMODE_ONESKIN0;
		//parie->kind = CALCMODE_ONESKIN0;
		parie->kind = CALCMODE_DIRECT0;

	}else{
		//skinningなし

		if( infnum != 1 ){

			DestroyIE();

			ie = (INFELEM*)malloc( sizeof( INFELEM ) * 1 );
			if( !ie ){
				DbgOut( "ie : SetInfElemDefault : ie alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			infnum = 1;
		}
		
		INFELEM* curie = ie;

		InitIE( curie );

		if( !srcbi ){
			curie->bonematno = -1;//!!!!!!!!!
			curie->childno = -1;//!!!!!!!!!!!
			curie->orginf = 1.0f;
			curie->dispinf = 1.0f;

			curie->kind = CALCMODE_NOSKIN0;

			return 0;
		}

		// dot
//		curie->bonematno = srcbi->jointno;
//		curie->childno = srcbi->childno;
		curie->bonematno = srcbi->childno;
		curie->childno = srcbi->childno;


		curie->orginf = 1.0f;
		curie->dispinf = 1.0f;

		curie->kind = CALCMODE_NOSKIN0;

	}

	return 0;
}

int CInfElem::CalcOneSkin0( CBoneInfo* srcbi, CShdElem* parselem, int mikoflag, D3DXVECTOR3* srcxv, float* rateptr )
{
	float mag, dist;
	float vecx, vecy, vecz;
	vecx = srcxv->x - srcbi->bonestart.x;
	vecy = srcxv->y - srcbi->bonestart.y;
	vecz = srcxv->z - srcbi->bonestart.z;

	mag = vecx * vecx + vecy * vecy + vecz * vecz;
	if( mag != 0.0f ){
		dist = (float)sqrt( mag );
	}else{
		dist = 0.0f;
	}

	if( srcbi->boneleng != 0.0f ){
		dist /= srcbi->boneleng;
	}else{
		//dist = 0.0f;
		dist = 1e6;
	}
		
	CVec3f nvec;
	nvec.x = vecx; nvec.y = vecy; nvec.z = vecz;
	nvec.Normalize();

	float dotproduct;
	float brate;
	dotproduct = nvec.x * srcbi->bonevec.x + nvec.y * srcbi->bonevec.y + nvec.z * srcbi->bonevec.z;

	float sqdist;
	if( dist != 0.0f )
		sqdist = (float)sqrt( dist );
	else
		sqdist = 0.0f;

	CVec3f parloc;
	CVec3f cur2par;
	float dot2par;
	if( parselem ){
		CPart* parpart = parselem->part;
		_ASSERT( parpart );
		parloc = parpart->jointloc;
		cur2par.x = parloc.x - srcbi->bonestart.x;
		cur2par.y = parloc.y - srcbi->bonestart.y;
		cur2par.z = parloc.z - srcbi->bonestart.z;
		cur2par.Normalize();
		dot2par = nvec.x * cur2par.x + nvec.y * cur2par.y + nvec.z * cur2par.z;
	}else{
		dot2par = 1.0f;
	}


	if( dotproduct > 0.0f ){
		if( dotproduct < dot2par ){

			//brate = 0.5f - 0.5f * sqdist;

			if( mikoflag == 0 ){
				brate = 0.5f - 0.5f * sqdist;
			}else{
				brate = 0.5f - 0.05f * sqdist;
				//brate = 0.5f - 0.1f * sqdist;
				//brate = 0.5f - dotproduct * 0.1f * sqdist;
			}


		}else{
			brate = 0.5f + dotproduct * sqdist;
		}
	}else{
		brate = 0.5f + dotproduct * sqdist - 0.5f * sqdist;
		//brate = 1.0f;
		//brate = 0.5f + dotproduct * sqdist;
	}


	if( brate < 0.0f )
		brate = 0.0f;
	else if( brate > 1.0f )
		brate = 1.0f;

	*rateptr = brate;

	return 0;

}
int CInfElem::CalcOneSkin1( CBoneInfo* srcbi, D3DXVECTOR3* srcxv, float* rateptr )
{
	float mag, dist;
	float vecx, vecy, vecz;
	vecx = srcxv->x - srcbi->bonestart.x;
	vecy = srcxv->y - srcbi->bonestart.y;
	vecz = srcxv->z - srcbi->bonestart.z;

	mag = vecx * vecx + vecy * vecy + vecz * vecz;
	if( mag != 0.0f ){
		dist = (float)sqrt( mag );
	}else{
		dist = 0.0f;
	}

	if( srcbi->boneleng != 0.0f ){
		dist /= srcbi->boneleng;
	}else{
		//dist = 0.0f;
		dist = 1e6;
	}
		
	float brate;
	float sqdist;
	if( dist != 0.0f )
		sqdist = (float)sqrt( dist );
	else
		sqdist = 0.0f;

	brate = 1.0f - 0.1f * sqdist;

	if( brate < 0.0f )
		brate = 0.0f;
	else if( brate > 1.0f )
		brate = 1.0f;

	*rateptr = brate;

	return 0;

}


int CInfElem::SetInfElemDefaultNoSkin( int srcmatno, int srcchildno )
{
	if( infnum != 1 ){

		DestroyIE();

		ie = (INFELEM*)malloc( sizeof( INFELEM ) * 1 );
		if( !ie ){
			DbgOut( "ie : SetInfElemDefaultNoSkin : ie alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		infnum = 1;
	}
	
	INFELEM* curie = ie;

	InitIE( curie );


//	curie->bonematno = srcmatno;
//	curie->childno = srcchildno;
	curie->bonematno = srcchildno;
	curie->childno = srcchildno;


	curie->orginf = 1.0f;
	curie->dispinf = 1.0f;
	curie->kind = CALCMODE_NOSKIN0;

	return 0;
}

int CInfElem::SetInfElemInitial()
{
	if( infnum != 1 ){

		DestroyIE();

		ie = (INFELEM*)malloc( sizeof( INFELEM ) * 1 );
		if( !ie ){
			DbgOut( "ie : SetInfInitial : ie alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		infnum = 1;
	}
	
	INFELEM* curie = ie;

	InitIE( curie );

	curie->bonematno = -1;
	curie->childno = -1;
	curie->orginf = 1.0f;
	curie->dispinf = 1.0f;//!!!!!!!!!!!!!!!
	curie->kind = CALCMODE_NOSKIN0;

	return 0;

}

int CInfElem::AddInfElem( INFELEM srcie )
{
	infnum++;

	ie = (INFELEM*)realloc( ie, sizeof( INFELEM ) * infnum );
	if( !ie ){
		DbgOut( "infelem : AddInfElem : ie alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( ie + infnum - 1 ) = srcie;

	return 0;
}
INFELEM* CInfElem::ExistINFELEM( int srcchildno )
{
	INFELEM* retIE = 0;

	int infno;
	INFELEM* curIE;
	for( infno = 0; infno < infnum; infno++ ){
		curIE = ie + infno;

		if( curIE->childno == srcchildno ){
			retIE = curIE;
			break;
		}
	}

	return retIE;
}	


int CInfElem::SetInfElem( CBoneInfo* srcbi, CShdElem* parselem, D3DXVECTOR3 srcxv, int paintmode, int calcmode, float rate, int normflag, float directval )
{
	
	int ret;

	INFELEM* curIE;

	if( paintmode == PAINT_NOR ){
		curIE = ExistINFELEM( srcbi->childno );
		if( !curIE ){
			if( infnum <= 3 ){
				INFELEM newIE;
				InitIE( &newIE );
				ret = AddInfElem( newIE );
				if( ret ){
					DbgOut( "infelem : SetInfElem : PAINT_NOR : AddInfElem error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				curIE = ie + infnum - 1;
				_ASSERT( curIE );
			}else{
				//ret 2 は、５個以上の影響度設定をしようとして失敗したとき。
				return 2;//!!!!!!!!!!!!!!!!!!!!!!!!!
			}
		}
//		curIE->childno = srcbi->childno;
//		curIE->bonematno = srcbi->jointno;
		curIE->childno = srcbi->childno;
		curIE->bonematno = srcbi->childno;
		curIE->kind = calcmode;
		curIE->userrate = rate;

		float brate;

		switch( curIE->kind ){
		case CALCMODE_ONESKIN0:
			CalcOneSkin0( srcbi, parselem, 0, &srcxv, &brate );
			curIE->orginf = brate;
			curIE->dispinf = curIE->userrate * brate;
			break;
		case CALCMODE_ONESKIN1:
			CalcOneSkin1( srcbi, &srcxv, &brate );
			curIE->orginf = brate;
			curIE->dispinf = curIE->userrate * brate;
			break;
		case CALCMODE_NOSKIN0:
			_ASSERT( 0 );
			// noskinは、排他で呼ばれるはず。

			curIE->orginf = 1.0f;
			curIE->dispinf = 1.0f;
			break;
		case CALCMODE_DIRECT0:
			curIE->orginf = directval;
			curIE->dispinf = curIE->userrate * directval;
			break;
		default:
			_ASSERT( 0 );
			curIE->orginf = 0.0f;
			curIE->dispinf = 0.0f;
			break;
		}

		if( normflag ){
			Normalize();
		}

	}else if( paintmode == PAINT_EXC ){
		
		DestroyIE();

		INFELEM newIE;
		InitIE( &newIE );
		ret = AddInfElem( newIE );
		if( ret ){
			DbgOut( "infelem : SetInfElem : PAINT_EXC : AddInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		INFELEM* curIE;
		curIE = ie;
		_ASSERT( curIE );
	
//		curIE->childno = srcbi->childno;
//		curIE->bonematno = srcbi->jointno;
		curIE->childno = srcbi->childno;
		curIE->bonematno = srcbi->childno;
		curIE->kind = calcmode;
		curIE->userrate = rate;

		float brate;

		switch( curIE->kind ){
		case CALCMODE_ONESKIN0:
			CalcOneSkin0( srcbi, parselem, 0, &srcxv, &brate );
			curIE->orginf = brate;
			curIE->dispinf = curIE->userrate * brate;
			break;
		case CALCMODE_ONESKIN1:
			CalcOneSkin1( srcbi, &srcxv, &brate );
			curIE->orginf = brate;
			curIE->dispinf = curIE->userrate * brate;
			break;
		case CALCMODE_NOSKIN0:
			curIE->orginf = 1.0f;
			curIE->dispinf = 1.0f;
			break;
		case CALCMODE_DIRECT0:
			curIE->orginf = directval;
			curIE->dispinf = curIE->userrate * directval;
			break;
		default:
			_ASSERT( 0 );
			curIE->orginf = 0.0f;
			curIE->dispinf = 0.0f;
			break;
		}

		if( normflag ){
			Normalize();
		}
		

	}else if( paintmode == PAINT_ADD ){
		curIE = ExistINFELEM( srcbi->childno );
		if( !curIE ){
			if( infnum <= 3 ){
				INFELEM newIE;
				InitIE( &newIE );
				ret = AddInfElem( newIE );
				if( ret ){
					DbgOut( "infelem : SetInfElem : PAINT_NOR : AddInfElem error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				curIE = ie + infnum - 1;
				_ASSERT( curIE );
			}else{
				//ret 2 は、５個以上の影響度設定をしようとして失敗したとき。
				return 2;//!!!!!!!!!!!!!!!!!!!!!!
			}
		}
//		curIE->childno = srcbi->childno;
//		curIE->bonematno = srcbi->jointno;
		curIE->childno = srcbi->childno;
		curIE->bonematno = srcbi->childno;
		curIE->kind = calcmode;
		curIE->userrate += rate;//!!!!!!!!!!

		float brate;

		switch( curIE->kind ){
		case CALCMODE_ONESKIN0:
			CalcOneSkin0( srcbi, parselem, 0, &srcxv, &brate );
			curIE->orginf = brate;
			curIE->dispinf = curIE->userrate * brate;
			break;
		case CALCMODE_ONESKIN1:
			CalcOneSkin1( srcbi, &srcxv, &brate );
			curIE->orginf = brate;
			curIE->dispinf = curIE->userrate * brate;
			break;
		case CALCMODE_NOSKIN0:
			_ASSERT( 0 );
			// noskinは、排他で呼ばれるはず。

			curIE->orginf = 1.0f;
			curIE->dispinf = 1.0f;
			break;
		case CALCMODE_DIRECT0:
			_ASSERT( 0 );
			//directは、ADDされないはず。

			curIE->orginf = directval;
			curIE->dispinf = curIE->userrate * directval;
			break;
		default:
			_ASSERT( 0 );
			curIE->orginf = 0.0f;
			curIE->dispinf = 0.0f;
			break;
		}

		if( normflag ){
			Normalize();
		}

	}else if( paintmode == PAINT_SUB ){
		curIE = ExistINFELEM( srcbi->childno );
		if( !curIE ){
			if( infnum <= 3 ){
				INFELEM newIE;
				InitIE( &newIE );
				ret = AddInfElem( newIE );
				if( ret ){
					DbgOut( "infelem : SetInfElem : PAINT_NOR : AddInfElem error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				curIE = ie + infnum - 1;
				_ASSERT( curIE );
			}else{
				//ret 2 は、５個以上の影響度設定をしようとして失敗したとき。
				return 2;//!!!!!!!!!!!!!!!!!
			}
		}
//		curIE->childno = srcbi->childno;
//		curIE->bonematno = srcbi->jointno;
		curIE->childno = srcbi->childno;
		curIE->bonematno = srcbi->childno;
		curIE->kind = calcmode;
		curIE->userrate -= rate;//!!!!!!!!!!
		if( curIE->userrate < 0.0f )
			curIE->userrate = 0.0f;//!!!!!!!!

		float brate;

		switch( curIE->kind ){
		case CALCMODE_ONESKIN0:
			CalcOneSkin0( srcbi, parselem, 0, &srcxv, &brate );
			curIE->orginf = brate;
			curIE->dispinf = curIE->userrate * brate;
			break;
		case CALCMODE_ONESKIN1:
			CalcOneSkin1( srcbi, &srcxv, &brate );
			curIE->orginf = brate;
			curIE->dispinf = curIE->userrate * brate;
			break;
		case CALCMODE_NOSKIN0:
			_ASSERT( 0 );
			// noskinは、排他で呼ばれるはず。

			curIE->orginf = 1.0f;
			curIE->dispinf = 1.0f;
			break;
		case CALCMODE_DIRECT0:
			_ASSERT( 0 );
			//directは、SUBされないはず。

			curIE->orginf = directval;
			curIE->dispinf = curIE->userrate * directval;
			break;
		default:
			_ASSERT( 0 );
			curIE->orginf = 0.0f;
			curIE->dispinf = 0.0f;
			break;
		}

		if( normflag ){
			Normalize();
		}
	}else{
		_ASSERT( 0 );
		return 0;
	}

	DeleteDummyINFELEM();


	return 0;
}

int CInfElem::Normalize()
{
	int infno;
	float mag = 0.0f;

	INFELEM* curIE;
	for( infno = 0; infno < infnum; infno++ ){
		curIE = ie + infno;
		if( curIE->bonematno > 0 ){
			mag += curIE->userrate * curIE->orginf;
		}
	}
	
	float divval;
	if( mag != 0.0f ){
		divval = 1.0f / mag;
	}else{
		divval = 1.0f;
	}

	for( infno = 0; infno < infnum; infno++ ){
		curIE = ie + infno;
		curIE->dispinf = curIE->userrate * curIE->orginf * divval;
	}

	return 0;
}

int CInfElem::DeleteInfElem( int srcchildno, int normflag )
{
	int ret;
	int infno;
	int delindex = -1;
	INFELEM* curIE;
	for( infno = 0; infno < infnum; infno++ ){
		curIE = ie + infno;

		if( curIE->childno == srcchildno ){
			delindex = infno;
			break;
		}
	}

	if( delindex >= 0 ){
		ret = DeleteInfElemByIndex( delindex );
		if( ret ){
			DbgOut( "infelem : DeleteInfElem : DeleteInfElemByIndex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( normflag ){
		Normalize();
	}

	return 0;
}

int CInfElem::DeleteInfElemByIndex( int delindex )
{

	int infno;

	if( infnum <= 1 ){
		//ダミーにセットし直し。
		_ASSERT( ie );
		InitIE( ie );
		ie->bonematno = -1;
		ie->childno = -1;
		ie->orginf = 1.0f;
		ie->dispinf = 1.0f;

	}else{
		INFELEM* newIE;

		newIE = (INFELEM*)malloc( sizeof( INFELEM ) * (infnum - 1) );
		
		int setno = 0;
		for( infno = 0; infno < infnum; infno++ ){
			if( infno != delindex ){
				*(newIE + setno) = *(ie + infno);
				setno++;
			}
		}

		free( ie );
		ie = newIE;
		infnum--;

	}

	return 0;
}

int CInfElem::DeleteDummyINFELEM()
{
	if( infnum <= 1 )
		return 0;//!!!!!!

	//ダミーは1個だけとする。
	int delindex = -1;
	int infno;
	INFELEM* curIE;
	for( infno = 0; infno < infnum; infno++ ){
		curIE = ie + infno;
		if( curIE->bonematno <= 0 ){
			delindex = infno;
			break;
		}
	}
	int ret;
	if( delindex >= 0 ){
		ret = DeleteInfElemByIndex( delindex );
		if( ret ){
			DbgOut( "infelem : DeleteDummyINFELEM : DeleteInfElemByIndex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CInfElem::CheckInfluence( int srcjointno, float srccmpval )
{

	int infno;
	INFELEM* curIE;
	for( infno = 0; infno < infnum; infno++ ){
		curIE = ie + infno;

		if( (curIE->bonematno == srcjointno) && (curIE->dispinf >= srccmpval) ){
			return 1;//!!!!!!!!!
		}
	}
	return 0;
}

int CInfElem::CopyIE( CInfElem* srcie, SERICONV* sericonv, int serinum )
{
	int infno;
	if( sericonv ){
		int findcnt = 0;
		for( infno = 0; infno < srcie->infnum; infno++ ){
			INFELEM* srcIE;
			srcIE = srcie->ie + infno;

			int dstboneno = 0;
			int chkno;
			for( chkno = 0; chkno < serinum; chkno++ ){
				if( ( sericonv + chkno )->extseri == srcIE->bonematno ){
					dstboneno = ( sericonv + chkno )->seri;
					if( dstboneno >= 0 ){// 0も可
						findcnt++;
						break;
					}
				}
			}
		}
		if( findcnt != srcie->infnum ){
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!  対応ボーンが１つでもない場合は、コピーしない。
		}
	}



	if( ie ){
		free( ie );
	}
	ie = (INFELEM*)malloc( sizeof( INFELEM ) * srcie->infnum );
	infnum = srcie->infnum;
	normalizeflag = srcie->normalizeflag;
	symaxis = srcie->symaxis;
	symdist = srcie->symdist;

	for( infno = 0; infno < infnum; infno++ ){
		INFELEM* srcIE;
		srcIE = srcie->ie + infno;

		INFELEM* dstIE;
		dstIE = ie + infno;
		
		int dstboneno = 0;
		int chkno;
		if( sericonv ){
			for( chkno = 0; chkno < serinum; chkno++ ){
				if( ( sericonv + chkno )->extseri == srcIE->bonematno ){
					dstboneno = ( sericonv + chkno )->seri;
					break;
				}
			}
			//_ASSERT( dstboneno > 0 );
		}else{
			dstboneno = srcIE->bonematno;
		}


		dstIE->childno = dstboneno;
		dstIE->bonematno = dstboneno;
		dstIE->kind = srcIE->kind;
		dstIE->userrate = srcIE->userrate;
		dstIE->orginf = srcIE->orginf;
		dstIE->dispinf = srcIE->dispinf;
	}

	return 0;
}

int CInfElem::GetInfElemByBone( int srcboneno )
{
	int findid = -1;

	int ieno;
	for( ieno = 0; ieno < infnum; ieno++ ){
		INFELEM* curie = ie + ieno;
		if( curie->bonematno == srcboneno ){
			findid = ieno;
			break;
		}
	}
	return findid;
}


