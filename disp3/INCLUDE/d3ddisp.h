#ifndef D3DDISPH
#define D3DDISPH

#include <D3DX9.h>
#include <math.h>

#define DBGH
#include <dbg.h>

#include <matrix2.h>
#include <basedat.h>
#include <infelem.h>

#include <coef.h>
#include <PCoef.h>

#include <bbox.h>

class CShdHandler;
class CShdElem;
class CBoneInfo;
class CMotHadnler;

class CVec3f;
class CBezMesh;
class CRevolved;
class CSphere;
class CExtrude;
class CPolygon;
class CPolyMesh;
class CPolyMesh2;
class CPart;
class CBillboard;
class CExtLine;
class CMCache;
class CPickData;
class CToon1Params;
class CLightData;
class CMorph;

//class CPCoef;

#ifndef CLIP_NONE

	// guard bandでの判定用
#define CLIP_NONE	0
#define CLIP_FRONT	1
#define CLIP_BACK	2
#define CLIP_LEFT	4
#define CLIP_RIGHT	8
#define CLIP_TOP	16
#define CLIP_BOTTOM	32
#define CLIP_FINISH	64
#define CLIP_NEAR	128

	// 画面サイズでの判定用
#define CLIP_N_LEFT		256
#define CLIP_N_RIGHT	512
#define CLIP_N_TOP		1024
#define CLIP_N_BOTTOM	2048 

#endif


//ChkConfVecAndFace2で使用
#define DIRECTION(a,b) (a=b)
#define UP 0
#define DOWN 1


class CD3DDisp
{
public:
	CD3DDisp();
	~CD3DDisp();

	//int CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CBezMesh* bm, int clockwise, int srcbdivV, int srcrepx, int srcrepy, int tlmode );
	//int CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CRevolved* rev, int clockwise, int srcrepx, int srcrepy, int tlmode );
	//int CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CSphere* sph, int clockwise, int srcrepx, int srcrepy, int tlmode );
	//int CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CExtrude* extr, int clockwise, int srcrepx, int srcrepy, int tlmode );
	//int CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CPolygon* poly, int clockwise, int srcrepx, int srcrepy, int tlmode );
	int CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CPolyMesh* polymesh, int clockwise, int srcrepx, int srcrepy, int tlmode );
	int CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CPolyMesh2* polymesh2, int clockwise, int srcrepx, int srcrepy, int tlmode, float facet );
	int CreateDispData( LPDIRECT3DDEVICE9 pdev, CPart* part, int tlmode );
	int CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CExtLine* extline, int tlmode );
	int CreateDispDataMPM2( LPDIRECT3DDEVICE9 pd3dDevice, CMorph* morph, int tlmode );



	int Create3DBuffersLine( LPDIRECT3DDEVICE9 pd3dDevice );
	int Create3DBuffersSkin();

	int CreateVertexAndIndex( int clockwise );

	//int CreateBoneElem( CShdHandler* srclpsh, int srcseri );
	int CalcInfElem( CShdHandler* srclpsh, int srcseri );
	int CalcInfElem1Vert( CShdHandler* srclpsh, int srcseri, int srcvert );


	int SetUV( int srctype, int srcrule, CPolyMesh2* pm2, CPolyMesh* pm, int vertno );
	int SetUVArray( int* dirtyflag, int srctype, int srcrule, CPolyMesh2* pm2, CPolyMesh* pm );


	int GetUV( int vertno, float* uptr, float* vptr );

	int TransformSkinMat( int serialno, int* seri2boneno, D3DXMATRIX* firstscale, CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3* vEyePt );
	int TransformSkinMatMorph( CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3* vEyePt );

	int TransformDispDataSCV( int* seri2boneno, int serialno, D3DXMATRIX* firstscale, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect );
	//int TransformDispDataPM2( int srcshader, int* seri2boneno, CPolyMesh2* pm2, D3DXMATRIX* firstscale, int serialno, CBBox* bbox, CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, int srctype, float aspect, int* bonearray, int* samebuf = 0, int divmode = 0 );
	//int TransformDispData( int srcshader, int* seri2boneno, D3DXMATRIX* firstscale, int serialno, CBBox* bbox, CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, int srctype, float aspect, int* bonearray, int* samebuf = 0, int divmode = 0 );
	//int TransformDispAsm( D3DXMATRIX* firstscale, int serialno, CBBox* bbox, CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, int srctype, float aspect, int* bonearray, int* samebuf = 0, int divmode = 0 );
	//int TransformBoneData( DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, int srctype, float aspect );

	int TransformOnlyWorld( CMCache* mcache, CMotHandler* mhptr, D3DXMATRIX matWorld, int calcflag, CBSphere* setbs, CBSphere* totalbs );
	int TransformOnlyWorld2( CMotHandler* mhptr, D3DXMATRIX matWorld, int vertno );
	int TransformOnlyWorld3( CMotHandler* mhptr, D3DXMATRIX matWorld, D3DXVECTOR3* dstvec, CInfElem* srcie );
	int TransformOnlyWorldMorph( CMotHandler* mhptr, D3DXMATRIX matWorld, TEXV* dstvec, CInfElem* srcie, int* oldpno2optpno );
	int TransformOnlyWorldInfOneBone( CMotHandler* srclpmh, D3DXMATRIX matWorld, 
		D3DXVECTOR3 srcpos, D3DXVECTOR3* dstpos, int infboneno );


	//int MixDispData( float weight1, float weight2, CD3DDisp* disp1, CD3DDisp* disp2 ); 
	
	//int InitColor( CBezMesh* bm, float alpha, D3DXMATRIX matWorld, int flag = 0 );
	//int InitColor( CRevolved* rev, float alpha, D3DXMATRIX matWorld, int flag = 0 );
	//int InitColor( CSphere* sph, float alpha, D3DXMATRIX matWorld, int flag = 0 );
	//int InitColor( CExtrude* extr, float alpha, D3DXMATRIX matWorld, int flag = 0 );
	//int InitColor( CPolygon* poly, float alpha, D3DXMATRIX matWorld, int flag = 0 );
	int InitColor( D3DXVECTOR3 vEyePt, int srcshader, int srcoverflow, CPolyMesh* polymesh, float alpha, D3DXMATRIX matWorld, int flag = 0 );
	int InitColor( D3DXVECTOR3 vEyePt, int srcshader, int srcoverflow, CPolyMesh2* polymesh2, float alpha, D3DXMATRIX matWorld, int flag = 0 );
	int InitColor( CExtLine* extline, float alpha, D3DXMATRIX matWorld, int flag = 0 );

	int MoveTexUV( float ustep, float vstep, int srctype );
	int MoveUVArray( int texanimtype, int* dirtyptr, float ustep, float vstep, int srctype );

	//int Render( LPDIRECT3DDEVICE9 pd3dDevice );// strip
	//int RenderFan( LPDIRECT3DDEVICE9 pd3dDevice ); // fan
	int RenderTri( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, float* toonparams, LPDIRECT3DTEXTURE9 lptex, 
		float* fogparams, CToon1Params* ptoon1, int lightflag, float* bluralpha, int blurflag = 0, int glowmode = 0, float* glowmult = 0 ); // tri list
	int RenderTri( LPDIRECT3DDEVICE9 pd3dDevice, int faceno, CShdHandler* lpsh, float* toonparams, LPDIRECT3DTEXTURE9 lptex, float* fogparams, CToon1Params* ptoon1 ); // tri list
	int RenderBone( LPD3DXSPRITE pd3dxsprite, LPDIRECT3DTEXTURE9 ptexture, int isselected, float jointsize, int jointalpha );
	int RenderLine( LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, float srcalpha );

	int RenderTriPhong( float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, 
		float* toonparams, float* fogparams, int lightflag,
		int srcenabletex, CHKALPHA* chkalpha, float* bluralpha, int blurflag = 0, int glowflag = 0 );
	int RenderPhongShadowMap0( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, int srcenabletex );
	int RenderBillboardShadowMap0( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, LPDIRECT3DTEXTURE9 lptex );
	int RenderPhongShadowMap1( float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, float* toonparams, 
		float* fogparams, int srcenabletex, CHKALPHA* chkalpha );
	int RenderBillboardShadowMap1( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, LPDIRECT3DTEXTURE9 lptex, float* fogparams );


	int RenderTriToon1( float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, float* toonparams, float* fogparams, 
		CToon1Params* ptoon1, int lightflag, int srcenabletex, CHKALPHA* chkalpha, float* bluralpha, int blurflag = 0, int glowflag = 0 );

	int RenderToon1ShadowMap1( float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, float* fogparams, 
		CToon1Params* ptoon1, int lightflag, int srcenabletex, CHKALPHA* chkalpha );

	int RenderZNPhong( int znflag, float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, 
		CHKALPHA* chkalpha );
	int RenderZNToon1( float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, 
		CHKALPHA* chkalpha );



	int SetRenderStateIfNotSame( LPDIRECT3DDEVICE9 pd3dDevice, int srcstate, DWORD srcval );

	int ResetPrimNum();

	int Copy2IndexBuffer( int framecnt );
	int Copy2IndexBufferNotCulling();
	int Copy2VertexBuffer( int framecnt, int cpmaterial = 1 );
	int Copy2VertexBufferMorph( CPolyMesh2* pm2, int cpmaterial );


	//for debug
	//int FindMinZVert( float* zval, D3DXVECTOR3* dstvec );
	int CheckRestVert();

	//int CalcDispMag( int srctlvno, float srcx, float srcy, float* retmag );

	
	int SetGroundObj( CMCache* mcache, CMotHandler* srclpmh, D3DXMATRIX matWorld );
	int SetGroundObj2( CMotHandler* srclpmh, D3DXMATRIX matWorld, int vertno );


	int ChkConfVecAndGroundFace( D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpointnum, CONFDATA* confdata, int confmaxnum, DISTSAMPLE* ds = 0 );
	int ChkConfVecAndFace( D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpointnum, CONFDATA* confdata, int confmaxnum, DISTSAMPLE* ds );

	int SetOrgScale( D3DXVECTOR3 scalevec, D3DXVECTOR3 centervec );

	int TransformOnlyWorld1Vert( CMotHandler* srclpmh, D3DXMATRIX matWorld, int motid, int frameno, int vertno, D3DXVECTOR3* dstpos, int scaleflag, CInfElem* ieptr );
	int TransformOnlyWorld1VertCurrent( CMotHandler* srclpmh, D3DXMATRIX matWorld, int vertno, D3DXVECTOR3* dstpos, int scaleflag, CInfElem* ieptr );

	int SetRevVertex( int copymode );// for TLMODE_D3D
	//int SetD3DVERTEX_RGB( CVec3f newrgb, int flag );// for TLMODE_D3D
	//int SetD3DVERTEX_Alpha( float alpha );
	int UpdateExtLine( CExtLine* extline, float alpha );

	int SetBIM( CShdHandler* lpsh, int serialno, int srcvertno, int srcchildno, int srcparentno, int srccalcmode );

	int SetOrgTlvColor( int vertno, DWORD dwcol, int colkind );
	int SetOrgTlvPower( int vertno, float srcpower );

	int SetOrgTlvColorArray( int* dirtyptr, DWORD dwcol, int colkind );
	int SetOrgTlvPowerArray( int* dirtyptr, float srcpower );


	//int SetOrgTlvAlpha( int vertno, int ialpha );
	int SetDispTlvAlpha( float srcalpha );


	int GetDispScreenPos( int vertno, D3DXVECTOR3* posptr, int tlmode );
	int CalcScreenPos( int bbflag, CMotHandler* srclpmh, int vertno, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect, int tlmode, D3DXVECTOR3* posptr );


	int PickVert( int srctype, int srcseri, int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr );
	//int PickVert2( int face, int srctype, int srcseri, int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr );
	//int PickVert3( D3DXVECTOR3 campos, int srctype, int srcseri, int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr );
	int GetRevCullingFlag( int srctype, int vertno, int* revcullptr );

	int GetBBox( int mode, CBBox* dstbb );
	int GetVertNoOfFace( int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr );
	int GetSamePosVert( int vertno, int* sameptr, int arrayleng, int* samenumptr );

	int CalcMeshWorldv( int* seri2boneno, CMotHandler* srclpmh, int serialno, D3DXMATRIX* firstscale, D3DXMATRIX* matWorldptr = 0 );

	int FillDispColor( DWORD srccol, CPolyMesh2* pm2 );

	int GetInitialPartPos( D3DXVECTOR3* partpos );

	int GetNormal( D3DXVECTOR3* normalptr );
	int CalcInitialNormal( CPolyMesh2* pm2 );
	int CalcNormalByWorldV( CPolyMesh2* pm2 );

	int GetRenderPrimNum( int* numptr );

	int SetIndexBuffer( int* srcindex, int srcclockwise );
	int CreateSkinMat( CShdHandler* lpsh );
	int CreateSkinMatToon1( CShdHandler* lpsh, MATERIALBLOCK* materialblock, int materialnum );
	int CreateSkinMatTex( CShdHandler* lpsh, TEXTUREBLOCK* texblock, int texblocknum );

	int CreateMorphSkinMat( CShdHandler* lpsh, CPolyMesh2* pm2 );
	int CreateMorphSkinMatTex( CShdHandler* lpsh, CPolyMesh2* pm2, TEXTUREBLOCK* texblock, int texblocknum );
	int CreateMorphSkinMatToon1( CShdHandler* lpsh, CPolyMesh2* pm2, MATERIALBLOCK* materialblock, int materialnum );

	//int SetTriNormalTLV( D3DXVECTOR3 vEyePt, D3DTLVERTEX* srctlv, int lightflag );
	//int SetTriNormalPM2TLV( D3DXVECTOR3 vEyePt, D3DTLVERTEX* srctlv, CPolyMesh2* pm2, int lightflag );

	int GetSkinMeshHeader( int leng, int* maxpervert, int* maxperface );

	int SaveToDispTempDiffuse();
	int RestoreDispTempDiffuse( CPolyMesh2* pm2 );

	int CopyUV2VertexBuffer( int vertno );
	int CopyUV2VertexBufferBatch( int* vertnoptr, int setnum );


	int GetFaceNormal( int faceno, D3DXMATRIX matWorld, CMotHandler* lpmh, D3DXVECTOR3* dstn );
	int SetPolyMeshPos( CPolyMesh* polymesh );
	int SetBeforeBlur( int bbflag );
	int InitBeforeBlur();

private:
	void InitParams();
	void DestroyObjs();
	int SetExtLineLV( CExtLine* extline );
	int SetPolyMesh2SkinV( CPolyMesh2* polymesh2 );
	int SetPolyMeshSkinV( CPolyMesh* polymesh );
	int SetPolyMesh2SkinVMorph( CPolyMesh2* polymesh2 );

	int SetTriUV();

	int SetUV_X( int* dirtyflag );
	int SetUV_Y( int* dirtyflag );
	int SetUV_Z( int* dirtyflag );
	int SetUV_Cylinder( int* dirtyflag );
	int SetUV_Sphere( int* dirtyflag );
	//int SetUV_BM_Cylinder();
	int SetUV_MQ( int* dirtyflag, CPolyMesh2* pm2, int vertno );
	int SetUV_Tex4( int* dirtyflag, CPolyMesh* pm, int vertno );
	int SetUV_Billboard( int* dirtyflag );


	int SetTriIndices( int* dstindices, int clockwise, int facenum, int* srclist );
	int SetExtLineIndices( int* dstindices, DWORD numindices );


	int SetTriNormalPM2_nolight( D3DXVECTOR3* worldv, CPolyMesh2* pm2 );
	int SetTriNormalPM();
	int SetRevNormal();


	//int SetTriNormalPM2TLV_nolight( D3DTLVERTEX* srctlv, CPolyMesh2* pm2 );


	//int	SetStripColor( CVec3f* diffuse, CVec3f* specular, CVec3f* ambient, DWORD numvertices, D3DXVECTOR3* pnormal, D3DTLVERTEX* ptlv, D3DTLVERTEX* orgtlv, float alpha, D3DXMATRIX matWorld, int revflag = 0 );
	//int	SetPMColor( D3DXVECTOR3 vEyePt, int srcshader, int srcoverflow, CPolyMesh* pm, DWORD numvertices, D3DXVECTOR3* pnormal, D3DTLVERTEX* ptlv, D3DTLVERTEX* orgtlv, float alpha, D3DXMATRIX matWorld, int revflag = 0 );
	//int SetPMColorToon1( CPolyMesh* pm, D3DXVECTOR3* pnormal, D3DTLVERTEX* ptlv, float alpha, int revflag );

	//int	SetPM2Color( D3DXVECTOR3 vEyePt, int srcshader, int srcoverflow, CPolyMesh2* pm2, DWORD numvertices, D3DXVECTOR3* pnormal, D3DTLVERTEX* ptlv, D3DTLVERTEX* orgtlv, float alpha, D3DXMATRIX matWorld, int revflag = 0 );
	//int SetPM2ColorToon1( CPolyMesh2* pm2, D3DXVECTOR3* pnormal, D3DTLVERTEX* ptlv, float alpha, D3DXMATRIX matWorld, int revflag );

	//int SetVertNormalPM();// for TLMODE_D3D
	//int SetVertColorPM( CPolyMesh* pm, float alpha );// for TLMODE_D3D

	//int SetVertNormalPM2( CPolyMesh2* pm2 );// for TLMODE_D3D
	//int SetVertColorPM2( CPolyMesh2* pm2, float alpha );// for TLMODE_D3D

	int SetSkinNormalPM2( CPolyMesh2* pm2 );
	int SetSkinColorPM2( CPolyMesh2* pm2, float alpha );
	int SetSkinNormalPM();
	int SetSkinColorPM( CPolyMesh* pm, float alpha );

	int SetLVColor( CExtLine* extline, float alpha );// for SHDEXTLINE

	//int CalcNormal( D3DXVECTOR3* newn, D3DTLVERTEX* curp, D3DTLVERTEX* aftp1, D3DTLVERTEX* aftp2 );
	//int AverageVector( D3DXVECTOR3* dstn, D3DXVECTOR3 srcn );


	
	//int MatConvD3DX( D3DXMATRIX* dstmat, CMatrix2 srcmat );
	//HRESULT TransformRenderTLV( LPDIRECT3DDEVICE9 pd3dDevice, D3DTLVERTEX* orgTLV, D3DTLVERTEX* renderTLV, DWORD tlvnum );

	int CalcBezDiv_V( int newlno, int beflno, int aftlno );

	CShdElem* FindNearestJoint( CShdHandler* srclpsh, D3DXVECTOR3* srcxv, int srcseri, int* ignoreflag, int ignoreleng, int needhaschild = 1 );
	void FindNearestJointReq( D3DXVECTOR3* srcxv, CShdElem* chkelem, float* distptr, CShdElem** ppnear, int broflag, int* ignoreflag, int ignoreleng, int needhaschild = 1 );

	CBoneInfo* FindNearestBone( CShdElem* srcelem, D3DXVECTOR3* srcxv, int* ignoreflag, int ignoreleng );
	CBoneInfo* FindNearestBone( CShdElem* srcelem, int srcchildno );

	CBoneInfo* FindNearestBoneMiko( CShdHandler* srclpsh, D3DXVECTOR3* srcv, CShdElem* srcselem, int centerflag );


	//int SetInfElem( CBoneInfo* srcbi, CShdElem* parselem, D3DXVECTOR3* srcxv, CBoneElem* dstbe, int mikoflag );


	int IsExistValidBone( int srcseri, CPart* partptr, int* ignoreflag, int leng, int needhaschild = 1 );

	int SetSubColor();

	int SetIgnoreFlag( CShdHandler* srclpsh, int srcseri, int* ignoreflag );
	void SetIgnoreFlagReq( CShdElem* ignoreelem, int* ignoreflag, int leng, int broflag );

	//int ChkConfVecAndFace2( D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, CPCoef* curpc, D3DXVECTOR3* v1, D3DXVECTOR3* v2, D3DXVECTOR3* v3, int* resultptr, CONFDATA* cdptr );

	int SetTLVScale( D3DXVECTOR3 scalevec, D3DXVECTOR3 centervec );

	int CheckMikoAnchor( CShdHandler* srclpsh, int srcseri );

	int CalcCenterV( D3DXVECTOR3* dstv );

	int SetSkinDecl( LPDIRECT3DDEVICE9 pdev );
	int SetLineDecl( LPDIRECT3DDEVICE9 pdev );

	int DestroySkinMat();
	int DestroyArrayVB();

	int GetCurrentBlockNo( int srcfaceno, RENDERBLOCK* srcrb );
	int GetCurrentSkinBlockNo( int srcfaceno, SKINBLOCK* srcsb );
	int CreateBlurMat();

	CLightData* GetLightData( int lid );

	__inline int CD3DDisp::MatConvD3DX( D3DXMATRIX* dstmat, CMatrix2 srcmat )
	{
		float* srcdat = &(srcmat.data[0][0]);

		
		dstmat->_11 = *srcdat;
		dstmat->_12 = *(srcdat + 1);//[0][1];
		dstmat->_13 = *(srcdat + 2);//[0][2];
		dstmat->_14 = *(srcdat + 3);//[0][3];

		dstmat->_21 = *(srcdat + 4);//[1][0];
		dstmat->_22 = *(srcdat + 5);//[1][1];
		dstmat->_23 = *(srcdat + 6);//[1][2];
		dstmat->_24 = *(srcdat + 7);//[1][3];
		
		dstmat->_31 = *(srcdat + 8);//[2][0];
		dstmat->_32 = *(srcdat + 9);//[2][1];
		dstmat->_33 = *(srcdat + 10);//[2][2];
		dstmat->_34 = *(srcdat + 11);//[2][3];

		dstmat->_41 = *(srcdat + 12);//[3][0];
		dstmat->_42 = *(srcdat + 13);//[3][1];
		dstmat->_43 = *(srcdat + 14);//[3][2];
		dstmat->_44 = *(srcdat + 15);//[3][3];
		

		/***
		//上の表記と速度は、ほとんど、変わらない。
		dstmat->_11 = srcmat[0][0];
		dstmat->_12 = srcmat[0][1];
		dstmat->_13 = srcmat[0][2];
		dstmat->_14 = srcmat[0][3];

		dstmat->_21 = srcmat[1][0];
		dstmat->_22 = srcmat[1][1];
		dstmat->_23 = srcmat[1][2];
		dstmat->_24 = srcmat[1][3];
		
		dstmat->_31 = srcmat[2][0];
		dstmat->_32 = srcmat[2][1];
		dstmat->_33 = srcmat[2][2];
		dstmat->_34 = srcmat[2][3];

		dstmat->_41 = srcmat[3][0];
		dstmat->_42 = srcmat[3][1];
		dstmat->_43 = srcmat[3][2];
		dstmat->_44 = srcmat[3][3];
		***/

		return 0;
	};
	__inline int CD3DDisp::MatConvD3DX( D3DXMATRIX* dstmat, CMatrix2* srcmat )
	{
		float* srcdat = &(srcmat->data[0][0]);
		
		dstmat->_11 = *srcdat;
		dstmat->_12 = *(srcdat + 1);//[0][1];
		dstmat->_13 = *(srcdat + 2);//[0][2];
		dstmat->_14 = *(srcdat + 3);//[0][3];

		dstmat->_21 = *(srcdat + 4);//[1][0];
		dstmat->_22 = *(srcdat + 5);//[1][1];
		dstmat->_23 = *(srcdat + 6);//[1][2];
		dstmat->_24 = *(srcdat + 7);//[1][3];
		
		dstmat->_31 = *(srcdat + 8);//[2][0];
		dstmat->_32 = *(srcdat + 9);//[2][1];
		dstmat->_33 = *(srcdat + 10);//[2][2];
		dstmat->_34 = *(srcdat + 11);//[2][3];

		dstmat->_41 = *(srcdat + 12);//[3][0];
		dstmat->_42 = *(srcdat + 13);//[3][1];
		dstmat->_43 = *(srcdat + 14);//[3][2];
		dstmat->_44 = *(srcdat + 15);//[3][3];
		

		return 0;
	};


	__inline int MatConvCM2( CMatrix2* dstmat, D3DXMATRIX* srcmat )
	{
		
		dstmat->data[0][0] = srcmat->_11;
		dstmat->data[0][1] = srcmat->_12;
		dstmat->data[0][2] = srcmat->_13;
		dstmat->data[0][3] = srcmat->_14;

		dstmat->data[1][0] = srcmat->_21;
		dstmat->data[1][1] = srcmat->_22;
		dstmat->data[1][2] = srcmat->_23;
		dstmat->data[1][3] = srcmat->_24;
		
		dstmat->data[2][0] = srcmat->_31;
		dstmat->data[2][1] = srcmat->_32;
		dstmat->data[2][2] = srcmat->_33;
		dstmat->data[2][3] = srcmat->_34;

		dstmat->data[3][0] = srcmat->_41;
		dstmat->data[3][1] = srcmat->_42;
		dstmat->data[3][2] = srcmat->_43;
		dstmat->data[3][3] = srcmat->_44;

		return 0;
	};



	__inline int XVec3Cross( D3DXVECTOR3* pOut, D3DXVECTOR3* pV1, D3DXVECTOR3* pV2 )
	{
		//D3DXVECTOR3 v;
	 
		float x1, y1, z1, x2, y2, z2;
		x1 = pV1->x; y1 = pV1->y; z1 = pV1->z;
		x2 = pV2->x; y2 = pV2->y; z2 = pV2->z;

		pOut->x = y1 * z2 - z1 * y2;
		pOut->y = z1 * x2 - x1 * z2;
		pOut->z = x1 * y2 - y1 * x2;

		//v.x = pV1->y * pV2->z - pV1->z * pV2->y;
		//v.y = pV1->z * pV2->x - pV1->x * pV2->z;
		//v.z = pV1->x * pV2->y - pV1->y * pV2->x;
	 
		//*pOut = v;

		return 0;
	};

	__inline int XVec3Normalize( D3DXVECTOR3* retvec, D3DXVECTOR3* srcvec )
	{
		float mag;
		float srcx, srcy, srcz;
		srcx = srcvec->x; srcy = srcvec->y; srcz = srcvec->z;

		mag = srcx * srcx + srcy * srcy + srcz * srcz;
		float leng;
		leng = (float)sqrtf( mag );

		if( leng > 0.0f ){
			float divleng;
			divleng = 1.0f / leng;
			retvec->x = srcx * divleng;
			retvec->y = srcy * divleng;
			retvec->z = srcz * divleng;
		}else{
			retvec->x = 0.0f;
			retvec->y = 0.0f;
			retvec->z = 0.0f;
		}

		return 0;
	};

	__inline int CalcNormal( D3DXVECTOR3* newn, SKINVERTEX* curp, SKINVERTEX* aftp1, SKINVERTEX* aftp2 )
	{
		D3DXVECTOR3 vec1, vec2, crossvec;

		vec1.x = aftp1->pos[0] - curp->pos[0];
		vec1.y = aftp1->pos[1] - curp->pos[1];
		vec1.z = aftp1->pos[2] - curp->pos[2];

		vec2.x = aftp2->pos[0] - curp->pos[0];
		vec2.y = aftp2->pos[1] - curp->pos[1];
		vec2.z = aftp2->pos[2] - curp->pos[2];

		//D3DXVec3Cross( &crossvec, &vec1, &vec2 );
		//D3DXVec3Normalize( newn, &crossvec );

		XVec3Cross( &crossvec, &vec1, &vec2 );
		XVec3Normalize( newn, &crossvec );

		return 0;
	};

	__inline int CalcNormal( D3DXVECTOR3* newn, D3DTLVERTEX* curp, D3DTLVERTEX* aftp1, D3DTLVERTEX* aftp2 )
	{
		D3DXVECTOR3 vec1, vec2, crossvec;

		vec1.x = aftp1->sx - curp->sx;
		vec1.y = aftp1->sy - curp->sy;
		vec1.z = aftp1->sz - curp->sz;

		vec2.x = aftp2->sx - curp->sx;
		vec2.y = aftp2->sy - curp->sy;
		vec2.z = aftp2->sz - curp->sz;

		//D3DXVec3Cross( &crossvec, &vec1, &vec2 );
		//D3DXVec3Normalize( newn, &crossvec );

		XVec3Cross( &crossvec, &vec1, &vec2 );
		XVec3Normalize( newn, &crossvec );

		return 0;
	};

	__inline int CalcNormal( D3DXVECTOR3* newn, D3DVERTEX* curp, D3DVERTEX* aftp1, D3DVERTEX* aftp2 )
	{
		D3DXVECTOR3 vec1, vec2, crossvec;

		vec1.x = aftp1->x - curp->x;
		vec1.y = aftp1->y - curp->y;
		vec1.z = aftp1->z - curp->z;

		vec2.x = aftp2->x - curp->x;
		vec2.y = aftp2->y - curp->y;
		vec2.z = aftp2->z - curp->z;

		//D3DXVec3Cross( &crossvec, &vec1, &vec2 );
		//D3DXVec3Normalize( newn, &crossvec );

		XVec3Cross( &crossvec, &vec1, &vec2 );
		XVec3Normalize( newn, &crossvec );

		return 0;
	};

	__inline int CalcNormal( D3DXVECTOR3* newn, D3DXVECTOR3* curp, D3DXVECTOR3* aftp1, D3DXVECTOR3* aftp2 )
	{
		D3DXVECTOR3 vec1, vec2, crossvec;

		vec1.x = aftp1->x - curp->x;
		vec1.y = aftp1->y - curp->y;
		vec1.z = aftp1->z - curp->z;

		vec2.x = aftp2->x - curp->x;
		vec2.y = aftp2->y - curp->y;
		vec2.z = aftp2->z - curp->z;

		//D3DXVec3Cross( &crossvec, &vec1, &vec2 );
		//D3DXVec3Normalize( newn, &crossvec );

		XVec3Cross( &crossvec, &vec1, &vec2 );
		XVec3Normalize( newn, &crossvec );

		return 0;
	};


	
	__inline int ConvScreenPos( int projmode, float xp, float yp, float zp, float wp, float h, float w, float aspect, D3DTLVERTEX* dsttlv )
	{
		float invwp;

		if( wp != 0.0f )
			invwp = 1.0f / wp;
		else
			invwp = 1e6;

		float xp2, yp2;
		xp2 = xp * invwp;
		yp2 = yp * invwp;

		
		if( projmode == PROJ_NORMAL ){
			dsttlv->sx  = ( 1.0f + xp2 ) * aspect * h + w - aspect * h;
			dsttlv->sy  = ( 1.0f - yp2 ) * h;
			dsttlv->sz  = zp * invwp;
			//dsttlv->rhw = wp;
			dsttlv->rhw = invwp;
		}else{
			float dx, dy;
			float deg, scale;

			dx = (float)fabs( xp2 );
			dy = (float)fabs( yp2 );

			deg = sqrtf( dx * dx + dy * dy );
			if( deg > PAIDIV2 )
				deg = (float)PAIDIV2;

			scale = 0.8f + 0.5f / tanf( deg );
						
			//dsttlv->sx  = ( 1.0f + xp2 * scale ) * dwClipWidth;
			dsttlv->sx  = ( 1.0f + xp2 * scale ) * aspect * h + w - aspect * h;
			dsttlv->sy  = ( 1.0f - yp2 * scale ) * h;
			dsttlv->sz  = zp * invwp;
			//dsttlv->rhw = wp;
			dsttlv->rhw = invwp;
		}

		return 0;

	};
	



	__inline int ChkConfVecAndFace2( D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, CPCoef* curpc, D3DXVECTOR3* v1, D3DXVECTOR3* v2, D3DXVECTOR3* v3, int* resultptr, CONFDATA* cdptr )
	{
		//int ret;

	//面と直線の交点を求める。
		D3DXVECTOR3 finalv;
		float i0, i1, finalt;
		
		int inbetween = 0;

		i0 = curpc->m_a * befpos->x + curpc->m_b * befpos->y + curpc->m_c * befpos->z;
		i1 = curpc->m_a * newpos->x + curpc->m_b * newpos->y + curpc->m_c * newpos->z;

		if( (befpos->x == newpos->x) && (befpos->z == newpos->z) ){

			//_ASSERT( curpc->m_b != 0.0f );
			if( curpc->m_b != 0.0f ){
				finalv.x = befpos->x;
				finalv.y = -( curpc->m_a * befpos->x + curpc->m_c * befpos->z + curpc->m_d ) / curpc->m_b;
				finalv.z = befpos->z;

				if( ((befpos->y <= finalv.y) && (finalv.y <= newpos->y)) ||
					((newpos->y <= finalv.y) && (finalv.y <= befpos->y)) ){

					inbetween = 1;
				}else{
					inbetween = 0;
				}
			}else{

				//面の法線ベクトルと、線分が垂直の場合。
				//つまり、面と線分が平行。
				
				//newposを、平面の式に代入して、成り立てば、線分全てが、平面上にある。
				//その場合、応急処置として、befposをfinalvとする。

				//成り立たない場合は、交点は存在しない。
				float testplane;
				testplane = i1 + curpc->m_d;

				if( testplane == 0.0f ){
					finalv = *befpos;
					finalt = 1.0f;//!!!!!!
					inbetween = 1;

	//////////// すり抜け防止のために、この場合は、あたりと見なす。
					*resultptr = 1;
					cdptr->adjustv = finalv;
					
					D3DXVECTOR3 nvec;
					nvec.x = curpc->m_a;
					nvec.y = curpc->m_b;
					nvec.z = curpc->m_c;
					//D3DXVec3Normalize( &nvec, &nvec );
					cdptr->nv = nvec;
					return 0;


				}else{
					inbetween = 0;
				}
			}

		}else{
			if( i0 != i1 ){
				finalt = -( i1 + curpc->m_d ) / ( i0 - i1 );
				
				finalv.x = befpos->x * finalt + newpos->x * ( 1.0f - finalt );
				finalv.y = befpos->y * finalt + newpos->y * ( 1.0f - finalt );
				finalv.z = befpos->z * finalt + newpos->z * ( 1.0f - finalt );

				if( (finalt >= 0.0f) && (finalt <= 1.0f) ){
					inbetween = 1;
				}else{
					inbetween = 0;
				}

			}else{
				//面の法線ベクトルと、線分が垂直の場合。
				//つまり、面と線分が平行。
				
				//newposを、平面の式に代入して、成り立てば、線分全てが、平面上にある。
				//その場合、応急処置として、befposをfinalvとする。

				//成り立たない場合は、交点は存在しない。
				float testplane;
				testplane = i1 + curpc->m_d;

				if( testplane == 0.0f ){
					finalv = *befpos;
					finalt = 1.0f;//!!!!!!
					inbetween = 1;

	//////////// すり抜け防止のために、この場合は、あたりと見なす。
					*resultptr = 1;
					cdptr->adjustv = finalv;
					
					D3DXVECTOR3 nvec;
					nvec.x = curpc->m_a;
					nvec.y = curpc->m_b;
					nvec.z = curpc->m_c;
					//D3DXVec3Normalize( &nvec, &nvec );
					cdptr->nv = nvec;
					return 0;


				}else{
					inbetween = 0;
				}

	//_ASSERT( 0 );
			}
		}


		
	//交点が、線分の間にあるかチェックする。
		if( inbetween == 0 ){
			//交点が無い場合は、return
			*resultptr = 0;
			return 0;
		}

		
	//三角形を投影して、２次元にする。
		  float a1,b1,a2,b2,a3,b3,a4,b4;
		  float center_x,center_y;
		  float m1,m2,m3;
		  float bb1,bb2,bb3;
		  int inside,direction;
		  int AB_vert,BC_vert,CA_vert;

		  int droptype;
		  if( (fabs( curpc->m_a ) >= fabs( curpc->m_b )) && (fabs( curpc->m_a ) >= fabs( curpc->m_c )) ){
			droptype = 1;

		  }else if( (fabs( curpc->m_b ) >= fabs( curpc->m_a )) && (fabs( curpc->m_b ) >= fabs( curpc->m_c )) ){
			droptype = 2;

		  }else if( (fabs( curpc->m_c ) >= fabs( curpc->m_a )) && (fabs( curpc->m_c ) >= fabs( curpc->m_b )) ){
			droptype = 3;
		  }
		  
		  switch( droptype ){
		  case 1:
				a1 = v1->y;
				b1 = v1->z;
				a2 = v2->y;
				b2 = v2->z;
				a3 = v3->y;
				b3 = v3->z;
				a4 = finalv.y;
				b4 = finalv.z;
				inside = 0;
			  break;
		  case 2:
				a1 = v1->x;
				b1 = v1->z;
				a2 = v2->x;
				b2 = v2->z;
				a3 = v3->x;
				b3 = v3->z;
				a4 = finalv.x;
				b4 = finalv.z;
				inside = 0;
			  break;
		  case 3:
				a1 = v1->x;
				b1 = v1->y;
				a2 = v2->x;
				b2 = v2->y;
				a3 = v3->x;
				b3 = v3->y;
				a4 = finalv.x;
				b4 = finalv.y;
				inside = 0;
			  break;
		  default:
			  _ASSERT( 0 );
				a1 = v1->y;
				b1 = v1->z;
				a2 = v2->y;
				b2 = v2->z;
				a3 = v3->y;
				b3 = v3->z;
				a4 = finalv.y;
				b4 = finalv.z;
				inside = 0;
			  break;
		  }

		  AB_vert = BC_vert = CA_vert = FALSE;

		  // y=mx+b for outer 3 lines
		  if ((a2-a1)!=0) {
			m1 = (b2-b1)/(a2-a1); // a->b
			bb1 = (b1)-(m1*a1);    // y/(mx) using vertex a
		  } else if ((a2-a1)==0) {
			  m1 = 0.0f;
			  bb1 = 0.0f;
		   AB_vert = TRUE;
		  }

		  if ((a3-a2)!=0) {
			m2 = (b3-b2)/(a3-a2); // b->c
			bb2 = (b2)-(m2*a2);    // y/(mx) using vertex b
		  } else if ((a3-a2)==0) {
			  m2 = 0.0f;
			  bb2 = 0.0f;
		   BC_vert = TRUE;
		  }

		  if ((a1-a3)!=0) {
			m3 = (b1-b3)/(a1-a3); // c->a
			bb3 = (b3)-(m3*a3);    // y/(mx) using vertex c
		  } else if ((a1-a3)==0) {
			  m3 = 0.0f;
			  bb3 = 0.0f;
		   CA_vert = TRUE;
		  }



		//三角点内部に交点があるかどうかチェックする。

			// find average point of triangle (point is guaranteed
		  center_x = (a1+a2+a3)/3.0f;        // to lie inside the triangle)
		  center_y = (b1+b2+b3)/3.0f;

		  // a->b
		  if (((m1*center_x)+bb1) >= center_y)
			DIRECTION(direction,UP);
		  else
			DIRECTION(direction,DOWN);
		  if (AB_vert==TRUE) {
			if ((a1 <= a4)&&(a1 <= center_x)) // vert projected line
			  inside++;
			else if ((a1 >= a4)&&(a1 >= center_x)) // vert projected line
			  inside++;
		  } else {
			if (direction==UP) {
			  if (b4 <= ((m1*a4)+bb1)) // b4 less than y to be inside
				inside++;              // (line is above point)
			} else if (direction==DOWN) {
			  if (b4 >= ((m1*a4)+bb1)) // b4 greater than y to be inside
				inside++;              // (line is below point)
			}
		  }

		  // b->c
		  if (((m2*center_x)+bb2) >= center_y)
			DIRECTION(direction,UP);
		  else
			DIRECTION(direction,DOWN);
		  if (BC_vert==TRUE) {
			if ((a2 <= a4)&&(a2 <= center_x)) // vert projected line
			  inside++;
			else if ((a2 >= a4)&&(a2 >= center_x)) // vert projected line
			  inside++;
		  } else {
			if (direction==UP) {
			  if (b4 <= ((m2*a4)+bb2)) // b4 less than y to be inside 
				inside++;              // (line is above point)
			} else if (direction==DOWN) {
			  if (b4 >= ((m2*a4)+bb2)) // b4 greater than y to be inside
				inside++;              // (line is below point)
			}
		  }
		  // c->a
		  if (((m3*center_x)+bb3) >= center_y)
			DIRECTION(direction,UP);
		  else
			DIRECTION(direction,DOWN);
		  if (CA_vert==TRUE) {
			if ((a3 <= a4)&&(a3 <= center_x)) // vert projected line
			  inside++;
			else if ((a3 >= a4)&&(a3 >= center_x)) // vert projected line
			  inside++;
		  } else {
			if (direction==UP) {
			  if (b4 <= ((m3*a4)+bb3)) // b4 less than y to be inside 
				inside++;              // (line is above point)
			} else if (direction==DOWN) {
			  if (b4 >= ((m3*a4)+bb3)) // b4 greater than y to be inside 
				inside++;              // (line is below point)
			}
		  }

		  
		  if (inside==3) {
			//return TRUE;
			*resultptr = 1;
			cdptr->adjustv = finalv;
			
			D3DXVECTOR3 nvec;
			nvec.x = curpc->m_a;
			nvec.y = curpc->m_b;
			nvec.z = curpc->m_c;
			//D3DXVec3Normalize( &nvec, &nvec );
			cdptr->nv = nvec;

		  } else {
			//return FALSE;
			*resultptr = 0;
		  }
		

		return 0;
	};

public:
	static int s_bonebmpsizex;
	static int s_bonebmpsizey;

	static DWORD totalPrim;
	static DWORD invalidPrim;
	static DWORD pracPrim;
	
	static float s_minx, s_maxx;
	static float s_miny, s_maxy;
	static float s_minz, s_maxz;

	static float s_tbsminx, s_tbsmaxx;// total bsphere 用の　バウンダリー情報
	static float s_tbsminy, s_tbsmaxy;
	static float s_tbsminz, s_tbsmaxz;


	float bsminx, bsmaxx; // bsphere　用の　バウンダリー情報
	float bsminy, bsmaxy;
	float bsminz, bsmaxz;


//	D3DTLVERTEX* m_orgTLV;// for TLmode_ORG
//	D3DTLVERTEX* m_orgrevTLV;// for TLmode_ORG

//	D3DTLVERTEX* m_preTLV;// for TLmode_ORG

//	D3DTLVERTEX* m_dispTLV;// for TLmode_ORG
//	D3DTLVERTEX* m_disprevTLV;// for TLmode_ORG
//	D3DTLVERTEX* m_dispsubTLV;// for TLmode_ORG

//	D3DVERTEX* m_orgV;// !!!!!  for TLmode_D3D !!!!!!!!
//	D3DVERTEX* m_orgrevV;// !!!!!  for TLmode_D3D !!!!!!!!

	D3DLVERTEX* m_lv;// for ExtLine


	D3DXVECTOR3* m_orgNormal;
	D3DXVECTOR3* m_revNormal;
	D3DXVECTOR3* m_faceNormal;

	//D3DVERTEXELEMENT9 m_skindecl[10];
	IDirect3DVertexDeclaration9* m_pskindecl_tima;
	IDirect3DVertexDeclaration9* m_pskindecl_mema;
	SKINVERTEX* m_skinv;
	SKINVERTEX* m_revskinv;
	SKINMATERIAL* m_smaterial;


	D3DXVECTOR3* m_faceBinormal;//PM2
	D3DXVECTOR3* m_revfaceBinormal;//PM2
	D3DXVECTOR3* m_Binormal;//PM
	D3DXVECTOR3* m_revBinormal;//PM
//	D3DXVECTOR3* m_Trangent;
//	D3DXVECTOR3* m_revTangent;

	IDirect3DVertexDeclaration9* m_pskintandecl;
//	IDirect3DVertexDeclaration9* m_pskintandecl_tima;
//	IDirect3DVertexDeclaration9* m_pskintandecl_mema;
	SKINTANGENT* m_skintan;
	SKINTANGENT* m_revskintan;


//	IDirect3DVertexDeclaration9* m_plinedecl;



	int mskinblocknum;
	int msaveskinblocknum;//destroy用。変化前の数を保存。
	SKINBLOCK* mskinblock;
	int mrenderblocknum;
	RENDERBLOCK* mrenderblock;
	int** mbonemat2skinmat;
	int** mskinmat2bonemat;
	int* mskinmatnum;
	D3DXMATRIX** mskinmat;

	int m_blurtime;
	int m_blurpos;
	D3DXMATRIX** m_blurmat;
	D3DXMATRIX* m_blurViewMat;


	//RENDERBLOCK* mclipblock;
	//RENDERBLOCK* mcliprevblock;
	//RENDERBLOCK* mcullblock;
	//RENDERBLOCK* mcullrevblock;

	//D3DXVECTOR3 m_orglightvec;
	//D3DXVECTOR3 m_lightvec;

    LPDIRECT3DVERTEXBUFFER9 m_VB;
//    LPDIRECT3DVERTEXBUFFER9 m_revVB;
//	LPDIRECT3DVERTEXBUFFER9 m_subVB;
	LPDIRECT3DVERTEXBUFFER9* m_ArrayVB;
	LPDIRECT3DVERTEXBUFFER9* m_revArrayVB;

    LPDIRECT3DVERTEXBUFFER9 m_VBtan;
    LPDIRECT3DVERTEXBUFFER9 m_revVBtan;

	LPDIRECT3DVERTEXBUFFER9 m_VBmaterial;

	LPDIRECT3DINDEXBUFFER9 m_IB;
	LPDIRECT3DINDEXBUFFER9 m_revIB;
//	LPDIRECT3DINDEXBUFFER9 m_subIB;


//	LPDIRECT3DVERTEXBUFFER9*	m_AfterVB;
//	LPDIRECT3DINDEXBUFFER9*		m_AfterIB;
//	DWORD*						m_afterPrim;


	DWORD                   m_numTLV;

    int*	                m_dispIndices;
	int*					m_revIndices;//裏面
    DWORD                   m_numIndices;
	DWORD					m_numPrim;


	D3DXMATRIX				m_matWorld;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matProj;

//	char*					m_dispdirtyflag;

//	char*					m_pmdirtyflag;
//	char*					m_pm2dirtyflag;
//	char*					m_pmrevdirtyflag;
//	char*					m_pm2revdirtyflag;


	D3DXVECTOR3*			m_worldv;
	D3DXVECTOR3*			m_scv;
	CBSphere*				m_bs;
	CPCoef*					m_pc;

	//D3DXVECTOR3				m_scale;
	//D3DXVECTOR3				m_scalecen;
	D3DXMATRIX				m_scalemat;

	int m_unum;
	int m_vnum;

	//CBoneElem*	m_BE;
	CInfElem* m_IEptr;


	int m_afterimage;

	int m_dbgfindcnt;

	int m_TLmode;
	int m_linekind;

	int m_savelightflag;

	DWORD m_dwClipWidth;
	DWORD m_dwClipHeight;

	D3DXVECTOR3 m_EyePt;

	LPDIRECT3DDEVICE9 m_pdev;
	int m_clockwise;

	float m_umove;
	float m_vmove;
	
	float m_alpha;

	DWORD* m_tempdiffuse;//leng m_numTLV
	int m_blurmode;
	D3DXMATRIX m_oldView;

};
#endif

