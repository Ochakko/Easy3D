#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <AlpChange.h>
#include <MQOMaterial.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CAlpChange::CAlpChange()
{
	InitParams();
}
CAlpChange::~CAlpChange()
{
	DestroyObjs();
}

int CAlpChange::InitParams()
{
	mqomat = 0;
	change = 1.0f;
	return 0;
}

int CAlpChange::DestroyObjs()
{
	return 0;
}
int CAlpChange::SetAlpChange( float srcchange )
{
	change = srcchange;
	return 0;
}