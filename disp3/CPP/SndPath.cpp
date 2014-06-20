#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <coef.h>

#define SNDPATHCPP
#include <SndPath.h>

#include <crtdbg.h>

#define DBGH
#include <dbg.h>


int SndMakePath( char* srcdirname, char* srcssfdir, char* filename, char* dstpath )
{
	ZeroMemory( dstpath, sizeof( char ) * MAX_PATH );

	char befdirpat[10] = "..\\";
	char curdirpat[10] = ".\\";

	char dirname[MAX_PATH];
	ZeroMemory( dirname, sizeof( char ) * MAX_PATH );
	if( srcdirname ){
		strcpy_s( dirname, MAX_PATH, srcdirname );
	}
	char ssfdir[MAX_PATH];
	ZeroMemory( ssfdir, sizeof( char ) * MAX_PATH );
	if( srcssfdir ){
		strcpy_s( ssfdir, MAX_PATH, srcssfdir );
	}

	if( srcssfdir && *srcssfdir ){
		if( srcdirname && *srcdirname ){
			int cmp11;
			cmp11 = strncmp( dirname, befdirpat, 3 );
			if( cmp11 == 0 ){
				int ch1 = '\\';
				char* lastenptr;
				lastenptr = strrchr( ssfdir, ch1 );
				if( lastenptr ){
					*lastenptr = 0;
					char* lastenptr2;
					lastenptr2 = strrchr( ssfdir, ch1 );
					if( lastenptr2 ){
						*lastenptr2 = 0;
						strcat_s( ssfdir, MAX_PATH, "\\" );
						strcpy_s( dstpath, MAX_PATH, ssfdir );
						strcat_s( dstpath, MAX_PATH, dirname + 3 );
						strcat_s( dstpath, MAX_PATH, "\\" );
						if( filename ){
							strcat_s( dstpath, MAX_PATH, filename );
						}else{
							strcat_s( dstpath, MAX_PATH, "*.*" );
						}
					}else{
						DbgOut( "SndPath : MakePath : ssfdir lastenptr2 NULL error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					DbgOut( "SndPath : MakePath: ssfdir lastenptr NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				char curdirpat[10] = ".\\";
				int cmp12;
				cmp12 = strncmp( dirname, curdirpat, 2 );
				if( cmp12 == 0 ){
					strcpy_s( dstpath, MAX_PATH, ssfdir );
					strcat_s( dstpath, MAX_PATH, dirname + 2 );
					strcat_s( dstpath, MAX_PATH, "\\" );
					if( filename ){
						strcat_s( dstpath, MAX_PATH, filename );
					}else{
						strcat_s( dstpath, MAX_PATH, "*.*" );
					}
				}else{
					strcpy_s( dstpath, MAX_PATH, ssfdir );
					strcat_s( dstpath, MAX_PATH, dirname );
					strcat_s( dstpath, MAX_PATH, "\\" );
					if( filename ){
						strcat_s( dstpath, MAX_PATH, filename );
					}else{
						strcat_s( dstpath, MAX_PATH, "*.*" );
					}
				}
			}
		}else{
			strcpy_s( dstpath, MAX_PATH, ssfdir );
			if( filename ){
				strcat_s( dstpath, MAX_PATH, filename );
			}else{
				strcat_s( dstpath, MAX_PATH, "*.*" );
			}
		}

	}else{
		char moduledir[MAX_PATH];
		int leng;
		ZeroMemory( moduledir, MAX_PATH );
		leng = GetEnvironmentVariable( (LPCTSTR)"MODULEDIR", (LPTSTR)moduledir, MAX_PATH );
		if( leng >= MAX_PATH ){
			DbgOut( "SndPath : MakePath : GetEnv MODULEDIR error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( srcdirname && *srcdirname ){
			int cmp1;
			cmp1 = strncmp( dirname, befdirpat, 3 );
			if( cmp1 == 0 ){
				int ch1 = '\\';
				char* lastenptr;
				lastenptr = strrchr( moduledir, ch1 );
				if( lastenptr ){
					*lastenptr = 0;
					char* lastenptr2;
					lastenptr2 = strrchr( moduledir, ch1 );
					if( lastenptr2 ){
						*lastenptr2 = 0;
						strcat_s( moduledir, MAX_PATH, "\\" );
						strcpy_s( dstpath, MAX_PATH, moduledir );
						strcat_s( dstpath, MAX_PATH, dirname + 3 );
						strcat_s( dstpath, MAX_PATH, "\\" );
						if( filename ){
							strcat_s( dstpath, MAX_PATH, filename );
						}else{
							strcat_s( dstpath, MAX_PATH, "*.*" );
						}
					}else{
						DbgOut( "SndPath : MakePath : lastenptr2 NULL error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					DbgOut( "SndPath : MakePath : lastenptr NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				int cmp2;
				cmp2 = strncmp( dirname, curdirpat, 2 );
				if( cmp2 == 0 ){
					strcpy_s( dstpath, MAX_PATH, moduledir );
					strcat_s( dstpath, MAX_PATH, dirname + 2 );
					strcat_s( dstpath, MAX_PATH, "\\" );
					if( filename ){
						strcat_s( dstpath, MAX_PATH, filename );
					}else{
						strcat_s( dstpath, MAX_PATH, "*.*" );
					}
				}else{
					strcpy_s( dstpath, MAX_PATH, moduledir );
					strcat_s( dstpath, MAX_PATH, dirname );
					strcat_s( dstpath, MAX_PATH, "\\" );
					if( filename ){
						strcat_s( dstpath, MAX_PATH, filename );
					}else{
						strcat_s( dstpath, MAX_PATH, "*.*" );
					}
				}
			}
		}else{
			strcpy_s( dstpath, MAX_PATH, moduledir );
			if( filename ){
				strcat_s( dstpath, MAX_PATH, filename );
			}else{
				strcat_s( dstpath, MAX_PATH, "*.*" );
			}
		}
	}

	return 0;
}


