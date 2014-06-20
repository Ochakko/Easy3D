#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <QElem.h>

#define DBGH
#include <dbg.h>


CQElem::CQElem()
{

	InitParams();
}

CQElem::~CQElem()
{
	DestroyObjs();
}

int CQElem::InitParams()
{
	frameno = 0;
	q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	eul.x = 0.0f;
	eul.y = 0.0f;
	eul.z = 0.0f;

	return 0;
}

int CQElem::DestroyObjs()
{
	return 0;
}
