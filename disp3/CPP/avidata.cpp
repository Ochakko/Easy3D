#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <D3DX9.h>

#include <coef.h>
#define DBGH
#include "dbg.h"

#include <vfw.h>

#include <dibfile.h>

#include <avidata.h>

#include <crtdbg.h>
#include <tchar.h>

static int s_allocno = 0;

CAviData::CAviData()
{
	InitParams();
	s_allocno++;
}

CAviData::~CAviData()
{
	DestroyObjs();
}

int CAviData::InitParams()
{
	serialno = s_allocno;
	curframeno = 0;
	completeflag = 0;
		
	ZeroMemory( filename, 2048 );
	datatype = 0;
	compkind = 0;
	framerate = 30;
	frameleng = 0;

	ZeroMemory( &bmih, sizeof( BITMAPINFOHEADER ) );

	InitCompVars();

    pavi = 0;
    pstm = 0;
	ptmp = 0;

	next = 0;
	prev = 0;
	ishead = 0;


	return 0;
}

int CAviData::InitCompVars()
{
	cv_cinepak, cv_ms, cv_notcomp;

	cvstate_cinepak = 0x726c6f63;
	cvstate_ms = 0x4b;

	ZeroMemory( &cv_cinepak, sizeof( COMPVARS ) );
	ZeroMemory( &cv_ms, sizeof( COMPVARS ) );
	ZeroMemory( &cv_notcomp, sizeof( COMPVARS ) );

/// cinepak
	cv_cinepak.cbSize = sizeof( COMPVARS );
    cv_cinepak.dwFlags = ICMF_COMPVARS_VALID;//
    cv_cinepak.fccHandler= mmioFOURCC( 'c', 'v', 'i', 'd' );
    cv_cinepak.lQ = 10000;
	cv_cinepak.lKey = 0;
	cv_cinepak.lDataRate = 0;
	cv_cinepak.lpState = &cvstate_cinepak;
	cv_cinepak.cbState = 4;
/// microsoft
	cv_ms.cbSize = sizeof( COMPVARS );
    cv_ms.dwFlags = ICMF_COMPVARS_VALID;//
    cv_ms.fccHandler= mmioFOURCC( 'M', 'S', 'V', 'C' );
    cv_ms.lQ = 7500;
	cv_ms.lKey = 0;
	cv_ms.lDataRate = 0;
	cv_ms.lpState = &cvstate_ms;
	cv_ms.cbState = 4;


/// not compress
	cv_notcomp.cbSize = sizeof( COMPVARS );
    cv_notcomp.dwFlags = ICMF_COMPVARS_VALID;//
    cv_notcomp.fccHandler= mmioFOURCC( 'D', 'I', 'B', ' ' );
    cv_notcomp.lQ = 10000;
	cv_notcomp.lKey = 0;
	cv_notcomp.lDataRate = 0;
	cv_notcomp.lpState = 0;
	cv_notcomp.cbState = 0;


	return 0;
}

int CAviData::DestroyObjs()
{
	if( ptmp ){
		AVIStreamRelease(ptmp);
		ptmp = 0;
	}
	if( pstm ){   
		AVIStreamRelease(pstm);
		pstm = 0;
	}
	if( pavi ){
		AVIFileRelease(pavi);
	    AVIFileExit();
		pavi = 0;
	}

	DbgOut( "avidata : DestroyObjs : destroy avi object #####\n" );

	return 0;
}


int CAviData::CreateAVIFile( LPDIRECT3DSURFACE9 pBack, char* srcfilename, int srcdatatype, int srccompkind, int srcframerate, int srcframeleng )
{
	if( pavi || pstm || ptmp ){
		DbgOut( "avidata : CreateAVIFile : object already exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char aviname[2048];
	ZeroMemory( aviname, 2048 );
	int chkleng;
	chkleng = (int)strlen( srcfilename ) + 4;
	if( chkleng >= 2048 ){
		DbgOut( "avidata : CreateAVIFile : filename too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( aviname, srcfilename );
	strcat( aviname, ".avi" );

	datatype = srcdatatype;
	compkind = srccompkind;
	framerate = srcframerate;
	frameleng = srcframeleng;

/////////
	HRESULT hr;
	D3DSURFACE_DESC desc;
	hr = pBack->GetDesc( &desc );
	if( hr != D3D_OK ){
		DbgOut( "avidata : CreateAVIFile : back GetDesc error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();
		return 1;
	}
	int backformat;
	UINT backWidth, backHeight;
	backformat = desc.Format;
	backWidth = desc.Width;
	backHeight = desc.Height;

	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = backWidth;
	rect.bottom = backHeight;


//////////////////
	//avi fileの作成に必要なので、ここで、サーフェスの大きさを求める。
	//書き出し中は、サーフェスの大きさ変更禁止！！！！！！

	int bmpwidth, bmpheight;
	bmpheight = backHeight;
	int wcnt;
	wcnt = backWidth / 4;// !!!!!!! 4の倍数に切りつめちゃう。
	bmpwidth = wcnt * 4;


	CDibFile* firstdib;
	firstdib = new CDibFile( "temp", bmpwidth, bmpheight, 24 );
	if( !firstdib ){
		DbgOut( "avidata : CreateAVIFile : firstdib alloc error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();
		return 1;
	}
	
	BITMAPINFOHEADER* firstbih;
	firstbih = firstdib->GetBMPInfoHeader();
	if( !firstbih ){
		DbgOut( "avidata : CreateAVIFile : firstdib error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();
		delete firstdib;
		return 1;
	}
	bmih = *firstbih;//!!!!!!!!!

////////////

    AVICOMPRESSOPTIONS opt;
    AVISTREAMINFO si={streamtypeVIDEO,comptypeDIB,0,0,0,0,1,framerate,0,frameleng,0,0,(DWORD)-1,0,{0,0,bmpwidth,bmpheight},0,0,_T("Video by E3DHSP")};

    AVIFileInit();

	COMPVARS cv;
	switch( compkind ){
	case 0:
		cv = cv_notcomp;
		break;
	case 1:
		cv = cv_cinepak;
		break;
	case 2:
		cv = cv_ms;
		break;
	default:
		DbgOut( "avidata : CreateAVIFile : invalid compkind warning !!! set default value : cinepak\n" );
		_ASSERT( 0 );
		cv = cv_cinepak;
		compkind = 1;
		break;
	}

	/***
	{
		DbgOut( "cv.cbSize %d\n", cv.cbSize );
		DbgOut( "cv.dwFlags %d valid %d\n", cv.dwFlags, ICMF_COMPVARS_VALID );
		DbgOut( "cv.fccHandler %c %c %c %c\n",
			cv.fccHandler & 0x000000FF,
			(cv.fccHandler >> 8) & 0x000000FF,
			(cv.fccHandler >> 16) & 0x000000FF,
			(cv.fccHandler >> 24) & 0x000000FF
		);
		DbgOut( "cv.lQ %d\n", cv.lQ );
		DbgOut( "cv.lKey %d\n", cv.lKey );
		DbgOut( "cv.lDataRate %d\n", cv.lDataRate );
		DbgOut( "cv.lpState 0x%x\n", cv.lpState );
		DbgOut( "cv.cbState %d\n", cv.cbState );
	}
	***/


    si.fccHandler=cv.fccHandler;
    opt.fccType=streamtypeVIDEO;
    opt.fccHandler=cv.fccHandler;
    opt.dwKeyFrameEvery=cv.lKey;
    opt.dwQuality=cv.lQ;
    opt.dwBytesPerSecond=cv.lDataRate;
    opt.dwFlags=(cv.lDataRate>0?AVICOMPRESSF_DATARATE:0)|(cv.lKey>0?AVICOMPRESSF_KEYFRAMES:0);
    opt.lpFormat=NULL;
    opt.cbFormat=0;
    opt.lpParms=cv.lpState;
    opt.cbParms=cv.cbState;
    opt.dwInterleaveEvery=0;

	/***
	{
		DbgOut( "opt.fccType %d  video %d\n", opt.fccType, streamtypeVIDEO );
		DbgOut( "opt.fccHandler %c %c %c %c\n",
			opt.fccHandler & 0x000000FF,
			(opt.fccHandler >> 8) & 0x000000FF,
			(opt.fccHandler >> 16) & 0x000000FF,
			(opt.fccHandler >> 24) & 0x000000FF
			);
		DbgOut( "opt.dwKeyFrameEvery %d\n", opt.dwKeyFrameEvery );
		DbgOut( "opt.dwQuality %d\n", opt.dwQuality );
		DbgOut( "opt.dwBytesPerSecond %d\n", opt.dwBytesPerSecond );
		DbgOut( "opt.dwFlags %d\n", opt.dwFlags );
		DbgOut( "opt.lpFormat 0x%x\n", opt.lpFormat );
		DbgOut( "opt.cbFormat %d\n", opt.cbFormat );
		DbgOut( "opt.lpParms 0x%x\n", opt.lpParms );
		DbgOut( "opt.cbParms %d\n", opt.cbParms );
		DbgOut( "opt.dwInterleaveEvery %d\n", opt.dwInterleaveEvery );
	}
	***/

//////////
	if (AVIFileOpen(&pavi,aviname,OF_CREATE | OF_WRITE | OF_SHARE_DENY_NONE,NULL)!=0){
		DbgOut( "avidata : CreateAVIFile : AVIFileOpen error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();
		delete firstdib;

		DestroyObjs();

        return 1;
	}
	if (AVIFileCreateStream(pavi,&pstm,&si)!=0){
		DbgOut( "avidata : CreateAVIFile : AVIFileCreateStream error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();
		delete firstdib;

		DestroyObjs();

        return 1;
	}
	if (AVIMakeCompressedStream(&ptmp,pstm,&opt,NULL)!=AVIERR_OK){
		DbgOut( "avidata : CreateAVIFile : AVIMakeCompressedStream error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();
		delete firstdib;

		DestroyObjs();

		return 1;
	}
	if (AVIStreamSetFormat(ptmp,0,&bmih,sizeof(BITMAPINFOHEADER))!=0){
		DbgOut( "avidata : CreateAVIFile : AVIStreamSetFormat error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();
		delete firstdib;

		DestroyObjs();

		return 1;
	}

	//pBack->Release();
	delete firstdib;//!!!!!!!

	return 0;
}
int CAviData::Write2AVIFile( LPDIRECT3DSURFACE9 pBack, int srcdatatype )
{
	if( !pavi || !pstm || !ptmp ){
		DbgOut( "avidata : Write2AVIFile : object not ready error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( completeflag ){
		DbgOut( "avidata : Write2AVIFile : this file is already completed warning !!! skip \n" );
		_ASSERT( 0 );
		return 0;//
	}

	if( curframeno >= frameleng ){
		DbgOut( "avidata : Write2AVIFile : cannot add data more over : frameno overflow warning !!! skip \n" );
		_ASSERT( 0 );
		return 0;
	}

//////////// サーフェスの大きさのチェックをする（初期化時と同じかどうか調べる）

	HRESULT hr;
	D3DSURFACE_DESC desc;
	hr = pBack->GetDesc( &desc );
	if( hr != D3D_OK ){
		DbgOut( "avidata : Write2AVIFile : back GetDesc error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();

		DestroyObjs();

		return 1;
	}
	int backformat;
	UINT backWidth, backHeight;
	backformat = desc.Format;
	backWidth = desc.Width;
	backHeight = desc.Height;

	D3DLOCKED_RECT lockedrect;
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = backWidth;
	rect.bottom = backHeight;


	int bmpwidth, bmpheight;
	bmpheight = backHeight;
	int wcnt;
	wcnt = backWidth / 4;// !!!!!!! 4の倍数に切りつめちゃう。
	bmpwidth = wcnt * 4;

	CDibFile* tempdib;
	tempdib = new CDibFile( "temp_1", bmpwidth, bmpheight, 24 );
	if( !tempdib ){
		DbgOut( "avidata : Write2AVIFile : tempdib alloc error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();

		DestroyObjs();

		return 1;
	}
	
	BITMAPINFOHEADER* tempbih;
	tempbih = tempdib->GetBMPInfoHeader();
	if( !tempbih ){
		DbgOut( "avidata : Write2AVIFile : tempbih error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();
		delete tempdib;

		DestroyObjs();

		return 1;
	}

	int ret;
	ret = CmpBITMAPINFOHEADER( tempbih, &bmih );
	if( ret ){
		DbgOut( "avidata : Write2AVIFile : display size was chenged error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();
		delete tempdib;

		DestroyObjs();

		return 1;
	}
////////////
	hr = pBack->LockRect( &lockedrect, &rect, 0 );
	if( hr != D3D_OK ){
		DbgOut( "avidata : Write2AVIFile : back LockRect error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();
		delete tempdib;

		DestroyObjs();
	
		return 1;
	}


	BYTE* bitshead;
	int hno;
	for( hno = 0; hno < bmpheight; hno++ ){
		//bitshead = (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * hno;
		//上下反転する。
		bitshead = (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * hno;
		ret = tempdib->SetRGB( bmpheight - 1 - hno, bitshead, backformat );
		if( ret ){
			DbgOut( "avidata : Write2AVIFile : dibfile SetRGB error !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			//pBack->Release();

			delete tempdib;

			DestroyObjs();

			return 1;
		}
	}


	BYTE* pBit;
	pBit = tempdib->GetBMPData();
	_ASSERT( pBit );

	if (AVIStreamWrite(ptmp,curframeno,1,pBit,tempdib->sizeimage, AVIIF_KEYFRAME,NULL,NULL)!=0){
		DbgOut( "avidata : Write2AVIFile : AVIStreamWrite error !!!\n" );
		_ASSERT( 0 );
		pBack->UnlockRect();
		//pBack->Release();

		delete tempdib;

		DestroyObjs();
			
		return 1;
	}
		
	delete tempdib;

	hr = pBack->UnlockRect();
	if( hr != D3D_OK ){
		DbgOut( "avidata : Write2AVIFile : back UnlockRect error !!!\n" );
		_ASSERT( 0 );
		//pBack->Release();

		DestroyObjs();
			
		return 1;
	}

	curframeno++;

	return 0;
}


int CAviData::CompleteAVIFile( LPDIRECT3DSURFACE9 pBack, int srcdatatype )
{
	if( !pavi || !pstm || !ptmp ){
		DbgOut( "avidata : CompleteAVIFile : object not ready error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( completeflag ){
		DbgOut( "avidata : CompleteAVIFile : this file is already completed warning !!! skip \n" );
		_ASSERT( 0 );
		return 0;//
	}

	int ret;

	if( (curframeno < frameleng) && (pBack) ){
		int fno;
		for( fno = curframeno; fno < frameleng; fno++ ){
			ret = Write2AVIFile( pBack, srcdatatype );
			if( ret ){
				DbgOut( "avidata : CompleteAVIFile : Write2AVIFile error !!! %d %d\n", curframeno, frameleng );
				_ASSERT( 0 );

				DestroyObjs();

				return 1;
			}
		}
	}

	DestroyObjs();

	completeflag = 1;

	return 0;
}

int CAviData::CmpBITMAPINFOHEADER( BITMAPINFOHEADER* bmih1, BITMAPINFOHEADER* bmih2 )
{
	int ret = 0;

	if( bmih1->biSize != bmih2->biSize )
		ret++;

	if( bmih1->biWidth != bmih2->biWidth )
		ret++;

	if( bmih1->biHeight != bmih2->biHeight )
		ret++;

	if( bmih1->biPlanes != bmih2->biPlanes )
		ret++;

	if( bmih1->biBitCount != bmih2->biBitCount )
		ret++;

	if( bmih1->biCompression != bmih2->biCompression )
		ret++;

	if( bmih1->biSizeImage != bmih2->biSizeImage )
		ret++;

	if( bmih1->biXPelsPerMeter != bmih2->biXPelsPerMeter )
		ret++;

	if( bmih1->biYPelsPerMeter != bmih2->biYPelsPerMeter )
		ret++;

	if( bmih1->biClrUsed != bmih2->biClrUsed )
		ret++;

	if( bmih1->biClrImportant != bmih2->biClrImportant )
		ret++;

	return ret;
}

int CAviData::AddToPrev( CAviData* addavi )
{
	CAviData *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addavi->prev = prev;
	addavi->next = this;

	prev = addavi;

	if( saveprev ){
		saveprev->next = addavi;
	}else{
		_ASSERT( ishead );
		addavi->ishead = 1;
		ishead = 0;
	}

	return 0;
}

int CAviData::AddToNext( CAviData* addavi )
{
	CAviData *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addavi->prev = this;
	addavi->next = savenext;

	next = addavi;

	if( savenext ){
		savenext->prev = addavi;
	}

	addavi->ishead = 0;

	return 0;
}

int CAviData::LeaveFromChain()
{
	CAviData *saveprev, *savenext;
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


