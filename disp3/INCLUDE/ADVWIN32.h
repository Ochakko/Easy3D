/*************************************************************
Module name: AdvWin32.H
Notices: Copyright (c) 1995 Jeffrey Richter
*************************************************************/


/* Disable ridiculous warnings so that the code         */
/* compiles cleanly using warning level 4.              */

/* nonstandard extension 'single line comment' was used */
#pragma warning(disable: 4001)

// nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)

// nonstandard extension used : bit field types other than int
#pragma warning(disable: 4214)

// Note: Creating precompiled header 
#pragma warning(disable: 4699)

// unreferenced inline function has been removed
#pragma warning(disable: 4514)

// unreferenced formal parameter
#pragma warning(disable: 4100)

// 'type' differs in indirection to slightly different base 
// types from 'other type'
#pragma warning(disable: 4057)

// named type definition in parentheses
#pragma warning(disable: 4115)

// nonstandard extension used : benign typedef redefinition
#pragma warning(disable: 4209)

/////////////////////////////////////////////////////////////

// Force all EXEs/DLLs to use STRICT type checking.
#define STRICT

/////////////////////////////////////////////////////////////

// Force all EXEs/DLLs to be compiled for Unicode.
// Uncomment the line below to compile using Unicode strings
// #define UNICODE
#ifdef UNICODE
#define _UNICODE
#endif

/////////////////////////////////////////////////////////////

// Create an ARRAY_SIZE macro that returns the number of
// elements in an array. This is a handy macro that I use
// frequently throughout the sample applications.
#define ARRAY_SIZE(Array) \
   (sizeof(Array) / sizeof((Array)[0]))

/////////////////////////////////////////////////////////////

// Create a BEGINTHREADEX macro that calls the C run-time's
// _beginthreadex function. The C run-time library doesn't
// want to have any reliance on Win32 data types such as
// HANDLE. This means that a Win32 programmer needs to cast
// the return value to a HANDLE. This is terribly inconvenient,
// so I have created this macro to perform the casting.
typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define BEGINTHREADEX(lpsa, cbStack, lpStartAddr, \
   lpvThreadParm, fdwCreate, lpIDThread)          \
      ((HANDLE)_beginthreadex(                    \
         (void *) (lpsa),                         \
         (unsigned) (cbStack),                    \
         (PTHREAD_START) (lpStartAddr),           \
         (void *) (lpvThreadParm),                \
         (unsigned) (fdwCreate),                  \
         (unsigned *) (lpIDThread)))


/////////////////////////////////////////////////////////////


// Compile all CONTEXT structures to use 32-bit members
// instead of 16-bit members.  Currently, the only sample
// application that requires this is TInjLib.16 in order
// for it to work correctly on the DEC Alpha AXP.
#define _PORTABLE_32BIT_CONTEXT


/////////////////////////////////////////////////////////////


// Force all EXEs and DLLs to be built for Windows 4.0.
// Comment out the one line below to create samples
// that run under Windows NT 3.1 or Win32s.
// NOTE: Windows NT 3.5 runs Win32 programs marked as 4.0.
//#pragma comment(lib, "msvcrt " "-subsystem:Windows,4.0")


////////////////////////// End Of File //////////////////////
