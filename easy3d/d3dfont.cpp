//-----------------------------------------------------------------------------
// File: D3DFont.cpp
//
// Desc: Texture-based font class
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#define STRICT
#include <stdio.h>
#include <tchar.h>
#include <D3DX9.h>
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"

extern DWORD g_cop0;
extern DWORD g_cop1;
extern DWORD g_aop0;
extern DWORD g_aop1;



//-----------------------------------------------------------------------------
// Custom vertex types for rendering text
//-----------------------------------------------------------------------------
#define MAX_NUM_VERTICES 50*6

struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     float tu, tv; };
struct FONT3DVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   float tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

inline FONT2DVERTEX InitFont2DVertex( const D3DXVECTOR4& p, D3DCOLOR color,
                                      float tu, float tv )
{
    FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
    return v;
}

inline FONT3DVERTEX InitFont3DVertex( const D3DXVECTOR3& p, const D3DXVECTOR3& n,
                                      float tu, float tv )
{
    FONT3DVERTEX v;   v.p = p;   v.n = n;   v.tu = tu;   v.tv = tv;
    return v;
}




//-----------------------------------------------------------------------------
// Name: CD3DFont()
// Desc: Font class constructor
//-----------------------------------------------------------------------------
CD3DFont::CD3DFont( TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags )
{
    _tcscpy_s( m_strFontName, 80, strFontName );
    m_dwFontHeight         = dwHeight;
    m_dwFontFlags          = dwFlags;

    m_pd3dDevice           = NULL;
    m_pTexture             = NULL;
    m_pVB                  = NULL;

    m_SavedStateBlock    = 0;
    m_DrawTextStateBlock = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CD3DFont()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CD3DFont::~CD3DFont()
{
    InvalidateDeviceObjects();
    DeleteDeviceObjects();
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
    HRESULT hr;

    // Keep a local copy of the device
    m_pd3dDevice = pd3dDevice;

    // Establish the font and texture size
    m_fTextScale  = 1.0f; // Draw fonts into texture without scaling

    // Large fonts need larger textures
    if( m_dwFontHeight > 40 )
        m_dwTexWidth = m_dwTexHeight = 1024;
    else if( m_dwFontHeight > 20 )
        m_dwTexWidth = m_dwTexHeight = 512;
    else
        m_dwTexWidth  = m_dwTexHeight = 256;

    // If requested texture is too big, use a smaller texture and smaller font,
    // and scale up when rendering.
    D3DCAPS9 d3dCaps;
    m_pd3dDevice->GetDeviceCaps( &d3dCaps );

    if( m_dwTexWidth > d3dCaps.MaxTextureWidth )
    {
        m_fTextScale = (float)d3dCaps.MaxTextureWidth / (float)m_dwTexWidth;
        m_dwTexWidth = m_dwTexHeight = d3dCaps.MaxTextureWidth;
    }

    // Create a new texture for the font
    hr = m_pd3dDevice->CreateTexture( m_dwTexWidth, m_dwTexHeight, 1,
                                      0, D3DFMT_A4R4G4B4,
                                      D3DPOOL_MANAGED, &m_pTexture, NULL );
    if( FAILED(hr) )
        return hr;

    // Prepare to create a bitmap
    DWORD*      pBitmapBits;
    BITMAPINFO bmi;
    ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  (int)m_dwTexWidth;
    bmi.bmiHeader.biHeight      = -(int)m_dwTexHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount    = 32;

    // Create a DC and a bitmap for the font
    HDC     hDC       = CreateCompatibleDC( NULL );
    HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
                                          (VOID**)&pBitmapBits, NULL, 0 );
    SetMapMode( hDC, MM_TEXT );

    // Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
    // antialiased font, but this is not guaranteed.
    INT nHeight    = -MulDiv( m_dwFontHeight, 
        (INT)(GetDeviceCaps(hDC, LOGPIXELSY) * m_fTextScale), 72 );
    DWORD dwBold   = (m_dwFontFlags&D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL;
    DWORD dwItalic = (m_dwFontFlags&D3DFONT_ITALIC) ? TRUE    : FALSE;
    HFONT hFont    = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
                          FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          VARIABLE_PITCH, m_strFontName );
    if( NULL==hFont )
        return E_FAIL;

    SelectObject( hDC, hbmBitmap );
    SelectObject( hDC, hFont );

    // Set text properties
    SetTextColor( hDC, RGB(255,255,255) );
    SetBkColor(   hDC, 0x00000000 );
    SetTextAlign( hDC, TA_TOP );

    // Loop through all printable character and output them to the bitmap..
    // Meanwhile, keep track of the corresponding tex coords for each character.
    DWORD x = 0;
    DWORD y = 0;
    TCHAR str[2] = _T("x");
    SIZE size;

    for( TCHAR c=32; c<127; c++ )
    {
        str[0] = c;
        GetTextExtentPoint32( hDC, str, 1, &size );

        if( (DWORD)(x+size.cx+1) > m_dwTexWidth )
        {
            x  = 0;
            y += size.cy+1;
        }

        ExtTextOut( hDC, x+0, y+0, ETO_OPAQUE, NULL, str, 1, NULL );

        m_fTexCoords[c-32][0] = ((float)(x+0))/m_dwTexWidth;
        m_fTexCoords[c-32][1] = ((float)(y+0))/m_dwTexHeight;
        m_fTexCoords[c-32][2] = ((float)(x+0+size.cx))/m_dwTexWidth;
        m_fTexCoords[c-32][3] = ((float)(y+0+size.cy))/m_dwTexHeight;

        x += size.cx+1;
    }

    // Lock the surface and write the alpha values for the set pixels
    D3DLOCKED_RECT d3dlr;
    m_pTexture->LockRect( 0, &d3dlr, 0, 0 );
    WORD* pDst16 = (WORD*)d3dlr.pBits;
    BYTE bAlpha; // 4-bit measure of pixel intensity

    for( y=0; y < m_dwTexHeight; y++ )
    {
        for( x=0; x < m_dwTexWidth; x++ )
        {
            bAlpha = (BYTE)((pBitmapBits[m_dwTexWidth*y + x] & 0xff) >> 4);
            if (bAlpha > 0)
            {
                *pDst16++ = (bAlpha << 12) | 0x0fff;
            }
            else
            {
                *pDst16++ = 0x0000;
            }
        }
    }

    // Done updating texture, so clean up used objects
    m_pTexture->UnlockRect(0);
    DeleteObject( hbmBitmap );
    DeleteDC( hDC );
    DeleteObject( hFont );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DFont::RestoreDeviceObjects()
{
    HRESULT hr;

    // Create vertex buffer for the letters
    if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( MAX_NUM_VERTICES*sizeof(FONT2DVERTEX),
                                                       D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
                                                       D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return hr;
    }

    // Create the state blocks for rendering text
    for( UINT which=0; which<2; which++ )
    {
        m_pd3dDevice->BeginStateBlock();
        m_pd3dDevice->SetTexture( 0, m_pTexture );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
        //m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );

		if( g_cop0 != D3DTOP_MODULATE ){
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			g_cop0 = D3DTOP_MODULATE;
		}
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		if( g_aop0 != D3DTOP_MODULATE ){
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			g_aop0 = D3DTOP_MODULATE;
		}
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		if( g_cop1 != D3DTOP_DISABLE ){
			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			g_cop1 = D3DTOP_DISABLE;
		}
		if( g_aop1 != D3DTOP_DISABLE ){
			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
			g_aop1 = D3DTOP_DISABLE;
		}

        if( which==0 )
            m_pd3dDevice->EndStateBlock( &m_SavedStateBlock );
        else
            m_pd3dDevice->EndStateBlock( &m_DrawTextStateBlock );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );

    // Delete the state blocks
    if( m_pd3dDevice )
    {
		if( m_SavedStateBlock )
			SAFE_RELEASE( m_SavedStateBlock );
            //m_pd3dDevice->DeleteStateBlock( m_SavedStateBlock );
        if( m_DrawTextStateBlock )
			SAFE_RELEASE( m_DrawTextStateBlock );
            //m_pd3dDevice->DeleteStateBlock( m_DrawTextStateBlock );
    }

    m_SavedStateBlock    = 0;
    m_DrawTextStateBlock = 0;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DeleteDeviceObjects()
{
    SAFE_RELEASE( m_pTexture );
    m_pd3dDevice = NULL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//-----------------------------------------------------------------------------
HRESULT CD3DFont::GetTextExtent( TCHAR* strText, SIZE* pSize )
{
    if( NULL==strText || NULL==pSize )
        return E_FAIL;

    float fRowWidth  = 0.0f;
    float fRowHeight = (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight;
    float fWidth     = 0.0f;
    float fHeight    = fRowHeight;

    while( *strText )
    {
        TCHAR c = *strText++;

        if( c == _T('\n') )
        {
            fRowWidth = 0.0f;
            fHeight  += fRowHeight;
        }
        if( c < _T(' ') )
            continue;

        float tx1 = m_fTexCoords[c-32][0];
        float tx2 = m_fTexCoords[c-32][2];

        fRowWidth += (tx2-tx1)*m_dwTexWidth;

        if( fRowWidth > fWidth )
            fWidth = fRowWidth;
    }

    pSize->cx = (int)fWidth;
    pSize->cy = (int)fHeight;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawTextScaled()
// Desc: Draws scaled 2D text.  Note that x and y are in viewport coordinates
//       (ranging from -1 to +1).  fXScale and fYScale are the size fraction 
//       relative to the entire viewport.  For example, a fXScale of 0.25 is
//       1/8th of the screen width.  This allows you to output text at a fixed
//       fraction of the viewport, even if the screen or window size changes.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DrawTextScaled( float x, float y, float z,
                                  float fXScale, float fYScale, DWORD dwColor,
                                  TCHAR* strText, DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;

    // Set up renderstate
    //m_pd3dDevice->CaptureStateBlock( m_SavedStateBlock );
    //m_pd3dDevice->ApplyStateBlock( m_DrawTextStateBlock );
	m_SavedStateBlock->Capture();
	m_DrawTextStateBlock->Apply();
    m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_FONT2DVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(FONT2DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }

    D3DVIEWPORT9 vp;
    m_pd3dDevice->GetViewport( &vp );
    float sx  = (x+1.0f)*vp.Width/2;
    float sy  = (y-1.0f)*vp.Height/2;
    float sz  = z;
    float rhw = 1.0f;
    float fStartX = sx;

    float fLineHeight = ( m_fTexCoords[0][3] - m_fTexCoords[0][1] ) * m_dwTexHeight;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices;
    DWORD         dwNumTriangles = 0L;
    m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );

    while( *strText )
    {
        TCHAR c = *strText++;

        if( c == _T('\n') )
        {
            sx  = fStartX;
            sy += fYScale*vp.Height;
        }
        if( c < _T(' ') )
            continue;

        float tx1 = m_fTexCoords[c-32][0];
        float ty1 = m_fTexCoords[c-32][1];
        float tx2 = m_fTexCoords[c-32][2];
        float ty2 = m_fTexCoords[c-32][3];

        float w = (tx2-tx1)*m_dwTexWidth;
        float h = (ty2-ty1)*m_dwTexHeight;

        w *= (fXScale*vp.Height)/fLineHeight;
        h *= (fYScale*vp.Height)/fLineHeight;

        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,sz,rhw), dwColor, tx1, ty2 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,sz,rhw), dwColor, tx1, ty1 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,sz,rhw), dwColor, tx2, ty2 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,sz,rhw), dwColor, tx2, ty1 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,sz,rhw), dwColor, tx2, ty2 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,sz,rhw), dwColor, tx1, ty1 );
        dwNumTriangles += 2;

        if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
        {
            // Unlock, render, and relock the vertex buffer
            m_pVB->Unlock();
            m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
            m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
            dwNumTriangles = 0L;
        }

        sx += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

    // Restore the modified renderstates
    //m_pd3dDevice->ApplyStateBlock( m_SavedStateBlock );
	m_SavedStateBlock->Apply();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws 2D text
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DrawText( float sx, float sy, DWORD dwColor,
                            TCHAR* strText, DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;

	if( g_cop0 != D3DTOP_MODULATE ){
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_cop0 = D3DTOP_MODULATE;
	}
	if( g_aop0 != D3DTOP_MODULATE ){
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		g_aop0 = D3DTOP_MODULATE;
	}
	if( g_cop1 != D3DTOP_DISABLE ){
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_cop1 = D3DTOP_DISABLE;
	}
	if( g_aop1 != D3DTOP_DISABLE ){
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		g_aop1 = D3DTOP_DISABLE;
	}

    // Setup renderstate
    //m_pd3dDevice->CaptureStateBlock( m_SavedStateBlock );
    //m_pd3dDevice->ApplyStateBlock( m_DrawTextStateBlock );
	m_SavedStateBlock->Capture();
	m_DrawTextStateBlock->Apply();
    m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_FONT2DVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(FONT2DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }

    float fStartX = sx;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices = NULL;
    DWORD         dwNumTriangles = 0;
    m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );

    while( *strText )
    {
        TCHAR c = *strText++;

        if( c == _T('\n') )
        {
            sx = fStartX;
            sy += (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight;
        }
        if( c < _T(' ') )
            continue;

        float tx1 = m_fTexCoords[c-32][0];
        float ty1 = m_fTexCoords[c-32][1];
        float tx2 = m_fTexCoords[c-32][2];
        float ty2 = m_fTexCoords[c-32][3];

        float w = (tx2-tx1) *  m_dwTexWidth / m_fTextScale;
        float h = (ty2-ty1) * m_dwTexHeight / m_fTextScale;

        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx1, ty2 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx2, ty1 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
        dwNumTriangles += 2;

        if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
        {
            // Unlock, render, and relock the vertex buffer
            m_pVB->Unlock();
            m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
            pVertices = NULL;
            m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
            dwNumTriangles = 0L;
        }

        sx += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

    // Restore the modified renderstates
    //m_pd3dDevice->ApplyStateBlock( m_SavedStateBlock );
	m_SavedStateBlock->Apply();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render3DText()
// Desc: Renders 3D text
//-----------------------------------------------------------------------------
HRESULT CD3DFont::Render3DText( TCHAR* strText, DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;

    // Setup renderstate
    //m_pd3dDevice->CaptureStateBlock( m_SavedStateBlock );
    //m_pd3dDevice->ApplyStateBlock( m_DrawTextStateBlock );
	m_SavedStateBlock->Capture();
	m_DrawTextStateBlock->Apply();
    m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_FONT3DVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(FONT3DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }

    // Position for each text element
    float x = 0.0f;
    float y = 0.0f;

    // Center the text block at the origin
    if( dwFlags & D3DFONT_CENTERED )
    {
        SIZE sz;
        GetTextExtent( strText, &sz );
        x = -(((float)sz.cx)/10.0f)/2.0f;
        y = -(((float)sz.cy)/10.0f)/2.0f;
    }

    // Turn off culling for two-sided text
    if( dwFlags & D3DFONT_TWOSIDED )
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    float fStartX = x;
    TCHAR c;

    // Fill vertex buffer
    FONT3DVERTEX* pVertices;
    DWORD         dwVertex       = 0L;
    DWORD         dwNumTriangles = 0L;
    m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );

    while( c = *strText++ )
    {
        if( c == '\n' )
        {
            x = fStartX;
            y -= (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight/10.0f;
        }
        if( c < 32 )
            continue;

        float tx1 = m_fTexCoords[c-32][0];
        float ty1 = m_fTexCoords[c-32][1];
        float tx2 = m_fTexCoords[c-32][2];
        float ty2 = m_fTexCoords[c-32][3];

        float w = (tx2-tx1) * m_dwTexWidth  / ( 10.0f * m_fTextScale );
        float h = (ty2-ty1) * m_dwTexHeight / ( 10.0f * m_fTextScale );

        *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+0,0), D3DXVECTOR3(0,0,-1), tx1, ty2 );
        *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), tx1, ty1 );
        *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), tx2, ty2 );
        *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+h,0), D3DXVECTOR3(0,0,-1), tx2, ty1 );
        *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), tx2, ty2 );
        *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), tx1, ty1 );
        dwNumTriangles += 2;

        if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
        {
            // Unlock, render, and relock the vertex buffer
            m_pVB->Unlock();
            m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
            m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
            dwNumTriangles = 0L;
        }

        x += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

    // Restore the modified renderstates
    //m_pd3dDevice->ApplyStateBlock( m_SavedStateBlock );
	m_SavedStateBlock->Apply();

    return S_OK;
}




