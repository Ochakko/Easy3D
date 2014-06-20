#ifndef SYMMOPEH
#define SYMMOPEH

#include <basedat.h>

class CShdHandler;
class CTreeHandler2;
class CMotionPoint;
class CMotionPoint2;

#ifdef SYMMOPEDEC
	int SOGetSymmNo( CTreeHandler2* lpth, CShdHandler* lpsh, int serino, int* symmnoptr, int* symmtypeptr );
	int SOGetSymmNo2( CTreeHandler2* lpth, CShdHandler* lpsh, int serino, int* symmnoptr, int* symmtypeptr );
	int SOSetSymMotionPoint( int symmtype, CMotionPoint2* srcmp, CMotionPoint2* dstmp, int deginvflag );
	int SOSetSymMotionPoint( int symmtype, CMotionPoint* srcmp, CMotionPoint* dstmp, int deginvflag );

	int SOIsSymX( CTreeHandler2* lpth, int serino, int* isxptr );
	int SOIsSymXP( CTreeHandler2* lpth, int serino, int* isxpptr );
	int SOIsSymXM( CTreeHandler2* lpth, int serino, int* isxmptr );
	int SOGetSymXName( CTreeHandler2* lpth, CShdHandler* lpsh, int serino, int* setflagptr, char* dstname, int bufleng, CVec3f* dstpos );
#else
	extern int SOGetSymmNo( CTreeHandler2* lpth, CShdHandler* lpsh, int serino, int* symmnoptr, int* symmtypeptr );
	extern int SOGetSymmNo2( CTreeHandler2* lpth, CShdHandler* lpsh, int serino, int* symmnoptr, int* symmtypeptr );
	extern int SOSetSymMotionPoint( int symmtype, CMotionPoint2* srcmp, CMotionPoint2* dstmp, int deginvflag );
	extern int SOSetSymMotionPoint( int symmtype, CMotionPoint* srcmp, CMotionPoint* dstmp, int deginvflag );

	extern int SOIsSymX( CTreeHandler2* lpth, int serino, int* isxptr );
	extern int SOIsSymXP( CTreeHandler2* lpth, int serino, int* isxpptr );
	extern int SOIsSymXM( CTreeHandler2* lpth, int serino, int* isxmptr );
	extern int SOGetSymXName( CTreeHandler2* lpth, CShdHandler* lpsh, int serino, int* setflagptr, char* dstname, int bufleng, CVec3f* dstpos );
#endif

#endif