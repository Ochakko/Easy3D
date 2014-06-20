#define	ARRAYMATRIXH	1

// d3dmatrix の　配列を持つ　CMatrix 、　motion matrix の　演算に使用

// CMatrix から継承される　親のD3DMATRIX の成分は　計算の　TEMP DATA として使用する


#ifndef	MATRIXH
	#include	"matrix.h"
#endif



//class	CArrayMatrix : public CMatrix
class CArrayMatrix : public D3DMATRIX
{
public:
	
	CArrayMatrix( DWORD	matnum );
	~CArrayMatrix();

	void	*operator new( size_t size );
	void	operator delete( void *p );
	
		// チェック　フラグ
	BOOL	bfinishConstruct; // obj 作成が　成功したか
	BOOL	bsetmultvars; // MultVertEBFix30 の　準備が出来ているか
	BOOL	bsetd3dmat;		// m_d3dmat に　D3DMATRIX が　セットされているか



	BOOL	SetD3DMatrix( D3DMATRIX* matptr, DWORD	setnum );

	D3DMATRIX*	GetMatPtr( DWORD matno );

	BOOL	EQD3DMat( DWORD	setno, DWORD destno );

	void	SetMultVars(
				int	*matindexptr,
				D3DTLVERTEX**	hebtlv,
				D3DTLVERTEX**	hstlv,
				int	*pnumptr,
				int	*vnumptr,
				int	ctrlnum
			);

		// ctrl ごとに　
		// MultMatrix2( tramat, (m_d3dmat + *(matindex + ctrlno) + matoffset ) を　計算し
		// この値を使って　tlv を　変換する
	BOOL	SetMatrixFix30( DWORD matoffset, D3DMATRIX* tramat );

	void	MultVertEBFix30( D3DMATRIX* smat, D3DTLVERTEX* lptlv, D3DTLVERTEX* svert,
				int planenum, int pointnum );

	void	MultMatrix2( D3DMATRIX* retm, D3DMATRIX* a, D3DMATRIX* b )
	{
		float	a11, a12, a13, a14;
		float	a21, a22, a23, a24;
		float	a31, a32, a33, a34;
		float	a41, a42, a43, a44;

		float	b11, b12, b13, b14;
		float	b21, b22, b23, b24;
		float	b31, b32, b33, b34;
		float	b41, b42, b43, b44;

		a11 = a->_11; a12 = a->_12; a13 = a->_13; a14 = a->_14;
		a21 = a->_21; a22 = a->_22; a23 = a->_23; a24 = a->_24;
		a31 = a->_31; a32 = a->_32; a33 = a->_33; a34 = a->_34;
		a41 = a->_41; a42 = a->_42; a43 = a->_43; a44 = a->_44;

		b11 = b->_11; b12 = b->_12; b13 = b->_13; b14 = b->_14;
		b21 = b->_21; b22 = b->_22; b23 = b->_23; b24 = b->_24;
		b31 = b->_31; b32 = b->_32; b33 = b->_33; b34 = b->_34;
		b41 = b->_41; b42 = b->_42; b43 = b->_43; b44 = b->_44;

		retm->_11 = a11 * b11 + a21 * b12 + a31 * b13 + a41 * b14;
		retm->_21 = a11 * b21 + a21 * b22 + a31 * b23 + a41 * b24;
		retm->_31 = a11 * b31 + a21 * b32 + a31 * b33 + a41 * b34;
		retm->_41 = a11 * b41 + a21 * b42 + a31 * b43 + a41 * b44;

		retm->_12 = a12 * b11 + a22 * b12 + a32 * b13 + a42 * b14;
		retm->_22 = a12 * b21 + a22 * b22 + a32 * b23 + a42 * b24;
		retm->_32 = a12 * b31 + a22 * b32 + a32 * b33 + a42 * b34;
		retm->_42 = a12 * b41 + a22 * b42 + a32 * b43 + a42 * b44;

		retm->_13 = a13 * b11 + a23 * b12 + a33 * b13 + a43 * b14;
		retm->_23 = a13 * b21 + a23 * b22 + a33 * b23 + a43 * b24;
		retm->_33 = a13 * b31 + a23 * b32 + a33 * b33 + a43 * b34;
		retm->_43 = a13 * b41 + a23 * b42 + a33 * b43 + a43 * b44;

		retm->_14 = a14 * b11 + a24 * b12 + a34 * b13 + a44 * b14;
		retm->_24 = a14 * b21 + a24 * b22 + a34 * b23 + a44 * b24;
		retm->_34 = a14 * b31 + a24 * b32 + a34 * b33 + a44 * b34;
		retm->_44 = a14 * b41 + a24 * b42 + a34 * b43 + a44 * b44;


	}


private:
	//heap
	static HANDLE	s_hHeap;
	static DWORD	s_uNumAllocsInHeap;

		// multvars
	int	m_matindex[CTLMAX];
	D3DTLVERTEX*	m_ebtlvptr[CTLMAX];
	D3DTLVERTEX*	m_stlvptr[CTLMAX];
	int	m_pnum[CTLMAX];
	int	m_vnum[CTLMAX];
	int	m_ctrlnum;

	D3DMATRIX	m_multmat[CTLMAX];
	char		m_multed[CTLMAX];

	D3DMATRIX*	m_d3dmat;
	DWORD		m_d3dmatnum;
	
	void	InitMultVars();

};

