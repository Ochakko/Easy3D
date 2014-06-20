#include <stdafx.h> //É_É~Å[

#include <coef.h>


#include <DSChange.h>
#include <DispSwitch.h>


#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CDSChange::CDSChange()
{
	InitParams();
}

CDSChange::~CDSChange()
{
	DestroyObjs();
}

int CDSChange::InitParams()
{
	m_dsptr = 0;
	m_change = 0;

	return 0;
}

int CDSChange::DestroyObjs()
{
	return 0;
}

int CDSChange::SetDSChange( int srcchange )
{
	m_change = srcchange;
	return 0;
}

