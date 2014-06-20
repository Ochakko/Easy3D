// 4 * 4 行列 の 操作用


	// 試しに 狂ったように inline に してみた
	// （ 軽めの 関数に 効果が あるらしいのだが 。。。 ）
	// 代償に calc が コンストラクタに 必要になった

#define	MATRIXH		1

#ifndef CALCH
	#include "Calc.h"
#endif

#ifndef	DISPMODEH
	#include	"dispmode.h"
#endif
extern CCalc*	Calc;

class	CMatrix	: public D3DMATRIX
{
public:
	
	void	*operator new( size_t size );
	void	operator delete( void *p );

	void	SetDbgHwnd( HWND hwnd );

	// static data の 初期化

	static void	InitDivTz2(); // tex4 の 前に 一度だけ 呼ぶ
	static void	InitDoorProj();
	static void	InitInstTex4();


	void	SetD3DMatrix( D3DMATRIX& mat )
	{
		_11 = mat._11; _21 = mat._21; _31 = mat._31; _41 = mat._41;
		_12 = mat._12; _22 = mat._22; _32 = mat._32; _42 = mat._42;
		_13 = mat._13; _23 = mat._23; _33 = mat._33; _43 = mat._43;
		_14 = mat._14; _24 = mat._24; _34 = mat._34; _44 = mat._44;
	};

	void	SetD3DMatrix( D3DMATRIX* mat )
	{
		_11 = mat->_11; _21 = mat->_21; _31 = mat->_31; _41 = mat->_41;
		_12 = mat->_12; _22 = mat->_22; _32 = mat->_32; _42 = mat->_42;
		_13 = mat->_13; _23 = mat->_23; _33 = mat->_33; _43 = mat->_43;
		_14 = mat->_14; _24 = mat->_24; _34 = mat->_34; _44 = mat->_44;
	};

	void	SetInitMatrix()
	{
		_11 = 1.0f; _21 = 0.0f; _31 = 0.0f; _41 = 0.0f;
		_12 = 0.0f; _22 = 1.0f; _32 = 0.0f; _42 = 0.0f;
		_13 = 0.0f; _23 = 0.0f; _33 = 1.0f; _43 = 0.0f;
		_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;

	};


	CMatrix()
	{
		_11 = 1.0f; _21 = 0.0f; _31 = 0.0f; _41 = 0.0f;
		_12 = 0.0f; _22 = 1.0f; _32 = 0.0f; _42 = 0.0f;
		_13 = 0.0f; _23 = 0.0f; _33 = 1.0f; _43 = 0.0f;
		_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;
	};
	CMatrix( float a_11, float a_21, float a_31, float a_41,
					float a_12, float a_22, float a_32, float a_42,
					float a_13, float a_23, float a_33, float a_43 )
	{
		_11 = a_11; _21 = a_21; _31 = a_31; _41 = a_41;
		_12 = a_12; _22 = a_22; _32 = a_32; _42 = a_42;
		_13 = a_13; _23 = a_23; _33 = a_33; _43 = a_43;
		_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;
	};

	CMatrix( float a_11, float a_21, float a_31, float a_41,
					float a_12, float a_22, float a_32, float a_42,
					float a_13, float a_23, float a_33, float a_43,
					float a_14, float a_24, float a_34, float a_44 )
	{
		
		_11 = a_11; _21 = a_21; _31 = a_31; _41 = a_41;
		_12 = a_12; _22 = a_22; _32 = a_32; _42 = a_42;
		_13 = a_13; _23 = a_23; _33 = a_33; _43 = a_43;
		_14 = a_14; _24 = a_24; _34 = a_34; _44 = a_44;

	};
	CMatrix( D3DMATRIX& mat )
	{
		
		_11 = mat._11; _21 = mat._21; _31 = mat._31; _41 = mat._41;
		_12 = mat._12; _22 = mat._22; _32 = mat._32; _42 = mat._42;
		_13 = mat._13; _23 = mat._23; _33 = mat._33; _43 = mat._43;
		_14 = mat._14; _24 = mat._24; _34 = mat._34; _44 = mat._44;
		
	};

	~CMatrix();


	CMatrix&	operator = ( const CMatrix& r)
	{
		_11 = r._11; _21 = r._21; _31 = r._31; _41 = r._41;
		_12 = r._12; _22 = r._22; _32 = r._32; _42 = r._42;
		_13 = r._13; _23 = r._23; _33 = r._33; _43 = r._43;

		_14 = r._14; _24 = r._24; _34 = r._34; _44 = r._44;
		//Calc = r.Calc;

		return *this;
	};

	CMatrix	operator - ()
	{
		return CMatrix( -_11, -_21, -_31, -_41, 
						-_12, -_22, -_32, -_42, 
						-_13, -_23, -_33, -_43, 
						-_14, -_24, -_34, -_44
						);
	};

	CMatrix	operator * ( const CMatrix& b )
	{
	// aを 左から 掛ける (aの変換が 後)

		float	t11, t12, t13, t14;
		float	t21, t22, t23, t24;
		float	t31, t32, t33, t34;
		float	t41, t42, t43, t44;

		t11 = _11 * b._11 + _21 * b._12 + _31 * b._13 + _41 * b._14;
		t12 = _11 * b._21 + _21 * b._22 + _31 * b._23 + _41 * b._24;
		t13 = _11 * b._31 + _21 * b._32 + _31 * b._33 + _41 * b._34;
		t14 = _11 * b._41 + _21 * b._42 + _31 * b._43 + _41 * b._44;

		t21 = _12 * b._11 + _22 * b._12 + _32 * b._13 + _42 * b._14;
		t22 = _12 * b._21 + _22 * b._22 + _32 * b._23 + _42 * b._24;
		t23 = _12 * b._31 + _22 * b._32 + _32 * b._33 + _42 * b._34;
		t24 = _12 * b._41 + _22 * b._42 + _32 * b._43 + _42 * b._44;

		t31 = _13 * b._11 + _23 * b._12 + _33 * b._13 + _43 * b._14;
		t32 = _13 * b._21 + _23 * b._22 + _33 * b._23 + _43 * b._24;
		t33 = _13 * b._31 + _23 * b._32 + _33 * b._33 + _43 * b._34;
		t34 = _13 * b._41 + _23 * b._42 + _33 * b._43 + _43 * b._44;

		t41 = _14 * b._11 + _24 * b._12 + _34 * b._13 + _44 * b._14;
		t42 = _14 * b._21 + _24 * b._22 + _34 * b._23 + _44 * b._24;
		t43 = _14 * b._31 + _24 * b._32 + _34 * b._33 + _44 * b._34;
		t44 = _14 * b._41 + _24 * b._42 + _34 * b._43 + _44 * b._44;

		return CMatrix(  t11, t12, t13, t14, 
						t21, t22, t23, t24,
						t31, t32, t33, t34,
						t41, t42, t43, t44 );
	};


	CMatrix&	operator *= (const CMatrix& r)
	{
		// deliberately create a temp to hold result
		//*this = *this * r;
		CMatrix	savematrix;
		savematrix = r;
		
		*this = savematrix * *this; // r の変換が 後になるように 掛ける ！！！！！
			// そのまま r * *this とすると
			// ｒ が const なので compile err となる
		return *this;
	};

		// ExecuteBuffer 用   tex４
			// 画角一定
	/***
	void	MultVertEBTex40( D3DTLVERTEX* lptlv, 
			D3DTLVERTEX* svert, INDEX_L*	sindex,
			RANGEDATA* calcrange, RANGEDATA2* indexrange, 
			int* rettlvnum, int tlvoffset, int* rettrinum,
			int	planenum, int pointnum, int startz );
	
	void	MultVertEBTex40( D3DTLVERTEX* lptlv, 
			LPD3DTLVERTEX* lplpsvert,
			int* rettlvnum, int* rettrinum,
			int	planenum, int pointnum, int startz );
	***/
	
	void	MultVertEBTex40( D3DTLVERTEX* lptlv, 
			LPD3DTLVERTEX* lplpsvert, int setnum, 
			RANGEDATA* calcrange, RANGEDATA2* indexrange, 
			int tlvoffset, int* rettrinum,
			int	planenum, int startz, CMatrix vrotmat );
	
	/***
	void	MultVertEBTex40( D3DTLVERTEX* lptlv, 
			LPD3DTLVERTEX* lplpsvert, DWORD tlvmaxnum,
			RANGEDATA* calcrange, RANGEDATA2* indexrange, 
			int* rettlvnum, int tlvoffset, int* rettrinum,
			int	planenum, int startz,
			LPD3DTRIANGLE triptr );
	***/
		// 画角　：　外側を　大きく
	void	MultVertEBTex41( D3DTLVERTEX* lptlv, 
			D3DTLVERTEX* svert,  INDEX_L*	sindex,
			RANGEDATA* calcrange, RANGEDATA2* indexrange, 
			int* rettlvnum, int tlvoffset, int* rettrinum,
			int	planenum, int pointnum, int startz );


	
		// svert の ＵＶをセット
	void	SetUVTex4( D3DTLVERTEX* svert,
			RANGEDATA* calcrange, RANGEDATA2* indexrange, 
			int* texkind4, 
			int	planenum, int pointnum );

	void	SetUVTex4( D3DTLVERTEX* svert,
			int* texkind4, 
			int	planenum, int pointnum );

	void	SetUVTex4( D3DTLVERTEX* svert,
			int bmpnoh, int bmpnow,
			int* texkind4,
			int planenum, int pointnum );


		// uvkaerucpp 固定長 の 頂点の executebuffer
	
			// tlv 座表計算のみ uv set なし
					// 画角 : 一定
	void	MultVertEBFix30( D3DTLVERTEX* lptlv, D3DTLVERTEX* svert,
				int planenum, int pointnum );
					// 画角 : DoorScope : 外側に行くほど大きく : ２次元
	void	MultVertEBFix31( D3DTLVERTEX* lptlv, D3DTLVERTEX* svert,
				int planenum, int pointnum );


				//uv set のみ
	void	SetUV2D( D3DTLVERTEX* lptlv, 
				int planenum, int pointnum, int textype );

	void	SetUV1D( D3DTLVERTEX* lptlv, int* memnum, 
				int fannum, int textype );


			// triangle data
			// 縦割りリング
	void	SetTridata(	D3DINSTRUCTION*	lpinst, int planenum, int pointnum );
			// 横割りリング
	void	SetTridataYoko( D3DTRIANGLE* lptri, int planenum, int pointnum, int poffset );
			// 横割りLINE
	void	SetTridataYLine( D3DTRIANGLE* lptri, int planenum, int pointnum, int poffset );
			// fans
	void	SetTridataFan( D3DTRIANGLE* lptri, int planenum, int pointnum, int poffset, int* memnum );
			// both side fans
	void	SetTridataBFan( D3DTRIANGLE* lptri, int planenum, int pointnum, int poffset, int* memnum );


			// JOINT TRIANGLE 付き 横割りリング
	void	SetTriYokoJ(  D3DTRIANGLE* lptri, int jindex, int planenum, int pointnum, int poffset );


		// setmatrix は 計算ではなくて 初期化
	 void	SetMatrixRotXYZ( float rotx, float roty, float rotz )
	{
		float	calcSinX, calcCosX, calcSinY, calcCosY, calcSinZ, calcCosZ;

		calcSinX = Calc->DEGSin( rotx ); calcCosX = Calc->DEGCos( rotx );
		calcSinY = Calc->DEGSin( roty ); calcCosY = Calc->DEGCos( roty );
		calcSinZ = Calc->DEGSin( rotz ); calcCosZ = Calc->DEGCos( rotz );

		_11 = calcCosY * calcCosZ;
		_21 = -calcCosY * calcSinZ;
		_31 = calcSinY;
		_41 = 0.0f;

		_12 = calcSinX * calcSinY * calcCosZ + calcCosX * calcSinZ;
		_22 = -calcSinX * calcSinY + calcCosX * calcCosZ;
		_32 = -calcSinX * calcCosY;
		_42 = 0.0f;

		_13 = -calcCosX * calcSinY * calcCosZ + calcSinX * calcSinZ;
		_23 = calcCosX * calcSinY * calcSinZ + calcSinX * calcCosZ;
		_33 = calcCosX * calcCosY;
		_43 = 0.0f;

		_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;

	};

	 void	SetMatrixTraXYZ( float trax, float tray, float traz )
	{
		_11 = 1.0f; _21 = 0.0f; _31 = 0.0f; _41 = trax;
		_12 = 0.0f; _22 = 1.0f; _32 = 0.0f; _42 = tray;
		_13 = 0.0f; _23 = 0.0f; _33 = 1.0f; _43 = traz;
		_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;

	};
	 void	SetMatrixTraXYZ( D3DVECTOR& tra )
	{
		_11 = 1.0f; _21 = 0.0f; _31 = 0.0f; _41 = tra.x;
		_12 = 0.0f; _22 = 1.0f; _32 = 0.0f; _42 = tra.y;
		_13 = 0.0f; _23 = 0.0f; _33 = 1.0f; _43 = tra.z;
		_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;

	};
	 void	SetMatrixCenterRotXYZ( float rotx, float roty, float rotz, D3DVERTEX& center )
	{
		float	calcSinX, calcCosX, calcSinY, calcCosY, calcSinZ, calcCosZ;

		calcSinX = Calc->DEGSin( rotx ); calcCosX = Calc->DEGCos( rotx );
		calcSinY = Calc->DEGSin( roty ); calcCosY = Calc->DEGCos( roty );
		calcSinZ = Calc->DEGSin( rotz ); calcCosZ = Calc->DEGCos( rotz );


		_11 = calcCosY * calcCosZ;
		_21 = -calcCosY * calcSinZ;
		_31 = calcSinY;
		_41 = -center.x * _11 - center.y * _21 - center.z * _31 + center.x;

		_12 = calcSinX * calcSinY * calcCosZ + calcCosX * calcSinZ;
		_22 = -calcSinX * calcSinY + calcCosX * calcCosZ;
		_32 = -calcSinX * calcCosY;
		_42 = -center.x * _12 - center.y * _22 - center.z * _32 + center.y;

		_13 = -calcCosX * calcSinY * calcCosZ + calcSinX * calcSinZ;
		_23 = calcCosX * calcSinY * calcSinZ + calcSinX * calcCosZ;
		_33 = calcCosX * calcCosY;
		_43 = -center.x * _13 - center.y * _23 - center.z * _33 + center.z;

		_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;

	};
	 void	SetMatrixCenterRotXYZ( float rotx, float roty, float rotz, D3DVECTOR& center )
	{
		float	calcSinX, calcCosX, calcSinY, calcCosY, calcSinZ, calcCosZ;

		calcSinX = Calc->DEGSin( rotx ); calcCosX = Calc->DEGCos( rotx );
		calcSinY = Calc->DEGSin( roty ); calcCosY = Calc->DEGCos( roty );
		calcSinZ = Calc->DEGSin( rotz ); calcCosZ = Calc->DEGCos( rotz );


		_11 = calcCosY * calcCosZ;
		_21 = -calcCosY * calcSinZ;
		_31 = calcSinY;
		_41 = -center.x * _11 - center.y * _21 - center.z * _31 + center.x;

		_12 = calcSinX * calcSinY * calcCosZ + calcCosX * calcSinZ;
		_22 = -calcSinX * calcSinY + calcCosX * calcCosZ;
		_32 = -calcSinX * calcCosY;
		_42 = -center.x * _12 - center.y * _22 - center.z * _32 + center.y;

		_13 = -calcCosX * calcSinY * calcCosZ + calcSinX * calcSinZ;
		_23 = calcCosX * calcSinY * calcSinZ + calcSinX * calcCosZ;
		_33 = calcCosX * calcCosY;
		_43 = -center.x * _13 - center.y * _23 - center.z * _33 + center.z;

		_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;

	};

	 void	SetMatrixCenterRotXYZ( float rotx, float roty, float rotz, D3DTLVERTEX& center )
	{
		float	calcSinX, calcCosX, calcSinY, calcCosY, calcSinZ, calcCosZ;

		calcSinX = Calc->DEGSin( rotx ); calcCosX = Calc->DEGCos( rotx );
		calcSinY = Calc->DEGSin( roty ); calcCosY = Calc->DEGCos( roty );
		calcSinZ = Calc->DEGSin( rotz ); calcCosZ = Calc->DEGCos( rotz );


		_11 = calcCosY * calcCosZ;
		_21 = -calcCosY * calcSinZ;
		_31 = calcSinY;
		_41 = -center.sx * _11 - center.sy * _21 - center.sz * _31 + center.sx;

		_12 = calcSinX * calcSinY * calcCosZ + calcCosX * calcSinZ;
		_22 = -calcSinX * calcSinY + calcCosX * calcCosZ;
		_32 = -calcSinX * calcCosY;
		_42 = -center.sx * _12 - center.sy * _22 - center.sz * _32 + center.sy;

		_13 = -calcCosX * calcSinY * calcCosZ + calcSinX * calcSinZ;
		_23 = calcCosX * calcSinY * calcSinZ + calcSinX * calcCosZ;
		_33 = calcCosX * calcCosY;
		_43 = -center.sx * _13 - center.sy * _23 - center.sz * _33 + center.sz;

		_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;

	};

	 void	SetMatrixCenterScaleXYZ( float scalex, float scaley, float scalez, D3DVERTEX& center )
	{

		_11 = scalex; 
		_21 = 0.0f; 
		_31 = 0.0f; 
		_41 = -center.x * scalex + center.x;

		_12 = 0.0f;	
		_22 = scaley;
		_32 = 0.0f;
		_42 = -center.y * scaley + center.y;

		_13 = 0.0f; 
		_23 = 0.0f; 
		_33 = scalez;
		_43 = -center.z * scalez + center.z;

		_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;

	};


	void	MultMatrix4( D3DMATRIX* a, D3DMATRIX* b, D3DMATRIX* c, D3DMATRIX* d )
	{
		float	a11, a12, a13, a14;
		float	a21, a22, a23, a24;
		float	a31, a32, a33, a34;
		float	a41, a42, a43, a44;

		float	b11, b12, b13, b14;
		float	b21, b22, b23, b24;
		float	b31, b32, b33, b34;
		float	b41, b42, b43, b44;

		float	c11, c12, c13, c14;
		float	c21, c22, c23, c24;
		float	c31, c32, c33, c34;
		float	c41, c42, c43, c44;

		float	d11, d12, d13, d14;
		float	d21, d22, d23, d24;
		float	d31, d32, d33, d34;
		float	d41, d42, d43, d44;

		float	t11, t12, t13, t14;
		float	t21, t22, t23, t24;
		float	t31, t32, t33, t34;
		float	t41, t42, t43, t44;

		float	u11, u12, u13, u14;
		float	u21, u22, u23, u24;
		float	u31, u32, u33, u34;
		float	u41, u42, u43, u44;

		a11 = a->_11; a12 = a->_12; a13 = a->_13; a14 = a->_14;
		a21 = a->_21; a22 = a->_22; a23 = a->_23; a24 = a->_24;
		a31 = a->_31; a32 = a->_32; a33 = a->_33; a34 = a->_34;
		a41 = a->_41; a42 = a->_42; a43 = a->_43; a44 = a->_44;

		b11 = b->_11; b12 = b->_12; b13 = b->_13; b14 = b->_14;
		b21 = b->_21; b22 = b->_22; b23 = b->_23; b24 = b->_24;
		b31 = b->_31; b32 = b->_32; b33 = b->_33; b34 = b->_34;
		b41 = b->_41; b42 = b->_42; b43 = b->_43; b44 = b->_44;

		c11 = c->_11; c12 = c->_12; c13 = c->_13; c14 = c->_14;
		c21 = c->_21; c22 = c->_22; c23 = c->_23; c24 = c->_24;
		c31 = c->_31; c32 = c->_32; c33 = c->_33; c34 = c->_34;
		c41 = c->_41; c42 = c->_42; c43 = c->_43; c44 = c->_44;
 
		d11 = d->_11; d12 = d->_12; d13 = d->_13; d14 = d->_14;
		d21 = d->_21; d22 = d->_22; d23 = d->_23; d24 = d->_24;
		d31 = d->_31; d32 = d->_32; d33 = d->_33; d34 = d->_34;
		d41 = d->_41; d42 = d->_42; d43 = d->_43; d44 = d->_44;

// a * b

		t11 = a11 * b11 + a21 * b12 + a31 * b13 + a41 * b14;
		t21 = a11 * b21 + a21 * b22 + a31 * b23 + a41 * b24;
		t31 = a11 * b31 + a21 * b32 + a31 * b33 + a41 * b34;
		t41 = a11 * b41 + a21 * b42 + a31 * b43 + a41 * b44;

		t12 = a12 * b11 + a22 * b12 + a32 * b13 + a42 * b14;
		t22 = a12 * b21 + a22 * b22 + a32 * b23 + a42 * b24;
		t32 = a12 * b31 + a22 * b32 + a32 * b33 + a42 * b34;
		t42 = a12 * b41 + a22 * b42 + a32 * b43 + a42 * b44;

		t13 = a13 * b11 + a23 * b12 + a33 * b13 + a43 * b14;
		t23 = a13 * b21 + a23 * b22 + a33 * b23 + a43 * b24;
		t33 = a13 * b31 + a23 * b32 + a33 * b33 + a43 * b34;
		t43 = a13 * b41 + a23 * b42 + a33 * b43 + a43 * b44;

		t14 = a14 * b11 + a24 * b12 + a34 * b13 + a44 * b14;
		t24 = a14 * b21 + a24 * b22 + a34 * b23 + a44 * b24;
		t34 = a14 * b31 + a24 * b32 + a34 * b33 + a44 * b34;
		t44 = a14 * b41 + a24 * b42 + a34 * b43 + a44 * b44;

// ( a * b ) * c
		u11 = t11 * c11 + t21 * c12 + t31 * c13 + t41 * c14;
		u21 = t11 * c21 + t21 * c22 + t31 * c23 + t41 * c24;
		u31 = t11 * c31 + t21 * c32 + t31 * c33 + t41 * c34;
		u41 = t11 * c41 + t21 * c42 + t31 * c43 + t41 * c44;
	
		u12 = t12 * c11 + t22 * c12 + t32 * c13 + t42 * c14;
		u22 = t12 * c21 + t22 * c22 + t32 * c23 + t42 * c24;
		u32 = t12 * c31 + t22 * c32 + t32 * c33 + t42 * c34;
		u42 = t12 * c41 + t22 * c42 + t32 * c43 + t42 * c44;

		u13 = t13 * c11 + t23 * c12 + t33 * c13 + t43 * c14;
		u23 = t13 * c21 + t23 * c22 + t33 * c23 + t43 * c24;
		u33 = t13 * c31 + t23 * c32 + t33 * c33 + t43 * c34;
		u43 = t13 * c41 + t23 * c42 + t33 * c43 + t43 * c44;

		u14 = t14 * c11 + t24 * c12 + t34 * c13 + t44 * c14;
		u24 = t14 * c21 + t24 * c22 + t34 * c23 + t44 * c24;
		u34 = t14 * c31 + t24 * c32 + t34 * c33 + t44 * c34;
		u44 = t14 * c41 + t24 * c42 + t34 * c43 + t44 * c44;
// (a *  b * c) * d
		_11 = u11 * d11 + u21 * d12 + u31 * d13 + u41 * d14;
		_21 = u11 * d21 + u21 * d22 + u31 * d23 + u41 * d24;
		_31 = u11 * d31 + u21 * d32 + u31 * d33 + u41 * d34;
		_41 = u11 * d41 + u21 * d42 + u31 * d43 + u41 * d44;

		_12 = u12 * d11 + u22 * d12 + u32 * d13 + u42 * d14;
		_22 = u12 * d21 + u22 * d22 + u32 * d23 + u42 * d24;
		_32 = u12 * d31 + u22 * d32 + u32 * d33 + u42 * d34;
		_42 = u12 * d41 + u22 * d42 + u32 * d43 + u42 * d44;

		_13 = u13 * d11 + u23 * d12 + u33 * d13 + u43 * d14;
		_23 = u13 * d21 + u23 * d22 + u33 * d23 + u43 * d24;
		_33 = u13 * d31 + u23 * d32 + u33 * d33 + u43 * d34;
		_43 = u13 * d41 + u23 * d42 + u33 * d43 + u43 * d44;

		_14 = u14 * d11 + u24 * d12 + u34 * d13 + u44 * d14;
		_24 = u14 * d21 + u24 * d22 + u34 * d23 + u44 * d24;
		_34 = u14 * d31 + u24 * d32 + u34 * d33 + u44 * d34;
		_44 = u14 * d41 + u24 * d42 + u34 * d43 + u44 * d44;

	}

	void	MultMatrix3( D3DMATRIX* a, D3DMATRIX* b, D3DMATRIX* c )
	{
		float	a11, a12, a13, a14;
		float	a21, a22, a23, a24;
		float	a31, a32, a33, a34;
		float	a41, a42, a43, a44;

		float	b11, b12, b13, b14;
		float	b21, b22, b23, b24;
		float	b31, b32, b33, b34;
		float	b41, b42, b43, b44;

		float	c11, c12, c13, c14;
		float	c21, c22, c23, c24;
		float	c31, c32, c33, c34;
		float	c41, c42, c43, c44;

		float	t11, t12, t13, t14;
		float	t21, t22, t23, t24;
		float	t31, t32, t33, t34;
		float	t41, t42, t43, t44;

		a11 = a->_11; a12 = a->_12; a13 = a->_13; a14 = a->_14;
		a21 = a->_21; a22 = a->_22; a23 = a->_23; a24 = a->_24;
		a31 = a->_31; a32 = a->_32; a33 = a->_33; a34 = a->_34;
		a41 = a->_41; a42 = a->_42; a43 = a->_43; a44 = a->_44;

		b11 = b->_11; b12 = b->_12; b13 = b->_13; b14 = b->_14;
		b21 = b->_21; b22 = b->_22; b23 = b->_23; b24 = b->_24;
		b31 = b->_31; b32 = b->_32; b33 = b->_33; b34 = b->_34;
		b41 = b->_41; b42 = b->_42; b43 = b->_43; b44 = b->_44;

		c11 = c->_11; c12 = c->_12; c13 = c->_13; c14 = c->_14;
		c21 = c->_21; c22 = c->_22; c23 = c->_23; c24 = c->_24;
		c31 = c->_31; c32 = c->_32; c33 = c->_33; c34 = c->_34;
		c41 = c->_41; c42 = c->_42; c43 = c->_43; c44 = c->_44;
 
// a * b

		t11 = a11 * b11 + a21 * b12 + a31 * b13 + a41 * b14;
		t21 = a11 * b21 + a21 * b22 + a31 * b23 + a41 * b24;
		t31 = a11 * b31 + a21 * b32 + a31 * b33 + a41 * b34;
		t41 = a11 * b41 + a21 * b42 + a31 * b43 + a41 * b44;

		t12 = a12 * b11 + a22 * b12 + a32 * b13 + a42 * b14;
		t22 = a12 * b21 + a22 * b22 + a32 * b23 + a42 * b24;
		t32 = a12 * b31 + a22 * b32 + a32 * b33 + a42 * b34;
		t42 = a12 * b41 + a22 * b42 + a32 * b43 + a42 * b44;

		t13 = a13 * b11 + a23 * b12 + a33 * b13 + a43 * b14;
		t23 = a13 * b21 + a23 * b22 + a33 * b23 + a43 * b24;
		t33 = a13 * b31 + a23 * b32 + a33 * b33 + a43 * b34;
		t43 = a13 * b41 + a23 * b42 + a33 * b43 + a43 * b44;

		t14 = a14 * b11 + a24 * b12 + a34 * b13 + a44 * b14;
		t24 = a14 * b21 + a24 * b22 + a34 * b23 + a44 * b24;
		t34 = a14 * b31 + a24 * b32 + a34 * b33 + a44 * b34;
		t44 = a14 * b41 + a24 * b42 + a34 * b43 + a44 * b44;

// ( a * b ) * c
		_11 = t11 * c11 + t21 * c12 + t31 * c13 + t41 * c14;
		_21 = t11 * c21 + t21 * c22 + t31 * c23 + t41 * c24;
		_31 = t11 * c31 + t21 * c32 + t31 * c33 + t41 * c34;
		_41 = t11 * c41 + t21 * c42 + t31 * c43 + t41 * c44;
	
		_12 = t12 * c11 + t22 * c12 + t32 * c13 + t42 * c14;
		_22 = t12 * c21 + t22 * c22 + t32 * c23 + t42 * c24;
		_32 = t12 * c31 + t22 * c32 + t32 * c33 + t42 * c34;
		_42 = t12 * c41 + t22 * c42 + t32 * c43 + t42 * c44;

		_13 = t13 * c11 + t23 * c12 + t33 * c13 + t43 * c14;
		_23 = t13 * c21 + t23 * c22 + t33 * c23 + t43 * c24;
		_33 = t13 * c31 + t23 * c32 + t33 * c33 + t43 * c34;
		_43 = t13 * c41 + t23 * c42 + t33 * c43 + t43 * c44;

		_14 = t14 * c11 + t24 * c12 + t34 * c13 + t44 * c14;
		_24 = t14 * c21 + t24 * c22 + t34 * c23 + t44 * c24;
		_34 = t14 * c31 + t24 * c32 + t34 * c33 + t44 * c34;
		_44 = t14 * c41 + t24 * c42 + t34 * c43 + t44 * c44;
	}

	void	MultMatrix2( D3DMATRIX* a, D3DMATRIX* b )
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

		_11 = a11 * b11 + a21 * b12 + a31 * b13 + a41 * b14;
		_21 = a11 * b21 + a21 * b22 + a31 * b23 + a41 * b24;
		_31 = a11 * b31 + a21 * b32 + a31 * b33 + a41 * b34;
		_41 = a11 * b41 + a21 * b42 + a31 * b43 + a41 * b44;

		_12 = a12 * b11 + a22 * b12 + a32 * b13 + a42 * b14;
		_22 = a12 * b21 + a22 * b22 + a32 * b23 + a42 * b24;
		_32 = a12 * b31 + a22 * b32 + a32 * b33 + a42 * b34;
		_42 = a12 * b41 + a22 * b42 + a32 * b43 + a42 * b44;

		_13 = a13 * b11 + a23 * b12 + a33 * b13 + a43 * b14;
		_23 = a13 * b21 + a23 * b22 + a33 * b23 + a43 * b24;
		_33 = a13 * b31 + a23 * b32 + a33 * b33 + a43 * b34;
		_43 = a13 * b41 + a23 * b42 + a33 * b43 + a43 * b44;

		_14 = a14 * b11 + a24 * b12 + a34 * b13 + a44 * b14;
		_24 = a14 * b21 + a24 * b22 + a34 * b23 + a44 * b24;
		_34 = a14 * b31 + a24 * b32 + a34 * b33 + a44 * b34;
		_44 = a14 * b41 + a24 * b42 + a34 * b43 + a44 * b44;


	}

private:
	//heap
	static HANDLE	s_hHeap;
	static DWORD	s_uNumAllocsInHeap;

protected:
	static HWND	s_mainhwnd;

	static void	InitDoorProj_char();
	static void	InitDoorProj_back();
};