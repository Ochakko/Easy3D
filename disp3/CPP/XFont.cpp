#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <D3DX9.h>

#include <coef.h>
#define DBGH
#include "dbg.h"

#include <vfw.h>

#include <dibfile.h>

#include <XFont.h>

#include <crtdbg.h>
#include <tchar.h>

static int s_allocno = 0;

CXFont::CXFont()
{
	InitParams();
	s_allocno++;
}

CXFont::~CXFont()
{
	DestroyObjs();
}


int CXFont::InitParams()
{
	m_serialno = s_allocno;

	m_height = 20;
	m_width = 0;
	m_weight = FW_NORMAL;
	m_Italic = 0;
	m_Underline = 0;
	m_Strikeout = 0;

	strcpy( m_fontname, "MSゴシック" );

	m_lpd3dxFont = 0;
	m_rect.left = 0; 
	m_rect.right = 400; 
	m_rect.top = 10; 
	m_rect.bottom = 0;


	prev = 0;
	next = 0;
	ishead = 0;

	m_pdev = 0;

	return 0;
}
int CXFont::DestroyObjs()
{
	if( m_psprite ){
		m_psprite->Release();
		m_psprite = 0;
	}


	if( m_lpd3dxFont ){
		m_lpd3dxFont->Release();
		m_lpd3dxFont = 0;
	}

	//SAFE_RELEASE (m_lpd3dxFont);

	return 0;
}


int CXFont::CreateFont( LPDIRECT3DDEVICE9 pdev, LPD3DXSPRITE psprite, int height, int width, int weight, 
	int bItalic, int bUnderline, int bStrikeout,
	char* fontname )
{
	m_height = height;
	m_width = width;
	m_weight = weight;
	m_Italic = bItalic;
	m_Underline = bUnderline;
	m_Strikeout = bStrikeout;

	m_psprite = psprite;

	int namelen;
	namelen = (int)strlen( fontname );
	if( (namelen <= 0) || (namelen >= 1024) ){
		DbgOut( "XFont : fontname error %s !!!\n", fontname );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( m_fontname, fontname );

	int ret;
	ret = Restore( pdev );
	if( ret ){
		DbgOut( "xfont : CreateFont : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
    

	/***
	// hFont の作成
    HDC					  hFontDC;
	HFONT				  hFont;
    hFontDC  = CreateCompatibleDC( NULL );
	if( hFontDC == NULL ){
		DbgOut( "XFont : CreateFont : CreateCompatibleDC error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
    ret = SetMapMode( hFontDC, MM_TEXT );
	if( ret == 0 ){
		DbgOut( "XFont : CreateFont : SetMapMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hFont = ::CreateFont( m_height, m_width, 0, 0, m_weight, m_Italic,
                          m_Underline, m_Strikeout, DEFAULT_CHARSET, 
						  OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          VARIABLE_PITCH,  m_fontname);
	if( hFont == NULL ){
		DbgOut( "XFont : CraeteFont : CreateFont error !!!\n" );
		_ASSERT( 0 );
		ReleaseDC (NULL, hFontDC);	
		return 1;
	}

	HGDIOBJ retobj;
    retobj = SelectObject( hFontDC, hFont );
	if( retobj == NULL ){
		DbgOut( "XFont : CreateFont : SelectObject error !!!\n" );
		_ASSERT( 0 );
		ReleaseDC( NULL, hFontDC );
		return 1;
	}

	ReleaseDC (NULL, hFontDC);	
	
	// フォントの設定
	HRESULT hr;
	hr = D3DXCreateFont( pdev, hFont, &m_lpd3dxFont );
	if( hr != D3D_OK ){
		DbgOut( "XFont : CreateFont : D3DXCreateFont error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/

	return 0;
}

int CXFont::DrawText( char* textstr, int posx, int posy, D3DCOLOR col, int bmode )
{
	if( !textstr )
		return 0;


	HRESULT hr;

	//m_clientWidth = clirect.right - clirect.left;
	//m_clientHeight = clirect.bottom - clirect.top;
	
	m_rect.left = posx;
//	m_rect.right = clirect.right - clirect.left;
	m_rect.top = posy;
	m_rect.right = 0;
	m_rect.bottom = 0;// <--- bottomをゼロにしておけば、自動的に調整されるみたい。

	//hr = m_psprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT );
	hr = m_psprite->Begin( D3DXSPRITE_ALPHABLEND );
	if( hr != D3D_OK ){
		DbgOut( "XFont : DrawText : xfont Begin error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( m_pdev && (bmode >= 0) ){
		if( bmode == 0 ){
			m_pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			m_pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}else if( bmode == 1 ){
			m_pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			m_pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		}else if( bmode == 2 ){
			m_pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			m_pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		}else if( bmode == 103 ){
			m_pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
			m_pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
		}else if( bmode == 104 ){
			m_pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
			m_pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			m_pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		}else if( bmode == 105 ){
			m_pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
			m_pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		}else if( bmode == 106 ){
			m_pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
			m_pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		}else if( bmode == 107 ){
			m_pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
			m_pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
		}else if( bmode == 108 ){
			m_pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			m_pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
		}
	}



	hr = m_lpd3dxFont->DrawText( m_psprite, textstr, -1, &m_rect, DT_LEFT | DT_NOCLIP, col );
	//if( hr != D3D_OK ){
	//	DbgOut( "XFont : DrawText : xfont DrawText error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}


	//DbgOut( "XFont : DrawText %s, hr %x\n", textstr, hr );
	//DbgOut( "XFont error %x %x %x %x\n", 
	//	D3DERR_INVALIDCALL, 
	//	D3DERR_NOTAVAILABLE, 
	//	D3DERR_OUTOFVIDEOMEMORY, 
	//	D3DXERR_INVALIDDATA
	//	);


	m_psprite->End();

	return 0;
}

int CXFont::CalcStrSize( char* textstr, SIZE* sizeptr )
{
	HRESULT hr;
	
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = 0;
	rect.bottom = 0;// <--- bottomをゼロにしておけば、自動的に調整されるみたい。

	D3DCOLOR col;
	col = D3DCOLOR_ARGB( 255, 255, 255, 255 );

	//hr = m_psprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT );
	hr = m_psprite->Begin( D3DXSPRITE_ALPHABLEND );
	if( hr != D3D_OK ){
		DbgOut( "XFont : DrawText : xfont Begin error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	hr = m_lpd3dxFont->DrawText( m_psprite, textstr, -1, &rect, DT_LEFT | DT_NOCLIP | DT_CALCRECT, col );

	sizeptr->cx = rect.right - rect.left;
	sizeptr->cy = rect.bottom - rect.top;

	m_psprite->End();

	return 0;


}


int CXFont::DrawTextWithCnt( char* textstr, int posx, int posy, D3DCOLOR col, int eachcnt, int curcnt )
{
	if( !textstr )
		return 0;

	int ret = 0;
	char* disptext = 0;
	int* stridearray = 0;

	int mcnum = 0;
	int dispmcnum = 0;
	int cpleng = 0;

	int orgcleng;
	orgcleng = (int)strlen( textstr );
	if( orgcleng <= 0 ){
		ret = 0;
		goto DrawTextWithCntExit;
	}

	stridearray = (int*)malloc( sizeof( int ) * orgcleng );
	if( !stridearray ){
		DbgOut( "xfont : DrawTextWithCnt : stridearray alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto DrawTextWithCntExit;
	}
	ZeroMemory( stridearray, sizeof( int ) * orgcleng );

	ret = GetStrideArray( textstr, orgcleng, stridearray, &mcnum );
	if( ret ){
		DbgOut( "xfont : DrawTextWithCnt : GetStrideArray error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto DrawTextWithCntExit;
	}
	
	if( mcnum <= 0 ){
		ret = 0;
		goto DrawTextWithCntExit;
	}

	if( eachcnt > 0 ){
		dispmcnum = curcnt / eachcnt;
		if( dispmcnum > mcnum )
			dispmcnum = mcnum;
	}else{
		dispmcnum = mcnum;
	}

	if( dispmcnum <= 0 ){
		ret = 0;
		goto DrawTextWithCntExit;
	}

	int mcno;
	for( mcno = 0; mcno < dispmcnum; mcno++ ){
		cpleng += *(stridearray + mcno);
	}

	if( cpleng <= 0 ){
		ret = 0;
		goto DrawTextWithCntExit;
	}

	disptext = (char*)malloc( sizeof( char ) * cpleng + 1 );
	if( !disptext ){
		DbgOut( "xfont : DrawTextWithCnt : disptext alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto DrawTextWithCntExit;
	}

	strncpy( disptext, textstr, cpleng );
	*( disptext + cpleng ) = 0;


	ret = DrawText( disptext, posx, posy, col );
	if( ret ){
		DbgOut( "xfont : DrawTextWithCnt : DrawText error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto DrawTextWithCntExit;
	}


	goto DrawTextWithCntExit;


DrawTextWithCntExit:
	if( stridearray )
		free( stridearray );
	if( disptext )
		free( disptext );

	return ret;
}

int CXFont::DrawTextWithCnt2( char* textstr, int posx, int posy, D3DCOLOR col, int* cntarray, int arrayleng, int curcnt )
{
	if( !textstr )
		return 0;

	int ret = 0;
	char* disptext = 0;


	int orgcleng;
	orgcleng = (int)strlen( textstr );
	if( orgcleng <= 0 ){
		ret = 0;
		goto DrawTextWithCntExit2;
	}


	disptext = (char*)malloc( sizeof( char ) * orgcleng + 1 );
	if( !disptext ){
		DbgOut( "xfont : DrawTextWithCnt : disptext alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto DrawTextWithCntExit2;
	}
	*( disptext + orgcleng ) = 0;
	

	ret = CopyTextWithCnt( disptext, orgcleng, textstr, cntarray, arrayleng, curcnt );
	if( ret ){
		DbgOut( "xfont : DrawTextWithCnt2 : CopyTextWithCnt error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto DrawTextWithCntExit2;		
	}


	ret = DrawText( disptext, posx, posy, col );
	if( ret ){
		DbgOut( "xfont : DrawTextWithCnt : DrawText error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto DrawTextWithCntExit2;
	}


	goto DrawTextWithCntExit2;


DrawTextWithCntExit2:
	if( disptext )
		free( disptext );

	return ret;

}


int CXFont::GetStrideArray( char* textstr, int maxlen, int* stridearray, int* mcnum )
{
	int curpos = 0;
	int totalmcnum = 0;
	unsigned char curc;
	while( curpos < maxlen ){
		curc = (unsigned char)(*( textstr + curpos ));

		if( curc == 0 )
			break;

		if( curc < 0x80 ){
			curpos++;
			*( stridearray + totalmcnum ) = 1;
		}else{
			curpos += 2;
			*( stridearray + totalmcnum ) = 2;
		}

		totalmcnum++;
	}

	*mcnum = totalmcnum;

	return 0;
}

int CXFont::CopyTextWithCnt( char* textptr, int textleng, char* orgptr, int* cntarray, int arrayleng, int curcnt )
{
		
	int curpos = 0;
	int mcnum = 0;
	unsigned char curc;
	int cmpcnt;

	while( (curpos < textleng) && (mcnum < arrayleng) ){
		curc = (unsigned char)(*(orgptr + curpos));

		cmpcnt = *( cntarray + mcnum );
		if( cmpcnt <= curcnt ){
			if( curc < 0x80 ){
				*( textptr + curpos ) = *( orgptr + curpos );
				curpos++;
			}else{
				*( textptr + curpos ) = *( orgptr + curpos );
				*( textptr + curpos + 1 ) = *( orgptr + curpos + 1 );
				curpos += 2;
			}
		}else{
			// スペースを入れる	
			if( curc < 0x80 ){
				*( textptr + curpos ) = ' ';
				curpos++;
			}else{
				strcpy( textptr + curpos, "　" );//全角スペース
				curpos += 2;
			}
		}

		mcnum++;
	}

	*( textptr + curpos ) = 0;

	return 0;
}



int CXFont::DestroyFont()
{

	DestroyObjs();

	return 0;
}
int CXFont::AddToPrev( CXFont* addfont )
{
	CXFont *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addfont->prev = prev;
	addfont->next = this;

	prev = addfont;

	if( saveprev ){
		saveprev->next = addfont;
	}else{
		_ASSERT( ishead );
		addfont->ishead = 1;
		ishead = 0;
	}

	return 0;
}

int CXFont::AddToNext( CXFont* addfont )
{
	CXFont *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addfont->prev = this;
	addfont->next = savenext;

	next = addfont;

	if( savenext ){
		savenext->prev = addfont;
	}

	addfont->ishead = 0;

	return 0;
}

int CXFont::LeaveFromChain()
{
	CXFont *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	prev = 0;
	next = 0;

	if( saveprev ){
		saveprev->next = savenext;
	}else{
		_ASSERT( ishead );
		if( savenext )
			savenext->ishead = 1;
		ishead = 0;
	}

	if( savenext ){
		savenext->prev = saveprev;
	}

	return 0;
}

int CXFont::Invalidate()
{
	DestroyFont();

	return 0;
}

int CXFont::Restore( LPDIRECT3DDEVICE9 pdev )
{

	m_pdev = pdev;

	HRESULT hr;

	if( m_psprite == 0 ){
		hr = D3DXCreateSprite( pdev, &m_psprite );
		if( hr != D3D_OK ){
			DbgOut( "xfont : Restore : D3DXCreateSprite error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	hr = D3DXCreateFont( pdev, m_height, m_width, m_weight, 1, m_Italic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, m_fontname, &m_lpd3dxFont );
	if( hr != D3D_OK ){
		DbgOut( "XFont : Restore : D3DXCreateFont error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//    D3DXCreateFont( pdev, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
//                              OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
//                              "Arial", &m_lpd3dxFont );


//	hr = m_lpd3dxFont->OnResetDevice();
//	if( hr != D3D_OK ){
//		DbgOut( "XFont : Restore : OnResetDevice error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

/***	
	// hFont の作成
    HDC					  hFontDC;
	HFONT				  hFont;
    hFontDC  = CreateCompatibleDC( NULL );
	if( hFontDC == NULL ){
		DbgOut( "XFont : Restore : CreateCompatibleDC error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
    ret = SetMapMode( hFontDC, MM_TEXT );
	if( ret == 0 ){
		DbgOut( "XFont : Restore : SetMapMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hFont = ::CreateFont( m_height, m_width, 0, 0, m_weight, m_Italic,
                          m_Underline, m_Strikeout, DEFAULT_CHARSET, 
						  OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          VARIABLE_PITCH,  m_fontname);


	if( hFont == NULL ){
		DbgOut( "XFont : Restore : CreateFont error !!!\n" );
		_ASSERT( 0 );
		ReleaseDC (NULL, hFontDC);	
		return 1;
	}

	HGDIOBJ retobj;
    retobj = SelectObject( hFontDC, hFont );
	if( retobj == NULL ){
		DbgOut( "XFont : Restore : SelectObject error !!!\n" );
		_ASSERT( 0 );
		ReleaseDC( NULL, hFontDC );
		return 1;
	}

	ReleaseDC (NULL, hFontDC);	
	
	// フォントの設定
	HRESULT hr;
	hr = D3DXCreateFont( pdev, hFont, &m_lpd3dxFont );
	if( hr != D3D_OK ){
		DbgOut( "XFont : Restore : D3DXCreateFont error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/
	return 0;
}

