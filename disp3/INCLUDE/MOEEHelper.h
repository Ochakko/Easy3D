#ifndef MOEEHELPERH
#define MOEEHELPERH

#include <coef.h>
class CShdHandler;

#ifdef MOEEGLOBAL
	int CheckAndDelInvalidMOEE( CShdHandler* lpsh, MOEELEM* moeeptr );
	int DelMOEEListByIndex( int selindex, MOEELEM* moeeptr );
	int DelMOEENotListByIndex( int selindex, MOEELEM* moeeptr );
#else
	extern int CheckAndDelInvalidMOEE( CShdHandler* lpsh, MOEELEM* moeeptr );
	extern int DelMOEEListByIndex( int selindex, MOEELEM* moeeptr );
	extern int DelMOEENotListByIndex( int selindex, MOEELEM* moeeptr );
#endif

#endif