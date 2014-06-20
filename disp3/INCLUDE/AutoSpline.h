#ifndef AUTOSPLINEH
#define AUTOSPLINEH

class CMotHandler;
class CMotionPoint2;

#ifdef AUTOSPLINECPP
	int AutoSplineAll( CMotHandler* lpmh, int motid, int boneno, int mkind, int changefumode );
	int AutoSplineOne( int mkind, CMotionPoint2* mp, int changefumode );
#else
	extern int AutoSplineAll( CMotHandler* lpmh, int motid, int boneno, int mkind, int changefumode );
	extern int AutoSplineOne( int mkind, CMotionPoint2* mp, int changefumode );
#endif

#endif