#ifndef GPANIMHANDLERH
#define GPANIMHANDLERH

#include <d3dx9.h>

#include <coef.h>

class CGPAnim;
class CShdHandler;
class CMotHandler;

class CGPAnimHandler
{
public:
	CGPAnimHandler();
	~CGPAnimHandler();

	CGPAnim* GetGPAnim( int animno, int* indexptr = 0 );
	CGPAnim* GetGPAnimByIndex( int srcindex );
	CGPAnim* GetCurGPAnim();

	CGPAnim* AddAnim( char* animname, int animtype, int framenum, int srcmotjump );
	int DestroyAnim( int delanim );

	int SetTotalFrame( int animno, int newtotal );

	int SetCurrentAnim( int animno );
	int GetCurrentAnim( int* animnoptr );

	int SetDefInterp( int srcinterp );
	int GetDefInterp( int* dstinterp );
	int SetNextAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno );
	int GetNextAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr );

	int SetAnimFrameNo( int animno, int srcframeno, int srcisfirst );
	int GetAnimFrameNo( int* animnoptr, int* framenoptr );

	int SetNewGP();

	int CalcFrameData( int animno, CShdHandler* lpsh, CMotHandler* lpmh, D3DXMATRIX* matWorld );
	int SetOffsetMat( D3DXMATRIX srcoff );

	int SetDefaultGPE( GPELEM srcgpe );

private:
	int InitParams();
	int DestroyObjs();

public:
	int m_animnum;
	int m_animno;
	CGPAnim** m_ppgpanim;
	GPELEM m_curgpe;
	GPELEM m_defaultgpe;

	int m_definterp;

	D3DXMATRIX m_offmat;
	int m_offflag;
};

#endif
