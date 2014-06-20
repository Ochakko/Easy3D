#define	FROGH	1

#ifndef	KAERUPARTSH
	#include	"kaeruparts.h"
#endif

#ifndef	KAERUCOEFH
	#include	"kaerucoef.h"
#endif
#ifndef	COLORNAMEH
	#include	"colorname.h"
#endif

#ifndef	MATRIXH
	#include	"matrix.h"
#endif

#ifndef	ARRAYMATRIXH
	#include	"ArrayMatrix.h"
#endif

#ifndef	PARAM2MATH
	#include	"Param2Mat.h"
#endif

#define	STRMAX	100

//EDITPOINT!!!	KTLVMAX
/*** DEFINE ***/
#define	KTLVMAX	24
// ユーザ定義したビジュアルのインスタンス毎が持つ情報の構造体。
typedef struct _FrogVisual{
    LPDIRECT3DDEVICE dev;

	// mat は 一つ   tex を tu で切って使う
    LPDIRECT3DMATERIAL mat0;
	D3DMATERIALHANDLE hmat0;

	D3DTEXTUREHANDLE htex;
	//D3DTexture texture;
	D3DTexture*	texture;

}FrogVisual;


class	CFrog
{

public:
	CFrog();
	~CFrog();

	BOOL	bfinishConstruct;

	static	DWORD	WINAPI	CalcFrThread( LPVOID	lpThreadParam );
	
	void	*operator new( size_t size );
	void	operator delete( void *p );

	void	RenderKaeru();
	void	KaeruMove( int sckind );
	void	CamPosSet( int sckind );

	//void	KaeruMultMatrix( CMatrix* multmatrix );
	void	KaeruMultMatrix( int curmove );
	void	MultKaeru( int dispmode );// mainfrmcpp から
	BOOL	PreKaeruRender();
	

	void	CurrentPause2Matrix( CHARPARAM& curparam );
	void	MakeBindMove( char* motfname, int pausecnt );

private:	
	//heap
	static HANDLE	s_hHeap;
	static unsigned int	s_uNumAllocsInHeap;

	//data
	FrogVisual*	m_Visual;
	char	tlvdatadir[STRMAX];
	char motiondir[STRMAX];

	LPDIRECT3DMATERIAL calcmat;
	D3DMATERIALHANDLE calchmat;

	CParam2Mat*	m_p2mat;

	int	sizeInstruction;
	int	sizeState;
	int	sizeProcessv;
	int	sizeTlv;
	int	sizeTriangle;

	LOCATION		kaeruloc, camloc;
	LOCATION		cam2char;
	int			curdirindex;

	D3DCLIPSTATUS		clipstatus;
	D3DTLVERTEX	charrect[2]; // 左上と右下 ｚ は 真ん中
	//D3DTLVERTEX	clipstatus2d[2]; // charrect に trans_cmatrix を かけたもの 

		// clip debug用
	D3DTLVERTEX	testcharrect[4];
	//D3DTLVERTEX	testclipvert[4];

	//LONG			ischgvert;
	LONG		invalidFrog;


	//CMatrix		m_mat4pause[ CTLMAX ];
	
		// NORMAL MOVE 用　MATRIX
	CArrayMatrix*	m_armat; // MYMOVEMAX * CTLMAX の　D3DMATRIX をメンバにもつ

		// MOTION 作成用　MATRIX
	CArrayMatrix*	m_arpause; // CTLMAX の　D3DMATRIX をメンバにもつ


	int	m_matindex[CTLMAX];
	D3DTLVERTEX*	m_ebtlvptr[CTLMAX];
	D3DTLVERTEX*	m_stlvptr[CTLMAX];
	int	m_pnum[CTLMAX];
	int	m_vnum[CTLMAX];
	int	m_ctrlnum;

	CMatrix		m_curtransmatrix;


	CMatrix		bodydir_cmat[9]; // gdir_cmatrix の前後に cenBodyvert の 平行移動
	CMatrix		m_curdirmatrix;


	CMatrix		proj_screen_cmatrix, temp_cmatrix; // extern を コピーして持っておく
	CMatrix		trans_cmatrix;
	CMatrix		world_cmatrix, tra_world_cmatrix, view_cmatrix;
	CMatrix		tra_cmatrix;

	CMatrix		m_vrotmat;

	// rgbdata
	RGBDATA		coldiffuse;
	float		colrate;

	//EDITPOINT!!!	TLVNAME
	/*** TLVERTEX ***/
	D3DTLVERTEX	tlvCTLALL[PNUMCTLALL][VNUMCTLALL];
	D3DTLVERTEX	tlvHEAD0[PNUMHEAD0][VNUMHEAD0];
	D3DTLVERTEX	tlvHEAD00[VNUMHEAD00];
	int	MemNumHEAD00[PNUMHEAD00];
	D3DTLVERTEX	tlvHEAD02[VNUMHEAD02];
	int	MemNumHEAD02[PNUMHEAD02];
	D3DTLVERTEX	tlvARML0[PNUMARML0][VNUMARML0];
	D3DTLVERTEX	tlvARML1[PNUMARML1][VNUMARML1];
	D3DTLVERTEX	tlvARML2[VNUMARML2];
	int	MemNumARML2[PNUMARML2];
	D3DTLVERTEX	tlvLEGL0[PNUMLEGL0][VNUMLEGL0];
	D3DTLVERTEX	tlvLEGL1[PNUMLEGL1][VNUMLEGL1];
	D3DTLVERTEX	tlvLEGL2[VNUMLEGL2];
	int	MemNumLEGL2[PNUMLEGL2];
	D3DTLVERTEX	tlvBody1[PNUMBODY1][VNUMBODY1]; //CTLALL
	D3DTLVERTEX	tlvKtlvheadl[PNUMKTLVHEADL][VNUMKTLVHEADL];// HEAD0 
	D3DTLVERTEX	tlvKtlveyel[VNUMKTLVEYEL];//  HEAD0 
	int	MemNumKtlveyel[PNUMKTLVEYEL];
	D3DTLVERTEX	tlvHEAD01[VNUMHEAD01];
	int	MemNumHEAD01[PNUMHEAD01];
	D3DTLVERTEX	tlvARMR0[PNUMARMR0][VNUMARMR0];
	D3DTLVERTEX	tlvARMR1[PNUMARMR1][VNUMARMR1];
	D3DTLVERTEX	tlvARMR2[VNUMARMR2];
	int	MemNumARMR2[PNUMARMR2];
	D3DTLVERTEX	tlvLEGR0[PNUMLEGR0][VNUMLEGR0];
	D3DTLVERTEX	tlvLEGR1[PNUMLEGR1][VNUMLEGR1];
	D3DTLVERTEX	tlvLEGR2[VNUMLEGR2];
	int	MemNumLEGR2[PNUMLEGR2];
	D3DTLVERTEX	tlvKtlvheadr[PNUMKTLVHEADR][VNUMKTLVHEADR];// HEAD0 
	D3DTLVERTEX	tlvKtlveyer[VNUMKTLVEYER];//  HEAD0 
	int	MemNumKtlveyer[PNUMKTLVEYER];

	/*** JOINT TLV ***/
	int	JindexARML1;
	int	JindexLEGL1;
	int	JindexARMR1;
	int	JindexLEGR1;


	//EDITPOINT!!!	OBJCENTER
	/*** OBJ 中心座標 ***/
	D3DTLVERTEX	cenCTLALL;
	D3DTLVERTEX	cenHEAD0;
	D3DTLVERTEX	cenHEAD00;
	D3DTLVERTEX	cenHEAD02;
	D3DTLVERTEX	cenARML0;
	D3DTLVERTEX	cenARML1;
	D3DTLVERTEX	cenARML2;
	D3DTLVERTEX	cenLEGL0;
	D3DTLVERTEX	cenLEGL1;
	D3DTLVERTEX	cenLEGL2;
	D3DTLVERTEX	cenBody1;
	D3DTLVERTEX	cenKtlvheadl;
	D3DTLVERTEX	cenKtlveyel;
	D3DTLVERTEX	cenHEAD01;
	D3DTLVERTEX	cenARMR0;
	D3DTLVERTEX	cenARMR1;
	D3DTLVERTEX	cenARMR2;
	D3DTLVERTEX	cenLEGR0;
	D3DTLVERTEX	cenLEGR1;
	D3DTLVERTEX	cenLEGR2;
	D3DTLVERTEX	cenKtlvheadr;
	D3DTLVERTEX	cenKtlveyer;


	//***************
	//Execute Buffer
	//***************
	//D3DTLVERTEX	zerotlv;

	LPD3DTLVERTEX	pkaerutlv[ KTLVMAX + 1 ]; //それぞれのparts tlvの 先頭のpointer
	int	ikaerutlv[ KTLVMAX + 1 ];	// それぞれのparts tlvの 先頭のindex
	int	ikaerutri[ KTLVMAX + 1 ];

	//EDITPOINT!!!	EBNUM(DEF)

	/*** VERTNUM FOR EXECUTE BUFFER ***/
	int num_tlvctlall;
	int num_trictlall;

	int num_tlvhead0;
	int num_trihead0;

	int num_tlvhead00;
	int num_trihead00; // InitTlv で初期化 

	int num_tlvhead02;
	int num_trihead02; // InitTlv で初期化 

	int num_tlvarml0;
	int num_triarml0;

	int num_tlvarml1;
	int num_triarml1; // JOINT TRIANGLE を 含む

	int num_tlvarml2;
	int num_triarml2; // InitTlv で初期化 

	int num_tlvlegl0;
	int num_trilegl0;

	int num_tlvlegl1;
	int num_trilegl1; // JOINT TRIANGLE を 含む

	int num_tlvlegl2;
	int num_trilegl2; // InitTlv で初期化 

	int num_tlvbody1;
	int num_tribody1;

	int num_tlvktlvheadl;
	int num_triktlvheadl;

	int num_tlvktlveyel;
	int num_triktlveyel; // InitTlv で初期化 

	int num_tlvhead01;
	int num_trihead01; // InitTlv で初期化 

	int num_tlvarmr0;
	int num_triarmr0;

	int num_tlvarmr1;
	int num_triarmr1; // JOINT TRIANGLE を 含む

	int num_tlvarmr2;
	int num_triarmr2; // InitTlv で初期化 

	int num_tlvlegr0;
	int num_trilegr0;

	int num_tlvlegr1;
	int num_trilegr1; // JOINT TRIANGLE を 含む

	int num_tlvlegr2;
	int num_trilegr2; // InitTlv で初期化 

	int num_tlvktlvheadr;
	int num_triktlvheadr;

	int num_tlvktlveyer;
	int num_triktlveyer; // InitTlv で初期化 

	/** total num **/
	int num_tlvkaeru1; // !!! InitKaeruVertex で 初期化
	int num_trikaeru1;

	int	num_kaeruinst1;// exit の分も忘れずに
	int	num_kaerustate1;
	int	num_kaeruprov1;



	LPDIRECT3DEXECUTEBUFFER	lpKaeruEB1;
	D3DEXECUTEBUFFERDESC	kaeruExeBuffDesc1;
	D3DEXECUTEDATA	kaeruExecuteData1;


	void*	kaerubaseEB; // inst, tridata, uvdata (tlv loc以外)を格納
	void*	calcEB;
	void*	tempEB;

	DWORD	m_dwVertSize;
	DWORD	m_dwInstSize;
	DWORD	m_dwEBSize;
	
	// renderstate, lightstate 用(基本的に データ更新無し)
		// 頂点と いっしょにすると メモリ位置を 複数の場所で管理することになり バグのもと 
			// なので 最初から 逃げ。。。

	LPDIRECT3DEXECUTEBUFFER	lpIKaeruEB;
	D3DEXECUTEBUFFERDESC	ikaeruExeBuffDesc;
	D3DEXECUTEDATA	ikaeruExecuteData;

	int	num_ini_kinst;// exit の分も忘れずに
	int	num_ini_kstate;

		// blend EB
	LPDIRECT3DEXECUTEBUFFER	lpBKaeruEB;
	D3DEXECUTEBUFFERDESC	bkaeruExeBuffDesc;
	D3DEXECUTEDATA	bkaeruExecuteData;

	int	num_bld_kinst;// exit の分も忘れずに
	int	num_bld_kstate;



	//********
	// device
	//********
	LPDIRECT3DDEVICE lpD3DDev;
	LPDIRECT3DVIEWPORT lpD3DView;

private:
	BOOL	InitKaeruVisual();
	BOOL	InitEBVars();
	void	InitDeviceVars();

	BOOL	InitMyVisual();
	//void	SetCharParam();
	BOOL	LoadKaeruMatrix();
	BOOL	LoadKaeruVertex();
	void	SetUpKaeruRes();

	BOOL	InitIniKaeruEB();// executebuffer
	BOOL	FillIniKaeruEB();
	BOOL	InitBldKaeruEB();
	BOOL	FillBldKaeruEB();
	BOOL	InitKaeruEB();
	BOOL	FillKaeruEB();

	void	CpCalc2Temp();
	void	CpTemp2Disp();

	void	ChangeAlpha();

	void	DestroyKaeru();

	BOOL	LoadCharTlvFile( char* filename,
		int dim, int planenum, int pointnum,
		int*	tlvnum, int* trinum, D3DTLVERTEX* cenptr, int* memnumptr, 
		D3DTLVERTEX*	dataptr );


	void	InitMultVars();

	//EDITPOINT!!!	FUNC INIT TLV
	/*** INIT TLVERTEX ***/
	BOOL	LoadTlvCTLALL( char* prename );
	BOOL	LoadTlvHEAD0( char* prename );
	BOOL	LoadTlvHEAD00( char* prename );
	BOOL	LoadTlvHEAD02( char* prename );
	BOOL	LoadTlvARML0( char* prename );
	BOOL	LoadTlvARML1( char* prename );
	BOOL	LoadTlvARML2( char* prename );
	BOOL	LoadTlvLEGL0( char* prename );
	BOOL	LoadTlvLEGL1( char* prename );
	BOOL	LoadTlvLEGL2( char* prename );
	BOOL	LoadTlvBody1( char* prename );
	BOOL	LoadTlvKtlvheadl( char* prename );
	BOOL	LoadTlvKtlveyel( char* prename );
	BOOL	LoadTlvHEAD01( char* prename );
	BOOL	LoadTlvARMR0( char* prename );
	BOOL	LoadTlvARMR1( char* prename );
	BOOL	LoadTlvARMR2( char* prename );
	BOOL	LoadTlvLEGR0( char* prename );
	BOOL	LoadTlvLEGR1( char* prename );
	BOOL	LoadTlvLEGR2( char* prename );
	BOOL	LoadTlvKtlvheadr( char* prename );
	BOOL	LoadTlvKtlveyer( char* prename );



	void	SetKaeruClipStatus();

	void InitPkaeruTlv();
	void	SetEBInst(); // triflag などの instruction の set 

		//svert の yz 平面に関して 対称な 点を retvert に格納
		// ccw で clip するため  inverse round する
	void MirrorTlv2D( D3DTLVERTEX* retvert, D3DTLVERTEX* svert, int planenum, int pointnum );
	void MirrorTlv1D( D3DTLVERTEX* retvert, int* retmemnum,  
							D3DTLVERTEX* svert, int* smemnum, 
							int fannum, int pointnum );

	void	KaeruPosSet();

		// tiranglelist 型 内積が 正のものを indexから はじく
			// normal 配列数は ３角形の数 ！！！！ 頂点の数ではない ！！！
	void	ClipInversFace( LOCATION* snormal, WORD* sindex, int trinum, 
							   WORD* retindex, DWORD* retdrawnum );

	void	ClipInversBody();


	void InitBodyDirMat();

};
