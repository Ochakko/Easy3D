#ifndef __XAUDIOFADEEFFECT_
#define __XAUDIOFADEEFFECT_

#include <xaudio2.h>
#include "SampleAPOBase.h"

struct FadeEffectParams
{
	bool IsFade;

	float Gain;
	float BeginGain;
	float EndGain;
	unsigned int Milliseconds;
	unsigned int Frames;

public:
	FadeEffectParams()
	{
		IsFade = false;

		Gain = 1.0f;
		BeginGain = 1.0f;
		EndGain = 1.0f;
		Milliseconds = 0;
		Frames = 0;
	}
};

class __declspec( uuid("{5EB8D611-FF96-429d-8365-2DDF89A7C1CD}")) 
XAudioFadeEffect : public CSampleXAPOBase < XAudioFadeEffect, FadeEffectParams >
{
public:
	XAudioFadeEffect();
    ~XAudioFadeEffect();

	bool Init( IXAudio2SourceVoice* pSourceVoice, const unsigned int MaxBufferCount );
//	bool Release();

    void DoProcess( const FadeEffectParams& Params, FLOAT32* __restrict pData, UINT32 cFrames, UINT32 cChannels );

	int CreateEffect( const unsigned int MaxBufferCount, IXAPO** ppAPO );

	CSampleXAPOBase* m_pFadeAPO;
};

#endif
