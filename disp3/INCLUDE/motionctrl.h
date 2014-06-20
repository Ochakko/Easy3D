#ifndef MOTIONCTRLH
#define MOTIONCTRLH

//#include <shddata.h>
#include <vecmesh.h>
#include <matrix2.h>
#include <motioninfo.h>

#include <treehandler2.h>
#include <mothandler.h>

#include <quaternion.h>

#include <MMotElem.h>

#define MC_BLOCKLENG	50

class CShdHandler;
class CShdElem;
class CShdTree;
//class CMotHandler;
class CMotFile;
class CMotionPoint;
class CMotionPoint2;
class CD3DDisp;
class CMOQMaterial;

class CMotionCtrl : public CBaseDat
{
public:

	
	//void	*operator new( size_t size );
	//void	operator delete( void *p );

	CMotionCtrl( CTreeHandler2* lphandler, int srcserino );
	~CMotionCtrl();


	// ������
	int ResetParams( CShdTree* ownertree );
	int SetMatrix( CMatrix2* srcmat, __int64 setflag ); // jointmat �ȊO�B
	int InitMatrix();
	
	int CopyChain( CMotHandler* mothandler, CTreeElem2* curte );

	void CalcDepthReq( int srcdepth );

	// debug
	int DumpCurMat( HANDLE hfile );
	int DumpPartMat( HANDLE hfile );
	//int DumpMotion( HANDLE hfile, int mcookie );
	//int DumpMotion( CMotFile* mfile, int mcookie );//binary
	//int DumpJointVal( CMotFile* mfile, int mcookie );


	int	DumpTreeReq( HANDLE hfile, int level, int dumpflag );
	// DumpMatrix �́@ownerptr->DumpMatrix ���g���܂킷�B

	int AddMotion( int srcstandard, char* motionname, int motiontype, int framemax, int srcmotjump );
	int SetMotionMatrix( CMatrix2* transmat, int motcookie, int frameno );
	//int SetJointValue( float srcval, int motcookie, int frameno );
	int SetHasMotion( int srchasmot );
	int GetHasMotion();
	int RemakeMotionMatrix( int srcmotkind, int newtotal );

	int CalcNoJointMatrix( CMotionCtrl* srcctrl, int mcookie );
	int CopyMotionMatrix( CMotionCtrl* srcctrl, int mcookie );
	int CopyMotionMatrix( CMatrix2* srcmat, int mcookie, int matnum );
	int CopyJointVal( float* srcval, int mcookie, int valnum );
	int MultCopyMotionMatrix( CMotionCtrl* srcctrl, CMatrix2 multmat, int mcookie );

	int SetMotionKind( int mcookie );
	int SetMotionStep( int srcmotkind, int srcmotstep );
	int SetMotionType( int srcmotkind, int srcmottype );
	int SetMotionJump( int srcmotkind, int srcmotjump );
	int SetInterpolationAll( int srcmotkind, int srcinterpolation );
	int PutQuaternionInOrder( int srcmotkind );

	int SetMotionFrameNo( CMQOMaterial* mqohead, int mcookie, int srcframeno, int srcisfirst );
	int SetNextMotionFrameNo( int mcookie, int nextmk, int nextframeno, int befframeno );

	int StepMotion( CMQOMaterial* mqohead, int nextno );
	//int GetNextNo();
	int GetNextMotion( NEXTMOTION* nextmot, int getonly = 0 );

	int SetCurrentMotion( CMQOMaterial* mqohead, int mcookie, int srcframeno );

	int GetDispSwitch( DWORD* dstdw, int srcmotkind, int srcframeno );
	int GetInterpolation( int* dstinterp, int srcmotkind, int srcframeno );

	CMotionPoint2* GetMotionPoint( int srcmotkind );
	CMotionPoint2* GetMotionPoint2( int srcmotkind, int mpid );
	int CalcMotionPointOnFrame( CShdElem* selem, CMotionPoint2* dstmp, int srcmotid, int srcframe, int* hasmpflag, CShdHandler* lpsh, CMotHandler* lpmh );



	CMotionPoint2* IsExistMotionPoint( int srcmotkind, int srcframeno );
	//CMotionPoint2* AddMotionPoint( int srcmotkind, int srcframeno,
	//	float srcrotx, float srcroty, float srcrotz,
	//	float srcmvx, float srcmvy, float srcmvz );
	CMotionPoint2* AddMotionPoint( int srcmotkind, int srcframeno,
		CQuaternion srcq,
		float srcmvx, float srcmvy, float srcmvz, DWORD srcdswitch, int srcinterp, float scalex, float scaley, float scalez, int userint1 );
	CMotionPoint2* AddMotionPoint( int srcmotkind, int srcframeno,
		CQuaternion srcq,
		float srcmvx, float srcmvy, float srcmvz, DWORD srcdswitch, int srcinterp, float scalex, float scaley, float scalez, int userint1, 
		D3DXVECTOR3 srceul );


	CMotionPoint2* AddMotionPoint( int srcmotkind, CMotionPoint* srcmp, int srcinterp );//ver1.0.0.9�ȑO�̃t�@�C���p
	CMotionPoint2* AddMotionPoint( int srcmotkind, CMotionPoint2* srcmp, int srcinterp );//ver1.0.1.0�ȍ~

	int DeleteMotionPoint( int srcmotkind, int srcframeno );
	int LeaveFromChainMP( int srcmotkind, int srcframeno );
	int DeleteMpOutOfRange( int srcmotkind, int maxno );

	int MakeRawmat( int mcookie, CShdElem* lpselem, int srcstart, int srcend, CShdHandler* lpsh, CMotHandler* lpmh, int calceulflag = 1 );
	int MakeRawmatCap( int mcookie, CShdElem* lpselem, int srcframe, CShdHandler* lpsh, CMotHandler* lpmh, CMotionPoint2* srcmp );

	int RawmatToMotionmat( int mcookie, CMotionCtrl* parmc, int srcstart, int srcend );
	int RawmatToMotionmatCap( int mcookie, CMotionCtrl* parmc, int srcframe );
	
	int DestroyMotionObj( int delcookie = -1 );


	int IsJoint();

	//int GetMotionFrameNum( int motid, int* framenum );
	int CopyMotionFrame( CShdElem* selem, int srcmotid, int srcframe, int dstmotid, int dstframe, CShdHandler* lpsh, CMotHandler* lpmh );
	int GetMotionJump( int motid, int* motjumpptr );

	int SetMotionName( int motid, char* srcname );	
	int CalcMLMat( CMotionCtrl* parmc );
	
	int AddCurMMotElem( CShdElem* srcbaseelem );
	int DeleteCurMMotElem( CShdElem* srcbaseelem );
	CMMotElem* GetCurMMotElem( CShdElem* srcbaseelem );
	int ScaleSplineMv( int motid, D3DXVECTOR3 srcmult );

	__inline int CMotionCtrl::MatConvD3DX( D3DXMATRIX* dstmat, CMatrix2 srcmat )
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

private:

	int InitParams( CTreeHandler2* lphandler );

	int CreateObjs();
	void DestroyObjs();

public:

	int serialno;
	int depth;
	int brono;
	int bronum; // brono == 0 �̃����o�����Ӗ������B

	int hasmotion;// �Ή�����shdelem���Amotion_interface�������Ă���Ƃ��́@�P�B
	int haschain; // SetChain���������Ă��邩�ǂ����B

	int boneno;
	int oldboneno;//SIGMAGICNO_5�ȑO��boneno (endjoint������)
	int morphno;

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//skip joint�̐��B
	// (hasmotion == 1)��elem�̂�skip���Ƃ��Đ�����̂ŁAmeshinfo.skip�Ƃ͒l���قȂ�B
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int leap; 


	ALIGNED CMatrix2 curmat;
	CMatrix2 partmat;
	float curval;

	int curmmenum;
	CMMotElem** curmmotelem;//morph�̃J�����g�o�����[�B�x�[�X�̐������̔z��B


	//D3DXMATRIX curwvmat;// curmat * scale * world * view


	DWORD curdispswitch;

	CTreeHandler2* lpthandler;

	//CMatrix2 partmat;

		// �����̃��[�V������jointmat �i�[�A�Ǘ��p
	CMotionInfo* motinfo;
	//int* motcookie;

	CMotionCtrl* parent;
	CMotionCtrl* brother;
	CMotionCtrl* sister;
	CMotionCtrl* child;

	/***
	static HANDLE s_hHeap;
	static unsigned int	s_uNumAllocsInHeap;

	static unsigned int s_DataNo;
	static void*	s_DataHead;

	static BYTE*	s_HeadIndex[2048];// s_DataHead���i�[����z��
	static int		s_HeadNum;// s_HeadIndex�̗L���f�[�^�̐��B

	static int	s_useflag[2048][TE2_BLOCKLENG];
	***/

	int curframeno;
	int m_nottoidle;
	int m_standard;

	CShdHandler* m_lpsh;
};

#endif
