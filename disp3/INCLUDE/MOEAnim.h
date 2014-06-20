#ifndef MOEANIMH
#define MOEANIMH

#include <coef.h>

class CMOEKey;
class CMState;
class CShdHandler;
class CMotHandler;
class CMCHandler;

class CMOEAnim
{
public:
	CMOEAnim( char* srcname, int srcmotkind );
	~CMOEAnim();

	int ExistMOEKey( int frameno, CMOEKey** ppmoek );
	int ExistBefMOEKey( int frameno, CMOEKey** ppmoek );

	int CreateMOEKey( int frameno, CMOEKey** ppmoek );
	CMOEKey* GetFirstMOEKey();
	CMOEKey* LeaveFromChainMOEKey( int frameno );
	int ChainMOEKey( CMOEKey* addmoek );
	int DeleteMOEKeyOutOfRange( int srcmaxframe );
	int SetMOEAnim();
	int SetKeyIdlingName( int frameno, char* idlingname );
	int AddMOEE( int frameno, MOEELEM* srcmoee );
	int DeleteMOEE( int frameno, int delindex );
	int SetMOEKey( int frameno, CMOEKey* srcmoek );
	int DeleteMOEKey( int frameno );


	int GetMOEAnim( CMState** ppms, int frameno );

	int GetMOEKeyframeNoRange( int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );


	int SetMOEAnimNo( int frameno, int srcisfirst );
	int GetNextMOE( NEXTMOTION* pnm, int getonly );
	int StepMOE( int frameno );

	int SetAnimName( char* srcname );
	int GetAnimName( char* dstname, int arrayleng );

	int CreateFrameData( CShdHandler* lpsh );
	int CalcFrameData( CShdHandler* lpsh, CMotHandler* lpmh, CMCHandler* lpmch );

	int CheckAndDelInvalid( CShdHandler* lpsh, int frameno, CMOEKey** ppmoek );//seriチェック。不正データ削除。

	int InitFrameData();
	int GetLocalMotionFrameNo( int moeframeno, int* lanimno, int* lframeno );
	int GetLocalNextMotionFrameNo( int moeframeno, int* lanimno, int* lframeno );
	int GetLocalMotionFrameNoML( CShdHandler* lpsh, int moeframeno, int boneid, int* lanimno, int* lframeno );
	int GetLocalNextMotionFrameNoML( CShdHandler* lpsh, int moeframeno, int boneid, int* lanimno, int* lframeno );

private:
	int InitParams();
	int DestroyObjs();
	int DestroyAllKeys();
	int DestroyFrameData();
	int DestroyBoneData();

	void InitStepFunc();
	int (CMOEAnim::*StepFunc[MOTIONTYPEMAX + 1])( NEXTMOTION* pnm );
	int StepStop( NEXTMOTION* pnm );
	int StepClamp( NEXTMOTION* pnm );
	int StepRound( NEXTMOTION* pnm );
	int StepInv( NEXTMOTION* pnm );
	int StepJump( NEXTMOTION* pnm );
	int StepDummy( NEXTMOTION* pnm );	

	int SetBoneno2Seri( CShdHandler* lpsh, int* boneno2seri, int* seri2boneno, int srcleng, int* getnumptr );

	int SetNewPoseByMOAML( MOEELEM* pmoee, CShdHandler* shandler, CMotHandler* mhandler, CMCHandler* mch );
	int SetMLList( CShdHandler* shandler, int listnum, int* listptr, int notlistnum, int* notlistptr );
	void SetMLNotListReq( CShdHandler* shandler, int srcnot, int broflag );
	void SetMLListReq( CShdHandler* shandler, int srclist, int broflag );


public:
	char m_animname[256];
	int m_motkind;

	int m_frameno;
	CMState* m_curms;//framedataの該当フレームのCMStateへのポインタ
	MOEELEM m_defmoee;
//	CMState* m_defmoeeptr;

	int m_isfirst;
	int m_maxframe;
	int m_animtype;
	int m_animstep;
	int m_stepdir;
	int m_motjump;
	NEXTMOTION m_nextmot;
	
	CMOEKey* m_firstkey;
	CMState* m_framedata;//maxframe + 1フレーム分のCMStateの配列
	int m_framedataleng;

	int m_bonenum;
	int* m_boneno2seri;
	int* m_seri2boneno;

	int m_mlleng;
	int* m_mllist;

	int m_dsfillupmode;
	int m_texfillupmode;
};


#endif


