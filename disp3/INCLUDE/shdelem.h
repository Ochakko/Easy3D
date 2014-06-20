#ifndef SHDELEMH
#define SHDELEMH

#include <D3DX9.h>

#include <coef.h>

#include <vecmesh.h>
#include <bezmesh.h>
#include <planes.h>
#include <part.h>
#include <morph.h>

#include <polymesh.h>
#include <polymesh2.h>
#include <grounddata.h>
#include <billboard.h>

#include <treehandler2.h>

#include <bsphere.h>
#include <bbox.h>

#define SE_BLOCKLENG	50

class CShdHandler;
class CMotHandler;
class CD3DDisp;
class CBezMesh;
class CMotionCtrl;
class CTexBank;
class CMQOMaterial;
class CExtLine;

class CMCache;
class CPickData;
class CInfScope;
class CBoneInfo;

class CBBox2;

class CShdElem : public CBaseDat
{
public:
	CShdElem( int srcserino );
	~CShdElem();
	
	//void	*operator new( size_t size );
	//void	operator delete( void *p );

	void SetHeapHandle( HANDLE srcheap );

	int InitParams( int srcserino );
	int	Init3DObj( CMeshInfo* srcmeshinfo );
	int CreateDispObj( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CheckDispData();
	//int CreateBoneElem( CShdHandler* srclpsh );
	int CalcInfElem( CShdHandler* srclpsh );
	int CalcInfElem1Vert( CShdHandler* srclpsh, int srcvert );
	int SetUV( CMQOMaterial* mathead );

	int CreateAfterImage( LPDIRECT3DDEVICE9 pd3dDevice, int imagenum );
	int DestroyAfterImage();
	
	int InvalidateDispObj();
	int ResetTotalPrim();

		// 3dobj の　set
	int SetChild( CShdElem* chilptr );
	int SetBrother( CShdElem* broptr );
	int CopyChain( CShdHandler* shdhandler, CTreeElem2* curte );

	int LeaveFromChain();
	//int LeaveFromChain( int delchildflag );
	int PutToChain( CShdElem* newparent, int add2lastflag = 0 );
	
	void CalcDepthReq( int srcdepth );

	int SetDepth( int srcdepth );
	int SetClockwise( int cw );
	void SetClockwiseReq( int addcnt );
	int ClearInvFaceCnt();

	int SetBDivUV( int srcdivU, int srcdivV );
	int SetTexName( char* srctexname );
	int SetTexRepetition( int srcrepx, int srcrepy );
	int SetTexTransparent( int srctp );
	int SetExtTex( unsigned char srctexnum, unsigned char srctexmode, unsigned char srctexrep, unsigned char srctexstep, char** srctexname );

	int SetAlpha( float srcalpha );
	int SetUVAnime( float srcuanime, float srcvanime );
	int SetDispFlag( int srcdflag );
	
	int SetTexRule( int srcrule );
	int SetNotUse( int srcint );
	int SetNoSkinning( int srcint );
	int SetDispSwitchNo( DWORD srcdw );

	int SetDirectionalLight( D3DXVECTOR3 dir );

	int SetMem( int* srcint, __int64 setflag );
	
	int SetVecLine( int* srcint, __int64 setflag );
	int SetVecLine( CVec3f* srcvec, int datano, __int64 setflag );
	int SetVecLine( VEC3F* srcvec, int vecnum );

	int SetPolygon( int* srcint, __int64 setflag );
	int SetPolygon( CVec3f* srcvec, int lineno, int linekind, int vecno, __int64 setflag );
	int SetPolygon( VEC3F* srcvec, int linevno, int linekind, int vnum );
	int Add2CurPolygon( CMeshInfo* srclineinfo ); // lineの頂点数が確定するたびにallocのために呼ぶ。

	int SetSphere( int* srcint, __int64 setflag );
	int SetSphere( CMatrix2* srcmatrix, __int64 setflag );

	int SetBMesh( int* srcint, __int64 setflag );
	int SetBMesh( CShdElem* blelem, int brono, int blseri );

	int SetBLine( int srcint, __int64 setflag ); // skip
	int SetBLine( CBezData* srcbez, int bezno, __int64 setflag );
	int SetBLine( CVec3f* srcvec, int srcvecleng, int bezno, __int64 setflag );
	int SetBLine( CVec3f* srcvec, int srclineno, int srcvecno );
	int SetBLine( VEC3F* srcvec, int srclineno, int srcvnum );

	//int SetBMesh( int srcint, int vno, __int64 setflag );
	//int SetBMesh( CBezData* srcbez, int bezno, __int64 setflag );
	//int SetBMesh( CVec3f* srcvec, int lineno, int vecno, __int64 setflag );

	int SetRevolved( int* srcint, __int64 setflag );
	int SetRevolved( CVec3f* srcvec, int lineno, int vecno, __int64 setflag );
	int SetRevolved( VEC3F* srcvec, int lineno, int vnum );
	int SetRevolved( CMatrix2* axis, __int64 setflag );
	int SetRevolved( float* srcfloat, __int64 setflag );

	int SetDisk( int* srcint, __int64 setflag );
	int SetDisk( float* srcfloat, __int64 setflag );
	int SetDisk( CMatrix2* srcmatrix, __int64 setflag );

	int SetExtrude( VEC3F srcexvec );
	int SetExtrude( CVec3f* srcvec, int pno, __int64 setflag );
	int SetExtrude( VEC3F* srcvec, int lineno, int vnum );

	int SetPolyMeshPoint( int srcpno, float srcx, float srcy, float srcz, int orgno );
	int SetPolyMeshPoint( int srcpnum, VEC3F* srcvec );
	int CreatePolyMeshIndex( int srcfacenum );
	int SetPolyMeshIndex( int srcfaceno, int* srclist );
	int SetPolyMeshIndex( int srcdatanum, int* srclist, int srcflag );
	int InvPolyMeshIndex( int srcfaceno );

	int SetPolyMesh2Point( int srcpno, float srcx, float srcy, float srcz, int ono );
	//int SetPolyMesh2Material( int faceno, float srcdiffuse, float srcambient, float srcspecular, float power, float emi, float srcalpha, float srcr, float srcg, float srcb, COORDINATE* srcuv, int srcvnum );
	int SetPolyMesh2Material( int faceno, float srcdiffuse, float srcambient, float srcspecular, float power, float emi, ARGBF color, ARGBF scene_ambient, COORDINATE* srcuv, int srcvnum );
	int SetPolyMesh2Attrib0( int faceno, int materialno );
	int SetPolyMesh2VCol( int vertno, __int64 vcol );

	int SetPolyMesh2Color( int srcpno, ARGBF* srccol );
	int SetPolyMesh2SamePoint();
	int SetPolyMesh2MItoMaterial();

	int SetJointLoc( CVec3f* srcvec );

	int AddBoneInfo( CShdElem* chielem, CShdElem* parelem );

	CMeshInfo* GetMeshInfo();
	//int GetSkipRoute( int* dstroute, int skipmax );	
	//int GetBoneScreenPos( float* dstx, float* dsty, float* dstz, float* dstrhw );
	int GetBoneOrgPos( float* dstx, float* dsty, float* dstz );
	//int GetBonePrePos( float* dstx, float* dsty, float* dstz, float* dstrhw );

	int InitColor( int* seri2boneno, D3DXVECTOR3 vEyePt, int srcshader, int srcoverflow, CMCache* mcache, CMotHandler* lpmh, int tlmode );
	
	int SetInvFace( int srcif );
	int AddInvFaceCnt( int srccnt );
	int CalcClockwise();

	


	// check
	//int IsDispObj( int dispflag );
	int IsJoint();
	
	//int DumpElem( HANDLE hfile, int dumpflag );
	int Dbg_Dump( HANDLE hfile, CTreeHandler2* thandler );
	void DumpReq( HANDLE hfile, CTreeHandler2* thandler, CMotHandler* lpmh, int* errcnt );
	int DumpElem( HANDLE hfile, int level, CMotHandler* lpmh );
	int DumpText( HANDLE hfile, CTreeHandler2* thandler, CMotHandler* lpmh );
	int DumpBoneInfo( CTreeHandler2* srclpth );

	
	int DumpObj( HANDLE hfile, int tabnum, int dumpflag );


	int TransformDispData( int* seri2boneno, D3DXVECTOR3 vEyePt, D3DXMATRIX* firstscale, 
		CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, int lightflag );


	//int TransformDBMesh( int bbiniflag, CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, int* bonearray = 0 );
	//int TransformDMorph( CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, int* bonearray = 0, int divmode = 0 );
	//int TransformDMeshes( CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, int* bonearray = 0 );
	//int TransformDSphere( CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, int* bonearray = 0 );
	//int TransformDExtrude( CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, int* bonearray = 0 );
	//int TransformDPolygon( CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, int* bonearray = 0 );
	int TransformDPolyMesh( int* seri2boneno, D3DXMATRIX* firstscale, D3DXVECTOR3* vEyePt, 
		int srcshader, int srcoverflow, int bbiniflag, CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, 
		CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, int projmode, int lightflag, 
		float aspect, int* bonearray = 0, int divmode = 0 );
	int TransformDPolyMesh( int* seri2boneno, D3DXMATRIX* firstscale, D3DXVECTOR3* vEyePt, 
		CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj );
	int TransformDPolyMesh2( int* seri2boneno, D3DXMATRIX* firstscale, D3DXVECTOR3* vEyePt, 
		CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj );
	int TransformDGroundData( CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, int* bonearray = 0 );
	//int TransformDBillboard( D3DXMATRIX* firstscale, CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, DWORD curdispswitch, int* bonearray = 0 );
	int TransformDExtLine( CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, DWORD curdispswitch, int* bonearray = 0 );
	
	int TransformDMorph( D3DXVECTOR3* vEyePt, CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj );
	int TransformDMorphPM();
	int TransformDMorphPM2();


	int TransformBillboard( CShdHandler* lpsh, D3DXMATRIX matWorld, D3DXMATRIX matWorld1, D3DXMATRIX matWorld2, D3DXMATRIX matView, D3DXMATRIX matProj, D3DXVECTOR3 vEyePt );


	int MoveTexUV( int tlmode );
	int MoveMaterialUV( CMQOMaterial* curmat );

	int UpdateVertexBuffer( int framecnt, int tlmode, int srcshader );

	int Render( int znflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, int glowflag );
	int Render( LPDIRECT3DDEVICE9 pd3dDevice, int faceno );
	int RenderBone( LPD3DXSPRITE pd3dxsprite, LPDIRECT3DTEXTURE9 ptexture, float jointsize, int jointalpha );
	int RenderBillboard( int transskip, CShdHandler* lpsh, LPDIRECT3DDEVICE9 pd3dDevice, CTexBank* texbnk, int tlmode, 
		D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, 
		D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2, D3DXVECTOR3 vEyePt, int glowflag );

	int RenderBillboardMotionBlur( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, CTexBank* texbnk );
	int RenderMotionBlur( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh );


	int RenderShadowMap0( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh );
	int RenderBillboardShadowMap0( CShdHandler* lpsh, LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2, D3DXVECTOR3 vEyePt );
	int RenderShadowMap1( int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh );
	int RenderBillboardShadowMap1( CShdHandler* lpsh, LPDIRECT3DDEVICE9 pd3dDevice, 
		D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2, D3DXVECTOR3 vEyePt );


	/// 呼び出しが　うざいので　表引き。　でもセットもうざい
	void InitObjFunc();
	int (CShdElem::*CreateObjFunc[SHDTYPEMAX])( CMeshInfo* srcmeshinfo );
	int (CShdElem::*DestroyObjFunc[SHDTYPEMAX])(); // 初期化データ、dispデータ片方だけ使うことはまず無いので両方destroy

	int (CShdElem::*CreateDObjFunc[SHDTYPEMAX])( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );

	int (CShdElem::*WriteObjFunc[SHDTYPEMAX])();

	//int CreateDMorph2( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int objtype, int morphnum, int* morphseri, int tlmode );


	int CreateBSphereData( int motkind, int framenum );
	int DestroyBSphereData();
	int RemakeBSphereData( int motid, int framenum );


	int SetCurrentBSphere( int motkind, int frameno );

	// for debug
	int DebugPolymeshM();

	
	int TransformOnlyWorld( CMCache* mcache, CMotHandler* mhptr, int motcookie, int frameno, D3DXMATRIX matWorld, int calcflag, CBSphere* totalbs );
	int TransformOnlyWorld3( CMotHandler* mhptr, D3DXMATRIX matWorld, D3DXVECTOR3* dstvec );

	int SetGroundObj( CMCache* mcache, CMotHandler* lpmh, D3DXMATRIX matWorld );
	int ChkConfVecAndGroundFace( D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpointnum, CONFDATA* confdata, int confmaxnum, DISTSAMPLE* ds = 0 );
	int ChkConfVecAndFace( D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpointnum, CONFDATA* confdata, int confmaxnum, DISTSAMPLE* ds );
	int ChkConfVecAndFaceInfScope( int isindex, D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpointnum, CONFDATA* confdata, int confmaxnum, DISTSAMPLE* ds );



	int MakeFaceno2Materialno( ARGBF* sceneamb, int srcshader, int sigmagicno, int** arrayptr, CMQOMaterial* newmathead, CMQOMaterial* shmathead );
	int WriteMQOObject( HANDLE hfile, int* matnoindex );
	int WriteMQOObjectOnFrame( HANDLE hfile, int* matnoindex, int motid, int frameno, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult, int writerevflag = 1 );

	int GetBillboardNum( int* bbnumptr );
	int GetAllBillboardInfo( int getnum, int* bbnoptr, D3DXVECTOR3* bbposptr, char** texnameptr, int* transparentptr, float* widthptr, float* heightptr );
	int GetBillboardInfo( int srcbbid, D3DXVECTOR3* bbposptr, char* texnameptr, int* transparentptr, float* widthptr, float* heightptr );

	int GetNearBillboard( D3DXVECTOR3 srcpos, float maxdist, int* nearbbid );

	int SetDispDiffuse( int srcr, int srcg, int srcb, int setflag, int tlmode, int vertno, int updateflag );
	int SetDispSpecular( int srcr, int srcg, int srcb, int setflag, int tlmode, int vertno );
	int SetDispAmbient( int srcr, int srcg, int srcb, int setflag, int tlmode, int vertno );
	int SetDispEmissive( int srcr, int srcg, int srcb, int setflag, int tlmode, int vertno );
	int SetDispSpecularPower( float srcpow, int setflag, int tlmode, int vertno );

	int SetMaterialDiffuse( int matno, int setflag, int srcr, int srcg, int srcb );
	int SetMaterialSpecular( int matno, int setflag, int srcr, int srcg, int srcb );
	int SetMaterialAmbient( int matno, int setflag, int srcr, int srcg, int srcb );
	int SetMaterialEmissive( int matno, int setflag, int srcr, int srcg, int srcb );
	int SetMaterialPower( int matno, int setflag, float srcpow );


	int GetDiffuse( int vertno, int* rptr, int* gptr, int* bptr );
	int GetAmbient( int vertno, int* rptr, int* gptr, int* bptr );
	int GetSpecular( int vertno, int* rptr, int* gptr, int* bptr );
	int GetEmissive( int vertno, int* rptr, int* gptr, int* bptr );
	int GetSpecularPower( int vertno, float* powerptr );
	int GetAlpha( int vertno, int* aptr );


	int SetDispScale( D3DXVECTOR3 scalevec, D3DXVECTOR3 centervec );

	//int SetD3DVERTEX_Alpha( float alpha );

	int SetBIM( CShdHandler* lpsh, int srcvertno, int srcchildno, int srcparentno, int srccalcmode );


	int GetDispScreenPos( int vertno, D3DXVECTOR3* posptr, int tlmode );
	int GetDispScreenPosInfScope( int isindex, int vertno, D3DXVECTOR2* posptr, int tlmode );
	int CalcScreenPos( int bbflag, CMotHandler* srclpmh, int vertno, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect, int tlmode, D3DXVECTOR3* posptr );

	int PickVert( int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr );
	//int PickVert2( int face, int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr );
	//int PickVert3( D3DXVECTOR3 campos, int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr );
	//int PickVertInfScope( int isindex, D3DXVECTOR3 campos, int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr );
	int PickVertInfScope( int isindex, int* seri2boneno, D3DXMATRIX firstscale, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh,
		D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float srcaspect,
		int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr );
	int PickVertWorld( D3DXVECTOR3 pickpos, float srcdist, int* dstvno, int arrayleng, int* getnumptr );


	int GetRevCullingFlag( int vertno, int* revcullptr );

	int GetOrgVertNo( int vertno, int* orgnoarray, int arrayleng, int* getnumptr );

	int GetBBox( int mode, CBBox* dstbb );
	int DeleteBBoxByTarget( CShdElem* srctarget );
	int GetVertNoOfFace( int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr );
	int GetVertNoOfFaceInfScope( int isindex, int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr );
	int GetSamePosVert( int vertno, int* sameptr, int arrayleng, int* samenumptr );

	int GetVertPos2( CMotHandler* lpmh, D3DXMATRIX matWorld, int motid, int frameno, int vertno, D3DXVECTOR3* dstpos, int scaleflag );
	int GetVertPos2InfScope( int isindex, CMotHandler* lpmh, D3DXMATRIX matWorld, int motid, int frameno, int vertno, D3DXVECTOR3* dstpos, int scaleflag );

	int GetVertPos3( CMotHandler* lpmh, D3DXMATRIX matWorld, int vertno, D3DXVECTOR3* dstpos );


	int GetTextureInfo( char* nameptr, int* repxptr, int* repyptr, float* animuptr, float* animvptr, int* transparentptr );
	int GetExtTextureInfo( char* extnameptr, int* texnumptr, int* texmodeptr, int* repflagptr, int* stepptr );
	//int HuGetBoneWeight( int vertno, int* boneno1, int* boneno2, float* rate1 );
	int GetTriIndices( WORD* buf, int wordnum, int* setwordnum, int* revflag );
	int GetTriIndicesInt( int* buf, int intnum, int* setintnum, int* revflag );

	//int GetUV( int vertno, float* uptr, float* vptr );//d3ddisp::GetUVを使う。

	int CalcMeshWorldv( int* seri2boneno, CMotHandler* srclpmh, D3DXMATRIX* firstscale, D3DXMATRIX* matWoldptr = 0 );

	void SetBoneLineDispReq( int srclinedisp, int broflag );
	int RestoreBoneInfo( CShdHandler* lpsh );

	int FillDispColor( DWORD srccol );

	int GetInitialPartPos( D3DXVECTOR3* partpos );
	int GetInitialInfScopeCenter( int isindex, D3DXVECTOR3* partpos );


	int Copy2IndexBuffer( int frameno );
	int Copy2IndexBufferNotCulling();

	int GetFaceNum( int* facenum );

	int GetNormal( D3DXVECTOR3* normalptr );
	int CalcInitialNormal();
	int CalcNormalByWorldV();

	int ChangeColor( CShdElem* srcse );
	void	DestroyObjs();

	int RemoveInfluenceNo( int delno );
	int AddInfScope( CPolyMesh2* pm2, CD3DDisp* srcd3ddisp, float srcfacet );
	int AddInfScope( CPolyMesh* pm, CD3DDisp* srcd3ddisp );
	int DeleteInfScopeElemByApplyChild( int srcchild );
	int DeleteInfScopeElemByIndex( int isindex );
	int DeleteInfScopeElemByTarget( int targetno );

	int GetInfScopePointNum( int isindex, int* pointnumptr );
	int GetInfScopePointBuf( int isindex, D3DXVECTOR3* pbuf );
	int GetInfScopeCenter( int isindex, D3DXVECTOR3* centerptr );
	int SetInfScopePointBuf( int isindex, D3DXVECTOR3* newp, int vertno );
	int GetInfScopeDiffuse( int isindex, COLORREF* colptr );
	int SetInfScopeDiffuseAmbient( int isindex, COLORREF srccol );

	int CreateAndSaveTempMaterial();
	int RestoreAndDestroyTempMaterial();

	int SetMaterialFromInfElem( int srcchildno, int srcvert, E3DCOLOR3UC* infcolarray = 0 );
	int SetInfElem( CShdHandler* lpsh, int srcvert, CBoneInfo* biptr, CShdElem* parselem, int paintmode, int calcmode, float rate, int normflag, float directval, int updateflag = 1 );
	int DeleteInfElem( int srcvert, int srcchildno, int normflag, int updateflag = 1 );
	int DestroyIE( int srcvert );
	int AddInfElem( int srcvert, INFELEM srcIE );
	int NormalizeInfElem( int srcvert );

	int GetNearVert( D3DXVECTOR3 findpos, float symdist, int* findvert, float* finddist );

//	int ChkMorphElemToAdd( CShdElem* addelem );
//	int DeleteMorphElem( CShdElem* deleelm );
//	int AddMorphElem( CShdElem* addelem );

	int CreateToon1Buffer( CShdHandler* lpsh, LPDIRECT3DDEVICE9 pdev, int forceRemakeflag = 0 );
	int CreateMaterialBlock( LPDIRECT3DDEVICE9 pdev, CShdHandler* lpsh );

	int MakeXSeri2InfBno( int serinum, int* seri2infbno, int* bonenumptr );
	int MakeXBoneInfluence( int serinum, int bonenum, int* seri2infbno, BONEINFLUENCE* biptr );

	int RepairInfElem( CShdHandler* srclpsh, LPDIRECT3DDEVICE9 pdev );

	int InitBBox();
	int SetBBox( CShdElem* srcbbxelem, D3DXMATRIX matWorld );

	int CreateSkinMat( CShdHandler* lpsh, int updateflag );
	int EnableToonEdge( int srctype, int srcflag );
	int SetToonEdge0Color( char* srcname, int srcr, int srcg, int srcb );
	int SetToonEdge0Width( char* srcname, float srcwidth );
	int GetToonEdge0Color( char* srcname, float* dstr, float* dstg, float* dstb );
	int GetToonEdge0Width( char* srcname, float* dstwidth );

	int InitParticleParams();

	int SetIndexBufColP();
	int GetOrgCenter( D3DXVECTOR3* dstcenter );

	int GetFaceNormal( int faceno, D3DXMATRIX matWorld, CMotHandler* lpmh, D3DXVECTOR3* dstn );
	int GetScaleMat( D3DXMATRIX* dstmat );

	int ChkAlphaNum( CMQOMaterial* srcmat );
	int GetFirstMaterialNo( int* matnoptr );
	int GetMaterialNo( int faceno, int* matnoptr );
	int GetMaterialNo2( int srcshader, int faceno, int* matnoptr );

	int SetBoneLim01( DVEC3* srclim0, DVEC3* srclim1 );
	int GetBoneLim01( DVEC3* dstlim0, DVEC3* dstlim1 );
	int SetIgnoreLim( int srcignore );
	int GetIgnoreLim( int* dstignore );

	int SetBoneAxisQ( CQuaternion srcq );
	int GetBoneAxisQ( CQuaternion* dstq );
	int SetZa4Q( CQuaternion srcq );
	int GetZa4Q( CQuaternion* dstq );
	int SetZa4Type( int srctype );
	int GetZa4Type( int* dsttype );
	int SetZa4RotAxis( int srcaxis );
	int GetZa4RotAxis( int* dstaxis );
	int SetZa4LocalQ( CQuaternion srcq );
	int GetZa4LocalQ( CQuaternion* dstq );

	int GetToon1MaterialNum( int* numptr );
	int GetToon1MaterialInfo( TOON1MATERIALINFO* infoptr, int arrayleng, int* getnumptr );
	int GetToon1Material( E3DTOON1MATERIAL* infoptr, int arrayleng, int* getnumptr );
	int GetToon1MaterialNoByName( char* matname, int* matnoptr );
	int GetToon1Ptr( int matno, CToon1Params** pptoon1 );
	int GetToon1Ptr( char* matname, CToon1Params** pptoon1 );
	int SetToon1Name( char* oldname, char* newname );
	int SetToon1Diffuse( char* matname, RDBColor3f diffuse );
	int SetToon1Specular( char* matname, RDBColor3f specular );
	int SetToon1Ambient( char* matname, RDBColor3f ambient );
	int SetToon1NL( char* matname, float darknl, float brightnl );
	int SetToon1Edge0( char* matname, RDBColor3f col, int validflag, int invflag, float width );
	



		//target, baseをセットしてから実行するため他のものをCreateDし終わってから直接呼ぶ。
	int CreateDMorph( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int SetMotionBlur( CShdHandler* lpsh, int mode, int blurtime );
	int SetBeforeBlur( int bbflag );
	int InitBeforeBlur();

	int GetMorphCenterPos( D3DXVECTOR3* dstcenter );
	int CloneDispObj( CShdElem* srcelem );

private:
	int	CreateObjs();

	CBezMesh* NewBMesh( CMeshInfo* srcmeshinfo );
	CBezLine* NewBLine( CMeshInfo* srcmeshinfo );

	//int Destroy3DObj( int srctype );
	int DestroyVLine();
	int DestroyPolygon();
	int DestroySphere();
	int DestroyBMesh();
	int DestroyRevolved();
	int DestroyDisk();
	int DestroyBLine();
	int DestroyMorph();
	int DestroyPart();
	int DestroyExtrude();
	int DestroyPolyMesh();
	int DestroyPolyMesh2();
	int DestroyGroundData();
	int DestroyBillboard();
	int DestroyExtLine();
	int DestroyInfScope();
	int DestroyBBox();
	int DestroyDummy();

	//int Create3DObj( CMeshInfo* srcmeshinfo );
	int CreateVLine( CMeshInfo* srcmeshinfo );
	int CreatePolygon( CMeshInfo* srcmeshinfo );
	int CreateSphere( CMeshInfo* srcmeshinfo );
	int CreateBMesh( CMeshInfo* srcmeshinfo );
	int CreateRevolved( CMeshInfo* srcmeshinfo );
	int CreateDisk( CMeshInfo* srcmeshinfo );
	int CreateBLine( CMeshInfo* srcmeshinfo );
	int CreateMorph( CMeshInfo* srcmeshinfo );
	int CreatePart( CMeshInfo* srcmeshinfo );
	int CreateExtrude( CMeshInfo* srcmeshinfo );
	int CreatePolyMesh( CMeshInfo* srcmeshinfo );
	int CreatePolyMesh2( CMeshInfo* srcmeshinfo );
	int CreateGroundData( CMeshInfo* srcmeshinfo );
	int CreateBillboard( CMeshInfo* srcmeshinfo );
	int CreateExtLine( CMeshInfo* srcmeshinfo );
	int CreateDummy( CMeshInfo* srcmeshinfo );

	//int CreateDObjFunc[i] 
	//int CreateDVLine( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	//int CreateDPolygon( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	//int CreateDSphere( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	//int CreateDBMesh( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	//int CreateDRevolved( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	//int CreateDDisk( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	//int CreateDBLine( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	//int CreateDMorph( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	//int CreateDExtrude( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CreateDPolyMesh( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CreateDPolyMesh2( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CreateDPart( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CreateDGroundData( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CreateDBillboard( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CreateDExtLine( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CreateDInfScope( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CreateDBBox( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CreateDDummy( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );

		//target, baseをセットしてから実行するため他のものをCreateDし終わってから直接呼ぶ。
//	int CreateDMorph( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );//publicへ
	int CreateDMorphPM( CPolyMesh* pm, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );
	int CreateDMorphPM2( CPolyMesh2* pm2, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe );

	//int WriteObjFunc
	int WriteVLine();
	int WritePolygon();
	int WriteSphere();
	int WriteBMesh();
	int WriteRevolved();
	int WriteDisk();
	int WriteBLine();
	int WriteDummy();

	int GetKindNum( int objtype );
	CMeshInfo* AdjustMeshInfo( CMeshInfo* srcmeshinfo );
	CMeshInfo* CreateLineInfo( CMeshInfo* srcmeshinfo );

	CBezMesh* BezDiv( CBezMesh* srcbmesh );
	
	int SetDefaultRenderState();
	int SetRenderState( LPDIRECT3DDEVICE9 pdev, CShdElem* selem );

	int ExistInfScopeByTarget( int targetno );
	int ExistInfScopeByApplyChild( int srcchild );

	int SetXInfluenceArray( CInfElem* ieptr, int vnum, int boneserino, DWORD* vertices, float* weights, int infnum, DWORD* setnumptr );

	int AddBBox( CShdHandler* srclpsh, CShdElem* curobj, CShdElem* curjoint, int* vertarray, int vertnum );
	int SetRenderStateIfNotSame( LPDIRECT3DDEVICE9 pd3dDevice, int srcstate, DWORD srcval );

	int DwcolToD3dcolv( DWORD srccol, D3DCOLORVALUE* dstcol );
	int D3dcolvToDwcol( D3DCOLORVALUE srccol, DWORD* dstcol );

public:

	int serialno;
	int depth;
	int brono;
	int bronum; // brono == 0 のメンバだけ意味を持つ。

	int clockwise; // 描画時の回り方
	int invface; // invface, invfacecnt はclockwiseを決定するのに使う。dumpの対象外。
	int invfacecnt;

	int bdivU;
	int bdivV;

	int texrepx; // texture のrepetition
	int texrepy;

	float uanime; // texture の　UVアニメの 変化量(frame単位)
	float vanime;

	int transparent;// sprite (transparent black)
	float alpha;
	int setalphaflag;

	char* texname;
	char* curtexname;

	int texrule;

	int noskinning;

	int blendmode;

	unsigned char exttexnum;
	unsigned char exttexmode; //重ねるか、パラパラアニメするかどうか。
	unsigned char exttexrep; //繰り返しかどうか
	unsigned char exttexstep; //何フレームごとに、変化するか。
	char** exttexname;


	int dispflag;
	int isselected;	
	int notuse;
	
	int* influencebone;
	int influencebonenum;

	int* ignorebone;
	int ignorebonenum;

	// influenceboneがbonenoで構成されている場合に１、serialnoに変換されていれば０
	int neededconvert; 
	int neededconvert_ig;//for ignorebone


	DWORD dispswitchno;//ver1.0.1.0

	int	invisibleflag;

	int m_renderstate[D3DRS_BLENDOP + 1];
	DWORD m_minfilter;
	DWORD m_magfilter;

	//int type; // !!!<-- CBaseDat のメンバ

	// chain は、CTreeElem2から取得してセット。
	CShdElem* parent;
	CShdElem* child;
	CShdElem* brother;
	CShdElem* sister;


	// Init3DObj( meshinfo )で作成
		// １つのTreeに１種、１個だけ。
	CVecLine* vline; // SHDPOLYLINE
	CPolygon* polygon; // SHDPOLYGON
	CSphere* sphere; // SHDSPHERE
	CBezMesh* bmesh; // SHDBEZIERSURF
	CRevolved* revolved; // SHDMESHES
	CDisk* disk; // SHDDISK
	CBezLine* bline; // SHDBEZLINE

	CMorph* morph; // SHDMORPH
	CPart* part; // SHDROOT SHDROTATE SHDSLIDER SHDSCALE SHDUNISCALE SHDPATH SHDBALLJOINT SHDPART
	CExtrude* extrude; // SHDEXTRUDE

	CPolyMesh* polymesh;//SHDPOLYMESH
	CPolyMesh2* polymesh2;

	CGroundData* grounddata;
	CBillboard* billboard;

	CExtLine* extline;//SHDEXTLINE

	CBezMesh* bdiv_bmesh; // ベジェ分割bmesh
	CBezMesh* bdiv_bmesh2;

	CD3DDisp* d3ddisp;

	
	CBSphere** firstbs;
	CBSphere	curbs;
	CBBox		m_bbox;//古いbbox : 使用しない。
	CBBox		m_bbx0;//新しいあたり判定用のbbox, matWorldが掛かる前のbbox
	CBBox		m_bbx1;//matWorldが掛かった後のbbox


	D3DXVECTOR3 m_center;

	D3DXMATRIX* m_tempscalemat;

	int m_mqono;

	int m_loadbimflag;

	int m_enabletexture;

	int m_notransik;
	int m_ikskip;

	int m_userint1;

	int m_iklevel;
	int m_bonelinedisp;

	int m_bbdirmode;//billboardのdirmode

	int m_anchorflag;

	int m_mikodef;

	/// for iktarget
	int m_iktarget;//０なし、１有効、２無効
	int m_iktargetlevel;
	int m_iktargetcnt;
	D3DXVECTOR3 m_iktargetpos;

	float facet;

	float m_toonparams[4];

	CMeshInfo	m_dummymeshinfo;


	int scopenum;
	CInfScope** ppscope;

	int bboxnum;
	CBBox2** ppbbox;


	D3DXMATRIX TransformationMatrix;// for xfile
	D3DXMATRIX combinedtra;// for xfile

	int m_lightflag;
	int m_particleflag;

	int m_tempfogenable;

	int m_shadowinflag;//coef.h SHADOWIN_*

	int m_notsel;

	D3DXVECTOR3 m_mikobonepos;//mikotoボーンのボーンを構成しない３点目の座標。za3の向きに使用。sh GetInitialBoneQ
	int m_setmikobonepos;//m_mikoboneposが有効な場合１。

	int m_mtype;//morphのbase形状のときM_BASE、morphのターゲットのときM_TARGET, それ以外はM_NONE
	int m_blurbefrender;//motionblurを描画したとき１、しないとき０。視野外-->視野内時にInitBeforeBlurするトリガーに使用する。
	float m_mbluralpha[2];//minalpha motionblur value, maxalpha motionblur 係数

	int m_bbexttexmode;//マテリアルのないbillboardのグローを設定するため
	float m_bbglowmult[3];//マテリアルのないbillboardのグローを設定するため

	int m_befdispswitch;//一回前のdisplayswitchを保存。off->onになったときにInitBeforeBlurを呼ぶ。

	/***
	static HANDLE s_hHeap;
	static unsigned int	s_uNumAllocsInHeap;

	static unsigned int s_DataNo;
	static void*	s_DataHead;

	static BYTE*	s_HeadIndex[2048];// s_DataHeadを格納する配列
	static int		s_HeadNum;// s_HeadIndexの有効データの数。

	static int	s_useflag[2048][TE2_BLOCKLENG];
	***/

};

#endif