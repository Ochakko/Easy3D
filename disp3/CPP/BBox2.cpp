#include "StdAfx.h"
#ifndef SHDTREEVIEWH
#include <windows.h>
#endif

#include <coef.h>
#include <BBox2.h>

#include <shdhandler.h>
#include <shdelem.h>
#include <polymesh.h>
#include <polymesh2.h>
#include <d3ddisp.h>
#include <basedat.h>

#include <matrix2.h>

#include <mothandler.h>
#include <motionctrl.h>

CBBox2::CBBox2()
{
	InitParams();
}

CBBox2::~CBBox2()
{
	DestroyObjs();
}

int CBBox2::InitParams()
{

	dispflag = 0;

//	dispflag = 1;//!!!!!!!!!

	applychild = 0;
	target = 0;
	polymesh2 = 0;
	d3ddisp = 0;
	pm2facet = 0.0f;

	ZeroMemory( &R0, sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &S0, sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &T0, sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &Q, sizeof( D3DXVECTOR3 ) );
	lengR0 = 0.0f;
	lengS0 = 0.0f;
	lengT0 = 0.0f;

	invindex = 0;
	ZeroMemory( pm2index, sizeof( int ) * 12 * 3 );


	ZeroMemory( m_vert, sizeof( D3DXVECTOR3 ) * 8 );
	ZeroMemory( m_transvert, sizeof( D3DXVECTOR3 ) * 8 );
//	ZeroMemory( m_rotvert, sizeof( D3DXVECTOR3 ) * 8 );

	int pcno;
	for( pcno = 0; pcno < 6; pcno++ ){
		m_pcoef[pcno].InitParams();
//		m_rotpcoef[pcno].InitParams();
	}

	int lno;
	for( lno = 0; lno < 12; lno++ ){
		m_line[lno].InitParams();
//		m_rotline[lno].InitParams();
	}

	m_bbx1.InitParams();


	return 0;

}
int CBBox2::DestroyObjs()
{
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}

	if( polymesh2 ){
		delete polymesh2;
		polymesh2 = 0;
	}

	return 0;
}

int CBBox2::InvalidateDispObj()
{
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}

	return 0;
}

int CBBox2::ResetPrimNum()
{
	if( d3ddisp ){
		d3ddisp->ResetPrimNum();
	}
	return 0;
}

int CBBox2::CalcInitialInfElem()
{
	int ret;

	int bonematno, childno;
	CShdElem* parelem;

	if( applychild ){
		childno = applychild->serialno;
		parelem = applychild->parent;
		if( parelem ){
			bonematno = parelem->serialno;
		}else{
			bonematno = 0;
		}
	}else{
		childno = 0;
		bonematno = 0;
	}
	

	_ASSERT( polymesh2 );
	ret = polymesh2->CalcInitialInfElem( bonematno, childno );
	if( ret ){
		DbgOut( "BBox2 : pm2 CalcInitialInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBBox2::CheckVertInScope( D3DXVECTOR3 srcv, int* insideptr )
{

/***
	int ret;

	ret = polymesh2->CheckVertInShape( srcv, insideptr );
	if( ret ){
		DbgOut( "is : CheckVertInScope : pm2 CheckVertInShape error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/

	return 0;
}


int CBBox2::SetBBox( CShdHandler* srclpsh, CShdElem* srctarget, CShdElem* srcjoint, int* vertarray, int vertnum )
{
	int ret;

//DbgOut( "check !!! bbox2 : SetBBox : %d %d\r\n", srctarget->serialno, srcjoint->serialno );


	DestroyObjs();//!!!!!!!!!!!!!!!!!


	applychild = srcjoint;
	target = srctarget;


	ret = srclpsh->GetBone3Vec( srcjoint->serialno, &R0, &S0, &T0 );
	if( ret ){
		DbgOut( "bbox2 : SetBBox : sh GetBone3Vec error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float minPR, maxPR, minPS, maxPS, minPT, maxPT;
	float inimin = 1e10;
	float inimax = -1e10;
	minPR = inimin;
	minPS = inimin;
	minPT = inimin;

	maxPR = inimax;
	maxPS = inimax;
	maxPT = inimax;


	//int vnum;
	VEC3F* targetv = 0;
	D3DTLVERTEX* targettlv = 0;

	if( srctarget->type == SHDPOLYMESH ){
		_ASSERT( srctarget->polymesh );
		//vnum = srctarget->polymesh->meshinfo->m;
		targetv = srctarget->polymesh->pointbuf;
	}else if( srctarget->type == SHDPOLYMESH2 ){
		_ASSERT( srctarget->polymesh2 );
		//vnum = srctarget->polymesh2->meshinfo->n * 3;
		//targetv = srctarget->polymesh2->pointbuf;
		targettlv = srctarget->polymesh2->opttlv;
	}

	int vindex, curvno;
	D3DXVECTOR3 curv;
	float curPR, curPS, curPT;
	for( vindex = 0; vindex < vertnum; vindex++ ){

		curvno = *( vertarray + vindex );

		if( targetv ){
			curv.x = ( targetv + curvno )->x;
			curv.y = ( targetv + curvno )->y;
			curv.z = ( targetv + curvno )->z;
		}else{
			curv.x = ( targettlv + curvno )->sx;
			curv.y = ( targettlv + curvno )->sy;
			curv.z = ( targettlv + curvno )->sz;
		}

		curPR = D3DXVec3Dot( &curv, &R0 );
		if( curPR < minPR ){
			minPR = curPR;
		}
		if( curPR > maxPR ){
			maxPR = curPR;
		}

		curPS = D3DXVec3Dot( &curv, &S0 );
		if( curPS < minPS ){
			minPS = curPS;
		}
		if( curPS > maxPS ){
			maxPS = curPS;
		}

		curPT = D3DXVec3Dot( &curv, &T0 );
		if( curPT < minPT ){
			minPT = curPT;
		}
		if( curPT > maxPT ){
			maxPT = curPT;
		}
	}

	float a, b, c;
	if( (minPR != inimin) && (maxPR != inimax) ){
		a = (minPR + maxPR) * 0.5f;
		lengR0 = (float)fabs( maxPR - minPR );
	}else{
		a = 0.0f;
		lengR0 = 0.0f;
		_ASSERT( 0 );
	}

	if( (minPS != inimin) && (maxPS != inimax) ){
		b = (minPS + maxPS) * 0.5f;
		lengS0 = (float)fabs( maxPS - minPS );
	}else{
		b = 0.0f;
		lengS0 = 0.0f;
		_ASSERT( 0 );
	}

	if( (minPT != inimin) && (maxPT != inimax) ){
		c = (minPT + maxPT) * 0.5f;
		lengT0 = (float)fabs( maxPT - minPT );
	}else{
		c = 0.0f;
		lengT0 = 0.0f;
		_ASSERT( 0 );
	}

	Q = a * R0 + b * S0 + c * T0;//!!!


//DbgOut( "check !!! bbox2 : SetBBox : %d %d : R %f, %f, %f, S %f, %f, %f, T %f, %f, %f\r\nlengR %f, lengS %f, lengT %f, Q %f, %f, %f\r\n",
//	srctarget->serialno, srcjoint->serialno,
//	R0.x, R0.y, R0.z,
//	S0.x, S0.y, S0.z,
//	T0.x, T0.y, T0.z,
//	lengR0, lengS0, lengT0,
//	Q.x, Q.y, Q.z );


	//////////

/***
	m_pcoef[0].SetPC( R0, -minPR );
	m_pcoef[1].SetPC( -R0, maxPR );
	m_pcoef[2].SetPC( S0, -minPS );
	m_pcoef[3].SetPC( -S0, maxPS );
	m_pcoef[4].SetPC( T0, -minPT );
	m_pcoef[5].SetPC( -T0, maxPT );

	int pcno;
	float onplane;
	for( pcno = 0; pcno < 6; pcno++ ){
		ret = m_pcoef[pcno].OnPlane( Q, &onplane );
		if( ret ){
			DbgOut( "bbox2 : SetBBox : pcoef OnPlane error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( onplane < 0.0f ){
			ret = m_pcoef[pcno].Inverse();
			if( ret ){
				DbgOut( "bbox2 : SetBBox : pcoef Inverse error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
***/

	//////////

	m_vert[0] = Q + 0.5f * lengR0 * R0 + 0.5f * lengS0 * S0 + 0.5f * lengT0 * T0;
	m_vert[1] = Q + 0.5f * lengR0 * R0 + 0.5f * lengS0 * S0 - 0.5f * lengT0 * T0;
	m_vert[2] = Q + 0.5f * lengR0 * R0 - 0.5f * lengS0 * S0 - 0.5f * lengT0 * T0;
	m_vert[3] = Q + 0.5f * lengR0 * R0 - 0.5f * lengS0 * S0 + 0.5f * lengT0 * T0;

	m_vert[4] = Q - 0.5f * lengR0 * R0 + 0.5f * lengS0 * S0 + 0.5f * lengT0 * T0;
	m_vert[5] = Q - 0.5f * lengR0 * R0 + 0.5f * lengS0 * S0 - 0.5f * lengT0 * T0;
	m_vert[6] = Q - 0.5f * lengR0 * R0 - 0.5f * lengS0 * S0 - 0.5f * lengT0 * T0;
	m_vert[7] = Q - 0.5f * lengR0 * R0 - 0.5f * lengS0 * S0 + 0.5f * lengT0 * T0;

	//////////

	int facenum = 12;
	int shdtype = SHDPOLYMESH2;
	CMeshInfo tempinfo;
	ret = tempinfo.ResetParams();
	_ASSERT( !ret );
	ret = tempinfo.SetMem( &shdtype, BASE_TYPE );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( facenum * 3, MESHI_M );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( facenum, MESHI_N );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 0, MESHI_TOTAL );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 1, MESHI_MCLOSED );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 0, MESHI_NCLOSED );
	_ASSERT( !ret );


	polymesh2 = new CPolyMesh2();
	if( !polymesh2 ){
		DbgOut( "bbox2 : SetBBox :  polymesh2 alloc error !!!\n" );
		return 1;
	}

	ret = polymesh2->CreateBuffers( &tempinfo );
	if( ret ){
		DbgOut( "bbox2 : SetBBox : pm2 CreateBuffers error !!!\n" );
		return 1;
	}

	//////////

	//面の向きチェック
	CPCoef tmpcp;
	tmpcp.SetPCFrom3V( &(m_vert[0]), &(m_vert[1]), &(m_vert[2]) );
	float tmponplane;
	ret = tmpcp.OnPlane( Q, &tmponplane );
	if( ret ){
		DbgOut( "bbox2 : SetBBox : tmpcp OnPlane error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	//int invindex = 0;
	if( tmponplane > 0.0 ){
		invindex = 1;
	}else{
		invindex = 0;
	}

	/////////////

	//int pm2index[12][3];
	if( invindex == 0 ){
		pm2index[0][0] = 0;
		pm2index[0][1] = 1;
		pm2index[0][2] = 2;

		pm2index[1][0] = 0;
		pm2index[1][1] = 2;
		pm2index[1][2] = 3;
		
		pm2index[2][0] = 1;
		pm2index[2][1] = 0;
		pm2index[2][2] = 4;
		
		pm2index[3][0] = 1;
		pm2index[3][1] = 4;
		pm2index[3][2] = 5;
		
		pm2index[4][0] = 3;
		pm2index[4][1] = 2;
		pm2index[4][2] = 6;
		
		pm2index[5][0] = 3;
		pm2index[5][1] = 6;
		pm2index[5][2] = 7;
		
		pm2index[6][0] = 7;
		pm2index[6][1] = 6;
		pm2index[6][2] = 5;
		
		pm2index[7][0] = 7;
		pm2index[7][1] = 5;
		pm2index[7][2] = 4;
		
		pm2index[8][0] = 0;
		pm2index[8][1] = 3;
		pm2index[8][2] = 7;
		
		pm2index[9][0] = 0;
		pm2index[9][1] = 7;
		pm2index[9][2] = 4;
		
		pm2index[10][0] = 2;
		pm2index[10][1] = 1;
		pm2index[10][2] = 6;
		
		pm2index[11][0] = 6;
		pm2index[11][1] = 1;
		pm2index[11][2] = 5;
	}else{
		pm2index[0][0] = 0;
		pm2index[0][1] = 2;
		pm2index[0][2] = 1;

		pm2index[1][0] = 0;
		pm2index[1][1] = 3;
		pm2index[1][2] = 2;
		
		pm2index[2][0] = 1;
		pm2index[2][1] = 4;
		pm2index[2][2] = 0;
		
		pm2index[3][0] = 1;
		pm2index[3][1] = 5;
		pm2index[3][2] = 4;
		
		pm2index[4][0] = 3;
		pm2index[4][1] = 6;
		pm2index[4][2] = 2;
		
		pm2index[5][0] = 3;
		pm2index[5][1] = 7;
		pm2index[5][2] = 6;
		
		pm2index[6][0] = 7;
		pm2index[6][1] = 5;
		pm2index[6][2] = 6;
		
		pm2index[7][0] = 7;
		pm2index[7][1] = 4;
		pm2index[7][2] = 5;
		
		pm2index[8][0] = 0;
		pm2index[8][1] = 7;
		pm2index[8][2] = 3;
		
		pm2index[9][0] = 0;
		pm2index[9][1] = 4;
		pm2index[9][2] = 7;
		
		pm2index[10][0] = 2;
		pm2index[10][1] = 6;
		pm2index[10][2] = 1;
		
		pm2index[11][0] = 6;
		pm2index[11][1] = 5;
		pm2index[11][2] = 1;
	}	

	VEC3F pbuf[36];
	int fno, pno;
	int setno = 0;
	for( fno = 0; fno < 12; fno++ ){
		for( pno = 0; pno < 3; pno++ ){
			pbuf[setno].x = m_vert[ pm2index[fno][pno] ].x;
			pbuf[setno].y = m_vert[ pm2index[fno][pno] ].y;
			pbuf[setno].z = m_vert[ pm2index[fno][pno] ].z;

			setno++;
		}
	}
	MoveMemory( polymesh2->pointbuf, pbuf, sizeof( VEC3F ) * facenum * 3 );

	ARGBF diffuse[12];
	ARGBF ambient[12];
	for( fno = 0; fno < 12; fno++ ){
		diffuse[fno].a = 0.5f;
		diffuse[fno].r = 0.0f;
		diffuse[fno].g = 0.0f;
		diffuse[fno].b = 1.0f;

		ambient[fno].a = 0.5f;
		ambient[fno].r = 0.0f;
		ambient[fno].g = 0.0f;
		ambient[fno].b = 0.25f;
	}
	MoveMemory( polymesh2->diffusebuf, diffuse, sizeof( ARGBF ) * 12 );
	MoveMemory( polymesh2->ambientbuf, ambient, sizeof( ARGBF ) * 12 );

///////////////
	ret = polymesh2->SetSamePoint();
	_ASSERT( !ret );

///////////////
	//polymesh2->m_mikoapplychild = childjoint;//!!!!!!!!
	//polymesh2->m_mikotarget = targetelem;//!!!!!!!!!!!!


	///////////



	return 0;
}

int CBBox2::TransformOnlyWorld( CMotHandler* srclpmh, D3DXMATRIX matWorld )
{
	
	CMatrix2 bonemat;
	D3DXMATRIX bonexmat;
	D3DXMATRIX savemat1;
	D3DXMATRIX* matSet1;

	D3DXMATRIX scalemat;
	_ASSERT( target->d3ddisp );
	scalemat = target->d3ddisp->m_scalemat;


	D3DXVECTOR3* srcv = m_vert;
	D3DXVECTOR3* dstv = m_transvert;//!!!!!!!!!!!!!!!!!!!!!	

	bonemat = (*srclpmh)( applychild->serialno )->curmat;
	float* srcdat = &(bonemat.data[0][0]);
		
	bonexmat._11 = *srcdat;
	bonexmat._12 = *(srcdat + 1);//[0][1];
	bonexmat._13 = *(srcdat + 2);//[0][2];
	bonexmat._14 = *(srcdat + 3);//[0][3];

	bonexmat._21 = *(srcdat + 4);//[1][0];
	bonexmat._22 = *(srcdat + 5);//[1][1];
	bonexmat._23 = *(srcdat + 6);//[1][2];
	bonexmat._24 = *(srcdat + 7);//[1][3];
		
	bonexmat._31 = *(srcdat + 8);//[2][0];
	bonexmat._32 = *(srcdat + 9);//[2][1];
	bonexmat._33 = *(srcdat + 10);//[2][2];

	bonexmat._34 = *(srcdat + 11);//[2][3];
	bonexmat._41 = *(srcdat + 12);//[3][0];
	bonexmat._42 = *(srcdat + 13);//[3][1];
	bonexmat._43 = *(srcdat + 14);//[3][2];
	bonexmat._44 = *(srcdat + 15);//[3][3];
		
	savemat1 = bonexmat * scalemat * matWorld;
	matSet1 = &savemat1;

	int vertno;
	for( vertno = 0; vertno < 8; vertno++ ){
		float x, y, z;
		x = srcv->x;
		y = srcv->y;
		z = srcv->z;

		float xpa, ypa, zpa;//, wpa;

		xpa = matSet1->_11*x + matSet1->_21*y + matSet1->_31*z + matSet1->_41;
		ypa = matSet1->_12*x + matSet1->_22*y + matSet1->_32*z + matSet1->_42;
		zpa = matSet1->_13*x + matSet1->_23*y + matSet1->_33*z + matSet1->_43;
		//wpa = matSet1->_14*x + matSet1->_24*y + matSet1->_34*z + matSet1->_44;
						
		dstv->x = xpa;
		dstv->y = ypa;
		dstv->z = zpa;

		srcv++;
		dstv++;
	}


	/////////////////

	int pcno;
	int selplane = 0;
	for( pcno = 0; pcno < 6; pcno++ ){
		selplane = pcno * 2;
		m_pcoef[pcno].SetPCFrom3V( m_transvert + pm2index[selplane][0], m_transvert + pm2index[selplane][1], m_transvert + pm2index[selplane][2] );
	}

	m_line[0].SetLineFrom2V( m_transvert + 0, m_transvert + 1 );
	m_line[1].SetLineFrom2V( m_transvert + 3, m_transvert + 2 );
	m_line[2].SetLineFrom2V( m_transvert + 7, m_transvert + 6 );
	m_line[3].SetLineFrom2V( m_transvert + 4, m_transvert + 5 );

	m_line[4].SetLineFrom2V( m_transvert + 0, m_transvert + 3 );
	m_line[5].SetLineFrom2V( m_transvert + 3, m_transvert + 7 );
	m_line[6].SetLineFrom2V( m_transvert + 7, m_transvert + 4 );
	m_line[7].SetLineFrom2V( m_transvert + 4, m_transvert + 0 );

	m_line[8].SetLineFrom2V( m_transvert + 1, m_transvert + 2 );
	m_line[9].SetLineFrom2V( m_transvert + 2, m_transvert + 6 );
	m_line[10].SetLineFrom2V( m_transvert + 6, m_transvert + 5 );
	m_line[11].SetLineFrom2V( m_transvert + 5, m_transvert + 1 );

	m_bbx1.InitParams();
	m_bbx1.SetBBox( this );


	return 0;
}



//-----------------------------------------------------------------------------
// Name: IntersectTriangle()
// Desc: Given a ray origin (orig) and direction (dir), and three vertices of
//       of a triangle, this function returns TRUE and the interpolated texture
//       coordinates if the ray intersects the triangle
//-----------------------------------------------------------------------------

int CBBox2::ChkConflict( CBBox2* chkbbx )
{
	int result0;
	D3DXVECTOR3 rate1( 1.0f, 1.0f, 1.0f );
	result0 = m_bbx1.ChkConflict( chkbbx->m_bbx1, rate1, rate1 );
	if( result0 == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!
	}


	//int ret;
	BOOL result1, result2, result3, result4;
	float t, u, v;

//線分と平面のあたり
	int lno, pcno;
	for( lno = 0; lno < 12; lno++ ){

		for( pcno = 0; pcno < 6; pcno++ ){

			result1 = IntersectTriangle(
				(m_line + lno)->P0, (m_line + lno)->V,
				chkbbx->m_transvert[ chkbbx->pm2index[pcno * 2][0] ], 
				chkbbx->m_transvert[ chkbbx->pm2index[pcno * 2][1] ], 
				chkbbx->m_transvert[ chkbbx->pm2index[pcno * 2][2] ], 
				&t, &u, &v );
			if( result1 != 0 ){
				return 1;//!!!!!!!!!
			}

			result2 = IntersectTriangle(
				(m_line + lno)->P0, (m_line + lno)->V,
				chkbbx->m_transvert[ chkbbx->pm2index[pcno * 2 + 1][0] ], 
				chkbbx->m_transvert[ chkbbx->pm2index[pcno * 2 + 1][1] ], 
				chkbbx->m_transvert[ chkbbx->pm2index[pcno * 2 + 1][2] ], 
				&t, &u, &v );
			if( result2 != 0 ){
				return 1;//!!!!!!!!!
			}

			result3 = IntersectTriangle(
				(chkbbx->m_line + lno)->P0, (chkbbx->m_line + lno)->V,
				m_transvert[ pm2index[pcno * 2][0] ],
				m_transvert[ pm2index[pcno * 2][1] ],
				m_transvert[ pm2index[pcno * 2][2] ],
				&t, &u, &v
				);
			if( result3 != 0 ){
				return 1;
			}

			result4 = IntersectTriangle(
				(chkbbx->m_line + lno)->P0, (chkbbx->m_line + lno)->V,
				m_transvert[ pm2index[pcno * 2 + 1][0] ],
				m_transvert[ pm2index[pcno * 2 + 1][1] ],
				m_transvert[ pm2index[pcno * 2 + 1][2] ],
				&t, &u, &v
				);
			if( result4 != 0 ){
				return 1;
			}


		}
	}

//頂点のボックス内外判定
	int vno3, pcno3, pcno4;
	float onplane3, onplane4;
	for( vno3 = 0; vno3 < 8; vno3++ ){
		onplane3 = 0.0f;
		for( pcno3 = 0; pcno3 < 6; pcno3++ ){
			chkbbx->m_pcoef[pcno3].OnPlane( m_transvert[ vno3 ], &onplane3 );
			if( onplane3 > 0.0f ){
				break;
			}
		}
		if( onplane3 <= 0.0f ){
			return 1;
		}

		onplane4 = 0.0f;
		for( pcno4 = 0; pcno4 < 6; pcno4++ ){
			m_pcoef[pcno4].OnPlane( chkbbx->m_transvert[ vno3 ], &onplane4 );
			if( onplane4 > 0.0f ){
				break;
			}
		}
		if( onplane4 <= 0.0f ){
			return 1;
		}

	}

	return 0;
}


/***
int CBBox2::ChkConflict( CBBox2* chkbbx )
{
	int result0;
	D3DXVECTOR3 rate1( 1.0f, 1.0f, 1.0f );
	result0 = m_bbx1.ChkConflict( chkbbx->m_bbx1, rate1, rate1 );
	if( result0 == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!
	}


	//int ret;
	BOOL result1, result2;
	float t, u, v;

//線分と平面のあたり
	int srclno, chkpcno;
	for( srclno = 0; srclno < 12; srclno++ ){

		for( chkpcno = 0; chkpcno < 6; chkpcno++ ){

			result1 = IntersectTriangle(
				(m_line + srclno)->P0, (m_line + srclno)->V,
				chkbbx->m_transvert[ chkbbx->pm2index[chkpcno * 2][0] ], 
				chkbbx->m_transvert[ chkbbx->pm2index[chkpcno * 2][1] ], 
				chkbbx->m_transvert[ chkbbx->pm2index[chkpcno * 2][2] ], 
				&t, &u, &v );
			if( result1 != 0 ){
				return 1;//!!!!!!!!!
			}

			result2 = IntersectTriangle(
				(m_line + srclno)->P0, (m_line + srclno)->V,
				chkbbx->m_transvert[ chkbbx->pm2index[chkpcno * 2 + 1][0] ], 
				chkbbx->m_transvert[ chkbbx->pm2index[chkpcno * 2 + 1][1] ], 
				chkbbx->m_transvert[ chkbbx->pm2index[chkpcno * 2 + 1][2] ], 
				&t, &u, &v );
			if( result2 != 0 ){
				return 1;//!!!!!!!!!
			}
		}
	}


	int chklno, srcpcno;
	for( chklno = 0; chklno < 12; chklno++ ){
		for( srcpcno = 0; srcpcno < 6; srcpcno++ ){

			result1 = IntersectTriangle(
				(chkbbx->m_line)->P0, (chkbbx->m_line)->V,
				m_transvert[ pm2index[srcpcno * 2][0] ],
				m_transvert[ pm2index[srcpcno * 2][1] ],
				m_transvert[ pm2index[srcpcno * 2][2] ],
				&t, &u, &v
				);
			if( result1 != 0 ){
				return 1;
			}

			result2 = IntersectTriangle(
				(chkbbx->m_line)->P0, (chkbbx->m_line)->V,
				m_transvert[ pm2index[srcpcno * 2 + 1][0] ],
				m_transvert[ pm2index[srcpcno * 2 + 1][1] ],
				m_transvert[ pm2index[srcpcno * 2 + 1][2] ],
				&t, &u, &v
				);
			if( result2 != 0 ){
				return 1;
			}

		}
	}

//頂点のボックス内外判定
	int srcvno3, chkpcno3;
	float onplane3 = 0.0f;
	for( srcvno3 = 0; srcvno3 < 8; srcvno3++ ){
		
		for( chkpcno3 = 0; chkpcno3 < 6; chkpcno3++ ){
			chkbbx->m_pcoef[chkpcno3].OnPlane( m_transvert[ srcvno3 ], &onplane3 );
			if( onplane3 > 0.0f ){
				break;
			}
		}

		if( onplane3 > 0.0f ){
			break;
		}
	}
	if( onplane3 <= 0.0f ){
		return 1;
	}


	int chkvno4, srcpcno4;
	float onplane4 = 0.0f;
	for( chkvno4 = 0; chkvno4 < 8; chkvno4++ ){
		for( srcpcno4 = 0; srcpcno4 < 6; srcpcno4++ ){
			m_pcoef[srcpcno4].OnPlane( chkbbx->m_transvert[ chkvno4 ], &onplane4 );
			if( onplane4 > 0.0f ){
				break;
			}
		}
		if( onplane4 > 0.0f ){
			break;
		}
	}
	if( onplane4 <= 0.0f ){
		return 1;
	}


	return 0;
}
***/

