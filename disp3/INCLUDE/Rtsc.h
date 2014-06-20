/*********************************************************************

        Pentium Read Time-Stamp Counter Class for Windows
        Copyright / Author: Shinji Chiba <ch3@mail.goo.ne.jp>

*********************************************************************/
#ifndef __RTSC_H__
#define __RTSC_H__

#include <windows.h>

#define GET_CPUSPEED_LOOPS	100000

class RTSC
{
private:
	__int64 m_start;
	__int64 m_stop;
	__int64 m_code;
	HANDLE m_hprocess;
	HANDLE m_hThread;
	DWORD m_oldclass;
	DWORD m_priority;

private:
	void Lock();
	void Unlock();

public:
	RTSC();
	~RTSC();
	void Start();
	DWORD Stop();
	int GetCPUSpeed();
};

#endif	/* __RTSC_H__ */
