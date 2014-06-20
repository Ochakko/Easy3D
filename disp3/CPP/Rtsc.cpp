/*********************************************************************

        Pentium Read Time-Stamp Counter Class for Windows
        Copyright / Author: Shinji Chiba <ch3@mail.goo.ne.jp>

        History: Jan. 5 2000 - Create.

*********************************************************************/
#include "stdafx.h"
#include <Rtsc.h>
#include <stdio.h>

// �R���X�g���N�^�{�����^�C�}�[�Z�b�g
RTSC::RTSC()
{
	__int64 q;
	m_hprocess = NULL;
	m_hThread = NULL;
	Lock();
	_asm {
		rdtsc
		mov dword ptr[q], eax
		mov dword ptr[q+4], edx
	}

	m_stop = q;

	_asm {
		rdtsc
		mov dword ptr[q], eax
		mov dword ptr[q+4], edx
	}

	// ���߂̎��s���Ԃ���������
	m_code = q - m_stop;

	_asm {
		rdtsc
		mov dword ptr[q], eax
		mov dword ptr[q+4], edx
	}
	Unlock();
	m_start = q;
}

RTSC::~RTSC()
{
	Unlock();
}

// �v���J�n
void RTSC::Start()
{
	__int64 q;
	Lock();
	_asm {
		rdtsc
		mov dword ptr[q], eax
		mov dword ptr[q+4], edx
	}
	m_start = q;
}

// �v���I��
DWORD RTSC::Stop()
{
	__int64 q;
	_asm {
		rdtsc
		mov dword ptr[q], eax
		mov dword ptr[q+4], edx
	}
	Unlock();
	return (DWORD) (q - m_start - m_code);
}

// ��萳�m�ȃN���b�N�����v�邽�߁A�v���Z�X�����b�N����
void RTSC::Lock()
{
	Unlock();
	m_hprocess = ::GetCurrentProcess();
	m_oldclass = ::GetPriorityClass( m_hprocess );
	m_hThread = ::GetCurrentThread();
	m_priority = ::GetThreadPriority( m_hThread );
	::SetThreadPriority( m_hThread, THREAD_PRIORITY_HIGHEST );
	::SetPriorityClass( m_hprocess, REALTIME_PRIORITY_CLASS );
}

// �v���Z�X���A�����b�N����
void RTSC::Unlock()
{
	if ( m_hThread )
	{
		::SetThreadPriority( m_hThread, m_priority );
		m_hThread = NULL;
	}
	if ( m_hprocess )
	{
		::SetPriorityClass( m_hprocess, m_oldclass );
		m_hprocess = NULL;
	}
}

// �v���Z�b�T�̑��x���v������
int RTSC::GetCPUSpeed()
{
	__int64 start, end, freq, time;
	int clocks;

	Lock();
	::QueryPerformanceFrequency( (LARGE_INTEGER *) &freq );
	::QueryPerformanceCounter( (LARGE_INTEGER *) &start );
	_asm {
		rdtsc
		mov		ecx, GET_CPUSPEED_LOOPS
LOOPFREQ:
		loop	LOOPFREQ
		mov		ecx, eax
		rdtsc
		sub		eax, ecx
		mov		dword ptr[clocks], eax
	}
	::QueryPerformanceCounter( (LARGE_INTEGER *) &end );
	Unlock();

	time = ((end - start) * 1000000 / freq);
	return (int) (((__int64) clocks + time / 2) / time);
}
