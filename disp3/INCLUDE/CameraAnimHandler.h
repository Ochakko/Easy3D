#ifndef CAMERAANIMHANDLERH
#define CAMERAANIMHANDLERH

#include <coef.h>

class CCameraAnim;

class CCameraAnimHandler
{
public:
	CCameraAnimHandler();
	~CCameraAnimHandler();

	CCameraAnim* GetCameraAnim( int animno, int* indexptr = 0 );
	CCameraAnim* GetCameraAnimByIndex( int srcindex );
	CCameraAnim* GetCurCameraAnim();

	CCameraAnim* AddAnim( char* animname, int animtype, int framenum, int srcmotjump );
	int DestroyAnim( int delanim );

	int SetTotalFrame( int animno, int newtotal );

	int SetCurrentAnim( int animno );
	int GetCurrentAnim( int* animnoptr );

	int SetDefInterp( int srcinterp );
	int GetDefInterp( int* dstinterp );
	int SetNextAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno );
	int GetNextAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr );

	int GetAnimFrameNo( int* animnoptr, int* camnoptr, int* framenoptr );

	int SetAnimFrameNo( int animno, int camno, int srcframeno, int srcisfirst );
	int SetNewCamera( int camno );

	int SetDefaultCamera( int camno, CAMERAELEM srcce );


private:
	int InitParams();
	int DestroyObjs();

public:
	int m_animnum;
	int m_animno;
	CCameraAnim** m_ppcamanim;
	CAMERAELEM m_curcamera;
	CAMERAELEM m_defaultcameras[3];

	int m_definterp;
};

#endif
