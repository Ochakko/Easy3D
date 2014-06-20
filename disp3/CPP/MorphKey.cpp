
#include "StdAfx.h"
#ifndef SHDTREEVIEWH
#include <windows.h>
#endif

#include <coef.h>
#include <MorphKey.h>


CMorphKey::CMorphKey()
{
	InitParams();
}

CMorphKey::~CMorphKey()
{
}

int CMorphKey::InitParams()
{
	frameno = -1;
	target1 = -1;
	target2 = -1;
	master = -1;
	blend1 = 0.0f;
	interp = 0;

	return 0;
}



