// 以下の ifdef ブロックは DLL からのエクスポートを容易にするマクロを作成するための 
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された EASY3D_EXPORTS
// シンボルでコンパイルされます。このシンボルは、この DLL を使うプロジェクトで定義することはできません。
// ソースファイルがこのファイルを含んでいる他のプロジェクトは、 
// EASY3D_API 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef EASY3D_EXPORTS
#define EASY3D_API __declspec(dllexport)
#else
#define EASY3D_API __declspec(dllimport)
#endif


#include <d3dx9.h>
#include <usercoef.h>


// このクラスは easy3d.dll からエクスポートされました。
//class EASY3D_API Ceasy3d {
//public:
//	Ceasy3d(void);
//	// TODO: メソッドをここに追加してください。
//};

//extern EASY3D_API int neasy3d;

//EASY3D_API int fneasy3d(void);


EASY3D_API int E3DGetVersion( int* verptr );
EASY3D_API int E3DInit( HINSTANCE srcinst, HWND srchwnd, int fullscflag, int bits, int multisamplenum, int gpuflag, int enablereverb, 
	int initcomflag, int index32bit, int* scidptr );
EASY3D_API int E3DBye();
EASY3D_API int E3DCreateSwapChain( HWND srchwnd, int* scidptr );
EASY3D_API int E3DDestroySwapChain( int scid );
EASY3D_API int E3DSigLoad( const char* tempname, int adjustuvflag, float mult, int* hsidptr );
EASY3D_API int E3DRender( int scid, int hsid, int withalpha, int lightflag, int transskip, int lastparent );
EASY3D_API int E3DPresent( int scid );
EASY3D_API int E3DBeginScene( int scid, int skipflag, int zscid = -1 );
EASY3D_API int E3DEndScene();
EASY3D_API int E3DCreateBG( int scid, const char* tempname1, const char* tempname2, 
	float uanim, float vanim, int isround, float fogdist );
EASY3D_API int E3DSetBGU( int scid, float startu, float endu );
EASY3D_API int E3DSetBGV( int scid, float startv, float endv );

EASY3D_API int E3DDestroyBG( int scid );
EASY3D_API int E3DAddMotion( int hsid, const char* tempname, float mvmult, int* motidptr, int* maxframeptr );
EASY3D_API int E3DSetMotionKind( int hsid, int motid );
EASY3D_API int E3DGetMotionKind( int hsid, int* motidptr );
EASY3D_API int E3DSetNewPose( int hsid, int* frameptr);
EASY3D_API int E3DSetMotionStep( int hsid, int motid, int step );
EASY3D_API int E3DSetPos( int hsid, D3DXVECTOR3 pos );
EASY3D_API int E3DGetPos( int hsid, D3DXVECTOR3* posptr );
EASY3D_API int E3DSetDir( int hsid, D3DXVECTOR3 dir );
EASY3D_API int E3DRotateInit( int hsid );

EASY3D_API int E3DRotateX( int hsid, float dirx );
EASY3D_API int E3DRotateY( int hsid, float diry );
EASY3D_API int E3DRotateZ( int hsid, float dirz );
EASY3D_API int E3DTwist( int hsid, float step );
EASY3D_API int E3DPosForward( int hsid, float step );
EASY3D_API int E3DCloseTo( int chghsid, int srchsid, float movestep );
EASY3D_API int E3DDirToTheOtherXZ( int chghsid, int srchsid );
EASY3D_API int E3DDirToTheOther( int chghsid, int srchsid );
EASY3D_API int E3DSeparateFrom( int chghsid, int srchsid, float dist );
EASY3D_API int E3DGetCameraPos( D3DXVECTOR3* posptr );

EASY3D_API int E3DSetCameraPos( D3DXVECTOR3 pos );
EASY3D_API int E3DGetCameraDeg( float* degxzptr, float* degyptr );
EASY3D_API int E3DSetCameraDeg( float degxz, float degy );
EASY3D_API int E3DSetCameraTarget( D3DXVECTOR3 target, D3DXVECTOR3 upvec);
EASY3D_API int E3DChkInView( int scid, int hsid, int* statptr );
EASY3D_API int E3DEnableDbgFile();
EASY3D_API int E3DSetProjection( float nearclip, float farclip, float fov );
EASY3D_API int E3DGetKeyboardState( int* stateptr );
EASY3D_API int E3DGetKeyboardCnt( int* cntptr );
EASY3D_API int E3DResetKeyboardCnt();
EASY3D_API int E3DChkConflictAABB( int srchsid, int chkhsid, int* confptr, int* inviewptr );

EASY3D_API int E3DChkConflictOBB( int hsid1, int partno1, int hsid2, int partno2, int* confptr, int* inviewptr );
EASY3D_API int E3DChkConflictOBBArray( int srchsid, int* srcpart, int srcpartnum, int chkhsid, int* chkpart, int chkpartnum, 
	int* confptr, int* inviewptr );
EASY3D_API int E3DChkConfBillboard( int hsid, float confrate, int* confptr, int* bbidptr, int arrayleng, int* confnumptr );
EASY3D_API int E3DChkConfBillboardBS( D3DXVECTOR3 center, float srcr, float confrate, 
	int* confptr, int* bbidptr, int arrayleng, int* confnumptr );
EASY3D_API int E3DChkConflictBS( int srchsid, int chkhsid, int conflevel, int* confptr );
EASY3D_API int E3DChkConflictBSPart( int srchsid, int srcpartno, int chkhsid, int chkpartno, int* confptr );
EASY3D_API int E3DChkConflictBSPartArray( int srchsid, int* srcpart, int srcpartnum, int chkhsid, int* chkpart, int chkpartnum, int* confptr );
EASY3D_API int E3DChkConfGround( int charahsid, int groundhsid, int mode, float diffmaxy, float mapminy, 
	int* resultptr, D3DXVECTOR3* adjust, D3DXVECTOR3* n );
EASY3D_API int E3DChkConfGroundVec( D3DXVECTOR3 befv, D3DXVECTOR3 newv, int groundhsid, int mode, float diffmaxy, float mapminy,
	int* resultptr, D3DXVECTOR3* adjust, D3DXVECTOR3* n );
EASY3D_API int E3DChkConfGroundPart( int charahsid, int groundhsid, int groundpart, int mode, float diffmaxy, float mapminy,
	int* resultptr, D3DXVECTOR3* adjust, D3DXVECTOR3* n );

EASY3D_API int E3DChkConfGroundVecPart( D3DXVECTOR3 befv, D3DXVECTOR3 newv, int groundhsid, int groundpart, int mode, float diffmaxy, float mapminy,
	int* resultptr, D3DXVECTOR3* adjust, D3DXVECTOR3* n );
EASY3D_API int E3DChkConfWall( int charahsid, int groundhsid, float dist, int* resultptr, D3DXVECTOR3* adjustptr, D3DXVECTOR3* nptr );
EASY3D_API int E3DChkConfWallVec( D3DXVECTOR3 befpos, D3DXVECTOR3 newpos, int groundhsid, float dist, 
	int* resultptr, D3DXVECTOR3* adjustptr, D3DXVECTOR3* nptr );
EASY3D_API int E3DChkConfWallPartArray( int charahsid, int groundhsid, int* partarray, int partnum, float dist,
	int* resultptr, D3DXVECTOR3* adjustptr, D3DXVECTOR3* nptr );
EASY3D_API int E3DChkConfLineAndFace( D3DXVECTOR3 srcpos1, D3DXVECTOR3 srcpos2, int hsid, int needtrans,
	int* partptr, int* faceptr, D3DXVECTOR3* posptr, D3DXVECTOR3* nptr, int* revfaceptr );
EASY3D_API int E3DChkConfParticle( int ptclid, int hsid, float rate, int* confptr );
EASY3D_API int E3DDrawText( D3DXVECTOR2 scpos, float scale, E3DCOLOR4UC col, const char* strptr );
EASY3D_API int E3DDrawBigText( D3DXVECTOR2 scpos, float scale, E3DCOLOR4UC col, const char* strptr );
EASY3D_API int E3DCreateFont( int height, int width, int weight, int bItalic, int bUnderline, int bStrikeout, const char* tempname, int* idptr );
EASY3D_API int E3DDrawTextByFontID( int scid, int fontid, POINT scpos, const char* tempname, E3DCOLOR4UC col, int bmode = -1 );

EASY3D_API int E3DDrawTextByFontIDWithCnt( int scid, int fontid, POINT scpos, const char* tempname, E3DCOLOR4UC col, int eachcnt, int curcnt );
EASY3D_API int E3DDrawTextByFontIDWithCntEach( int scid, int fontid, POINT scpos, const char* tempname, E3DCOLOR4UC col,
	int* cntarray, int arrayleng, int curcnt );
EASY3D_API int E3DGetCharacterNum( const char* tempname, int* numptr );
EASY3D_API int E3DDestroyFont( int fontid );
EASY3D_API int E3DGetFPS( int* fpsptr );
EASY3D_API int E3DWaitbyFPS( int srcfps, int* retfpsptr );
EASY3D_API int E3DCreateLight( int* lidptr );
EASY3D_API int E3DSetDirectionalLight( int lid, D3DXVECTOR3 dir, E3DCOLOR4UC col );
EASY3D_API int E3DSetPointLight( int lid, D3DXVECTOR3 pos, float dist, E3DCOLOR4UC col );
EASY3D_API int E3DDestroyLight( int lid );

EASY3D_API int E3DClearZ();
EASY3D_API int E3DDestroyHandlerSet( int hsid );
EASY3D_API int E3DSetMotionFrameNo( int hsid, int motid, int srcframeno );
EASY3D_API int E3DCreateSprite( const char* tempname, int transparentflag, E3DCOLOR4UC* tpcolptr, int* spriteidptr );
EASY3D_API int E3DBeginSprite();
EASY3D_API int E3DEndSprite();
EASY3D_API int E3DRenderSprite( int spid, float scalex, float scaley, D3DXVECTOR3 tra );
EASY3D_API int E3DGetSpriteSize( int spid, int* widthptr, int* heightptr );
EASY3D_API int E3DSetSpriteRotation( int spid, D3DXVECTOR2 center, float rotdeg );
EASY3D_API int E3DSetSpriteARGB( int spid, E3DCOLOR4UC col );

EASY3D_API int E3DSetSpriteUV( int spid, float startu, float endu, float startv, float endv );
EASY3D_API int E3DDestroySprite( int spid );
EASY3D_API int E3DCreateProgressBar();
EASY3D_API int E3DSetProgressBar( int newpos );
EASY3D_API int E3DDestroyProgressBar();
EASY3D_API int E3DLoadGroundBMP( const char* tempname1, const char* tempname2, const char* tempname3, const char* tempname4, 
	float maxx, float maxz, int divx, int divz, float maxheight, int* hsidptr );
EASY3D_API int E3DLoadMQOFileAsGround( const char* tempname1, float mult, int adjustuvflag, int bonetype, int* hsidptr );
EASY3D_API int E3DLoadSigFileAsGround( const char* tempname, float mult, int adjustuvflag, int* hsidptr );
EASY3D_API int E3DCameraPosForward( float step );
EASY3D_API int E3DSetBeforePos( int hsid );

EASY3D_API int E3DGetPartNoByName( int hsid, const char* tempname, int* partnoptr );
EASY3D_API int E3DGetVertNumOfPart( int hsid, int partno, int* vertnumptr );
EASY3D_API int E3DGetVertPos( int hsid, int partno, int vertno, int kind, D3DXVECTOR3* posptr );
EASY3D_API int E3DSetVertPos( int hsid, int partno, int vertno, D3DXVECTOR3 pos );
EASY3D_API int E3DSetVertPosBatch( int hsid, int partno, int* vertnoarray, int vertnum, D3DXVECTOR3* posptr );
EASY3D_API int E3DSetPosOnGround( int hsid, int groundid, float mapmaxy, float mapminy, float posx, float posz );
EASY3D_API int E3DSetPosOnGroundPart( int hsid, int groundid, int groundpart, float mapmaxy, float mapminy, float posx, float posz );
EASY3D_API int E3DCreateBillboard( const char* tempname1, float width, float height, int transparentflag, int dirmode, int orgflag, int* bbidptr );
EASY3D_API int E3DRenderBillboard( int scid, int transskip );
EASY3D_API int E3DSetBillboardPos( int bbid, D3DXVECTOR3 pos );

EASY3D_API int E3DSetBillboardOnGround( int bbid, int groundid, float mapmaxy, float mapminy, float posx, float posz );
EASY3D_API int E3DGetBillboardInfo( BBINFO* bbinfo );
EASY3D_API int E3DGetNearBillboard( D3DXVECTOR3 pos, float maxdist, int* bbidptr );
EASY3D_API int E3DRotateBillboard( int bbid, float rotdeg, int rotkind );
EASY3D_API int E3DSetBillboardUV( int bbid, int unum, int vnum, int texno, int revuflag );
EASY3D_API int E3DDestroyBillboard( int bbid );
EASY3D_API int E3DDestroyAllBillboard();
EASY3D_API int E3DSaveMQOFile( int hsid, const char* tempname1 );
EASY3D_API int E3DGetInvisibleFlag( int hsid, int partno, int* flagptr );
EASY3D_API int E3DSetInvisibleFlag( int hsid, int partno, int srcflag );

EASY3D_API int E3DSetMovableArea( const char* tempname1, float maxx, float maxz, int divx, int divz, float wallheight, int* hsidptr );
EASY3D_API int E3DVec3Normalize( D3DXVECTOR3 srcvec, D3DXVECTOR3* dstvec );
EASY3D_API int E3DVec2CCW( D3DXVECTOR2 vec1, D3DXVECTOR2 vec2, int* ccwptr );
EASY3D_API int E3DVec3RotateY( D3DXVECTOR3 srcvec, float deg, D3DXVECTOR3* dstvec );
EASY3D_API int E3DLoadMQOFileAsMovableArea( const char* tempname1, float mult, int* hsidptr );
EASY3D_API int E3DLoadSound( const char* tempname1, int use3dflag, int reverbflag, int bufnum, int* idptr );
EASY3D_API int E3DPlaySound( int soundid, int startsample, int openo, int numloops );
EASY3D_API int E3DStopSound( int soundid );
EASY3D_API int E3DSetSoundLoop( int soundid, int loopflag );
EASY3D_API int E3DSetSoundVolume( float volume, int soundid );

EASY3D_API int E3DSetSoundTempo( float tempo );
EASY3D_API int E3DDestroySound( int soundid );
EASY3D_API int E3DGetSoundVolume( int soundid, float* volptr );
EASY3D_API int E3DSet3DSoundListener( float doppler, float rolloff );
EASY3D_API int E3DSet3DSoundListenerMovement( int hsid );
EASY3D_API int E3DSet3DSoundDistance( int soundid, float min, float max );
EASY3D_API int E3DSet3DSoundMovement( int soundid, D3DXVECTOR3 pos, D3DXVECTOR3 vel );
EASY3D_API int E3DCreateNaviLine( int* nlidptr );
EASY3D_API int E3DDestroyNaviLine( int nlid );
EASY3D_API int E3DAddNaviPoint( int nlid, int befnpid, int* npidptr );

EASY3D_API int E3DRemoveNaviPoint( int nlid, int npid );
EASY3D_API int E3DGetNaviPointPos( int nlid, int npid, D3DXVECTOR3* posptr );
EASY3D_API int E3DSetNaviPointPos( int nlid, int npid, D3DXVECTOR3 pos );
EASY3D_API int E3DGetNaviPointOwnerID( int nlid, int npid, int* idptr );
EASY3D_API int E3DSetNaviPointOwnerID( int nlid, int npid, int oid );
EASY3D_API int E3DGetNextNaviPoint( int nlid, int npid, int* nextidptr );
EASY3D_API int E3DGetPrevNaviPoint( int nlid, int npid, int* previdptr );
EASY3D_API int E3DGetNearestNaviPoint( int nlid, D3DXVECTOR3 pos, int* nearidptr, int* previdptr, int* nextidptr );
EASY3D_API int E3DFillUpNaviLine( int nlid, int div, int flag );
EASY3D_API int E3DSetNaviLineOnGround( int nlid, int groundid, float mapmaxy, float mapminy );

EASY3D_API int E3DControlByNaviLine( int hsid, int nlid, int ctrlmode, int roundflag, int reverseflag,
	float maxdist, float posstep, float dirstep, D3DXVECTOR3* newposptr, D3DXQUATERNION* newqptr, int* tpidptr );
EASY3D_API int E3DSetDirQ( int hsid, D3DXQUATERNION q, D3DXQUATERNION* befqptr = 0 );
EASY3D_API int E3DGetDirQ( int hsid, D3DXQUATERNION* qptr );
EASY3D_API int E3DSetWallOnGround( int whsid, int ghsid, float mapmaxy, float mapminy, float wheight );
EASY3D_API int E3DCreateNaviPointClearFlag( int hsid, int nlid, int roundnum );
EASY3D_API int E3DDestroyNaviPointClearFlag( int hsid );
EASY3D_API int E3DInitNaviPointClearFlag( int hsid );
EASY3D_API int E3DSetNaviPointClearFlag( int hsid, D3DXVECTOR3 pos, float maxdist, int* npidptr, int* roundptr, float* distptr );
EASY3D_API int E3DGetOrder( int* hsidarray, int arrayleng, int* orderarray, int* cleararray );
EASY3D_API int E3DSetValidFlag( int hsid, int partno, int flag );

EASY3D_API int E3DSetBillboardDiffuse( int bbid, E3DCOLOR4UC col, int setflag, int vertno );
EASY3D_API int E3DSetBillboardSpecular( int bbid, E3DCOLOR4UC col, int setflag, int vertno );
EASY3D_API int E3DSetBillboardAmbient( int bbid, E3DCOLOR4UC col, int setflag, int vertno );
EASY3D_API int E3DSetBillboardEmissive( int bbid, E3DCOLOR4UC col, int setflag, int vertno );
EASY3D_API int E3DSetBillboardSpecularPower( int bbid, float power, int setflag, int vertno );
EASY3D_API int E3DSetBillboardAlpha( int bbid, float alphaval);
EASY3D_API int E3DGetBillboardDiffuse( int bbid, int vertno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetBillboardAmbient( int bbid, int vertno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetBillboardSpecular( int bbid, int vertno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetBillboardEmissive( int bbid, int vertno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetBillboardAlpha( int bbid, int vertno, float* alphaptr );

EASY3D_API int E3DGetBillboardSpecularPower( int bbid, int vertno, float* powerptr );
EASY3D_API int E3DSetBlendingMode( int hsid, int partno, int bmode );
EASY3D_API int E3DSetRenderState( int hsid, int partno, int statetype, int value );
EASY3D_API int E3DSetScale( int hsid, int partno, D3DXVECTOR3 scale, int centerflag );
EASY3D_API int E3DGetScreenPos3F( int scid, int hsid, int partno, int vertno, D3DXVECTOR3* scposptr );
EASY3D_API int E3DGetScreenPos3FVec( int scid, D3DXVECTOR3 pos3d, D3DXVECTOR3* scposptr );
EASY3D_API int E3DConvScreenTo3D( int scid, D3DXVECTOR3 scpos, D3DXVECTOR3* pos3dptr );
EASY3D_API int E3DCreateQ( int* qidptr );
EASY3D_API int E3DDestroyQ( int qid );
EASY3D_API int E3DInitQ( int qid );

EASY3D_API int E3DSetQAxisAndDeg( int qid, D3DXVECTOR3 axis, float deg );
EASY3D_API int E3DGetQAxisAndDeg( int qid, D3DXVECTOR3* axisptr, float* degptr );
EASY3D_API int E3DRotateQX( int qid, float degx );
EASY3D_API int E3DRotateQY( int qid, float degy );
EASY3D_API int E3DRotateQZ( int qid, float degz );
EASY3D_API int E3DMultQ( int resqid, int befqid, int aftqid );
EASY3D_API int E3DNormalizeQ( int qid );
EASY3D_API int E3DCopyQ( int dstqid, int srcqid );
EASY3D_API int E3DQtoEuler( int qid, D3DXVECTOR3* eulptr, D3DXVECTOR3 befeul );
EASY3D_API int E3DGetBoneNoByName( int hsid, const char* tempname, int* noptr );

EASY3D_API int E3DGetNextMP( int hsid, int motid, int boneno, int prevmpid, int* mpidptr );
EASY3D_API int E3DGetMPInfo( int hsid, int motid, int boneno, MPINFO2* mpiptr );
EASY3D_API int E3DSetMPInfo( int hsid, int motid, int boneno, MPINFO2* mpiptr, int* flagptr );
EASY3D_API int E3DExistMP( int hsid, int motid, int boneno, int frameno, int* mpidptr );
EASY3D_API int E3DGetMotionFrameLength( int hsid, int motid, int* lengptr );
EASY3D_API int E3DSetMotionFrameLength( int hsid, int motid, int frameleng );
EASY3D_API int E3DAddMP( int hsid, int motid, int boneno, MPINFO2* mpiptr );
EASY3D_API int E3DDeleteMP( int hsid, int motid, int boneno, int mpid );
EASY3D_API int E3DFillUpMotion( int hsid, int motid, int boneno, int startframe, int endframe );
EASY3D_API int E3DCopyMotionFrame( int hsid, int srcmotid, int srcframe, int dstmotid, int dstframe );

EASY3D_API int E3DGetDirQ2( int hsid, int dstqid );
EASY3D_API int E3DSetDirQ2( int hsid, int srcqid );
EASY3D_API int E3DLookAtQ( int dstqid, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int upflag, int divnum );
EASY3D_API int E3DMultQVec( int qid, D3DXVECTOR3 befvec, D3DXVECTOR3* aftvecptr);
EASY3D_API int E3DTwistQ( int qid, float twistdeg, D3DXVECTOR3 basevec );
EASY3D_API int E3DInitTwistQ( int qid, D3DXVECTOR3 basevec );
EASY3D_API int E3DGetTwistQ( int qid, float* twistptr );
EASY3D_API int E3DRotateQLocalX( int qid, float deg );
EASY3D_API int E3DRotateQLocalY( int qid, float deg );
EASY3D_API int E3DRotateQLocalZ( int qid, float deg );
//200

EASY3D_API int E3DGetBonePos( int hsid, int boneno, int poskind, int motid, int frameno, D3DXVECTOR3* posptr );
EASY3D_API int E3DCreateLine( D3DXVECTOR3* posptr, int pointnum, int maxpointnum, int linekind, int* lidptr );
EASY3D_API int E3DDestroyLine( int lid );
EASY3D_API int E3DSetLineColor( int lid, E3DCOLOR4UC col );
EASY3D_API int E3DAddPoint2Line( int lid, int previd, int* pidptr );
EASY3D_API int E3DDeletePointOfLine( int lid, int pid );
EASY3D_API int E3DSetPointPosOfLine( int lid, int pid, D3DXVECTOR3 pos );
EASY3D_API int E3DGetPointPosOfLine( int lid, int pid, D3DXVECTOR3* posptr );
EASY3D_API int E3DGetNextPointOfLine( int lid, int previd, int* nextidptr );
EASY3D_API int E3DGetPrevPointOfLine( int lid, int pid, int* previdptr );

EASY3D_API int E3DWriteDisplay2BMP( const char* bmpname, int scid );
EASY3D_API int E3DCreateAVIFile( int scid, const char* aviname, int datatype, int compkind, int framerate, int frameleng, int* idptr );
EASY3D_API int E3DWriteData2AVIFile( int scid, int aviid, int datatype );
EASY3D_API int E3DCompleteAVIFile( int scid, int aviid, int datatype );
EASY3D_API int E3DCameraLookAt( D3DXVECTOR3 tarvec, int upflag, int divnum );
EASY3D_API int E3DCameraOnNaviLine( int nlid, int mode, int roundflag, int reverseflag, D3DXVECTOR3 offset, float posstep, 
	int dirdivnum, int upflalg, int* tpidptr );
EASY3D_API int E3DCameraDirUp( float deg, int divnum );
EASY3D_API int E3DCameraDirDown( float deg, int divnum );
EASY3D_API int E3DCameraDirRight( float deg, int divnum );
EASY3D_API int E3DCameraDirLeft( float deg, int divnum );

EASY3D_API int E3DSlerpQ( int startqid, int endqid, float t, int resqid );
EASY3D_API int E3DSquadQ( int befqid, int startqid, int endqid, int aftqid, float t, int resqid );
EASY3D_API int E3DSplineVec( D3DXVECTOR3 pos0, D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, D3DXVECTOR3 pos3, float t, D3DXVECTOR3* resptr );
EASY3D_API int E3DDbgOut( char* lpFormat, ... );
EASY3D_API int E3DSaveQuaFile( int quatype, int hsid, int motid, const char* tempname1 );
EASY3D_API int E3DSaveSigFile( int hsid, const char* filename );
EASY3D_API int E3DPickVert( int scid, int hsid, POINT scpos, POINT range, int* partarray, int* vertarray, int arrayleng, int* getnumptr );
EASY3D_API int E3DGetOrgVertNo( int hsid, int partno, int vertno, int* orgnoarray, int arrayleng, int* getnumptr );
EASY3D_API int E3DChkIM2Status( int hsid, int* statusptr );
EASY3D_API int E3DLoadIM2File( int hsid, const char* filename );

EASY3D_API int E3DSaveIM2File( int hsid, const char* filename );
EASY3D_API int E3DGetJointNum( int hsid, int* numptr );
EASY3D_API int E3DGetJointInfo( int hsid, JOINTINFO* jiarray, int arrayleng, int* getnumptr );
EASY3D_API int E3DGetFirstJointNo( int hsid, int* jointnoptr );
EASY3D_API int E3DGetDispObjNum( int hsid, int* numptr );
EASY3D_API int E3DGetDispObjInfo( int hsid, DISPOBJINFO* doiarray, int arrayleng, int* getnumptr );
EASY3D_API int E3DEnableTexture( int hsid, int partno, int enableflag );
EASY3D_API int E3DJointAddToTree( int hsid, int parentjoint, int childjoint, int lastflag );
EASY3D_API int E3DJointRemoveFromParent( int hsid, int rmjoint );
EASY3D_API int E3DJointRemake( int hsid );

EASY3D_API int E3DSigImport( int hsid, const char* filename, int adjustuvflag, float mult, D3DXVECTOR3 offset, D3DXVECTOR3 rot );
EASY3D_API int E3DSigLoadFromBuf( const char* texdir, char* buf, int bufsize, int adjustuvflag, float mult, int* hsidptr );
EASY3D_API int E3DSigImportFromBuf( int hsid, const char* texdir, char* buf, int bufsize, int adjustuvflag, float mult, D3DXVECTOR3 offset, D3DXVECTOR3 rot );
EASY3D_API int E3DAddMotionFromBuf( int hsid, char* buf, int bufsize, float mvmult, int* motidptr, int* maxframeptr );
EASY3D_API int E3DCheckFullScreenParams( FULLSCPARAMS srcparams, FULLSCPARAMS* validparams, int flag = 1 );
EASY3D_API int E3DGetMaxMultiSampleNum( int bits, int iswindowmode, int* numptr );
EASY3D_API int E3DPickFace( int scid, int hsid, POINT scpos, float maxdist, int* partptr, int* faceptr, 
	D3DXVECTOR3* pos3dptr, D3DXVECTOR3* nptr, float* distptr );
EASY3D_API int E3DPickFacePartArray( int scid, int hsid, int* partarray, int partnum, POINT scpos, float maxdist, int* partptr, int* faceptr,
	D3DXVECTOR3* pos3dptr, D3DXVECTOR3* nptr, float* distptr );
EASY3D_API int E3DGetBBox( int hsid, int partno, int mode, D3DXVECTOR3* minptr, D3DXVECTOR3* maxptr );
EASY3D_API int E3DGetVertNoOfFace( int hsid, int partno, int faceno, TRIVNO* vnoptr );

EASY3D_API int E3DGetSamePosVert( int hsid, int partno, int vertno, int* samearray, int arrayleng, int* getnumptr );
EASY3D_API int E3DRtscStart();
EASY3D_API int E3DRtscStop( DWORD* timeptr );
EASY3D_API int E3DSaveSig2Buf( int hsid, char* buf, int bufsize, int* wsizeptr );
EASY3D_API int E3DSaveQua2Buf( int quatype, int hsid, int motid, char* buf, int bufsize, int* wsizeptr );
EASY3D_API int E3DCameraShiftLeft( float shift );
EASY3D_API int E3DCameraShiftRight( float shift );
EASY3D_API int E3DCameraShiftUp( float shift );
EASY3D_API int E3DCameraShiftDown( float shift );
EASY3D_API int E3DGetCameraQ( int dstqid );
EASY3D_API int E3DInvQ( int srcqid, int dstqid );

EASY3D_API int E3DSetCameraTwist( float twistdeg );
EASY3D_API int E3DIKRotateBeta( int scid, int hsid, int boneno, int motid, int frameno, int iklevel, int axiskind, D3DXVECTOR3 axis,
	int calclevel, D3DXVECTOR3 target, D3DXVECTOR3* resptr, int* lastparentptr );
EASY3D_API int E3DIKRotateBeta2D( int scid, int hsid, int bnoeno, int motid, int frameno, int iklevel, int axiskind, D3DXVECTOR3 axis,
	int calclevel, D3DXVECTOR2 target2d, D3DXVECTOR3* resptr, int* lastparentptr );
EASY3D_API int E3DGetMotionType( int hsid, int motid, int* typeptr );
EASY3D_API int E3DSetMotionType( int hsid, int motid, int type );
EASY3D_API int E3DGetIKTransFlag( int hsid, int boneno, int* flagptr );
EASY3D_API int E3DSetIKTransFlag( int hsid, int boneno, int flag );
EASY3D_API int E3DDestroyAllMotion( int hsid );
EASY3D_API int E3DGetUserInt1OfPart( int hsid, int partno, int* userint1ptr );
EASY3D_API int E3DSetUserInt1OfPart( int hsid, int partno, int userint1 );

EASY3D_API int E3DGetBSphere( int hsid, int partno, BSPHERE* bsptr );
EASY3D_API int E3DGetChildJoint( int hsid, int parentno, int* childarray, int arrayleng, int* getnumptr );
EASY3D_API int E3DDestroyMotionFrame( int hsid, int motid, int frameno );
EASY3D_API int E3DGetKeyFrameNo( int hsid, int motid, int boneno, int* framenoarray, int arrayleng, int* getnumptr );
EASY3D_API int E3DSetUV( int hsid, int partno, int vertno, UV uv, int setflag, int clampflag );
EASY3D_API int E3DGetUV( int hsid, int partno, int vertno, UV* uvptr );
EASY3D_API int E3DPickBone( int hsid, POINT scpos, int* bonenoptr );
EASY3D_API int E3DShiftBoneTree2D( int scid, int hsid, int boneno, int motid, int frameno, D3DXVECTOR2 target2d );
EASY3D_API int E3DSetDispSwitch( int hsid, int boneno, int motid, int swid, int srcframeno, int swflag );
EASY3D_API int E3DGetDispSwitch( int hsid, int* swarray, int arrayleng );
EASY3D_API int E3DCreateTexture( const char* filename, int pool, int transparent, int* texidptr );

EASY3D_API int E3DGetTextureInfo( TEXTUREINFO* texinfoptr );
EASY3D_API int E3DGetTextureFromDispObj( int hsid, int partno, int* texidptr );
EASY3D_API int E3DSetTextureToDispObj( int hsid, int partno, int texid );
EASY3D_API int E3DDestroyTexture( int texid );
EASY3D_API int E3DSetLightSpecular( int lid, E3DCOLOR4UC col );
EASY3D_API int E3DCreateTextureFromBuf( char* buf, int bufsize, int pool, int transparent, int* texidptr );
EASY3D_API int E3DLoadSoundFromBuf( char* buf, int bufleng, int type, int use3dflag, int rebervflag, int bufnum, int* soundidptr );
EASY3D_API int E3DTwistBone( int hsid, int motid, int frameno, int boneno, float twistdeg );
EASY3D_API int E3DSetStartPointOfSound( int soundid, int sec );
EASY3D_API int E3DGetBoneQ( int hsid, int boneno, int motid, int frameno, int kind, int qid );
EASY3D_API int E3DSetBoneQ( int hsid, int boneno, int motid, int frameno, int qid, int curboneonly = 0 );

EASY3D_API int E3DIsSoundPlaying( int soundid, int* playingptr );
EASY3D_API int E3DIKTranslate( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3 pos );
EASY3D_API int E3DSetUVTile( int hsid, int partno, int texrule, int unum, int vnum, int tileno );
EASY3D_API int E3DImportMQOFileAsGround( int hsid, const char* filename, int adjustuvflag, float mult, D3DXVECTOR3 offset, D3DXVECTOR3 rot );
EASY3D_API int E3DLoadMQOFileAsMovableAreaFromBuf( char* buf, int bufsize, float mult, int* hsidptr );
EASY3D_API int E3DChkThreadWorking( int threadid, int* workingptr, int* ret1ptr, int* ret2ptr );
EASY3D_API int E3DLoadMQOFileAsGroundThread( const char* filename, float mult, int adjustuvflag, int* threadidptr );
EASY3D_API int E3DSigLoadThread( const char* filename, int adjustuvflag, float mult, int* thraedidptr );
EASY3D_API int E3DSigLoadFromBufThread( const char* texdir, char* buf, int bufsize, int adjustuvflag, float mult, int* threadidptr );
EASY3D_API int E3DLoadMQOFileAsMovableAreaThread( const char* filename, float mult, int* threadidptr );
//300


EASY3D_API int E3DLoadMQOFileAsMovableAreaFromBufThread( char* buf, int bufisze, float mult, int* threadidptr );
EASY3D_API int E3DLoadGroundBMPThread( const char* tempname1, const char* tempname2, const char* tempname3, const char* tempname4,
	float maxx, float maxz, int divx, int divz, float maxheight, int* threadidptr );
EASY3D_API int E3DAddMotionThread( int hsid, const char* tempname, int* threadidptr );
EASY3D_API int E3DAddMotionFromBufThread( int hsid, char* buf, int bufsize, int* threadidptr );
EASY3D_API int E3DGetShaderType( int hsid, int* shaderptr );
EASY3D_API int E3DSetShaderType( int hsid, int shader );
EASY3D_API int E3DGetInfElemNum( int hsid, int partno, int vertno, int* numptr );
EASY3D_API int E3DGetInfElem( int hsid, int partno, int vertno, E3DINFELEM* infelemptr );
EASY3D_API int E3DSetInfElem( int hsid, int partno, int vertno, int childjointno, int calcmode, int paintmode, int normalizeflag,
	float userrate, float directval );
EASY3D_API int E3DDeleteInfElem( int hsid, int partno, int vertno, int childjointno, int normalizeflag );

EASY3D_API int E3DSetInfElemDefault( int hsid, int partno, int vertno );
EASY3D_API int E3DNormalizeInfElem( int hsid, int partno, int vertno );
EASY3D_API int E3DGetMidiMusicTime( int soundid, int* mtimeptr );
EASY3D_API int E3DGetReferenceTime( int soundid, int* mtimeptr, double* reftimeptr );
EASY3D_API int E3DSetNextMotionFrameNo( int hsid, int motid, int nextmotid, int nextframeno, int befframeno );
EASY3D_API int E3DSetLinearFogParams( int enable, E3DCOLOR4UC col, float start, float end, int srchsid );
EASY3D_API int E3DSetToon0Params( int hsid, int partno, TOON0PARAMS toon0 );
EASY3D_API int E3DSetMovableAreaThread( const char* tempname, float maxx, float maxz, int divx, int divz, float wallheight, int* threadidptr );
EASY3D_API int E3DEncodeBeta( const char* tempname, int* xorptr );
EASY3D_API int E3DDecodeBeta( const char* tempname, int* xorptr, char* bufptr);

EASY3D_API int E3DChkVertexShader( int* chkvs );
EASY3D_API int E3DLoadMOAFile( int hsid, const char* moaname, int fillupleng, float mvmult );
EASY3D_API int E3DSetNewPoseByMOA( int hsid, int eventno );
EASY3D_API int E3DGetMotionFrameNo( int hsid, int* motidptr, int* framenoptr );
EASY3D_API int E3DGetMoaInfo( int hsid, int maxnum, MOAINFO* moainfoptr, int* getnumptr );
EASY3D_API int E3DGetNextMotionFrameNo( int hsid, int motid, int* nextmotidptr, int* nextframeptr );
EASY3D_API int E3DEnableToonEdge( int hsid, int partno, int type, int flag );
EASY3D_API int E3DSetToonEdge0Color( int hsid, int partno, const char* mataerialname, E3DCOLOR4UC col );
EASY3D_API int E3DSetToonEdge0Width( int hsid, int partno, const char* materialname, float width );
EASY3D_API int E3DGetToonEdge0Color( int hsid, int partno, const char* materialname, E3DCOLOR4UC* colptr );

EASY3D_API int E3DGetToonEdge0Width( int hsid, int partno, const char* materialname, float* widthptr );
EASY3D_API int E3DCreateParticle( int maxnum, const char* texname, float width, float height, int blendmode, 
	int transparentflag, int cmpalways, int* particleidptr );
EASY3D_API int E3DDestroyParticle( int particleid );
EASY3D_API int E3DSetParticlePos( int particleid, D3DXVECTOR3 pos );
EASY3D_API int E3DSetParticleGravity( int particleid, float gravity );
EASY3D_API int E3DSetParticleLife( int particleid, float life );
EASY3D_API int E3DSetParticleEmitNum( int particleid, float emitnum );
EASY3D_API int E3DSetParticleVel0( int particleid, D3DXVECTOR3 minvel, D3DXVECTOR3 maxvel );
EASY3D_API int E3DSetParticleRotation( int particleid, float rotation );
EASY3D_API int E3DSetParticleDiffuse( int particleid, E3DCOLOR4UC col );

EASY3D_API int E3DRenderParticle( int scid, int particleid, int fps, int onlyupdate );
EASY3D_API int E3DSetParticleAlpha( int particleid, float mintime, float maxtime, float alpha );
EASY3D_API int E3DSetParticleUVTile( int particleid, float mintime, float maxtime, int unum, int vnum, int tileno );
EASY3D_API int E3DInitParticle( int particleid );
EASY3D_API int E3DGetMotionIDByName( int hsid, const char* motname, int* motidptr );
EASY3D_API int E3DGetMotionNum( int hsid, int* numptr );
EASY3D_API int E3DLoadSigFileAsGroundFromBuf( const char* texdir, char* buf, int bufsize, int adjustuvflag, float mult, int* hsidptr );
EASY3D_API int E3DGetCenterPos( int hsid, int partno, D3DXVECTOR3* posptr );
EASY3D_API int E3DGetFaceNum( int hsid, int partno, int* numptr );
EASY3D_API int E3DGetFaceNormal( int hsid, int partno, int faceno, D3DXVECTOR3* nptr );

EASY3D_API int E3DCreateEmptyMotion( int hsid, const char* motname, int frameleng, int* motidptr, int zatype = ZA_1 );
EASY3D_API int E3DSetTextureMinMagFilter( int scid, int hsid, int partno, int minfilter, int magfilter );
EASY3D_API int E3DGetMotionName( int hsid, int motid, char* motname );
EASY3D_API int E3DSetMotionName( int hsid, int motid, const char* motname );
EASY3D_API int E3DGetMaterialNoByName( int hsid, const char* matname, int* matnoptr);
EASY3D_API int E3DGetMaterialAlpha( int hsid, int matno, float* alphaptr );
EASY3D_API int E3DGetMaterialDiffuse( int hsid, int matno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetMaterialSpecular( int hsid, int matno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetMaterialAmbient( int hsid, int matno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetMaterialEmissive( int hsid, int matno, E3DCOLOR4UC* colptr );

EASY3D_API int E3DGetMaterialPower( int hsid, int matno, float* powptr );
EASY3D_API int E3DSetMaterialAlpha( int hsid, int matno, float alpha );
EASY3D_API int E3DSetMaterialDiffuse( int hsid, int matno, int setflag, E3DCOLOR4UC col );
EASY3D_API int E3DSetMaterialSpecular( int hsid, int matno, int setflag, E3DCOLOR4UC col );
EASY3D_API int E3DSetMaterialAmbient( int hsid, int matno, int setflag, E3DCOLOR4UC col );
EASY3D_API int E3DSetMaterialEmissive( int hsid, int matno, int setflag, E3DCOLOR4UC col );
EASY3D_API int E3DSetMaterialPower( int hsid, int matno, int setflag, float power );
EASY3D_API int E3DGetTextureFromMaterial( int hsid, int matno, int* texidptr );
EASY3D_API int E3DSetTextureToMaterial( int hsid, int matno, int texid );
EASY3D_API int E3DGetMaterialBlendingMode( int hsid, int matno, int* modeptr );

EASY3D_API int E3DSetMaterialBlendingMode( int hsid, int matno, int mode );
EASY3D_API int E3DGetMaterialNo( int hsid, int partno, int faceno, int* matnoptr );
EASY3D_API int E3DSetMotionKindML( int hsid, int motid, int* listptr, int* notlistptr );
EASY3D_API int E3DSetMotionFrameNoML( int hsid, int motid, int frameno, int* listptr, int* notlistptr );
EASY3D_API int E3DSetNewPoseML( int hsid );
EASY3D_API int E3DSetMOAEventNoML( int hsid, int eventno, int* listptr, int* notlistptr );
EASY3D_API int E3DSetNextMotionFrameNoML( int hsid, int motid, int nextmotid, int nextframe, int befframe, int* listptr, int* notlistptr);
EASY3D_API int E3DGetMotionFrameNoML( int hsid, int bnoeno, int* motidptr, int* framenoptr );
EASY3D_API int E3DGetNextMotionFrameNoML( int hsid, int boneno, int motid, int* motidptr, int* framenoptr );
EASY3D_API int E3DGetCurrentBonePos( int hsid, int boneno, int poskind, D3DXVECTOR3* posptr );

EASY3D_API int E3DGetCurrentBoneQ( int hsid, int boneno, int kind, int qid );
EASY3D_API int E3DChkBumpMapEnable( int* flagptr );
EASY3D_API int E3DEnableBumpMap( int flag );
EASY3D_API int E3DConvHeight2NormalMap( const char* hname, const char* nname, float hparam );
EASY3D_API int E3DSetBumpMapToMaterial( int hsid, int matno, int texid );
EASY3D_API int E3DGetMOATrunkInfo( int hsid, int motid, MOATRUNKINFO* trunkptr );
EASY3D_API int E3DGetMOABranchInfo( int hsid, int motid, MOABRANCHINFO* branchptr, int arrayeleng, int* getnumptr );
EASY3D_API int E3DSetMOABranchFrame1( int hsid, int tmotid, int bmotid, int frameno1 );
EASY3D_API int E3DSetMOABranchFrame2( int hsid, int tmotid, int bmotid, int frameno2 );
EASY3D_API int E3DEnablePhongEdge( int hsid, int flag );

EASY3D_API int E3DSetPhongEdge0Params( int hsid, E3DCOLOR4UC col, float width, int blendmode, float alpha );
EASY3D_API int E3DFreeThread( int threadid );
EASY3D_API int E3DLoadSigFileAsGroundThread( const char* filename, int adjustuvflag, float mult, int* threadidptr );
EASY3D_API int E3DLoadSigFileAsGroundFromBufThread( const char* texdir, char* buf, int bufsize, int adjustuvflag, float mult, int* threadidptr );
EASY3D_API int E3DSetLightIdOfBumpMap( int lid );
EASY3D_API int E3DCreateRenderTargetTexture( SIZE size, int srcfmt, int* scidptr, int* texidptr, int* okflag, int needz = 1 );
EASY3D_API int E3DDestroyRenderTargetTexture( int scid, int texid );
EASY3D_API int E3DSetDSFillUpMode( int hsid, int mode );
EASY3D_API int E3DSetTexFillUpMode( int hsid, int mode );
EASY3D_API int E3DSetShadowMapCamera( D3DXVECTOR3 campos, D3DXVECTOR3 camtarget, D3DXVECTOR3 camupvec );
//400


EASY3D_API int E3DSetShadowMapProjOrtho( int rttexid, float pnear, float pfar, float volsize );
EASY3D_API int E3DRenderWithShadow( int scid, int rtscid, int rttexid, int* hsidarray, int num, int skipflag );
EASY3D_API int E3DSetShadowBias( float bias );
EASY3D_API int E3DCheckRTFormat( int srcfmt, int* okflag );
EASY3D_API int E3DChkShadowEnable( int* flagptr );
EASY3D_API int E3DEnableShadow( int flag );
EASY3D_API int E3DRenderBatch( int scid, int* hsidarray, int num, int needchkinview, int skipflag );
EASY3D_API int E3DSetShadowDarkness( float darkness );
EASY3D_API int E3DSetShadowMapLightDir( D3DXVECTOR3 ldir );
EASY3D_API int E3DGlobalToLocal( int hsid, D3DXVECTOR3 srcvec, D3DXVECTOR3* dstvecptr );

EASY3D_API int E3DLocalToGlobal( int hsid, D3DXVECTOR3 srcvec, D3DXVECTOR3* dstvecptr );
EASY3D_API int E3DSetShadowMapMode( int mode );
EASY3D_API int E3DRenderBatchMode( int scid, int* hsidarray, int num, int needchkinview, int skipflag, int* modeptr );
EASY3D_API int E3DLoadTextureFromBuf( int texid, char* buf, int bufsize, int transparent );
EASY3D_API int E3DSetShadowMapInterFlag( int hsid, int partno, int srcflag );
EASY3D_API int E3DTransformBillboard();
EASY3D_API int E3DSetMaterialAlphaTest( int hsid, int matno, int alphatest0, int alphaval0, int valphatest1, int alphaval1 );

EASY3D_API int E3DGetMaterialNum( int hsid, int* numptr );
EASY3D_API int E3DGetMaterialInfo( int hsid, MATERIALINFO* infoptr, int arrayleng, int* getnumptr );
EASY3D_API int E3DGetToon1MaterialNum( int hsid, int partno, int* numptr );
EASY3D_API int E3DGetToon1MaterialInfo( int hsid, int partno, TOON1MATERIALINFO* infoptr, int arrayleng, int* getnumptr );

EASY3D_API int E3DSetJointInitialPos( int hsid, int jointno, D3DXVECTOR3 pos, int calcflag, int excludeflag );

EASY3D_API int E3DSetVertexDiffuse( int hsid, int partno, int vertno, E3DCOLOR4UC col, int setflag );
EASY3D_API int E3DSetVertexAmbient( int hsid, int partno, int vertno, E3DCOLOR4UC col, int setflag );
EASY3D_API int E3DSetVertexSpecular( int hsid, int partno, int vertno, E3DCOLOR4UC col, int setflag );
EASY3D_API int E3DSetVertexEmissive( int hsid, int partno, int vertno, E3DCOLOR4UC col, int setflag );
EASY3D_API int E3DSetVertexSpecularPower( int hsid, int partno, int vertno, float power, int setflag );
EASY3D_API int E3DGetVertexDiffuse( int hsid, int partno, int vertno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetVertexAmbient( int hsid, int partno, int vertno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetVertexSpecular( int hsid, int partno, int vertno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetVertexEmissive( int hsid, int partno, int vertno, E3DCOLOR4UC* colptr );
EASY3D_API int E3DGetVertexSpecularPower( int hsid, int partno, int vertno, float* powerptr );

EASY3D_API int E3DCreateSkinMat( int hsid, int partno );
EASY3D_API int E3DSetSymInfElem( int hsid, int partno, int vertno, int symaxis, float symdist );
EASY3D_API int E3DUpdateSymInfElem( int hsid, int excludeflag );
EASY3D_API int E3DCalcMLMotion( int hsid );

EASY3D_API int E3DSetLightId2OfBumpMap( int lid );
EASY3D_API int E3DSetLightId3OfBumpMap( int lid );

EASY3D_API int E3DGetCharaDir( int hsid, float degxz, float degy, D3DXVECTOR3* dstdir );
EASY3D_API int E3DGetStrSizeByFontID( int fontid, const char* strmes, SIZE* sizeptr );
EASY3D_API int E3DSetUVBatch( int hsid, int partno, int* vertnoptr, int vertnum, UV* uvptr, int setflag, int clampflag );
EASY3D_API int E3DSetPresentInterval( int flag );
EASY3D_API int E3DSetBillboardSize( int bbid, float width, float height, int dirmode, int orgflag );
EASY3D_API int E3DSetHeightFogParams( int enable, E3DCOLOR4UC col, float start, float end, int srchsid );
EASY3D_API int E3DSetTextureToBG( int scid, int stageno, int texid );
EASY3D_API int E3DGetTextureFromBG( int scid, int stageno, int* texidptr );
EASY3D_API int E3DChangeMoaIdling( int hsid, int motid );

EASY3D_API int E3DSetMotionBlur( int hsid, int mode, int blurtime );
EASY3D_API int E3DSetBeforeBlur( int hsid );
EASY3D_API int E3DRenderMotionBlur( int scid, int* hsidarray, int num );
EASY3D_API int E3DInitBeforeBlur( int hsid );
EASY3D_API int E3DSetMotionBlurPart( int hsid, int mode, int blurtime, int* partptr, int num );

EASY3D_API int E3DBlurTexture16Box( int dstscid, int srctexid, int adjustuvflag = 0 );
EASY3D_API int E3DBlurTexture9Corn( int dstscid, int srctexid, int adjustuvflag = 0 );
EASY3D_API int E3DBlurTextureGauss5x5( int dstscid, int srctexid, float dispersion, int adjustuvflag = 0 );
EASY3D_API int E3DBlurTextureGauss( int finalscid, int workscid, int worktexid, int srctexid, float dispersion, int adjustuvflag = 0 );
EASY3D_API int E3DBlendRTTexture( int finalscid, int texid1, E3DCOLOR4F col1, int texf1,
	int texid2, E3DCOLOR4F col2, int texf2, int blendop, int srcblend, int destblend, int adjustuvflag = -1 );
EASY3D_API int E3DBeginSceneZbuf( int scid, int skipflag, int zscid );

EASY3D_API int E3DSetClearCol( int scid, E3DCOLOR4UC col );
EASY3D_API int E3DSetMotionBlurMinAlpha( int hsid, int partno, float minalpha );

EASY3D_API int E3DRenderGlow( int* hsidarray, int arrayleng );
EASY3D_API int E3DSetMaterialGlowParams( int hsid, int matno, int glowmode, int setcolflag, E3DCOLOR4F multcol );
EASY3D_API int E3DGetMaterialGlowParams( int hsid, int matno, int* modeptr, E3DCOLOR4F* colptr );
EASY3D_API int E3DGetMaterialTransparent( int hsid, int matno, int*transptr );
EASY3D_API int E3DSetMaterialTransparent( int hsid, int matno, int transparent );

EASY3D_API int E3DChkHDR( int* okflag, int* hdrformat, int* toneformat );
EASY3D_API int E3DSetHDRLightDiffuse( int lid, E3DCOLOR4F col );
EASY3D_API int E3DSetHDRLightSpecular( int lid, E3DCOLOR4F col );
EASY3D_API int E3DDownScaleRTTexture4x4( int dstscid, int srctexid );
EASY3D_API int E3DDownScaleRTTexture2x2( int dstscid, int srctexid );
EASY3D_API int E3DCreateToneMapTexture( int tonenum, int toneformat, int* tonemapid, int* okflag );
EASY3D_API int E3DDestroyToneMapTexture( int tonemapid );
EASY3D_API int E3DRenderToneMap( int tonemapid, int srctexid );
EASY3D_API int E3DBrightPassRTTextureHDR( int dstscid, int srctexid, float middlegray, float threshold, float offset, int tonemapid );
EASY3D_API int E3DRenderHDRFinal( int dstscid, int srctexid0, int srctexid1, int tonemapid, float middlegray );
EASY3D_API int E3DDisablePostEffect();
EASY3D_API int E3DCheckSpec( HINSTANCE srchinst, HWND srchwnd, int* okflag );

EASY3D_API int E3DMonoRTTexture( int dstscid, int srctexid );
EASY3D_API int E3DCbCrRTTexture( int dstscid, int srctexid, float srccb, float srccr );

EASY3D_API int E3DShaderConstOffset16( D3DXVECTOR2* offsetarray );
EASY3D_API int E3DShaderConstWeight16( D3DXVECTOR4* weightarray );
EASY3D_API int E3DShaderConstUserFL4( int datano, D3DXVECTOR4 fl4 );
EASY3D_API int E3DShaderConstUserTex( int datano, int srctexid );
EASY3D_API int E3DCallUserShader( int shaderno, int passno, int dstscid );

EASY3D_API int E3DCreateBone( int hsid, const char* nameptr, int parentid, int* serialptr );
EASY3D_API int E3DDestroyBone( int hsid, int boneno );


EASY3D_API int E3DGetMorphBaseNum( int hsid, int* basenumptr );
EASY3D_API int E3DGetMorphBaseInfo( int hsid, E3DMORPHBASE* baseinfoptr, int arrayleng );
EASY3D_API int E3DGetMorphTargetInfo( int hsid, int baseid, E3DMORPHTARGET* targetinfoptr, int arrayleng );
EASY3D_API int E3DGetMorphKeyNum( int hsid, int motid, int boneid, int baseid, int targetid, int* keynumptr );
EASY3D_API int E3DGetMorphKey( int hsid, int motid, int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr, int arrayleng );
EASY3D_API int E3DGetCurMorphInfo( int hsid, int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr );
EASY3D_API int E3DCreateMorph( int hsid, int baseid, int boneid );
EASY3D_API int E3DAddMorphTarget( int hsid, int baseid, int targetid );
EASY3D_API int E3DSetMorphKey( int hsid, int motid, int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr );
EASY3D_API int E3DDestroyMorphKey( int hsid, int motid, int boneid, int frameno, int baseid );


//EASY3D_API int E3DGetToon1MaterialNum( int hsid, int partno, int* matnumptr );
EASY3D_API int E3DGetToon1Material( int hsid, int partno, E3DTOON1MATERIAL* toon1ptr, int arrayleng );
EASY3D_API int E3DSetToon1Name( int hsid, int partno, char* oldname, char* newname );
EASY3D_API int E3DSetToon1Diffuse( int hsid, int partno, char* matname, RDBColor3f diffuse );
EASY3D_API int E3DSetToon1Specular( int hsid, int partno, char* matname, RDBColor3f specular );
EASY3D_API int E3DSetToon1Ambient( int hsid, int partno, char* matname, RDBColor3f ambient );
EASY3D_API int E3DSetToon1NL( int hsid, int partno, char* matname, float darknl, float brightnl );
EASY3D_API int E3DSetToon1Edge0( int hsid, int partno, char* matname, RDBColor3f col, int validflag, int invflag, float width );


EASY3D_API int E3DLoadCameraFile( char* filename, int* animnoptr );
EASY3D_API int E3DDestroyCameraAnim( int animno );
EASY3D_API int E3DSaveCameraFile( char* filename, int animno );
EASY3D_API int E3DSetCameraAnimNo( int animno );
EASY3D_API int E3DGetCameraAnimNo( int* animnoptr );
EASY3D_API int E3DSetCameraFrameNo( int animno, int camerano, int frameno, int isfirst );
EASY3D_API int E3DGetCameraFrameNo( int* animnoptr, int* camnoptr, int* framenoptr );
EASY3D_API int E3DSetNewCamera( int camno );
EASY3D_API int E3DSetNextCameraAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno );
EASY3D_API int E3DGetNextCameraAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr );
EASY3D_API int E3DCreateNewCameraAnim( char* animname, int animtype, int framenum, int* animnoptr );
EASY3D_API int E3DSetCameraFrameNum( int animno, int newtotal );
EASY3D_API int E3DGetCameraFrameNum( int animno, int* framenumptr );
EASY3D_API int E3DSetCameraAnimName( int animno, char* srcname );
EASY3D_API int E3DGetCameraAnimName( int animno, char* dstname, int arrayleng );
EASY3D_API int E3DSetCameraAnimType( int animno, int type );
EASY3D_API int E3DGetCameraAnimType( int animno, int* typeptr );
EASY3D_API int E3DSetCameraKey( int animno, int camno, int frameno, E3DCAMERASTATE camstate );
EASY3D_API int E3DGetCameraKey( int animno, int camno, int frameno, E3DCAMERASTATE* camstateptr, int* existptr );
EASY3D_API int E3DDeleteCameraKey( int animno, int camno, int frameno );
EASY3D_API int E3DGetCameraKeyframeNoRange( int animno, int camno, int start, int end, int* framearray, int arrayleng, int* getnumptr );
EASY3D_API int E3DSetCameraSWKey( int animno, int frameno, int camno );
EASY3D_API int E3DGetCameraSWKey( int animno, int frameno, int* camnoptr, int* existptr );
EASY3D_API int E3DDeleteCameraSWKey( int animno, int frameno );
EASY3D_API int E3DGetCameraSWKeyframeNoRange( int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr );

EASY3D_API int E3DSetOrtho( float nearz, float farz, float orthosize );

EASY3D_API int E3DLoadGPFile( int hsid, const char* filename, int* animnoptr );
EASY3D_API int E3DDestroyGPAnim( int hsid, int animno );
EASY3D_API int E3DSaveGPFile( int hsid, const char* filename, int animno );
EASY3D_API int E3DSetGPAnimNo( int hsid, int animno );
EASY3D_API int E3DGetGPAnimNo( int hsid, int* animnoptr );
EASY3D_API int E3DSetGPFrameNo( int hsid, int animno, int frameno, int isfirst );
EASY3D_API int E3DGetGPFrameNo( int hsid, int* animnoptr, int* framenoptr );
EASY3D_API int E3DSetNewGP( int hsid );
EASY3D_API int E3DSetNextGPAnimFrameNo( int hsid, int animno, int nextanimno, int nextframeno, int befframeno );
EASY3D_API int E3DGetNextGPAnimFrameNo( int hsid, int animno, int* nextanimptr, int* nextframenoptr );
EASY3D_API int E3DCreateNewGPAnim( int hsid, const char* animname, int animtype, int framenum, int* animnoptr );
EASY3D_API int E3DSetGPFrameNum( int hsid, int animno, int newtotal );
EASY3D_API int E3DGetGPFrameNum( int hsid, int animno, int* framenumptr );
EASY3D_API int E3DSetGPAnimName( int hsid, int animno, const char* srcname );
EASY3D_API int E3DGetGPAnimName( int hsid, int animno, char* dstname, int arrayleng );
EASY3D_API int E3DSetGPAnimType( int hsid, int animno, int type );
EASY3D_API int E3DGetGPAnimType( int hsid, int animno, int* typeptr );
EASY3D_API int E3DSetGPKey( int hsid, int animno, int frameno, E3DGPSTATE gpstate );
EASY3D_API int E3DGetGPKey( int hsid, int animno, int frameno, E3DGPSTATE* gpstateptr, int* existptr );
EASY3D_API int E3DDeleteGPKey( int hsid, int animno, int frameno );
EASY3D_API int E3DGetGPKeyframeNoRange( int hsid, int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr );
EASY3D_API int E3DCalcGPFullFrame( int hsid, int animno );
EASY3D_API int E3DSetGPGroundHSID( int hsid, int animno, int ghsid );
EASY3D_API int E3DGetGPGroundHSID( int hsid, int animno, int* ghsidptr );

EASY3D_API int E3DLoadMOEFile( int hsid, const char* filename, int* animnoptr );
EASY3D_API int E3DDestroyMOEAnim( int hsid, int animno );
EASY3D_API int E3DSaveMOEFile( int hsid, const char* filename, int animno );
EASY3D_API int E3DSetMOEAnimNo( int hsid, int animno );
EASY3D_API int E3DGetMOEAnimNo( int hsid, int* animnoptr );
EASY3D_API int E3DSetMOEFrameNo( int hsid, int animno, int frameno, int isfirst );
EASY3D_API int E3DGetMOEFrameNo( int hsid, int* animnoptr, int* framenoptr );
EASY3D_API int E3DSetNewMOE( int hsid );
EASY3D_API int E3DSetNextMOEAnimFrameNo( int hsid, int animno, int nextanimno, int nextframeno, int befframeno );
EASY3D_API int E3DGetNextMOEAnimFrameNo( int hsid, int animno, int* nextanimptr, int* nextframenoptr );
EASY3D_API int E3DCreateNewMOEAnim( int hsid, char* animname, int animtype, int framenum, int* animnoptr );
EASY3D_API int E3DSetMOEFrameNum( int hsid, int animno, int newtotal );
EASY3D_API int E3DGetMOEFrameNum( int hsid, int animno, int* framenumptr );
EASY3D_API int E3DSetMOEAnimName( int hsid, int animno, char* srcname );
EASY3D_API int E3DGetMOEAnimName( int hsid, int animno, char* dstname, int arrayleng );
EASY3D_API int E3DSetMOEAnimType( int hsid, int animno, int type );
EASY3D_API int E3DGetMOEAnimType( int hsid, int animno, int* typeptr );
EASY3D_API int E3DGetMOEKeyframeNoRange( int hsid, int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr );
EASY3D_API int E3DGetMOEElemNum( int hsid, int animno, int frameno, int* moeenumptr, int* existkeyptr );
EASY3D_API int E3DGetMOEElem( int hsid, int animno, int frameno, int moeeindex, MOEELEM* moeeptr );
EASY3D_API int E3DAddMOEElem( int hsid, int animno, int frameno, MOEELEM moee );
EASY3D_API int E3DDeleteMOEElem( int hsid, int animno, int frameno, int moeeindex );
EASY3D_API int E3DSetMOEKeyIdling( int hsid, int animno, int frameno, int motid );
EASY3D_API int E3DGetMOEKeyIdling( int hsid, int animno, int frameno, int* motidptr );
EASY3D_API int E3DDeleteMOEKey( int hsid, int animno, int frameno );
EASY3D_API int E3DCalcMOEFullFrame( int hsid, int animno );

EASY3D_API int E3DCreateAndSaveTempMaterial( int hsid );
EASY3D_API int E3DRestoreAndDestroyTempMaterial( int hsid );
EASY3D_API int E3DSetMaterialFromInfElem(  int hsid, int jointno, int partno, int vertno, E3DCOLOR3UC* infcolarray = 0 );

EASY3D_API int E3DUpdateSound();
EASY3D_API int E3DGetSoundSamplesPlayed( int soundid, double* psmp );
EASY3D_API int E3DSetSoundFrequency( int soundid, float freq );
EASY3D_API int E3DGetSoundFrequency( int soundid, float* pfreq );
EASY3D_API int E3DSet3DSoundEmiDist( int soundid, float dist );

EASY3D_API int E3DFadeSound( int soundid, int msec, float begingain, float endgain );

EASY3D_API int E3DGlobalToLocalParBone( int hsid, int motid, int frameno, int bnoeno, int flag,
	int srcqid, int dstqid, D3DXVECTOR3* srcvec, D3DXVECTOR3* dstvec );
EASY3D_API int E3DLocalToGlobalParBone( int hsid, int motid, int frameno, int boneno, int flag,
	int srcqid, int dstqid, D3DXVECTOR3* srcvecptr, D3DXVECTOR3* dstvecptr );

EASY3D_API int E3DGetInfElemSymParams( int hsid, int partno, int vertno, int* symtypeptr, float* distptr );


EASY3D_API int E3DDestroyMotion( int hsid, int motid, IDCHANGE* idchgptr, int idnum );

EASY3D_API int E3DGetIKTargetPos( int hsid, int boneno, D3DXVECTOR3* posptr );
EASY3D_API int E3DGetIKTargetState( int hsid, int boneno, int* stateptr );
EASY3D_API int E3DGetIKTargetCnt( int hsid, int boneno, int* cntptr );
EASY3D_API int E3DGetIKTargetLevel( int hsid, int boneno, int* levelptr );
EASY3D_API int E3DGetIKLevel( int hsid, int boneno, int* levelptr );
EASY3D_API int E3DSetIKTargetPos( int hsid, int boneno, D3DXVECTOR3 srcpos );
EASY3D_API int E3DSetIKTargetState( int hsid, int boneno, int srcstate );
EASY3D_API int E3DSetIKTargetCnt( int hsid, int boneno, int srccnt );
EASY3D_API int E3DSetIKTargetLevel( int hsid, int boneno, int srclevel );
EASY3D_API int E3DSetIKLevel( int hsid, int boneno, int srclevel );

EASY3D_API int E3DGetQVal( int qid, E3DQ* qptr );
EASY3D_API int E3DSetQVal( int qid, E3DQ srcq );
EASY3D_API int E3DMultQArray( int* qidarray, int qnum, int resultid );
EASY3D_API int E3DEulerZXY2Q( int qid, D3DXVECTOR3 srceul );

EASY3D_API int E3DGetEulerLimit( int hsid, int boneno, EULERLIMIT* elptr );
EASY3D_API int E3DSetEulerLimit( int hsid, int boneno, EULERLIMIT srcel );
EASY3D_API int E3DCalcEuler( int hsid, int motid, int zakind );
EASY3D_API int E3DGetBoneEuler( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3* eulptr );

EASY3D_API int E3DGetZa4Elem( int hsid, int boneno, ZA4ELEM* z4eptr );
EASY3D_API int E3DSetZa4Elem( int hsid, int boneno, ZA4ELEM srcz4e );
EASY3D_API int E3DCalcZa4LocalQ( int hsid, int motid, int frameno );
EASY3D_API int E3DGetZa4LocalQ( int hsid, int boneno, E3DQ* dstq );
EASY3D_API int E3DGetZa3IniQ( int hsid, int boneno, E3DQ* dstq );

EASY3D_API int E3DGetBoneAxisQ( int hsid, int boneno, E3DQ* dstq );
EASY3D_API int E3DSetBoneAxisQ( int hsid, int boneno, int motid, int frameno, int zakind );


EASY3D_API int E3DGetBoneTra( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3* traptr, int* existptr );
EASY3D_API int E3DGetBoneScale( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3* scaleptr, int* existptr );
EASY3D_API int E3DSetBoneTra( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3 srctra );
EASY3D_API int E3DSetBoneScale( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3 srcscale );
EASY3D_API int E3DGetBoneMatrix( int hsid, int boneno, int motid, int frameno, int kind, D3DXMATRIX* matptr );
EASY3D_API int E3DCreateMotionPoint( int hsid, int boneno, int motid, int frameno );

EASY3D_API int E3DTransform( int hsid, int boneonly );

EASY3D_API int E3DGetIKSkip( int hsid, int boneno, int* flagptr );
EASY3D_API int E3DSetIKSkip( int hsid, int boneno, int flag );

EASY3D_API int E3DCreateTexKey( int hsid, int boneno, int motid, int frameno );
EASY3D_API int E3DDelTexKey( int hsid, int boneno, int motid, int frameno );
EASY3D_API int E3DAddTexChange( int hsid, int boneno, int motid, int frameno, TEXCHANGE srctc );
EASY3D_API int E3DDelTexChange( int hsid, int boneno, int motid, int frameno, int materialno );
EASY3D_API int E3DGetTexChangeNum( int hsid, int boneno, int motid, int frameno, int* tcnumptr );
EASY3D_API int E3DGetTexChange( int hsid, int boneno, int motid, int frameno, int tcindex, TEXCHANGE* dsttc );
EASY3D_API int E3DGetTexAnimKeyframeNoRange( int hsid, int boneno, int motid, 
		int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );

EASY3D_API int E3DCreateDSKey( int hsid, int boneno, int motid, int frameno );
EASY3D_API int E3DDelDSKey( int hsid, int boneno, int motid, int frameno );
EASY3D_API int E3DAddDSChange( int hsid, int boneno, int motid, int frameno, DSCHANGE srcdsc );
EASY3D_API int E3DDelDSChange( int hsid, int boneno, int motid, int frameno, int switchno );
EASY3D_API int E3DGetDSChangeNum( int hsid, int boneno, int motid, int frameno, int* dscnumptr );
EASY3D_API int E3DGetDSChange( int hsid, int boneno, int motid, int frameno, int dscindex, DSCHANGE* dstdsc );
EASY3D_API int E3DGetDSAnimKeyframeNoRange( int hsid, int boneno, int motid, 
		int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );

EASY3D_API int E3DGetMaterialTexParams( int hsid, int materialno, MATERIALTEX* dstparams );
EASY3D_API int E3DSetMaterialTexParams( int hsid, int materialno, MATERIALTEX* srcparams );
EASY3D_API int E3DGetMaterialTexParams1DIM( int hsid, int materialno, MATERIALTEX1DIM* dstparams );
EASY3D_API int E3DSetMaterialTexParams1DIM( int hsid, int materialno, MATERIALTEX1DIM* srcparams );

EASY3D_API int E3DGetMaterialAlphaTest( int hsid, int materialno, int* test0ptr, int* val0ptr, int* test1ptr, int* val1ptr );

EASY3D_API int E3DSet3DSoundDopplerScaler( int soundid, float srcdp );
EASY3D_API int E3DGet3DSoundDopplerScaler( int soundid, float* dpptr );
EASY3D_API int E3DLoadMQOFileAsChara( const char* tempname1, float mult, int adjustuvflag, int bonetype, int* hsidptr );

EASY3D_API int E3DGetPartPropName( int hsid, int partno, char* nameptr, int arrayleng );
EASY3D_API int E3DSetPartPropName( int hsid, int partno, char* srcname );
EASY3D_API int E3DGetPartPropDS( int hsid, int partno, int* dsptr );
EASY3D_API int E3DSetPartPropDS( int hsid, int partno, int srcds );
EASY3D_API int E3DGetPartPropNotUse( int hsid, int partno, int* flagptr );
EASY3D_API int E3DSetPartPropNotUse( int hsid, int partno, int srcflag );

EASY3D_API int E3DLoadSSF( const char* filename );
EASY3D_API int E3DDestroyAllSoundSet();
EASY3D_API int E3DGetSoundSetNum( int* numptr );
EASY3D_API int E3DGetSoundSetInfo( int ssindex, E3DSSINFO* ssinfoptr );
EASY3D_API int E3DGetSoundSetBGM( int setid, int* bgmidptr );

EASY3D_API int E3DLoadSAF( const char* filename, int* animnoptr );
EASY3D_API int E3DDestroyAllSndAnim();
EASY3D_API int E3DGetSndAnimFrameNum( int animno, int* numptr );
EASY3D_API int E3DGetSndAnimBGM( int animno, int* bgmidptr );
EASY3D_API int E3DGetSndAnimFPS( int animno, int* fpsptr );
EASY3D_API int E3DGetSndAnimMode( int animno, int* modeptr );
EASY3D_API int E3DStartSndAnim( int animno, int startframe );
EASY3D_API int E3DSetSndAnimFrameNo( int animno, int srcframeno );
EASY3D_API int E3DStopSndAnim( int animno );

EASY3D_API int E3DGetSoundTotalTime( int soundid, double* timeptr );
EASY3D_API int E3DConvSoundSampleToFrame( int soundid, int srcfps, double srcsample, int* frameptr );
EASY3D_API int E3DGetSoundStartCnt( int soundid, int* cntptr );
EASY3D_API int E3DGetSoundEndCnt( int soundid, int* cntptr );

EASY3D_API int E3DPCWait( int srcfps, double* dstfps );

EASY3D_API int E3DCloneDispObj( int hsid, int partno, char* newname, int* newseriptr );
EASY3D_API int E3DGetJointNotSel( int hsid, int boneno, int* notselptr );
EASY3D_API int E3DSetJointNotSel( int hsid, int boneno, int notsel );
EASY3D_API int E3DDestroyMorph( int hsid, int baseno );
EASY3D_API int E3DDestroyDispObj( int hsid, int partno );

EASY3D_API int E3DSetCameraOffset( D3DXVECTOR3 offbef, int offqid, D3DXVECTOR3 offaft );
EASY3D_API int E3DAllClearInfElem( int hsid, int partno, int vertno );
EASY3D_API int E3DSetGPOffset( int hsid, D3DXVECTOR3 tra, int qid );
EASY3D_API int E3DSelectLightType( int ltype );
EASY3D_API int E3DSetSpotLight( int lid, D3DXVECTOR3 pos, D3DXVECTOR3 dir, float dist, float angle, E3DCOLOR4UC col );
EASY3D_API int E3DRenderZandN( int outzscid, int outnscid, int inzscid, int hsid, int skipflag, int renderkind );

EASY3D_API int E3DCreateInfScope( int hsid, int boneno, int dispno, int* isindexptr );
EASY3D_API int E3DDestroyInfScope( int hsid, int isindex );
EASY3D_API int E3DExistInfScope( int hsid, int boneno, int dispno, int* isindexptr );
EASY3D_API int E3DGetInfScopeNum( int hsid, int* numptr );
EASY3D_API int E3DGetInfScopeInfo( int hsid, ISINFO* isiptr, int arrayleng, int* getnumptr );
EASY3D_API int E3DSetInfScopeVert( int hsid, ISVERT srcisv );
EASY3D_API int E3DGetInfScopeVert( int hsid, ISVERT* isvptr );
EASY3D_API int E3DGetInfScopeVertNum( int hsid, int isindex, int* numptr );
EASY3D_API int E3DPickInfScopeVert( int scid, int hsid, POINT scpos, POINT range, int isindex, ISVERT* isvptr, int arrayleng, int* getnumptr );
EASY3D_API int E3DGetInfScopeAxis( int hsid, int isindex, D3DXVECTOR3* dstvec3x3ptr );
EASY3D_API int E3DSetInfScopeColor( int hsid, int isindex, E3DCOLOR4UC srccol );
EASY3D_API int E3DGetInfScopeColor( int hsid, int isindex, E3DCOLOR4UC* dstcolptr );
EASY3D_API int E3DSetInfScopeDispFlag( int hsid, int isindex, int srcdispflag, int exclusiveflag );
EASY3D_API int E3DGetInfScopeDispFlag( int hsid, int isindex, int* dstflagptr );
EASY3D_API int E3DImportMQOAsInfScope( int hsid, char* filename, float mult, int boneno, int dispno, int* isindexptr );

EASY3D_API int E3DGetMOELocalMotionFrameNo( int hsid, int moeanimno, int moeframeno, int* localanimno, int* localframeno );
EASY3D_API int E3DGetMOELocalNextMotionFrameNo( int hsid, int moeanimno, int moeframeno, int* localnextanimno, int* localnextframeno );

EASY3D_API int E3DQtoEulerAxis( int qid, int axisqid, D3DXVECTOR3* eulptr, D3DXVECTOR3 befeul );
EASY3D_API int E3DSetMotionBlurMaxAlpha( int hsid, int partno, float maxalpha );

EASY3D_API int E3DSetMOEKeyGoOnFlag( int hsid, int animno, int frameno, int flag );
EASY3D_API int E3DGetMOEKeyGoOnFlag( int hsid, int animno, int frameno, int* flagptr );
EASY3D_API int E3DSetMOATrunkNotComID( int hsid, int motid, int idnum, int* srcid );
EASY3D_API int E3DGetMOELocalMotionFrameNoML( int hsid, int moeanimno, int moeframeno, int boneid, int* localanimno, int* localframeno );
EASY3D_API int E3DGetMOELocalNextMotionFrameNoML( int hsid, int moeanimno, int moeframeno, int boneid, int* localnextanimno, int* localnextframeno );

EASY3D_API int E3DSetDefaultCamera( int camno, E3DCAMERASTATE camstate );
EASY3D_API int E3DSetDefaultGP( int hsid, E3DGPSTATE gpstate );

EASY3D_API int E3DBeginSceneSimple( int skipflag );
EASY3D_API int E3DEndSceneSimple();
EASY3D_API int E3DShaderConstZN( int hsid, unsigned char dataz, unsigned char datan );
EASY3D_API int E3DShaderConstZN2( int hsid, unsigned char data[4] );
EASY3D_API int E3DRenderZN2( int outzscid, int outnscid, int outtscid, int outuscid, int inzscid, int hsid, int skipflag, int renderkind );

EASY3D_API int E3DSetKinectUpVec( int hsid, int boneno, D3DXVECTOR3 upvec );
EASY3D_API int E3DValidHsid( int hsid, int* flagptr );

EASY3D_API int E3DKinectLoadLib( int* validflag );
EASY3D_API int E3DKinectSetChara( int hsid, char* kstname, int motid, int capmode );
EASY3D_API int E3DKinectStartCapture();
EASY3D_API int E3DKinectEndCapture();
EASY3D_API int E3DKinectIsTracking( int* flagptr, int* capcntptr );
EASY3D_API int E3DKinectGetSkelPos( D3DXVECTOR3* posptr, int* getflag );
EASY3D_API int E3DKinectSetSkelPos( int kinectflag, int posmode, D3DXVECTOR3* srcpos, int* framenoptr );
EASY3D_API int E3DKinectRenderSilhouette( int withalpha, TPOS* tpos );

EASY3D_API int E3DGetInViewFlag( int hsid, int partno, int* flagptr );
EASY3D_API int E3DWriteBmpFromSurface( char* nameptr, int scid );
EASY3D_API int E3DGetMotionJumpFrame( int hsid, int motid, int* frameptr );
EASY3D_API int E3DSetMotionJumpFrame( int hsid, int motid, int frameno );
EASY3D_API int E3DCopyTexAnimFrame( int hsid, int srcmotid, int srcframe, int dstmotid, int dstframe );
EASY3D_API int E3DDestroyTexAnimFrame( int hsid, int motid, int frameno );
EASY3D_API int E3DGetMaterialExtTextureNum( int hsid, int matno, int* numptr );
EASY3D_API int E3DGetMaterialExtTexture( int hsid, int matno, int extno, char* nameptr );
EASY3D_API int E3DLoadMQOFileAsGroundFromBuf( char* filename, char* bufptr, int bufsize, int* hsidptr, int adjustuvflag, float mult );
EASY3D_API int E3DChgMaterial1stTexFromBuf( int hsid, int matno, char* bufptr, int bufsize, int* texidptr );
EASY3D_API int E3DChgMaterialExtTexFromBuf( int hsid, int matno, int extno, char* bufptr, int bufsize, int* texidptr );
EASY3D_API int E3DChgMaterial1stTexFromID( int hsid, int matno, int texid );
EASY3D_API int E3DChgMaterialExtTexFromID( int hsid, int matno, int extno, int texid );
EASY3D_API int E3DSetCameraAnimTargetHsidAll( int animno, int srchsid );

EASY3D_API int E3DSetDispSwitchForce( int hsid, int swid, int state );
EASY3D_API int E3DSetWindowRgn( int deskscid, int figscid );
EASY3D_API int E3DDestroyDispPart( int hsid, int partno );
EASY3D_API int E3DPlayMovie( int scid, const char* filename );


EASY3D_API int E3DPndInit( char* key, int keyleng, int* pndidptr );
EASY3D_API int E3DPndDestroy( int pndid );
EASY3D_API int E3DPndEncrypt( int pndid, char* srcrootfolder, char* pndpath );
EASY3D_API int E3DPndParse( int pndid, char* pndpath );
EASY3D_API int E3DPndGetRootPath( int pndid, char* dstpath, int dstleng );
EASY3D_API int E3DPndGetPropertyNum( int pndid, int* propnumptr );
EASY3D_API int E3DPndGetProperty( int pndid, int propno, char* sourcepath, int pathleng, int* sourcesizeptr );
EASY3D_API int E3DPndDecrypt( int pndid, char* sourcepath, char* dstbuf, int dstleng, int* getlengptr );
EASY3D_API int E3DPndLoadSigFile( int pndid, char* sourcepath, float mult, int* hsidptr );
EASY3D_API int E3DPndLoadMoaFile( int pndid, int hsid, char* sourcepath, int fillupleng, float mvmult );
EASY3D_API int E3DPndLoadSigFileAsGround( int pndid, char* sourcepath, float mult, int* hsidptr );

EASY3D_API int E3DPlayMovieEx( int scid, char* filename );
EASY3D_API int E3DIsPlayingMovieEx( int* playflagptr );
EASY3D_API int E3DStopMovieEx();
EASY3D_API int E3DSetMovieVolumeEx( int srcvol );

