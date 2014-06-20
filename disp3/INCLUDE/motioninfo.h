
#ifndef MOTIONINFOH
#define MOTIONINFOH

#include <basedat.h>
#include <matrix2.h>
#include <crtdbg.h>

#include <quaternion.h>
#include <motionpoint2.h>

#define	DBGH
#include "dbg.h"

//class CShdTree;

//class CMotFile;
class CShdHandler;
class CShdElem;
class CTexKey;
class CDSKey;
//class CMotionPoint2;
class CDispSwitch;
class CMMotKey;
class CAlpKey;
class CAlpChange;
class CMotionCtrl;
class CMQOMaterial;

class CMotionInfo	: public CBaseDat
{
	// �eCShdTree �Ɏ�������B
	// GetMotion, StepMotion, ���_���Z �̏����ƁAskipjoint���ɁAmotno�̈���������Ȃ�B
	// �̂ŁA�@StepMotion, GetMotion, ���_���Z�̏��Ŏg�p����B

	// SetMotionKind���Ă�ł����΁A�����StepMotion�́A���������悤�ɂ���B


public:
	CMotionInfo();
	~CMotionInfo();

	CMatrix2* operator() ( int srckind, int srcno ){
		CMatrix2* matptr = 0;
#ifdef _DEBUG
		int ret;
		ret = CheckIndex( srckind, srcno );
		if( ret ){
			DbgOut( "CMotionInfo : CheckIndex error %d %d %d %d!!!\n", srckind, srcno, kindnum, *(motnum + srckind) );
			_ASSERT( 0 );
		}
#endif
		matptr = *( firstmot + srckind );

		return ( matptr + srcno );
	};

	CMatrix2* operator() ( int srckind ){
		// motno �́@current�l
		CMatrix2* matptr = 0;
#ifdef _DEBUG
		int ret;
		ret = CheckIndex( srckind, motno );
		_ASSERT( !ret );
#endif
		matptr = *( firstmot + srckind );

		return ( matptr + motno );
	};


	static int CreateTempMP();
	static int DestroyTempMP();


		// motionname ---> motkindno ���̊֐����͍̂��������Ȃ����A
		// �Ԃ�l��cookie �̂悤�Ɏg�����Ƃɂ���āACMatrix2 �ւ̃A�N�Z�X���X���[�Y�ɂ���B
		// �o�^����Ă��Ȃ����O�ɑ΂��ẮA-1 ��Ԃ��B
	int Name2Index( char* srcmotname ); 

	int AddMotion( int srcstandard, int srctype, char* srcname, int srcmottype, int srcmotnum, int srcmotjump ); //���O�̓o�^�Aallocate
	//int SetMotionMatrix( int srcmotkind, int srcmotno, CMatrix2& srcmat );
	int SetMotionMatrix( CMatrix2* transmat, int mcookie, int frameno );
	//int SetJointValue( float srcval, int mcookie, int frameno );
	int RemakeMotionMatrix( int srctype, int srcmotkind, int newtotal );


		// motion��indexno �̑��������w��B
	int SetMotionType( int srcmotkind, int srcmottype );
	int SetMotionJump( int srcmotkind, int srcmotjump );
	int SetMotionStep( int srcmotkind, int srcmotstep );
	int SetInterpolationAll( int srcmotkind, int srcinterpolation );

		// current�f�[�^���w��B
	int SetMotionKind( int srcmotkind );// isfirst, motno ���Z�b�g����B
	int SetMotionNo( int srcmotno );

	int SetMotionFrameNo( int srcmotkind, int srcframeno, int srcisfirst );
	int SetNextMotionFrameNo( int srcmotkind, int nextmk, int nextframeno, int befframeno );

		// �t���f�[�^
	//int SetSeqMax( int srcmotkind, float srcseqmax );
	//int SetSeqStep( int srcmotkind, float seqstep );
	//int SetSeqVal( int srcmotkind, int srcmotno, float srcseqval );
	//int SetJointVal( int srcmotkind, int srcmotno, float srcjointval );


	int GetMotionKindNum();
	int GetMotionNo();

	int GetMotionNum( int srcmotkind );
	int GetMotionType( int srcmotkind );
	int GetMotionJump( int srcmotkind );
	int GetMotionStep( int srcmotkind );
	//float* GetMorphVal( int srcmotkind );

	char* Index2Name( int srcmotkind ); // for debug

	CMotionPoint2* GetMotionPoint( int srcmotkind );
	CMotionPoint2* GetMotionPoint2( int srcmotkind, int srcmpid );
	CMotionPoint2* IsExistMotionPoint( int srcmotkind, int srcframeno );
	int CalcMotionPointOnFrame( CQuaternion* axisq, CShdElem* selem, CMotionPoint2* dstmp, int srcmotid, int srcframe, int* hasmpflag );



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


	int DeleteMotionPoint( int srcmotkind, int srcframeno );
	CMotionPoint2* LeaveFromChainMP( int srcmotkind, int srcframeno );

	int GetDispSwitch( DWORD* dstdw, int srcmotkind, int srcframeno );

	int GetInterpolation( int* dstinterp, int srcmotkind, int srcframeno );//mp����interp���擾
//	int GetInterpolation( int srcmotkind );//���o�[�W����


	int ExistTexKey( int motid, int frameno, CTexKey** pptexkey );
	int CreateTexKey( int motid, int frameno, CTexKey** pptexkey );
	CTexKey* GetFirstTexKey( int motid );
	int DeleteTexKey( int motid, int frameno );
	CTexKey* LeaveFromChainTexKey( int motid, int frameno );
	int ChainTexKey( int motid, CTexKey* tkptr );
	int DeleteTexKeyOutOfRange( int motid, int srcmaxframe );
	int SetTexAnim();
	int GetTexAnim( int motid, int frameno );


	int ExistAlpKey( int motid, int frameno, CAlpKey** ppalpkey );
	int CreateAlpKey( int motid, int frameno, CAlpKey** ppalpkey );
	CAlpKey* GetFirstAlpKey( int motid );
	int DeleteAlpKey( int motid, int frameno );
	CAlpKey* LeaveFromChainAlpKey( int motid, int frameno );
	int ChainAlpKey( int motid, CAlpKey* addalpk );
	int DeleteAlpKeyOutOfRange( int motid, int srcmaxframe );
	int SetAlpAnim( CMQOMaterial* srcmathead );
	int GetAlpAnim( int motid, int frameno, CMQOMaterial* srcmat, float* alphaptr, int* existflag );
	int FillUpAlpha( CAlpKey* befkey, CAlpChange* befchange, CAlpKey* nextkey, CAlpChange* nextchange, 
		int frameno, float* alphaptr );


	int ExistDSKey( int motid, int frameno, CDSKey** ppdskey );
	int CreateDSKey( int motid, int frameno, CDSKey** ppdskey );
	CDSKey* GetFirstDSKey( int motid );
	int DeleteDSKey( int motid, int frameno );
	CDSKey* LeaveFromChainDSKey( int motid, int frameno );
	int ChainDSKey( int motid, CDSKey* dskptr );
	int DeleteDSKeyOutOfRange( int motid, int srcmaxframe );
	int SetDSAnim( CShdHandler* lpsh );
	int GetDSAnim( CDispSwitch* dsptr, int motid, int frameno );


	int ExistMMotKey( int motid, int frameno, CMMotKey** ppmmkey );
	int CreateMMotKey( int motid, int frameno, CMMotKey** ppmmkey );
	CMMotKey* GetFirstMMotKey( int motid );
	int DeleteMMotKey( int motid, int frameno );
	int DeleteMMotKey( int motid, int frameno, CShdElem* srcbase );

	CMMotKey* LeaveFromChainMMotKey( int motid, int frameno );
	int ChainMMotKey( int motid, CMMotKey* mmkptr );
	int DeleteMMotKeyOutOfRange( int motid, int srcmaxframe );
	int SetMMotAnim( CMotionCtrl* mcptr );
	int GetMMotAnim( float* valptr, int motid, int frameno, CShdElem* srcbase, CShdElem* srctarget, int* existflag );
	int SetMMotValue( int motid, int frameno, CShdElem* srcbase, CShdElem* srctarget, float srcval );
	int GetMMotAnimKeyframeNoRange( int motid, int startframe, int endframe, int* keyframeptr, int arrayleng, int* getnum );
	int GetMMotAnimKeyAll( int motid, CShdElem* baseelem, CShdElem* targetelem,
			E3DMORPHMOTION* motionptr, int arrayleng, int* getnum );


	int MakeRawmat( int srcmotkind, CShdElem* lpselem, int srcstart, int srcend, CShdHandler* lpsh, CMotHandler* lpmh, int calceulflag = 1 );
	int MakeRawmatCap( int mcookie, CShdElem* lpselem, int srcframe, CShdHandler* lpsh, CMotHandler* lpmh, CMotionPoint2* srcmp );

	int FillUpMotionPoint( CQuaternion* axisq, CMotionPoint2* srcstartmp, CMotionPoint2* srcendmp, 
		CMotionPoint2* dstmp, int srcmpnum, int srcinterp );
	int FillUpMotionPointOnFrame( CQuaternion* axisq, CMotionPoint2* srcstartmp, CMotionPoint2* srcendmp, CMotionPoint2* dstmp, int srcframe );
	int FillUpMMotOnFrame( CMMotKey* srcstartmmk, CMMotKey* srcendmmk, float* valptr, int srcframe, CShdElem* srcbase, CShdElem* srctarget );
	


	int PutQuaternionInOrder( int srcmotkind );

	//int MPtoRawmat( int srcmotkind, CMotionPoint2* srcmp, int srcmatnum, CShdElem* lpselem );
	int MPtoRawmat( int srcmotkind, CMotionPoint2* srcmp, int srcstart, int srcend, CShdElem* lpselem );

	int DestroyMotionObj( int delcookie = -1 );

		// current�f�[�^���擾
	CMatrix2* GetCurMotion();

		// motno ���@�i�߂� ( ����motkind ���ł̑J�ځB)
	CMatrix2* StepMotion( int nextno );// isfirst == 1 �̂Ƃ��́Astep���Ȃ��ŁAisfirst ���O�ɃZ�b�g���邾���B
	//float StepJoint( int nextno );
	DWORD StepDispSwitch( int nextno );
	//int GetNextNo();
	int GetNextMotion( NEXTMOTION* pnm, int getonly = 0 );

		// ���O�ȊO�̃����o���R�s�[
	int CopyMotion( int dstmotkind, int srcmotkind );
	
	int MultMotionMatrix( CMotionInfo* srcmotinfo, int mcookie, CMatrix2 multmat );
	int CopyMotionMatrix( CMotionInfo* srcmotinfo, int mcookie );
	int CopyMotionMatrix( CMatrix2* srcmat, int mcookie, int matnum );
	//int CopyJointVal( float* srcval, int mcookie, int valnum );
	int MultCopyMotionMatrix( CMotionInfo* srcmotinfo, CMatrix2 multmat, int mcookie );

	void InitParams();
	void DestroyObjs();
	//int DumpMem( HANDLE hfile, int tabnum, char* header );
	int DumpMem( HANDLE hfile, int mcookie );
	//int DumpMem( CMotFile* mfile, int mcookie );
	//int DumpJointVal( CMotFile* mfile, int mcookie, int srcseri );


	int CopyMotionFrame( CShdElem* selem, int srcmotid, int srcframe, int dstmotid, int dstframe,
		CShdHandler* lpsh, CMotHandler* lpmh );
	int CopyMMotAnimFrame( CMotionCtrl* mcptr, int srcmotid, int srcframe, int dstmotid, int dstframe );


	CMotionPoint2* GetSmallMotionPoint( int srcmotkind, int srcframe );
	CMotionPoint2* GetNearMotionPoint( int srcmotkind, int srcframe );
	CMotionPoint2* GetLargeMotionPoint( int srcmotkind, int srcframe );

	int GetCurrentRawMat( CMatrix2* rawmptr );
	int SetMotionName( int motid, char* srcname );

	int CalcMPEuler( CMotHandler* lpmh, CShdHandler* lpsh, CShdElem* selem, int motid, int zakind );
	int ScaleSplineMv( int motid, D3DXVECTOR3 srcmult );

	int StepStop( NEXTMOTION* pnm )
	{
		pnm->mk = kindno;
		pnm->aftframeno = motno;
		return 0;
	};
	int StepClamp( NEXTMOTION* pnm )
	{
		int motmax, curstep, nextno;

		motmax = *(motnum + kindno) - 1;
		curstep = *(motstep + kindno);
		
		nextno = motno + curstep;

		if( nextno > motmax ){
			if( (nextmot + kindno)->mk == -1 ){
				pnm->mk = kindno;
				pnm->aftframeno = motmax;
			}else{
				*pnm = *(nextmot + kindno);
			}
		}else if( nextno < 0 ){
			pnm->mk = kindno;
			pnm->aftframeno = 0;
		}else{
			if( ((nextmot + kindno)->mk != -1) && ((nextmot + kindno)->befframeno != -1) && (nextno > (nextmot + kindno)->befframeno) ){
				*pnm = *(nextmot + kindno);
			}else{
				pnm->mk = kindno;
				pnm->aftframeno = nextno;
			}
		}

		return 0;
	};
	int StepRound( NEXTMOTION* pnm )
	{
		int motmax, curstep, nextno;

		motmax = *(motnum + kindno) - 1;
		curstep = *(motstep + kindno);
		
		nextno = motno + curstep;
		if( nextno > motmax ){
			if( (nextmot + kindno)->mk == -1 ){
				pnm->mk = kindno;
				pnm->aftframeno = 0;
			}else{
				*pnm = *(nextmot + kindno);
			}
		}else if( nextno < 0 ){
			pnm->mk = kindno;
			pnm->aftframeno = motmax;
		}else{
			if( ((nextmot + kindno)->mk != -1) && ((nextmot + kindno)->befframeno != -1) && (nextno > (nextmot + kindno)->befframeno) ){
				*pnm = *(nextmot + kindno);
			}else{
				pnm->mk = kindno;
				pnm->aftframeno = nextno;
			}
		}
		
		return 0;
	};
	int StepInv( NEXTMOTION* pnm )
	{
		//static int stepdir = 1;
		int motmax, curstep, nextno;

		motmax = *(motnum + kindno) - 1;
		curstep = *(motstep + kindno);
		
		nextno = motno + m_stepdir * curstep;

		if( nextno > motmax ){
			if( (nextmot + kindno)->mk == -1 ){
				pnm->mk = kindno;
				pnm->aftframeno = motmax;
			}else{
				*pnm = *(nextmot + kindno);
			}
			m_stepdir *= -1;
		}else if( nextno < 0 ){
			pnm->mk = kindno;
			pnm->aftframeno = 0;
			m_stepdir *= -1;
		}else{
			if( ((nextmot + kindno)->mk != -1) && ((nextmot + kindno)->befframeno != -1) && (nextno > (nextmot + kindno)->befframeno) ){
				*pnm = *(nextmot + kindno);
			}else{
				pnm->mk = kindno;
				pnm->aftframeno = nextno;
			}
		}

		return 0;
	};
	int StepJump( NEXTMOTION* pnm )
	{
		int motmax, curstep, nextno;

		motmax = *(motnum + kindno) - 1;
		curstep = *(motstep + kindno);
		
		nextno = motno + curstep;
		if( nextno > motmax ){
			if( (nextmot + kindno)->mk == -1 ){
				pnm->mk = kindno;
				pnm->aftframeno = *(motjump + kindno);
			}else{
				*pnm = *(nextmot + kindno);
			}
		}else if( nextno < 0 ){
			pnm->mk = kindno;
			pnm->aftframeno = motmax;
		}else{
			if( ((nextmot + kindno)->mk != -1) && ((nextmot + kindno)->befframeno != -1) && (nextno > (nextmot + kindno)->befframeno) ){
				*pnm = *(nextmot + kindno);
			}else{
				pnm->mk = kindno;
				pnm->aftframeno = nextno;
			}
		}
		
		return 0;
	};

	int StepDummy( NEXTMOTION* pnm )	
	{
		DbgOut( "CMotionInfo : StepFunc called StepDummy warning !!!\n" );
		pnm->mk = kindno;
		pnm->aftframeno = motno;
		return 0;
	};


public:
	CMotionPoint* m_capmp;

	int kindnum; // motion �̎�ނ̐�
	int kindno;	// motion �̎�ނ́@current�l
	int motno; // current motion No.
	int isfirst;

	char** motname; // �emotkind �� motionname�B 
	int* motnum; // �emotkindno �ɑ΂���motno �̍ő�l + 1(�v�f��)�B
	int* mottype; // �emotkind �� motiontype
	int* motstep; // �emotkind �� motno �̕ω���
	int* motjump;// �emotkind �� �W�����v��t���[���ԍ��iMOTION_JUMP���j

	NEXTMOTION* nextmot;//�emotkind�̍ŏI�t���[���ԍ��̌�̃��[�V����


	//unsigned long* firstmot; // �emotkind�̐擪��CMatrix2* ���i�[����B
	CMatrix2** firstmot;
	CMatrix2** rawmat; //�e�s��̊|�����Ă��Ȃ�matrix

	CMatrix2 mlmat;// �}���`���C���[���[�V�����p�A�e�̂��������s��B�e�{�[���̃��[�V�������m�肵�Ă��烊�A���^�C���Ōv�Z����B


//	float** firstval; //SHDMORPH����joint�̒l


	CMotionPoint2** firstmp;
	CTexKey** firsttex;
	CAlpKey** firstalp;
	CDSKey** firstds;
	CMMotKey** firstmorph;

	CMotionPoint2 m_initmp;

	// ����index�݂̂Ő��䂷��̂ŁA
	// ���܂̂Ƃ���g��Ȃ���(���̂S��)�A�ꉞshade���瓾������Ȃ̂ŁA�i�[���Ă����B
	//float* seqmax;
	//float* seqstep;
	//float** seqval;
	//float** jointval;

	DWORD tick;
	int m_standard;

private: 
	int m_stepdir;// StepInv�Ŏg�p�B
	
	
	// motno ����
	void InitStepFunc();
	int (CMotionInfo::*StepFunc[MOTIONTYPEMAX + 1])( NEXTMOTION* pnm );
	/***
	int StepStop();
	int StepClamp();
	int StepRound();
	int StepInv();
	int StepDummy();	
	***/

	int CheckIndex( int srckind, int srcno )
	{
		if( (srckind >= 0) && (srckind < kindnum) && (srcno >= 0) && (srcno < *(motnum + srckind)) ){
			return 0;
		}else{
			return 1;
		}
	}
	int CheckIndex( int srckind )
	{
		if( (srckind >= 0) && (srckind < kindnum) )
			return 0;
		else
			return 1;
	}

	void InitMotParams();
};

#endif





