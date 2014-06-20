#ifndef RpsFileH
#define RpsFileH

#include <d3dx9.h>
#include <usercoef.h>

#define RPSLINELEN	1024

class CTraQ;
class CQuaternion;
class CShdHandler;

class CHandlerSet;
class CMotionPoint2;

class CRpsFile
{
public:
	CRpsFile( CHandlerSet* hsptr );
	~CRpsFile();

	int CreateParams();
	int CalcTraQ( TSELEM* tsptr, int chksym );
	int SetMotion( int posmode, TSELEM* tsptr, int motid, int setframe );

	int SetRpsElem( int frameno, D3DXVECTOR3* srcpos );
	int InitArMp( TSELEM* tsptr, int motid, int frameno );



private:
	int InitParams();
	int DestroyObjs();

	int SetBuffer( char* filename );

	int CreateRpsElem();

	int SetSkipFlag();
	int ResetOutputFlag( D3DXVECTOR3* keyeul, D3DXVECTOR3* keymv );

	int SetSym( int skelno, int sksym, TSELEM* tsptr, CShdHandler* lpsh );
	int CalcSymPose( TSELEM* tsptr, CShdHandler* lpsh, CTraQ* traq, 
		CQuaternion* newqptr, D3DXVECTOR3* newtraptr, D3DXVECTOR3* neweulptr, D3DXVECTOR3 befeul );

	int CreateMotionPoints( int cookie );

private:
	CHandlerSet* m_hs;

	//fileëÄçÏóp
	HANDLE m_hfile;
	char* m_buf;
	DWORD m_pos;
	DWORD m_bufleng;


	char m_line[ RPSLINELEN ];
	RPSELEM* m_pelem;
	CTraQ* m_traq;
//	int m_framenum;

//	int m_sampling;
//	int m_keystep;

	int m_skipflag[ SKEL_MAX ];

	CMotionPoint2* m_armp[ SKEL_MAX ];


};


#endif