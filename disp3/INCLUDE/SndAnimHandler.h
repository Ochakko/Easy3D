#ifndef SNDANIMHANDLERH
#define SNDANIMHANDLERH

#include <coef.h>

class CSndAnim;
class CSndFrame;
class CSoundSet;

class CSndAnimHandler
{
public:
	CSndAnimHandler();
	~CSndAnimHandler();

	CSndAnim* GetSndAnim( int animno, int* indexptr = 0 );
	CSndAnim* GetSndAnimByIndex( int srcindex );
	CSndAnim* GetCurSndAnim();

	CSndAnim* AddAnim( char* animname, int srcfps, CSoundSet* srcss );
	int DestroyAnim( int delanim );
	int SetTotalFrame( int animno, int newtotal );

	int SetCurrentAnim( int animno );
	int GetCurrentAnim( int* animnoptr );

	int SetAnimFrameNo( int animno, int srcframeno );
	int GetAnimFrameNo( int* animnoptr, int* framenoptr );

	int CalcFrameData( int animno, CSoundSet* pss );

private:
	int InitParams();
	int DestroyObjs();

public:
	int m_animnum;
	int m_animno;
	CSndAnim** m_ppsndanim;
	CSndFrame* m_cursf;
};

#endif