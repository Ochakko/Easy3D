#ifndef LWOSURFH
#define LWOSURFH

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <LWOsurfblok.h>

class CLWOsurf
{
public:
	CLWOsurf();
	~CLWOsurf();

	int SetName( char* srcname );
	int SetSourceName( char* srcsource );

	int SetColor( D3DXVECTOR3 srccol );
	int SetDiff( float srcdiff );
	int SetSpec( float srcspec );
	int SetTran( float srctran );

	int SetSide( int srcside );

	int SetVcol( char* srctype, char* srcname );


	CLWOsurfblok* AddSurfBlock();


private:
	int InitParams();
	int DestroyObjs();

public:
	char* name;
	char* source;

	D3DXVECTOR3 color;
	float diff;
	float spec;
	float tran;

	int side;

	char vcoltypestr[5];
	char* vcolname;

	int sbnum;
	CLWOsurfblok** ppsb;// AddSurfBlockÇ≈ÅA*ppsbÇ…í«â¡ÇµÇƒÇ¢Ç≠ÅB

	//CLWOsurf* next;

};

#endif