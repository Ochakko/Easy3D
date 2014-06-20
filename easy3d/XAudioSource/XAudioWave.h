#ifndef __XAUDIOWAVE_H__
#define __XAUDIOWAVE_H__

#include "XAudio.h"
#include "XAudioFadeEffect.h"
#include <vector>
#include <algorithm>

#include "audiocoef.h"

#include "SampleAPOBase.h"


//#define INPUT3DCHANNELS 1  // number of source channels
//#define OUTPUT3DCHANNELS 8 // maximum number of destination channels supported in this sample




class WaveCallback;

class XAudioWave{

private:
typedef void (*PFUNC)();
typedef void (*PFUNC_VOIDP)( void * );
typedef void (*PFUNC_VOIDP_HRESULT)( void *, HRESULT );

public:
	XAudioWave();
	~XAudioWave();

	bool Load( const char *pFileName );
	int LoadFromBuf( char* srcbuf, int srcbufleng );

	bool Release();

	bool Play( UINT32 startsample, int NumLoops, UINT32 openo = XAUDIO2_COMMIT_NOW );
	bool Resume();
	bool Stop();
	STATUS GetStatus(){ return Status; };

	int FlushSourceBuffers();

	bool SetVolume( const float Volume );
	float GetVolume() const;
	int SetFrequencyRatio( const float freq );
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

/***
enum {
	CB_StreamEnd,
	CB_VoiceProcessingPassEnd,
	CB_VoiceProcessingPassStart,
	CB_MAX
};
enum {
	CBV_BufferEnd,
	CBV_BufferStart,
	CBV_LoopEnd,
	CBV_MAX
};
***/

	void (XAudioWave::*CallbackPFUNC[CB_MAX])();
	void (XAudioWave::*CallbackPFUNCV[CBV_MAX])( void* p);
	void (XAudioWave::*CallbackVoiceError)( void* p, HRESULT hr);


	void OnStreamEnd();
	void OnVoiceProcessingPassEnd();
	void OnVoiceProcessingPassStart();
	void OnBufferEnd( void* p );
	void OnBufferStart( void* p );
	void OnLoopEnd( void* p );
	void OnVoiceError( void* p, HRESULT hr );


private:
	// callback functions
	friend class WaveCallback;

	int InitAudioState();
	int InitCallbackArray();

public:
	AUDIO_STATE m_audioState;
	int flag3d;
	int flagreverb;
	float freqratio;//for 3d sound
	float dpfactor;
	double startsmp;
	int WaveSize;
	WAVEFORMATEX m_WaveFormatEx;
	int* m_stopcntptr;
	int* m_startcntptr;
private:
	IXAudio2SourceVoice* pSourceVoice;

	BYTE *pData;

	XAudioFadeEffect *pXAudioFadeEffect;
	IUnknown *pRevEffect;

	WaveCallback *pStatusCallBack;

	STATUS Status;

	IXAPO* m_pFadeAPO;

/***
	std::vector < PFUNC >				Callback_StreamEnds;
	std::vector < PFUNC >				Callback_VoiceProcessingPassEnds;
	std::vector < PFUNC >				Callback_VoiceProcessingPassStarts;
	std::vector < PFUNC_VOIDP >			Callback_BufferEnds;
	std::vector < PFUNC_VOIDP >			Callback_BufferStarts;
	std::vector < PFUNC_VOIDP >			Callback_LoopEnds;
	std::vector < PFUNC_VOIDP_HRESULT >	Callback_VoiceErrors;
***/
};

// Callback Function
class WaveCallback : public IXAudio2VoiceCallback
{
public:
    WaveCallback(): pXAudioWave( NULL ){}
    ~WaveCallback(){}

    //Called when the voice has just finished playing a contiguous audio stream.
	STDMETHOD_( void, OnStreamEnd )					(){ if(pXAudioWave){pXAudioWave->OnStreamEnd();} };
	STDMETHOD_( void, OnVoiceProcessingPassEnd )	(){ if(pXAudioWave){pXAudioWave->OnVoiceProcessingPassEnd();} };
	STDMETHOD_( void, OnVoiceProcessingPassStart )	( UINT32 SamplesRequired ){ if(pXAudioWave){pXAudioWave->OnVoiceProcessingPassStart();} };
	STDMETHOD_( void, OnBufferEnd )					( void * pBufferContext ){ if(pXAudioWave){pXAudioWave->OnBufferEnd( pBufferContext );} };
	STDMETHOD_( void, OnBufferStart )				( void * pBufferContext ){ if(pXAudioWave){pXAudioWave->OnBufferStart( pBufferContext );} };
	STDMETHOD_( void, OnLoopEnd )					( void * pBufferContext ){ if(pXAudioWave){pXAudioWave->OnLoopEnd( pBufferContext );} };
	STDMETHOD_( void, OnVoiceError )				( void * pBufferContext, HRESULT Error ){ if(pXAudioWave){pXAudioWave->OnVoiceError( pBufferContext, Error );} };

public:
	XAudioWave *pXAudioWave;
};


/***
***/


#endif //__XAUDIOWAVE_H__

