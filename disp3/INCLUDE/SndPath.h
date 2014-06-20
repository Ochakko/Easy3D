#ifndef SNDPATHH
#define SNDPATHH

#ifdef SNDPATHCPP
	int SndMakePath( char* dirname, char* ssfdir, char* filename, char* dstpath );
#else
	extern int SndMakePath( char* dirname, char* ssfdir, char* filename, char* dstpath );
#endif

#endif