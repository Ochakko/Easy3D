#include "stdafx.h"
#include "XAudio.h"
#include "XAudioVorbis.h"
#include <stdio.h>

#define DBGH
#include "dbg.h"


#pragma warning( disable: 4996 )

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

/***
#ifdef _DEBUG
//#pragma comment( lib, ".\\lib\\vorbis\\win32\\VorbisFile_Static_Debug\\vorbisfile_static_d.lib" )
//#pragma comment( lib, ".\\lib\\vorbis\\win32\\Vorbis_Static_Debug\\vorbis_static_d.lib" )
//#pragma comment( lib, ".\\lib\\ogg\\win32\\Static_Debug\\ogg_static_d.lib" )
#pragma comment( lib, "vorbisfile_static_d.lib" )
#pragma comment( lib, "vorbis_static_d.lib" )
#pragma comment( lib, "ogg_static_d.lib" )
#else
//#pragma comment( lib, ".\\lib\\vorbis\\win32\\VorbisFile_Static_Release\\vorbisfile_static.lib" )
//#pragma comment( lib, ".\\lib\\vorbis\\win32\\Vorbis_Static_Release\\vorbis_static.lib" )
//#pragma comment( lib, ".\\lib\\ogg\\win32\\Static_Release\\ogg_static.lib" )
#pragma comment( lib, "vorbisfile_static.lib" )
#pragma comment( lib, "vorbis_static.lib" )
#pragma comment( lib, "ogg_static.lib" )
#endif

vorbisfile_static.lib vorbis_static.lib ogg_static.lib
***/

XAudioVorbis::XAudioVorbis()
: fp( NULL ), pSourceVoice( NULL ), NumLoops( 0 ), pXAudioFadeEffect( NULL )
{
	pXAudioFadeEffect = new XAudioFadeEffect();
	pStatusCallBack = new VorbisCallback();

	calcsize = 0;
	calcdata = 0;

	pData = 0;
	WaveSize = 0;


	memset( &vf, 0, sizeof( OggVorbis_File ) );

	Status = STATUS_WAITING;


	pRevEffect = NULL;
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

int XAudioVorbis::InitCallbackArray()
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
	CallbackPFUNC[CB_StreamEnd] = &XAudioVorbis::OnStreamEnd;
	CallbackPFUNC[CB_VoiceProcessingPassEnd] = &XAudioVorbis::OnVoiceProcessingPassEnd;
	CallbackPFUNC[CB_VoiceProcessingPassStart] = &XAudioVorbis::OnVoiceProcessingPassStart;

	CallbackPFUNCV[CBV_BufferEnd] = &XAudioVorbis::OnBufferEnd;
	CallbackPFUNCV[CBV_BufferStart] = &XAudioVorbis::OnBufferStart;
	CallbackPFUNCV[CBV_LoopEnd] = &XAudioVorbis::OnLoopEnd;

	CallbackVoiceError = &XAudioVorbis::OnVoiceError;

	return 0;
}

int XAudioVorbis::CreateBuffer()
{

	DestroyBuffer();

	calcdata = (char*)malloc( sizeof( char ) * calcsize );
	if( !calcdata ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( calcdata, sizeof( char ) * calcsize );

	return 0;
}

int XAudioVorbis::DestroyBuffer()
{
	if( calcdata ){
		free( calcdata );
		calcdata = 0;
	}
	//calcsize = 0;
	return 0;
}


int XAudioVorbis::InitAudioState()
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

int XAudioVorbis::SetFlag3D( int srcflag3d )
{
	flag3d = srcflag3d;
	return 0;
}


XAudioVorbis::~XAudioVorbis()
{
	Release();

//	std::vector<PFUNC>::clear();


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

bool XAudioVorbis::Release()
{
	Stop();

	if( pRevEffect ){
		pRevEffect->Release();
		pRevEffect = NULL;
	}


	if( vf.datasource != 0 ){
		ov_clear( &vf );
		memset( &vf, 0, sizeof( OggVorbis_File ) );
	}

	if( fp ){
		fclose( fp );
		fp = NULL;
	}

	if( pSourceVoice ){
		pSourceVoice->FlushSourceBuffers();

		for( int i = 0; i < 10; ++i ){ // 10 は適当
	        XAUDIO2_VOICE_STATE s;
			pSourceVoice->GetState( &s );
			if( s.BuffersQueued == 0 ){
				break;
			}

			Sleep( 100 );
		}

        pSourceVoice->DestroyVoice();
		pSourceVoice = NULL;

		//ReleaseCallback_BufferEnd( Callback_BufferEnd );
	}

	Status = STATUS_WAITING;

	DestroySoundBuffer();

	NumLoops = 0;

	return true;
}

int XAudioVorbis::DestroySoundBuffer( int allflag )
{
	DestroyBuffer();

	if( allflag == 1 ){
		if( pData ){
			free( pData );
			pData = 0;
		}
		WaveSize = 0;
	}

	return 0;
}


bool XAudioVorbis::Load( const char *pFileName )
{
	Release();

	int ret;
	fp = fopen( pFileName, "rb" );
	if( !fp ){
		_ASSERT( 0 );
		Release();
		return false;
	}

	if( ov_open( fp, &vf, NULL, 0 ) < 0 ){
		_ASSERT( 0 );
		fclose( fp );
		Release();
		return false;
	}

	double totaltime = 0.0;
	totaltime = ov_time_total( &vf, -1 );
DbgOut( "vorbis : Load : %s : totaltime %f\r\n", pFileName, totaltime );

	vorbis_info *vi = ov_info( &vf, -1 );
	//ビットレートサイズ　*　サンプリング周波数　*　チャンネル数　*　演奏時間
	calcsize = (int)( 2.0 * (double)vi->rate * (double)vi->channels * totaltime );
	ret = CreateBuffer();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	unsigned int ReadSize = 0;
	Read( calcdata, calcsize, ReadSize, IsBufferedAll );
	if( ReadSize <= 0 || IsBufferedAll == true ){
		_ASSERT( 0 );
	}
	WaveSize = (int)ReadSize;

	if( WaveSize > calcsize ){
		_ASSERT( 0 );
		WaveSize = calcsize;
	}

//double time2;
//time2 = (double)WaveSize / 2.0 / (double)vi->rate / (double)vi->channels;
//DbgOut( "vorbis : Load : time2 %f\r\n", time2 );

//char strchk[256];
//sprintf_s( strchk, 256, "calcsize %d, WaveSize %d, rate %d", calcsize, WaveSize, vi->rate );
//::MessageBox( NULL, strchk, "check", MB_OK );	

	if( WaveSize > 0 ){
		pData = (BYTE*)malloc( sizeof( BYTE ) * WaveSize );
		ZeroMemory( pData, sizeof( BYTE ) * WaveSize );

		MoveMemory( pData, calcdata, WaveSize );
	}else{
		_ASSERT( 0 );
	}

	ret = DestroyBuffer();
	_ASSERT( !ret );


	// PCM audio
	//WAVEFORMATEX WaveFormatEx;

	m_WaveFormatEx.wFormatTag = 1; // PCM audio
	m_WaveFormatEx.nChannels = vi->channels;

	if( flag3d || (vi->channels <= 1) ){
		::MessageBox( NULL, "oggはステレオのみです。", "エラー", MB_OK );
		_ASSERT( 0 );
		Release();
		return false;
	}
	m_audioState.emitter.ChannelCount = vi->channels;


	m_WaveFormatEx.nSamplesPerSec = vi->rate;
	m_WaveFormatEx.wBitsPerSample = 16;
	m_WaveFormatEx.nBlockAlign = m_WaveFormatEx.nChannels * m_WaveFormatEx.wBitsPerSample / 8;
	m_WaveFormatEx.nAvgBytesPerSec = m_WaveFormatEx.nSamplesPerSec * m_WaveFormatEx.nBlockAlign;
	m_WaveFormatEx.cbSize = 0;

	pStatusCallBack->pXAudioVorbis = this;


    XAUDIO2_SEND_DESCRIPTOR sendDescriptors;
    sendDescriptors.Flags = XAUDIO2_SEND_USEFILTER; // LPF direct-path
	sendDescriptors.pOutputVoice = XAudio::GetInstance().pMasteringVoice;
    const XAUDIO2_VOICE_SENDS sendList = { 1, &sendDescriptors };

	if( FAILED( XAudio::GetInstance().GetXAudio2()->CreateSourceVoice(
			&pSourceVoice, &m_WaveFormatEx, 0, XAUDIOFREQMAX, pStatusCallBack, &sendList ) ) ){
		_ASSERT( 0 );
		Release();
		return false;
	}
//	if( !pXAudioFadeEffect->Init( pSourceVoice, vi->channels ) ) return false;



	HRESULT hr;
//	IXAPO* pFadeAPO = NULL;
//	ret = pXAudioFadeEffect->CreateEffect( vi->channels, &pFadeAPO );
	ret = pXAudioFadeEffect->CreateEffect( vi->channels, &m_pFadeAPO );
	if( ret || !m_pFadeAPO ){
		_ASSERT( 0 );
		Release();
		return false;
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
			_ASSERT( 0 );
			Release();
			return false;
		}

		Descriptor[0].InitialState = true;
		Descriptor[0].OutputChannels = vi->channels;
		Descriptor[0].pEffect = m_pFadeAPO;

		Descriptor[1].InitialState = true;
		//Descriptor[1].OutputChannels = 1;
		Descriptor[1].OutputChannels = vi->channels;
		Descriptor[1].pEffect = pRevEffect;

		Chain.EffectCount = 2;
		Chain.pEffectDescriptors = &(Descriptor[0]);
	}else{
		FadeDescriptor.InitialState = true;
		FadeDescriptor.OutputChannels = vi->channels;
		FadeDescriptor.pEffect = m_pFadeAPO;

		Chain.EffectCount = 1;
		Chain.pEffectDescriptors = &FadeDescriptor;
	}
	
	hr = pSourceVoice->SetEffectChain( &Chain );
	if( hr != S_OK ){
		_ASSERT( 0 );
		Release();
		return false;
	}

	//pFadeAPO->Release();

	FadeEffectParams Param;
	Param.IsFade = false;
	if( FAILED( pSourceVoice->SetEffectParameters( 0, &Param, sizeof( FadeEffectParams ) ) ) ){
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
			_ASSERT( 0 );
			Release();
			return false;
		}
	}



//	FadeEffectParams Param;
//	Param.IsFade = false;
//
//	if( FAILED( pSourceVoice->SetEffectParameters( 0, &Param, sizeof( FadeEffectParams ) ) ) ){
//		return false;
//	}

	//SetCallback_BufferEnd( Callback_BufferEnd );

	IsBufferedAll = false;

	return true;
}

void XAudioVorbis::Read( char *pBuffer, const unsigned int BufferSize, unsigned int &ReadSize, bool &IsEnd )
{
	int CurrentSection;
	char Buffer[4096];

	ReadSize = 0;
	IsEnd = false;

	while( 1 ){
		int Size = ov_read( &vf, Buffer, 4096, 0, 2, 1, &CurrentSection );
		if( Size <= 0 ){
			IsEnd = true;
			break;
		}

		memcpy( pBuffer + ReadSize, Buffer, Size );

		ReadSize += Size;
		if( ReadSize + 4096 >= BufferSize ) break;
	}
}


bool XAudioVorbis::Play( UINT32 startsample, int NumLoops, UINT32 openo )
{
	if( pSourceVoice ){
//_ASSERT( 0 );

		Stop();
		Fade( 0, 1.0f, 1.0f );
//_ASSERT( 0 );
		if( WaveSize > 0 ){
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
			}else{
				XAudio2Buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
			}

			if( FAILED( pSourceVoice->SubmitSourceBuffer( &XAudio2Buffer ) ) ){
				pSourceVoice->DestroyVoice();
				pSourceVoice = NULL;
				return false;
			}
		}




		if( NumLoops >= 0 ){
			this->NumLoops = NumLoops;
		}
		else{
			this->NumLoops = XAUDIOVORBIS_LOOP_INFINITE;
		}

//_ASSERT( 0 );
		if( FAILED( pSourceVoice->Start( 0, openo ) ) ){
			_ASSERT( 0 );
			return false;
		}
//_ASSERT( 0 );
	}

	Status = STATUS_PLAYING;

	return true;
}

bool XAudioVorbis::Resume()
{
	if( pSourceVoice ){
		if( FAILED( pSourceVoice->Start( 0 ) ) ){
			return false;
		}
	}

	Status = STATUS_PLAYING;

	return true;
}

bool XAudioVorbis::Stop()
{
	bool bret = true;

	if( pSourceVoice ){
		if( FAILED( pSourceVoice->Stop( XAUDIO2_PLAY_TAILS ) ) ){
			bret = false;
		}
		pSourceVoice->FlushSourceBuffers();
	}

	Status = STATUS_STOP;

//	if( m_stopcntptr ){
//		(*m_stopcntptr)++;
//	}

	return bret;
}


// -2^24 to 2^24
// 1.0 means there is no attenuation or gain and 0 means silence. Negative levels can be used to invert the audio's phase.
bool XAudioVorbis::SetVolume( const float Volume )
{
	if( Volume < -16777216.0 || Volume > 16777216.0 ) return false;

	if( FAILED( pSourceVoice->SetVolume( Volume ) ) ){
		return false;
	}

	return true;
}

int XAudioVorbis::GetSamplesPlayed( double* psmp )
{
	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState( &state );

	*psmp = (double)state.SamplesPlayed;

	return 0;
}

float XAudioVorbis::GetVolume() const
{
	float Volume;
	pSourceVoice->GetVolume( &Volume );

	return Volume;
}

int XAudioVorbis::SetFrequencyRatio( const float Ratio )
{
	if( Ratio < XAUDIO2_MIN_FREQ_RATIO || Ratio > XAUDIOFREQMAX ){
		return 0;
	}

	freqratio = Ratio;

	if( pSourceVoice ){
		pSourceVoice->SetFrequencyRatio( dpfactor * freqratio );
	}

	return 0;
}

float XAudioVorbis::GetFrequencyRatio() const
{
	float Ratio;
	pSourceVoice->GetFrequencyRatio( &Ratio );

	return Ratio;
}

bool XAudioVorbis::Fade( const unsigned int Milliseconds, const float BeginGain, const float EndGain )
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

// PrevParam.Gainの初期化が必要なので、必ずXAudioVorbis::Fade( unsigned int Milliseconds, const float BeginGain, const float EndGain )を呼んだ後で使うこと。
bool XAudioVorbis::Fade( const unsigned int Milliseconds, const float EndGain )
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
void XAudioVorbis::OnBufferStart( void* p )
{
	Status = STATUS_PLAYING;
	XAUDIO2_VOICE_STATE vst;
	if( pSourceVoice ){
		pSourceVoice->GetState( &vst );
		startsmp = (double)vst.SamplesPlayed;
		if( m_startcntptr ){
			(*m_startcntptr)++;
		}
		//DbgOut( "vorbis : Callback : OnBufferStart : startsmp %f\r\n", startsmp );
	}

	return;
}

void XAudioVorbis::OnBufferEnd( void *p )
{
	Status = STATUS_WAITING;
	if( m_stopcntptr ){
		(*m_stopcntptr)++;
	}
//	DbgOut( "checksnd !!! Vorbis : OnBufferEnd wavesize %d, stopcnt %d\r\n", WaveSize, tmpcnt );
}

void XAudioVorbis::OnStreamEnd()
{
	return;
}
void XAudioVorbis::OnVoiceProcessingPassStart()
{
	return;
}
void XAudioVorbis::OnVoiceProcessingPassEnd()
{
	return;
}
//void XAudioVorbis::OnBufferEnd( void* p )
//{
//	return;
//}
void XAudioVorbis::OnLoopEnd( void* p )
{
//	static int loopcnt = 0;
//	DbgOut( "checksnd !!! Vorbis : OnLoopEnd %d, wavesize %d\r\n", loopcnt, WaveSize );
	return;
}
void XAudioVorbis::OnVoiceError( void* p, HRESULT hr )
{
	return;
}

int XAudioVorbis::FlushSourceBuffers()
{
	pSourceVoice->FlushSourceBuffers();	
	return 0;
}

int XAudioVorbis::Update3DSound()
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

//    X3DAudioCalculate( XAudio::GetInstance().x3DInstance, &m_audioState.listener, &m_audioState.emitter, 
//		dwCalcFlags, &m_audioState.dspSettings );

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

int XAudioVorbis::SetEmitterParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel )
{
	_ASSERT( pSourceVoice );
	m_audioState.vEmitterPos = pos;
	m_audioState.emitter.Position = pos;
	m_audioState.emitter.Velocity = vel;
//m_audioState.emitter.Velocity = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	return 0;
}

int XAudioVorbis::SetListenerParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel, D3DXVECTOR3 dir, D3DXVECTOR3 up )
{
    m_audioState.listener.Position = pos;
	m_audioState.listener.Velocity = vel;
	m_audioState.listener.OrientFront = dir;
    m_audioState.listener.OrientTop = up;

	return 0;
}

int XAudioVorbis::Set3DEmiDist( float dist )
{
    m_audioState.emitter.CurveDistanceScaler = dist;

	return 0;
}
int XAudioVorbis::GetDopplerScaler( float* dopplerptr )
{
	*dopplerptr = m_audioState.emitter.DopplerScaler;
	return 0;
}
int XAudioVorbis::SetDopplerScaler( float srcdoppler )
{
	m_audioState.emitter.DopplerScaler = srcdoppler;
	return 0;
}

