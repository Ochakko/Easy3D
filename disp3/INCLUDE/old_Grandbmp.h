#define	GRANDBMPH	1


#include <wingdi.h>
#include <d3drmwin.h>

#ifndef	COEFH
	#include	"coef.h"
#endif
#ifndef		CHARPOSH
	#include	"charpos.h"
#endif

class	CGrandBMP
{
public:
	CGrandBMP( int bmpw, int bmph, int bmpno, char* bmpname );
	~CGrandBMP();

	BITMAPINFO*	GetBitmapInfoAddress()
	{return m_pBMI;}

	BYTE*	GetBitsAddress()
	{return m_pBits;}

	RGBQUAD*	GetClrTabAddress()
	{return (LPRGBQUAD)(((BYTE*)(m_pBMI)) + sizeof(BITMAPINFOHEADER));}

	BOOL	IsCreated()
	{return m_bmpcreated;}


	BOOL	Load( CFile* fp );
	BOOL	SetGrandVertex( D3DTLVERTEX* lpvert, int* lph, int* lptexkind );// sx, sy, sz
	//BOOL	SetWalkFlag();

		// object Ž¯•Ê—p
	int		m_bmpno, m_bmpnow, m_bmpnoh;
	char	m_filename[500];


protected:
	BITMAPINFO*	m_pBMI;
	BYTE*	m_pBits;
	int		m_bitdepth;
	int		m_palnum;
	BOOL	m_bmpcreated;

	//int		m_sizew;
	//int		m_sizeh;

	//RGBDATA	m_diffcol, m_ambcol;
private:
	int	NumDIBColorEntries( LPBITMAPINFO pBmpInfo );
	BOOL	IsWinDIB( BITMAPINFOHEADER* pBIH);

};

