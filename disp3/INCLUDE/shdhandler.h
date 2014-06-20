#ifndef SHDHANDLERH
#define SHDHANDLERH

#include <D3DX9.h>

#include <shdelem.h>

#define DBGH
#include <dbg.h>

#include <MCache.h>
#include <bbox.h>
#include <GPData.h>
//typedef struct tag_boundarybox
//{
//	int setflag;
//	float minx, maxx;
//	float miny, maxy;
//	float minz, maxz;
//} BOUNDARYBOX;

class CTreeHandler2;
class CMotHandler;
//class CNameBank;
class CTexBank;
class CMotionCtrl;
class CQuaternion;
class CQuaternion2;
class CInfScope;
class CMOQMaterial;
class CDispSwitch;
class CVec3List;
class CPanda;

class CShdHandler
{
public:
	CShdHandler( CTreeHandler2* srcthandler );
	~CShdHandler();

/***
#ifdef _DEBUG
	void	*operator new( size_t size );
	void	operator delete( void *p );
#endif
***/

	CShdElem* operator() ( int srcserino ){
#ifdef _DEBUG
		if( !CheckNo( srcserino ) ){
			return *(s2shd + srcserino);
		}else{
			DbgOut( "CShdHandler : operator () : serino error %d : s2shd_leng %d !!!\n",
				srcserino, s2shd_leng );

			_ASSERT( 0 );
			return 0;
		}
#else
	return *(s2shd + srcserino);
#endif
	};

	int CheckNo( int srcseri ){
		// root elem　(dummy elem) は　s2shd_leng が 0 なので、参照禁止???
		if( (srcseri >= 0) && (srcseri < s2shd_leng) ) 
			return 0;
		else
			return 1;
	};

	int InitParams( CTreeHandler2* srcthandler );


	// TreeElemからchain、表の引継ぎ
	//int CreateShdChain();
	int AddShdElem( int srcserino, int srcdepth );

	int SetChain( int offset = 0 );
	int SetColors();// hascolor == 0 obj へのmaterial のセット
	int SetClockwise();
	int SetClockwise( int srcseri, int cw );
	int ClearInvFaceCnt();

	int CalcDepth();

	int SetTexName( int srcserino, char* srctexname );
	int SetTexName();

	int SetCurTexname( int srcseri );
	int SetExtTex( int srcserino, unsigned char srctexnum, unsigned char srctexmode, unsigned char srctexrep, unsigned char srctexstep, char** srctexname );
	

	int SetTexRule( int srcserino, int srcrule );
	int SetNoSkinning( int srcserino, int srcint );
	int SetAllNoSkinning( int srcint );

	int SetDispFlag( int boneflag );
	int SetIsSelected( int selno, int infscopevisible, int pmsonomama, int curboneonly = 0 );

	int SetInfluenceList( int srcserino, int* listptr, int listleng );
	int SetIgnoreList( int srcserino, int* listptr, int listleng );

	// 各shdelemの初期化
	int	Init3DObj( int srcserino, CMeshInfo* srcmeshinfo );

	int InvalidateDispObj();
	int InvalidateTexture( int resetflag = 0 );
	int InvalidateBoneElem();

	int CreateBoneInfo( CMotHandler* lpmh ); //joint情報からBone情報へ変換
	int DestroyBoneInfo( CMotHandler* lpmh );
	//int CreateBoneElem(); //頂点単位のボーン情報の作成
	//int CalcInfElem(); //BoneElemの計算しなおし。
	int CalcInfElem( int srcseri, int forceflag, int excludeflag = 0, int onlymfolder = 0 );//exclude --> notuseを除外
	int CalcInfElem1Vert( int srcseri, int srcvert );

	int CreateDispObj( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, int needcalcbe );
	//int CreateMorphObj( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh );
	int CreateTexture( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int neededbone, UINT miplevels, DWORD mipfilter, int dispwarnflag, int billboardflag );
	int CreateTextureFromPnd( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int neededbone, UINT miplevels, DWORD mipfilter, int dispwarnflag, CPanda* panda, int pndid );

	int AddTexture( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int srcseri, UINT miplevels, DWORD mipfilter );

	int DestroyTexture();
	int SetTextureAlpha( LPDIRECT3DTEXTURE9 lptexture, int isbone );

	int ChangeJointLoc( int serino, CVec3f* newloc );

	int CreateAfterImage( LPDIRECT3DDEVICE9 pd3dDevice, int imagenum );
	int DestroyAfterImage();

	int SetBMesh( int bmseri, int blseri, int brono );
	int SetInvFace( int srcseri, int srcif );
	int SetBDivUV( int srcseri, int srcbdivU, int srcbdivV );
	int SetTexTransparent();

	int SetAlpha( float alpha, int srcseri, int updateflag );
	int SetDirectionalLight( D3DXVECTOR3 dir );

	// check

	int DumpBoneInfo( CTreeHandler2* srclpth );
	int	DumpShd( CMotHandler* lpmh, int dumpflag, char* fname = 0 );
	

	int	DumpText( HANDLE hfile, int serialno, int type );


	//int ChkInView( CMotHandler* lpmh, D3DMATRIX matWorld, D3DXVECTOR3 viewpos, D3DXVECTOR3 viewvec, float projnear, float projfar, float* projindex );
	int ChkInView( CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, int* retptr = 0 );

//	int TransformDispData( D3DXVECTOR3 vEyePt, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, 
//		D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, 
//		int projmode, int lightflag, int* bonearray = 0, int tra_boneonly = 0, int divmode = 0 );
	int TransformDispData( D3DXVECTOR3 vEyePt, CMotHandler* lpmh, 
		D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, 
		int lightflag, int tra_boneonly );


	int TransformAndRenderIKT( D3DXVECTOR3 vEyePt, LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh,
			CShdHandler* lpmodelsh, CMotHandler* lpmodelmh,
			CShdHandler* lpbmsh,
			int motid, int frameno,
			D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, D3DXMATRIX matProjX, GUARDBAND gband );


	int TransformBillboardDispData( D3DXMATRIX matWorld, D3DXMATRIX matWorld1, D3DXMATRIX matWorld2, D3DXMATRIX matView, D3DXMATRIX matProj, D3DXVECTOR3 camerapos );


	//void TransformDispDataReq( CShdElem* selem, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, 
	//	D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband,
	//	int* transcnt, int* errorflag, int broflag = 1 );

	int CalcSWVMat( CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView );


	int Render( int znflag, int inbatch, LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, int polydispmode, CMotHandler* lpmh, int srcmotcookie, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, D3DXVECTOR3 vEyePt, D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2, int glowflag );
	int RenderShadowMap0( LPDIRECT3DDEVICE9 pd3dDevice, int polydispmode, CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3 vEyePt, D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2 );
	int RenderShadowMap1( LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, int polydispmode, CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3 vEyePt, D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2 );
//	int RenderBone( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXSPRITE pd3dxsprite, float jointsize, int jointalpha );
	int RenderBone( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXSPRITE pd3dxsprite, float jointsize, int jointalpha,
		DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh, 
		D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj );
	int RenderMotionBlur( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh );


//	int RenderSelVert( CMotHandler* lpmh, LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXSPRITE pd3dxsprite, 
//		int selpart, int* pselvert, int selvertnum, int brushtype,
//		DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj  );
	int RenderSelVert( CMotHandler* lpmh, LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXSPRITE pd3dxsprite, 
		int selpart, int* pselvert, int selvertnum, int brushtype,
		int markpart, int markvert,
		DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj  );

	int RenderBoneMark( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, COLORREF srcselcol, COLORREF srcunselcol, int srcselectno );

	int UpdateVertexBuffer( int framecnt );

	int InitColor( D3DXVECTOR3 vEyePt, CMotHandler* lpmh, int serino );

	//for dbg
	//int CheckMinZObj( int* dstno, D3DXVECTOR3* dstvec );
	int CheckRestVert();
	int DebugPolymeshM();


	//int PickBone( int srcx, int srcy );
	int PickBone( CMotHandler* srclpmh, int srcx, int srcy, DWORD dwWidth, DWORD dwHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float srcaspect );

	CShdElem* FindUpperJoint( CShdElem* curelem, int findnotuse = 0 );

	int GetBoneVec( D3DXVECTOR3* dstvec, int srcseri, int srcparseri );

	int CreateBSphereData( int motkindnum, int framenum );
	int SetBSphereData( CMotHandler* mhptr, int motcookie, int startframe, int endframe );
	int SetBSphereDataOnFrame( CMotHandler* mhptr, int motcookie, int frameno, DWORD curdispswitch, CBSphere* dsttotalbs );
	int RemakeBSphereData( CMotHandler* lpmh, int motid, int frameleng );



	int ChkConfPartsBySphere( CMotHandler* srcmh, CShdHandler* chksh, CMotHandler* chkmh, int* confflag, float srcrate, float chkrate, int* confidptr, int arrayleng, int* confnumptr );
	int ChkConfPartsBySphere( CMotHandler* srcmh, D3DXVECTOR3 srccenter, float srcr, int* confflagptr, float srcrate, float chkrate, int* confidptr, int arrayleng, int* confnumptr );
	int ChkConfPartsBySphere( CMotHandler* srcmh, CShdHandler* chksh, CMotHandler* chkmh, int chkpartno, int* confflagptr, float srcrate, float chkrate, int* confidptr, int arrayleng, int* confnumptr );

	//int ChkConfParts( CShdHandler* chksh, int chkno, int* confflagptr );
	int ChkConfParts( CMotHandler* srcmh, CShdHandler* chksh, CMotHandler* chkmh, int chkno, int* confflagptr );

	int ChkConfBBX( int srcpartno, CShdHandler* chksh, int chkpartno, int* confflag );


	int SetGroundObj( CMotHandler* srcmh, D3DXMATRIX matWorld );
	int ChkConfGround( int* partarray, int partnum, D3DXMATRIX* matWorld, D3DXVECTOR3 bpos, D3DXVECTOR3 npos, CMotHandler* lpmh, int mode, float diffmaxy, float mapminy, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv, DISTSAMPLE* ds = 0, CONFDATA* retcf = 0 );
	int ChkConfGroundPart( D3DXMATRIX* matWorld, int groundpart, D3DXVECTOR3 bpos, D3DXVECTOR3 npos, CMotHandler* lpmh, int mode, float diffmaxy, float mapminy, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv, DISTSAMPLE* ds = 0, CONFDATA* retcf = 0 );
	int ChkConfWall( int* partarray, int partnum, D3DXMATRIX* matWorld, D3DXVECTOR3 bpos, D3DXVECTOR3 npos, CMotHandler* lpmh, float srcdist, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv );
	
	int ChkConfFace( int* partarray, int partnum, D3DXVECTOR3 bpos, D3DXVECTOR3 npos, D3DXVECTOR3 bposwv, D3DXVECTOR3 nposwv, CMotHandler* lpmh, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv, CONFDATA* retcf, DISTSAMPLE* ds );



	int ChkConfVecAndGroundParts( int groundonly, D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpartsnum, CShdElem** confparts, int confmaxnum );



	int ChkConfLineAndFace( DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView,
		D3DXVECTOR3 srcpos1, D3DXVECTOR3 srcpos2, int needtrans, int* partptr, int* faceptr, D3DXVECTOR3* dstpos, D3DXVECTOR3* dstn, int* revfaceptr );
	int ChkConfLineAndFacePart( int chkpartno, DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView,
		D3DXVECTOR3 srcpos1, D3DXVECTOR3 srcpos2, int needtrans, int* partptr, int* faceptr, D3DXVECTOR3* dstpos, D3DXVECTOR3* dstn, int* revfaceptr );

	int ChkConfLineAndFaceSelDisp( int seldisp, DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView,
		D3DXVECTOR3 srcpos1, D3DXVECTOR3 srcpos2, int needtrans, int* partptr, int* faceptr, D3DXVECTOR3* dstpos, D3DXVECTOR3* dstn, int* revfaceptr );

	int ChkConfLineAndFaceInfScope( int isindex, DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj,
		D3DXVECTOR3 srcpos1, D3DXVECTOR3 srcpos2, int* faceptr, D3DXVECTOR3* dstpos, D3DXVECTOR3* dstn, int* revfaceptr );



	int GetVertNumOfPart( int partno, int* vertnumptr );
	int GetVertNumOfPartInfScope( int isindex, int* vertnumptr );
	int GetVertPos( int partno, int vertno, D3DXVECTOR3* vpos );
	int GetVertPosInfScope( CInfScope* curis, int vertno, D3DXVECTOR3* posptr );
	int GetVertPos2( CMotHandler* lpmh, D3DXMATRIX matWorld, int motid, int frameno, int partno, int vertno, D3DXVECTOR3* dstpos, int scaleflag );
	int GetVertPos2InfScope( int isindex, CMotHandler* lpmh, D3DXMATRIX matWorld, int motid, int frameno, int vertno, D3DXVECTOR3* dstpos, int scaleflag );
	int GetVertPos3( int kind, CMotHandler* lpmh, D3DXMATRIX matWorld, int partno, int vertno, D3DXVECTOR3* vpos );



	int SetVertPos( CMotHandler* mhptr, int partno, int vertno, D3DXVECTOR3 vpos );
	int SetVertPosBatchDouble( CMotHandler* mhptr, int partno, int* vnoarray, int vnum, double* varray, int aorder );
	int SetVertPosBatchVec3( CMotHandler* mhptr, int partno, int* vnoarray, int vnum, D3DXVECTOR3* varray );
	int SetVertPosInfScope( CInfScope* curis, CMotHandler* mhptr, int vertno, D3DXVECTOR3 vpos );
	int SetVertPos2( CMotHandler* lpmh, int motid, int frameno, int partno, int vertno, D3DXVECTOR3 targetobj );
	int SetVertPos2InfScope( int isindex, CMotHandler* lpmh, int motid, int frameno, int vertno, D3DXVECTOR3 targetobj );

	int SortBillboard( D3DXVECTOR3 vDir );
	int SetBillboardPos( int bbid, float posx, float posy, float posz );
	int RotateBillboard( int bbid, float fdeg, int rotkind );
	int SetBillboardUV( int bbid, int unum, int vnum, int texno, int revuflag );
	int SetBillboardSize( int bbid, float width, float height, int dirmode, int orgflag );
	int SetBillboardDispFlag( int bbid, int flag );
	int DestroyBillboard( int bbid, CTreeHandler2* lpth, CMotHandler* lpmh );

	int SetWallOnGround( D3DXMATRIX* wallmatWorld, D3DXMATRIX* gmatWorld, CShdHandler* gsh, CMotHandler* gmh, float mapmaxy, float mapminy, float wheight );
	int MagnetPosition( float dist );

	int SetValidFlag( int srcseri, int srcflag );
	int SetDispDiffuse( int srcseri, int srcr, int srcg, int srcb, int setflag, int vertno, int updateflag );
	int SetDispSpecular( int srcseri, int srcr, int srcg, int srcb, int setflag, int vertno );
	int SetDispAmbient( int srcseri, int srcr, int srcg, int srcb, int setflag, int vertno );
	int SetDispEmissive( int srcseri, int srcr, int srcg, int srcb, int setflag, int vertno );
	int SetDispSpecularPower( int srcseri, float srcpow, int setflag, int vertno );

	int SetMaterialAlpha( int matno, float srcalpha );
	int SetMaterialDiffuse( int matno, int setflag, int srcr, int srcg, int srcb );
	int SetMaterialSpecular( int matno, int setflag, int srcr, int srcg, int srcb );
	int SetMaterialAmbient( int matno, int setflag, int srcr, int srcg, int srcb );
	int SetMaterialEmissive( int matno, int setflag, int srcr, int srcg, int srcb );
	int SetMaterialPower( int matno, int setflag, float srcpow );
	int SetMaterialBlendingMode( int matno, int mode );
	int SetMaterialAlphaTest( int matno, int alphatest0, int alphaval0, int alphatest1, int alphaval1 );
	int GetMaterialAlphaTest( int materialno, int* test0ptr, int* val0ptr, int* test1ptr, int* val1ptr );
	int Material2Color();


	int GetDiffuse( int srcseri, int vertno, int* rptr, int* gptr, int* bptr );
	int GetAmbient( int srcseri, int vertno, int* rptr, int* gptr, int* bptr );
	int GetSpecular( int srcseri, int vertno, int* rptr, int* gptr, int* bptr );
	int GetEmissive( int srcseri, int vertno, int* rptr, int* gptr, int* bptr );
	int GetSpecularPower( int srcseri, int vertno, float* powerptr );
	int GetAlpha( int srcseri, int vertno, int* aptr );
	int GetAlphaF( int srcseri, int vertno, float* aptr );

	int GetMaterialNo( int partno, int faceno, int* matnoptr );// optindexbufを元にセット RDB Plugin用
	int GetMaterialNo2( int partno, int faceno, int* matnoptr );// d3ddispのindexを元にセット　E3D用

	int CreateAndSaveTempMaterial();
	int RestoreAndDestroyTempMaterial();


	int SetBlendingMode( int srcseri, int bmode );

	int SetElemRenderState( int srcseri, int statetype, DWORD value );
	int SetElemTextureMinMagFilter( int srcseri, DWORD minfilter, DWORD magfilter );
	int SetDispScale( int srcseri, D3DXVECTOR3 scalevec, int centerflag, D3DXMATRIX matWorld );

	int GetFirstJoint( CMotionCtrl** dstjoint, CMotHandler* lpmh, int needbone );

	int GetScreenPos( int bbflag, CMotHandler* lpmh, int srcseri, int vertno, int calcmode, int* scxptr, int* scyptr, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj );
	int GetScreenPos3( int bbflag, CMotHandler* lpmh, int srcseri, int vertno, int calcmode, int* scxptr, int* scyptr, float* sczptr, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int outflag = 0 );
	int GetScreenPos4( int bbflag, CMotHandler* lpmh, int srcseri, int vertno, int calcmode, float* scxptr, float* scyptr, float* sczptr, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj );
	int GetScreenPos3F( int bbflag, CMotHandler* lpmh, int srcseri, int vertno, int calcmode, DWORD dwClipWidth, DWORD dwClipHeight,
		D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, D3DXVECTOR3* scpos );
	int GetScreenPosInfScope( CMotHandler* lpmh, int isindex, int vertno, int* scxptr, int* scyptr, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj );

	int GetBonePos( int boneno, int poskind, int motid, int frameno, D3DXVECTOR3* dstpos, CMotHandler* lpmh, D3DXMATRIX matWorld, int scaleflag );
	int GetCurrentBonePos( int boneno, int poskind, D3DXVECTOR3* dstpos, CMotHandler* lpmh, D3DXMATRIX matWorld );

	int SetExtLineColor( int srcseri, int a, int r, int g, int b );
	int AddPoint2ExtLine( int previd, int* newidptr );
	int DeletePointOfExtLine( int pid );
	int SetPointPosOfExtLine( int pid, D3DXVECTOR3 srcpos );
	int GetPointPosOfExtLine( int pid, D3DXVECTOR3* dstpos );
	int GetNextPointOfExtLine( int previd, int* nextptr );
	int GetPrevPointOfExtLine( int pid, int* prevptr );
	int RestoreExtLineColor();


	//int PickVert( CMotHandler* srclpmh, int posx, int posy, int rangex, int rangey, int* partptr, int* vertptr, int arrayleng, int* getnumptr );
	int PickVert( CMotHandler* srclpmh, DWORD dwClipW, DWORD dwClipH, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj,
		float srcaspect, int posx, int posy, int rangex, int rangey, int* partptr, int* vertptr, int arrayleng, int* getnumptr );
	int PickVertSelDisp( int seldisp, CMotHandler* srclpmh, DWORD dwClipW, DWORD dwClipH, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj,
		float srcaspect, int posx, int posy, int rangex, int rangey, int* partptr, int* vertptr, int arrayleng, int* getnumptr );

	//int PickVert2( int part, int face, CMotHandler* srclpmh, int posx, int posy, int rangex, int rangey, int* partptr, int* vertptr, int arrayleng, int* getnumptr );
	//int PickVert3( D3DXVECTOR3 campos, CMotHandler* srclpmh, int posx, int posy, int rangex, int rangey, int* partptr, int* vertptr, int arrayleng, int* getnumptr );
	//int PickVertInfScope( int isindex, D3DXVECTOR3 campos, CMotHandler* srclpmh, int posx, int posy, int rangex, int rangey, int* vertptr, int arrayleng, int* getnumptr );
	int PickVertInfScope( int isindex, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh,
		D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float srcaspect,
		int posx, int posy, int rangex, int rangey, int* vertptr, int arrayleng, int* getnumptr );

	int PickFace( float maxdist, DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, 
		D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, 
		int* partarray, int partnum, int pos2x, int pos2y, int* partptr, int* faceptr, 
		D3DXVECTOR3* pos3ptr, D3DXVECTOR3* nptr, float* distptr, int calcmode );
	int PickInfScopeFace( int isindex, float maxdist, DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int pos2x, int pos2y, int* faceptr, D3DXVECTOR3* pos3ptr, D3DXVECTOR3* nptr, float* distptr );

	int GetCullingFlag( CMotHandler* lpmh, int srcseri, int vertno, int* viewcullptr, int* revcullptr );
	int GetOrgVertNo( int srcseri, int vertno, int* orgnoarray, int arrayleng, int* getnumptr );


	int CheckOrgNoBuf( int* nocntptr );
	int CheckBoneNum( int* bonenumptr );
	int GetJointNum( int* numptr );

	int EnableTexture( int partno, int enableflag );
	int SetTextureDirtyFlag( int srcseri );

	int IsValidJoint( int jno );

	int GetBBox( int partno, int mode, CBBox* dstbb );
	int GetVertNoOfFace( int partno, int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr );
	int GetVertNoOfFaceInfScope( int isindex, int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr );
	int GetSamePosVert( int partno, int vertno, int* sameptr, int arrayleng, int* samenumptr );

	int GetFirstScale( D3DXMATRIX* firstscale );

	//int HuGetBoneWeight( int partno, int vertno, int* boneno1, int* boneno2, float* rate1 );
	int GetTriIndices( int partno, WORD* buf, int wordnum, int* setwordnum, int* revflag );
	int GetTriIndicesInt( int partno, int* buf, int intnum, int* setintnum, int* revflag );
	//int GetUV( int partno, int vertno, float* uptr, float* vptr );

	int HuGetOffsetMatrix( int boneno, D3DXMATRIX* offsetptr, float mult = 1.0f, int isstandard = 0 );

	int GetIKTransFlag( int jointno, int* flagptr );
	int SetIKTransFlag( int jointno, int flag );

	int GetUserInt1OfPart( int partno, int* userint1ptr );
	int SetUserInt1OfPart( int partno, int userint1 );

	int GetBSphere( int partno, D3DXVECTOR3* dstcenter, float* dstr );

	int GetChildJoint( int parentno, int arrayleng, int* childarray, int* childnum );

	int SetCurrentPose2OrgData( CMotHandler* srclpmh, CQuaternion* multq );
	int ConvSymmXShape();

	int ChangeJointInitialPos( int jointno, D3DXVECTOR3 target );

	int FindNearestBoneFromAnchor( CShdElem* srcanchor, CShdElem** ppnearbone );
	int SetMikoAnchorApply( CTreeHandler2* thandler );

	int SetUV( int partno, int vertno, float fu, float fv, int setflag, int clampflag = 0, int updateflag = 1 );
	int SetUVTile( int partno, int texrule, int unum, int vnum, int tileno );
	int SetUVBatchDouble( int partno, int* vertnoptr, int setnum, double* uvptr, int setflag, int clampflag );
	int SetUVBatchUV( int partno, int* vertnoptr, int setnum, UV* uvptr, int setflag, int clampflag );

	int GetUV( int partno, int vertno, float* uptr, float* vptr );

	int FillDispColor( int srcseri, DWORD srccol );

	int GetInitialFacePos( int part, int face, D3DXVECTOR3* facepos, D3DXMATRIX matWorld );
	int GetInitialPartPos( int part, D3DXVECTOR3* partpos, D3DXMATRIX matWorld );
	int GetInitialInfScopeCenter( int isindex, D3DXVECTOR3* partpos, D3DXMATRIX matWorld );

	int GetFaceNum( int partno, int* facenum );

	int GetParent( int partno, int* partnoptr );
	int GetChild( int partno, int arrayleng, int* childnoptr, int* childnumptr );

	int GetInfElemNum( int partno, int vertno, int* infnumptr );
	int GetInfElemParam( int partno, int vertno, int infno, int* bonenoptr, float* rateptr );
	int GetInfElemParam2( int partno, int vertno, int infno, int* bonenoptr, int* childnoptr, float* rateptr );
	int GetInfElem( int partno, int vertno, int infno, INFELEM** ppIE );

	int SetInfElemSymParams( int partno, int vertno, int srcsymaxis, float srcsymdist );
	int GetInfElemSymParams( int partno, int vertno, int* dstsymaxis, float* dstsymdist );
	int GetInfElemCalcmode( int partno, int vertno, int infno, int* dstcalcmode );
	int SetInfElemCalcmode( int partno, int vertno, int infno, int srccalcmode );


	int ChangeColor( CTreeHandler2* dstth, CShdHandler* srcsh, CTreeHandler2* srcth );

	int DeleteJoint( int delno, CTreeHandler2* lpth, CMotHandler* lpmh );
	int DeleteDispObj( int delno, CTreeHandler2* lpth, CMotHandler* lpmh );
	int DeletePart( int delno, CTreeHandler2* lpth, CMotHandler* lpmh );
	

	int Anchor2InfScope( CTreeHandler2* lpth, CMotHandler* lpmh );
	CShdElem* GetInfScopeElem();
	CShdElem* GetInfScopeElem( int offset );
	int InfScopeConvTempApplyChild();
	int ExistInfScope( int srcboneno, int srcdispno );
	int ExistInfScopeByTarget( int targetno );

	int MeshConv2InfScope( CTreeHandler2* lpth, CMotHandler* lpmh, int srcboneno, int srcdispno, int srcconvno );
	int SetInfScopeDispFlag( int srcindex, int srcflag, int exclusiveflag );

	int CreateInfScopePM2( int srcapplychild, int srctarget );

	int GetBone3Vec( int childno, D3DXVECTOR3* vecx, D3DXVECTOR3* vecy, D3DXVECTOR3* vecz );
	int GetInitialBoneQ( int childno, CQuaternion* dstq );


	int CreateShdBBoxIfNot( CTreeHandler2* lpth, CMotHandler* lpmh );
	CShdElem* GetBBoxElem();
	CShdElem* GetBBoxElem( int offset );




	int SetMaterialFromInfElem( int srcchildno, int srcseri, int srcvert, E3DCOLOR3UC* infcolarray = 0 );

	int SetInfElem( int srcseri, int srcvert, int srcchildno, int paintmode, int calcmode, float rate, int normflag, float directval, int updateflag = 1 );
	int DeleteInfElem( int srcseri, int srcvert, int srcchildno, int normflag, int updateflag = 1 );
	int DestroyIE( int srcseri, int srcvert );
	int AddInfElem( int srcseri, int srcvert, INFELEM srcIE );
	int NormalizeInfElem( int srcseri, int srcvert );

	int GetSymVert( int srcseri, int srcvert, int symaxis, float symdist, int* sympart, int* symvert );

	int GetVisiblePolygonNum( CMotHandler* lpmh, int* num1ptr, int* num2ptr );

	int CreateToon1Buffer( LPDIRECT3DDEVICE9 pdev, int forceRemakeflag = 0 );
	int CreateMaterialBlock( LPDIRECT3DDEVICE9 pdev );

	void CalcXTransformationMatrixReq( CShdElem* curselem, D3DXMATRIX parenttra, float mult, int isstandard = 0 );
	CShdElem* GetValidBrother( CShdElem* selem );
	CShdElem* GetValidChild( CShdElem* selem );

	int RepairInfElem( LPDIRECT3DDEVICE9 pdev );

	int SaveToDispTempDiffuse();
	int RestoreDispTempDiffuse();

	int CreateSkinMat( int srcsri, int updateflag );
	int EnableToonEdge( int srcpartno, int srctype, int srcflag );
	int SetToonEdge0Color( int srcpartno, char* srcname, int srcr, int srcg, int srcb );
	int SetToonEdge0Width( int srcpartno, char* srcname, float srcwidth );
	int GetToonEdge0Color( int srcpartno, char* srcname, float* dstr, float* dstg, float* dstb );
	int GetToonEdge0Width( int srcpartno, char* srcname, float* dstwidth );

	int SetIndexBufColP();

	int GetFaceNormal( int partno, int faceno, D3DXMATRIX matWorld, CMotHandler* lpmh, D3DXVECTOR3* dstn );

	int InitMQOMaterial();
	int InitGroundBMPMaterial( char* texname );
	CMQOMaterial* GetLastMaterial();
	CMQOMaterial* GetMaterialFromNo( CMQOMaterial* mathead, int matno );

	int ChkAlphaNum();
	int GetMaterialNoByName( char* srcname, int* matnoptr );

	int SetCurDS();

	int EnablePhongEdge0( int flag );
	int SetPhongEdge0Params( int r, int g, int b, float width, int srcblendmode, float srcalpha );
	int SetDirName( char* pathname );
	int InitTempDS();

	int SetPointListForShadow( CVec3List* plist );
	int SetShadowInterFlag( int partno, int srcflag );

	int CalcZa4LocalQ( CMotHandler* srcmh, int srcmotid, int srcframeno );

	int ConvMorphElem();
	int GetMorphElem( CShdElem** ppselem, int* numptr, int arrayleng );
	int GetMorphElem( CShdElem** ppselem, int baseid );
	int SetMorphDispFlag( int flag );

	int GetMorphObj( CShdElem* srcbase, CMorph** dstmorph );

	int SetMotionBlur( int mode, int blurtime );
	int SetBeforeBlur( int bbflag );
	int InitBeforeBlur();
	int SetMotionBlurMinAlpha( int partno, float minalpha );
	int SetMotionBlurMaxAlpha( int partno, float maxalpha );
	int SetMaterialGlowParams( int matno, int glowmode, int setcolflag, E3DCOLOR4F* colptr );
	int GetMaterialGlowParams( int matno, int* glowmodeptr, E3DCOLOR4F* colptr );
	int SetMaterialTransparent( int matno, int transparent );
	int GetMaterialTransparent( int matno, int* transptr );

	int GetDestroyedSeri( int* seriptr );
	int GetDestroyedNum( int* numptr );
	int GetFirstParentNo( int* parnoptr );

	int CreateMorphTable();

	int SetMikoBonePos( int boneno, D3DXVECTOR3 bdir );

	int GetTopJoints( int arrayleng, CShdElem** dstarray, int* getnum );

private:
	//int CreateS2Shd();
	//int CreateD2Shd();
	//int SetChain( CShdElem* newshd );
	int AddS2Shd( int srcserino, int srcbrono, int srcdepth, int srctype );
	//int AddD2Shd( int srcserino, int srcdepth );

	void SetIsSelectedReq( CShdElem* srcselem, int setbroflag );
	int SetMorphDispFlag();

	void SetBoneNoReq( CMotHandler* lpmh, CShdElem* selem, int* bonenoptr );
	void SetOldBoneNoReq( CMotHandler* lpmh, CShdElem* selem, int* bonenoptr );
	void SetMorphNoReq( CMotHandler* lpmh, CShdElem* selem, int* morphnoptr );
	
	int ConvertInfluenceList( CMotHandler* lpmh );

	int InitBoundaryBox();
	int SetBoundaryBox();
	int SetTBSBBox();

	int SetRenderState( LPDIRECT3DDEVICE9 pdev, CShdElem* selem );

	int UpdateFrustumInfo( D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj );
	int CalcMeshWorldv( CMotHandler* srcmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX* firstscale );

	int LeaveOneElemFromChain( int delno, CMotHandler* lpmh );

	int LookAtQ( CQuaternion2* dstqptr, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int divnum, int upflag );
	int DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec );

	void DestroyMQOMaterial();


public:
	static HANDLE	s_hHeap;
	static unsigned int	s_uNumAllocsInHeap;

	char m_dirname[ PATH_LENG ];

	int isfirst; // 初回のAddShdElemでクリア
	CTreeHandler2* thandler;

	int s2shd_allocleng;
	int s2shd_leng;
	CShdElem** s2shd;

	CShdElem* m_bbxelem;

	//int depthmax;
	//int* d2shdnum;
	//CShdElem*** d2shd;

	//CNameBank* texnbk;
	//CTexBank* texbnk;

	//BOUNDARYBOX	m_bbox;
	CBBox		m_bbox;//古いbbox : 使用しない。
	CBBox		m_bbx;//新しいあたり判定用のbbox, m_bboxは、使わない。

	int m_lastboneno;
	int m_lastoldboneno;

	float m_aspect;

	int m_inRDBflag;

	CMCache m_mc;

	D3DXVECTOR3 m_center;
	D3DXVECTOR3 m_orgcenter;//scaleを考慮しない中心

	int m_TLmode;

	int m_im2enableflag;

	FRUSTUMINFO m_frinfo;

	int m_bonetype;

	DWORD m_curdispswitch;

	int m_mikoblendtype;

	int m_shader;
	ARGBF m_scene_ambient;
	int m_overflow;

	int m_sigmagicno;

	int* m_boneno2seri;
	int* m_seri2boneno;//!!!! 存在しない場合は、(bonenoの最大値 + 1) == m_lastboneno !!!!


	CMQOMaterial* m_mathead;
	int m_materialcnt;

	CDispSwitch* m_ds;//モーションのDS。
	CDispSwitch* m_ds2;//モーションを適用していない時のDS。E3Dでセットする。
	CDispSwitch* m_dsF;//force set : プログラムで強制セット
	CDispSwitch* m_curds;//m_ds か　m_ds2かのポインタがセットされる。
	CDispSwitch* m_tempds;//計算用のDS


	int m_phongedge0enable;
	float m_phongedge0[4];
	int m_phongedge0Blend;
	float m_phongedge0Alpha;

	int m_bbtransskip;

	CGPData m_gpdata;

	int m_mnum;//morph table
	CShdElem** m_ppm;//morph table

};

#endif
