#ifndef ASMFUNCH
#define ASMFUNCH

#include <windows.h>
#include <mmintrin.h>
#include <xmmintrin.h>
#include <D3DX9.h>
#include <matrix2.h>
#include <coef.h>
//#include <stdio.h>

#define DBGH
#include <dbg.h>

typedef struct {
	float x;
	float y;
	float z;
	float w;
} VECTOR4;



__inline void AsmSetMXCSR( int setflag )
{
	DWORD MASK_ON = 0x8840;

	DWORD flag0;
	DWORD flag1;
	DWORD flag2;

	flag0 = _mm_getcsr();
	if( setflag == 1 ){
		flag1 = flag0 | MASK_ON;
	}else{
		flag1 = 0x1F80;
	}
	_mm_setcsr( flag1 );
	
	flag2 = _mm_getcsr();

	DbgOut( "AsmSetMXCSR : bef %x, aft %x\r\n", flag0, flag2 );

}

//__inline void AsmMultMat44DX( D3DXMATRIX* xmat, D3DXMATRIX* ymat, D3DXMATRIX* outmat )
__inline void AsmMultMat44fl( const float* x, const float* y, float* output )
{
	//float* x = (float*)xmat;
	//float* y = (float*)ymat;
	//float* output = (float*)outmat;

	_asm {
		mov		edx, dword ptr[x]			;x
		mov		ecx, dword ptr[y]			;y
		mov		eax, dword ptr[output]		;output
		movss	xmm0, dword ptr[edx]		;xmm0: **** **** **** x[0][0]
		movaps	xmm1, xmmword ptr[ecx]		;xmm1: y[0][3] y[0][2] y[0][1] y[0][0]
		shufps	xmm0, xmm0, 0				;xmm0: x[0][0] x[0][0] x[0][0] x[0][0]
		movss	xmm2, dword ptr[edx+4]		;xmm2: **** **** **** x[0][1]
		mulps	xmm0, xmm1					;xmm0: x[0][0]*y[0][3] x[0][0]*y[0][2] x[0][0]*y[0][1] x[0][0]*y[0][0]
		shufps	xmm2, xmm2, 0				;xmm2: x[0][1] x[0][1] x[0][1] x[0][1]
		movaps	xmm3, xmmword ptr[ecx+16]	;xmm3: y[1][3] y[1][2] y[1][1] y[1][0]
		movss	xmm7, dword ptr[edx+8]		;xmm7: **** **** **** x[0][2]
		mulps	xmm2, xmm3					;xmm2: x[0][1]*y[1][3] x[0][1]*y[1][2] x[0][1]*y[1][1] x[0][1]*y[1][0]
		shufps	xmm7, xmm7, 0				;xmm7: x[0][2] x[0][2] x[0][2] x[0][2]
		addps	xmm0, xmm2					;xmm0: x[0][0]*y[0][3]+x[0][1]*y[1][3] x[0][0]*y[0][2]+x[0][1]*y[1][2] x[0][0]*y[0][1]+x[0][1]*y[1][1] x[0][0]*y[0][0]+x[0][1]*y[1][0]
		movaps	xmm4, xmmword ptr[ecx+32]	;xmm4: y[2][3] y[2][2] y[2][1] y[2][0]
		movss	xmm2, dword ptr[edx+12]		;xmm2: **** **** **** x[0][3]
		mulps	xmm7, xmm4					;xmm7: x[0][2]*y[2][3] x[0][2]*y[2][2] x[0][2]*y[2][1] x[0][2]*y[2][0]
		shufps	xmm2, xmm2, 0				;xmm2: x[0][3] x[0][3] x[0][3] x[0][3]
		addps	xmm0, xmm7					;xmm0: x[0][0]*y[0][3]+x[0][1]*y[1][3]+x[0][2]*y[2][3] x[0][0]*y[0][2]+x[0][1]*y[1][2]+x[0][2]*y[2][2] x[0][0]*y[0][1]+x[0][1]*y[1][1]+x[0][2]*y[2][1] x[0][0]*y[0][0]+x[0][1]*y[1][0]+x[0][2]*y[2][0]
		movaps	xmm5, xmmword ptr[ecx+48]	;xmm5: y[3][3] y[3][2] y[3][1] y[3][0]
		movss	xmm6, dword ptr[edx+16]		;xmm6: **** **** **** x[1][0]
		mulps	xmm2, xmm5					;xmm2: x[0][3]*y[3][3] x[0][3]*y[3][2] x[0][3]*y[3][1] x[0][3]*y[3][0]
		movss	xmm7, dword ptr[edx+20]		;xmm7: **** **** **** x[1][1]
		shufps	xmm6, xmm6, 0				;xmm6: x[1][0] x[1][0] x[1][0] x[1][0]
		addps	xmm0, xmm2					;xmm0: x[0][0]*y[0][3]+x[0][1]*y[1][3]+x[0][2]*y[2][3]+x[0][3]*y[3][3] x[0][0]*y[0][2]+x[0][1]*y[1][2]+x[0][2]*y[2][2]+x[0][3]*y[3][2] x[0][0]*y[0][1]+x[0][1]*y[1][1]+x[0][2]*y[2][1]+x[0][3]*y[3][1] x[0][0]*y[0][0]+x[0][1]*y[1][0]+x[0][2]*y[2][0]+x[0][3]*y[3][0]
		shufps	xmm7, xmm7, 0				;xmm7: x[1][1] x[1][1] x[1][1] x[1][1]
		movlps	qword ptr[eax], xmm0		;output[0][2] [0][3]
		movhps	qword ptr[eax+8], xmm0		;output[0][0] [0][1]
		mulps	xmm7, xmm3					;xmm7: x[1][1]*y[1][3] x[1][1]*y[1][2] x[1][1]*y[1][1] x[1][1]*y[1][0]
		movss	xmm0, dword ptr[edx+24]		;xmm0: **** **** **** x[1][2]
		mulps	xmm6, xmm1					;xmm6: x[1][0]*y[0][3] x[1][0]*y[0][2] x[1][0]*y[0][1] x[1][0]*y[0][0]
		shufps	xmm0, xmm0, 0				;xmm0: x[1][2] x[1][2] x[1][2] x[1][2]
		addps	xmm6, xmm7					;xmm6: x[1][0]*y[0][3]+x[1][1]*y[1][3] x[1][0]*y[0][2]+x[1][1]*y[1][2] x[1][0]*y[0][1]+x[1][1]*y[1][1] x[1][0]*y[0][0]+x[1][1]*y[1][0]
		mulps	xmm0, xmm4					;xmm0: x[1][2]*y[2][3] x[1][2]*y[2][2] x[1][2]*y[2][1] x[1][2]*y[2][0]
		movss	xmm2, dword ptr[edx+36]		;xmm2: **** **** **** x[2][1]
		addps	xmm6, xmm0					;xmm6: x[1][0]*y[0][3]+x[1][1]*y[1][3]+x[1][2]*y[2][3] x[1][0]*y[0][2]+x[1][1]*y[1][2]+x[1][2]*y[2][2] x[1][0]*y[0][1]+x[1][1]*y[1][1]+x[1][2]*y[2][1] x[1][0]*y[0][0]+x[1][1]*y[1][0]+x[1][2]*y[2][0]
		movss	xmm0, dword ptr[edx+28]		;xmm0: **** **** **** x[1][3]
		movss	xmm7, dword ptr[edx+32]		;xmm7: **** **** **** x[2][0]
		shufps	xmm0, xmm0, 0				;xmm0: x[1][3] x[1][3] x[1][3] x[1][3]
		shufps	xmm7, xmm7, 0				;xmm7: x[2][0] x[2][0] x[2][0] x[2][0]
		mulps	xmm0, xmm5					;xmm0: x[1][3]*y[3][3] x[1][3]*y[3][2] x[1][3]*y[3][1] x[1][3]*y[3][0]
		mulps	xmm7, xmm1					;xmm7: x[2][0]*y[0][3] x[2][0]*y[0][2] x[2][0]*y[0][1] x[2][0]*y[0][0]
		addps	xmm6, xmm0					;xmm6: x[1][0]*y[0][3]+x[1][1]*y[1][3]+x[1][2]*y[2][3]+x[1][3]*y[3][3] x[1][0]*y[0][2]+x[1][1]*y[1][2]+x[1][2]*y[2][2]+x[1][3]*y[3][2] x[1][0]*y[0][1]+x[1][1]*y[1][1]+x[1][2]*y[2][1]+x[1][3]*y[3][1] x[1][0]*y[0][0]+x[1][1]*y[1][0]+x[1][2]*y[2][0]+x[1][3]*y[3][0]
		shufps	xmm2, xmm2, 0				;xmm2: x[2][1] x[2][1] x[2][1] x[2][1]
		movlps	qword ptr[eax+16], xmm6		;output[1][2] [1][3]
		movhps	qword ptr[eax+24], xmm6		;output[1][0] [1][1]
		mulps	xmm2, xmm3					;xmm2: x[2][1]*y[1][3] x[2][1]*y[1][2] x[2][1]*y[1][1] x[2][1]*y[1][0]
		movss	xmm6, dword ptr[edx+40]		;xmm6: **** **** **** x[2][2]
		addps	xmm7, xmm2					;xmm7: x[2][0]*y[0][3]+x[2][1]*y[1][3] x[2][0]*y[0][2]+x[2][1]*y[1][2] x[2][0]*y[0][1]+x[2][1]*y[1][1] x[2][0]*y[0][0]+x[2][1]*y[1][0]
		shufps	xmm6, xmm6, 0				;xmm6: x[2][2] x[2][2] x[2][2] x[2][2]
		movss	xmm2, dword ptr[edx+44]		;xmm2: **** **** **** x[2][3]
		mulps	xmm6, xmm4					;xmm6: x[2][2]*y[2][3] x[2][2]*y[2][2] x[2][2]*y[2][1] x[2][2]*y[2][0]
		shufps	xmm2, xmm2, 0				;xmm2: x[2][3] x[2][3] x[2][3] x[2][3]
		addps	xmm7, xmm6					;xmm7: x[2][0]*y[0][3]+x[2][1]*y[1][3]+x[2][2]*y[2][3] x[2][0]*y[0][2]+x[2][1]*y[1][2]+x[2][2]*y[2][2] x[2][0]*y[0][1]+x[2][1]*y[1][1]+x[2][2]*y[2][1] x[2][0]*y[0][0]+x[2][1]*y[1][0]+x[2][2]*y[2][0]
		mulps	xmm2, xmm5					;xmm2: x[2][3]*y[3][3] x[2][3]*y[3][2] x[2][3]*y[3][1] x[2][3]*y[3][0]
		movss	xmm0, dword ptr[edx+52]		;xmm0: **** **** **** x[3][1]
		addps	xmm7, xmm2					;xmm7: x[2][0]*y[0][3]+x[2][1]*y[1][3]+x[2][2]*y[2][3]+x[2][3]*y[3][3] x[2][0]*y[0][2]+x[2][1]*y[1][2]+x[2][2]*y[2][2]+x[2][3]*y[3][2] x[2][0]*y[0][1]+x[2][1]*y[1][1]+x[2][2]*y[2][1]+x[2][3]*y[3][1] x[2][0]*y[0][0]+x[2][1]*y[1][0]+x[2][2]*y[2][0]+x[2][3]*y[3][0]
		shufps	xmm0, xmm0, 0				;xmm0: x[3][1] x[3][1] x[3][1] x[3][1]
		movlps	qword ptr[eax+32], xmm7		;output [2][2] [2][3]
		movss	xmm2, dword ptr[edx+48]		;xmm2: **** **** **** x[3][0]
		movhps	qword ptr[eax+40], xmm7		;output [2][0] [2][1]
		mulps	xmm0, xmm3					;xmm0: x[3][1]*y[1][3] x[3][1]*y[1][2] x[3][1]*y[1][1] x[3][1]*y[1][0]
		shufps	xmm2, xmm2, 0				;xmm2: x[3][0] x[3][0] x[3][0] x[3][0]
		movss	xmm6, dword ptr[edx+56]		;xmm6: **** **** **** x[3][2]
		mulps	xmm2, xmm1					;xmm2: x[3][0]*y[0][3] x[3][0]*y[0][2] x[3][0]*y[0][1] x[3][0]*y[0][0]
		shufps	xmm6, xmm6, 0				;xmm6: x[3][2] x[3][2] x[3][2] x[3][2]
		addps	xmm2, xmm0					;xmm2: x[3][0]*y[0][3]+x[3][1]*y[1][3] x[3][0]*y[0][2]+x[3][1]*y[1][2] x[3][0]*y[0][1]+x[3][1]*y[1][1] x[3][0]*y[0][0]+x[3][1]*y[1][0]
		mulps	xmm6, xmm4					;xmm6: x[3][2]*y[2][3] x[3][2]*y[2][2] x[3][2]*y[2][1] x[3][2]*y[2][0]
		movss	xmm7, dword ptr[edx+60]		;xmm7: **** **** **** x[3][3]
		shufps	xmm7, xmm7, 0				;xmm7: x[3][3] x[3][3] x[3][3] x[3][3]
		addps	xmm2, xmm6					;xmm2: x[3][0]*y[0][3]+x[3][1]*y[1][3]+x[3][2]*y[2][3] x[3][0]*y[0][2]+x[3][1]*y[1][2]+x[3][2]*y[2][2] x[3][0]*y[0][1]+x[3][1]*y[1][1]+x[3][2]*y[2][1] x[3][0]*y[0][0]+x[3][1]*y[1][0]+x[3][2]*y[2][0]
		mulps	xmm7, xmm5					;xmm7: x[3][3]*y[3][3] x[3][3]*y[3][2] x[3][3]*y[3][1] x[3][3]*y[3][0]
		addps	xmm2, xmm7					;xmm2: x[3][0]*y[0][3]+x[3][1]*y[1][3]+x[3][2]*y[2][3]+x[3][3]*y[3][3] x[3][0]*y[0][2]+x[3][1]*y[1][2]+x[3][2]*y[2][2]+x[3][3]*y[3][2] x[3][0]*y[0][1]+x[3][1]*y[1][1]+x[3][2]*y[2][1]+x[3][3]*y[3][1] x[3][0]*y[0][0]+x[3][1]*y[1][0]+x[3][2]*y[2][0]+x[3][3]*y[3][0]
		movaps	xmmword ptr[eax+48], xmm2	;output [3][3] [3][2] [3][1] [3][0]
	}
}

__inline void AsmMultMat44CMat( CMatrix2* xmat, CMatrix2* ymat, CMatrix2* outmat )
{
	float* x = &(xmat->data[0][0]);
	float* y = &(ymat->data[0][0]);
	float* output = &(outmat->data[0][0]);

	_asm {
		mov		edx, dword ptr[x]			;x
		mov		ecx, dword ptr[y]			;y
		mov		eax, dword ptr[output]		;output
		movss	xmm0, dword ptr[edx]		;xmm0: **** **** **** x[0][0]
		movaps	xmm1, xmmword ptr[ecx]		;xmm1: y[0][3] y[0][2] y[0][1] y[0][0]
		shufps	xmm0, xmm0, 0				;xmm0: x[0][0] x[0][0] x[0][0] x[0][0]
		movss	xmm2, dword ptr[edx+4]		;xmm2: **** **** **** x[0][1]
		mulps	xmm0, xmm1					;xmm0: x[0][0]*y[0][3] x[0][0]*y[0][2] x[0][0]*y[0][1] x[0][0]*y[0][0]
		shufps	xmm2, xmm2, 0				;xmm2: x[0][1] x[0][1] x[0][1] x[0][1]
		movaps	xmm3, xmmword ptr[ecx+16]	;xmm3: y[1][3] y[1][2] y[1][1] y[1][0]
		movss	xmm7, dword ptr[edx+8]		;xmm7: **** **** **** x[0][2]
		mulps	xmm2, xmm3					;xmm2: x[0][1]*y[1][3] x[0][1]*y[1][2] x[0][1]*y[1][1] x[0][1]*y[1][0]
		shufps	xmm7, xmm7, 0				;xmm7: x[0][2] x[0][2] x[0][2] x[0][2]
		addps	xmm0, xmm2					;xmm0: x[0][0]*y[0][3]+x[0][1]*y[1][3] x[0][0]*y[0][2]+x[0][1]*y[1][2] x[0][0]*y[0][1]+x[0][1]*y[1][1] x[0][0]*y[0][0]+x[0][1]*y[1][0]
		movaps	xmm4, xmmword ptr[ecx+32]	;xmm4: y[2][3] y[2][2] y[2][1] y[2][0]
		movss	xmm2, dword ptr[edx+12]		;xmm2: **** **** **** x[0][3]
		mulps	xmm7, xmm4					;xmm7: x[0][2]*y[2][3] x[0][2]*y[2][2] x[0][2]*y[2][1] x[0][2]*y[2][0]
		shufps	xmm2, xmm2, 0				;xmm2: x[0][3] x[0][3] x[0][3] x[0][3]
		addps	xmm0, xmm7					;xmm0: x[0][0]*y[0][3]+x[0][1]*y[1][3]+x[0][2]*y[2][3] x[0][0]*y[0][2]+x[0][1]*y[1][2]+x[0][2]*y[2][2] x[0][0]*y[0][1]+x[0][1]*y[1][1]+x[0][2]*y[2][1] x[0][0]*y[0][0]+x[0][1]*y[1][0]+x[0][2]*y[2][0]
		movaps	xmm5, xmmword ptr[ecx+48]	;xmm5: y[3][3] y[3][2] y[3][1] y[3][0]
		movss	xmm6, dword ptr[edx+16]		;xmm6: **** **** **** x[1][0]
		mulps	xmm2, xmm5					;xmm2: x[0][3]*y[3][3] x[0][3]*y[3][2] x[0][3]*y[3][1] x[0][3]*y[3][0]
		movss	xmm7, dword ptr[edx+20]		;xmm7: **** **** **** x[1][1]
		shufps	xmm6, xmm6, 0				;xmm6: x[1][0] x[1][0] x[1][0] x[1][0]
		addps	xmm0, xmm2					;xmm0: x[0][0]*y[0][3]+x[0][1]*y[1][3]+x[0][2]*y[2][3]+x[0][3]*y[3][3] x[0][0]*y[0][2]+x[0][1]*y[1][2]+x[0][2]*y[2][2]+x[0][3]*y[3][2] x[0][0]*y[0][1]+x[0][1]*y[1][1]+x[0][2]*y[2][1]+x[0][3]*y[3][1] x[0][0]*y[0][0]+x[0][1]*y[1][0]+x[0][2]*y[2][0]+x[0][3]*y[3][0]
		shufps	xmm7, xmm7, 0				;xmm7: x[1][1] x[1][1] x[1][1] x[1][1]
		movlps	qword ptr[eax], xmm0		;output[0][2] [0][3]
		movhps	qword ptr[eax+8], xmm0		;output[0][0] [0][1]
		mulps	xmm7, xmm3					;xmm7: x[1][1]*y[1][3] x[1][1]*y[1][2] x[1][1]*y[1][1] x[1][1]*y[1][0]
		movss	xmm0, dword ptr[edx+24]		;xmm0: **** **** **** x[1][2]
		mulps	xmm6, xmm1					;xmm6: x[1][0]*y[0][3] x[1][0]*y[0][2] x[1][0]*y[0][1] x[1][0]*y[0][0]
		shufps	xmm0, xmm0, 0				;xmm0: x[1][2] x[1][2] x[1][2] x[1][2]
		addps	xmm6, xmm7					;xmm6: x[1][0]*y[0][3]+x[1][1]*y[1][3] x[1][0]*y[0][2]+x[1][1]*y[1][2] x[1][0]*y[0][1]+x[1][1]*y[1][1] x[1][0]*y[0][0]+x[1][1]*y[1][0]
		mulps	xmm0, xmm4					;xmm0: x[1][2]*y[2][3] x[1][2]*y[2][2] x[1][2]*y[2][1] x[1][2]*y[2][0]
		movss	xmm2, dword ptr[edx+36]		;xmm2: **** **** **** x[2][1]
		addps	xmm6, xmm0					;xmm6: x[1][0]*y[0][3]+x[1][1]*y[1][3]+x[1][2]*y[2][3] x[1][0]*y[0][2]+x[1][1]*y[1][2]+x[1][2]*y[2][2] x[1][0]*y[0][1]+x[1][1]*y[1][1]+x[1][2]*y[2][1] x[1][0]*y[0][0]+x[1][1]*y[1][0]+x[1][2]*y[2][0]
		movss	xmm0, dword ptr[edx+28]		;xmm0: **** **** **** x[1][3]
		movss	xmm7, dword ptr[edx+32]		;xmm7: **** **** **** x[2][0]
		shufps	xmm0, xmm0, 0				;xmm0: x[1][3] x[1][3] x[1][3] x[1][3]
		shufps	xmm7, xmm7, 0				;xmm7: x[2][0] x[2][0] x[2][0] x[2][0]
		mulps	xmm0, xmm5					;xmm0: x[1][3]*y[3][3] x[1][3]*y[3][2] x[1][3]*y[3][1] x[1][3]*y[3][0]
		mulps	xmm7, xmm1					;xmm7: x[2][0]*y[0][3] x[2][0]*y[0][2] x[2][0]*y[0][1] x[2][0]*y[0][0]
		addps	xmm6, xmm0					;xmm6: x[1][0]*y[0][3]+x[1][1]*y[1][3]+x[1][2]*y[2][3]+x[1][3]*y[3][3] x[1][0]*y[0][2]+x[1][1]*y[1][2]+x[1][2]*y[2][2]+x[1][3]*y[3][2] x[1][0]*y[0][1]+x[1][1]*y[1][1]+x[1][2]*y[2][1]+x[1][3]*y[3][1] x[1][0]*y[0][0]+x[1][1]*y[1][0]+x[1][2]*y[2][0]+x[1][3]*y[3][0]
		shufps	xmm2, xmm2, 0				;xmm2: x[2][1] x[2][1] x[2][1] x[2][1]
		movlps	qword ptr[eax+16], xmm6		;output[1][2] [1][3]
		movhps	qword ptr[eax+24], xmm6		;output[1][0] [1][1]
		mulps	xmm2, xmm3					;xmm2: x[2][1]*y[1][3] x[2][1]*y[1][2] x[2][1]*y[1][1] x[2][1]*y[1][0]
		movss	xmm6, dword ptr[edx+40]		;xmm6: **** **** **** x[2][2]
		addps	xmm7, xmm2					;xmm7: x[2][0]*y[0][3]+x[2][1]*y[1][3] x[2][0]*y[0][2]+x[2][1]*y[1][2] x[2][0]*y[0][1]+x[2][1]*y[1][1] x[2][0]*y[0][0]+x[2][1]*y[1][0]
		shufps	xmm6, xmm6, 0				;xmm6: x[2][2] x[2][2] x[2][2] x[2][2]
		movss	xmm2, dword ptr[edx+44]		;xmm2: **** **** **** x[2][3]
		mulps	xmm6, xmm4					;xmm6: x[2][2]*y[2][3] x[2][2]*y[2][2] x[2][2]*y[2][1] x[2][2]*y[2][0]
		shufps	xmm2, xmm2, 0				;xmm2: x[2][3] x[2][3] x[2][3] x[2][3]
		addps	xmm7, xmm6					;xmm7: x[2][0]*y[0][3]+x[2][1]*y[1][3]+x[2][2]*y[2][3] x[2][0]*y[0][2]+x[2][1]*y[1][2]+x[2][2]*y[2][2] x[2][0]*y[0][1]+x[2][1]*y[1][1]+x[2][2]*y[2][1] x[2][0]*y[0][0]+x[2][1]*y[1][0]+x[2][2]*y[2][0]
		mulps	xmm2, xmm5					;xmm2: x[2][3]*y[3][3] x[2][3]*y[3][2] x[2][3]*y[3][1] x[2][3]*y[3][0]
		movss	xmm0, dword ptr[edx+52]		;xmm0: **** **** **** x[3][1]
		addps	xmm7, xmm2					;xmm7: x[2][0]*y[0][3]+x[2][1]*y[1][3]+x[2][2]*y[2][3]+x[2][3]*y[3][3] x[2][0]*y[0][2]+x[2][1]*y[1][2]+x[2][2]*y[2][2]+x[2][3]*y[3][2] x[2][0]*y[0][1]+x[2][1]*y[1][1]+x[2][2]*y[2][1]+x[2][3]*y[3][1] x[2][0]*y[0][0]+x[2][1]*y[1][0]+x[2][2]*y[2][0]+x[2][3]*y[3][0]
		shufps	xmm0, xmm0, 0				;xmm0: x[3][1] x[3][1] x[3][1] x[3][1]
		movlps	qword ptr[eax+32], xmm7		;output [2][2] [2][3]
		movss	xmm2, dword ptr[edx+48]		;xmm2: **** **** **** x[3][0]
		movhps	qword ptr[eax+40], xmm7		;output [2][0] [2][1]
		mulps	xmm0, xmm3					;xmm0: x[3][1]*y[1][3] x[3][1]*y[1][2] x[3][1]*y[1][1] x[3][1]*y[1][0]
		shufps	xmm2, xmm2, 0				;xmm2: x[3][0] x[3][0] x[3][0] x[3][0]
		movss	xmm6, dword ptr[edx+56]		;xmm6: **** **** **** x[3][2]
		mulps	xmm2, xmm1					;xmm2: x[3][0]*y[0][3] x[3][0]*y[0][2] x[3][0]*y[0][1] x[3][0]*y[0][0]
		shufps	xmm6, xmm6, 0				;xmm6: x[3][2] x[3][2] x[3][2] x[3][2]
		addps	xmm2, xmm0					;xmm2: x[3][0]*y[0][3]+x[3][1]*y[1][3] x[3][0]*y[0][2]+x[3][1]*y[1][2] x[3][0]*y[0][1]+x[3][1]*y[1][1] x[3][0]*y[0][0]+x[3][1]*y[1][0]
		mulps	xmm6, xmm4					;xmm6: x[3][2]*y[2][3] x[3][2]*y[2][2] x[3][2]*y[2][1] x[3][2]*y[2][0]
		movss	xmm7, dword ptr[edx+60]		;xmm7: **** **** **** x[3][3]
		shufps	xmm7, xmm7, 0				;xmm7: x[3][3] x[3][3] x[3][3] x[3][3]
		addps	xmm2, xmm6					;xmm2: x[3][0]*y[0][3]+x[3][1]*y[1][3]+x[3][2]*y[2][3] x[3][0]*y[0][2]+x[3][1]*y[1][2]+x[3][2]*y[2][2] x[3][0]*y[0][1]+x[3][1]*y[1][1]+x[3][2]*y[2][1] x[3][0]*y[0][0]+x[3][1]*y[1][0]+x[3][2]*y[2][0]
		mulps	xmm7, xmm5					;xmm7: x[3][3]*y[3][3] x[3][3]*y[3][2] x[3][3]*y[3][1] x[3][3]*y[3][0]
		addps	xmm2, xmm7					;xmm2: x[3][0]*y[0][3]+x[3][1]*y[1][3]+x[3][2]*y[2][3]+x[3][3]*y[3][3] x[3][0]*y[0][2]+x[3][1]*y[1][2]+x[3][2]*y[2][2]+x[3][3]*y[3][2] x[3][0]*y[0][1]+x[3][1]*y[1][1]+x[3][2]*y[2][1]+x[3][3]*y[3][1] x[3][0]*y[0][0]+x[3][1]*y[1][0]+x[3][2]*y[2][0]+x[3][3]*y[3][0]
		movaps	xmmword ptr[eax+48], xmm2	;output [3][3] [3][2] [3][1] [3][0]
	}
}

__inline void AsmTransformVecBlend( float* src, float* matptr1, float* matptr2, float* rate1, float* rate2, float* dst )
{
	//float* matptr1 = (float*)mat1;
	//float* matptr2 = (float*)mat2;

	_asm {
		mov ecx, dword ptr[src]
		mov eax, dword ptr[matptr1]

		movaps	xmm0, xmmword ptr[ecx] ;src vector
		shufps	xmm0, xmm0, 0 ;//x x x x
		movaps	xmm4, xmmword ptr[eax] ;//00 01 02 03
		mulps	xmm4, xmm0

		movaps	xmm1, xmmword ptr[ecx]
		shufps	xmm1, xmm1, 01010101b ;//y y y y
		movaps	xmm5, xmmword ptr[eax+16] ;//10 11 12 13
		mulps	xmm5, xmm1

		movaps	xmm2, xmmword ptr[ecx]
		shufps	xmm2, xmm2, 10101010b ;//z z z z
		movaps	xmm6, xmmword ptr[eax+32] ;//20 21 22 23
		mulps	xmm6, xmm2

		movaps	xmm3, xmmword ptr[ecx]
		shufps	xmm3, xmm3, 11111111b ;//w w w w
		movaps	xmm7, xmmword ptr[eax+48] ;//30 31 32 33
		mulps	xmm7, xmm3

		addps	xmm4, xmm5
		addps	xmm4, xmm6
		addps	xmm4, xmm7 ;//結果その１

///////////////
		mov	eax, dword ptr[matptr2]

		movaps	xmm5, xmmword ptr[eax]
		mulps	xmm5, xmm0

		movaps	xmm6, xmmword ptr[eax+16]
		mulps	xmm6, xmm1
		addps	xmm5, xmm6

		movaps	xmm6, xmmword ptr[eax+32]
		mulps	xmm6, xmm2
		addps	xmm5, xmm6

		movaps	xmm7, xmmword ptr[eax+48]
		mulps	xmm7, xmm3
		addps	xmm5, xmm7 ;//結果その２

;//////// ブレンド
		mov eax, rate1
		mov edx, rate2


		movss	xmm0, dword ptr[eax]
		shufps	xmm0, xmm0, 0

		movss	xmm1, dword ptr[edx]
		shufps	xmm1, xmm1, 0

		mulps	xmm4, xmm0
		mulps	xmm5, xmm1
		addps	xmm4, xmm5

		mov edx, dword ptr[dst]
		movaps	xmmword ptr[edx], xmm4
	}

}

__inline void AsmTransformVecBlend33( float* src, float* matptr1, float* matptr2, float* rate1, float* rate2, float* dst )
{
	//float* matptr1 = (float*)mat1;
	//float* matptr2 = (float*)mat2;

	_asm {
		mov ecx, dword ptr[src]
		mov eax, dword ptr[matptr1]

		movaps	xmm0, xmmword ptr[ecx] ;src vector
		movaps	xmm4, xmmword ptr[eax] ;//00 01 02 03
		shufps	xmm0, xmm0, 0 ;//x x x x
		mulps	xmm4, xmm0

		movaps	xmm1, xmmword ptr[ecx]
		movaps	xmm5, xmmword ptr[eax+16] ;//10 11 12 13
		shufps	xmm1, xmm1, 01010101b ;//y y y y
		mulps	xmm5, xmm1

		movaps	xmm2, xmmword ptr[ecx]
		movaps	xmm6, xmmword ptr[eax+32] ;//20 21 22 23
		shufps	xmm2, xmm2, 10101010b ;//z z z z
		mulps	xmm6, xmm2

		;//movaps	xmm3, xmmword ptr[ecx]
		;//shufps	xmm3, xmm3, 11111111b ;//w w w w
		movaps	xmm7, xmmword ptr[eax+48] ;//30 31 32 33
		;//mulps	xmm7, xmm3

		addps	xmm4, xmm5
		addps	xmm4, xmm6
		addps	xmm4, xmm7 ;//結果その１

///////////////
		mov	eax, dword ptr[matptr2]

		movaps	xmm5, xmmword ptr[eax]
		mulps	xmm5, xmm0

		movaps	xmm6, xmmword ptr[eax+16]
		mulps	xmm6, xmm1
		addps	xmm5, xmm6

		movaps	xmm6, xmmword ptr[eax+32]
		movaps	xmm7, xmmword ptr[eax+48]
		mulps	xmm6, xmm2
		addps	xmm5, xmm6

		;//mulps	xmm7, xmm3
		addps	xmm5, xmm7 ;//結果その２

;//////// ブレンド
		mov eax, rate1
		mov edx, rate2

		movss	xmm0, dword ptr[eax]
		movss	xmm1, dword ptr[edx]
		shufps	xmm0, xmm0, 0
		shufps	xmm1, xmm1, 0

		mov edx, dword ptr[dst]

		mulps	xmm4, xmm0
		mulps	xmm5, xmm1
		addps	xmm4, xmm5

		movaps	xmmword ptr[edx], xmm4
	}

}

/***
__inline void AsmTransformVecBlendS( float* src, float* matptr1, float* matptr2, float* rate1, float* rate2, float* dst )
{

	//_mm_prefetch( (char*)matptr1, _MM_HINT_NTA );
	//float* stopptr = 0;
	//*stopptr = 1;



	_asm{
		mov		eax, dword ptr[matptr1]
		mov		edx, dword ptr[matptr2]
		mov		esi, dword ptr[dst]
		mov		edi, dword ptr[src]

		mov		ebx, 3
		or		ebx, ebx
		jnz		BLENDS
		
BLENDS:
		push	ebx

		;prefetchnta	[eax]
		;prefetchnta [eax+16]
		;prefetchnta [eax+32]
		;prefetchnta [eax+48]

		;prefetchnta	[edx]
		;prefetchnta [edx+16]
		;prefetchnta [edx+32]
		;prefetchnta [edx+48]

		movss	xmm0, dword ptr[edi];//x
		movss	xmm1, dword ptr[edi+4];//y
		movss	xmm2, dword ptr[edi+8];//z
		movss	xmm4, dword ptr[edi];//x
		movss	xmm5, dword ptr[edi+4];//y
		movss	xmm6, dword ptr[edi+8];//z

		mulss	xmm0, dword ptr[eax];//00
		mulss	xmm4, dword ptr[edx];//00
		mulss	xmm1, dword ptr[eax+16];//10
		mulss	xmm5, dword ptr[edx+16];//10
		mulss	xmm2, dword ptr[eax+32];//20
		mulss	xmm6, dword ptr[edx+32];//20
		addss	xmm0, xmm1
		addss	xmm4, xmm5
		addss	xmm0, xmm2
		addss	xmm4, xmm6
		addss	xmm0, dword ptr[eax+48];//30
		addss	xmm4, dword ptr[edx+48];//30

		mov		ebx, dword ptr[rate1]
		mulss	xmm0, dword ptr[ebx];//xpa * rate1

		mov		ebx, dword ptr[rate2]
		mulss	xmm4, dword ptr[ebx]

		addss	xmm0, xmm4
		movss	dword ptr[esi], xmm0;//xp

		pop ebx

		add	eax, 4
		add edx, 4
		add esi, 4

		sub	ebx, 1
		jnz	BLENDS

	}

}
***/

//12_26_2

__inline void AsmTransformVecBlendS( float* src, float* matptr1, float* matptr2, float* rate1, float* rate2, float* dst )
{

	_asm{
		mov		eax, dword ptr[matptr1]
		mov		edx, dword ptr[matptr2]
		mov		esi, dword ptr[dst]
		mov		edi, dword ptr[src]

;////////// xp
		movss	xmm0, dword ptr[edi];//x
		movss	xmm1, dword ptr[edi+4];//y
		movss	xmm2, dword ptr[edi+8];//z
		movss	xmm4, dword ptr[edi];//x
		movss	xmm5, dword ptr[edi+4];//y
		movss	xmm6, dword ptr[edi+8];//z

		mulss	xmm0, dword ptr[eax];//00
		mulss	xmm4, dword ptr[edx];//00
		mulss	xmm1, dword ptr[eax+16];//10
		mulss	xmm5, dword ptr[edx+16];//10
		mulss	xmm2, dword ptr[eax+32];//20
		mulss	xmm6, dword ptr[edx+32];//20
		addss	xmm0, xmm1
		addss	xmm4, xmm5
		addss	xmm0, xmm2
		addss	xmm4, xmm6
		addss	xmm0, dword ptr[eax+48];//30
		addss	xmm4, dword ptr[edx+48];//30

		mov		ebx, dword ptr[rate1]
		mulss	xmm0, dword ptr[ebx];//xpa * rate1

		mov		ebx, dword ptr[rate2]
		mulss	xmm4, dword ptr[ebx]

		addss	xmm0, xmm4
		movss	dword ptr[esi], xmm0;//xp
		
;/////////// yp
		movss	xmm0, dword ptr[edi];//x
		movss	xmm1, dword ptr[edi+4];//y
		movss	xmm2, dword ptr[edi+8];//z
		movss	xmm4, dword ptr[edi];//x
		movss	xmm5, dword ptr[edi+4];//y
		movss	xmm6, dword ptr[edi+8];//z

		mulss	xmm0, dword ptr[eax+4];//01
		mulss	xmm4, dword ptr[edx+4];//01
		mulss	xmm1, dword ptr[eax+20];//11
		mulss	xmm5, dword ptr[edx+20];//11
		mulss	xmm2, dword ptr[eax+36];//21
		mulss	xmm6, dword ptr[edx+36];//21
		addss	xmm0, xmm1
		addss	xmm4, xmm5
		addss	xmm0, xmm2
		addss	xmm4, xmm6
		addss	xmm0, dword ptr[eax+52];//31
		addss	xmm4, dword ptr[edx+52];//31

		mov		ebx, dword ptr[rate1]
		mulss	xmm0, dword ptr[ebx];//ypa * rate1

		mov		ebx, dword ptr[rate2]
		mulss	xmm4, dword ptr[ebx]

		addss	xmm0, xmm4
		movss	dword ptr[esi+4], xmm0;//yp
		
;/////////// zp
		movss	xmm0, dword ptr[edi];//x
		movss	xmm1, dword ptr[edi+4];//y
		movss	xmm2, dword ptr[edi+8];//z
		movss	xmm4, dword ptr[edi];//x
		movss	xmm5, dword ptr[edi+4];//y
		movss	xmm6, dword ptr[edi+8];//z

		mulss	xmm0, dword ptr[eax+8];//02
		mulss	xmm4, dword ptr[edx+8];//02
		mulss	xmm1, dword ptr[eax+24];//12
		mulss	xmm5, dword ptr[edx+24];//12
		mulss	xmm2, dword ptr[eax+40];//22
		mulss	xmm6, dword ptr[edx+40];//22
		addss	xmm0, xmm1
		addss	xmm4, xmm5
		addss	xmm0, xmm2
		addss	xmm4, xmm6
		addss	xmm0, dword ptr[eax+56];//32
		addss	xmm4, dword ptr[edx+56];//32

		mov		ebx, dword ptr[rate1]
		mulss	xmm0, dword ptr[ebx];//zpa * rate1

		mov		ebx, dword ptr[rate2]
		mulss	xmm4, dword ptr[ebx]

		addss	xmm0, xmm4
		movss	dword ptr[esi+8], xmm0;//yp
	}
}


/***
//12_26_1
__inline void AsmTransformVecBlendS( float* src, float* matptr1, float* matptr2, float* rate1, float* rate2, float* dst )
{

	_asm{
		mov		eax, dword ptr[matptr1]
		mov		edx, dword ptr[matptr2]
		mov		esi, dword ptr[dst]
		mov		edi, dword ptr[src]

;////////// xp
		movss	xmm0, dword ptr[edi];//x
		movss	xmm1, dword ptr[edi+4];//y
		movss	xmm2, dword ptr[edi+8];//z
		mulss	xmm0, dword ptr[eax];//00
		mulss	xmm1, dword ptr[eax+16];//10
		mulss	xmm2, dword ptr[eax+32];//20
		addss	xmm0, xmm1
		addss	xmm0, xmm2
		addss	xmm0, dword ptr[eax+48];//30
		mov		ebx, dword ptr[rate1]
		mulss	xmm0, dword ptr[ebx];//xpa * rate1

		movss	xmm4, dword ptr[edi];//x
		movss	xmm5, dword ptr[edi+4];//y
		movss	xmm6, dword ptr[edi+8];//z
		mulss	xmm4, dword ptr[edx];//00
		mulss	xmm5, dword ptr[edx+16];//10
		mulss	xmm6, dword ptr[edx+32];//20
		addss	xmm4, xmm5
		addss	xmm4, xmm6
		addss	xmm4, dword ptr[edx+48];//30
		mov		ebx, dword ptr[rate2]
		mulss	xmm4, dword ptr[ebx]

		addss	xmm0, xmm4
		movss	dword ptr[esi], xmm0;//xp
		
;/////////// yp
		movss	xmm0, dword ptr[edi];//x
		movss	xmm1, dword ptr[edi+4];//y
		movss	xmm2, dword ptr[edi+8];//z
		mulss	xmm0, dword ptr[eax+4];//01
		mulss	xmm1, dword ptr[eax+20];//11
		mulss	xmm2, dword ptr[eax+36];//21
		addss	xmm0, xmm1
		addss	xmm0, xmm2
		addss	xmm0, dword ptr[eax+52];//31
		mov		ebx, dword ptr[rate1]
		mulss	xmm0, dword ptr[ebx];//ypa * rate1

		movss	xmm4, dword ptr[edi];//x
		movss	xmm5, dword ptr[edi+4];//y
		movss	xmm6, dword ptr[edi+8];//z
		mulss	xmm4, dword ptr[edx+4];//01
		mulss	xmm5, dword ptr[edx+20];//11
		mulss	xmm6, dword ptr[edx+36];//21
		addss	xmm4, xmm5
		addss	xmm4, xmm6
		addss	xmm4, dword ptr[edx+52];//31
		mov		ebx, dword ptr[rate2]
		mulss	xmm4, dword ptr[ebx]

		addss	xmm0, xmm4
		movss	dword ptr[esi+4], xmm0;//yp
		
;/////////// zp
		movss	xmm0, dword ptr[edi];//x
		movss	xmm1, dword ptr[edi+4];//y
		movss	xmm2, dword ptr[edi+8];//z
		mulss	xmm0, dword ptr[eax+8];//02
		mulss	xmm1, dword ptr[eax+24];//12
		mulss	xmm2, dword ptr[eax+40];//22
		addss	xmm0, xmm1
		addss	xmm0, xmm2
		addss	xmm0, dword ptr[eax+56];//32
		mov		ebx, dword ptr[rate1]
		mulss	xmm0, dword ptr[ebx];//zpa * rate1

		movss	xmm4, dword ptr[edi];//x
		movss	xmm5, dword ptr[edi+4];//y
		movss	xmm6, dword ptr[edi+8];//z
		mulss	xmm4, dword ptr[edx+8];//02
		mulss	xmm5, dword ptr[edx+24];//12
		mulss	xmm6, dword ptr[edx+40];//22
		addss	xmm4, xmm5
		addss	xmm4, xmm6
		addss	xmm4, dword ptr[edx+56];//32
		mov		ebx, dword ptr[rate2]
		mulss	xmm4, dword ptr[ebx]

		addss	xmm0, xmm4
		movss	dword ptr[esi+8], xmm0;//yp
	}
}
***/


/***
//12_25_2
__inline void AsmTransformVecBlendS( float* src, float* matptr1, float* matptr2, float* rate1, float* rate2, float* dst )
{

	_asm{
		;pushad
		mov		eax, dword ptr[matptr1]
		mov		edx, dword ptr[matptr2]
		mov		esi, dword ptr[dst]
		mov		edi, dword ptr[src]
		movss	xmm0, dword ptr[edi] ;//x
		movss	xmm1, dword ptr[edi+4] ;//y
		movss	xmm2, dword ptr[edi+8] ;//z
;/////// xpa * rate1	
		movss	xmm3, xmm0
		movss	xmm4, xmm1
		movss	xmm5, xmm2

		mulss	xmm3, dword ptr[eax];00
		mulss	xmm4, dword ptr[eax+16];10
		mulss	xmm5, dword ptr[eax+32];20
		addss	xmm3, xmm4
		addss	xmm3, xmm5
		addss	xmm3, dword ptr[eax+48] ;30//xpa
		mov		ebx, dword ptr[rate1]
		;movss	xmm7, dword ptr[ebx]
		;mulss	xmm3, xmm7
		mulss	xmm3, dword ptr[ebx]
;////// xpb * rate2
		movss	xmm4, xmm0
		movss	xmm5, xmm1
		movss	xmm6, xmm2

		mulss	xmm4, dword ptr[edx]
		mulss	xmm5, dword ptr[edx+16]
		mulss	xmm6, dword ptr[edx+32]
		addss	xmm4, xmm5
		addss	xmm4, xmm6
		addss	xmm4, dword ptr[edx+48]

		mov		ebx, dword ptr[rate2]
		;movss	xmm7, dword ptr[ebx]
		;mulss	xmm4, xmm7
		mulss	xmm4, dword ptr[ebx]
;/////// xp
		addss	xmm3, xmm4
		movss	dword ptr[esi], xmm3
;/////////////
;/////////////

;/////// ypa * rate1	
		movss	xmm3, xmm0
		movss	xmm4, xmm1
		movss	xmm5, xmm2

		mulss	xmm3, dword ptr[eax+4];01
		mulss	xmm4, dword ptr[eax+20];11
		mulss	xmm5, dword ptr[eax+36];21
		addss	xmm3, xmm4
		addss	xmm3, xmm5
		addss	xmm3, dword ptr[eax+52] ;31//ypa
		mov		ebx, dword ptr[rate1]
		;movss	xmm7, dword ptr[ebx]
		;mulss	xmm3, xmm7
		mulss	xmm3, dword ptr[ebx]
;////// ypb * rate2
		movss	xmm4, xmm0
		movss	xmm5, xmm1
		movss	xmm6, xmm2

		mulss	xmm4, dword ptr[edx+4]
		mulss	xmm5, dword ptr[edx+20]
		mulss	xmm6, dword ptr[edx+36]
		addss	xmm4, xmm5
		addss	xmm4, xmm6
		addss	xmm4, dword ptr[edx+52]
		mov		ebx, dword ptr[rate2]
		;movss	xmm7, dword ptr[ebx]
		;mulss	xmm4, xmm7
		mulss	xmm4, dword ptr[ebx]
;/////// yp
		addss	xmm3, xmm4
		movss	dword ptr[esi+4], xmm3
;/////////////
;/////////////


;/////// zpa * rate1	
		movss	xmm3, xmm0
		movss	xmm4, xmm1
		movss	xmm5, xmm2

		mulss	xmm3, dword ptr[eax+8];03
		mulss	xmm4, dword ptr[eax+24];13
		mulss	xmm5, dword ptr[eax+40];23
		addss	xmm3, xmm4
		addss	xmm3, xmm5
		addss	xmm3, dword ptr[eax+56] ;33//zpa
		mov		ebx, dword ptr[rate1]
		;movss	xmm7, dword ptr[ebx]
		;mulss	xmm3, xmm7
		mulss	xmm3, dword ptr[ebx]
;////// zpb * rate2
		movss	xmm4, xmm0
		movss	xmm5, xmm1
		movss	xmm6, xmm2

		mulss	xmm4, dword ptr[edx+8]
		mulss	xmm5, dword ptr[edx+24]
		mulss	xmm6, dword ptr[edx+40]
		addss	xmm4, xmm5
		addss	xmm4, xmm6
		addss	xmm4, dword ptr[edx+56]
		mov		ebx, dword ptr[rate2]
		;movss	xmm7, dword ptr[ebx]
		;mulss	xmm4, xmm7
		mulss	xmm4, dword ptr[ebx]
;/////// zp
		addss	xmm3, xmm4
		movss	dword ptr[esi+8], xmm3
		;popad

	}
}
***/

__inline void AsmTransformVecS( float* src, float* matptr, float* dst )
{

	_asm{
		mov		eax, dword ptr[matptr]
		mov		esi, dword ptr[dst]
		mov		edi, dword ptr[src]

;////////// xp
;/////////// yp
		movss	xmm0, dword ptr[edi];//x
		movss	xmm1, dword ptr[edi+4];//y
		movss	xmm2, dword ptr[edi+8];//z
		movss	xmm3, dword ptr[edi];//x
		movss	xmm4, dword ptr[edi+4];//y
		movss	xmm5, dword ptr[edi+8];//z

		mulss	xmm0, dword ptr[eax];//00
		mulss	xmm3, dword ptr[eax+4];//01
		mulss	xmm1, dword ptr[eax+16];//10
		mulss	xmm4, dword ptr[eax+20];//11
		mulss	xmm2, dword ptr[eax+32];//20
		mulss	xmm5, dword ptr[eax+36];//21
		addss	xmm0, xmm1
		addss	xmm3, xmm4
		addss	xmm0, xmm2
		addss	xmm3, xmm5
		addss	xmm0, dword ptr[eax+48];//30
		addss	xmm3, dword ptr[eax+52];//31

		movss	dword ptr[esi], xmm0;//xp
		movss	dword ptr[esi+4], xmm3;//yp
		
;/////////// zp
		movss	xmm0, dword ptr[edi];//x
		movss	xmm1, dword ptr[edi+4];//y
		movss	xmm2, dword ptr[edi+8];//z

		mulss	xmm0, dword ptr[eax+8];//02
		mulss	xmm1, dword ptr[eax+24];//12
		mulss	xmm2, dword ptr[eax+40];//22
		addss	xmm0, xmm1
		addss	xmm0, xmm2
		addss	xmm0, dword ptr[eax+56];//32

		movss	dword ptr[esi+8], xmm0;//zp

	}

}

/***
//12_25
__inline void AsmTransformVecS( float* src, float* matptr, float* dst )
{

	_asm{
		;pushad
		mov		eax, dword ptr[matptr]
		mov		esi, dword ptr[dst]
		mov		edi, dword ptr[src]
		movss	xmm0, dword ptr[edi] ;//x
		movss	xmm1, dword ptr[edi+4] ;//y
		movss	xmm2, dword ptr[edi+8] ;//z
;/////// xp	
		movss	xmm3, xmm0
		movss	xmm4, xmm1
		movss	xmm5, xmm2

		mulss	xmm3, dword ptr[eax];00
		mulss	xmm4, dword ptr[eax+16];10
		mulss	xmm5, dword ptr[eax+32];20
		addss	xmm3, xmm4
		addss	xmm3, xmm5
		addss	xmm3, dword ptr[eax+48] ;30//xpa

		movss	dword ptr[esi], xmm3
;/////////////
;/////////////

;/////// yp	
		movss	xmm3, xmm0
		movss	xmm4, xmm1
		movss	xmm5, xmm2

		mulss	xmm3, dword ptr[eax+4];01
		mulss	xmm4, dword ptr[eax+20];11
		mulss	xmm5, dword ptr[eax+36];21
		addss	xmm3, xmm4
		addss	xmm3, xmm5
		addss	xmm3, dword ptr[eax+52] ;31//ypa

		movss	dword ptr[esi+4], xmm3
;/////////////
;/////////////


;/////// zpa * rate1	
		movss	xmm3, xmm0
		movss	xmm4, xmm1
		movss	xmm5, xmm2

		mulss	xmm3, dword ptr[eax+8];03
		mulss	xmm4, dword ptr[eax+24];13
		mulss	xmm5, dword ptr[eax+40];23
		addss	xmm3, xmm4
		addss	xmm3, xmm5
		addss	xmm3, dword ptr[eax+56] ;33//zpa

		movss	dword ptr[esi+8], xmm3
		;popad

	}
}
***/


__inline void AsmTransformVec( float* src, float* matptr, float* dst )
{
	//float* matptr = (float*)(mat);

	_asm {
		mov ecx, dword ptr[src]
		mov edx, dword ptr[dst]
		mov eax, dword ptr[matptr]

		movaps	xmm0, xmmword ptr[ecx] ;src vector
		shufps	xmm0, xmm0, 0 ;//x x x x
		movaps	xmm4, xmmword ptr[eax] ;//00 01 02 03
		mulps	xmm0, xmm4

		movaps	xmm1, xmmword ptr[ecx]
		shufps	xmm1, xmm1, 01010101b ;//y y y y
		movaps	xmm5, xmmword ptr[eax+16] ;//10 11 12 13
		mulps	xmm1, xmm5

		movaps	xmm2, xmmword ptr[ecx]
		shufps	xmm2, xmm2, 10101010b ;//z z z z
		movaps	xmm6, xmmword ptr[eax+32] ;//20 21 22 23
		mulps	xmm2, xmm6

		movaps	xmm3, xmmword ptr[ecx]
		shufps	xmm3, xmm3, 11111111b ;//w w w w
		movaps	xmm7, xmmword ptr[eax+48] ;//30 31 32 33
		mulps	xmm3, xmm7

		addps	xmm0, xmm1
		addps	xmm0, xmm2
		addps	xmm0, xmm3

		movaps	xmmword ptr[edx], xmm0
	}

}


__inline void AsmTransformVecGL( float* src, D3DXMATRIX* mat, float* dst )
{
	float* matptr = (float*)(mat);

	_asm {
		mov		ecx, dword ptr[src]
		mov		edx, dword ptr[matptr]
		movlps	xmm6, qword ptr[ecx]	; xmm6:## ## x1 x0
		movlps	xmm0, qword ptr[edx]	; xmm0:### ### y01 y00
		shufps	xmm6, xmm6, 01000100b	; xmm6:x1 x0 x1 x0
		movhps	xmm0, qword ptr[edx+16]	; xmm0:y11 y10 y01 y00
		movlps	xmm7, qword ptr[ecx+8]	; xmm7:## ## x3 x2
		movlps	xmm2, qword ptr[edx+8]	; xmm2:### ### y03 y02
		mulps	xmm0, xmm6				; xmm0:y11*x1 y10*x0 y01*x1 y00*x0
		shufps	xmm7, xmm7, 01000100b	; xmm7:x3 x2 x3 x2
		movhps	xmm2, qword ptr[edx+24]	; xmm2:y13 y12 y03 y02
		movlps	xmm1, qword ptr[edx+32]	; xmm1:### ### y21 y20
		mulps	xmm2, xmm7				; xmm2:y13*x3 y12*x2 y03*x3 y02*x2
		movhps	xmm1, qword ptr[edx+48]	; xmm1:y31 y30 y21 y20
		movlps	xmm3, qword ptr[edx+40]	; xmm3:### ### y23 y22
		mulps	xmm1, xmm6				; xmm1:y31*x1 y30*x0 y21*x1 y20*x0
		movhps	xmm3, qword ptr[edx+56]	; xmm3:y33 y32 y23 y22
		addps	xmm0, xmm2				; xmm0:y11*x1+y13*x3 y10*x0+y12*x2 y01*x1+y03*x3 y00*x0+y02*x2
		mov		eax, dword ptr[dst]
		mulps	xmm3, xmm7				; xmm3:y33*x3 y32*x2 y23*x3 y22*x2
		movaps	xmm4, xmm0				; xmm4:xy111_133 xy100_122 xy011_033 xy000_022
		addps	xmm1, xmm3				; xmm1:y31*x1+y33*x3 y30*x0+y32*x2 y21*x1+y23*x3 y20*x0+y22*x2
		shufps	xmm4, xmm1, 10001000b	; xmm4:xy300_322 xy200_222 xy100_122 xy000_022
		shufps	xmm0, xmm1, 11011101b	; xmm0:xy311_333 xy211_233 xy111_133 xy011_033
		addps	xmm0, xmm4				; xmm0:xy300_311_322_333 xy200_211_222_233 xy100_111_122_133 xy000_011_022_033
		movaps	xmmword ptr[eax], xmm0
	}
};

__inline void AsmNormalize3( D3DXVECTOR3 *pv, float x )
{
	const static ALIGNED float _0_5[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	const static ALIGNED float _1_5[4] = { 1.5f, 1.5f, 1.5f, 1.5f };
	static ALIGNED VECTOR4 tmpret;

	tmpret.x = pv->x;
	tmpret.y = pv->y;
	tmpret.z = pv->z;
	tmpret.w = 0.0f;

	_asm {
		mov		eax, dword ptr[pv]
		movss	xmm0, x
		movaps	xmm2, xmmword ptr[eax]
		shufps	xmm0, xmm0, 0
		rsqrtps	xmm1, xmm0	;approx
		mulps	xmm0, xmm1	;start
		mulps	xmm0, xmm1
		mulps	xmm0, xmm1
		mulps	xmm0, xmmword ptr[_0_5]
		mulps	xmm1, xmmword ptr[_1_5]
		subps	xmm1, xmm0	;final
		mulps	xmm1, xmm2
		movaps	xmmword ptr[tmpret], xmm1
	}

	pv->x = tmpret.x;
	pv->y = tmpret.y;
	pv->z = tmpret.z;
};


// 反逆数を用いて平方根を求める
__inline void AsmSqrt( float* dst, float src )
{
	
	const static ALIGNED float _0_5[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	const static ALIGNED float _1_5[4] = { 1.5f, 1.5f, 1.5f, 1.5f };

	_asm {
		mov eax, src
		movss xmm0, eax
		
		rsqrtss xmm1, xmm0  ;approx
		movss   xmm2, xmm0  ;backup
		mulss   xmm0, xmm1  ;start
		mulss   xmm0, xmm1
		mulss   xmm0, xmm1
		mulss   xmm0, dword ptr[_0_5]
		mulss   xmm1, dword ptr[_1_5]
		subss   xmm1, xmm0  ;final
		mulss   xmm1, xmm2  ;invert

		movss	dword ptr[dst], xmm1
	}

};

// ニュートン・ラフソン法による逆数平方根の精度向上
//
__inline void AsmInvSqrt( float* dst, float src )
{
	const static ALIGNED float _0_5[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	const static ALIGNED float _1_5[4] = { 1.5f, 1.5f, 1.5f, 1.5f };

	_asm {
		mov	eax, src
		movss xmm0, eax
		
		rsqrtss xmm1, xmm0  ;approx
		mulss   xmm0, xmm1  ;start
		mulss   xmm0, xmm1
		mulss   xmm0, xmm1
		mulss   xmm0, dword ptr[_0_5]
		mulss   xmm1, dword ptr[_1_5]
		subss   xmm1, xmm0  ;final

		movss	dword ptr[dst], xmm1
	}

};


// xmm0 レジスタの最下位に逆数を求めたい値が入っているものとし
// 結果は xmm1 レジスタに格納されます。
__inline void AsmInv( float* dst, float src )
{
	_asm {
		mov eax, src
		movss	xmm0, eax
		
		rcpss   xmm1, xmm0  ;approx
		mulss   xmm0, xmm1  ;start
		mulss   xmm0, xmm1
		addss   xmm1, xmm1
		subss   xmm1, xmm0  ;final

		movss	dword ptr[dst], xmm1
	}
};

__inline void AsmSwiz( float *dst, float *src )
{
    _asm {
        mov       eax, dword ptr[src]
        mov       edx, dword ptr[dst]

        movhps    xmm1, qword ptr[eax+16] ;** ** B0 B1
        movlps    xmm1, qword ptr[eax]    ;A0 A1 B0 B1
        movhps    xmm4, qword ptr[eax+48] ;** ** D0 D1
        movlps    xmm4, qword ptr[eax+32] ;C0 C1 D0 D1
        movaps    xmm0, xmm1
        shufps    xmm1, xmm4, 10001000b   ;A1 B1 C1 D1
        movhps    xmm3, qword ptr[eax+24] ;** ** B3 B4
        movlps    xmm3, qword ptr[eax+8]  ;A3 A4 B3 B4
        shufps    xmm0, xmm4, 11011101b   ;A0 B0 C0 D0
        movhps    xmm5, qword ptr[eax+56] ;** ** D3 D4
        movlps    xmm5, qword ptr[eax+40] ;C3 C4 D3 D4
        movaps    xmm2, xmm3
        shufps    xmm3, xmm5, 10001000b   ;A4 B4 C4 D4
        ;add        eax, 64
        movaps    xmmword ptr[edx+16], xmm0
        movaps    xmmword ptr[edx], xmm1
        shufps    xmm2, xmm5, 11011101b   ;A3 B3 C3 D3
        movaps    xmmword ptr[edx+32], xmm3
        movaps    xmmword ptr[edx+48], xmm2
        ;add      edx, 64
    }
}


__inline void AsmClamp( float* clvec, float* smin4, float* smax4 )
{

	_asm {
		mov eax, dword ptr[clvec]
		movaps	xmm2, xmmword ptr[eax]

		mov	eax, dword ptr[smin4]
		mov edx, dword ptr[smax4]

		movaps	xmm0, xmmword ptr[eax]
		movaps	xmm1, xmmword ptr[edx]

						
		maxps	xmm0, xmm2
		movaps	xmmword ptr[eax], xmm0

		minps	xmm1, xmm2
		movaps	xmmword ptr[edx], xmm1
	}
}



#endif