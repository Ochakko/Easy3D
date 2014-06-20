#include "stdafx.h"
#include "XAudio.h"

int XAudio::ReferenceCounter = 0;
IXAudio2* XAudio::pXAudio2 = NULL;
IXAudio2MasteringVoice* XAudio::pMasteringVoice = NULL;
X3DAUDIO_HANDLE XAudio::x3DInstance = {0};
DWORD XAudio::m_dwChannelMask = 0;
WORD XAudio::m_OutputChannels = 2;


XAudio* _pXAudioSingleton_ = NULL;

#ifdef _DEBUG
#pragma comment( lib, "xapobased.lib" )
#else
#pragma comment( lib, "xapobase.lib" )
#endif

XAudio& XAudio::GetInstance()
{
	if( !_pXAudioSingleton_ ){
		_pXAudioSingleton_ = new XAudio();

		CoInitializeEx( NULL, COINIT_MULTITHREADED );

		UINT32 flags = 0;
	#ifdef _DEBUG
		flags |= XAUDIO2_DEBUG_ENGINE;
	#endif

		if( FAILED( XAudio2Create( &pXAudio2, flags ) ) ){
			CoUninitialize();
			MessageBoxA( NULL, "Failed: XAudio::XAudio2Create DirectXのバージョンが古い可能性があります。November 2008以降のバージョンのDirectXをインストールして下さい。", "Error", MB_OK );
		}

		if( FAILED( pXAudio2->CreateMasteringVoice( &pMasteringVoice ) ) ){
			XAD_SAFE_RELEASE( pXAudio2 );
			CoUninitialize();
			MessageBoxA( NULL, "Failed: XAudio::CreateMasteringVoice", "Error", MB_OK );
		}

		HRESULT hr;
		// Check device details to make sure it's within our sample supported parameters
		XAUDIO2_DEVICE_DETAILS details;
		if( FAILED( hr = pXAudio2->GetDeviceDetails( 0, &details ) ) )
		{
			XAD_SAFE_RELEASE( pXAudio2 );
			CoUninitialize();
			MessageBoxA( NULL, "Failed: XAudio::GetDeviceDetails", "Error", MB_OK );
		}

		if( details.OutputFormat.Format.nChannels > XAUDIO2_MAX_AUDIO_CHANNELS )
		{
			XAD_SAFE_RELEASE( pXAudio2 );
			CoUninitialize();
			MessageBoxA( NULL, "Failed: details channels", "Error", MB_OK );
		}

		XAudio::m_dwChannelMask = details.OutputFormat.dwChannelMask;
		XAudio::m_OutputChannels = details.OutputFormat.Format.nChannels;

		X3DAudioInitialize( XAudio::m_dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, XAudio::x3DInstance );

	}

	return *_pXAudioSingleton_;
}

bool XAudio::Release()
{
	if( _pXAudioSingleton_ ){
		if( pMasteringVoice ) pMasteringVoice->DestroyVoice();
		XAD_SAFE_RELEASE( pXAudio2 );

		CoUninitialize();

		delete _pXAudioSingleton_;
		_pXAudioSingleton_ = NULL;
	}

	return true;
}
