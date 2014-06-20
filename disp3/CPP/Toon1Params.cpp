#include "StdAfx.h"
#ifndef SHDTREEVIEWH
#include <windows.h>
#endif

#include <coef.h>

#define	DBGH
#include <dbg.h>

#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>


#include <Toon1Params.h>


//static int s_alloccnt = 0;

// global
int g_toon1matcnt = 0;

#ifndef INEASY3D
#include "c:\pgfile9\RokDeBone2DX\inidata.h"
extern CIniData* g_inidata;
#endif

CToon1Params::CToon1Params()
{
	g_toon1matcnt++;
	InitParams();
}

CToon1Params::~CToon1Params()
{
	if( tex ){
		tex->Release();
		tex = 0;
	}
}

int CToon1Params::InitParams()
{
	sprintf_s( name, 32, "Material%d", g_toon1matcnt );

	diffuse.r = 1.0f;
	diffuse.g = 1.0f;
	diffuse.b = 1.0f;
	diffuse.a = 1.0f;

	ambient.r = 0.25f;
	ambient.g = 0.25f;
	ambient.b = 0.25f;
	ambient.a = 1.0f;

	specular.r = 1.0f;
	specular.g = 1.0f;
	specular.b = 1.0f;
	specular.a = 1.0f;

	darkh = 0.333f;
	brighth = 0.666f;

	ambientv = 0.15f;
	diffusev = 0.5f;
	specularv = 0.9f;

	darknl = 0.4f;
	brightnl = 0.9f;


	tex = 0;

	edgecol0.r = 0.0f;
	edgecol0.g = 0.0f;
	edgecol0.b = 0.0f;
//	edgecol0.a = 8.0f;

#ifndef INEASY3D
	edgecol0.a = g_inidata->toonedgew;
#else
	edgecol0.a = 0.8f;
#endif

	edgevalid0 = 1;
	edgeinv0 = 0;

	toon0dnl = 0.4f;
	toon0bnl = 0.9f;

	return 0;
}


int CToon1Params::SetDiffuse( D3DCOLORVALUE srcdiff, int overlayflag )
{
	diffuse = srcdiff;

	if( overlayflag ){
		
		float pb = 0.0f;
		float fr, fg, fb;
		if( diffuse.r < 0.5f ){
			fr = 2.0f * diffuse.r * pb;
		}else{
			fr = 1.0f - 2.0f * (1.0f - diffuse.r) * (1.0f - pb);
		}

		if( diffuse.g < 0.5f ){
			fg = 2.0f * diffuse.g * pb;
		}else{
			fg = 1.0f - 2.0f * (1.0f - diffuse.g) * (1.0f - pb);
		}

		if( diffuse.b < 0.5f ){
			fb = 2.0f * diffuse.b * pb;
		}else{
			fb = 1.0f - 2.0f * (1.0f - diffuse.b) * (1.0f - pb);
		}

		ambient.r = fr;
		ambient.g = fg;
		ambient.b = fb;
		ambient.a = 1.0f;
////////////
		float pb2 = 0.9f;
		float fr2, fg2, fb2;
		if( diffuse.r < 0.5f ){
			fr2 = 2.0f * diffuse.r * pb2;
		}else{
			fr2 = 1.0f - 2.0f * (1.0f - diffuse.r) * (1.0f - pb2);
		}

		if( diffuse.g < 0.5f ){
			fg2 = 2.0f * diffuse.g * pb2;
		}else{
			fg2 = 1.0f - 2.0f * (1.0f - diffuse.g) * (1.0f - pb2);
		}

		if( diffuse.b < 0.5f ){
			fb2 = 2.0f * diffuse.b * pb2;
		}else{
			fb2 = 1.0f - 2.0f * (1.0f - diffuse.b) * (1.0f - pb2);
		}

		specular.r = fr2;
		specular.g = fg2;
		specular.b = fb2;
		specular.a = 1.0f;
/////////////////
#ifndef INEASY3D
		edgecol0.r = ambient.r * g_inidata->toonedgecol;
		edgecol0.g = ambient.g * g_inidata->toonedgecol;
		edgecol0.b = ambient.b * g_inidata->toonedgecol;
#else
		edgecol0.r = ambient.r * 0.6f;
		edgecol0.g = ambient.g * 0.6f;
		edgecol0.b = ambient.b * 0.6f;
#endif
		edgecol0.r = min( 1.0f, edgecol0.r );
		edgecol0.g = min( 1.0f, edgecol0.g );
		edgecol0.b = min( 1.0f, edgecol0.b );
		//edgecol0.a = 1.0f;//<--- alpha‚ÍAedgewidth

	}

	return 0;
}

int CToon1Params::CreateTexture( LPDIRECT3DDEVICE9 pdev )
{

	if( tex ){
		tex->Release();
		tex = 0;
	}

	HRESULT hr;
	hr = D3DXCreateTexture( pdev, 32, 64, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex );
	if( hr != D3D_OK ){
		DbgOut( "toon1params : CreateTexture : D3DXCreateTexture error %x\r\n", hr );
		_ASSERT( 0 );
		return 1;
	}

//////////
	D3DSURFACE_DESC	sdesc;
	hr = tex->GetLevelDesc( 0, &sdesc );
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}
	D3DFORMAT fmt = sdesc.Format;

	if( (fmt != D3DFMT_X8R8G8B8) && (fmt != D3DFMT_A8R8G8B8) ){
		DbgOut( "toon1params : CreateTexture : d3dformat %d is not supported error !!!\n", fmt );
		_ASSERT( 0 );
		return 1;
	}

	int w = sdesc.Width;
	int h = sdesc.Height;
	D3DLOCKED_RECT lockedrect;
	RECT rect;

	rect.left = 0; rect.right = w; rect.top = 0; rect.bottom = h;
	hr = tex->LockRect( 0, &lockedrect, &rect, 0 );
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}


	BYTE*	bitshead;
	DWORD* dwbits;


	int height, width;

	int darkheight, brightheight;
	darkheight = (int)((float)h * darkh);
	brightheight = (int)((float)h * brighth);


	DWORD diffcol, ambcol, speccol;
	int colr, colg, colb;

	colr = (int)( diffuse.r * 255.0f );
	min( colr, 255 );
	max( colr, 0 );

	colg = (int)( diffuse.g * 255.0f );
	min( colg, 255 );
	max( colg, 0 );

	colb = (int)( diffuse.b * 255.0f );
	min( colb, 255 );
	max( colb, 0 );

	diffcol = D3DCOLOR_ARGB( 255, colr, colg, colb );

/////
	colr = (int)( ambient.r * 255.0f );
	min( colr, 255 );
	max( colr, 0 );

	colg = (int)( ambient.g * 255.0f );
	min( colg, 255 );
	max( colg, 0 );

	colb = (int)( ambient.b * 255.0f );
	min( colb, 255 );
	max( colb, 0 );

	ambcol = D3DCOLOR_ARGB( 255, colr, colg, colb );

///////
	colr = (int)( specular.r * 255.0f );
	min( colr, 255 );
	max( colr, 0 );

	colg = (int)( specular.g * 255.0f );
	min( colg, 255 );
	max( colg, 0 );

	colb = (int)( specular.b * 255.0f );
	min( colb, 255 );
	max( colb, 0 );

	speccol = D3DCOLOR_ARGB( 255, colr, colg, colb );

////////

	for( height = 0; height < h; height++ ){
				
		bitshead = (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * height;

		for( width = 0; width < w; width++ ){
					
			dwbits = (DWORD*)bitshead + width;



			if( height < darkheight ){
				*dwbits = ambcol;
			}else if( height < brightheight ){
				*dwbits = diffcol;
			}else{
				*dwbits = speccol;
			}	
		}	
	}

	hr = tex->UnlockRect( 0 );
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


