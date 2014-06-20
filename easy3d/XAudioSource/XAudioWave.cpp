#include "stdafx.h"
#include "XAudio.h"
#include "XAudioWave.h"


#pragma warning( disable: 4996 )
#include <stdio.h>

#define DBGH
#include "dbg.h"


// Must match order of g_PRESET_NAMES
extern XAUDIO2FX_REVERB_I3DL2_PARAMETERS g_PRESET_PARAMS[ NUM_PRESETS ];


static const X3DAUDIO_CONE Listener_DirectionalCone = { X3DAUDIO_PI*5.0f/6.0f, X3DAUDIO_PI*11.0f/6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };
// Specify LFE level distance curve such that it rolls off much sooner than
// all non-LFE channels, making use of the subwoofer more dramatic.
static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_LFE_CurvePoints[3] = { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f };
static const X3DAUDIO_DISTANCE_CURVE       Emitter_LFE_Curve          = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_LFE_CurvePoints[0], 3 };

// Specify reverb send level distance curve such that reverb send increases
// slightly with distance before rolling off to silence.
// With the direct channels being increasingly attenuated with distance,
// this has the effect of increasing the reverb-to-direct sound ratio,
// reinforcing the perception of distance.
static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_Reverb_CurvePoints[3] = { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f };
static const X3DAUDIO_DISTANCE_CURVE       Emitter_Reverb_Curve          = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_Reverb_CurvePoints[0], 3 };



XAudioWave::XAudioWave()
{
	pSourceVoice = NULL;
	WaveSize = 0;
	pData = NULL;
	pXAudioFadeEffect = new XAudioFadeEffect();
	pRevEffect = NULL;
	pStatusCallBack = new WaveCallback();

	Status = STATUS_WAITING;

	flag3d = 0;
	flagreverb = 1;

	ZeroMemory( &m_WaveFormatEx, sizeof( WAVEFORMATEX ) );

	InitAudioState();

	InitCallbackArray();

	m_pFadeAPO = 0;
	freqratio = 1.0f;
	dpfactor = 1.0f;
	startsmp = 0.0;
	m_stopcntptr = 0;
	m_startcntptr = 0;
}

int XAudioWave::InitCallbackArray()
{
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
	CallbackPFUNC[CB_StreamEnd] = &XAudioWave::OnStreamEnd;
	CallbackPFUNC[CB_VoiceProcessingPassEnd] = &XAudioWave::OnVoiceProcessingPassEnd;
	CallbackPFUNC[CB_VoiceProcessingPassStart] = &XAudioWave::OnVoiceProcessingPassStart;

	CallbackPFUNCV[CBV_BufferEnd] = &XAudioWave::OnBufferEnd;
	CallbackPFUNCV[CBV_BufferStart] = &XAudioWave::OnBufferStart;
	CallbackPFUNCV[CBV_LoopEnd] = &XAudioWave::OnLoopEnd;

	CallbackVoiceError = &XAudioWave::OnVoiceError;

	return 0;
}


int XAudioWave::InitAudioState()
{
    ZeroMemory( &m_audioState, sizeof( AUDIO_STATE ) );

    m_audioState.vListenerPos = D3DXVECTOR3( 0, 0, 0 );
    m_audioState.vEmitterPos = D3DXVECTOR3( 0, 0, 0.5f );

    m_audioState.fListenerAngle = 0;
    m_audioState.fUseListenerCone = TRUE;
    m_audioState.fUseInnerRadius = TRUE;
//    m_audioState.fUseRedirectToLFE = ((details.OutputFormat.dwChannelMask & SPEAKER_LOW_FREQUENCY) != 0);
	m_audioState.fUseRedirectToLFE = TRUE;

    //
    // Setup 3D audio structs
    //
    m_audioState.listener.Position = m_audioState.vListenerPos;
	m_audioState.listener.Velocity = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_audioState.listener.OrientFront = D3DXVECTOR3( 0, 0, 1 );
    m_audioState.listener.OrientTop = D3DXVECTOR3( 0, 1, 0 );
	m_audioState.listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;


    m_audioState.emitter.pCone = &m_audioState.emitterCone;
    m_audioState.emitter.pCone->InnerAngle = 0.0f;
    // Setting the inner cone angles to X3DAUDIO_2PI and
    // outer cone other than 0 causes
    // the emitter to act like a point emitter using the
    // INNER cone settings only.
    m_audioState.emitter.pCone->OuterAngle = 0.0f;
    // Setting the outer cone angles to zero causes
    // the emitter to act like a point emitter using the
    // OUTER cone settings only.
    m_audioState.emitter.pCone->InnerVolume = 0.0f;
    m_audioState.emitter.pCone->OuterVolume = 1.0f;
    m_audioState.emitter.pCone->InnerLPF = 0.0f;
    m_audioState.emitter.pCone->OuterLPF = 1.0f;
    m_audioState.emitter.pCone->InnerReverb = 0.0f;
    m_audioState.emitter.pCone->OuterReverb = 1.0f;

    m_audioState.emitter.Position = m_audioState.vEmitterPos;
    m_audioState.emitter.OrientFront = D3DXVECTOR3( 0, 0, 1 );
    m_audioState.emitter.OrientTop = D3DXVECTOR3( 0, 1, 0 );
    //m_audioState.emitter.ChannelCount = INPUTCHANNELS;
m_audioState.emitter.ChannelCount = 2;// for stereo

    m_audioState.emitter.ChannelRadius = 1.0f;
    m_audioState.emitter.pChannelAzimuths = m_audioState.emitterAzimuths;
m_audioState.emitterAzimuths[0] = 0.0f;

    // Use of Inner radius allows for smoother transitions as
    // a sound travels directly through, above, or below the listener.
    // It also may be used to give elevation cues.
    m_audioState.emitter.InnerRadius = 2.0f;
    m_audioState.emitter.InnerRadiusAngle = X3DAUDIO_PI/4.0f;;

	m_audioState.emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
	m_audioState.emitter.pLFECurve    = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_LFE_Curve;
    m_audioState.emitter.pLPFDirectCurve = NULL; // use default curve
    m_audioState.emitter.pLPFReverbCurve = NULL; // use default curve
	m_audioState.emitter.pReverbCurve    = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_Reverb_Curve;
    m_audioState.emitter.CurveDistanceScaler = 14.0f;
    m_audioState.emitter.DopplerScaler = 1.0f;

	//m_audioState.dspSettings.SrcChannelCount = INPUTCHANNELS;
    //m_audioState.dspSettings.DstChannelCount = m_audioState.nChannels;

//	m_audioState.dspSettings.SrcChannelCount = m_WaveFormatEx.nChannels;
//	m_audioState.dspSettings.DstChannelCount = XAudio::GetInstance().m_OutputChannels;
m_audioState.dspSettings.SrcChannelCount = 1;
m_audioState.dspSettings.DstChannelCount = 2;


	m_audioState.dspSettings.pMatrixCoefficients = m_audioState.matrixCoefficients;

    //
    // Done
    //
    m_audioState.bInitialized = true;

	return 0;
}

int XAudioWave::SetFlag3D( int srcflag3d )
{
	flag3d = srcflag3d;
	return 0;
}

XAudioWave::~XAudioWave()
{

	Release();

	if( pStatusCallBack ){
		delete pStatusCallBack;
		pStatusCallBack = NULL;
	}

	if( pXAudioFadeEffect ){
		delete pXAudioFadeEffect;
		pXAudioFadeEffect = NULL;
	}

	if( m_pFadeAPO ){
		m_pFadeAPO->Release();
		//delete m_pFadeAPO;
		m_pFadeAPO = 0;
	}
}

int XAudioWave::LoadFromBuf( char* srcbuf, int srcbufleng )
{
	Stop();
	Release();

	int curpos = 0;

	unsigned char Header[4];
	MoveMemory( Header, srcbuf + curpos, 4 );
	curpos += 4;

	// RIFF
	if( memcmp( Header, "RIFF", 4 ) != 0 ){
		_ASSERT( 0 );
		Release();
		return 1;
	}

	// File Size
	MoveMemory( Header, srcbuf + curpos, 4 );
	curpos += 4;
	int FileSize = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// WAVE
	MoveMemory( Header, srcbuf + curpos, 4 );
	curpos += 4;
	if( memcmp( Header, "WAVE", 4 ) != 0 ){
		_ASSERT( 0 );
		Release();
		return 1;
	}

	// fmt
	MoveMemory( Header, srcbuf + curpos, 4 );
	curpos += 4;
	if( memcmp( Header, "fmt ", 4 ) != 0 ){
		_ASSERT( 0 );
		Release();
		return 1;
	}

	// Linear PCM
	MoveMemory( Header, srcbuf + curpos, 4 );
	curpos += 4;
	int FMTChunkBytes = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	MoveMemory( Header, srcbuf + curpos, 2 );
	curpos += 2;
	int FormatID = ( ( (int)Header[1] ) << 8 ) + (int)Header[0];
	if( FormatID != 0x0001 && FormatID != 0xFFFE ){ // 0x0001: WAVE_FORMAT_PCM 0xFFFE: WAVE_FORMAT_EXTENSIBLE
		_ASSERT( 0 );
		Release();
		return 1;
	}

	// Number of Channels
	MoveMemory( Header, srcbuf + curpos, 2 );
	curpos += 2;
	int NumChannels = ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

/////////
	if( flag3d && (NumChannels >= 2) ){
		::MessageBox( NULL, "３Dサウンドはモノラルのみです。", "エラー", MB_OK );
		_ASSERT( 0 );
		Release();
		return 1;
	}
	m_audioState.emitter.ChannelCount = NumChannels;


	// Sampling Rate
	MoveMemory( Header, srcbuf + curpos, 4 );
	curpos += 4;
	int SamplingRate = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// Data Speed
	MoveMemory( Header, srcbuf + curpos, 4 );
	curpos += 4;
	int DataSpeed = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// Block Size
	MoveMemory( Header, srcbuf + curpos, 2 );
	curpos += 2;
	int BlockSize = ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// Bits / Sample
	MoveMemory( Header, srcbuf + curpos, 2 );
	curpos += 2;
	int BitsPerSample = ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// Skip
	if( FMTChunkBytes - 16 > 0 ){
		curpos += FMTChunkBytes - 16;
		//fseek( fp, FMTChunkBytes - 16, SEEK_CUR );
	}
		
	// skip chunks
	for( int i = 0; ; ++i ){
		MoveMemory( Header, srcbuf + curpos, 4 );
		curpos += 4;

		if( memcmp( Header, "data", 4 ) == 0 ){
			break;
		}

		MoveMemory( Header, srcbuf + curpos, 4 );
		curpos += 4;
		int Size = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];
		curpos += Size;
		//fseek( fp, Size, SEEK_CUR );
	}


	// data
	// WaveSize (byte)
	MoveMemory( Header, srcbuf + curpos, 4 );
	curpos += 4;
	WaveSize = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	if( WaveSize > XAUDIO2_MAX_BUFFER_BYTES ) {
		_ASSERT( 0 );
		Release();
		return 1;
	}


	pData = new unsigned char[WaveSize];


	MoveMemory( pData, srcbuf + curpos, sizeof( char ) * WaveSize );
	curpos += (sizeof( char ) * WaveSize);


	// PCM audio
//	WAVEFORMATEX WaveFormatEx;

	m_WaveFormatEx.wFormatTag = 1; // PCM audio
	m_WaveFormatEx.nChannels = NumChannels;
	m_WaveFormatEx.nSamplesPerSec = SamplingRate;
	m_WaveFormatEx.wBitsPerSample = BitsPerSample;
	m_WaveFormatEx.nBlockAlign = m_WaveFormatEx.nChannels * m_WaveFormatEx.wBitsPerSample / 8;
	m_WaveFormatEx.nAvgBytesPerSec = m_WaveFormatEx.nSamplesPerSec * m_WaveFormatEx.nBlockAlign;
	m_WaveFormatEx.cbSize = WaveSize;//!!!!! 0

	pStatusCallBack->pXAudioWave = this;


    XAUDIO2_SEND_DESCRIPTOR sendDescriptors;
    sendDescriptors.Flags = XAUDIO2_SEND_USEFILTER; // LPF direct-path
	sendDescriptors.pOutputVoice = XAudio::GetInstance().pMasteringVoice;
    const XAUDIO2_VOICE_SENDS sendList = { 1, &sendDescriptors };

	if( FAILED( XAudio::GetInstance().GetXAudio2()->CreateSourceVoice(
			&pSourceVoice, &m_WaveFormatEx, 0, XAUDIOFREQMAX, pStatusCallBack, &sendList ) ) ){
		_ASSERT( 0 );
		Release();
		return 1;
	}

//	if( !pXAudioFadeEffect->Init( pSourceVoice, NumChannels ) ) return false;


	int ret;
	HRESULT hr;
//	IXAPO* pFadeAPO = NULL;
//	ret = pXAudioFadeEffect->CreateEffect( NumChannels, &pFadeAPO );
	ret = pXAudioFadeEffect->CreateEffect( NumChannels, &m_pFadeAPO );
	if( ret || !m_pFadeAPO ){
		DbgOut( "wave : LoadFromBuf : CreateEffect error !!!\r\n" );
		_ASSERT( 0 );
		Release();
		return 1;
	}


	XAUDIO2_EFFECT_CHAIN Chain;
	ZeroMemory( &Chain, sizeof( XAUDIO2_EFFECT_CHAIN ) );
	XAUDIO2_EFFECT_DESCRIPTOR Descriptor[2];
	ZeroMemory( Descriptor, sizeof( XAUDIO2_EFFECT_DESCRIPTOR ) * 2 );
	XAUDIO2_EFFECT_DESCRIPTOR FadeDescriptor;
	ZeroMemory( &FadeDescriptor, sizeof( XAUDIO2_EFFECT_DESCRIPTOR ) );

	if( flagreverb ){
		hr = XAudio2CreateReverb( &pRevEffect );
		if( (hr != 0) || !pRevEffect ){
			DbgOut( "wave : LoadFromBuf : CreateReverb error !!!\r\n" );
			_ASSERT( 0 );
			Release();
			return 1;
		}

		Descriptor[0].InitialState = true;
		Descriptor[0].OutputChannels = NumChannels;
		Descriptor[0].pEffect = m_pFadeAPO;

		Descriptor[1].InitialState = true;
		//Descriptor[1].OutputChannels = 1;
		Descriptor[1].OutputChannels = NumChannels;
		Descriptor[1].pEffect = pRevEffect;

		Chain.EffectCount = 2;
		Chain.pEffectDescriptors = &(Descriptor[0]);
	}else{
		FadeDescriptor.InitialState = true;
		FadeDescriptor.OutputChannels = NumChannels;
		FadeDescriptor.pEffect = m_pFadeAPO;

		Chain.EffectCount = 1;
		Chain.pEffectDescriptors = &FadeDescriptor;
	}
	
	hr = pSourceVoice->SetEffectChain( &Chain );
	if( hr != S_OK ){
		DbgOut( "wave : loadFromBuf : source SetEffectChain error !!!\r\n" );
		_ASSERT( 0 );
		Release();
		return 1;
	}



	FadeEffectParams Param;
	ZeroMemory( &Param, sizeof( FadeEffectParams ) );
	Param.IsFade = false;
	if( FAILED( pSourceVoice->SetEffectParameters( 0, &Param, sizeof( FadeEffectParams ) ) ) ){
		DbgOut( "wave : LoadFromBuf : SetEffectParameters error !!!\r\n" );
		_ASSERT( 0 );
		Release();
		return 1;
	}

	if( flagreverb ){
		XAUDIO2FX_REVERB_PARAMETERS reverbParameters;
		reverbParameters.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
		reverbParameters.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
		reverbParameters.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
		reverbParameters.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		reverbParameters.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		reverbParameters.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		reverbParameters.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		reverbParameters.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
		reverbParameters.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
		reverbParameters.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
		reverbParameters.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
		reverbParameters.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
		reverbParameters.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
		reverbParameters.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
		reverbParameters.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
		reverbParameters.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
		reverbParameters.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
		reverbParameters.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
		reverbParameters.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
		reverbParameters.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
		reverbParameters.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
		reverbParameters.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;

		hr = pSourceVoice->SetEffectParameters( 1, &reverbParameters, sizeof( reverbParameters ) );
		if( hr != 0 ){
			DbgOut( "wave : LoadFromBuf : SetEffectParameters 1 error !!!\r\n" );

			_ASSERT( 0 );
			Release();
			return 1;
		}
	}

	//SetBufferEnd( Callback_BufferEnd );

	DbgOut( "wave : loadFromBuf : success\r\n" );

	return 0;
}


bool XAudioWave::Load( const char *pFileName )
{
	Stop();
	Release();

	FILE *fp = fopen( pFileName, "rb" );
	if( !fp ){
		DbgOut( "wave : Load : fopen error !!!\r\n" );
		_ASSERT( 0 );
		Release();
		return false;
	}

	unsigned char Header[4];
	ZeroMemory( Header, sizeof( unsigned char ) * 4 );
	fread( Header, 4, 1, fp );

	// RIFF
	if( memcmp( Header, "RIFF", 4 ) != 0 ){
		DbgOut( "wave : Load : RIFF error !!!\r\n" );
		_ASSERT( 0 );
		fclose( fp );
		Release();
		return false;
	}

	// File Size
	fread( Header, 4, 1, fp );
	int FileSize = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// WAVE
	fread( Header, 4, 1, fp );
	if( memcmp( Header, "WAVE", 4 ) != 0 ){
		DbgOut( "wave : Load : WAVE header error !!!\r\n" );
		_ASSERT( 0 );
		fclose( fp );
		Release();
		return false;
	}

	// fmt
	fread( Header, 4, 1, fp );
	if( memcmp( Header, "fmt ", 4 ) != 0 ){
		DbgOut( "wave : Load : fmt error !!!\r\n" );
		_ASSERT( 0 );
		fclose( fp );
		Release();
		return false;
	}

	// Linear PCM
	fread( Header, 4, 1, fp );
	int FMTChunkBytes = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// http://www.web-sky.org/program/other/wave.php
	// WAVE_FORMAT_UNKNOWN	0x0000	Microsoft Corporation
	// WAVE_FORMAT_PCM	0x0001	Microsoft PCM format
	// WAVE_FORMAT_MS_ADPCM	0x0002	Microsoft ADPCM
	// WAVE_FORMAT_IEEE_FLOAT	0x0003	Micrososft 32 bit float format
	// WAVE_FORMAT_VSELP	0x0004	Compaq Computer Corporation
	// WAVE_FORMAT_IBM_CVSD	0x0005	IBM Corporation
	// WAVE_FORMAT_ALAW	0x0006	Microsoft Corporation
	// WAVE_FORMAT_MULAW	0x0007	Microsoft Corporation
	// WAVE_FORMAT_OKI_ADPCM	0x0010	OKI
	// WAVE_FORMAT_IMA_ADPCM	0x0011	Intel Corporation
	// WAVE_FORMAT_MEDIASPACE_ADPCM	0x0012	Videologic
	// WAVE_FORMAT_SIERRA_ADPCM	0x0013	Sierra Semiconductor Corp
	// WAVE_FORMAT_G723_ADPCM	0x0014	Antex Electronics Corporation
	// WAVE_FORMAT_DIGISTD	0x0015	DSP Solutions, Inc.
	// WAVE_FORMAT_DIGIFIX	0x0016	DSP Solutions, Inc.
	// WAVE_FORMAT_DIALOGIC_OKI_ADPCM	0x0017	Dialogic Corporation
	// WAVE_FORMAT_MEDIAVISION_ADPCM	0x0018	Media Vision, Inc.
	// WAVE_FORMAT_CU_CODEC	0x0019	Hewlett-Packard Company
	// WAVE_FORMAT_YAMAHA_ADPCM	0x0020	Yamaha Corporation of America
	// WAVE_FORMAT_SONARC	0x0021	Speech Compression
	// WAVE_FORMAT_DSPGROUP_TRUESPEECH	0x0022	DSP Group, Inc
	// WAVE_FORMAT_ECHOSC1	0x0023	Echo Speech Corporation
	// WAVE_FORMAT_AUDIOFILE_AF36	0x0024	Audiofile, Inc.
	// WAVE_FORMAT_APTX	0x0025	Audio Processing Technology
	// WAVE_FORMAT_AUDIOFILE_AF10	0x0026	Audiofile, Inc.
	// WAVE_FORMAT_PROSODY_1612	0x0027	Aculab plc
	// WAVE_FORMAT_LRC	0x0028	Merging Technologies S.A.
	// WAVE_FORMAT_DOLBY_AC2	0x0030	Dolby Laboratories
	// WAVE_FORMAT_GSM610	0x0031	Microsoft Corporation
	// WAVE_FORMAT_MSNAUDIO	0x0032	Microsoft Corporation
	// WAVE_FORMAT_ANTEX_ADPCME	0x0033	Antex Electronics Corporation
	// WAVE_FORMAT_CONTROL_RES_VQLPC	0x0034	Control Resources Limited
	// WAVE_FORMAT_DIGIREAL	0x0035	DSP Solutions, Inc.
	// WAVE_FORMAT_DIGIADPCM	0x0036	DSP Solutions, Inc.
	// WAVE_FORMAT_CONTROL_RES_CR10	0x0037	Control Resources Limited
	// WAVE_FORMAT_NMS_VBXADPCM	0x0038	Natural MicroSystems
	// WAVE_FORMAT_ROLAND_RDAC	0x0039	Roland
	// WAVE_FORMAT_ECHOSC3	0x003A	Echo Speech Corporation
	// WAVE_FORMAT_ROCKWELL_ADPCM	0x003B	Rockwell International
	// WAVE_FORMAT_ROCKWELL_DIGITALK	0x003C	Rockwell International
	// WAVE_FORMAT_XEBEC	0x003D	Xebec Multimedia Solutions Limited
	// WAVE_FORMAT_G721_ADPCM	0x0040	Antex Electronics Corporation
	// WAVE_FORMAT_G728_CELP	0x0041	Antex Electronics Corporation
	// WAVE_FORMAT_MSG723	0x0042	Microsoft Corporation
	// WAVE_FORMAT_MPEG	0x0050	Microsoft Corporation
	// WAVE_FORMAT_RT24	0x0052	InSoft Inc.
	// WAVE_FORMAT_PAC	0x0053	InSoft Inc.
	// WAVE_FORMAT_MPEGLAYER3	0x0055	MPEG 3 Layer 1
	// WAVE_FORMAT_LUCENT_G723	0x0059	Lucent Technologies
	// WAVE_FORMAT_CIRRUS	0x0060	Cirrus Logic
	// WAVE_FORMAT_ESPCM	0x0061	ESS Technology
	// WAVE_FORMAT_VOXWARE	0x0062	Voxware Inc
	// WAVE_FORMAT_CANOPUS_ATRAC	0x0063	Canopus, Co., Ltd.
	// WAVE_FORMAT_G726_ADPCM	0x0064	APICOM
	// WAVE_FORMAT_G722_ADPCM	0x0065	APICOM
	// WAVE_FORMAT_DSAT	0x0066	Microsoft Corporation
	// WAVE_FORMAT_DSAT_DISPLAY	0x0067	Microsoft Corporation
	// WAVE_FORMAT_VOXWARE_BYTE_ALIGNED	0x0069	Voxware Inc.
	// WAVE_FORMAT_VOXWARE_AC8	0x0070	Voxware Inc.
	// WAVE_FORMAT_VOXWARE_AC10	0x0071	Voxware Inc.
	// WAVE_FORMAT_VOXWARE_AC16	0x0072	Voxware Inc.
	// WAVE_FORMAT_VOXWARE_AC20	0x0073	Voxware Inc.
	// WAVE_FORMAT_VOXWARE_RT24	0x0074	Voxware Inc.
	// WAVE_FORMAT_VOXWARE_RT29	0x0075	Voxware Inc.
	// WAVE_FORMAT_VOXWARE_RT29HW	0x0076	Voxware Inc.
	// WAVE_FORMAT_VOXWARE_VR12	0x0077	Voxware Inc.
	// WAVE_FORMAT_VOXWARE_VR18	0x0078	Voxware Inc.
	// WAVE_FORMAT_VOXWARE_TQ40	0x0079	Voxware Inc.
	// WAVE_FORMAT_SOFTSOUND	0x0080	Softsound, Ltd.
	// WAVE_FORMAT_VOXARE_TQ60	0x0081	Voxware Inc.
	// WAVE_FORMAT_MSRT24	0x0082	Microsoft Corporation
	// WAVE_FORMAT_G729A	0x0083	AT&T Laboratories
	// WAVE_FORMAT_MVI_MV12	0x0084	Motion Pixels
	// WAVE_FORMAT_DF_G726	0x0085	DataFusion Systems (Pty) (Ltd)
	// WAVE_FORMAT_DF_GSM610	0x0086	DataFusion Systems (Pty) (Ltd)
	// WAVE_FORMAT_ONLIVE	0x0089	OnLive! Technologies, Inc.
	// WAVE_FORMAT_SBC24	0x0091	Siemens Business Communications Systems
	// WAVE_FORMAT_DOLBY_AC3_SPDIF	0x0092	Sonic Foundry
	// WAVE_FORMAT_ZYXEL_ADPCM	0x0097	ZyXEL Communications, Inc.
	// WAVE_FORMAT_PHILIPS_LPCBB	0x0098	Philips Speech Processing
	// WAVE_FORMAT_PACKED	0x0099	Studer Professional Audio AG
	// WAVE_FORMAT_RHETOREX_ADPCM	0x0100	Rhetorex, Inc.
	// IBM_FORMAT_MULAW	0x0101	IBM mu-law format
	// IBM_FORMAT_ALAW	0x0102	IBM a-law format
	// IBM_FORMAT_ADPCM	0x0103	IBM AVC Adaptive Differential PCM format
	// WAVE_FORMAT_VIVO_G723	0x0111	Vivo Software
	// WAVE_FORMAT_VIVO_SIREN	0x0112	Vivo Software
	// WAVE_FORMAT_DIGITAL_G723	0x0123	Digital Equipment Corporation
	// WAVE_FORMAT_CREATIVE_ADPCM	0x0200	Creative Labs, Inc
	// WAVE_FORMAT_CREATIVE_FASTSPEECH8	0x0202	Creative Labs, Inc
	// WAVE_FORMAT_CREATIVE_FASTSPEECH10	0x0203	Creative Labs, Inc
	// WAVE_FORMAT_QUARTERDECK	0x0220	Quarterdeck Corporation
	// WAVE_FORMAT_FM_TOWNS_SND	0x0300	Fujitsu Corporation
	// WAVE_FORMAT_BZV_DIGITAL	0x0400	Brooktree Corporation
	// WAVE_FORMAT_VME_VMPCM	0x0680	AT&T Labs, Inc.
	// WAVE_FORMAT_OLIGSM	0x1000	Ing C. Olivetti & C., S.p.A.
	// WAVE_FORMAT_OLIADPCM	0x1001	Ing C. Olivetti & C., S.p.A.
	// WAVE_FORMAT_OLICELP	0x1002	Ing C. Olivetti & C., S.p.A.
	// WAVE_FORMAT_OLISBC	0x1003	Ing C. Olivetti & C., S.p.A.
	// WAVE_FORMAT_OLIOPR	0x1004	Ing C. Olivetti & C., S.p.A.
	// WAVE_FORMAT_LH_CODEC	0x1100	Lernout & Hauspie
	// WAVE_FORMAT_NORRIS	0x1400	Norris Communications, Inc.
	// WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS	0x1500	AT&T Labs, Inc.
	// WAVE_FORMAT_DVM	0x2000	FAST Multimedia AG
	// WAVE_FORMAT_INTERWAV_VSC112	0x7150	?????
	// WAVE_FORMAT_EXTENSIBLE	0xFFFE

	fread( Header, 2, 1, fp );
	int FormatID = ( ( (int)Header[1] ) << 8 ) + (int)Header[0];
	if( FormatID != 0x0001 && FormatID != 0xFFFE ){ // 0x0001: WAVE_FORMAT_PCM 0xFFFE: WAVE_FORMAT_EXTENSIBLE
		DbgOut( "wave : Load : FormatID error !!!\r\n" );
		_ASSERT( 0 );

		fclose( fp );
		Release();
		return false;
	}

	// Number of Channels
	fread( Header, 2, 1, fp );
	int NumChannels = ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

/////////
	if( flag3d && (NumChannels >= 2) ){
		::MessageBox( NULL, "３Dサウンドはモノラルのみです。", "エラー", MB_OK );
		_ASSERT( 0 );
		fclose( fp );
		Release();
		return false;
	}
	m_audioState.emitter.ChannelCount = NumChannels;


	// Sampling Rate
	fread( Header, 4, 1, fp );
	int SamplingRate = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// Data Speed
	fread( Header, 4, 1, fp );
	int DataSpeed = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// Block Size
	fread( Header, 2, 1, fp );
	int BlockSize = ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// Bits / Sample
	fread( Header, 2, 1, fp );
	int BitsPerSample = ( ( (int)Header[1] ) << 8 ) + (int)Header[0];

	// Skip
	if( FMTChunkBytes - 16 > 0 ) fseek( fp, FMTChunkBytes - 16, SEEK_CUR );

	// skip chunks
	for( int i = 0; ; ++i ){
		fread( Header, 4, 1, fp );

		if( memcmp( Header, "data", 4 ) == 0 ){
			break;
		}

		fread( Header, 4, 1, fp );

		int Size = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];
		fseek( fp, Size, SEEK_CUR );
		DbgOut( "wave : load : skip Size %d\r\n", Size );
	}


	// data
	// WaveSize (byte)
	fread( Header, 4, 1, fp );
	WaveSize = ( ( (int)Header[3] ) << 24 ) + ( ( (int)Header[2] ) << 16 ) + ( ( (int)Header[1] ) << 8 ) + (int)Header[0];
	DbgOut( "wave : load : WaveSize %d\r\n", WaveSize );


	if( WaveSize > XAUDIO2_MAX_BUFFER_BYTES ) {
		DbgOut( "wave : Load : WaveSize Overflow error !!!\r\n" );
		_ASSERT( 0 );

		fclose( fp );
		Release();
		return false;
	}

	pData = new unsigned char[WaveSize];

	fread( pData, sizeof( char ), WaveSize, fp );

	fclose( fp );

	// PCM audio
//	WAVEFORMATEX WaveFormatEx;

	m_WaveFormatEx.wFormatTag = 1; // PCM audio
	m_WaveFormatEx.nChannels = NumChannels;
	m_WaveFormatEx.nSamplesPerSec = SamplingRate;
	m_WaveFormatEx.wBitsPerSample = BitsPerSample;
	m_WaveFormatEx.nBlockAlign = m_WaveFormatEx.nChannels * m_WaveFormatEx.wBitsPerSample / 8;
	m_WaveFormatEx.nAvgBytesPerSec = m_WaveFormatEx.nSamplesPerSec * m_WaveFormatEx.nBlockAlign;
	m_WaveFormatEx.cbSize = WaveSize;//!!!!! 0

	pStatusCallBack->pXAudioWave = this;


    XAUDIO2_SEND_DESCRIPTOR sendDescriptors;
    sendDescriptors.Flags = XAUDIO2_SEND_USEFILTER; // LPF direct-path
	sendDescriptors.pOutputVoice = XAudio::GetInstance().pMasteringVoice;
    const XAUDIO2_VOICE_SENDS sendList = { 1, &sendDescriptors };

	if( FAILED( XAudio::GetInstance().GetXAudio2()->CreateSourceVoice(
			&pSourceVoice, &m_WaveFormatEx, 0, XAUDIOFREQMAX, pStatusCallBack, &sendList ) ) ){
		DbgOut( "wave : Load : CreateSourceVoice error !!!\r\n" );
		_ASSERT( 0 );
		Release();
		return false;
	}

//	if( !pXAudioFadeEffect->Init( pSourceVoice, NumChannels ) ) return false;

	int ret;
	HRESULT hr;
//	IXAPO* pFadeAPO = NULL;
//	ret = pXAudioFadeEffect->CreateEffect( NumChannels, &pFadeAPO );
	ret = pXAudioFadeEffect->CreateEffect( NumChannels, &m_pFadeAPO );
	if( ret || !m_pFadeAPO ){
		DbgOut( "wave : Load : CreateEffect error !!!\r\n" );
		_ASSERT( 0 );
		Release();
		return false;
	}

DbgOut( "wave : Load : NumChannels %d\r\n", NumChannels );

	XAUDIO2_EFFECT_CHAIN Chain;
	ZeroMemory( &Chain, sizeof( XAUDIO2_EFFECT_CHAIN ) );
	XAUDIO2_EFFECT_DESCRIPTOR Descriptor[2];
	ZeroMemory( Descriptor, sizeof( XAUDIO2_EFFECT_DESCRIPTOR ) * 2 );
	XAUDIO2_EFFECT_DESCRIPTOR FadeDescriptor;
	ZeroMemory( &FadeDescriptor, sizeof( XAUDIO2_EFFECT_DESCRIPTOR ) );

	if( flagreverb ){
		hr = XAudio2CreateReverb( &pRevEffect );
		if( (hr != 0) || !pRevEffect ){
			DbgOut( "wave : Load : CreateReverb error !!!\r\n" );
			_ASSERT( 0 );
			Release();
			return false;
		}

		Descriptor[0].InitialState = true;
		Descriptor[0].OutputChannels = NumChannels;
		Descriptor[0].pEffect = m_pFadeAPO;

		Descriptor[1].InitialState = true;
		//Descriptor[1].OutputChannels = 1;
		Descriptor[1].OutputChannels = NumChannels;
		Descriptor[1].pEffect = pRevEffect;

		Chain.EffectCount = 2;
		Chain.pEffectDescriptors = &(Descriptor[0]);
	}else{
		FadeDescriptor.InitialState = true;
		FadeDescriptor.OutputChannels = NumChannels;
		FadeDescriptor.pEffect = m_pFadeAPO;

		Chain.EffectCount = 1;
		Chain.pEffectDescriptors = &FadeDescriptor;
	}
	
	hr = pSourceVoice->SetEffectChain( &Chain );
	if( hr != S_OK ){
		DbgOut( "wave : load : source SetEffectChain error !!!\r\n" );
		_ASSERT( 0 );
		Release();
		return false;
	}



	FadeEffectParams Param;
	ZeroMemory( &Param, sizeof( FadeEffectParams ) );
	Param.IsFade = false;
	if( FAILED( pSourceVoice->SetEffectParameters( 0, &Param, sizeof( FadeEffectParams ) ) ) ){
		DbgOut( "wave : Load : SetEffectParameters error !!!\r\n" );
		_ASSERT( 0 );
		Release();
		return false;
	}

	if( flagreverb ){
		XAUDIO2FX_REVERB_PARAMETERS reverbParameters;
		reverbParameters.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
		reverbParameters.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
		reverbParameters.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
		reverbParameters.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		reverbParameters.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		reverbParameters.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		reverbParameters.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		reverbParameters.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
		reverbParameters.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
		reverbParameters.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
		reverbParameters.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
		reverbParameters.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
		reverbParameters.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
		reverbParameters.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
		reverbParameters.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
		reverbParameters.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
		reverbParameters.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
		reverbParameters.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
		reverbParameters.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
		reverbParameters.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
		reverbParameters.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
		reverbParameters.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;

		hr = pSourceVoice->SetEffectParameters( 1, &reverbParameters, sizeof( reverbParameters ) );
		if( hr != 0 ){
			DbgOut( "wave : Load : SetEffectParameters 1 error !!!\r\n" );

			_ASSERT( 0 );
			Release();
			return false;
		}
	}

	//SetBufferEnd( Callback_BufferEnd );

	DbgOut( "wave : load : %s success \r\n", pFileName );

	return true;
}

bool XAudioWave::Release()
{
	Stop();

	if( pRevEffect ){
		pRevEffect->Release();
		pRevEffect = NULL;
	}

	if( pSourceVoice ){
		pSourceVoice->FlushSourceBuffers();

		for( int i = 0; i < 10; ++i ){ // 10 は適当
	        XAUDIO2_VOICE_STATE s;
			pSourceVoice->GetState( &s );
			if( s.BuffersQueued == 0 ) break;

			Sleep( 100 );
		}

        pSourceVoice->DestroyVoice();
		pSourceVoice = NULL;

		//ReleaseCallback_BufferEnd( Callback_BufferEnd );
	}

	if( pData ){
		delete [] pData;
		pData = NULL;
	}


	Status = STATUS_WAITING;

	return true;
}

bool XAudioWave::Play( UINT32 startsample, int NumLoops, UINT32 openo )
{
	if( pSourceVoice ){
		//Stop();
		Fade( 0, 1.0f, 1.0f );

		// Submit the wave sample data using an XAUDIO2_BUFFER structure
		XAUDIO2_BUFFER XAudio2Buffer;
		memset( &XAudio2Buffer, 0, sizeof( XAUDIO2_BUFFER ) );

		XAudio2Buffer.PlayBegin = startsample;
		XAudio2Buffer.PlayLength = 0;
//		XAudio2Buffer.PlayLength = pleng;//(WaveSize * 8) / m_WaveFormatEx.wBitsPerSample - startsample;だとハング。

		XAudio2Buffer.AudioBytes = WaveSize;
		XAudio2Buffer.pAudioData = pData;
		XAudio2Buffer.pContext = this;
		XAudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;

		if( NumLoops >= 0 ){
			XAudio2Buffer.LoopCount = NumLoops;
		}
		else{
			XAudio2Buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		}

		if( FAILED( pSourceVoice->SubmitSourceBuffer( &XAudio2Buffer ) ) ){
			_ASSERT( 0 );
			pSourceVoice->DestroyVoice();
			pSourceVoice = NULL;
			return false;
		}

//		_ASSERT( 0 );

		if( FAILED( pSourceVoice->Start( 0, openo ) ) ){
			_ASSERT( 0 );
			return false;
		}

//		_ASSERT( 0 );
	}else{
		_ASSERT( 0 );
	}

	Status = STATUS_PLAYING;

	return true;
}

bool XAudioWave::Resume()
{
	if( pSourceVoice ){
		if( FAILED( pSourceVoice->Start( 0 ) ) ){
			return false;
		}
	}

	Status = STATUS_PLAYING;

	return true;
}

bool XAudioWave::Stop()
{
	bool bret = true;

	if( pSourceVoice ){
		if( FAILED( pSourceVoice->Stop( XAUDIO2_PLAY_TAILS ) ) ){
			bret = false;
		}
		pSourceVoice->FlushSourceBuffers();
	}

	Status = STATUS_STOP;

	return bret;
}

// -2^24 to 2^24
// 1.0 means there is no attenuation or gain and 0 means silence. Negative levels can be used to invert the audio's phase.
bool XAudioWave::SetVolume( const float Volume )
{
	if( Volume < -16777216.0 || Volume > 16777216.0 ) return false;

	if( FAILED( pSourceVoice->SetVolume( Volume ) ) ){
		return false;
	}

	return true;
}

int XAudioWave::GetSamplesPlayed( double* psmp )
{
	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState( &state );

	*psmp = (double)state.SamplesPlayed;

	return 0;
}


float XAudioWave::GetVolume() const
{
	float Volume;
	pSourceVoice->GetVolume( &Volume );

	return Volume;
}

int XAudioWave::SetFrequencyRatio( const float Ratio )
{
	if( Ratio < XAUDIO2_MIN_FREQ_RATIO || Ratio > XAUDIOFREQMAX ) return false;

	freqratio = Ratio;

	if( pSourceVoice ){
		pSourceVoice->SetFrequencyRatio( dpfactor * freqratio );
	}

	return 0;
}

/***
float XAudioWave::GetFrequencyRatio() const
{
	float Ratio;
	pSourceVoice->GetFrequencyRatio( &Ratio );

	return Ratio;
}
***/

bool XAudioWave::Fade( const unsigned int Milliseconds, const float BeginGain, const float EndGain )
{
	FadeEffectParams Param;
	Param.IsFade = true;
	Param.BeginGain = BeginGain;
	Param.EndGain = EndGain;
	Param.Milliseconds = Milliseconds;
	Param.Frames = 0;

	if( FAILED( pSourceVoice->SetEffectParameters( 0, &Param, sizeof( FadeEffectParams ) ) ) ){
		return false;
	}

	return true;
}

// PrevParam.Gainの初期化が必要なので、必ずXAudioWave::Fade( unsigned int Milliseconds, const float BeginGain, const float EndGain )を呼んだ後で使うこと。
bool XAudioWave::Fade( const unsigned int Milliseconds, const float EndGain )
{
	FadeEffectParams PrevParam;
	if( FAILED( pSourceVoice->GetEffectParameters( 0, &PrevParam, sizeof( FadeEffectParams ) ) ) ){
		return false;
	}

	FadeEffectParams Param;
	Param.IsFade = true;
	Param.BeginGain = PrevParam.Gain;
	Param.EndGain = EndGain;
	Param.Milliseconds = Milliseconds;
	Param.Frames = 0;

	if( FAILED( pSourceVoice->SetEffectParameters( 0, &Param, sizeof( FadeEffectParams ) ) ) ){
		return false;
	}

	return true;
}

void XAudioWave::OnBufferEnd( void *p )
{
	Status = STATUS_WAITING;
	if( m_stopcntptr ){
		(*m_stopcntptr)++;
	}

	//pXAudioWave->Status = STATUS_STOP;
	return;
}

void XAudioWave::OnStreamEnd()
{
	return;
}
void XAudioWave::OnVoiceProcessingPassEnd()
{
	return;
}
void XAudioWave::OnVoiceProcessingPassStart()
{
	return;
}
//void XAudioWave::OnBufferEnd( void* p )
//{
//	return;
//}
void XAudioWave::OnBufferStart( void* p )
{
	Status = STATUS_PLAYING;
	XAUDIO2_VOICE_STATE vst;
	pSourceVoice->GetState( &vst );
	startsmp = (double)vst.SamplesPlayed;
	if( m_startcntptr ){
		(*m_startcntptr)++;
	}

	return;
}
void XAudioWave::OnLoopEnd( void* p )
{
	return;
}
void XAudioWave::OnVoiceError( void* p, HRESULT hr )
{
	return;
}


int XAudioWave::FlushSourceBuffers()
{
	pSourceVoice->FlushSourceBuffers();	
	return 0;
}

int XAudioWave::Update3DSound()
{

//    m_audioState.listener.pCone = NULL;
//    m_audioState.emitter.InnerRadius = 0.0f;
//    m_audioState.emitter.InnerRadiusAngle = 0.0f;

    DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER;
//    DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
//        | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
//        | X3DAUDIO_CALCULATE_REVERB;
//        if (g_audioState.fUseRedirectToLFE)
//        {
            // On devices with an LFE channel, allow the mono source data
            // to be routed to the LFE destination channel.
//            dwCalcFlags |= X3DAUDIO_CALCULATE_REDIRECT_TO_LFE;
//        }

m_audioState.dspSettings.SrcChannelCount = 1;
//m_audioState.dspSettings.DstChannelCount = 2;
m_audioState.dspSettings.DstChannelCount = XAudio::GetInstance().m_OutputChannels;
m_audioState.dspSettings.pMatrixCoefficients = m_audioState.matrixCoefficients;

    X3DAudioCalculate( XAudio::GetInstance().x3DInstance, &m_audioState.listener, &m_audioState.emitter, 
		dwCalcFlags, &m_audioState.dspSettings );

//_ASSERT( 0 );

//	char mes[1024];
//	sprintf_s( mes, 1024, "outchannel %d, coef %f, %f", 
//		XAudio::GetInstance().m_OutputChannels,
//		*( m_audioState.dspSettings.pMatrixCoefficients ), *( m_audioState.dspSettings.pMatrixCoefficients + 1) );
//	::MessageBox( NULL, mes, "check", MB_OK );

	_ASSERT( pSourceVoice );

	dpfactor = m_audioState.dspSettings.DopplerFactor;

    // Apply X3DAudio generated DSP settings to XAudio2
	//pSourceVoice->SetFrequencyRatio( m_audioState.dspSettings.DopplerFactor * freqratio );
	pSourceVoice->SetFrequencyRatio( dpfactor * freqratio );

//	pSourceVoice->SetOutputMatrix( XAudio::GetInstance().pMasteringVoice, m_WaveFormatEx.nChannels, 
//		XAudio::GetInstance().m_OutputChannels, m_audioState.dspSettings.pMatrixCoefficients );

	pSourceVoice->SetOutputMatrix( XAudio::GetInstance().pMasteringVoice, 1, 
		XAudio::GetInstance().m_OutputChannels, m_audioState.dspSettings.pMatrixCoefficients );


//	HRESULT hr;
//	hr = pSourceVoice->SetOutputMatrix( XAudio::GetInstance().pMasteringVoice, 
//		1, 
//		2, 
//		m_audioState.dspSettings.pMatrixCoefficients );
//	_ASSERT( hr == S_OK );

//   XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI/6.0f * m_audioState.dspSettings.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
//	pSourceVoice->SetOutputFilterParameters( XAudio::GetInstance().pMasteringVoice, &FilterParametersDirect);

	return 0;
}

int XAudioWave::SetEmitterParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel )
{
	_ASSERT( pSourceVoice );
	m_audioState.vEmitterPos = pos;
	m_audioState.emitter.Position = pos;
	m_audioState.emitter.Velocity = vel;

	return 0;
}

int XAudioWave::SetListenerParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel, D3DXVECTOR3 dir, D3DXVECTOR3 up )
{
    m_audioState.listener.Position = pos;
	m_audioState.listener.Velocity = vel;
	m_audioState.listener.OrientFront = dir;
    m_audioState.listener.OrientTop = up;

	return 0;
}

int XAudioWave::Set3DEmiDist( float dist )
{
    m_audioState.emitter.CurveDistanceScaler = dist;

	return 0;
}

int XAudioWave::GetDopplerScaler( float* dopplerptr )
{
	*dopplerptr = m_audioState.emitter.DopplerScaler;
	return 0;
}
int XAudioWave::SetDopplerScaler( float srcdoppler )
{
	m_audioState.emitter.DopplerScaler = srcdoppler;
	return 0;
}

