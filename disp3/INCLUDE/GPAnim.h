#ifndef GPANIMH
#define GPANIMH

#include <d3dx9.h>

#include <coef.h>

class CGPKey;
class CShdHandler;
class CMotHandler;

class CGPAnim
{
public:
	CGPAnim( char* srcname, int srcmotkind, GPELEM* defgpptr, int* definterptr, D3DXMATRIX* offmatptr, int* offflagptr );
	~CGPAnim();

	int ExistGPKey( int frameno, CGPKey** ppgpk );
	int CreateGPKey( int frameno, CGPKey** ppgpk );
	CGPKey* GetFirstGPKey();
	int DeleteGPKey( int frameno );
	CGPKey* LeaveFromChainGPKey( int frameno );
	int ChainGPKey( CGPKey* addgpk );
	int DeleteGPKeyOutOfRange( int srcmaxframe );
	int SetGPAnim();
	int SetGPKey( int frameno, GPELEM srcgpe, int srcinterp );
	int GetGPAnim( GPELEM* gpeptr, int* interpptr, int frameno, int* existflag );
	//int CalcGPAnim( GPELEM* gpeptr, int* interpptr, int srcframeno, int* existflag );

	int GetGPKeyframeNoRange( int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );


	int SetGPAnimNo( int frameno, int srcisfirst );
	int GetNextGP( NEXTMOTION* pnm, int getonly );
	int StepGP( int frameno );

	int SetAnimName( char* srcname );
	int GetAnimName( char* dstname );

	int SetFillUpMode( int frameno, int setfu );

	int CreateFrameData( int framenum );
	int CalcFrameData( CShdHandler* lpsh, CMotHandler* lpmh, D3DXMATRIX* matWorld );

private:
	int InitParams();
	int DestroyObjs();
	int DestroyFrameData();

//	int FillUpGPOnFrame( CGPKey* srcstartgpk, CGPKey* srcendgpk, GPELEM* dstptr, int srcframeno );
	int FillUpGPElem( CGPKey* startgpk, CGPKey* endgpk, GPELEM* dstgpe, int framenum, int framecnt, int interp,
		CShdHandler* lpsh, CMotHandler* lpmh, D3DXMATRIX* matWorld );

	int SplineF( float val1, float val2, float val3, float val4, float* dstptr, float t );
	int SplineVec3( D3DXVECTOR3 befv, D3DXVECTOR3 startv, D3DXVECTOR3 endv, D3DXVECTOR3 aftv, D3DXVECTOR3* dstv, float t );

	void InitStepFunc();
	int (CGPAnim::*StepFunc[MOTIONTYPEMAX + 1])( NEXTMOTION* pnm );
	int StepStop( NEXTMOTION* pnm );
	int StepClamp( NEXTMOTION* pnm );
	int StepRound( NEXTMOTION* pnm );
	int StepInv( NEXTMOTION* pnm );
	int StepJump( NEXTMOTION* pnm );
	int StepDummy( NEXTMOTION* pnm );	

public:
	char m_animname[256];
	int m_motkind;

	int m_frameno;
	GPELEM m_curgpe;
	int m_curinterp;
	GPELEM* m_defgpeptr;
	int* m_definterptr;

	int m_isfirst;
	int m_maxframe;
	int m_animtype;
	int m_animstep;
	int m_stepdir;
	int m_motjump;
	NEXTMOTION m_nextmot;
	
	CGPKey* m_firstkey;
	CGPKey* m_framedata;

	int m_groundhsid;

	D3DXMATRIX* m_offmatptr;
	int* m_offflagptr;
};


#endif


