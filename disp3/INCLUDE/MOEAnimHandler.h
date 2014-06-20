#ifndef MOEANIMHANDLERH
#define MOEANIMHANDLERH

#include <coef.h>

class CMOEAnim;
class CShdHandler;
class CMotHandler;
class CMCHandler;
class CMState;

class CMOEAnimHandler
{
public:
	CMOEAnimHandler();
	~CMOEAnimHandler();

	CMOEAnim* GetMOEAnim( int animno, int* indexptr = 0 );
	CMOEAnim* GetMOEAnimByIndex( int srcindex );
	CMOEAnim* GetCurMOEAnim();

	CMOEAnim* AddAnim( CShdHandler* lpsh, char* animname, int animtype, int framenum, int srcmotjump );
	int DestroyAnim( int delanim );

	int SetTotalFrame( CShdHandler* lpsh, int animno, int newtotal );

	int SetCurrentAnim( int animno );
	int GetCurrentAnim( int* animnoptr );

	int SetNextAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno );
	int GetNextAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr );

	int SetAnimFrameNo( CShdHandler* lpsh, CMotHandler* lpmh, int animno, int srcframeno, int srcisfirst );
	int GetAnimFrameNo( int* animnoptr, int* framenoptr );

	int SetNewMOE( CShdHandler* lpsh, CMotHandler* lpmh );

	int CalcFrameData( int animno, CShdHandler* lpsh, CMotHandler* lpmh, CMCHandler* lpmch );

private:
	int InitParams();
	int DestroyObjs();

public:
	int m_animnum;
	int m_animno;
	CMOEAnim** m_ppmoeanim;
	CMState* m_curms;
};

#endif
