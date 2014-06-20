#ifndef __XAUDIO_H__
#define __XAUDIO_H__
#include <xaudio2.h>
#include <X3DAudio.h>

#define XAD_SAFE_RELEASE( p ) { if( p ){ (p)->Release(); (p) = NULL; } }

class XAudio{
public:
	~XAudio(){};
	static XAudio& GetInstance();
	static bool Release();
	IXAudio2* GetXAudio2(){ return pXAudio2; };

protected:
	XAudio(){}; // ƒVƒ“ƒOƒ‹ƒgƒ“

public:
	static DWORD m_dwChannelMask;
	static WORD m_OutputChannels;
	static int ReferenceCounter;
	static IXAudio2* pXAudio2;
	static IXAudio2MasteringVoice* pMasteringVoice;
	static X3DAUDIO_HANDLE x3DInstance;
};

#endif //__XAUDIO_H__
