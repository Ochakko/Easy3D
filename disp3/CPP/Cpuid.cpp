//
// プロセッサのタイプと拡張命令の使用可・不可をチェックするプログラム
//
#include "stdafx.h"

#include <windows.h>

#include <cpuid.h>

#define SUPPORT_MMX		0x00800000
#define SUPPORT_SSE		0x02000000
#define SUPPORT_SSE2	0x04000000
#define SUPPORT_3DNOW	0x80000000
#define SUPPORT_E3DNOW	0x40000000

//
// それぞれのインストラクションを実際に実行してみてエラーが発生するかどうかを確かめる。
// 仮にプロセッサ側のフラグが立っていたとしても実行された時に例外(エラー)が発生する場合は
// オペレーティングシステムサイドが拡張命令に対応していないということになる。
// CPUIDは必ずプロセッサの拡張命令フラグ検査とオペレーティングシステム実行での
// ２通りの方法を用いてチェックしなければならない。
// 例えばPentium4の最新マシンであったとしてもWindows95上ではSSE命令などを実行することができない。
//

CCpuid::CCpuid()
{
	bCPUID = FALSE, bMMX = FALSE, bSSE = FALSE, bSSE2 = FALSE, b3DNow = FALSE, bE3DNow = FALSE;
	dwStandard = 0, dwFeature = 0;
	dwFamily = 0, dwModel = 0, dwSteppingId = 0;
	ZeroMemory( vd.id, sizeof(vender) );

}

CCpuid::~CCpuid()
{


}


BOOL CCpuid::OSCheckCPUID()
{
	__try {
		_asm {
			xor eax, eax
			cpuid
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( _exception_code() == STATUS_ILLEGAL_INSTRUCTION ) return FALSE;
	}
	return TRUE;
}

BOOL CCpuid::OSCheckMMX()
{
	__try { _asm emms }
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( _exception_code() == STATUS_ILLEGAL_INSTRUCTION ) return FALSE;
	}
	return TRUE;
}

BOOL CCpuid::OSCheckSSE()
{
	__try { _asm andps xmm0, xmm0 }
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( _exception_code() == STATUS_ILLEGAL_INSTRUCTION ) return FALSE;
	}
	return TRUE;
}

BOOL CCpuid::OSCheckSSE2()
{
	__try { _asm andpd xmm0, xmm0 }
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( _exception_code() == STATUS_ILLEGAL_INSTRUCTION ) return FALSE;
	}
	return TRUE;
}

BOOL CCpuid::OSCheck3DNow()
{
	__try { _asm femms }
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( _exception_code() == STATUS_ILLEGAL_INSTRUCTION ) return FALSE;
	}
	return TRUE;
}

BOOL CCpuid::OSCheckE3DNow()
{
	__try { _asm pswapd mm0, mm0
			_asm emms }
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( _exception_code() == STATUS_ILLEGAL_INSTRUCTION ) return FALSE;
	}
	return TRUE;
}


int CCpuid::CheckCPU()
{
	//char str[256];
	//const char *say[2] = { "No", "Yes" };

	vender* vdptr = &vd;
	DWORD* dwFeatptr = &dwFeature;
	DWORD* dwStepptr = &dwSteppingId;
	DWORD* dwModelptr = &dwModel;
	DWORD* dwFamptr = &dwFamily;

	bCPUID = OSCheckCPUID();
	if ( bCPUID )
	{
		_asm {
			pusha
			xor		eax, eax
			cpuid
			mov		esi, vdptr
			mov		[esi].id0, ebx
			mov		[esi].id1, edx
			mov		[esi].id2, ecx
			;mov		vd.id0, ebx
			;mov		vd.id1, edx
			;mov		vd.id2, ecx
			
			or		eax, eax
			jz		FEATURESKIP
			mov		eax, 1
			cpuid
			mov		esi, dwFeatptr
			mov		[esi], edx
			;mov		dwFeature, edx
			mov		edx, eax
			and		eax, 1111b
			mov		esi, dwStepptr
			mov		esi, eax
			;mov		dwSteppingId, eax
			mov		eax, edx
			shr		eax, 4
			and		eax, 1111b
			mov		esi, dwModelptr
			mov		[esi], eax
			;mov		dwModel, eax
			shr		edx, 8
			and		edx, 1111b
			mov		esi, dwFamptr
			mov		[esi], edx
			;mov		dwFamily, edx
FEATURESKIP:
			popa
		}
		if ( dwFeature & SUPPORT_MMX ) bMMX = OSCheckMMX();
		if ( dwFeature & SUPPORT_SSE ) bSSE = OSCheckSSE();
		if ( dwFeature & SUPPORT_SSE2 ) bSSE2 = OSCheckSSE2();
		if ( dwFeature & SUPPORT_3DNOW ) b3DNow = OSCheck3DNow();
		if ( dwFeature & SUPPORT_E3DNOW ) bE3DNow = OSCheckE3DNow();

	}

		/***
		wsprintf( str,
			"CPU Vender: \"%s\"\n"
			"Family: %d  Model: %d  Stepping ID: %d\n"
			"\n"
			"Supported CPUID: %s\n"
			"Supported MMX: %s\n"
			"Supported SSE: %s\n"
			"Supported SSE2: %s\n"
			"Supported 3DNow!: %s\n"
			"Supported Enhanced 3DNow!: %s\n"
			, vender.id
			, dwFamily, dwModel, dwSteppingId
			, say[bCPUID], say[bMMX], say[bSSE], say[bSSE2], say[b3DNow], say[bE3DNow]
		);
	}
	else wsprintf( str, "This CPU is not supported CPUID instruction." );
	MessageBox( NULL, str, "Cpuid", MB_OK );
	***/
  return 0;
}
