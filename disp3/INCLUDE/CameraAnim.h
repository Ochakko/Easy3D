#ifndef CAMERAANIMH
#define CAMERAANIMH

#include <coef.h>

class CCameraKey;
class CCameraSWKey;

class CCameraAnim
{
public:
	CCameraAnim( char* srcname, int srcmotkind, CAMERAELEM* defcamptr, int* definterptr );
	~CCameraAnim();


	int ExistCameraKey( int camno, int frameno, CCameraKey** ppck );
	int CreateCameraKey( int camno, int frameno, CCameraKey** ppck );
	CCameraKey* GetFirstCameraKey( int camno );
	int DeleteCameraKey( int camno, int frameno );
	CCameraKey* LeaveFromChainCameraKey( int camno, int frameno );
	int ChainCameraKey( int camno, CCameraKey* addck );
	int DeleteCameraKeyOutOfRange( int camno, int srcmaxframe );
	int SetCameraAnim();
	int SetCameraKey( int camno, int frameno, CAMERAELEM srcce, int srcinterp );
	int GetCameraAnim( CAMERAELEM* ceptr, int* interpptr, int camno, int frameno, int* existflag );
	int GetCameraKeyframeNoRange( int camno, int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );


	int ExistSWKey( int frameno, CCameraSWKey** ppcswk );
	int CreateSWKey( int frameno, CCameraSWKey** ppcswk );
	CCameraSWKey* GetFirstSWKey();
	int DeleteSWKey( int frameno );
	CCameraSWKey* LeaveFromChainSWKey( int frameno );
	int ChainSWKey( CCameraSWKey* addcswk );
	int DeleteSWKeyOutOfRange( int srcmaxframe );
	int SetSWAnim();
	int SetSWKey( int frameno, int srccamerano );
	int GetSWAnim( int* camnoptr, int frameno, int* existflag );
	int GetSWKeyframeNoRange( int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );


	int SetCameraAnimNo( int camno, int frameno, int srcisfirst );
	int GetNextCamera( NEXTMOTION* pnm, int getonly );
	int StepCamera( int camno, int frameno );

	int SetAnimName( char* srcname );
	int GetAnimName( char* dstname );

	int SetFillUpMode( int camno, int frameno, int setfu );
	int SetTargetHsidAll( int srchsid );


private:
	int InitParams();
	int DestroyObjs();

	int FillUpCameraOnFrame( int camno, CCameraKey* srcstartck, CCameraKey* srcendck, CAMERAELEM* dstptr, int srcframeno );
	int FillUpCameraElem( CCameraKey* startck, CCameraKey* endck, CAMERAELEM* dstce, int framenum, int framecnt, int interp );

	int SplineF( float val1, float val2, float val3, float val4, float* dstptr, float t );
	int SplineVec3( D3DXVECTOR3 befv, D3DXVECTOR3 startv, D3DXVECTOR3 endv, D3DXVECTOR3 aftv, D3DXVECTOR3* dstv, float t );

	void InitStepFunc();
	int (CCameraAnim::*StepFunc[MOTIONTYPEMAX + 1])( NEXTMOTION* pnm );
	int StepStop( NEXTMOTION* pnm );
	int StepClamp( NEXTMOTION* pnm );
	int StepRound( NEXTMOTION* pnm );
	int StepInv( NEXTMOTION* pnm );
	int StepJump( NEXTMOTION* pnm );
	int StepDummy( NEXTMOTION* pnm );	

public:
	char m_animname[256];
	int m_motkind;

	int m_camerano;
	int m_frameno;
	CAMERAELEM m_curcamera;
	int m_curinterp;
	CAMERAELEM* m_defcamptr;
	int* m_definterptr;

	int m_isfirst;
	int m_maxframe;
	int m_animtype;
	int m_animstep;
	int m_stepdir;
	int m_motjump;
	NEXTMOTION m_nextmot;
	
	CCameraKey* m_firstkey[CAMERANUMMAX];
	CCameraSWKey* m_firstsw;

};


#endif


