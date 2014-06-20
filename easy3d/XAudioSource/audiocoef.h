#ifndef AUDIOCOEFH
#define AUDIOCOEFH

#include <X3DAudio.h>
#include <D3DX9.h>
#include <xaudio2fx.h>

#define XAUDIOFREQMAX	10.0f

#define NUM_PRESETS 30

enum STATUS{
	STATUS_WAITING,
	STATUS_PLAYING,
	STATUS_STOP,
};

struct AUDIO_STATE
{
    bool bInitialized;
    int nFrameToApply3DAudio;
//    DWORD dwChannelMask;
//    UINT32 nChannels;
    X3DAUDIO_DSP_SETTINGS dspSettings;
    X3DAUDIO_LISTENER listener;
    X3DAUDIO_EMITTER emitter;
    X3DAUDIO_CONE emitterCone;

    D3DXVECTOR3 vListenerPos;
    D3DXVECTOR3 vEmitterPos;
    float fListenerAngle;
    bool  fUseListenerCone;
    bool  fUseInnerRadius;
    bool  fUseRedirectToLFE;

    FLOAT32 emitterAzimuths[8];
    FLOAT32 matrixCoefficients[8 * 8];
};

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

#endif