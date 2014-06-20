#ifndef FORBIDIDH
#define FORBIDIDH

#include <coef.h>

int SetMCEForbidID( MCELEM* mcelem, int forbidnum, int* forbidid );
int HitTestForbidID( MCELEM* mcelem, int srcid );
int IsSameForbidID( MCELEM* mcelem, int forbidnum, int* forbidid );



#endif