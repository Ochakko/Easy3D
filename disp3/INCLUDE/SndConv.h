#ifndef SNDCONVH
#define SNDCONVH

#include <coef.h>

class CSoundSet;

#ifdef SNDCONVCPP
	int SCConvHaku( int srcfps, int srcbpm, ANIMTIMING* tptr );
	int SCConvSec( int srcfps, int srcbpm, ANIMTIMING* tptr );
	int SCConvFrame( int srcfps, int srcbpm, ANIMTIMING* tptr );
	int SCConvSample2Frame( CSoundSet* ssptr, int srcfps, double srcsample, int* dstframe );
	int SCConvFrame2Sample( CSoundSet* ssptr, int srcfps, int srcframe, double* dstsample );
	int SCConvTime2Sample( CSoundSet* ssptr, double srctime, double* dstsample );
#else
	extern int SCConvHaku( int srcfps, int srcbpm, ANIMTIMING* tptr );
	extern int SCConvSec( int srcfps, int srcbpm, ANIMTIMING* tptr );
	extern int SCConvFrame( int srcfps, int srcbpm, ANIMTIMING* tptr );
	extern int SCConvSample2Frame( CSoundSet* ssptr, int srcfps, double srcsample, int* dstframe );
	extern int SCConvFrame2Sample( CSoundSet* ssptr, int srcfps, int srcframe, double* dstsample );
	extern int SCConvTime2Sample( CSoundSet* ssptr, double srctime, double* dstsample );
#endif

#endif