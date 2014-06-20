#include	"stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <d3drmwin.h>

#ifndef	COLORNAMEH
	#include	"colorname.h"
#endif

#include	"playarea.h"

#include	"ArrayMatrix.h"


// extern !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
extern CPlayArea*	gplayarea;
extern LONG		gprojmode;
extern HANDLE	g_hMuteCampos;
extern CHARPOS campos;


static DWORD	sized3dmat = sizeof( D3DMATRIX );

// texture UV セット用
static int		texpcnt = 0;
static int		texvcnt = 0;

static int		next_texpcnt[2] = {1, 0};
static int		next_texvcnt[2] = {1, 0};

static int		texkind = 0;

static D3DVALUE	texu4[4][2][2] = { { {0.0f, 0.5f}, {0.0f, 0.5f} }, { {0.5f, 1.0f}, {0.5f, 1.0f} }, 
								   { {0.0f, 0.5f}, {0.0f, 0.5f} }, { {0.5f, 1.0f}, {0.5f, 1.0f} } };// [texkind][texpcnt][texvcnt]

static D3DVALUE	texv4[4][2][2] = { { {0.0f, 0.0f}, {0.5f, 0.5f} }, { {0.0f, 0.0f}, {0.5f, 0.5f} }, 
								   { {0.5f, 0.5f}, {1.0f, 1.0f} }, { {0.5f, 0.5f}, {1.0f, 1.0f} }  };

	// 境界線で 他色が 出ないように 境界線の値を 避ける
static D3DVALUE	texu42[4][2][2] = { { {0.0f, 0.4f}, {0.0f, 0.4f} }, { {0.6f, 0.9f}, {0.6f, 0.9f} }, 
								   { {0.0f, 0.4f}, {0.0f, 0.4f} }, { {0.6f, 0.9f}, {0.6f, 0.9f} } };// [texkind][texpcnt][texvcnt]

static D3DVALUE	texv42[4][2][2] = { { {0.0f, 0.0f}, {0.4f, 0.4f} }, { {0.0f, 0.0f}, {0.4f, 0.4f} }, 
								   { {0.6f, 0.6f}, {0.9f, 0.9f} }, { {0.6f, 0.6f}, {0.9f, 0.9f} }  };


///////////////////////////////////
// init  static 

HANDLE	CArrayMatrix::s_hHeap = NULL;
DWORD	CArrayMatrix::s_uNumAllocsInHeap = 0;

///////////////////////////////////


CArrayMatrix::CArrayMatrix( DWORD	matnum )
{
	bfinishConstruct = FALSE;
	bsetmultvars = FALSE;
	bsetd3dmat = FALSE;

	m_d3dmatnum = matnum;

	DWORD	matsize;

	matsize = sized3dmat * matnum;
	m_d3dmat = (D3DMATRIX*)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, matsize );
	if( !m_d3dmat ){
		TRACE( "ARRAYMATRIX : m_d3dmat alloc error!!!\n" );
		return;
	}
	memset( m_d3dmat, 0, matsize );

	_11 = 1.0f; _21 = 0.0f; _31 = 0.0f; _41 = 0.0f;
	_12 = 0.0f; _22 = 1.0f; _32 = 0.0f; _42 = 0.0f;
	_13 = 0.0f; _23 = 0.0f; _33 = 1.0f; _43 = 0.0f;
	_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;

	InitMultVars();

	bfinishConstruct = TRUE;
}

CArrayMatrix::~CArrayMatrix()
{
	if( m_d3dmat ){
		HeapFree( s_hHeap, HEAP_NO_SERIALIZE, m_d3dmat );
		m_d3dmat = 0;
	}
}

void	*CArrayMatrix::operator new ( size_t size )
{

	if( s_hHeap == NULL ){
		s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
		
		if( s_hHeap == NULL )
			return NULL;
	}

	void	*p;
	p = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, size );
		
	if( p != NULL ){
		s_uNumAllocsInHeap++;
	}

	return (p);
}


void	CArrayMatrix::operator delete ( void *p )
{
	if( HeapFree( s_hHeap, HEAP_NO_SERIALIZE, p ) ){
		s_uNumAllocsInHeap--;
	}

	if( s_uNumAllocsInHeap == 0 ){
		if( HeapDestroy( s_hHeap ) ){
			s_hHeap = NULL;
		}
	}
}

void	CArrayMatrix::SetMultVars(
				int	*matindexptr,
				D3DTLVERTEX**	hebtlv,
				D3DTLVERTEX**	hstlv,
				int	*pnumptr,
				int	*vnumptr,
				int	ctrlnum
			)
{
	memcpy( m_matindex, matindexptr, sizeof( int ) * ctrlnum );
	memcpy( m_ebtlvptr, hebtlv, sizeof( D3DTLVERTEX* ) * ctrlnum );
	memcpy( m_stlvptr, hstlv, sizeof( D3DTLVERTEX* ) * ctrlnum );
	memcpy( m_pnum, pnumptr, sizeof( int ) * ctrlnum );
	memcpy( m_vnum, vnumptr, sizeof( int ) * ctrlnum );
	m_ctrlnum = ctrlnum;

	bsetmultvars = TRUE;

}

void	CArrayMatrix::InitMultVars()
{
	memset( m_matindex, 0, sizeof( int ) * CTLMAX );
	memset( m_ebtlvptr, 0, sizeof( D3DTLVERTEX* ) * CTLMAX );
	memset( m_stlvptr, 0, sizeof( D3DTLVERTEX* ) * CTLMAX );
	memset( m_pnum, 0, sizeof( int ) * CTLMAX );
	memset( m_vnum, 0, sizeof( int ) * CTLMAX );
	m_ctrlnum = 0;

	memset( m_multmat, 0, sizeof( D3DMATRIX ) * CTLMAX );
	memset( m_multed, 0, sizeof( char ) * CTLMAX );
}


BOOL	CArrayMatrix::SetD3DMatrix( D3DMATRIX* matptr, DWORD setnum )
{
	DWORD setsize;

	if( setnum > m_d3dmatnum || !matptr ){
		TRACE( "ARRAYMATRIX : SetD3DMatrix : param error!!!\n" );
		return FALSE;
	}
	if( !m_d3dmat ){
		TRACE( "ARRAYMATRIX : SetD3DMatrix : d3dmat not created yet error!!!\n" );
		return FALSE;		
	}
	
	setsize = sized3dmat * setnum;
	memcpy( m_d3dmat, matptr, setsize );

	bsetd3dmat = TRUE;
	return TRUE;
}

BOOL	CArrayMatrix::EQD3DMat( DWORD setno, DWORD destno )
{
	//if( !bfinishConstruct || !bsetd3dmat )
	//	return FALSE;

	*(m_d3dmat + setno) = *(m_d3dmat + destno);
	//memcpy( m_d3dmat + setno, m_d3dmat + destno, sizeof( D3DMATRIX ) );

	return TRUE;
}

D3DMATRIX*	CArrayMatrix::GetMatPtr( DWORD matno )
{
	if( matno < 0 || matno > m_d3dmatnum )
		return 0;

	return (m_d3dmat + matno);
}

BOOL	CArrayMatrix::SetMatrixFix30( DWORD matoffset, D3DMATRIX* tramat )
{
	int setno;
	
	D3DMATRIX	*matptr, *mat1st, *multmat;
	D3DTLVERTEX	**hret, **hsource;
	int	*pnum, *vnum;
	char*	multed;

	if( !bsetmultvars )
		return FALSE;

	memset( m_multed, 0, sizeof( char ) * m_ctrlnum );

	multmat = m_multmat;
	mat1st = m_d3dmat + matoffset;
	hret = m_ebtlvptr;
	hsource = m_stlvptr;
	pnum = m_pnum;
	vnum = m_vnum;
	multed = m_multed;

	// ctrl によっては　matrix が使いまわせるものがあるので　multed でチェックする
	for( setno = 0; setno < m_ctrlnum; setno++ ){
		matptr = mat1st + *(m_matindex + setno);

		if( !*multed ){
			MultMatrix2( multmat, tramat, matptr );
			*multed = 1;
		}else{
			// 使いまわし
			*multmat = *(m_multmat + setno);
		}

		MultVertEBFix30( multmat, *hret, *hsource, *pnum, *vnum );
		hret++; hsource++; pnum++; vnum++; multmat++; multed++;
	}

	return TRUE;
}


void	CArrayMatrix::MultVertEBFix30( D3DMATRIX* smat, D3DTLVERTEX* lptlv, D3DTLVERTEX* svert,
				int planenum, int pointnum )
{
	// tan( fov ) == FROGTANFOV の 円錐 VIEWPORT

	int	datano;
	float	tx, ty, tz, aw, rhw;


	int	datamaxno;
	LPD3DTLVERTEX		lpfirsttlv, curtlv;
	LPD3DTLVERTEX		sourcetlv;
	static	float	projsc = ( NEARCLIP * SCWIDTH2 ) / ( (float)FROGTANFOV );
    D3DVALUE s11, s12, s13, s14; 
    D3DVALUE s21, s22, s23, s24; 
    D3DVALUE s31, s32, s33, s34; 
    D3DVALUE s41, s42, s43, s44; 
	
    s11 = smat->_11, s12 = smat->_12, s13 = smat->_13, s14 = smat->_14; 
    s21 = smat->_21, s22 = smat->_22, s23 = smat->_23, s24 = smat->_24; 
    s31 = smat->_31, s32 = smat->_32, s33 = smat->_33, s34 = smat->_34; 
    s41 = smat->_41, s42 = smat->_42, s43 = smat->_43, s44 = smat->_44; 

	curtlv = lptlv;
	lpfirsttlv = lptlv;


	datamaxno = planenum * pointnum;


	for( datano = 0; datano < datamaxno; datano++ ){
		sourcetlv = svert + datano;

		tx = sourcetlv->sx;
		ty = sourcetlv->sy;
		tz = sourcetlv->sz;			

		
		aw = s13 * tx + s23 * ty + s33 * tz + s43;

		if( aw )
			rhw = projsc / aw;
		else
			rhw = 0.0f;

		curtlv->rhw = rhw;
		curtlv->sx = (s11 * tx + s21 * ty + s31 * tz + s41) * rhw + SCWIDTH2;
		curtlv->sy = (s12 * tx + s22 * ty + s32 * tz + s42) * -rhw + SCHEIGHT2; 
		curtlv->sz = ( aw - NEARCLIP ) * INV_ZDIST;

		curtlv->color = sourcetlv->color;
		curtlv->specular = sourcetlv->specular;

		curtlv++;
	}

}






