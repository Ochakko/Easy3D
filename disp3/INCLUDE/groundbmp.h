#ifndef GROUNDBMPH
#define GROUNDBMPH

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <basedat.h>


#define DBGH
#include <dbg.h>


class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CShdElem;


// パレットなしdib専用。
// 24bpp。

class CGroundBMP
{
public:
	CGroundBMP();
	~CGroundBMP();

	int LoadGroundBMP( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, 
		char* bmpname1,
		char* bmpname2, 
		char* bmpname3,
		char* texname, 
		float maxx, float maxz, 
		int divx, int divz, 
		float maxheight );

	int LoadGroundBMPTS( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, 
		char* bmpname1,
		float maxx, float maxz, 
		int divx, int divz, 
		float maxheight );

	int DbgOutBMPHeader( char* bmpname );


	int LoadMovableBMP( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, 
		char* bmpname, float maxx, float maxz, int divx, int divz, float wallheight );


private:
	void InitLoadParams();
	//RGBDAT* GetBmpPtr( char* fname, HANDLE* fileptr, BITMAPFILEHEADER* hdrptr, BITMAPINFOHEADER* biheaderptr );
	float* GetBmpY( char* fname, HANDLE* fileptr, BITMAPFILEHEADER* hdrptr, BITMAPINFOHEADER* biheaderptr, float* minntscy, float* maxntscy, int* notfound, int revflag = 0 );
	
	int AddShape2Tree( char* elemname, char* texname );
	int SetMeshInfo( CMeshInfo* dstmi, int mitype, int mim, int min, int skip = 0 );
	int Init3DObj();

public:
	//char filename[1024];
	BITMAPFILEHEADER m_hdr1;
	//BITMAPINFO bi;//<------ パレット有りの場合のみ、使用する。
	BITMAPINFOHEADER m_biheader1;
	RGBDAT* m_rgbdat1;
	float* m_ntsc_y1;
	float miny1;
	float maxy1;


	BITMAPFILEHEADER m_hdr2;
	BITMAPINFOHEADER m_biheader2;
	RGBDAT* m_rgbdat2;
	float* m_ntsc_y2;
	float miny2;
	float maxy2;

	BITMAPFILEHEADER m_hdr3;
	BITMAPINFOHEADER m_biheader3;
	RGBDAT* m_rgbdat3;
	float* m_ntsc_y3;
	float miny3;
	float maxy3;


	int m_bmpw;
	int m_bmph;

	CTreeHandler2* m_lpth;
	CShdHandler* m_lpsh;
	CMotHandler* m_lpmh;


	CMeshInfo tempinfo;
	int curseri;
	int befseri;
	int curdepth;
	int befdepth;
	int curshdtype;
	int befshdtype;

};

#endif