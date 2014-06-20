#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <Line.h>

#define	DBGH
#include <dbg.h>


CLine::CLine()
{
	InitParams();
}
CLine::~CLine()
{

}

int CLine::InitParams()
{
	ZeroMemory( &P0, sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &P1, sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &V, sizeof( D3DXVECTOR3 ) );

	return 0;
}

int CLine::SetLineFrom2V( D3DXVECTOR3* srcp1, D3DXVECTOR3* srcp2 )
{
	P0 = *srcp1;
	P1 = *srcp2;
	V = *srcp2 - P0;

	return 0;
}
