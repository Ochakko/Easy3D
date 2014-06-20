
#ifndef MOTHANDLERH
#define MOTHANDLERH

//#include <motioninfo.h>
//#include <motionctrl.h>
#define DBGH
#include <dbg.h>

#include <matrix2.h>
#include <D3DX9.h>

#include <bsphere.h>
#include <EventKey.h>

class CTreeHandler2;
class CMotionCtrl;
class CShdHandler;
class CShdElem;
class CQuaternion;
class CMotionPoint2;
class CTexKey;
class CAlpKey;
class CDSKey;
class CDispSwitch;
class CMState;
class CMQOMaterial;

class CMotHandler
{

public:

	CMotHandler( CTreeHandler2* srchandler );
	~CMotHandler();
/***
#ifdef _DEBUG
	void	*operator new( size_t size );
	void	operator delete( void *p );
#endif
***/

	CMotionCtrl* operator() ( int srcserino ){
#ifdef _DEBUG
		if( !CheckNo( srcserino ) ){
			return *(s2mot + srcserino);
		}else{
			DbgOut( "CMotHandler : operator () : serino error %d : s2mot_leng %d !!!\n",
				srcserino, s2mot_leng );

			_ASSERT( 0 );
			return 0;
		}
#else
	return *(s2mot + srcserino);
#endif
	};

	int CheckNo( int srcseri ){
		if( (srcseri >= 0) && (srcseri < s2mot_leng) ) 
			return 0;
		else
			return 1;		
	}

	//void	DestroyMotionObj();
	int DestroyMotionObj( int delcookie = -1 );

	int AddMotionCtrl( int srcseri, int srcdepth );

	int AddS2Mot( int srcseri, int srcbro, int srcdepth, int srctype );
	//int AddD2Mot( int srcseri, int srcdepth );
	
	int InitMatrix( int srcseri );
	int SetTransMat( int srcseri, CMatrix2* srcmat );
	int SetPartMat( int srcseri, CMatrix2* srcmat );
	int	SetMotionMatrix( CMatrix2* transmat, int shapeno, int motcookie, int frameno );
	int SetHasMotion( int srcseri, int srchasmot );
	int SetNoJointMotion( int mcookie ); // motioninterfaceを持たないelemのmotionをセット。
	int SetSkipMotion( CShdHandler* lpsh, int mcookie );
	int CopyMotionMatrix( int dstseri, int srcseri, int mcookie );
	int MultCopyMotionMatrix( int dstseri, int srcseri, CMatrix2 multmat, int mcookie );

	int PrepMotionCtrl( CShdHandler* lpsh );

	int MakeRawmat( int mcookie, CShdHandler* lpsh, int srcstart, int srcend, int calceulflag = 1 );//全boneのrawmatセット
	int MakeRawmat( int mcookie, CShdElem* lpselem, int srcstart, int srcend, CShdHandler* lpsh, int calceulflag = 1 );//指定boneのrawmatセット
	int MakeRawmatCap( int mcookie, CShdElem* lpselem, int srcframe, CShdHandler* lpsh, CMotionPoint2* srcmp );//指定boneのrawmatセット

	void MakeRawmatReq( int mcookie, CShdElem* lpselem, int srcstart, int srcend, int broflag, CShdHandler* lpsh, int calceulflag = 1 );
	int RawmatToMotionmat( int mcookie, CShdHandler* lpsh, int startframe, int endframe );
	void RawmatToMotionmatReq( int mcookie, CShdHandler* lpsh, int srcseri, int* errorflag, int srcstart, int srcend, int addbroflag = 1 );
	void RawmatToMotionmatReqCap( int mcookie, CShdHandler* lpsh, int srcseri, int* errorflag, int srcframe, int addbroflag = 1 );

	//int SetShapeMat0( int srcseri, CMatrix2* srcmat );

	int SetChain( int offset = 0 );

	int CopyChainFromShd( CShdHandler* lpsh );
	int CalcDepth();

	int InitParams( CTreeHandler2* lpthandler );

	int DumpCurMat( int dbgflag, char* fname = 0 );
	int DumpPartMat( int dbgflag, char* fname = 0 );
	//int DumpMotion( char* mname, int mcookie, int framemax, int mottype );

	int GetCurMatrix( CMatrix2* dstmat, int* skiproute, int skipmax );
	int GetCurMatrix( CMatrix2* dstmat, int srcseri );
	int SetCurMatrix( int dstseri, CMatrix2 srcmat );

	int GetPartMatrix( CMatrix2* dstmat, int dstseri );
	int GetPartMatrix( CMatrix2* dstmat, int* skiproute, int skipmax );

	int AddMotion( int zatype, char* motionname, int motiontype, int framenum, int srcinterpolation, int srcmotjump );

	int SetMotionKind( int mcookie );
	int GetMotionKind( int* motkindptr );
	int SetMotionStep( int srcmotkind, int srcmotstep );
	int SetMotionType( int srcmotkind, int srcmottype );
	int SetMotionJump( int srcmotkind, int srcmotjump );
	int GetMotionType( int motid, int* typeptr );
	int GetDefInterp( int srcmotkind, int* dstinterp );
	int SetDefInterp( int srcmotkind, int srcinterpolation );
	int SetInterpolationAll( int srcmotkind, int srcinterpolation );

	int ChangeTotalFrame( int srcmotkind, int newtotal, int expandflag );
	
	int GetMotionFrameNo( int* motidptr, int* framenoptr );


	int SetMotionFrameNo( CShdHandler* lpsh, int mcookie, int srcframeno, int srcisfirst );
	int SetNextMotionFrameNo( CShdHandler* lpsh, int mcookie, int nextmk, int nextframeno, int befframeno );
	int GetNextMotionFrameNo( int mcookie, int* nextmkptr, int* nextframenoptr );


	int SetNewPose( CShdHandler* lpsh );

	int SetCurrentMotion( CShdHandler* lpsh, int mcookie, int srcframeno );
	void SetCurrentMotionReq( CMQOMaterial* mqohead, CMotionCtrl* mcptr, int mcookie, int srcframeno, int broflag, int* errorflag );


	int SetMotionKindML( int seri, int motid );
	int SetMotionFrameNoML( CMQOMaterial* mqohead, int seri, int mcookie, int srcframeno, int srcisfirst );
	int SetNextMotionFrameNoML( int seri, int mcookie, int nextmk, int nextframeno, int befframeno );
	int GetMotionFrameNoML( int boneno, int* motidptr, int* framenoptr );
	int GetNextMotionFrameNoML( int boneno, int mcookie, int* nextmkptr, int* nextframenoptr );
	int SetNewPoseML( CShdHandler* lpsh );



	//int LoadMotData( char* fname, char* motname, int* framemax, int* mottype );

	int MakeLeapRoute( int serialno, int* skip2leap, int* leaproute, int skipmax );
	int GetLeapRoute( int serialno, int* leaproute, int leapmax );

	int SetCurrentBSphere( int motkind, int frameno );

	int ChkConfTotalBySphere( CMotHandler* chkmh, int* confflag );

	int GetNextMP( int motid, int boneno, int prevmpid, int* mpidptr );
	int GetMPInfo( int motid, int boneno, int mpid, CQuaternion* dstq, D3DXVECTOR3* dsttra, int* dstframeno, DWORD* dstds, int* dstinterp, D3DXVECTOR3* dstscale, int* dstuserint1 );
	int SetMPInfo( int motid, int boneno, int mpid, CQuaternion* srcqptr, D3DXVECTOR3* srctra, int srcframeno, DWORD srcds, int srcinterp, D3DXVECTOR3* srcscale, int srcuserint1, int* infoflagptr );
	int SetMPInfo( int motid, int boneno, CQuaternion* srcqptr, MPINFO2* mpiptr, int* flagptr );
	int GetBoneQ( CShdHandler* lpsh, int boneno, int motid, int frameno, int kind, CQuaternion* dstq );
	int GetCurrentBoneQ( CShdHandler* lpsh, int boneno, int kind, CQuaternion* dstq );
	int GetCurrentMotionPoint( CShdHandler* srclpsh, int bnoeno, CMotionPoint2* dstmp, int* hasmpflag );


	int IsExistMotionPoint( int motid, int boneno, int frameno, int* mpidptr );
	int IsExistMotionPoint( int motid, int boneno, int frameno, CMotionPoint2** ppmp );
	int IsExistCurrentMotionPoint( int boneno, CMotionPoint2** ppmp );
	int GetMotionFrameLength( int motid, int* lengptr );

	int AddMotionPoint( int motid, int boneno, CQuaternion* srcqptr, D3DXVECTOR3* srctra, D3DXVECTOR3* srcscale, int srcframeno, DWORD srcds, int srcinterp, int srcuserint1, int* mpidptr );
	int DeleteMotionPoint( int motid, int boneno, int mpid );
	int FillUpMotion( CShdHandler* lpsh, int motid, int boneno, int startframe, int endframe, int initflag, int calceulflag = 1 );
	int FillUpMotionML( CShdHandler* lpsh, int motid, int boneno, int startframe, int endframe );


	int CopyMotionFrame( CShdHandler* lpsh, int srcmotid, int srcframe, int dstmotid, int dstframe );
	int CopyMotionFrameML( CShdHandler* lpsh, int seri, int srcmotid, int srcframe, int dstmotid, int dstframe );
	int CopyMMotAnimFrame( int srcseri, CShdHandler* lpsh, int srcmotid, int srcframe, int dstmotid, int dstframe );

	CMotionPoint2* GetFirstMotionPoint( int boneno, int motid );

	int RemakeBSphereData( int motid, int framenum );

	int ExistTexKey( int boneno, int motid, int frameno, CTexKey** pptexkey );
	int CreateTexKey( int boneno, int motid, int frameno, CTexKey** pptexkey );
	CTexKey* GetFirstTexKey( int boneno, int motid );
	int DeleteTexKey( int boneno, int motid, int frameno );
	int LeaveFromChainTexKey( int boneno, int motid, int frameno );
	int ChainTexKey( int boneno, int motid, CTexKey* tkptr );
	int GetTexAnim( int boneno, int motid, int frameno );
	int GetTexAnimKeyframeNoRange( int boneno, int motid, int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );


	int ExistAlpKey( int boneno, int motid, int frameno, CAlpKey** ppalpkey );
	int CreateAlpKey( int boneno, int motid, int frameno, CAlpKey** ppalpkey );
	CAlpKey* GetFirstAlpKey( int boneno, int motid );
	int DeleteAlpKey( int boneno, int motid, int frameno );
	int LeaveFromChainAlpKey( int boneno, int motid, int frameno );
	int ChainAlpKey( int boneno, int motid, CAlpKey* addalpk );
	int GetAlpAnim( int boneno, int motid, int frameno, CMQOMaterial* srcmat, float* alphaptr, int* existflag );
	int GetAlpAnimKeyframeNoRange( int boneno, int motid, int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );


	int ExistDSKey( int boneno, int motid, int frameno, CDSKey** ppdskey );
	int CreateDSKey( int boneno, int motid, int frameno, CDSKey** ppdskey );
	CDSKey* GetFirstDSKey( int boneno, int motid );
	int DeleteDSKey( int boneno, int motid, int frameno );
	int LeaveFromChainDSKey( int boneno, int motid, int frameno );
	int ChainDSKey( int boneno, int motid, CDSKey* dskptr );
	int GetDSAnim( CDispSwitch* dsptr, int boneno, int motid, int frameno );
	int GetDSAnimKeyframeNoRange( int boneno, int motid, int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );


	int HuGetBoneMatrix( int boneno, int motid, int frameno, D3DXMATRIX* bonematptr );
	int HuGetRawMatrix( int boneno, int motid, int frameno, D3DXMATRIX* rawmatptr );
	int HuGetKeyframeSRT( CShdHandler* lpsh, int motid, int boneno, int frameno, D3DXVECTOR3* pscale, D3DXQUATERNION* prot, D3DXVECTOR3* ptrans, int isstandard = 0 );
	int GetKeyframeSRT2( CShdHandler* lpsh, int motid, int boneno, int frameno, D3DXVECTOR3* pscale, D3DXQUATERNION* prot, D3DXVECTOR3* ptrans );
	int HuGetKeyframeNo( int motid, int boneno, int* framearray, int arrayleng, int* framenumptr );
	int GetKeyframeNo( int motid, int boneno, KEYFRAMENO* framearray, int arrayleng, int* framenumptr );
	int GetKeyframeNoRange( int motid, int boneno, int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );



	int DestroyMotionFrame( CShdHandler* lpsh, int motid, int frameno );

	int GetTotalQOnFrame( CShdHandler* lpsh, int motid, int frameno, int boneno, CQuaternion* dstq );
	int GetDispSwitch( CShdHandler* lpsh, int motid, int frameno, DWORD* dswitch );
	int GetDispSwitch2( CShdHandler* lpsh, int* swptr, int leng );

	int SetMotionName( int motid, char* srcname );

	int GetMotionIDByName( char* srcname );
	int SetTickCount( int seri, DWORD srctick );
	int GetFirstTickJoint( int nextflag, int srcmotid, int* dstseri, int* dstframeno );

	int CalcMPEuler( CShdHandler* lpsh, int motid, int zakind );

	int GetZaType( int motid );

	__inline int CMotHandler::MatConvD3DX( D3DXMATRIX* dstmat, CMatrix2 srcmat )
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

		return 0;
	};

	int GetMotionName( int motid, char* motname );
	int SetBoneAxisQ( CShdHandler* srclpsh, int seri, int srczakind, int srcmotid, int srcframeno );

	int InitTexAnim( CShdHandler* lpsh );
	int InitAlpAnim( CShdHandler* lpsh );
	int InitDSAnim( CShdHandler* lpsh );

	int CalcMLMotion( CShdHandler* lpsh );

	int SetMState( CShdHandler* lpsh, CMState* dstms, int bonenum, int* boneno2seri );
	int SetCurMatFromMState( CShdHandler* lpsh, CMState* srcms, int bonenum, int* boneno2seri );

	int GetNotToIdle( int boneno, int* flagptr );
	int SetNotToIdle( int boneno, int srcflag );

	int HasKey( int motid, int frameno, int* flagptr );

private:
	int DestroyMotionFrameOne( CShdHandler* lpsh, int motid, int frameno );
	void CalcMLMatReq( CShdHandler* lpsh, int seri, int broflag );
	void InitMLMatReq( int seri, int broflag );


public:
	static HANDLE	s_hHeap;
	static unsigned int	s_uNumAllocsInHeap;

	int depth;
	int isfirst; // 初回のAddShdElemでクリア
	CTreeHandler2* thandler;

	int s2mot_allocleng;
	int s2mot_leng;
	CMotionCtrl** s2mot;
	
/////////

	int m_curmotkind;
	int m_curframeno;

	int m_kindnum; // motion の種類の数
	//int m_kindno;	// motion の種類の　current値
	//int m_motno; // current motion No.
	int* m_motnum; // 各motkindno に対するmotno の最大値 + 1(要素数)。
	int* m_definterp;//各motkindnoに対するデフォルトのinterpolation
	int* m_zatype;//各motkindnoに対するza。

	//float** m_firstrmag; //半径の２乗
	//D3DXVECTOR3** m_firstcenter;

	CBSphere** m_firstbs;
	CBSphere m_curbs;
	
	int m_fuid;//for moa : fill up motion id
	int m_fuleng;

	CEventKey m_ek;
	int m_nottoidle;

	int m_standard;

	//int m_zakind;
};

#endif