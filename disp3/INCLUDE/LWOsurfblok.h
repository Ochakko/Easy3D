#ifndef LWOSURFBLOKH
#define LWOSURFBLOKH

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

class CLWOsurfblok
{
public:
	CLWOsurfblok();
	~CLWOsurfblok();


private:
	int InitParams();
	int DestroyObjs();

public:
	int SetImOrderStr( char* srcorder );
	int SetImChan( char* srcchan );
	int SetImEnab( int srcenab );

	int SetTmCntr( D3DXVECTOR3 srccntr );
	int SetTmSize( D3DXVECTOR3 srcsize );
	int SetTmRota( D3DXVECTOR3 srcrota );
	int SetTmOref( char* srcoref );
	int SetTmCsys( int srccsys );

	int SetProj( int srcproj );
	int SetAxis( int srcaxis );
	int SetImag( int srcimag );
	int SetWidthWrap( int srcwwrap );
	int SetHeightWrap( int srchwrap );
	int SetWrapW( float srcwrpw );
	int SetWrapH( float srcwrph );

	int SetVmapname( char* srcvmap );


public:
	char* im_orderstr;
	char im_chan[5];
	int	im_enab;

	D3DXVECTOR3 tm_cntr;
	D3DXVECTOR3 tm_size;
	D3DXVECTOR3 tm_rota;
	char* tm_oref;
	int	tm_csys;

	int proj;
	int axis;
	int imag;
	int width_wrap;
	int height_wrap;
	float wrpw;
	float wrph;

	char* vmapname;

};

#endif