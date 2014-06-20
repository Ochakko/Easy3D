#ifndef SOUNDBANKH
#define SOUNDBANKH

class CSoundSet;
class CSoundElem;
class CHuSound;

class CSoundBank
{
public:
	CSoundBank();
	~CSoundBank();

	CSoundSet* AddSoundSet( char* srcname, int* dstsetid );
	int DelSoundSet( int setid );

	int AddSoundElem( CHuSound* srchs, int setid, CSoundElem* pse, int* dstsetid, int* dstsoundid );
	int DelSoundElem( int setid, int soundid );
	int GetIDByName( int setid, char* filename, int* dstsetid, int* dstsoundid );//setid -1Ç≈ëSåüçı

	int GetSoundSet( int setid, CSoundSet** ppss );
	int GetSoundSet( char* setname, CSoundSet** ppss );

	int UpSoundSet( int setid );
	int DownSoundSet( int setid );


	int GetSoundElem( int setid, int soundid, CSoundElem** ppse );
	int GetSoundElem( int setid, char* filename, CSoundElem** ppse );	

	int GetSoundSetIndexByID( int setid, int* dstindex );


private:	
	int InitParams();
	int DestroyObjs();

public:
	int m_setnum;
	CSoundSet** m_ppset;
};


#endif