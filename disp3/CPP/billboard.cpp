#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <shdhandler.h>
#include <shdelem.h>
#include <treehandler2.h>
#include <treeelem2.h>
#include <mothandler.h>
#include <motionctrl.h>
#include <d3ddisp.h>
#include <polymesh.h>
#include <billboard.h>
#include <billboardelem.h>

/// extern

extern int g_useGPU;

/// static
static D3DXVECTOR3 s_vDir;
static float s_difftime = 1.0f;

D3DXVECTOR3 CBillboard::s_campos( 0.0f, 0.0f, 0.0f );


//////////////

// Desc: Callback function for sorting trees in back-to-front order
//-----------------------------------------------------------------------------
int SortCB( const VOID* arg1, const VOID* arg2 )
{
    CBillboardElem* p1 = *((CBillboardElem**)arg1);
    CBillboardElem* p2 = *((CBillboardElem**)arg2);

    
	D3DXVECTOR3 vec1, vec2;
	vec1 = p1->pos - CBillboard::s_campos;
	vec2 = p2->pos - CBillboard::s_campos;

	float d1, d2;

	d1 = D3DXVec3Dot( &vec1, &s_vDir );
	d2 = D3DXVec3Dot( &vec2, &s_vDir );

	if( d1 < d2 ){
		return 1;
	}else if( d1 == d2 ){
		return 0;
	}else{
		return -1;
	}

/***
    float d1 = p1->pos.x * s_vDir.x + p1->pos.z * s_vDir.z;
    float d2 = p2->pos.x * s_vDir.x + p2->pos.z * s_vDir.z;

    if (d1 < d2)
        return +1;

    return -1;
***/
}



CBillboard::CBillboard()
{
	InitParams();

}
CBillboard::~CBillboard()
{
	DestroyObjs();
}

void CBillboard::InitParams()
{
	CBaseDat::InitParams();

	meshinfo = 0;
	bbarray = 0;
	disparray = 0;
	dispnum = 0;

	cmpalways = 0;

	InitParticleParams0();

}

void CBillboard::InitParticleParams0()
{
	emitnum = 0.0f;
	femittotal = 0.0f;
	iemittotal = 0;

	particlepos.x = 0.0f;
	particlepos.y = 0.0f;
	particlepos.z = 0.0f;

	gravity = 0.0f;

	life = 0.0f;//[sec]
	time = 0.0f;//[sec]

	ZeroMemory( &minvel, sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &maxvel, sizeof( D3DXVECTOR3 ) );

	time0alpha = 1.0f;
	time0uv_unum = 1;
	time0uv_vnum = 1;
	time0uvtile = 0;
}


void CBillboard::DestroyObjs()
{
	CBaseDat::DestroyObjs();

	if( meshinfo ){
		delete meshinfo;
		meshinfo = 0;
	}

	if( bbarray ){
		delete [] bbarray;
		bbarray = 0;
	}

	if( disparray ){
		//delete [] disparray;
		free( disparray );
		disparray = 0;
	}


	InitParams();
}

int CBillboard::FindUnusedElem()
{
	int retno = -1;

	int elemno;
	CBillboardElem* curelem;
	for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
		curelem = bbarray + elemno;

		if( curelem->useflag == 0 ){
			retno = elemno;
			break;
		}
	}

	return retno;
}

int CBillboard::SetSElemPtr( CShdElem* srcselem, float srcR )
{

	int newelemno;

	newelemno = FindUnusedElem();
	if( newelemno < 0 ){
		DbgOut( "billboard : SetSElemPtr : bbarray overflow error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBillboardElem* newelem;
	newelem = bbarray + newelemno;

	newelem->selem = srcselem;

	newelem->pos.x = 0.0f;
	newelem->pos.y = 0.0f;
	newelem->pos.z = 0.0f;

	newelem->dwOffset = 0;

	newelem->useflag = 1;
	newelem->dispflag = 1;

	newelem->particleR = srcR;

	return 0;
}

int CBillboard::SortElem( D3DXVECTOR3 vDir )
{
	s_vDir = vDir;

	dispnum = 0;//!!!!!!

	int elemno;
	for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
		CBillboardElem* curelem;
		curelem = bbarray + elemno;

		CShdElem* curselem;
		curselem = curelem->selem;

		if( (curelem->useflag == 1) && (curelem->dispflag == 1) &&
			(curselem->curbs.visibleflag == 1) && (curselem->notuse == 0)
		){
			*(disparray + dispnum) = curelem;
			dispnum++;
		}
	}

	qsort( disparray, dispnum, sizeof( CBillboardElem* ), SortCB );

//DbgOut( "check!!!, dispnum %d\r\n", dispnum );

	//qsort( bbarray, MAX_BILLBOARD_NUM, sizeof( CBillboardElem ), SortCB );	

	return 0;
}

int CBillboard::CreateBuffers( CMeshInfo* srcmeshinfo )
{

//DbgOut( "billboard : CreateBuffers : m %d, n %d, total %d\r\n", srcmeshinfo->m, srcmeshinfo->n, srcmeshinfo->total );

	int ret;
	// meshinfo
	ret = srcmeshinfo->NewMeshInfo( &meshinfo );
	if( ret ){
		DbgOut( "billboard : CreateBuffers ; srcmeshinfo->NewMeshinfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	_ASSERT( (meshinfo->type > SHDTYPENONE) && (meshinfo->type < SHDTYPEMAX) );

	ret = CBaseDat::SetType( meshinfo->type );
	if( ret ){
		DbgOut( "billboard : CreateBuffers : base::SetType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////

	bbarray = new CBillboardElem[ MAX_BILLBOARD_NUM ];
	if( !bbarray ){
		DbgOut( "billboard : CreateBuffers bbarray alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	//disparray = new CBillboardElem[ MAX_BILLBOARD_NUM ];
	disparray = (CBillboardElem**)malloc( sizeof( CBillboardElem* ) * MAX_BILLBOARD_NUM );
	if( !disparray ){
		DbgOut( "billboard : CreateBuffers disparray alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int bbelemno;
	for( bbelemno = 0; bbelemno < MAX_BILLBOARD_NUM; bbelemno++ ){
		(bbarray + bbelemno)->InitParams();
		//(disparray + bbelemno)->InitParams();
	}
	ZeroMemory( disparray, sizeof( CBillboardElem* ) * MAX_BILLBOARD_NUM );

	dispnum = 0;

	return 0;
}

int CBillboard::SetBillboardPos( int bbid, float posx, float posy, float posz )
{
	CBillboardElem* setelem;

	setelem = FindBillboardByID( bbid );
	if( !setelem ){
		DbgOut( "billboard : SetBillboardPos : FindBillboardByID return NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	setelem->pos.x = posx;
	setelem->pos.y = posy;
	setelem->pos.z = posz;

	return 0;
}

int CBillboard::RotateBillboard( int bbid, float fdeg, int rotkind )
{
	int ret;

	if( bbid < 0 ){
		int elemno;
		CBillboardElem* curelem;
		CShdElem* curselem;
		int curbbid;

		for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
			curelem = bbarray + elemno;

			if( curelem ){
				curselem = curelem->selem;
				if( curselem && (curelem->useflag == 1) && (curselem->notuse == 0) ){
					curbbid = curselem->serialno;
					ret = RotateBillboard( curbbid, fdeg, rotkind );
					if( ret ){
						DbgOut( "billboard : RotateBillboard %d error !!!\n", curbbid );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}

	}else{

		CBillboardElem* setelem;

		setelem = FindBillboardByID( bbid );
		if( !setelem ){
			DbgOut( "billboard : RotateBillboard : FindBillboardByID return NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = setelem->Rotate( fdeg, rotkind );
		if( ret ){
			DbgOut( "billboard : RotateBillboard : bbelem Rotate error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}	

int CBillboard::SetBillboardSize( int bbid, float srcw, float srch, int dirmode, int orgflag )
{
	int ret;
	CBillboardElem* setelem;
	setelem = FindBillboardByID( bbid );
	if( !setelem ){
		DbgOut( "billboard : SetBillboardSize : FindBillboardByID return NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = setelem->selem;
	if( !selem ){
		DbgOut( "billboard : SetBillboardSize : selem NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( selem->type != SHDPOLYMESH ){
		DbgOut( "billboard : SetBillboardSize : not polymesh error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CPolyMesh* pm;
	pm = selem->polymesh;
	if( !pm ){
		DbgOut( "billboard : SetBillboardSize : polymesh NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	setelem->particleR = max( srcw, srch );

	ret = pm->SetBillboardPoints( srcw, srch, orgflag );
	if( ret ){
		DbgOut( "billboard : SetBillboardSize : pm SetBillboardPoints error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	selem->m_bbdirmode = dirmode;//!!!!!!!!!!!
	selem->curbs.SetBSFromVEC3F( pm->pointbuf, 4 );
	
	CD3DDisp* d3ddisp;
	d3ddisp = selem->d3ddisp;
	if( d3ddisp ){
		ret = d3ddisp->SetPolyMeshPos( pm );
		if( ret ){
			DbgOut( "billboard : SetBillboardSize : d3ddisp SetPolyMeshPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = d3ddisp->Copy2VertexBuffer( 0, 0 );
		if( ret ){
			DbgOut( "billboard : SetBillboardSize : d3ddisp Copy2VertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}


int CBillboard::SetBillboardUV( int bbid, int unum, int vnum, int texno, int revuflag )
{
	int ret;

	CBillboardElem* setelem;
	setelem = FindBillboardByID( bbid );
	if( !setelem ){
		DbgOut( "billboard : SetBillboardUV : FindBillboardByID return NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = setelem->selem;
	if( !selem ){
		DbgOut( "billboard : SetBillboardUV : selem NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( selem->type != SHDPOLYMESH ){
		DbgOut( "billboard : SetBillboardUV : not polymesh error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CPolyMesh* pm;
	pm = selem->polymesh;
	if( !pm ){
		DbgOut( "billboard : SetBillboardUV : polymesh NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( pm->uvbuf ){
		ret = pm->SetBillboardUV( unum, vnum, texno, revuflag );
		if( ret ){
			DbgOut( "billboard : SetBillboardUV : pm SetBillboardUV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = pm->CreateTextureBuffer();
		if( ret ){
			DbgOut( "billboard : SetBillboardUV : pm CreateTextureBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!

		ret = pm->SetBillboardUV( unum, vnum, texno, revuflag );
		if( ret ){
			DbgOut( "billboard : SetBillboardUV : pm SetBillboardUV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//////
	CD3DDisp* d3ddisp;
	d3ddisp = selem->d3ddisp;
	if( !d3ddisp ){
		DbgOut( "billboard : SetBillboardUV : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = d3ddisp->SetUV( selem->type, selem->texrule, 0, pm, -1 );
	if( ret ){
		DbgOut( "billboard : SetBillboardUV : d3ddisp SetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_useGPU ){
		ret = d3ddisp->CopyUV2VertexBuffer( -1 );
		if( ret ){
			DbgOut( "billboard : SetBillboardUV: d3ddisp CopyUV2VertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	return 0;
}



int CBillboard::SetBillboardDispFlag( int bbid, int flag )
{
	CBillboardElem* setelem;

	setelem = FindBillboardByID( bbid );
	if( !setelem ){
		DbgOut( "billboard : SetBillboardDispFlag : FindBillboardByID return NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( flag != 0 )
		flag = 1; //!!!!!!!!

	setelem->dispflag = flag;


	return 0;
}

int CBillboard::DestroyBillboard( int bbid, CTreeHandler2* lpth, CMotHandler* lpmh )
{
	CBillboardElem* setelem;
	int delseri;

	if( bbid >= 0 ){
		setelem = FindBillboardByID( bbid );
		if( !setelem ){
			DbgOut( "billboard : DestroyBillboard : FindBillboardByID return NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;
		}
		
		setelem->useflag = 0;//!!!!!!!!!!

		if( setelem->selem ){
			delseri = setelem->selem->serialno;
			setelem->selem->notuse = 1;		

			setelem->selem->DestroyObjs();
			setelem->selem->type = SHDDESTROYED;//!!!!!!!!!!!!!!!!!!

			CMotionCtrl* delmc2;
			delmc2 = (*lpmh)( delseri );
			_ASSERT( delmc2 );
			delmc2->type = SHDDESTROYED;//!!!!!!!!!!!!!!


			CTreeElem2* delte2;
			delte2 = (*lpth)( delseri );
			_ASSERT( delte2 );
			delte2->type = SHDDESTROYED;//!!!!!!!!!!!!!!!
			

			if( setelem->selem->texname ){
				free( setelem->selem->texname );
				setelem->selem->texname = 0;
			}
			setelem->selem->transparent = 0;
		}
		setelem->InitParams();
		
	}else{
		//bbid が負の場合は、全てのbillboardelemをリセットする。

		int bbelemno;
		for( bbelemno = 0; bbelemno < MAX_BILLBOARD_NUM; bbelemno++ ){
			if( (bbarray + bbelemno)->selem ){
				(bbarray + bbelemno)->useflag = 0;//!!!!!!!!
				(bbarray + bbelemno)->selem->notuse = 1;

				delseri = (bbarray + bbelemno)->selem->serialno;
				(bbarray + bbelemno)->selem->DestroyObjs();
				(bbarray + bbelemno)->selem->type = SHDDESTROYED;//!!!!!!!!!!!!!!!!!!

				CMotionCtrl* delmc2;
				delmc2 = (*lpmh)( delseri );
				_ASSERT( delmc2 );
				delmc2->type = SHDDESTROYED;//!!!!!!!!!!!!!!

				CTreeElem2* delte2;
				delte2 = (*lpth)( delseri );
				_ASSERT( delte2 );
				delte2->type = SHDDESTROYED;//!!!!!!!!!!!!!!!

				if( (bbarray + bbelemno)->selem->texname ){
					free( (bbarray + bbelemno)->selem->texname );
					(bbarray + bbelemno)->selem->texname = 0;
				}
				(bbarray + bbelemno)->selem->transparent = 0;
			}


			(bbarray + bbelemno)->InitParams();
			//(disparray + bbelemno)->InitParams();
			*(disparray + bbelemno) = 0;
		}
		dispnum = 0;
	}

	return 0;
}



CBillboardElem* CBillboard::FindBillboardByID( int bbid )
{
	CBillboardElem* retelem = 0;

	int elemno;
	CBillboardElem* chkelem;
	CShdElem* chkselem;
	for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
		chkelem = bbarray + elemno;
		if( !chkelem ){
			DbgOut( "billboard : FindBillboardByID : chkelem error !!!\n" );
			_ASSERT( 0 );
			return 0;
		}

		chkselem = chkelem->selem;
		//if( !chkselem ){
		//	DbgOut( "billboard : FindBillboardByID : chkselem error !!!\n" );
		//	_ASSERT( 0 );
		//	return 0;
		//}
		if( !chkselem )
			continue;


		if( (chkelem->useflag == 1) && (chkselem->serialno == bbid) ){
			retelem = chkelem;
			break;
		}
	}

	return retelem;
}

int CBillboard::SetParticlePos( D3DXVECTOR3 srcpos )
{
	particlepos = srcpos;

	return 0;
}
int CBillboard::SetParticleGravity( float srcgravity )
{
	gravity = srcgravity;

	return 0;
}

int CBillboard::SetParticleLife( float srclife )
{
	life = srclife;
	return 0;
}
int CBillboard::SetParticleEmitNum( float srcemitnum )
{
	emitnum = srcemitnum;
	return 0;
}
int CBillboard::SetParticleVel0( D3DXVECTOR3 srcminvel, D3DXVECTOR3 srcmaxvel )
{
	minvel = srcminvel;
	maxvel = srcmaxvel;
	return 0;
}

int CBillboard::UpdateParticle( CShdHandler* lpsh, int srcfps )
{
	
	//float difftime;
	if( srcfps != 0 ){
		s_difftime = 1.0f / (float)srcfps;
	}else{
		s_difftime = 1.0f / 60.0f;
	}
	time += s_difftime;


//lifeを過ぎたものをnotuseにする。
	int elemno;
	CBillboardElem* curbb;
	CShdElem* curselem;
	for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
		curbb = bbarray + elemno;
		if( curbb && (curbb->useflag != 0) ){
			curselem = curbb->selem;
			if( curselem && (curselem->notuse == 0) ){
				if( (time - curbb->createtime) >= life ){
					curselem->notuse = 1;
				}
			}
		}
	}
//有効なものの位置を進める
	for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
		curbb = bbarray + elemno;
		if( curbb && (curbb->useflag != 0) ){
			curselem = curbb->selem;
			if( curselem && (curselem->notuse == 0) ){
				curbb->befpos = curbb->pos;
				float movetime;
				movetime = time - curbb->createtime;
				curbb->pos = curbb->pos0 + curbb->vel0 * movetime;
				curbb->pos.y -= gravity * movetime * movetime;
			}
		}
	}
	

//emitnum個だけ、新たに有効にする
	femittotal += emitnum;

	int ret;
	int newnum;
	newnum = (int)femittotal - iemittotal;
	iemittotal += newnum;


//DbgOut( "bb : UpdateParticle : newnum %d, time %f\r\n", newnum, time );

	int emitno;
	for( emitno = 0; emitno < newnum; emitno++ ){
		curbb = GetNotUseElem();
		if( curbb ){
			curbb->selem->notuse = 0;

			curbb->pos = particlepos;
			curbb->pos0 = particlepos;
			curbb->befpos = particlepos;
			curbb->createtime = time;
			
			if( curbb->selem ){
				ret = lpsh->SetAlpha( time0alpha, curbb->selem->serialno, 1 );
				if( ret ){
					DbgOut( "bb SetParticleAlpha : sh SetAlpha error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			
				curbb->m_textileno = time0uvtile;
				ret = lpsh->SetUVTile( curbb->selem->serialno, 2, time0uv_unum, time0uv_vnum, time0uvtile );
				if( ret ){
					DbgOut( "bb UpdateParticle : sh SetUVTile error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}


			D3DXVECTOR3 diffv;
			diffv = maxvel - minvel;
			int idiffx, idiffy, idiffz;
			/***
			idiffx = (int)( diffv.x * 10.0f );
			idiffy = (int)( diffv.y * 10.0f );
			idiffz = (int)( diffv.z * 10.0f );

			float addvx, addvy, addvz;

			if( idiffx > 0 ){
				addvx = (float)( rand() % idiffx ) * 0.1f;
			}else{
				addvx = 0.0f;
			}
			if( idiffy > 0 ){
				addvy = (float)( rand() % idiffy ) * 0.1f;
			}else{
				addvy = 0.0f;
			}
			if( idiffz > 0 ){
				addvz = (float)( rand() % idiffz ) * 0.1f;
			}else{
				addvz = 0.0f;
			}
			***/
			idiffx = (int)( diffv.x );
			idiffy = (int)( diffv.y );
			idiffz = (int)( diffv.z );

			float addvx, addvy, addvz;

			if( idiffx > 0 ){
				addvx = (float)( rand() % idiffx );
			}else{
				addvx = 0.0f;
			}
			if( idiffy > 0 ){
				addvy = (float)( rand() % idiffy );
			}else{
				addvy = 0.0f;
			}
			if( idiffz > 0 ){
				addvz = (float)( rand() % idiffz );
			}else{
				addvz = 0.0f;
			}

			curbb->vel0.x = minvel.x + addvx;
			curbb->vel0.y = minvel.y + addvy;
			curbb->vel0.z = minvel.z + addvz;

		}
	}

	return 0;
}

CBillboardElem* CBillboard::GetNotUseElem()
{
	CBillboardElem* retbb = 0;

	int elemno;
	CBillboardElem* curbb;
	CShdElem* curselem;
	for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
		curbb = bbarray + elemno;
		if( curbb && (curbb->useflag != 0) ){
			curselem = curbb->selem;
			if( curselem && (curselem->notuse == 1) ){
				retbb = curbb;
				break;
			}
		}
	}

	return retbb;
}

int CBillboard::SetParticleAlpha( CShdHandler* lpsh, float srcmintime, float srcmaxtime, float srcalpha )
{
	int ret;
	int elemno;
	CBillboardElem* curbb;
	CShdElem* curselem;
	for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
		curbb = bbarray + elemno;
		if( curbb && (curbb->useflag != 0) ){
			curselem = curbb->selem;
			if( curselem && (curselem->notuse == 0) ){
				float movetime;
				movetime = time - curbb->createtime;

				if( (movetime >= srcmintime) && (movetime <= srcmaxtime) ){
					ret = lpsh->SetAlpha( srcalpha, curselem->serialno, 1 );
					if( ret ){
						DbgOut( "bb SetParticleAlpha : sh SetAlpha error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}
	}

	if( srcmintime == 0.0f ){
		time0alpha = srcalpha;
	}

	return 0;
}
int CBillboard::SetParticleUVTile( CShdHandler* lpsh, float srcmintime, float srcmaxtime, int srcunum, int srcvnum, int srctileno )
{
	int ret;
	int elemno;
	CBillboardElem* curbb;
	CShdElem* curselem;
	for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
		curbb = bbarray + elemno;
		if( curbb && (curbb->useflag != 0) ){
			curselem = curbb->selem;
			if( curselem && (curselem->notuse == 0) ){
				float movetime;
				movetime = time - curbb->createtime;
				if( (movetime >= srcmintime) && (movetime <= srcmaxtime) ){
					if( curbb->m_textileno != srctileno ){
						curbb->m_textileno = srctileno;
						ret = lpsh->SetUVTile( curselem->serialno, 2, srcunum, srcvnum, srctileno );
						if( ret ){
							DbgOut( "bb SetParticleUVTile : sh SetUVTile error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}
			}
		}
	}

	if( srcmintime == 0.0f ){
		time0uv_unum = srcunum;
		time0uv_vnum = srcvnum;
		time0uvtile = srctileno;
	}


	return 0;
}

int CBillboard::InitParticle()
{

	int elemno;
	CBillboardElem* curbb;
	CShdElem* curselem;
	for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
		curbb = bbarray + elemno;
		if( curbb && (curbb->useflag != 0) ){
			curselem = curbb->selem;
			if( curselem ){
				curselem->notuse = 1;
				curbb->pos = curbb->pos0;
				curbb->befpos = curbb->pos0;
			}
		}
	}

	return 0;
}

int CBillboard::ChkConfParticle( CBSphere* chkbs, float srcrate )
{
	int flag = 0;
	int elemno;
	CBillboardElem* curbb;
	CShdElem* curselem;
	for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
		curbb = bbarray + elemno;
		if( curbb && (curbb->useflag != 0) ){
			curselem = curbb->selem;
			if( curselem && (curselem->notuse == 0) ){
				flag = chkbs->ChkConfMoveSphere( curbb->befpos, curbb->pos, curbb->particleR, srcrate );
				if( flag ){
					return 1;
				}
			}
		}
	}

	return 0;
}


