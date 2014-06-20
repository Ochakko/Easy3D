#include "stdafx.h"
#include "XAudioFadeEffect.h"

XAudioFadeEffect::XAudioFadeEffect()
: CSampleXAPOBase < XAudioFadeEffect, FadeEffectParams >()
{
	m_pFadeAPO = NULL;
}

XAudioFadeEffect::~XAudioFadeEffect()
{
	if( m_pFadeAPO ){
		delete m_pFadeAPO;
		m_pFadeAPO = NULL;
	}
}

int XAudioFadeEffect::CreateEffect( const unsigned int MaxBufferCount, IXAPO** ppAPO )
{
	*ppAPO = NULL;

	IID XAPOIID;
	if( FAILED( CoCreateGuid( (GUID*)( &XAPOIID ) ) ) ) return 1;

	XAPO_REGISTRATION_PROPERTIES Properties;
	Properties.clsid = XAPOIID;
	wcscpy_s( Properties.CopyrightInfo, L"Ochakko LAB, 2010" );
	wcscpy_s( Properties.FriendlyName, L"Fade XAPO" );
	Properties.Flags = XAPOBASE_DEFAULT_FLAG;

	Properties.MajorVersion = 1;
	Properties.MinorVersion = 0;
	Properties.MinInputBufferCount = 1;
	Properties.MaxInputBufferCount = MaxBufferCount;
	Properties.MinOutputBufferCount = 1;
	Properties.MaxOutputBufferCount = MaxBufferCount;

    XAudioFadeEffect* pXAudioFadeEffect = NULL;
    XAudioFadeEffect::CreateInstance( NULL, 0, &pXAudioFadeEffect );

	//pXAudioFadeEffect->Initialize( NULL, 0 );

	*ppAPO = static_cast < IXAPO* >( pXAudioFadeEffect );

	return 0;
}

bool XAudioFadeEffect::Init( IXAudio2SourceVoice* pSourceVoice, const unsigned int MaxBufferCount )
{
	IID XAPOIID;
	if( FAILED( CoCreateGuid( (GUID*)( &XAPOIID ) ) ) ) return false;

	XAPO_REGISTRATION_PROPERTIES Properties;
	Properties.clsid = XAPOIID;
	wcscpy_s( Properties.CopyrightInfo, L"Denpa no Kandume, 2008" );
	wcscpy_s( Properties.FriendlyName, L"Fade XAPO" );
	Properties.Flags = XAPOBASE_DEFAULT_FLAG;

	Properties.MajorVersion = 1;
	Properties.MinorVersion = 0;
	Properties.MinInputBufferCount = 1;
	Properties.MaxInputBufferCount = MaxBufferCount;
	Properties.MinOutputBufferCount = 1;
	Properties.MaxOutputBufferCount = MaxBufferCount;

    XAudioFadeEffect* pXAudioFadeEffect = NULL;
    XAudioFadeEffect::CreateInstance( NULL, 0, &pXAudioFadeEffect );

	pXAudioFadeEffect->Initialize( NULL, 0 );

	XAUDIO2_EFFECT_DESCRIPTOR Descriptor;
	Descriptor.InitialState = true;
	Descriptor.OutputChannels = MaxBufferCount;
	Descriptor.pEffect = static_cast < IXAPO* >( pXAudioFadeEffect );

	XAUDIO2_EFFECT_CHAIN Chain;
	Chain.EffectCount = 1;
	Chain.pEffectDescriptors = &Descriptor;

	pSourceVoice->SetEffectChain( &Chain );

	pXAudioFadeEffect->Release();
	

	// 上のReleaseですでにpXAudioFadeEffectのインスタンスは解放されている。
	// CXAPOBase::Release()の中でやっている。
	pXAudioFadeEffect = NULL;

	return true;
}

//bool XAudioFadeEffect::Release()
//{
//	return true;
//}
#include <stdio.h>
void XAudioFadeEffect::DoProcess( const FadeEffectParams& Params, FLOAT32* __restrict pData, UINT32 cFrames, UINT32 cChannels )
{
	FadeEffectParams NextParams = Params;

	unsigned int TotalFrames = Params.Milliseconds * WaveFormat().nSamplesPerSec / 1000;

	for( UINT32 i = 0; i < cFrames * cChannels; ++i ){
		unsigned int a = Params.Frames + i;
		if( a > TotalFrames ) a = TotalFrames;

		NextParams.Gain = 1.0f;
		if( TotalFrames != 0 ){
			NextParams.Gain = NextParams.BeginGain + ( NextParams.EndGain - NextParams.BeginGain ) * a / TotalFrames;
			if( TotalFrames < Params.Frames + i ){
				NextParams.IsFade = false;
			}
		}

		pData[i] *= NextParams.Gain;

	}
//printf( "%f\n", NextParams.Gain );
	if( NextParams.IsFade ) NextParams.Frames = Params.Frames + cFrames;
	SetParameters( &NextParams, sizeof( FadeEffectParams ) );
}
