#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define SNDCONVCPP
#include <SndConv.h>

#include "c:\pgfile9\RokDeBone2DX\HuSound.h"
#include "c:\pgfile9\RokDeBone2DX\SoundSet.h"
#include "c:\pgfile9\RokDeBone2DX\SoundElem.h"

#include <crtdbg.h>

#define DBGH
#include <dbg.h>


int SCConvHaku( int srcfps, int srcbpm, ANIMTIMING* tptr )
{
	tptr->sec = tptr->haku / (float)srcbpm * 60.0f;
	tptr->frame = (float)srcfps * tptr->sec;

	return 0;
}
int SCConvSec( int srcfps, int srcbpm, ANIMTIMING* tptr )
{
	tptr->haku = tptr->sec / 60.0f * (float)srcbpm;
	tptr->frame = (float)srcfps * tptr->sec;

	return 0;
}
int SCConvFrame( int srcfps, int srcbpm, ANIMTIMING* tptr )
{
	tptr->sec = tptr->frame / (float)srcfps;
	tptr->haku = tptr->sec / 60.0f * (float)srcbpm;

	return 0;
}


int SCConvSample2Frame( CSoundSet* ssptr, int srcfps, double srcsample, int* dstframe )
{
	int ret;
	DWORD samplespersec = 0;
	ret = ssptr->m_bgm->GetSamplesPerSec( &samplespersec );
	if( ret || (samplespersec <= 0) ){
		*dstframe = 0;
		return 0;
	}

	double dframe;
	dframe = srcsample / (double)samplespersec * (double)srcfps;
	*dstframe = (int)( dframe + 0.5 );

	return 0;
}

int SCConvFrame2Sample( CSoundSet* ssptr, int srcfps, int srcframe, double* dstsample )
{
	int ret;
	DWORD samplespersec = 0;
	ret = ssptr->m_bgm->GetSamplesPerSec( &samplespersec );
	if( ret || (samplespersec <= 0) ){
		*dstsample = 0.0;
		return 0;
	}

	*dstsample = (double)srcframe / (double)srcfps * (double)samplespersec;

	return 0;
}

int SCConvTime2Sample( CSoundSet* ssptr, double srctime, double* dstsample )
{
	int ret;
	DWORD samplespersec = 0;
	ret = ssptr->m_bgm->GetSamplesPerSec( &samplespersec );
	if( ret || (samplespersec <= 0) ){
		*dstsample = 0.0;
		return 0;
	}

	*dstsample = srctime * (double)samplespersec;
	return 0;
}