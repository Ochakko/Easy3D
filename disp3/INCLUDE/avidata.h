#ifndef AVIDATAH
#define AVIDATAH

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <basedat.h>

#include <vfw.h>
#include <D3DX9.h>

#define DBGH
#include <dbg.h>

class CAviData
{
public:
	CAviData();
	~CAviData();

//	int CreateAVIFile( LPDIRECT3DDEVICE9 pd3dDevice, char* srcfilename, int srcdatatype, int srccmpkind, int srcframerate, int srcframeleng );
//	int Write2AVIFile( LPDIRECT3DDEVICE9 pd3dDevice, int srcdatatype );
//	int CompleteAVIFile( LPDIRECT3DDEVICE9 pd3dDevice, int srcdatatype );

	int CreateAVIFile( LPDIRECT3DSURFACE9 pBack, char* srcfilename, int srcdatatype, int srccmpkind, int srcframerate, int srcframeleng );
	int Write2AVIFile( LPDIRECT3DSURFACE9 pBack, int srcdatatype );
	int CompleteAVIFile( LPDIRECT3DSURFACE9 pBack, int srcdatatype );

	int AddToPrev( CAviData* addavi );
	int AddToNext( CAviData* addavi );
	int LeaveFromChain();


private:
	int InitParams();
	int InitCompVars();
	int DestroyObjs();

	int CmpBITMAPINFOHEADER( BITMAPINFOHEADER* bmih1, BITMAPINFOHEADER* bmih2 );

public:
	int serialno;
	int curframeno;
	int completeflag;
		
	char filename[2048];
	int datatype;
	int compkind;
	int framerate;
	int frameleng;

	BITMAPINFOHEADER bmih;
	COMPVARS cv_cinepak, cv_ms, cv_notcomp;
	unsigned long cvstate_cinepak, cvstate_ms;

    PAVIFILE pavi;
    PAVISTREAM pstm;
	PAVISTREAM ptmp;

	CAviData* next;
	CAviData* prev;
	int ishead;
};

#endif