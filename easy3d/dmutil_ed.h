//-----------------------------------------------------------------------------
// File: DMUtil.h
//
// Desc: 
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef DMUTIL_H
#define DMUTIL_H

#include <d3dx9.h>

#include <dmusicc.h>
#include <dmusici.h>
#include <dsound.h>


//-----------------------------------------------------------------------------
// Classes used by this header
//-----------------------------------------------------------------------------
class CMusicManager;
class CMusicSegment;
class CMusicScript;

//-----------------------------------------------------------------------------
// Name: class CMusicManager
// Desc: 
//-----------------------------------------------------------------------------
class CMusicManager
{
protected:
    BOOL                      m_bCleanupCOM;
    IDirectMusicLoader8*      m_pLoader;
    IDirectMusicPerformance8* m_pPerformance;

public:
    CMusicManager();
    ~CMusicManager();

    inline IDirectMusicLoader8*      GetLoader()      { return m_pLoader; }
    inline IDirectMusicPerformance8* GetPerformance() { return m_pPerformance; }
    IDirectMusicAudioPath8* GetDefaultAudioPath();

    HRESULT Initialize( HWND hWnd, DWORD dwPChannels = 128, DWORD dwDefaultPathType = DMUS_APATH_DYNAMIC_STEREO );

    HRESULT SetSearchDirectory( const TCHAR* strMediaPath );
    VOID    CollectGarbage();

    HRESULT CreateSegmentFromFile( CMusicSegment** ppSegment, TCHAR* strFileName, 
		                           BOOL bDownloadNow = TRUE, BOOL bIsMidiFile = FALSE );

	HRESULT CreateSegmentFromBuf( CMusicSegment** ppSegment, char* buf, int bufsize,
		                           BOOL bDownloadNow = TRUE, BOOL bIsMidiFile = FALSE );

    
	HRESULT CreateScriptFromFile( CMusicScript** ppScript, TCHAR* strFileName );
    HRESULT CreateChordMapFromFile( IDirectMusicChordMap8** ppChordMap, TCHAR* strFileName );
    HRESULT CreateStyleFromFile( IDirectMusicStyle8** ppStyle, TCHAR* strFileName );
    HRESULT GetMotifFromStyle( IDirectMusicSegment8** ppMotif, TCHAR* strStyle, TCHAR* wstrMotif );

    HRESULT CreateSegmentFromResource( CMusicSegment** ppSegment, TCHAR* strResource, TCHAR* strResourceType, 
		                           BOOL bDownloadNow = TRUE, BOOL bIsMidiFile = FALSE );

	HRESULT SetVolume( long lvol );
};




//-----------------------------------------------------------------------------
// Name: class CMusicSegment
// Desc: Encapsulates functionality of an IDirectMusicSegment
//-----------------------------------------------------------------------------
class CMusicSegment
{
protected:
    IDirectMusicSegment8*     m_pSegment;
    IDirectMusicLoader8*      m_pLoader;
    IDirectMusicPerformance8* m_pPerformance;
    IDirectMusicAudioPath8*   m_pEmbeddedAudioPath;
    BOOL                      m_bDownloaded;

public:
	//int serialno;
	//int is3d;
	//CMusicSegment* next;

	//IDirectSound3DBuffer*   m_pDS3DBuffer;//            = NULL;   // 3D sound buffer
	//DS3DBUFFER              m_dsBufferParams;//                  // 3D buffer properties

	//IDirectMusicAudioPath*  m_p3DAudioPath;
	//IDirectSound3DListener* m_pDSListener;// 3D listener object
	//DS3DLISTENER            m_dsListenerParams;// Listener properties

	//IDirectMusicAudioPath*  m_pStereoAudioPath;
	IDirectSoundBuffer8*   m_pDSBuffer;//            = NULL;   // stereo sound buffer


public:
    CMusicSegment( IDirectMusicPerformance8* pPerformance, 
                   IDirectMusicLoader8* pLoader,
                   IDirectMusicSegment8* pSegment );
    virtual ~CMusicSegment();

    inline  IDirectMusicSegment8* GetSegment() { return m_pSegment; }
    HRESULT GetStyle( IDirectMusicStyle8** ppStyle, DWORD dwStyleIndex = 0 );

    HRESULT SetRepeats( DWORD dwRepeats );
    HRESULT Play( DWORD dwFlags = DMUS_SEGF_SECONDARY, IDirectMusicAudioPath8* pAudioPath = NULL );
    HRESULT Stop( DWORD dwFlags = 0 );
    HRESULT Download( IDirectMusicAudioPath8* pAudioPath = NULL );
    HRESULT Unload( IDirectMusicAudioPath8* pAudioPath = NULL );

    BOOL    IsPlaying();


	HRESULT SetStartPointOfSound( int sec1000 );
	HRESULT GetMusicTime( int* mtimeptr, double* rtimeptr );

	//int Set3DListener( float fDopplerFactor, float fRolloffFactor );
	//int Set3DDistance( float fMinDistance, float fMaxDistance );
	//int Set3DMovement( D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity );

	//int Set3DListenerMovement( D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity, D3DXVECTOR3* pvOrientFront, D3DXVECTOR3* pvOrientTop );


};




//-----------------------------------------------------------------------------
// Name: class CMusicScript
// Desc: Encapsulates functionality of an IDirectMusicScript
//-----------------------------------------------------------------------------
class CMusicScript
{
protected:
    IDirectMusicScript8*      m_pScript;
    IDirectMusicLoader8*      m_pLoader;
    IDirectMusicPerformance8* m_pPerformance;

public:
    CMusicScript( IDirectMusicPerformance8* pPerformance, 
                  IDirectMusicLoader8* pLoader,
                  IDirectMusicScript8* pScript );
    virtual ~CMusicScript();

    inline  IDirectMusicScript8* GetScript() { return m_pScript; }

    HRESULT CallRoutine( TCHAR* strRoutine );
    HRESULT SetVariableNumber( TCHAR* strVariable, LONG lValue );
    HRESULT GetVariableNumber( TCHAR* strVariable, LONG* plValue );
};


#endif // DMUTIL_H
