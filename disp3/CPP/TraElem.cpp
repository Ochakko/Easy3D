#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <TraElem.h>

#define DBGH
#include <dbg.h>


CTraElem::CTraElem()
{

	InitParams();
}

CTraElem::~CTraElem()
{
	DestroyObjs();
}

int CTraElem::InitParams()
{
	frameno = 0;
	ZeroMemory( &tra, sizeof( D3DXVECTOR3 ) );

	return 0;
}

int CTraElem::DestroyObjs()
{
	return 0;
}
