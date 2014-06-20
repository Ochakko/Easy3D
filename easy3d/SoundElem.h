#ifndef SOUNDELEMH
#define SOUNDELEMH

class CHuSound;


class CSoundElem
{
public:
	CSoundElem( CHuSound* phs );
	~CSoundElem();

	int LoadSound( int setid, char* dirname, char* ssfdir, char* filename, int bgmflag, int reverbflag, int bufnum, int* dstseriptr );
	int GetSamplesPerSec( DWORD* dstsample );
	int GetSamplesPlayed( double* dstsample );
private:
	int InitParams();
	int DestroyObjs();

private:
	CHuSound* m_hs;

public:
	char m_filename[256];//フォルダ名なしのファイル名のみ
	int m_serialno;
	int m_setid;
	int m_bgmflag;
	int m_bufnum;
	int m_reverbflag;
	double m_totaltime;

};

#endif