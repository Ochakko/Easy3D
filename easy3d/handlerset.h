#ifndef HANDLERSETH
#define HANDLERSETH

#include <D3DX9.h>


class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CBillboardIO;
class CNaviLine;
class CQuaternion;
class CQuaternion2;
class CExtLineIO;
class CShdElem;
class CMotionCtrl;
class CMCHandler;
class CVec3List;
class CGPAnimHandler;
class CMOEAnimHandler;

#include <coef.h>

#include <quaternion.h>
#include <quaternion2.h>
#include <npcleardata.h>
#include <GPData.h>

class CPanda;

class CHandlerSet
{
public:
	CHandlerSet();
	~CHandlerSet();

	int LoadSigFile( char* fname, int adjustuvflag, float srcmult );
	int LoadSigFileFromPnd( CPanda* panda, int pndid, int propno, float srcmult );	
	int LoadQuaFile( char* fname, int* cookieptr, int* framemaxptr, float mvmult, CPanda* panda = 0, int pndid = 0 );
	int LoadMotFile( char* fname, int* cookieptr, int* framemaxptr );
	int LoadBIMFile( char* fname );
	int LoadIM2File( char* fname );

	int ImportSigFile( char* fname, int adjustuvflag, float srcmult, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot );
	int LoadSigFileFromBuf( char* bufptr, int bufsize, int adjustuvflag, float srcmult );
	int ImportSigFileFromBuf( char* bufptr, int bufsize, int adjustuvflag, float srcmult, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot );


	int LoadQuaFileFromBuf( char* bufptr, int bufsize, int* cookieptr, int* framemaxptr, float mvmult );	
	int LoadMotFileFromBuf( char* bufptr, int bufsize, int* cookieptr, int* framemaxptr );

	//int LoadGroundBMP( char* bmpname, char* texname, int divx, int divy, float maxheight );
	int LoadGroundBMP( char* bmpname1, char* bmpname2, char* bmpname3, char* texturename, float maxx, float maxz, int divx, int divz, float maxheight );


	int SaveMQOFile( char* filename, HWND apphwnd );
	int SaveSigFile( char* tempname1 );
	int SaveIM2File( char* filename );
	int SaveQuaFile( int quatype, int mkid, char* filename );
	
	int SaveSig2Buf( char* buf, int bufsize, int* writesize );
	int SaveQua2Buf( int quatype, int mkid, char* buf, int bufsize, int* writesize );

	int SaveGNDFile( char* tempname1 );
	int LoadGNDFile( char* fname, int adjustuvflag, float srcmult );
	int LoadGNDFileFromBuf( char* bufptr, int bufsize, int adjustuvflag );
	

	int SetMotionKind( int motcookie );
	int GetMotionKind( int* motkindptr );
	int SetNewPose();

	//int Transform( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX matView, int framecnt, int lightflag, int projmode, int divmode );
	//int Transform( D3DXVECTOR3 camerapos, LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX matView, int framecnt, int lightflag, int projmode, int* bonearray, int trabone_only );
	int Transform( D3DXVECTOR3 camerapos, LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX matView, int lightflag, int trabone_only );
	
	int TransformBillboard( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX matView, D3DXVECTOR3* camerapos, D3DXVECTOR3* cameratarget );
	int TransformBillboardDispData( D3DXMATRIX matView, D3DXVECTOR3 campos );	
	int UpdateParticle( D3DXMATRIX matView, D3DXVECTOR3* camerapos, D3DXVECTOR3* cameratarget, int srcfps );
	
	int Render( int inbatch, LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, D3DXMATRIX matView, D3DXVECTOR3 camerapos );
	int RenderShadowMap0( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3 camerapos );
	int RenderShadowMap1( LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3 camerapos );

	int Restore( LPDIRECT3DDEVICE9 pd3dDevice, HWND appwnd, int billboardid, int needcalc, CPanda* panda = 0, int pndid = 0 );
	int Invalidate( int resetflag );
	int SetMotionStep( int motcookie, int step );

	int SerialnoCmp( CHandlerSet* cmphs );
	int SerialnoCmp( int cmpno );
//	int AddToPrev( CHandlerSet* addhs );
//	int AddToNext( CHandlerSet* addhs );
//	int LeaveFromChain();

	int CreateHandler();

	int SetPosition( D3DXVECTOR3 pos );
	//int SetRotate( D3DXVECTOR3 rot );
	int SetRotate( D3DXMATRIX dirmat );
	int GetDirVector3( int mult, int* xptr, int* yptr, int* zptr );


	int ChkConflict( CHandlerSet* chkhs, D3DXVECTOR3 rate1, D3DXVECTOR3 rate2, int* inviewflag );
	int ChkConflict2( int srcpartno, CHandlerSet* chkhs, int chkpartno, D3DXVECTOR3 rate1, D3DXVECTOR3 rate2, int* inviewflag );
	int ChkConflict3( int* srcpartno, int srcpartnum, CHandlerSet* chkhs, int* chkpartno, int chkpartnum, D3DXVECTOR3 rate1, D3DXVECTOR3 rate2, int* inviewflag );
	int ChkConfBySphere( CHandlerSet* chkhs, int conflevel, int* confflagptr );
	int ChkConfBySphere2( int srcpartno, CHandlerSet* chkhs, int chkpartno, int* confflagptr );
	int ChkConfBySphere3( int* srcpartno, int srcpartnum, CHandlerSet* chkhs, int* chkpartno, int chkpartnum, int* confflagptr );

	int ChkConfBillboard( CHandlerSet* chkhs, int* confflagptr, float confrate, int* confbbid, int arrayleng, int* confnumptr );
	int ChkConfParticle( CHandlerSet* chkhs, float rate, int* flagptr );

	int CreateAfterImage( LPDIRECT3DDEVICE9 pd3dDevice, int imagenum );
	int DestroyAfterImage();

	int SetAlpha( float alpha, int partno, int updateflag );
	//int SetDirectionalLight( D3DXVECTOR3 dir );

	int GetBoundaryBox( float* dstptr );

	int SetDispSwitch( int mk, int swid, int srdframeno, int flag );
	int SetDispSwitch2( int boneno, int mk, int swid, int srdframeno, int flag );
	int SetDispSwitchForce( int swid, int state );
	int GetDispSwitch( int motid, int frameno, int* dswitch );
	int GetDispSwitch2( int* swptr, int leng );

	int SetMotionFrameNo( int mk, int srcframeno );
	int SetNextMotionFrameNo( int mk, int nextmk, int nextframeno, int befframeno );

	int SetCurrentBSphereData();

	//int ChkInView( D3DXVECTOR3 viewpos, D3DXVECTOR3 viewvec, float projnear, float projfar, float projfov );
	//int ChkInView( D3DXVECTOR3 viewpos, D3DXVECTOR3 viewvec, float projnear, float projfar, float* projindex );
	int ChkInView( D3DXMATRIX matView, int* retptr, D3DXMATRIX* matProj );


	int SetBeforePos();
	int ChkConfGround( CHandlerSet* charahs, int mode, float diffmaxy, float mapminy, int* resultptr, float* adxptr, float* adyptr, float* adzptr, float* nxptr, float* nyptr, float* nzptr );
	int ChkConfGround2( D3DXVECTOR3 befpos, D3DXVECTOR3 newpos, int mode, float diffmaxy, float mapminy, int* resultptr, float* adxptr, float* adyptr, float* adzptr, float* nxptr, float* nyptr, float* nzptr );
	int ChkConfGroundPart( CHandlerSet* charahs, int groundpart, int mode, float diffmaxy, float mapminy, int* resultptr, float* adxptr, float* adyptr, float* adzptr, float* nxptr, float* nyptr, float* nzptr );
	int ChkConfGroundPart2( int groundpart, D3DXVECTOR3 befpos, D3DXVECTOR3 newpos, int mode, float diffmaxy, float mapminy, int* resultptr, float* adxptr, float* adyptr, float* adzptr, float* nxptr, float* nyptr, float* nzptr );


	int GetPartNoByName( char* partname, int* partnoptr );
	int GetBoneNoByName( char* tempname, int* bonenoptr );

	int GetVertNumOfPart( int partno, int* vertnumptr );
	int GetVertPos( int kind, int partno, int vertno, float* vxptr, float* vyptr, float* vzptr );
	int SetVertPos( int partno, int vertno, float vertx, float verty, float vertz );
	int SetVertPosBatchDouble( int partno, int* vnoarray, int vnum, double* varray, int aorder );
	int SetVertPosBatchVec3( int partno, int* vnoarray, int vnum, D3DXVECTOR3* varray );

	int SetPosOnGround( CHandlerSet* hs, float mapmaxy, float mapminy, float posx, float posz );
	int SetPosOnGroundPart( int groundpart, CHandlerSet* hs, float mapmaxy, float mapminy, float posx, float posz );


	int CreateBillboard( int particleflag = 0, int cmpalways = 0 );
	int AddBillboard( char* bmpname, float width, float height, int transparentflag, int dirmode, int orgflag, int* bbidptr );
	int SetBillboardPos( int bbid, float posx, float posy, float posz );
	int SetBillboardDispFlag( int bbid, int flag );
	int SetBillboardOnGround( int bbid, CHandlerSet* groundhs, float mapmaxy, float mapminy, float posx, float posz );
	int DestroyBillboard( int bbid );
	int RotateBillboard( int bbid, float fdeg, int rotkind );
	int SetBillboardUV( int bbid, int unum, int vnum, int texno, int revuflag );
	int SetBillboardSize( int bbid, float width, float height, int dirmode, int orgflag );



	int LoadMQOFileAsGround( char* mqoname, float mult, HWND hwnd, int adjustuvflag, int* hsidptr, int bonetype );
	int LoadMQOFileAsChara( char* mqoname, float mult, HWND hwnd, int adjustuvflag, int* hsidptr, int bonetype );

	int ImportMQOFileAsGround( char* mqoname, HWND appwnd, int adjustuvflag, float fmult, D3DXVECTOR3 offset, D3DXVECTOR3 rot );


	int ChkConfBillboard2( float centerx, float centery, float centerz, float srcr, int* resultptr, float confrate, int* confbbid, int arrayleng, int* confnumptr );

	int GetBillboardInfo( int srcbbid, float* posxptr, float* posyptr, float* poszptr, char* texnameptr, int* transparentptr, float* widthptr, float* heightptr );
	int GetNearBillboard( float srcposx, float srcposy, float srcposz, float maxdist, int* bbidptr );
	int GetInvisibleFlag( int partno, int* flagptr );
	int SetInvisibleFlag( int partno, int srcflag );


	int SetMovableArea( char* bmpname, float maxx, float maxz, int divx, int divz, float wallheight );
	int ChkConfWall( CHandlerSet* charahs, float dist, int* resultptr, float* adjustxptr, float* adjustyptr, float* adjustzptr, float* nxptr, float* nyptr, float* nzptr );
	int ChkConfWall2( int* partarray, int partnum, float befposx, float befposy, float befposz, float newposx, float newposy, float newposz, float dist, int* resultptr, float* adjustxptr, float* adjustyptr, float* adjustzptr, float* nxptr, float* nyptr, float* nzptr );
	int ChkConfWall3( CHandlerSet* charahs, int* partarray, int partnum, float dist, int* resultptr, float* adjustxptr, float* adjustyptr, float* adjustzptr, float* nxptr, float* nyptr, float* nzptr );
	int LoadMQOFileAsMovableArea( char* mqoname, float mult, HWND hwnd, int* hsidptr );
	int LoadMQOFileAsMovableAreaFromBuf( HWND appwnd, char* bufptr, int bufsize, float fmult );

	int SetNaviLineOnGround( CNaviLine* nl, float mapmaxy, float mapminy );

	int SetWallOnGround( CHandlerSet* wallhs, float mapmaxy, float mapminy, float wheight );
	int MagnetPosition( float dist );


	int CreateNaviPointClearFlag( CNaviLine* nlptr, int roundnum );
	int DestroyNaviPointClearFlag();
	int InitNaviPointClearFlag();
	int SetNaviPointClearFlag( D3DXVECTOR3 srcpos, float maxdist, int* pidptr, int* roundptr, float* distptr );

	int SetValidFlag( int partno, int flag );
	int SetDispDiffuse( int partno, int r, int g, int b, int setflag, int vertno, int updateflag );
	int SetDispSpecular( int partno, int r, int g, int b, int setflag, int vertno );
	int SetDispAmbient( int partno, int r, int g, int b, int setflag, int vertno );
	int SetDispEmissive( int partno, int r, int g, int b, int setflag, int vertno );
	int SetDispSpecularPower( int partno, float pow, int setflag, int vertno );

	int GetDiffuse( int partno, int vertno, int* rptr, int* gptr, int* bptr );
	int GetAmbient( int partno, int vertno, int* rptr, int* gptr, int* bptr );	
	int GetSpecular( int partno, int vertno, int* rptr, int* gptr, int* bptr );
	int GetEmissive( int partno, int vertno, int* rptr, int* gptr, int* bptr );
	int GetSpecularPower( int partno, int vertno, float* powptr );
	int GetAlpha( int partno, int vertno, int* aptr );
	int GetAlphaF( int partno, int vertno, float* aptr );


	int SetBlendingMode( int partno, int bmode );
	int SetRenderState( int partno, int statetype, DWORD value );
	int SetScale( int partno, D3DXVECTOR3 scalevec, int centerflag );

	int GetScreenPos( int partno, int* scxptr, int* scyptr, D3DXMATRIX matView, int vertno, int calcmode );
	int GetScreenPos3( int partno, int* scxptr, int* scyptr, float* sczptr, D3DXMATRIX matView, int vertno, int calcmode );
	int GetScreenPos3F( int partno, int vertno, int calcmode, D3DXMATRIX matView, D3DXVECTOR3* scpos );


	int GetNextMP( int motid, int boneno, int prevmpid, int* mpidptr );
	int GetMPInfo( int motid, int boneno, int mpid, CQuaternion* dstq, D3DXVECTOR3* dsttra, int* dstframeno, DWORD* dstds, int* dstinterp, D3DXVECTOR3* dstscale, int* dstuserint1 );
	int SetMPInfo( int motid, int boneno, int mpid, CQuaternion* srcqptr, D3DXVECTOR3* srctra, int srcframeno, DWORD srcds, int srcinterp, D3DXVECTOR3* srcscale, int srcuserint1, int* infoflagptr );
	int SetMPInfo( int motid, int boneno, CQuaternion* setq, MPINFO2* mpiptr, int* flagptr );

	int IsExistMP( int motid, int boneno, int frameno, int* mpidptr );

	int GetMotionFrameLength( int motid, int* lengptr );
	int SetMotionFrameLength( int motid, int frameleng, int initflag );

	int AddMotionPoint( int motid, int boneno, CQuaternion* srcqptr, D3DXVECTOR3* srctra, int srcframeno, DWORD srcds, int srcinterp, D3DXVECTOR3* srcscale, int srcuserint1, int* mpidptr );
	int DeleteMotionPoint( int motid, int boneno, int mpid );

	int FillUpMotion( int motid, int boneno, int startframe, int endframe, int initflag );
	int CopyMotionFrame( int srcmotid, int srcframe, int dstmotid, int dstframe );

	int GetBonePos( int boneno, int poskind, int motid, int frameno, int scaleflag, D3DXVECTOR3* dstpos );
	int GetCurrentBonePos( int boneno, int poskind, D3DXVECTOR3* dstpos );


	int CreateLine( double* posptr, int pointnum, int maxpointnum, int linekind );
	int CreateLine( D3DXVECTOR3* posptr, int pointnum, int maxpointnum, int linekind );
	int SetExtLineColor( int a, int r, int g, int b );
	int AddPoint2ExtLine( int previd, int* newidptr );
	int DeletePointOfExtLine( int pid );
	int SetPointPosOfExtLine( int pid, D3DXVECTOR3 srcpos );
	int GetPointPosOfExtLine( int pid, D3DXVECTOR3* dstpos );
	int GetNextPointOfExtLine( int previd, int* nextptr );
	int GetPrevPointOfExtLine( int pid, int* prevptr );

	//int PickVert( HWND hwnd, int posx, int posy, int rangex, int rangey, int* partptr, int* vertptr, int arrayleng, int* getnumptr );
	int PickVert( D3DXMATRIX matView, int posx, int posy, int rangex, int rangey, int* partptr, int* vertptr, int arrayleng, int* getnumptr );
	int PickFace( D3DXMATRIX matView, float maxdist, int pos2x, int pos2y, int* partptr, int* faceptr, D3DXVECTOR3* pos3ptr, D3DXVECTOR3* nptr, float* distptr, int calcmode );
	int PickFace2( D3DXMATRIX matView, int* partarray, int partnum, float maxdist, int pos2x, int pos2y, int* partptr, int* faceptr, D3DXVECTOR3* pos3ptr, D3DXVECTOR3* nptr, float* distptr, int calcmode );

	int GetCullingFlag( int partno, int vertno, int* viewcullptr, int* revcullptr );
	int GetOrgVertNo( int partno, int vertno, int* orgnoarray, int arrayleng, int* getnumptr );

	int ChkIM2Status( int* statusptr );


	int GetJointNum( int* numptr );
	//int GetJointInfo( int maxnum, PVal* pvalstr, APTR aptrstr, int* seriptr, int* getnumptr );
	int GetJointInfo( int maxnum, char* nameptr, int* seriptr, int* getnumptr );
	int GetJointInfo( JOINTINFO* jiarray, int arrayleng, int* getnumptr );

	int GetFirstJointNo( int* noptr );

	//int GetMoaInfo( int maxnum, PVal* pvalstr, APTR aptrstr, int* idptr, int* getnumptr );
	int GetMoaInfo( int maxnum, char* nameptr, int* idptr, int* getnumptr );
	int GetMoaInfo( int maxnum, MOAINFO* moainfoptr, int* getnumptr );

	int GetDispObjNum( int* numptr );
	//int GetDispObjInfo( int maxnum, PVal* pvalstr, APTR aptrstr, int* seriptr, int* getnumptr );
	int GetDispObjInfo( int maxnum, char* nameptr, int* seriptr, int* getnumptr );
	int GetDispObjInfo( DISPOBJINFO* doiarray, int arrayleng, int* getnumptr );

	int SetIM2Params( int partno, int optvno, int childjoint, int parentjoint, int calcmode );
	int GetIM2Params( int partno, int optvno, int* childjointptr, int* parentjointptr, int* calcmodeptr );

	int EnableTexture( int partno, int enableflag );
	int SetTextureDirtyFlag( int srcseri );

	int JointAddToTree( int parentjoint, int childjoint, int lastflag );
	int JointRemoveFromParent( int rmjoint );
	int JointRemake();

	

	int GetBBox( int partno, int mode, float* fminx, float* fmaxx, float* fminy, float* fmaxy, float* fminz, float* fmaxz );
	int GetVertNoOfFace( int partno, int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr );
	int GetSamePosVert( int partno, int vertno, int* sameptr, int arrayleng, int* samenumptr );

	int IKRotateBeta( D3DXVECTOR3 camerapos, LPDIRECT3DDEVICE9 pdev, D3DXMATRIX matView, int jointno, int motid, int frameno, int iklevel, int axiskind, D3DXVECTOR3 axisvec, int calclevel, D3DXVECTOR3 targetpos, D3DXVECTOR3* respos, int* lastparent );
	int IKTranslate( int boneno, int motid, int frameno, D3DXVECTOR3 srcpos );
	int SetBoneArray( int srcseri );

	int GetMotionType( int motid, int* typeptr );
	int SetMotionType( int motid, int type );
	int GetIKTransFlag( int jointno, int* flagptr );
	int SetIKTransFlag( int jointno, int flag );

	int DestroyAllMotion();

	int GetUserInt1OfPart( int partno, int* userint1ptr );
	int SetUserInt1OfPart( int partno, int userint1 );

	int GetBSphere( int partno, D3DXVECTOR3* dstcenter, float* dstr );

	int GetChildJoint( int parentno, int arrayleng, int* childarray, int* childnum );

	int DestroyMotionFrame( int motid, int frameno );
	int HuGetKeyFrameNo( int motid, int boneno, int* framearray, int arrayleng, int* framenumptr );

	int ChkConfLineAndFace( D3DXVECTOR3 camdir, D3DXMATRIX matView, D3DXVECTOR3 srcpos1, D3DXVECTOR3 srcpos2, int needtrans, 
		int* partptr, int* faceptr, D3DXVECTOR3* dstpos, D3DXVECTOR3* dstn, int* revfaceptr );


	int SetUV( int partno, int vertno, float fu, float fv, int setflag, int clampflag );
	int SetUVTile( int partno, int texrule, int unum, int vnum, int tileno );
	int SetUVBatchUV( int partno, int* vertnoptr, int setnum, UV* uvptr, int setflag, int clampflag );


	int GetUV( int partno, int vertno, float* uptr, float* vptr );

	int PickBone( D3DXMATRIX matView, int posx, int posy, int* bonenoptr );
	int ShiftBoneTreeDiff( int jointno, D3DXVECTOR3 mvdiff, int motcookie, int frameno );

	int GetTextureFromDispObj( int partno, int* texidptr );
	int SetTextureToDispObj( int partno, int texid );


	int TwistBone( int motid, int frameno, int boneno, float fdeg );

	int GetBoneQ( int boneno, int motid, int frameno, int kind, CQuaternion* dstq );
	int GetCurrentBoneQ( int boneno, int kind, CQuaternion* dstq );

	int SetQ( int boneno, int motid, int frameno, CQuaternion srcq, int curboneonly = 0 );

	int GetShaderType( int* shaderptr, int* overflowptr );
	int SetShaderType( LPDIRECT3DDEVICE9 pdev, int shader, int overflow );

	int GetInfElemNum( int partno, int vertno, int* numptr );
	int GetInfElem( int partno, int vertno, int infno, int* childjointptr, int* calcmodeptr, float* userrate, float* orginf, float* dispinf );
	int SetInfElem( int partno, int vertno, int childjointno, int calcmode, int paintmode, int normalizeflag, float fuserrate, float fdirectval );
	int DeleteInfElem( int partno, int vertno, int childjointno, int normalizeflag );
	int SetInfElemDefault( int partno, int vertno );
	int NormalizeInfElem( int partno, int vertno );

	int GetVisiblePolygonNum( int* num1ptr, int* num2ptr );

	int SetToon0Params( int partno, float darkrate, float brightrate, float darkcos, float brightcos );

	int LoadMAFile( char* srcname, int fillupleng, HWND srchwnd, HMODULE hMod, float mvmult, CPanda* panda = 0, int pndid = 0 );


	int AddChildMC( int parentcookie, MCELEM childmc );
	int LoadMAFMotion( char* srcfilename, int srcidling, int ev0idle, int commonid, int forbidnum, int* forbidid, 
		int* cookieptr, float mvmult, int srcnotfu, CPanda* panda = 0, int pndid = 0 );
	int SetNewPoseByMOA( int eventno );

	int GetMotionFrameNo( int* dstmotkind, int* dstmotframe );

	int GetNextMotionFrameNo( int motid, int* nextid, int* nextframe );

	int EnableToonEdge( int srcpartno, int srctype, int srcflag );
	int SetToonEdge0Color( int srcpartno, char* srcname, int srcr, int srcg, int srcb );
	int SetToonEdge0Width( int srcpartno, char* srcname, float srcwidth );
	int GetToonEdge0Color( int srcpartno, char* srcname, float* dstr, float* dstg, float* dstb );
	int GetToonEdge0Width( int srcpartno, char* srcname, float* dstwidth );

	int SetParticlePos( D3DXVECTOR3 srcpos );
	int SetParticleGravity( float srcgravity );
	int SetParticleLife( float srclife );
	int SetParticleEmitNum( float srcemitnum );
	int SetParticleVel0( D3DXVECTOR3 srcminvel, D3DXVECTOR3 srcmaxvel );
	int SetParticleAlpha( float srcmintime, float srcmaxtime, float srcalpha );
	int SetParticleUVTile( float srcmintime, float srcmaxtime, int srcunum, int srcvnum, int srctileno );
	int InitParticle();

	int GetMotionIDByName( char* srcmotname, int* dstid );
	int GetMotionNum( int* motnum );

	int GetCenterPos( int partno, D3DXVECTOR3* posptr );

	int GetFaceNum( int partno, int* facenumptr );
	int GetFaceNormal( int partno, int faceno, D3DXVECTOR3* dstn );

	int CreateEmptyMotion( char* motname, int frameleng, int* motidptr, int zatype = ZA_1 );
	int SetTextureMinMagFilter( int partno, int minfilter, int magfilter );
	
	//int GetMotionName( PVal* pvalstr, APTR aptrstr, int motid );
	int GetMotionName( char* nameptr, int motid );
	int SetMotionName( int motid, char* srcmotname );

	int GetMaterialNoByName( char* srcname, int* matnoptr );
	int GetMaterialAlpha( int matno, float* alphaptr );
	int GetMaterialDiffuse( int matno, int* rptr, int* gptr, int* bptr );
	int GetMaterialSpecular( int matno, int* rptr, int* gptr, int* bptr );
	int GetMaterialAmbient( int matno, int* rptr, int* gptr, int* bptr );
	int GetMaterialEmissive( int matno, int* rptr, int* gptr, int* bptr );
	int GetMaterialPower( int matno, float* powptr );
	int GetMaterialBlendingMode( int matno, int* modeptr );

	int SetMaterialAlpha( int matno, float srcalpha );
	int SetMaterialDiffuse( int matno, int setflag, int r, int g, int b );
	int SetMaterialSpecular( int matno, int setflag, int r, int g, int b );
	int SetMaterialAmbient( int matno, int setflag, int r, int g, int b );
	int SetMaterialEmissive( int matno, int setflag, int r, int g, int b );
	int SetMaterialPower( int matno, int setflag, float power );
	int SetMaterialBlendingMode( int matno, int mode );

	int GetTextureFromMaterial( int matno, int* texidptr );
	int SetTextureToMaterial( int matno, int texid );
	int SetBumpMapToMaterial( int matno, int texid );

	int GetMaterialNo( int partno, int faceno, int* matnoptr );

	int SetMotionKindML( int motid, int* listptr, int* notlistptr );
	int SetMotionFrameNoML( int motid, int frameno, int* listptr, int* notlistptr );
	int SetNextMotionFrameNoML( int motid, int nextmk, int nextframe, int befframe, int* listptr, int* notlistptr );
	int GetMotionFrameNoML( int boneno, int* dstmotkind, int* dstmotframe );
	int GetNextMotionFrameNoML( int boneno, int motid, int* nextid, int* nextframe );
	int SetNewPoseML();
	int SetNewPoseByMOAML( int eventno, int* listptr, int* notlistptr );
	

	int GetMOATrunkInfo( int motid, int* infoptr );
	int GetMOATrunkInfo( int motid, MOATRUNKINFO* trunkptr );
	int GetMOABranchInfo( int motid, int* infoptr, int branchnum, int* getnum );
	int GetMOABranchInfo( int motid, MOABRANCHINFO* branchptr, int arrayleng, int* getnum );
	int SetMOABranchFrame1( int tmotid, int bmotid, int frame1 );
	int SetMOABranchFrame2( int tmotid, int bmotid, int frame2 );

	int EnablePhongEdge0( int flag );
	int SetPhongEdge0Params( int r, int g, int b, float width, int blendmode, float alpha );

	int SetResDir( char* pathname );
	char* GetResDir();
	int SetDSFillUpMode( int mode );
	int SetTexFillUpMode( int mode );

	int SetTempFogEnable( int srcenable );
	int RestoreTempFogEnable();

	int SetPointListForShadow( CVec3List* plist );

	int GlobalToLocal( D3DXVECTOR3 srcv, D3DXVECTOR3* dstv );
	int LocalToGlobal( D3DXVECTOR3 srcv, D3DXVECTOR3* dstv );

	int SetShadowInterFlag( int partno, int srcflag );
	int SetMaterialAlphaTest( int matno, int alphatest0, int alphaval0, int alphatest1, int alphaval1 );

	int GetMaterialNum( int* numptr );
	int GetMaterialInfo( MATERIALINFO* infoptr, int arrayleng, int* getnumptr );
	int GetToon1MaterialNum( int partno, int* numptr );
	int GetToon1MaterialInfo( int partno, TOON1MATERIALINFO* infoptr, int arrayleng, int* getnumptr );
	int SetJointInitialPos( int jointno, D3DXVECTOR3 pos, int calcflag, int excludeflag );

	int CreateSkinMat( int partno );
	int SetSymInfElem( int partno, int vertno, int symaxis, float symdist );
	int UpdateSymInfElem( int excludeflag );
	int CalcMLMotion();
	int ChangeMoaIdling( int motid );

	int SetMotionBlur( int mode, int blurtime );
	int SetMotionBlurPart( int mode, int blurtime, int* partptr, int num );
	int SetBeforeBlur();
	int InitBeforeBlur();
	int RenderMotionBlur( LPDIRECT3DDEVICE9 pd3dDevice );
	int SetMotionBlurMinAlpha( int partno, float minalpha );
	int SetMotionBlurMaxAlpha( int partno, float maxalpha );

	int RenderGlow( LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, D3DXMATRIX matView, D3DXVECTOR3 camerapos );
	int SetMaterialGlowParams( int matno, int glowmode, int setcolflag, E3DCOLOR4F* colptr );
	int GetMaterialGlowParams( int matno, int* glowmodeptr, E3DCOLOR4F* colptr );
	int SetMaterialTransparent( int matno, int transparent );
	int GetMaterialTransparent( int matno, int* transptr );

	int CreateBone( LPDIRECT3DDEVICE9 pdev, HWND appwnd, char* nameptr, int parentid, int* seriptr );
	int DestroyBone( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int delno );
	
	int GetMorphBaseNum( int* basenumptr );
	int GetMorphBaseInfo( E3DMORPHBASE* baseinfoptr, int arrayleng );
	int GetMorphTargetInfo( int baseid, E3DMORPHTARGET* targetinfoptr, int arrayleng );
	int GetMorphKeyNum( int motid, int boneid, int baseid, int targetid, int* keynumptr );
	int GetMorphKey( int motid, int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr, int arrayleng );
	int GetCurMorphInfo( int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr );
	int CreateMorph( LPDIRECT3DDEVICE9 pdev, HWND apphwnd, int baseid, int boneid );
	int AddMorphTarget( int baseid, int targetid );
	int SetMorphKey( int motid, int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr );
	int DestroyMorphKey( int motid, int boneid, int frameno, int baseid );

	//int GetToon1MaterialNum( int partno, int* matnumptr );
	int GetToon1Material( int partno, E3DTOON1MATERIAL* toon1ptr, int arrayleng );
	int SetToon1Name( int partno, char* oldname, char* newname );
	int SetToon1Diffuse( int partno, char* matname, RDBColor3f diffuse );
	int SetToon1Specular( int partno, char* matname, RDBColor3f specular );
	int SetToon1Ambient( int partno, char* matname, RDBColor3f ambient );
	int SetToon1NL( int partno, char* matname, float darknl, float brightnl );
	int SetToon1Edge0( int partno, char* matname, RDBColor3f col, int validflag, int invflag, float width );


	int LoadGPFile( char* fname, int* animnoptr );
	int DestroyGPAnim( int animno );
	int SaveGPFile( char* fname, int animno );
	int SetGPAnimNo( int animno );
	int GetGPAnimNo( int* animnoptr );
	int SetGPFrameNo( int animno, int frameno, int isfirst );
	int GetGPFrameNo( int* animnoptr, int* framenoptr );
	int SetNewGP();
	int SetNextGPAnimFarmeNo( int animno, int nextanimno, int nextframeno, int befframeno );
	int GetNextGPAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr );
	int CreateNewGPAnim( char* aname, int animtype, int framenum, int* animnoptr );
	int SetGPFrameNum( int animno, int newtotal );
	int GetGPFrameNum( int animno, int* framenumptr );
	int SetGPAnimName( int animno, char* aname );
	int GetGPAnimName( int animno, char* dstname, int arrayleng );
	int SetGPAnimType( int animno, int type );
	int GetGPAnimType( int animno, int* typeptr );
	int SetGPKey( int animno, int frameno, E3DGPSTATE gpstate );
	int GetGPKey( int animno, int frameno, E3DGPSTATE* gpstateptr, int* existptr );
	int DeleteGPKey( int animno, int frameno );
	int GetGPKeyframeNoRange( int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr );
	int CalcGPFullFrame( int animno );
	int SetGPGroundHSID( int animno, int ghsid );
	int GetGPGroundHSID( int animno, int* ghsidptr );

	int LoadMOEFile( char* filename, int* animnoptr );
	int DestroyMOEAnim( int animno );
	int SaveMOEFile( char* filename, int animno );
	int SetMOEAnimNo( int animno );
	int GetMOEAnimNo( int* animnoptr );
	int SetMOEFrameNo( int animno, int frameno, int isfirst );
	int GetMOEFrameNo( int* animnoptr, int* framenoptr );
	int SetNewMOE();
	int SetNextMOEAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno );
	int GetNextMOEAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr );
	int CreateNewMOEAnim( char* animname, int animtype, int framenum, int* animnoptr );
	int SetMOEFrameNum( int animno, int newtotal );
	int GetMOEFrameNum( int animno, int* framenumptr );
	int SetMOEAnimName( int animno, char* srcname );
	int GetMOEAnimName( int animno, char* dstname, int arrayleng );
	int SetMOEAnimType( int animno, int type );
	int GetMOEAnimType( int animno, int* typeptr );
	int GetMOEKeyframeNoRange( int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr );
	int GetMOEElemNum( int animno, int frameno, int* moeenumptr, int* existkeyptr );
	int GetMOEElem( int animno, int frameno, int moeeindex, MOEELEM* moeeptr );
	int AddMOEElem( int animno, int frameno, MOEELEM moee );
	int DeleteMOEElem( int animno, int frameno, int moeeindex );
	int SetMOEKeyIdling( int animno, int frameno, int motid );
	int GetMOEKeyIdling( int animno, int frameno, int* motidptr );
	int DeleteMOEKey( int animno, int frameno );
	int CalcMOEFullFrame( int animno );

	int CreateAndSaveTempMaterial();
	int RestoreAndDestroyTempMaterial();
	int SetMaterialFromInfElem( int jointno, int partno, int vertno, E3DCOLOR3UC* infcolarray );

	int GlobalToLocalParBone( int motid, int frameno, int boneno, int flag, 
		CQuaternion2* srcqptr, CQuaternion2* dstqptr, D3DXVECTOR3* srcvec, D3DXVECTOR3* dstvec );
	int LocalToGlobalParBone( int motid, int frameno, int boneno, int flag, 
		CQuaternion2* srcqptr, CQuaternion2* dstqptr, D3DXVECTOR3* srcvec, D3DXVECTOR3* dstvec );
	
	int GetInfElemSymParams( int partno, int vertno, int* symtypeptr, float* distptr );

	int DestroyMotion( int motid, IDCHANGE* idchgptr, int idnum );
	int GetIKTargetPos( int boneno, D3DXVECTOR3* posptr );
	int GetIKTargetState( int boneno, int* stateptr );
	int GetIKTargetCnt( int boneno, int* cntptr );
	int GetIKTargetLevel( int boneno, int* levelptr );
	int GetIKLevel( int boneno, int* levelptr );
	int SetIKTargetPos( int boneno, D3DXVECTOR3 srcpos );
	int SetIKTargetState( int boneno, int srcstate );
	int SetIKTargetCnt( int boneno, int srccnt );
	int SetIKTargetLevel( int boneno, int srclevel );
	int SetIKLevel( int boneno, int srclevel );
	int GetEulerLimit( int boneno, EULERLIMIT* elptr );
	int SetEulerLimit( int boneno, EULERLIMIT srcel );
	int CalcEuler( int motid, int zakind );
	int GetBoneEuler( int boneno, int motid, int frameno, D3DXVECTOR3* eulptr );
	int GetZa4Elem( int boneno, ZA4ELEM* z4eptr );
	int SetZa4Elem( int boneno, ZA4ELEM srcz4e );
	int CalcZa4LocalQ( int motid, int frameno );
	int GetZa4LocalQ( int boneno, E3DQ* dstq );
	int GetZa3IniQ( int boneno, E3DQ* dstq );
	int GetBoneAxisQ( int boneno, E3DQ* dstq );
	int SetBoneAxisQ( int boneno, int motid, int frameno, int zakind );

	int GetBoneTra( int boneno, int motid, int frameno, D3DXVECTOR3* traptr, int* existptr );
	int GetBoneScale( int boneno, int motid, int frameno, D3DXVECTOR3* scaleptr, int* existptr );
	int SetBoneTra( int boneno, int motid, int frameno, D3DXVECTOR3 srctra );
	int SetBoneScale( int boneno, int motid, int frameno, D3DXVECTOR3 srcscale );
	int GetBoneMatrix( int boneno, int motid, int frameno, int kind, D3DXMATRIX* matptr );
	int CreateMotionPoint( int boneno, int motid, int frameno );
	int GetIKSkip( int boneno, int* flagptr );
	int SetIKSkip( int boneno, int flag );

	int CreateTexKey( int boneno, int motid, int frameno );
	int DelTexKey( int boneno, int motid, int frameno );
	int AddTexChange( int boneno, int motid, int frameno, TEXCHANGE srctc );
	int DelTexChange( int boneno, int motid, int frameno, int materialno );
	int GetTexChangeNum( int boneno, int motid, int frameno, int* tcnumptr );
	int GetTexChange( int boneno, int motid, int frameno, int tcindex, TEXCHANGE* dsttc );
	int GetTexAnimKeyframeNoRange( int boneno, int motid, 
			int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );

	int CreateDSKey( int boneno, int motid, int frameno );
	int DelDSKey( int boneno, int motid, int frameno );
	int AddDSChange( int boneno, int motid, int frameno, DSCHANGE srcdsc );
	int DelDSChange( int boneno, int motid, int frameno, int switchno );
	int GetDSChangeNum( int boneno, int motid, int frameno, int* tcnumptr );
	int GetDSChange( int boneno, int motid, int frameno, int dscindex, DSCHANGE* dstdsc );
	int GetDSAnimKeyframeNoRange( int boneno, int motid, 
			int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );

	int GetMaterialTexParams( int materialno, MATERIALTEX* dstparams );
	int SetMaterialTexParams( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int materialno, MATERIALTEX* srcparams );
	int GetMaterialTexParams1DIM( int materialno, MATERIALTEX1DIM* dstparams );
	int SetMaterialTexParams1DIM( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int materialno, MATERIALTEX1DIM* srcparams );

	int GetMaterialAlphaTest( int materialno, int* test0ptr, int* val0ptr, int* test1ptr, int* val1ptr );

	int GetPartPropName( int partno, char* nameptr, int arrayleng );
	int SetPartPropName( int partno, char* srcname );
	int GetPartPropDS( int partno, int* dsptr );
	int SetPartPropDS( int partno, int srcds );
	int GetPartPropNotUse( int partno, int* flagptr );
	int SetPartPropNotUse( int partno, int srcflag );

	int CloneDispObj( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int orgseri, char* tmpname, int* newseriptr );
	int GetJointNotSel( int boneno, int* notselptr );
	int SetJointNotSel( int boneno, int notsel );

	int DestroyMorph( int partno );
	int DestroyDispObj( int partno );
	int AllClearInfElem( int partno, int vertno );
	int SetGPOffset( D3DXVECTOR3 tra, CQuaternion2* srcqptr );
	int RenderZandN( int znflag, LPDIRECT3DDEVICE9 pdev, int withalpha, D3DXMATRIX matView, D3DXVECTOR3 camerapos );
	int RenderZandN2( int znflag, LPDIRECT3DDEVICE9 pdev, int withalpha, D3DXMATRIX matView, D3DXVECTOR3 camerapos );


	int CreateInfScope( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int boneno, int dispno, int* isindexptr );
	int DestroyInfScope( int isindex );
	int ExistInfScope( int boneno, int dispno, int* isindexptr );
	int GetInfScopeNum( int* numptr );
	int GetInfScopeInfo( ISINFO* isiptr, int arrayleng, int* getnumptr );
	int SetInfScopeVert( ISVERT srcisv );
	int GetInfScopeVert( ISVERT* isvptr );
	int GetInfScopeVertNum( int isindex, int* numptr );
	int PickInfScopeVert( D3DXMATRIX matView, POINT scpos, POINT range, int isindex, ISVERT* isvptr, int arrayleng, int* getnumptr );
	int GetInfScopeAxis( int isindex, D3DXVECTOR3* dstvec3x3ptr );
	int SetInfScopeColor( int isindex, E3DCOLOR4UC srccol );
	int GetInfScopeColor( int isindex, E3DCOLOR4UC* dstcolptr );
	int SetInfScopeDispFlag( int isindex, int srcflag, int exclusiveflag );
	int GetInfScopeDispFlag( int isindex, int* dstflagptr );
	int ImportMQOAsInfScope( LPDIRECT3DDEVICE9 pdev, HWND appwnd, char* filename, float mult, int boneno, int dispno, int* isindexptr );

	int GetMOELocalMotionFrameNo( int moeanimno, int moeframeno, int* lanimno, int* lframeno );
	int GetMOELocalNextMotionFrameNo( int moeanimno, int moeframeno, int* lnanimno, int* lnframeno );

	int SetMOEKeyGoOnFlag( int animno, int frameno, int flag );
	int GetMOEKeyGoOnFlag( int animno, int frameno, int* flagptr );

	int SetMOATrunkNotComID( int motid, int arrayleng, int* srcid );

	int GetMOELocalMotionFrameNoML( int moeanimno, int moeframeno, int boneid, int* lanimno, int* lframeno );
	int GetMOELocalNextMotionFrameNoML( int moeanimno, int moeframeno, int boneid, int* lnanimno, int* lnframeno );

	int SetDefaultGP( E3DGPSTATE srcgps );
	int SetKinectUpVec( int boneno, D3DXVECTOR3 upvec );

	int GetInViewFlag( int partno, int* flagptr );
	int GetMotionJumpFrame( int motid, int* frameptr );
	int SetMotionJumpFrame( int motid, int srcframe );
	int CopyTexAnimFrame( int srcmotid, int srcframe, int dstmotid, int dstframe );
	int DestroyTexAnimFrame( int motid, int frameno );
	int GetMaterialExtTextureNum( int matno, int* numptr );
	int GetMaterialExtTexture( char* nameptr, int matno, int extno );
	int SetExtTextureToMaterial( int matno, int extno, int texid );
	int LoadMQOFileAsGroundFromBuf( HWND hwnd, char* bufptr, int bufsize, int adjustuvflag, float mult );
	int ChgMaterial1stTexFromBuf( LPDIRECT3DDEVICE9 pdev, int matno, char* buf, int bufsize, int* texidptr );
	int ChgMaterialExtTexFromBuf( LPDIRECT3DDEVICE9 pdev, int matno, int extno, char* buf, int bufsize, int* texidptr );
	int ChgMaterial1stTexFromID( LPDIRECT3DDEVICE9 pdev, int matno, int texid );
	int ChgMaterialExtTexFromID( LPDIRECT3DDEVICE9 pdev, int matno, int extno, int texid );

	int DestroyDispPart( int partno );

private:
	int DestroyHandler();

	int MoveBone( D3DXVECTOR3 camerapos, LPDIRECT3DDEVICE9 pdev, int mvno, D3DXVECTOR3 targetobj, int* lastparent, D3DXMATRIX matView, int motid, int frameno, int iklevel, int axiskind, D3DXVECTOR3 axisvec, int calclevel );
	int SetMotionPointIK( int mvboneno, CQuaternion multq, int motid, int frameno );
	int SetMotionPointIK( int mvboneno, float srcmvx, float srcmvy, float srcmvz, int motcookie, int frameno );

	int GetQ( int boneno, int motid, int frameno, CQuaternion* dstq );
	void SetBoneArrayReq( CShdElem* selem, int* setnum, int broflag );
	int UpdateMatrixSelected( int srcseri, int motid, int frameno );
	//int CreateMotionPoints( int motcookie, int frameno );
	int GetShadow( D3DXVECTOR3* dstshadow, D3DXVECTOR3 planedir, D3DXVECTOR3 planep, D3DXVECTOR3 srcp, D3DXVECTOR3 srcdir );
	int DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec );

	int GetTopJointNo( int srcseri, int* topseri );


	//int CreateLackedMotionPoints( int motid );
	int CreateMotionPoints( int srcseri, CMotionCtrl* firstjoint, int motcookie, int frameno );

	int SetMLList( int* listptr, int* notlistptr );
	void SetMLNotListReq( int srcnot, int broflag );
	void SetMLListReq( int srclist, int broflag );

	int SetSymInfElemPart( int partno, int vertno, int symaxis, float symdist );
	int CopySymInf( int seldisp, int selvert, int symaxis, float symdist );
	int SetSymIE( int sympart, int symvert, int symaxis, int* syminfnumptr );
	int Conv2SymIE( INFELEM* srcIE, int infnum, int symaxis, INFELEM* dstIE );
	int GetSymJointNo( char* srcname, int srcno, int symkind );

	int Add2Tree( char* newname, int serino, int srctype, int depth, int hasmotion, int createflag );

	int CalcSplineParam( int motid, int allmkind, int calcseri );

public:
	static int s_alloccnt;

	int serialno;
	CTreeHandler2* m_thandler;
	CShdHandler* m_shandler;
	CMotHandler* m_mhandler;
//    D3DXMATRIX m_matWorld;
	
	CMCHandler* m_mch;

	CGPData m_gpd;
	float m_twist;

	DWORD m_clientWidth;
	DWORD m_clientHeight;

	D3DXVECTOR3 m_befpos;	

	CBillboardIO* m_bbio;

	CNPClearData m_npcd;

	int m_TLmode;

	int m_groundflag;
	int m_billboardflag;
	int m_extlineflag;
	D3DXMATRIX m_billmatW;
	D3DXMATRIX m_billmatW2;

	CExtLineIO* m_lineio;


//	char m_resdir[MAX_PATH];

	int m_bonearray[MAXBONENUM + 1];

	CRITICAL_SECTION m_crit_addmotion;

	int* m_mllist;
	int m_mlleng;
	int m_dsfillupmode;
	int m_texfillupmode;

	int m_syminfnum;
	INFELEM m_symIE[ INFNUMMAX ];

	CGPAnimHandler* m_gpah;
	CMOEAnimHandler* m_moeah;
};

#endif