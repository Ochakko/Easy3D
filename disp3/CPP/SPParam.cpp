#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <SPParam.h>

#define	DBGH
#include <dbg.h>


CSPParam::CSPParam()
{
	InitParams();
}

CSPParam::~CSPParam()
{
	DestroyObjs();
}

int CSPParam::InitParams()
{
	ZeroMemory( &m_rotparam, sizeof( SPPARAM ) );
	ZeroMemory( &m_mvparam, sizeof( SPPARAM ) );
	ZeroMemory( &m_scparam, sizeof( SPPARAM ) );
	ZeroMemory( &reserved, sizeof( int ) * 4 );

	int ikind;
	for( ikind = INDX; ikind <= INDZ; ikind++ ){
		m_rotparam.ctrlS[ikind].spt = 0.25f;
		m_rotparam.ctrlS[ikind].spdist = 0.0f;
		m_rotparam.ctrlS[ikind].distmax = 10.0f;
		m_rotparam.ctrlS[ikind].tmin = 0.0f;
		m_rotparam.ctrlS[ikind].tmax = 1.0f;
		m_rotparam.ctrlE[ikind].spt = 0.75f;
		m_rotparam.ctrlE[ikind].spdist = 0.0f;
		m_rotparam.ctrlE[ikind].distmax = 10.0f;
		m_rotparam.ctrlE[ikind].tmin = 0.0f;
		m_rotparam.ctrlE[ikind].tmax = 1.0f;
		m_rotparam.syncflag = 0;

		m_mvparam.ctrlS[ikind].spt = 0.25f;
		m_mvparam.ctrlS[ikind].spdist = 0.0f;
		m_mvparam.ctrlS[ikind].distmax = 100.0f;
		m_mvparam.ctrlS[ikind].tmin = 0.0f;
		m_mvparam.ctrlS[ikind].tmax = 1.0f;
		m_mvparam.ctrlE[ikind].spt = 0.75f;
		m_mvparam.ctrlE[ikind].spdist = 0.0f;
		m_mvparam.ctrlE[ikind].distmax = 100.0f;
		m_mvparam.ctrlE[ikind].tmin = 0.0f;
		m_mvparam.ctrlE[ikind].tmax = 1.0f;
		m_mvparam.syncflag = 0;

		m_scparam.ctrlS[ikind].spt = 0.25f;
		m_scparam.ctrlS[ikind].spdist = 0.0f;
		m_scparam.ctrlS[ikind].distmax = 1.0f;
		m_scparam.ctrlS[ikind].tmin = 0.0f;
		m_scparam.ctrlS[ikind].tmax = 1.0f;
		m_scparam.ctrlE[ikind].spt = 0.75f;
		m_scparam.ctrlE[ikind].spdist = 0.0f;
		m_scparam.ctrlE[ikind].distmax = 1.0f;
		m_scparam.ctrlE[ikind].tmin = 0.0f;
		m_scparam.ctrlE[ikind].tmax = 1.0f;
		m_scparam.syncflag = 0;
	}

	return 0;
}

int CSPParam::DestroyObjs()
{
	return 0;
}

/***
int CSPParam::ScaleMvDist( float mult )
{
	int ikind;
	for( ikind = INDX; ikind <= INDZ; ikind++ ){
		m_mvparam.ctrlS[ikind].spdist *= mult;
		m_mvparam.ctrlS[ikind].distmax *= mult;

		m_mvparam.ctrlE[ikind].spdist *= mult;
		m_mvparam.ctrlE[ikind].distmax *= mult;
	}

	return 0;
}
***/