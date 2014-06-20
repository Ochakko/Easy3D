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

#include <grounddata.h>


CGroundData::CGroundData()
{
	InitParams();

}

int CGroundData::CreateBuffers( CMeshInfo* srcmeshinfo )
{
	int ret;
	// meshinfo
	ret = srcmeshinfo->NewMeshInfo( &meshinfo );
	if( ret ){
		DbgOut( "GroundData : CreateBuffers ; srcmeshinfo->NewMeshinfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	_ASSERT( (meshinfo->type > SHDTYPENONE) && (meshinfo->type < SHDTYPEMAX) );

	ret = CBaseDat::SetType( meshinfo->type );
	if( ret ){
		DbgOut( "GroundData : CreateBuffers : base::SetType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//_ASSERT( meshinfo->skip == -1 );


	if( meshinfo->m != 0 ){
		/////////////////
		m_divx = meshinfo->m;
		m_divz = meshinfo->n;
	/////////
		int xpartnum, xpartrest, xtotal;
		int zpartnum, zpartrest, ztotal;

		if( meshinfo->skip != -1 ){
			xpartnum = m_divx / m_bulkx;
			xpartrest = m_divx - (xpartnum * m_bulkx);
			if( xpartrest == 0 )
				xtotal = xpartnum;
			else
				xtotal = xpartnum + 1;

			zpartnum = m_divz / m_bulkz;
			zpartrest = m_divz - (xpartnum * m_bulkz);
			if( zpartrest == 0 )
				ztotal = zpartnum;
			else
				ztotal = zpartnum + 1;

			m_pmnum = xtotal * ztotal;//!!!!
		}else{
			xpartnum = 1;
			xpartrest = 0;
			xtotal = 1;

			zpartnum = 1;
			zpartrest = 0;
			ztotal = 1;

			m_pmnum = 1;
		}

	/////////
		m_pmarray = (CPolyMesh**)malloc( sizeof( CPolyMesh* ) * m_pmnum );
		if( !m_pmarray ){
			DbgOut( "GroundData : CreateBuffers : m_pmarray alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( m_pmarray, sizeof( CPolyMesh* ) * m_pmnum );


		m_rect = (RECT*)malloc( sizeof( RECT ) * m_pmnum );
		if( !m_rect ){
			DbgOut( "GroundData : CreateBuffers : m_rect alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( m_rect, sizeof( RECT ) * m_pmnum );

	/////////
		if( meshinfo->skip != -1 ){

			int xcnt, zcnt;
			int setcnt = 0;
			int startx, endx;
			int startz, endz;
			RECT* currect;
			for( zcnt = 0; zcnt < ztotal; zcnt++ ){
				startz = zcnt * m_bulkz;
				if( zcnt < zpartnum )
					endz = (zcnt + 1) * m_bulkz;
				else
					endz = zcnt * m_bulkz + zpartrest;

				for( xcnt = 0; xcnt < xtotal; xcnt++ ){
					startx = xcnt * m_bulkx;
					if( xcnt < xpartnum )
						endx = (xcnt + 1) * m_bulkx;
					else
						endx = xcnt * m_bulkx + xpartrest;

					currect = m_rect + setcnt;

					currect->left = startx;
					currect->right = endx;

					currect->top = endz;
					currect->bottom = startz;

					setcnt++;
				}

			}

			if( setcnt != m_pmnum ){
				DbgOut( "grounddata : CreateBuffers : setcnt error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			m_rect->left = 0;
			m_rect->right = m_divx;
			m_rect->top = m_divz;
			m_rect->bottom = 0;

			_ASSERT( m_pmnum == 1 );
		}
	}else{
		// meshinfo->m == 0 は、n に	polymesh2 の個数が入っていることを示す。

		//polymesh2 のときは、何もしない。
		m_pmnum = 0;
	}

	//m_rect->left = 0;
	//m_rect->top = m_divz;
	//m_rect->right = m_divx;
	//m_rect->bottom = 0;

//////////


	return 0;
}

void CGroundData::InitParams()
{
	CBaseDat::InitParams();

	meshinfo = 0;

	ZeroMemory( m_gbmpname, MAX_PATH );
	m_divx = 2; //分割数。0から始まるナンバー。線の本数 - 1
	m_divz = 2; //分割数。0から始まるナンバー。線の本数 - 1

	m_bulkx = 20;
	m_bulkz = 20;


	m_pmnum = 0;
	m_pmarray = 0;

	m_rect = 0;

	m_bf = 0;
	m_hitpoint = 0;
	m_indexbuf = 0;

}

CGroundData::~CGroundData()
{
	DestroyObjs();
}

void CGroundData::DestroyObjs()
{
	CBaseDat::DestroyObjs();

	if( meshinfo ){
		delete meshinfo;
		meshinfo = 0;
	}

	if( m_rect ){
		free( m_rect );
		m_rect = 0;
	}

	if( m_pmarray ){
		free( m_pmarray );
		m_pmarray = 0;
		//pmarrayの内容のfreeは、行わない。
	}

	DestroyInvisibleWallObj();

	InitParams();

}

int CGroundData::SetPolyMeshPtr( int pmno, CPolyMesh* srcpm )
{
	if( (pmno < 0) || (pmno >= m_pmnum) ){
		DbgOut( "grounddata : SetPolyMeshPtr : pmno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !m_pmarray ){
		DbgOut( "grounddata : SetPolyMeshPtr : pmarray error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*(m_pmarray + pmno) = srcpm;

	return 0;
}


int CGroundData::CreateInvisibleWallObj( float* ntscy, float miny, float maxy, int bmpw, int bmph, float maxx, float maxz )
{
	int ret = 0;
	int* dirtyp = 0;

	CBorderFace* curbf;
	//CBorderFace samebf;
	CBorderFace* samebf;

	CBorderFace* newbf = 0;

	//CBorderFace* commonbf1;
	//CBorderFace* commonbf2;

	int faceno;
	int facenum, indexnum;

	if( !ntscy ){
		DbgOut( "grounddata : CreateInvisibleWallObj : ntscy error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_hitpoint = (int*)malloc( sizeof( int ) * (m_divx + 1) * (m_divz + 1) );
	if( !m_hitpoint ){
		DbgOut( "grounddata : CreateInvisibleWallObj : m_hitpoint alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto createiwexit;
	}
	ZeroMemory( m_hitpoint, sizeof( int ) * (m_divx + 1) * (m_divz + 1) );


	facenum = m_divx * m_divz * 2;
	indexnum = facenum * 3;


	m_indexbuf = (int*)malloc( sizeof( int ) * m_divx * m_divz * 2 * 3 );
	if( !m_indexbuf ){
		DbgOut( "grounddata : CreateInvisibleWallObj : m_indexbuf alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto createiwexit;
	}
	faceno = 0;
	int xno, zno;
	for( zno = 0; zno < m_divz; zno++ ){
		for( xno = 0; xno < m_divx; xno++ ){
			*(m_indexbuf + faceno * 3) = zno * (m_divx + 1) + xno;
			*(m_indexbuf + faceno * 3 + 1) = (zno + 1) * (m_divx + 1) + xno;
			*(m_indexbuf + faceno * 3 + 2) = (zno + 1) * (m_divx + 1) + xno + 1;

			faceno++;

			*(m_indexbuf + faceno * 3) = zno * (m_divx + 1) + xno;
			*(m_indexbuf + faceno * 3 + 1) = (zno + 1) * (m_divx + 1) + xno + 1;
			*(m_indexbuf + faceno * 3 + 2) = zno * (m_divx + 1) + xno + 1;

			faceno++;
		}
	}
	if( faceno != (m_divx * m_divz * 2) ){
		DbgOut( "grounddata : CreateInvisibleWallObj : m_indexbuf faceno error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto createiwexit;
	}



/// hitpoint のセット
	float bmpstepx, bmpstepz;
	_ASSERT( (m_divx > 0) && (m_divz > 0) );
	bmpstepx = (float)(bmpw - 1) / (float)m_divx;
	bmpstepz = (float)(bmph - 1) / (float)m_divz;

	float curntscy;
	float threshy;
	threshy = miny + (maxy - miny) * 0.3f;


	int indexz, indexx;
	int curz, curx;
	//float curposz, curposx;


	for( indexz = 0; indexz <= m_divz; indexz++ ){
		curz = (int)(bmpstepz * (float)indexz);
		//curposz = posstepz * (float)indexz;

		for( indexx = 0; indexx <= m_divx; indexx++ ){
			curx = (int)(bmpstepx * (float)indexx);
			//curposx = posstepx * (float)indexx;
				
			curntscy = *(ntscy + bmpw * curz + curx);
		
			if( curntscy >= threshy ){
				*( m_hitpoint + (m_divx + 1) * indexz + indexx ) = 1;
			}else{
				*( m_hitpoint + (m_divx + 1) * indexz + indexx ) = 0;
			}
		}
	}


/// borderface の作成
	faceno = 0;
	int i1, i2, i3;
	int hitmark[3];
	int hitcnt;
	for( faceno = 0; faceno < facenum; faceno++ ){
		i1 = *( m_indexbuf + faceno * 3 );
		i2 = *( m_indexbuf + faceno * 3 + 1 );
		i3 = *( m_indexbuf + faceno * 3 + 2 );

		hitcnt = 0;
		ZeroMemory( hitmark, sizeof( int ) * 3 );
		if( *( m_hitpoint + i1 ) == 1 ){
			hitmark[0] = 1;
			hitcnt++;
		}
		if( *( m_hitpoint + i2 ) == 1 ){
			hitmark[1] = 1;
			hitcnt++;
		}
		if( *( m_hitpoint + i3 ) == 1 ){
			hitmark[2] = 1;
			hitcnt++;
		}

		int p[3];
		if( hitcnt == 2 ){
			if( hitmark[0] == 0 ){
				p[0] = i2;
				p[1] = i3;
				p[2] = i1;
			}else if( hitmark[1] == 0 ){
				p[0] = i1;
				p[1] = i3;
				p[2] = i2;
			}else if( hitmark[2] == 0 ){
				p[0] = i1;
				p[1] = i2;
				p[2] = i3;
			}

			int newotherp;
			// p[0], p[1]を共有する（p[2]は共有しない）面を探し、その面の、p[0], p[1]以外の点を、dstotherに代入する。
			ret = FindSame2Point( m_indexbuf, facenum, p[0], p[1], p[2], &newotherp );
			if( ret ){
				DbgOut( "grounddata : CreateInvisibleWallObj : FindSame2Point error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto createiwexit;
			}

			if( newotherp >= 0 ){
				newbf = new CBorderFace();
				if( !newbf ){
					DbgOut( "grounddata : CreateInvisibleWallObj : newbf alloc error !!!\n" );
					_ASSERT( 0 );
					ret = 1;
					goto createiwexit;
				}

				newbf->next = m_bf;
				m_bf = newbf;

				ret = newbf->SetParams( p[0], p[1], newotherp, m_divx, m_divz );
				if( ret ){
					DbgOut( "grounddata : CreateInvisibleWallObj : newbf SetParams error !!!\n" );
					_ASSERT( 0 );
					ret = 1;
					goto createiwexit;
				}

				
			}
		}
	}

//borderfaceのギザギザの解消、その１。
	dirtyp = (int*)malloc( sizeof( int ) * (m_divx + 1) * (m_divz + 1) );
	if( !m_hitpoint ){
		DbgOut( "grounddata : CreateInvisibleWallObj : dirtyp alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto createiwexit;
	}
	ZeroMemory( dirtyp, sizeof( int ) * (m_divx + 1) * (m_divz + 1) );
	
	curbf = m_bf;
	while( curbf ){
		int chkp[3];
		chkp[0] = curbf->p1;
		chkp[1] = curbf->p2;
		chkp[2] = curbf->other;

		int i;
		for( i = 0; i < 2; i++ ){
			if( *(dirtyp + chkp[i]) == 0 ){
				
				// chkp[i] だけを含み(bf->p1あるいはbf->p2のどちらかと等しい)、他の２点を含まないBorderFaceを探す。
				if( i == 0 )
					ret = FindSame1EdgeFromBF( chkp[0], chkp[1], chkp[2], &samebf );
				else if( i == 1 )
					ret = FindSame1EdgeFromBF( chkp[1], chkp[0], chkp[2], &samebf );
				//else if( i == 2 )
				//	ret = FindSame1EdgeFromBF( chkp[2], chkp[0], chkp[1], &samebf );

				if( ret ){
					DbgOut( "grounddata : CreateInvisibleWallObj : FindSameOnly1PointFromBF error !!!\n" );
					_ASSERT( 0 );
					ret = 1;
					goto createiwexit;
				}

				if( samebf && (samebf->useflag == 1) ){
					float dot;
					dot = curbf->CalcDot( samebf );
					if( fabs( dot ) <= 0.10f ){
						// chkbfとsamebf の　nvec の角度が９０度に近い場合は、ショートカットする。


						curbf->useflag = 0;
						samebf->useflag = 0;


						newbf = new CBorderFace();
						if( !newbf ){
							DbgOut( "grounddata : CreateInvisibleWallObj : shortcut : newbf alloc error !!!\n" );
							_ASSERT( 0 );
							ret = 1;
							goto createiwexit;
						}

						newbf->next = m_bf;
						m_bf = newbf;

						int scp1, scp2;
						if( chkp[i] == curbf->p1 ){
							scp1 = curbf->p2;
						}else if( chkp[i] == curbf->p2 ){
							scp1 = curbf->p1;
						}else{
							DbgOut( "grounddata : CreateInvisibleWallObj : shortcut : scp1 error !!!\n" );
							_ASSERT( 0 );
							ret = 1;
							goto createiwexit;
						}

						if( chkp[i] == samebf->p1 ){
							scp2 = samebf->p2;
						}else if( chkp[i] == samebf->p2 ){
							scp2 = samebf->p1;
						}else{
							DbgOut( "grounddata : CreateInvisibleWallObj : shortcut : scp2 error !!!\n" );
							_ASSERT( 0 );
							ret = 1;
							goto createiwexit;
						}
						
						ret = newbf->SetParams( scp1, scp2, chkp[i], m_divx, m_divz );
						if( ret ){
							DbgOut( "grounddata : CreateInvisibleWallObj : shortcut : newbf SetParams error !!!\n" );
							_ASSERT( 0 );
							ret = 1;
							goto createiwexit;
						}

						D3DXVECTOR3 compvec;
						if( curbf->clockwise == 1 )
							compvec = curbf->nvec;
						else
							compvec = -1.0f * curbf->nvec;

						ret = newbf->SetClockwise( compvec );// curbf->nvec との内積がの符号により、clockwiseをセットする。
						if( ret ){
							DbgOut( "grounddata : CreateInvisibleWallObj : shortcut : newbf SetClockwise error !!!\n" );
							_ASSERT( 0 );
							ret = 1;
							goto createiwexit;
						}

					}

				}

				*( dirtyp + chkp[i] ) = 1;
			}
		}

		curbf = curbf->next;

	}

	/***
// 枝の除去
	//マップの端以外で、他のBFとエッジを共有しないBFを取り除く。

	curbf = m_bf;
	while( curbf ){
		ret = FindSame1EdgeFromBF( curbf->p1, curbf->p2, curbf->other, &commonbf1 );
		_ASSERT( !ret );

		ret = FindSame1EdgeFromBF( curbf->p2, curbf->p1, curbf->other, &commonbf2 );
		_ASSERT( !ret );

		if( commonbf1 && commonbf2 ){
			//両方のエッジを共有しているＢＦがある場合

		}else if( (commonbf1 == 0) && (commonbf == 0) ){
			//一つもエッジを共有していない場合は、無効にする。

			curbf->useflag = 0;
		}else{
			//片方だけ、エッジを共有していない場合は、マップの端かどうかを、チェックする
			int mapedge;
			if( commonbf1 == 0 ){
				mapedge = IsEdgeOfMap( curbf->p1, m_divx, m_divz );
				if( mapedge == 0 ){
					curbf->useflag = 0;
				}
			}else if( commonbf2 == 0 ){
				mapedge = IsEdgeOfMap( curbf->p2, m_divx, m_divz );
				if( mapedge == 0 ){
					curbf->useflag = 0;
				}				
			}
		}
		curbf = curbf->next;
	}
	***/

// borderface::areanoのセット
	curbf = m_bf;
	int areano;
	while( curbf ){
		ret = Index2PolymeshNo( curbf->p1, m_divx, m_divz, &areano );
		if( ret ){
			DbgOut( "grounddata : CreateInvisibleWallObj : Index2PolymeshNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curbf->areano = areano;

		curbf = curbf->next;
	}



	goto createiwexit;
createiwexit:
	if( ret ){
		DestroyInvisibleWallObj();
	}

	if( dirtyp ){
		free( dirtyp );
	}

	return ret;
}

int CGroundData::SetGroundData( 
		float* ntscy1, float minntscy1, float maxntscy1,
		float* ntscy2, float minntscy2, float maxntscy2,
		float* ntscy3, float minntscy3, float maxntscy3,
		
		int bmpw, int bmph, 
		float maxx, float maxz,
		int divx, int divz, 
		float maxheight )
{

	if( !ntscy1 ){
		DbgOut( "grounddata : SetGroundData : ntscy1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (m_divx != divx) || (m_divz != divz) ){
		DbgOut( "grounddata : SetGroundData : divx, divz error !!!\n" );
		_ASSERT( 0 );
		return 1;		
	}

	int ret;

	float bmpstepx, bmpstepz;
	_ASSERT( (divx > 0) && (divz > 0) );
	bmpstepx = (float)(bmpw - 1) / (float)divx;
	bmpstepz = (float)(bmph - 1) / (float)divz;

	float posstepx, posstepz;
	posstepx = maxx / (float)divx;
	posstepz = maxz / (float)divz;

	float heightstep;
	heightstep = maxheight / (maxntscy1 - minntscy1);


	int pmno;
	CPolyMesh* curpm;
	RECT* currect;
	int indexz, indexx;
	int curx, curz;	
	float curntscy;

	float curposx, curposz, curposy;

	int setno;

	for( pmno = 0; pmno < m_pmnum; pmno++ ){
		curpm = *( m_pmarray + pmno );
		currect = m_rect + pmno;
		
	//座標データのセット。
		setno = 0;
		for( indexz = currect->bottom; indexz <= currect->top; indexz++ ){
			curz = (int)(bmpstepz * (float)indexz);
			curposz = posstepz * (float)indexz;

			for( indexx = currect->left; indexx <= currect->right; indexx++ ){
				curx = (int)(bmpstepx * (float)indexx);
				curposx = posstepx * (float)indexx;
				
				curntscy = *(ntscy1 + bmpw * curz + curx);

				curposy = maxheight - (curntscy - minntscy1) * heightstep;

//DbgOut( "grounddata : SetGroundData : check height : x %f, z %f : rgb %d %d %d : curposy %f\n",
//	   curposx, curposz, 
//	   currgb->r, currgb->g, currgb->b,
//	   curposy );

				ret = curpm->SetPointBuf( setno, curposx, curposy, curposz, setno );
				if( ret ){
					DbgOut( "grounddata : SetGroundData : SetPointBuf error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			
				//texkind
				int curkind;
				//if( curposy > maxheight * 0.5f )
				//	curkind = 0;
				//else
				//	curkind = 1;
				curkind = 0;

				ret = curpm->SetTextureKind( setno, curkind );
				if( ret ){
					DbgOut( "grounddata : SetGroundData : SetTextureKind error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				setno++;
			}
		}

	//indexbufのセット
		ret = curpm->SetDefaultIndex( currect->right - currect->left + 1, currect->top - currect->bottom + 1 );
		if( ret ){
			DbgOut( "grounddata : SetGroundData : SetDefaultIndex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


	//道のtexture kindのセット
		if( ntscy2 ){
			setno = 0;
			float midy;
			midy = minntscy2 + (maxntscy2 - minntscy2) * 0.5f;
			for( indexz = currect->bottom; indexz <= currect->top; indexz++ ){
				curz = (int)(bmpstepz * (float)indexz);
				curposz = posstepz * (float)indexz;

				for( indexx = currect->left; indexx <= currect->right; indexx++ ){
					curx = (int)(bmpstepx * (float)indexx);
					curposx = posstepx * (float)indexx;
					
					curntscy = *(ntscy2 + bmpw * curz + curx);

					if( curntscy <= midy ){
						ret = curpm->SetTextureKind( setno, 2 );
						if( ret ){
							DbgOut( "grounddata : SetGroundData : SetTextureKind error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
					setno++;
				}
			}
		}


	//川のtexture kindのセット
		if( ntscy3 ){
			setno = 0;
			float midy;
			midy = minntscy3 + (maxntscy3 - minntscy3) * 0.5f;
			for( indexz = currect->bottom; indexz <= currect->top; indexz++ ){
				curz = (int)(bmpstepz * (float)indexz);
				curposz = posstepz * (float)indexz;

				for( indexx = currect->left; indexx <= currect->right; indexx++ ){
					curx = (int)(bmpstepx * (float)indexx);
					curposx = posstepx * (float)indexx;
					
					curntscy = *(ntscy3 + bmpw * curz + curx);


//DbgOut( "grounddata : SetGroundData : check river texkind : curntscy %f, midy %f\n", curntscy, midy );

					if( curntscy <= midy ){
						ret = curpm->SetTextureKind( setno, 3 );
						if( ret ){
							DbgOut( "grounddata : SetGroundData : SetTextureKind error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
					setno++;
				}
			}
		}



	//UVのセット
		ret = curpm->SetTex4UV( currect->right - currect->left + 1, currect->top - currect->bottom + 1 );
		if( ret ){
			DbgOut( "grounddata : SetGroundData : SetTex4UV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


	//面単位のCBSphere, 平面係数の作成とセット



	}

	return 0;
}

int CGroundData::SetGroundDataTS( 
		float* ntscy1, float minntscy1, float maxntscy1,
		int bmpw, int bmph, 
		float maxx, float maxz,
		int divx, int divz, 
		float maxheight )
{

	if( !ntscy1 ){
		DbgOut( "grounddata : SetGroundData : ntscy1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (m_divx != divx) || (m_divz != divz) ){
		DbgOut( "grounddata : SetGroundData : divx, divz error !!!\n" );
		_ASSERT( 0 );
		return 1;		
	}

	int ret;

	float bmpstepx, bmpstepz;
	_ASSERT( (divx > 0) && (divz > 0) );
	bmpstepx = (float)(bmpw - 1) / (float)divx;
	bmpstepz = (float)(bmph - 1) / (float)divz;

	float posstepx, posstepz;
	posstepx = maxx / (float)divx;
	posstepz = maxz / (float)divz;

	float heightstep;
	heightstep = maxheight / (maxntscy1 - minntscy1);


	int pmno;
	CPolyMesh* curpm;
	RECT* currect;
	int indexz, indexx;
	int curx, curz;	
	float curntscy;

	float curposx, curposz, curposy;

	int setno;

	for( pmno = 0; pmno < m_pmnum; pmno++ ){
		curpm = *( m_pmarray + pmno );
		currect = m_rect + pmno;
		
	//座標データのセット。
		setno = 0;
		for( indexz = currect->bottom; indexz <= currect->top; indexz++ ){
			curz = (int)(bmpstepz * (float)indexz);
			curposz = posstepz * (float)indexz;

			for( indexx = currect->left; indexx <= currect->right; indexx++ ){
				curx = (int)(bmpstepx * (float)indexx);
				curposx = posstepx * (float)indexx;
				
				curntscy = *(ntscy1 + bmpw * curz + curx);

				curposy = maxheight - (curntscy - minntscy1) * heightstep;

//DbgOut( "grounddata : SetGroundData : check height : x %f, z %f : rgb %d %d %d : curposy %f\n",
//	   curposx, curposz, 
//	   currgb->r, currgb->g, currgb->b,
//	   curposy );

				ret = curpm->SetPointBuf( setno, curposx, curposy, curposz, setno );
				if( ret ){
					DbgOut( "grounddata : SetGroundData : SetPointBuf error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				float setu, setv;
				setu = 1.0f / (float)m_divx * indexx;
				setv = 1.0f - 1.0f / (float)m_divz * indexz;

				ret = curpm->SetUV( setno, setu, setv, 0, 0 );
				_ASSERT( !ret );

				setno++;
			}
		}

	//indexbufのセット
		ret = curpm->SetDefaultIndex( currect->right - currect->left + 1, currect->top - currect->bottom + 1 );
		if( ret ){
			DbgOut( "grounddata : SetGroundData : SetDefaultIndex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


	//面単位のCBSphere, 平面係数の作成とセット



	}

	return 0;
}


int CGroundData::DestroyInvisibleWallObj()
{
	if( m_bf ){
		CBorderFace* curbf = m_bf;
		CBorderFace* nextbf;

		while( curbf ){
			nextbf = curbf->next;
			delete curbf;
			curbf = nextbf;
		}
		m_bf = 0;
	}

	if( m_hitpoint ){
		free( m_hitpoint );
		m_hitpoint = 0;
	}

	if( m_indexbuf ){
		free( m_indexbuf );
		m_indexbuf = 0;
	}

	return 0;
}

int CGroundData::FindSame2Point( int* indexbuf, int facenum, int p1, int p2, int srcother, int* dstother )
{
	// p1, p2を共有する（srcotherは共有しない）面を探し、その面の、p1, p2以外の点を、dstotherに代入する。


	*dstother = -1;//!!!!!!!!


	int faceno;
	int i1, i2, i3;
	int newp1, newp2;
	int setflag[3];
	for( faceno = 0; faceno < facenum; faceno++ ){
		i1 = *( indexbuf + faceno * 3 );
		i2 = *( indexbuf + faceno * 3 + 1 );
		i3 = *( indexbuf + faceno * 3 + 2 );

		ZeroMemory( setflag, sizeof( int ) * 3 );

		if( p1 == i1 ){
			newp1 = i1;
			setflag[0] = 1;
		}else if( p1 == i2 ){
			newp1 = i2;
			setflag[1] = 1;
		}else if( p1 == i3 ){
			newp1 = i3;
			setflag[2] = 1;
		}else{
			continue;
		}

		if( p2 == i1 ){
			newp2 = i1;
			setflag[0] = 1;
		}else if( p2 == i2 ){
			newp2 = i2;
			setflag[1] = 1;
		}else if( p2 == i3 ){
			newp2 = i3;
			setflag[2] = 1;
		}else{
			continue;
		}

		if( (srcother == i1) || (srcother == i2) || (srcother == i3) ){
			//３点とも同じ場合は、スキップ。
			continue;
		}else{
			if( setflag[0] == 0 ){
				*dstother = i1;
				return 0;
			}else if( setflag[1] == 0 ){
				*dstother = i2;
				return 0;
			}else if( setflag[2] == 0 ){
				*dstother = i3;
				return 0;
			}else{
				DbgOut( "grounddata : FindSame2Point : setflag error !!!\n" );
				_ASSERT( 0 );
				*dstother = -1;
				return 1;
			}

		}
	}

	return 0;
}
int CGroundData::FindSame1EdgeFromBF( int srcp1, int srcp2, int srcp3, CBorderFace** samebf )
{
	// srcp1 だけを含み(p1あるいはp2のどちらかと等しい)、srcp2, srcp3を含まないBorderFaceを探す。

	*samebf = 0;

	CBorderFace* curbf = m_bf;
	while( curbf ){
		if( curbf->useflag == 1 ){
			if( (srcp1 == curbf->p1) || (srcp1 == curbf->p2) ){// <---- otherは、調べない！！
				if(	(srcp2 != curbf->p1) && (srcp2 != curbf->p2) ){
					*samebf = curbf;
					break;
				}
			}
		}
		curbf = curbf->next;
	}

	return 0;
}

/***
int CGroundData::FindSame1EdgeFromBF( int srcp1, int srcp2, int srcp3, CBorderFace* samebf )
{
	// srcp1 だけを含み(p1あるいはp2のどちらかと等しい)、srcp2, srcp3を含まないBorderFaceを探す。

	samebf->serialno = -1;

	CBorderFace* curbf = m_bf;
	while( curbf ){
		if( (srcp1 == curbf->p1) || (srcp1 == curbf->p2) ){// <---- otherは、調べない！！
			if(	(srcp2 != curbf->p1) && (srcp2 != curbf->p2) && (srcp2 != curbf->other) ){
				if( (srcp3 != curbf->p1) && (srcp3 != curbf->p2) && (srcp3 != curbf->other) ){
					*samebf = *curbf;
					break;
				}
			}
		}
		curbf = curbf->next;
	}

	return 0;
}
***/

int CGroundData::GetBorderFaceNum( int pmno, int* bfnum )
{
	int retcnt = 0;
	CBorderFace* curbf = m_bf;

	while( curbf ){
		if( (curbf->useflag == 1) && (curbf->areano == pmno) ){
			retcnt++;
		}
		curbf = curbf->next;
	}

	*bfnum = retcnt;

	return 0;
}
int CGroundData::SetInvisibleWallPM( int pmno, int pmvnum, int pmfnum, int divx, int divz, float maxx, float maxz, float wallheight )
{
	int ret;
	CPolyMesh* curpm;
	curpm = *( m_pmarray + pmno );

	int setpno = 0;
	CBorderFace* curbf = m_bf;
	D3DXVECTOR3 v1, v2, v3, v4;
	while( curbf ){
		if( (curbf->useflag == 1) && (curbf->areano == pmno) ){
			ret = Index2Pos( curbf->p1, divx, divz, maxx, maxz, &v1 );
			if( ret ){
				DbgOut( "grounddata : SetInvisibleWallPM : Index2Pos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = Index2Pos( curbf->p2, divx, divz, maxx, maxz, &v2 );
			if( ret ){
				DbgOut( "grounddata : SetInvisibleWallPM : Index2Pos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			v3 = v1;
			v3.y = wallheight;

			v4 = v2;
			v4.y = wallheight;

			if( curbf->clockwise == 1 ){
				ret = curpm->SetPointBuf( setpno, v1.x, v1.y, v1.z, setpno );
				_ASSERT( !ret );
				ret += curpm->SetPointBuf( setpno + 1, v2.x, v2.y, v2.z, setpno + 1 );
				_ASSERT( !ret );
				ret += curpm->SetPointBuf( setpno + 2, v3.x, v3.y, v3.z, setpno + 2 );
				_ASSERT( !ret );
				ret += curpm->SetPointBuf( setpno + 3, v4.x, v4.y, v4.z, setpno + 3 );
				_ASSERT( !ret );
			}else{
				ret = curpm->SetPointBuf( setpno,      v2.x, v2.y, v2.z, setpno );
				_ASSERT( !ret );
				ret += curpm->SetPointBuf( setpno + 1, v1.x, v1.y, v1.z, setpno + 1 );
				_ASSERT( !ret );
				ret += curpm->SetPointBuf( setpno + 2, v4.x, v4.y, v4.z, setpno + 2 );
				_ASSERT( !ret );
				ret += curpm->SetPointBuf( setpno + 3, v3.x, v3.y, v3.z, setpno + 3 );
				_ASSERT( !ret );
			}
			if( ret ){
				DbgOut( "grounddata : SetInvisibleWallPM : curpm SetPointBuf error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			setpno += 4;
		}
		curbf = curbf->next;
	}

	ret = curpm->SetInvisibleWallIndex( pmfnum );
	if( ret ){
		DbgOut( "grounddata : SetInvisibleWallPM : curpm SetInvisibleWallIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( setpno != pmvnum ){
		DbgOut( "grounddata : SetInvisibleWallPM : setpno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CGroundData::Index2Pos( int srcindex, int divx, int divz, float maxx, float maxz, D3DXVECTOR3* dstv )
{
	int indexx, indexz;
	float stepx, stepz;

	stepx = maxx / (float)divx;
	stepz = maxz / (float)divz;

	indexz = srcindex / (divx + 1);
	indexx = srcindex - (divx + 1) * indexz;

	dstv->x = (float)indexx * stepx;
	dstv->z = (float)indexz * stepz;
	//dstv->y = 0.0f;
	dstv->y = -1000.0f;//!!!!!!!!!!!!!

	return 0;
}

int CGroundData::Index2PolymeshNo( int srcindex, int divx, int divz, int* areanoptr )
{
	if( !m_rect || (m_pmnum <= 0) ){
		DbgOut( "grounddata : Index2PolymeshNo : m_rect, m_pmnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int indexx, indexz;

	indexz = srcindex / (divx + 1);
	indexx = srcindex - (divx + 1) * indexz;
	
	int pmno;
	int retno = 0;
	RECT* currect;
	for( pmno = 0; pmno < m_pmnum; pmno++ ){
		currect = m_rect + pmno;

		if( (indexz >= currect->bottom) && (indexz <= currect->top) && 
			(indexx >= currect->left) && (indexx <= currect->right) ){

			retno = pmno;
			break;
		}
	}

	*areanoptr = retno;

	return 0;
}

int CGroundData::IsEdgeOfMap( int srcindex, int divx, int divz )
{
	int isedge = 0;

	int indexx, indexz;

	indexz = srcindex / (divx + 1);
	indexx = srcindex - (divx + 1) * indexz;

	if( (indexx == 0) || (indexx == divx) || 
		(indexz == 0) || (indexz == divz) ){

		isedge = 1;
	}

	return isedge;
}


