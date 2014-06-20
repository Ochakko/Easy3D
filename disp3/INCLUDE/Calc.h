#define	CALCH	1

//#include	<d3drmwin.h>
#include <D3DX9.h>

#ifndef CHARPOSH
	#include "CharPos.h"
#endif
#ifndef CHARPARAMH
	#include "CharParam.h"
#endif

//#include	"3dplus.h"
class CCalc
{
public:
	CCalc();
	~CCalc();

	//void	*operator new( size_t size );
	//void	operator delete( void *p );

	//heap
	//static HANDLE	s_hHeap;
	//static unsigned int	s_uNumAllocsInHeap;

public:
	/***
	void DEGPushRotX( float deg );//以下のCos, Sinを使った glMultMatrix 
	void DEGPushRotY( float deg );
	void DEGPushRotZ( float deg );
	void DEGPushRotXYZ( float deg, LOCATION& jiku );
	void DEGPushRotXpYpZ( ROTATE& rot );
	void PAIPushRotX( float deg );//以下のCos, Sinを使った glMultMatrix 
	void PAIPushRotY( float deg );
	void PAIPushRotZ( float deg );
	void PushTra( float x, float y, float z );
	void PushTra( LOCATION& loc );
	void PushScale( float x, float y, float z );
	void PushScale( LOCATION& loc );
	***/

	void	SetVec( LOCATION* retvec, LOCATION& vec1, LOCATION& vec0 );
	void	SetVec( LOCATION* retvec, D3DTLVERTEX& vec1, D3DTLVERTEX& vec0 );
	void	Gaiseki( LOCATION* retlocdat, LOCATION& locdat1, LOCATION& locdat2 );
	float	Naiseki( LOCATION& locdat1, LOCATION& locdat2 );

	/***
//EDITPOINT!!!INITCHARPARAM
	void	InitCharParam( CHARPARAM*	pparam );

//CTRL GROUP ごとの CHARPARAM の INIT 
	void	InitParamCTLALL( CHARPARAM*	pparam );
	void	InitParamHEAD0( CHARPARAM*	pparam );
	void	InitParamHEAD00( CHARPARAM*	pparam );
	void	InitParamHEAD000( CHARPARAM*	pparam );
	void	InitParamHEAD010( CHARPARAM*	pparam );
	void	InitParamARML0( CHARPARAM*	pparam );
	void	InitParamARMR0( CHARPARAM*	pparam );
	void	InitParamLEGL0( CHARPARAM*	pparam );
	void	InitParamLEGR0( CHARPARAM*	pparam );
	void	InitParamPIS( CHARPARAM*	pparam );
//ENDEDIT
	***/

	float DEGCos( float deg );//0.5度刻みの表引き
	float DEGSin( float deg );//0.5度刻みの表引き
	float DEGTan( float deg );
	float DEGAtan( float z, float x );// ret deg  atan2( z, x );

	//int	  IndexAbs( int val0, int val1 );// ret 0～150

	float Sqrt2( float x, float z ); //sqrt( x*x + z*z ) を 
							   //max * sqrt( 1 + min*min/max/max ) の形にしてから表引き
	float Sqrt3( float x, float y, float z );
	
	float Sqrt2( LOCATION& loc0, LOCATION& loc1 );
	float Sqrt3( LOCATION& loc0, LOCATION& loc1 );


	/***
	void  MakeRotXYZPoints( float rotdeg, LOCATION& dir, 
			float* point, float* retpoint, int pointnum );
	void  MakeRotXYZPoints( float rotdeg, LOCATION& dir, LOCATION& tra, 
			float* point, float* retpoint, int pointnum );

	void MakeRotPoint3( D3DVERTEX* retvert, float rotx, float roty, float rotz, int pointnum );
	void MakeRotPoint3( D3DVERTEX* retvert, float rotx, float roty, float rotz,
		int planenum, int pointnum );
	void MakeRotPoint3( D3DVERTEX* retvert, D3DVERTEX& centervert, 
		float rotx, float roty, float rotz,
		int planenum, int pointnum );
	void MakeRotPoint3( D3DVERTEX* retvert, D3DVERTEX& centervert, 
		float rotx, float roty, float rotz ); // center よう

	void MakeRotPoint3f( D3DVERTEX* retvert, D3DVERTEX* firstvert, D3DVERTEX& centervert, 
		float rotx, float roty, float rotz,
		int planenum, int pointnum ); // 初回用 ： 初期座標付き
	void MakeRotPoint3f( D3DVERTEX* retvert, D3DVERTEX* firstvert, D3DVERTEX& centervert, 
		float rotx, float roty, float rotz ); // center よう 初期化座標付き


	void MakeRotTraPoint3( D3DVERTEX* retvert, D3DVERTEX& centervert, 
		float rotx, float roty, float rotz,
		float trax, float tray, float traz,
		int planenum, int pointnum );
	void MakeRotTraPoint3( D3DVERTEX* retvert, D3DVERTEX& centervert, 
		float rotx, float roty, float rotz, 
		float trax, float tray, float traz ); // center よう

	void MakeRotTraPoint3f( D3DVERTEX* retvert, D3DVERTEX* firstvert, D3DVERTEX& centervert, 
		float rotx, float roty, float rotz,
		float trax, float tray, float traz,
		int planenum, int pointnum ); // 初回用 ： 初期座標付き
	void MakeRotTraPoint3f( D3DVERTEX* retvert, D3DVERTEX* firstvert, D3DVERTEX& centervert, 
		float rotx, float roty, float rotz, 
		float trax, float tray, float traz ); // center よう 初期化座標付き


	void  MakeTraPoints( LOCATION& tra, float* point, 
		float* retpoint, int pointnum );

	int		IsFaced( CHARPOS& pos, D3DVECTOR& loc1, float deg );
	//int		IsFaced( C3dVector& camloc, C3dVector& camdir, D3DVECTOR& targetloc, float deg, BOOL xznormalize );

	//int	  IsFaced( CHARPOS& pos, LOCATION& loc1, float deg );
	//int	  IsFaced( LOCATION& loc0, LOCATION& dir0, LOCATION& loc1, float deg );
	//int	  IsFaced( LOCATION& loc0, float dirdeg, LOCATION& loc1, float deg );
	//int	  IsFaced( LOCATION& loc0, int viewdiry, LOCATION& loc1, float deg );

		// camdir 0, 0, 1 固定で 判定
	int		IsClip( D3DTLVERTEX* testvert, D3DVECTOR* camloc, float deg, int pointnum );


	void	VecNormalizeXZ( LOCATION* loc );
	//void	VecNormalizeXZ( C3dVector* vec );
	void	VecNormalizeXYZ( LOCATION* loc );
	void	VecNormalizeXYZ( D3DVECTOR* vec );
	//void	VecNormalizeXYZ( C3dVector* vec );


	void	CenterLoc( LOCATION* retloc, D3DVERTEX* svert, int pointnum );
	void	CenterLoc( LOCATION* retloc, D3DTLVERTEX* svert, int pointnum );
	void	CenterLoc( LOCATION* retloc, LOCATION* sloc, int pointnum );
	void	CenterVertex( D3DVERTEX* retvert, D3DVERTEX* svert, int pointnum );
	void	CenterVertex( D3DTLVERTEX* retvert, D3DTLVERTEX* svert, int pointnum );

	void	AvaLoc( LOCATION* retloc, LOCATION* sloc1, LOCATION* sloc2 );

	void	DEGPlus( float*, float ); // 足した後に クランプするだけのもの

	void	ClampRGB( RGBDATA* rgb );
	***/
private:
	float sinindex[720];
	float cosindex[720];
	float sqrtindex[1001];
	float atanindexpp[1000];
	float atanindexmp[1000];
	float atanindexpm[1000];
	float atanindexmm[1000];
	float tanindex[720];


	float	rotXorg[16];
	float	rotYorg[16];
	float	rotZorg[16];
	float	rotXYZorg[16];
	float	traorg[16];
	float	scaleorg[16];
	int		rgb500[501];
private:
	void	InitFuncIndex();
	//LOCATION RetRotYLoc( float rotydeg, LOCATION& loc );
	void	InitRGB500();
};
