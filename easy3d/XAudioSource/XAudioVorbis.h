#ifndef __XAUDIOVORBIS_H__
#define __XAUDIOVORBIS_H__

#include "XAudio.h"
#include "XAudioFadeEffect.h"
#include <vector>
#include <algorithm>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "audiocoef.h"

#include "SampleAPOBase.h"



//#define WAV_BUFFERSIZE ( 4096 * 20 )
//#define WAV_BUFFERSIZE ( 1024 * 2 )
// WAV_NUMBUFFER‚ÍSubmitSourceBuffer‚ª64‚ð’´‚¦‚é‚Æ‚±‚¯‚é‚Ì‚Å‚»‚êˆÈ‰º‚É‚·‚é‚±‚Æ
//#define WAV_NUMBUFFER 16
//#define WAV_NUMBUFFER 300

class VorbisCallback;

// The vorbis libraries are fully reentrant, but not thread safe.
class XAudioVorbis{

private:
typedef void (*PFUNC)();
typedef void (*PFUNC_VOIDP)( void * );
typedef void (*PFUNC_VOIDP_HRESULT)( void *, HRESULT );

public:

#define XAUDIOVORBIS_LOOP_INFINITE INT_MIN

public:
	XAudioVorbis();
	~XAudioVorbis();

	bool Load( const char *pFileName );
	bool Release();

	bool Play( UINT32 startsample, int NumLoops, UINT32 openo = XAUDIO2_COMMIT_NOW );
	bool Resume();
	bool Stop();
	STATUS GetStatus(){ return Status; };

	int FlushSourceBuffers();

	bool SetVolume( const float Volume );
	float GetVolume() const;
	int SetFrequencyRatio( const float Ratio );
	float GetFrequencyRatio() const;
	int GetSamplesPlayed( double* psmp );


	bool Fade( const unsigned int Milliseconds, const float BeginGain, const float EndGain );
	bool Fade( const unsigned int Milliseconds, const float EndGain );

	int Update3DSound();
	int SetEmitterParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel );
	int SetListenerParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel, D3DXVECTOR3 dir, D3DXVECTOR3 up );
	int SetFlag3D( int srcflag3d );
	int Set3DEmiDist( float dist );

	int GetDopplerScaler( float* dopplerptr );
	int SetDopplerScaler( float srcdoppler );


	void (XAudioVorbis::*CallbackPFUNC[CB_MAX])();
	void (XAudioVorbis::*CallbackPFUNCV[CBV_MAX])( void* p);
	void (XAudioVorbis::*CallbackVoiceError)( void* p, HRESULT hr);


	void OnStreamEnd();
	void OnVoiceProcessingPassEnd();
	void OnVoiceProcessingPassStart();
	void OnBufferEnd( void* p );
	void OnBufferStart( void* p );
	void OnLoopEnd( void* p );
	void OnVoiceError( void* p, HRESULT hr );

private:
	void Read( char *pBuffer, const unsigned int BufferSize, unsigned int &ReadSize, bool &IsEnd );

	// callback functions
	friend class VorbisCallback;

	int InitAudioState();
	int DestroySoundBuffer( int allflag = 1 );
	int InitCallbackArray();

	int CreateBuffer();
	int DestroyBuffer();

public:
	AUDIO_STATE m_audioState;
	int flag3d;
	int flagreverb;
	float freqratio;
	float dpfactor;
	double startsmp;

	int WaveSize;
	WAVEFORMATEX m_WaveFormatEx;
	int* m_stopcntptr;
	int* m_startcntptr;
private:
//	char** ppBuffer;
//	int* pValidBufferSize;
//	int WAV_NUMBUFFER;
//	int BufferTopPos;

	int calcsize;
	char* calcdata;

	BYTE *pData;


	OggVorbis_File vf;
	FILE *fp;

	bool IsBufferedAll;

	int NumLoops;


	IUnknown *pRevEffect;
	XAudioFadeEffect *pXAudioFadeEffect;

	IXAudio2SourceVoice* pSourceVoice;
	VorbisCallback *pStatusCallBack;


	STATUS Status;

	IXAPO* m_pFadeAPO;

};

// Callback Function
class VorbisCallback : public IXAudio2VoiceCallback
{
public:
    VorbisCallback(): pXAudioVorbis( NULL ){}
    ~VorbisCallback(){}

    //Called when the voice has just finished playing a contiguous audio stream.
	STDMETHOD_( void, OnStreamEnd )					(){ if(pXAudioVorbis){pXAudioVorbis->OnStreamEnd();} };
	STDMETHOD_( void, OnVoiceProcessingPassEnd )	(){ if(pXAudioVorbis){pXAudioVorbis->OnVoiceProcessingPassEnd();} };
	STDMETHOD_( void, OnVoiceProcessingPassStart )	( UINT32 SamplesRequired ){ if(pXAudioVorbis){pXAudioVorbis->OnVoiceProcessingPassStart();} };
	STDMETHOD_( void, OnBufferEnd )					( void * pBufferContext ){ if(pXAudioVorbis){pXAudioVorbis->OnBufferEnd( pBufferContext );} };
	STDMETHOD_( void, OnBufferStart )				( void * pBufferContext ){ if(pXAudioVorbis){pXAudioVorbis->OnBufferStart( pBufferContext );} };
	STDMETHOD_( void, OnLoopEnd )					( void * pBufferContext ){ if(pXAudioVorbis){pXAudioVorbis->OnLoopEnd( pBufferContext );} };
	STDMETHOD_( void, OnVoiceError )				( void * pBufferContext, HRESULT Error ){ if(pXAudioVorbis){pXAudioVorbis->OnVoiceError( pBufferContext, Error );} };

public:
	XAudioVorbis *pXAudioVorbis;
};

#endif //__XAUDIOVORBIS_H__
