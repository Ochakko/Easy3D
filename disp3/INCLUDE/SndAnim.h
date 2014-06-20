#ifndef SNDANIMH
#define SNDANIMH

#include <coef.h>

class CSndKey;
class CSndFrame;
class CSndFullFrame;
class CSoundSet;

class CSndAnim
{
public:
	CSndAnim( char* srcname, int srcmotkind );
	~CSndAnim();

	int ExistSndKey( int frameno, CSndKey** ppsndk );
	int CreateSndKey( int frameno, CSndKey** ppsndk );
	CSndKey* GetFirstSndKey();
	CSndKey* LeaveFromChainSndKey( int frameno );
	int ChainSndKey( CSndKey* addsndk );
	int DeleteSndKeyOutOfRange( int srcmaxframe );
//	int SetSndAnim();
	int AddSndE( int frameno, SNDELEM* srcsnde );
	int DeleteSndE( int frameno, int delindex, int* delkeyflag );
	int SetSndKey( int frameno, CSndKey* srcsndk );
	int DeleteSndKey( int frameno );

	int GetSndAnim( CSndFrame** ppsf, int frameno );
	int GetSndKeyframeNoRange( int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr );

	int SetSndAnimFrameNo( int frameno );

	int SetAnimName( char* srcname );
	int GetAnimName( char* dstname, int arrayleng );

	int SetFrameData();
	int CheckAndDelInvalid( CSoundSet* lpss );//seriチェック。不正データ削除。

	int ResetDoneFlag( int srcframe );

private:
	int InitParams();
	int DestroyObjs();
	int DestroyAllKeys();
	int DestroyFrameData();

public:
	char m_ssname[32];
	char m_animname[256];
	int m_motkind;
	int m_frameno;
	SNDELEM m_defsnde;
	int m_maxframe;	
	CSndKey* m_firstkey;
	CSndFullFrame* m_fullframe;
	CSndFrame* m_curframe;
	int m_fps;
	int m_bgmloop;
};


#endif