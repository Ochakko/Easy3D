#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <windows.h>

#include "quafile.h"
#include <crtdbg.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <shdelem.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>

#include <TexKey.h>
#include <TexChange.h>
#include <AlpKey.h>
#include <AlpChange.h>
#include <mqomaterial.h>

#include <DSKey.h>
#include <DSChange.h>
#include <DispSwitch.h>

#include <SymmOpe.h>
#include <BoneProp.h>

#include <morph.h>
#include <MMotKey.h>
#include <MMotElem.h>

#include <Panda.h>

//#include "e:\pgfile9\RokDeBone2\inidata.h"


#define FLAGS_HASSCALE	0x01
#define FLAGS_NAMEFILE	0x02


//extern CIniData* g_inidata;


CQuaFile::CQuaFile()
{
	hfile = INVALID_HANDLE_VALUE;
	lpsh = 0;
	lpmh = 0;
	m_motcookie = -1;
	m_writemode = 0;

	m_frombuf = 0;
	m_buf.buf = 0;
	m_buf.bufsize = 0;
	m_buf.pos = 0;

	m_hasscale = 1;

	m_savestart = 0;
	m_saveend = 0;

	m_quamult = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );

	m_morphindex = 0;
	m_morphindexnum = 0;
}

CQuaFile::~CQuaFile()
{
	DestroyObjs();
}

void CQuaFile::DestroyObjs()
{
	if( hfile != INVALID_HANDLE_VALUE ){
		if( m_writemode ){
			FlushFileBuffers( hfile );
			SetEndOfFile( hfile );
		}
		CloseHandle( hfile );
		hfile = INVALID_HANDLE_VALUE;
	}

	if( m_buf.buf ){
		free( m_buf.buf );
		m_buf.buf = 0;
	}
	m_buf.bufsize = 0;
	m_buf.pos = 0;

	if( m_morphindex ){
		free( m_morphindex );
		m_morphindex = 0;
	}
	m_morphindexnum = 0;

}

int CQuaFile::SaveQuaFile2Buf( int namequaflag, CTreeHandler2* srclpth, CShdHandler* srclpsh, char* buf, int bufsize, int* writesize, CMotHandler* srclpmh, int mcookie )
{
	int ret = 0;

	*writesize = 0;
	m_buf.buf = buf;
	m_buf.bufsize = bufsize;
	m_buf.pos = 0;

	if( !srclpmh )
		return 1;

	if( (mcookie < 0) || (mcookie >= srclpmh->m_kindnum) ){
		DbgOut( "quafile : SaveQuaFile2Buf : mcookie error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	lpsh = srclpsh;
	lpmh = srclpmh;
	lpth = srclpth;
	m_motcookie = mcookie;

	if( bufsize > 0 ){
		m_savemode = SAVEQUA_BUF;
	}else{
		m_savemode = SAVEQUA_BUFSIZE;
	}

	m_writemode = 1;

	m_savestart = 0;
	m_saveend = *(srclpmh->m_motnum + mcookie) - 1;//!!!!!!!!!!!!!!!!!!


	ret = WriteQuaFile_aft( namequaflag );
	if( ret ){
		DbgOut( "quafile : SaveQuaFile2Buf : SaveQuaFile_aft error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savequaexit2;
	}

savequaexit2:
	*writesize = m_buf.pos;
	m_buf.buf = 0;
	m_buf.pos = 0;

	return ret;

}


int CQuaFile::SaveQuaFile( int namequaflag, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, char* fname, int mcookie, int srcstart, int srcend )
{
	int ret = 0;

	if( !srclpmh )
		return 1;

	if( (mcookie < 0) || (mcookie >= srclpmh->m_kindnum) ){
		DbgOut( "quafile : SaveQuaFile : mcookie error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;
	m_motcookie = mcookie;

	m_savemode = SAVEQUA_FILE;

	m_savestart = srcstart;
	m_saveend = srcend;

	hfile = CreateFile( (LPCTSTR)fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CQuaFile::SaveQuaFile : CreateFile error !!!\n" );
		_ASSERT( 0 );
		//return 1;
		ret = 1;
		goto savequaexit1;
	}	
	m_writemode = 1;



	ret = WriteQuaFile_aft( namequaflag );
	if( ret ){
		DbgOut( "quafile : SaveQuaFile : SaveQuaFile_aft error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savequaexit1;
	}

savequaexit1:

	FlushFileBuffers( hfile );
	SetEndOfFile( hfile );
	CloseHandle( hfile );
	hfile = INVALID_HANDLE_VALUE;

	return ret;
}

int CQuaFile::WriteNameFileMark( HANDLE srchfile )
{
	int ret;
	char wname[256];
	ZeroMemory( wname, sizeof( char ) * 256 );
	strcpy_s( wname, 256, "名前基準" );
	ret = WriteUCharaData( srchfile, (unsigned char*)wname, 9 );
	if( ret ){
		DbgOut( "quafile : SaveQuaFile : Write namebasefile header error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CQuaFile::WriteQuaFile_aft( int namequaflag )
{

	QUAFILEHDR header;
	int leng;
	//header.magicno = QUAFILEMAGICNO;
	//header.magicno = QUAFILEMAGICNO2;
	//header.magicno = QUAFILEMAGICNO4;
	//header.magicno = QUAFILEMAGICNO5;
	//header.magicno = QUAFILEMAGICNO6;
	//header.magicno = QUAFILEMAGICNO7;
	//header.magicno = QUAFILEMAGICNO8;
	//header.magicno = QUAFILEMAGICNO9;
	//header.magicno = QUAFILEMAGICNO10;
	//header.magicno = QUAFILEMAGICNO11;
	//header.magicno = QUAFILEMAGICNO12;
	header.magicno = QUAFILEMAGICNO13;
	char* mname;
	CMotionCtrl* mcptr = (*lpmh)( 1 );
	_ASSERT( mcptr );
	CMotionInfo* miptr = mcptr->motinfo;
	_ASSERT( miptr );

	_ASSERT( miptr->motname );
	mname = *(miptr->motname + m_motcookie);
	_ASSERT( mname );
	leng = (int)strlen( mname );
	if( leng > 255 ){
		DbgOut( "CQuaFile::SaveQuaFile : name leng error %d !!!\n", leng );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( header.motname, 256 );
	strncpy_s( header.motname, 256, mname, leng );

	_ASSERT( miptr->motnum );
	//header.framenum = *(miptr->motnum + m_motcookie);
	header.framenum = m_saveend - m_savestart + 1;//!!!!!!!!!!!!!!!!!!!!!!
	_ASSERT( miptr->mottype );
	header.mottype = *(miptr->mottype + m_motcookie);
	int ret;
	ret = WriteQuaHeader( header );
	if( ret ){
		DbgOut( "CQuaFile::SaveQuaFile : WriteQuaHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	//////////
	QUAFILEHDR2 header2;
	ZeroMemory( &header2, sizeof( QUAFILEHDR2 ) );
	lpmh->GetDefInterp( m_motcookie, &(header2.interpolation) );
	header2.flags |= FLAGS_HASSCALE;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 2005/04/22
	if( namequaflag != 0 ){
		header2.flags |= FLAGS_NAMEFILE;
	}
	m_hasscale = 1;//!!!!!!!!!!!!!!!!!!!!!!!
	header2.motjump = *(miptr->motjump + m_motcookie);
	header2.zatype = lpmh->GetZaType( m_motcookie );
	ret = WriteQuaHeader2( header2 );
	if( ret ){
		DbgOut( "CQuaFile::SaveQuaFile : WriteQuaHeader2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( header2.flags & FLAGS_NAMEFILE ){
		ret = WriteNameFileMark( hfile );
		if( ret ){
			DbgOut( "quafile : SaveQuaFile : WriteNameFileMark error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	/////////

	ret = WriteMorphIndices( hfile );
	if( ret ){
		DbgOut( "quafile : WriteQuaFile_aft : WriteMorphIndices error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


////////////
	CMotionCtrl* firstmc;
	firstmc = (*lpmh)( 1 );
	_ASSERT( firstmc );

	if( namequaflag == 0 ){

		WriteQuaDataReq( firstmc );

		QUADATA2 enddata;
		enddata.boneno = -1;
		ret = WriteQuaData2( enddata );
		if( ret ){
			DbgOut( "CQuaFile::SaveQuaFile : WriteQuaData enddata error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		QUASPPARAM qspp;
		ZeroMemory( &qspp, sizeof( QUASPPARAM ) );
		ret = WriteUCharaData( hfile, (unsigned char*)(&qspp), sizeof( QUASPPARAM ) );
		_ASSERT( !ret );

	///////////
		WriteTexAnimReq( firstmc );

		QUATEXKEY findata;
		ZeroMemory( &findata, sizeof( QUATEXKEY ) );
		findata.boneno = -1;
		ret = WriteQuaTexKey( findata );
		if( ret ){
			DbgOut( "quafile : SaveQuaFile : WriteQuaTexKey findata error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	///////////
		WriteAlpAnimReq( firstmc );

		QUAALPKEY findataA;
		ZeroMemory( &findataA, sizeof( QUAALPKEY ) );
		findataA.boneno = -1;
		ret = WriteQuaAlpKey( findataA );
		if( ret ){
			DbgOut( "quafile : SaveQuaFile : WriteQuaAlpKey findataA error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	////////////
		WriteDSReq( firstmc );
		QUADSKEY findata2;
		ZeroMemory( &findata2, sizeof( QUADSKEY ) );
		findata2.boneno = -1;
		ret = WriteQuaDSKey( findata2 );
		if( ret ){
			DbgOut( "quafile : SaveQuaFile : WriteQuaDSKey findata2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	//////////////
		WriteMorphReq( firstmc );
		int finboneno = -1;
		ret = WriteUCharaData( hfile, (unsigned char*)&finboneno, sizeof( int ) );
		if( ret ){
			DbgOut( "quafile : SaveQuaFile : write finboneno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		int totalelemnum;
		totalelemnum = lpsh->s2shd_leng;
		ret = WriteUCharaData( hfile, (unsigned char*)&totalelemnum, sizeof( int ) );
		if( ret ){
			DbgOut( "quafile : WriteQuaFile_aft : write totalelemnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int topnum = 0;
		ret = GetTopLevelElem( 0, &topnum );
		if( ret ){
			DbgOut( "quafile : SaveQuaFile : GetTopLevelElem 0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( topnum <= 0 ){
			return 0;
		}
		CMotionCtrl** pptopmc;
		pptopmc = (CMotionCtrl**)malloc( sizeof( CMotionCtrl* ) * topnum );
		if( !pptopmc ){
			DbgOut( "quafile : SaveQuaFile : topmc alloc error !!!\n" );
			_ASSERT( 0 );
		}
		ZeroMemory( pptopmc, sizeof( CMotionCtrl* ) * topnum );

		int tempnum = 0;
		ret = GetTopLevelElem( pptopmc, &tempnum );
		if( ret || (tempnum != topnum) ){
			DbgOut( "quafile : SaveQuaFile : GetTopLevelElem 1 error !!!\n" );
			_ASSERT( 0 );
			free( pptopmc );
			return 1;
		}

		int topno;
		for( topno = 0; topno < topnum; topno++ ){
			CMotionCtrl* curtop = *( pptopmc + topno );
			int broflag = 0;
			int errorcnt = 0;
			WriteNameQuaElemReq( curtop, broflag, &errorcnt );
			if( errorcnt > 0 ){
				DbgOut( "quafile : SaveQuaFile : WriteNameQuaElemReq error %d\n", errorcnt );
				_ASSERT( 0 );
				free( pptopmc );
				return 1;
			}
		}
		free( pptopmc );

		QUANAMEHDR endmark;
		ZeroMemory( &endmark, sizeof( QUANAMEHDR ) );
		endmark.seri = -1;
		strcpy_s( endmark.name, 256, "__endofnamequa__" );
		ret = WriteQuaNameHdr( endmark );
		if( ret ){
			DbgOut( "quafile : WriteQuaFile_aft : write endmark error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int CQuaFile::GetTopLevelElem( CMotionCtrl** pptopmc, int* topnum )
{
	int seri;
	CShdElem* selem;


	*topnum = 0;
	int setno = 0;

	for( seri = 0; seri < lpsh->s2shd_leng; seri++ ){
		selem = (*lpsh)( seri );
		if( selem->IsJoint() ){
			CShdElem* parjoint;
			parjoint = lpsh->FindUpperJoint( selem );
			if( !parjoint ){
				if( pptopmc ){
					*( pptopmc + setno ) = (*lpmh)( selem->serialno );
				}
				setno++;
			}
		}
	}
	*topnum = setno;

	return 0;
}
void CQuaFile::WriteNameQuaElemReq( CMotionCtrl* mcptr, int broflag, int* errorcnt )
{
	QUADATA3 data3;
	QUANAMEHDR namehdr;
	CShdElem* selem;
	CShdElem* parselem;


	int ret;

	selem = (*lpsh)( mcptr->serialno );
	_ASSERT( selem );


	if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
		if( mcptr->boneno >= 0 ){
			ZeroMemory( &namehdr, sizeof( QUANAMEHDR ) );
			
			CTreeElem2* telem;
			telem = (*lpth)( mcptr->serialno );
			_ASSERT( telem );
			strcpy_s( namehdr.name, 256, telem->name );
			namehdr.seri = mcptr->serialno;
					

			parselem = lpsh->FindUpperJoint( selem );
			if( parselem ){
				namehdr.parent = parselem->serialno;
			}else{
				namehdr.parent = -1;
			}

			int frameleng = m_saveend - m_savestart + 1;
			int* framearray;
			framearray = (int*)malloc( sizeof( int ) * frameleng );
			if( !framearray ){
				DbgOut( "quafile : WriteNameQuaElemReq : framearray alloc error !!!\n" );
				_ASSERT( 0 );
				(*errorcnt)++;
				return;
			}

			ZeroMemory( framearray, sizeof( int ) * frameleng );

			int framenum = 0;
			ret = lpmh->GetKeyframeNoRange( m_motcookie, mcptr->serialno, m_savestart, m_saveend, framearray, frameleng, &framenum );
			if( ret ){
				DbgOut( "quafile : WriteNameQuaElemReq : HuGetKeyframeNo error !!!\n" );
				_ASSERT( 0 );
				free( framearray );
				(*errorcnt)++;
				return;
			}
			namehdr.framenum = framenum;

			int* texframearray;
			texframearray = (int*)malloc( sizeof( int ) * frameleng );
			if( !texframearray ){
				DbgOut( "quafile : WriteNameQuaElemReq : texframearray alloc error !!!\n" );
				_ASSERT( 0 );
				(*errorcnt)++;
				free( framearray );
				return;
			}
			ZeroMemory( texframearray, sizeof( int ) * frameleng );
			int texframenum = 0;
			ret = lpmh->GetTexAnimKeyframeNoRange( mcptr->serialno, m_motcookie, m_savestart, m_saveend, texframearray, frameleng, &texframenum );
			if( ret ){
				DbgOut( "quafile : WriteNameQuaElemReq : GetTexAnimKeyframeNoRange error !!!\n" );
				_ASSERT( 0 );
				free( framearray );
				free( texframearray );
				(*errorcnt)++;
				return;
			}
			namehdr.texframenum = texframenum;


			int* alpframearray;
			alpframearray = (int*)malloc( sizeof( int ) * frameleng );
			if( !alpframearray ){
				DbgOut( "quafile : WriteNameQuaElemReq : alpframearray alloc error !!!\n" );
				_ASSERT( 0 );
				(*errorcnt)++;
				free( framearray );
				return;
			}
			ZeroMemory( alpframearray, sizeof( int ) * frameleng );
			int alpframenum = 0;
			ret = lpmh->GetAlpAnimKeyframeNoRange( mcptr->serialno, m_motcookie, m_savestart, m_saveend, alpframearray, frameleng, &alpframenum );
			if( ret ){
				DbgOut( "quafile : WriteNameQuaElemReq : GetAlpAnimKeyframeNoRange error !!!\n" );
				_ASSERT( 0 );
				free( framearray );
				free( alpframearray );
				(*errorcnt)++;
				return;
			}
			namehdr.alpframenum = alpframenum;


			int* dsframearray;
			dsframearray = (int*)malloc( sizeof( int ) * frameleng );
			if( !dsframearray ){
				DbgOut( "quafile : WriteNameQuaElemReq : dsframearray alloc error !!!\n" );
				_ASSERT( 0 );
				(*errorcnt)++;
				free( framearray );
				free( texframearray );
				return;
			}
			ZeroMemory( dsframearray, sizeof( int ) * frameleng );
			int dsframenum = 0;
			ret = lpmh->GetDSAnimKeyframeNoRange( mcptr->serialno, m_motcookie, m_savestart, m_saveend, dsframearray, frameleng, &dsframenum );
			if( ret ){
				DbgOut( "quafile : WriteNameQuaElemReq : GetTexDSAnimKeyframeNoRange error !!!\n" );
				_ASSERT( 0 );
				free( framearray );
				free( texframearray );
				free( dsframearray );
				(*errorcnt)++;
				return;
			}
			namehdr.dsframenum = dsframenum;
			

			ret = WriteQuaNameHdr( namehdr );
			_ASSERT( !ret );		
// motion data			 
			int elemno;
			for( elemno = 0; elemno < framenum; elemno++ ){
				int frameno;
				int writeframeno;
				frameno = *( framearray + elemno );
				CMotionPoint2* mpptr = 0;
				ret = lpmh->IsExistMotionPoint( m_motcookie, mcptr->serialno, frameno, &mpptr );
				if( ret || !mpptr ){
					DbgOut( "quafile : WriteNameQuaElemReq : IsExistMotionPoint error !!!\n" );
					_ASSERT( 0 );
					free( framearray );
					free( texframearray );
					free( dsframearray );
					(*errorcnt)++;
					return;
				}						
				writeframeno = frameno - m_savestart;
				
				QUADATA3 data3;
				ZeroMemory( &data3, sizeof( QUADATA3 ) );

				data3.ishead = mpptr->ishead;
				data3.m_frameno = writeframeno;
				data3.m_rot.x = mpptr->m_rotx;
				data3.m_rot.y = mpptr->m_roty;
				data3.m_rot.z = mpptr->m_rotz;
				data3.m_mv.x = mpptr->m_mvx;
				data3.m_mv.y = mpptr->m_mvy;
				data3.m_mv.z = mpptr->m_mvz;
				data3.m_q = mpptr->m_q;
				data3.serialno = mpptr->serialno;
				data3.interp = mpptr->interp;
				data3.m_scale.x = mpptr->m_scalex;
				data3.m_scale.y = mpptr->m_scaley;
				data3.m_scale.z = mpptr->m_scalez;
				data3.m_userint1 = mpptr->m_userint1;
				data3.m_eul = mpptr->m_eul;

				ret = WriteQuaData3( data3 );
				if( ret ){
					DbgOut( "quafile : WriteNameQuaElemReq : WriteQuaData3 error !!!\n" );
					_ASSERT( 0 );
					free( framearray );
					free( texframearray );
					free( dsframearray );
					(*errorcnt)++;
					return;
				}

				QUASPPARAM qspp;
				ZeroMemory( &qspp, sizeof( QUASPPARAM ) );
				qspp.m_rotparam = mpptr->m_spp->m_rotparam;
				qspp.m_mvparam = mpptr->m_spp->m_mvparam;
				qspp.m_scparam = mpptr->m_spp->m_scparam;
				ret = WriteUCharaData( hfile, (unsigned char*)(&qspp), sizeof( QUASPPARAM ) );
				if( ret ){
					DbgOut( "quafile : WriteNameQuaElemReq : WriteUC qspp error !!!\n" );
					_ASSERT( 0 );
					free( framearray );
					free( texframearray );
					free( dsframearray );
					(*errorcnt)++;
					return;
				}
			}


// texanim
			int texelemno;
			for( texelemno = 0; texelemno < texframenum; texelemno++ ){
				int frameno;
				int writeframeno;
				frameno = *( texframearray + texelemno );
				
				CTexKey* tkptr = 0;
				ret = lpmh->ExistTexKey( mcptr->serialno, m_motcookie, frameno, &tkptr );
				if( ret || !tkptr ){
					DbgOut( "quafile : WriteNameQuaElemReq : ExistTexKey error !!!\n" );
					_ASSERT( 0 );
					free( framearray );
					free( texframearray );
					free( dsframearray );
					(*errorcnt)++;
					return;
				}						
				writeframeno = frameno - m_savestart;

				QUATEXKEY qtk;
				ZeroMemory( &qtk, sizeof( QUATEXKEY ) );
				qtk.boneno = mcptr->boneno;
				qtk.frameno = writeframeno;
				qtk.changenum = tkptr->changenum;
				ret = WriteQuaTexKey( qtk );
				if( ret ){
					DbgOut( "quafile : WriteNameQuaElemReq : WriteQuaTexKey error !!!\n" );
					_ASSERT( 0 );
					(*errorcnt)++;
					return;
				}

				int tcno;
				for( tcno = 0; tcno < tkptr->changenum; tcno++ ){
					CTexChange* tcptr = *( tkptr->texchange + tcno );
					_ASSERT( tcptr );

					QUATEXCHANGE qtc;
					ZeroMemory( &qtc, sizeof( QUATEXCHANGE ) );
					strcpy_s( qtc.matname, 256, tcptr->m_mqomat->name );
					strcpy_s( qtc.texname, 256, tcptr->m_change );
					ret = WriteQuaTexChange( qtc );
					if( ret ){
						DbgOut( "quafile : WriteNameQuaElemReq : WriteQuaTexChange error !!!\n" );
						_ASSERT( 0 );
						(*errorcnt)++;
						return;
					}
				}
			}

// alpanim
			int alpelemno;
			for( alpelemno = 0; alpelemno < alpframenum; alpelemno++ ){
				int frameno;
				int writeframeno;
				frameno = *( alpframearray + alpelemno );
				
				CAlpKey* alpkptr = 0;
				ret = lpmh->ExistAlpKey( mcptr->serialno, m_motcookie, frameno, &alpkptr );
				if( ret || !alpkptr ){
					DbgOut( "quafile : WriteNameQuaElemReq : ExistAlpKey error !!!\n" );
					_ASSERT( 0 );
					free( framearray );
					free( alpframearray );
					free( dsframearray );
					(*errorcnt)++;
					return;
				}						
				writeframeno = frameno - m_savestart;

				QUAALPKEY qalpk;
				ZeroMemory( &qalpk, sizeof( QUAALPKEY ) );
				qalpk.boneno = mcptr->boneno;
				qalpk.frameno = writeframeno;
				qalpk.changenum = alpkptr->changenum;
				ret = WriteQuaAlpKey( qalpk );
				if( ret ){
					DbgOut( "quafile : WriteNameQuaElemReq : WriteQuaAlpKey error !!!\n" );
					_ASSERT( 0 );
					(*errorcnt)++;
					return;
				}

				int alpcno;
				for( alpcno = 0; alpcno < alpkptr->changenum; alpcno++ ){
					CAlpChange* alpcptr = *( alpkptr->alpchange + alpcno );
					_ASSERT( alpcptr );

					QUAALPCHANGE alpc;
					ZeroMemory( &alpc, sizeof( QUAALPCHANGE ) );
					strcpy_s( alpc.matname, 256, alpcptr->mqomat->name );
					alpc.alpha = alpcptr->change;
					ret = WriteQuaAlpChange( alpc );
					if( ret ){
						DbgOut( "quafile : WriteNameQuaElemReq : WriteQuaAlpChange error !!!\n" );
						_ASSERT( 0 );
						(*errorcnt)++;
						return;
					}
				}
			}



// dsanim
			int dselemno;
			for( dselemno = 0; dselemno < dsframenum; dselemno++ ){
				int frameno;
				int writeframeno;
				frameno = *( dsframearray + dselemno );
				
				CDSKey* dskptr = 0;
				ret = lpmh->ExistDSKey( mcptr->serialno, m_motcookie, frameno, &dskptr );
				if( ret || !dskptr ){
					DbgOut( "quafile : WriteNameQuaElemReq : ExistDSKey error !!!\n" );
					_ASSERT( 0 );
					free( framearray );
					free( texframearray );
					free( dsframearray );
					(*errorcnt)++;
					return;
				}						
				writeframeno = frameno - m_savestart;

				QUADSKEY qdsk;
				ZeroMemory( &qdsk, sizeof( QUADSKEY ) );
				qdsk.boneno = mcptr->boneno;
				qdsk.frameno = writeframeno;
				qdsk.changenum = dskptr->changenum;
				ret = WriteQuaDSKey( qdsk );
				if( ret ){
					DbgOut( "quafile : WriteNameQuaElemReq : WriteQuaDSKey error !!!\n" );
					_ASSERT( 0 );
					(*errorcnt)++;
					return;
				}

				int dscno;
				for( dscno = 0; dscno < dskptr->changenum; dscno++ ){
					CDSChange* dscptr = *( dskptr->dschange + dscno );
					_ASSERT( dscptr );

					QUADSCHANGE qdsc;
					ZeroMemory( &qdsc, sizeof( QUADSCHANGE ) );
					qdsc.switchno = dscptr->m_dsptr->switchno;
					qdsc.change = dscptr->m_change;
					ret = WriteQuaDSChange( qdsc );
					if( ret ){
						DbgOut( "quafile : WriteNameQuaElemReq : WriteQuaDSChange error !!!\n" );
						_ASSERT( 0 );
						(*errorcnt)++;
						return;
					}
				}
			}

//			
			if( framearray )
				free( framearray );
			if( texframearray )
				free( texframearray );
			if( dsframearray )
				free( dsframearray );
			if( alpframearray )
				free( alpframearray );
			
			////////// morph anim
			ret = WriteMorphKey( mcptr, QUATYPE_NAME );
			if( ret ){
				DbgOut( "quafile : WriteNameQuaElemReq : WriteMorphKey error !!!\n" );
				_ASSERT( 0 );
				(*errorcnt)++;
				return;
			}
		}
	}


	//child
	CShdElem* childselem;
	childselem = selem->child;
	if( childselem ){
		CShdElem* validchild;
		validchild = lpsh->GetValidChild( childselem );
		if( validchild ){
			CMotionCtrl* childmc;
			childmc = (*lpmh)( validchild->serialno );
			WriteNameQuaElemReq( childmc, 1, errorcnt );
		}
	}

	//brother
	CShdElem* broselem;
	broselem = selem->brother;
	if( broflag && broselem ){
		CShdElem* validbro;
		validbro = lpsh->GetValidBrother( broselem );
		if( validbro ){
			CMotionCtrl* bromc;
			bromc = (*lpmh)( validbro->serialno );
			WriteNameQuaElemReq( bromc, 1, errorcnt );
		}
	}

	return;
}


int CQuaFile::WriteMorphKey( CMotionCtrl* mcptr, int quatype )
{
	int ret;
	if( quatype == QUATYPE_NUM ){
		int boneno = mcptr->boneno;
		ret = WriteUCharaData( hfile, (unsigned char*)&boneno, sizeof( int ) );
		_ASSERT( !ret );
	}

	CMotionInfo* motinfo;
	motinfo = mcptr->motinfo;
	_ASSERT( motinfo );

	CMMotKey* curmmk;
	curmmk = motinfo->GetFirstMMotKey( m_motcookie );
	while( curmmk ){
		int mmeno;
		for( mmeno = 0; mmeno < curmmk->mmenum; mmeno++ ){
			CMMotElem* curmme;
			curmme = *( curmmk->ppmme + mmeno );
			CShdElem* curbase;
			curbase = curmme->m_baseelem;
			int curbaseindex;
			curbaseindex = Seri2MorphIndex( curbase->serialno );
			if( curbaseindex >= 0 ){
				int tno;
				for( tno = 0; tno < curmme->m_primnum; tno++ ){
					MPRIM* targetmp;
					targetmp = curmme->m_primarray + tno;
					CShdElem* curtarget;
					curtarget = targetmp->selem;
					int curtargetindex;
					curtargetindex = Seri2MorphIndex( curtarget->serialno );
					if( curtargetindex >= 0 ){
						MORPHVAL curmv;
						curmv.frameno = curmmk->frameno - m_savestart;
						curmv.baseindex = curbaseindex;
						curmv.targetindex = curtargetindex;
						curmv.value = targetmp->value;
						ret = WriteMorphVal( &curmv );
						if( ret ){
							_ASSERT( 0 );
							return 1;
						}
					}
				}
			}
		}
		curmmk = curmmk->next;
	}
	//end mark
	MORPHVAL endmv;
	endmv.frameno = -1;
	endmv.baseindex = -1;
	endmv.targetindex = -1;
	endmv.value = 0.0f;
	ret = WriteMorphVal( &endmv );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CQuaFile::WriteUCharaData( HANDLE outfile, unsigned char* dataptr, DWORD outsize )
{
	/***
	DWORD writeleng;
	WriteFile( outfile, (void*)(dataptr), outsize, &writeleng, NULL );
	if( outsize != writeleng ){
		DbgOut( "CQuaFile::WriteUCharaData : writefile error %d %d\n", outsize, writeleng );
		_ASSERT( 0 );
		return 1;
	}
	***/

	DWORD writeleng;
	DWORD wleng = outsize;

	switch( m_savemode ){
	case SAVEQUA_FILE:

		WriteFile( outfile, (void*)(dataptr), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteUCharaData : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteUCharaData : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(dataptr), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}


int CQuaFile::WriteQuaHeader( QUAFILEHDR hdr )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUAFILEHDR );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)(&hdr), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaHeader : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteQuaHeader : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&hdr), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}
	
	return 0;
}
int CQuaFile::WriteQuaHeader2( QUAFILEHDR2 hdr )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUAFILEHDR2 );

	switch( m_savemode ){
	case SAVEQUA_FILE:

		WriteFile( hfile, (void*)(&hdr), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaHeader2 : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteQuaHeader2 : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&hdr), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}
/***
int CQuaFile::WriteQuaData( QUADATA data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUADATA );
	WriteFile( hfile, (void*)(&data), wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "CQuaFile::WriteQuaData : writefile error %d %d\n", wleng, writeleng );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/
int CQuaFile::WriteQuaData2( QUADATA2 data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUADATA2 );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)(&data), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaData2 : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteQuaData2 : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&data), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}
int CQuaFile::WriteQuaTexKey( QUATEXKEY data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUATEXKEY );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)(&data), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaTexKey : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteTexKey : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&data), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}
int CQuaFile::WriteQuaAlpKey( QUAALPKEY data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUAALPKEY );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)(&data), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaAlpKey : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteAlpKey : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&data), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}

int CQuaFile::WriteQuaTexChange( QUATEXCHANGE data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUATEXCHANGE );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)(&data), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaTexChange : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteTexChange : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&data), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}
int CQuaFile::WriteQuaAlpChange( QUAALPCHANGE data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUAALPCHANGE );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)(&data), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaAlpChange : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteAlpChange : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&data), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}

void CQuaFile::WriteTexAnimReq( CMotionCtrl* mcptr )
{
	int ret;

	if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
		if( mcptr->boneno >= 0 ){
			CTexKey* tkptr = lpmh->GetFirstTexKey( mcptr->serialno, m_motcookie );
			while( tkptr ){
				QUATEXKEY qtk;
				ZeroMemory( &qtk, sizeof( QUATEXKEY ) );
				qtk.boneno = mcptr->boneno;
				qtk.frameno = tkptr->frameno - m_savestart;
				qtk.changenum = tkptr->changenum;
				ret = WriteQuaTexKey( qtk );
				if( ret ){
					DbgOut( "quafile : WriteTexAnimReq : WriteQuaTexKey error !!!\n" );
					_ASSERT( 0 );
					return;
				}

				int tcno;
				for( tcno = 0; tcno < tkptr->changenum; tcno++ ){
					CTexChange* tcptr = *( tkptr->texchange + tcno );
					_ASSERT( tcptr );

					QUATEXCHANGE qtc;
					ZeroMemory( &qtc, sizeof( QUATEXCHANGE ) );
					strcpy_s( qtc.matname, 256, tcptr->m_mqomat->name );
					strcpy_s( qtc.texname, 256, tcptr->m_change );
					ret = WriteQuaTexChange( qtc );
					if( ret ){
						DbgOut( "quafile : WriteTexAnimReq : WriteQuaTexChange error !!!\n" );
						_ASSERT( 0 );
						return;
					}
				}

				tkptr = tkptr->next;
			}
		}
	}
	
	CMotionCtrl* chilmc;
	chilmc = mcptr->child;
	if( chilmc ){
		WriteTexAnimReq( chilmc );
	}
	
	CMotionCtrl* bromc;
	bromc = mcptr->brother;
	if( bromc ){
		WriteTexAnimReq( bromc );
	}
	
}
void CQuaFile::WriteAlpAnimReq( CMotionCtrl* mcptr )
{
	int ret;

	if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
		if( mcptr->boneno >= 0 ){
			CAlpKey* alpkptr = lpmh->GetFirstAlpKey( mcptr->serialno, m_motcookie );
			while( alpkptr ){
				QUAALPKEY alpk;
				ZeroMemory( &alpk, sizeof( QUAALPKEY ) );
				alpk.boneno = mcptr->boneno;
				alpk.frameno = alpkptr->frameno - m_savestart;
				alpk.changenum = alpkptr->changenum;
				ret = WriteQuaAlpKey( alpk );
				if( ret ){
					DbgOut( "quafile : WriteAlpAnimReq : WriteQuaAlpKey error !!!\n" );
					_ASSERT( 0 );
					return;
				}

				int alpcno;
				for( alpcno = 0; alpcno < alpkptr->changenum; alpcno++ ){
					CAlpChange* alpcptr = *( alpkptr->alpchange + alpcno );
					_ASSERT( alpcptr );

					QUAALPCHANGE qalpc;
					ZeroMemory( &qalpc, sizeof( QUAALPCHANGE ) );
					strcpy_s( qalpc.matname, 256, alpcptr->mqomat->name );
					qalpc.alpha = alpcptr->change;
					ret = WriteQuaAlpChange( qalpc );
					if( ret ){
						DbgOut( "quafile : WriteAlpAnimReq : WriteQuaAlpChange error !!!\n" );
						_ASSERT( 0 );
						return;
					}
				}

				alpkptr = alpkptr->next;
			}
		}
	}
	
	CMotionCtrl* chilmc;
	chilmc = mcptr->child;
	if( chilmc ){
		WriteAlpAnimReq( chilmc );
	}
	
	CMotionCtrl* bromc;
	bromc = mcptr->brother;
	if( bromc ){
		WriteAlpAnimReq( bromc );
	}
	
}

int CQuaFile::WriteQuaNameHdr( QUANAMEHDR data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUANAMEHDR );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)(&data), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaNameHdr : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteQuaNameHdr : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&data), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}

int CQuaFile::WriteQuaData3( QUADATA3 data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUADATA3 );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)(&data), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaData3 : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteQuaData3 : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&data), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}


int CQuaFile::WriteQuaDSKey( QUADSKEY data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUADSKEY );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)(&data), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaDSKey : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteDSKey : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&data), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}
int CQuaFile::WriteQuaDSChange( QUADSCHANGE data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( QUADSCHANGE );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)(&data), wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteQuaDSChange : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteDSChange : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)(&data), wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}
int CQuaFile::WriteMorphIndex( HANDLE srchfile, MORPHINDEX* data, int num )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( MORPHINDEX ) * num;

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( srchfile, (void*)data, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteMorphIndex : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteMorphIndex : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)data, wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}
int CQuaFile::WriteMorphVal( MORPHVAL* data )
{
	DWORD wleng, writeleng;
	
	wleng = sizeof( MORPHVAL );

	switch( m_savemode ){
	case SAVEQUA_FILE:
		WriteFile( hfile, (void*)data, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "CQuaFile::WriteMorphIndex : writefile error %d %d\n", wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SAVEQUA_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "quafile : WriteMorphIndex : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, (void*)data, wleng );
		m_buf.pos += wleng;

		break;
	case SAVEQUA_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}

void CQuaFile::WriteMorphReq( CMotionCtrl* mcptr )
{
	int ret;

	if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
		if( mcptr->boneno >= 0 ){
			ret = WriteMorphKey( mcptr, QUATYPE_NUM );
			if( ret ){
				DbgOut( "quafile : WRiteMorphReq : WriteMorphKey error !!!\n" );
				_ASSERT( 0 );
				return;
			}
		}
	}

	CMotionCtrl* chilmc;
	chilmc = mcptr->child;
	if( chilmc ){
		WriteMorphReq( chilmc );
	}
	
	CMotionCtrl* bromc;
	bromc = mcptr->brother;
	if( bromc ){
		WriteMorphReq( bromc );
	}

}


void CQuaFile::WriteDSReq( CMotionCtrl* mcptr )
{
	int ret;

	if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
		if( mcptr->boneno >= 0 ){
			CDSKey* dskptr = lpmh->GetFirstDSKey( mcptr->serialno, m_motcookie );
			while( dskptr ){
				QUADSKEY qdsk;
				ZeroMemory( &qdsk, sizeof( QUADSKEY ) );
				qdsk.boneno = mcptr->boneno;
				qdsk.frameno = dskptr->frameno - m_savestart;
				qdsk.changenum = dskptr->changenum;
				ret = WriteQuaDSKey( qdsk );
				if( ret ){
					DbgOut( "quafile : WriteDSReq : WriteQuaDSKey error !!!\n" );
					_ASSERT( 0 );
					return;
				}

				int dscno;
				for( dscno = 0; dscno < dskptr->changenum; dscno++ ){
					CDSChange* dscptr = *( dskptr->dschange + dscno );
					_ASSERT( dscptr );

					QUADSCHANGE qdsc;
					ZeroMemory( &qdsc, sizeof( QUADSCHANGE ) );
					qdsc.switchno = dscptr->m_dsptr->switchno;
					qdsc.change = dscptr->m_change;
					ret = WriteQuaDSChange( qdsc );
					if( ret ){
						DbgOut( "quafile : WriteDSReq : WriteQuaDSChange error !!!\n" );
						_ASSERT( 0 );
						return;
					}
				}

				dskptr = dskptr->next;
			}
		}
	}
	
	CMotionCtrl* chilmc;
	chilmc = mcptr->child;
	if( chilmc ){
		WriteDSReq( chilmc );
	}
	
	CMotionCtrl* bromc;
	bromc = mcptr->brother;
	if( bromc ){
		WriteDSReq( bromc );
	}
	
}



void CQuaFile::WriteQuaDataReq( CMotionCtrl* mcptr )
{
	//QUADATA data;
	QUADATA2 data2;

	int ret;
	if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
		if( mcptr->boneno >= 0 ){
			CMotionPoint2* mpptr = mcptr->GetMotionPoint( m_motcookie );
			CMotionPoint2* curmp = mpptr;
			while( curmp ){
				data2.boneno = mcptr->boneno;
				data2.mp = *curmp;
				data2.mp.m_spp = 0;				

//if( data2.mp.dispswitch != 0 ){
//	DbgOut( "quafile : WriteQuaDataReq : dispswitch %x\n", data2.mp.dispswitch );
//}

				if( (data2.mp.m_frameno >= m_savestart) && (data2.mp.m_frameno <= m_saveend) ){

					data2.mp.m_frameno -= m_savestart;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!

					ret = WriteQuaData2( data2 );
					if( ret ){
						DbgOut( "QuaFile : WriteQuaDataReq : WriteQuaData2 error !!!\n" );
						_ASSERT( 0 );
						return;
					}
					QUASPPARAM qspp;
					ZeroMemory( &qspp, sizeof( QUASPPARAM ) );
					qspp.m_rotparam = curmp->m_spp->m_rotparam;
					qspp.m_mvparam = curmp->m_spp->m_mvparam;
					qspp.m_scparam = curmp->m_spp->m_scparam;

					ret = WriteUCharaData( hfile, (unsigned char*)(&qspp), sizeof( QUASPPARAM ) );
					if( ret ){
						DbgOut( "QuaFile : WriteQuaDataReq : WriteUC qspp error !!!\n" );
						_ASSERT( 0 );
						return;
					}

				}
				curmp = curmp->next;
			}
		}
	}

	CMotionCtrl* chilmc;
	chilmc = mcptr->child;
	if( chilmc ){
		WriteQuaDataReq( chilmc );
	}
	
	CMotionCtrl* bromc;
	bromc = mcptr->brother;
	if( bromc ){
		WriteQuaDataReq( bromc );
	}
}


int CQuaFile::LoadQuaFile( CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, char* fname, LONG* mcookieptr, D3DXVECTOR3 quamult )
{
	if( !srclpmh ){
		*mcookieptr = -1;
		return 1;
	}
	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;

	m_quamult = quamult;


	char pathname[2048];
	int ch = '\\';
	char* enptr = 0;
	enptr = strrchr( fname, ch );
	if( enptr ){
		strcpy_s( pathname, 2048, fname );
	}else{
		ZeroMemory( pathname, 2048 );
		int resdirleng;
		resdirleng = GetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPTSTR)pathname, 2048 );
		_ASSERT( resdirleng );
		strcat_s( pathname, 2048, fname );
	}

	hfile = CreateFile( (LPCTSTR)pathname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CQuaFile : LoadQuaFile : CreateFile error !!!\n" );
		_ASSERT( 0 );
		*mcookieptr = -1;
		return D3DAPPERR_MEDIANOTFOUND;
	}	


	m_writemode = 0;
	m_frombuf = 0;

	int ret;
	ret = LoadQuaFile_aft( mcookieptr );
	if( ret ){
		DbgOut( "quafile : LoadQuaFile : LoadQuaFile_aft error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

int CQuaFile::LoadQuaFileFromPnd( CPanda* panda, int pndid, char* fname, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, LONG* mcookieptr, float quamult )
{
	if( !srclpmh ){
		*mcookieptr = -1;
		return 1;
	}
	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;

	m_quamult = D3DXVECTOR3( quamult, quamult, quamult );


	int ret;
	PNDPROP prop;
	ZeroMemory( &prop, sizeof( PNDPROP ) );
	ret = panda->GetProperty( pndid, fname, &prop );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	m_buf.buf = (char*)malloc( sizeof( char ) * prop.sourcesize );
	if( !(m_buf.buf) ){
		DbgOut( "quafile : LoadQuaFileFromPnd : buf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int getsize = 0;
	ret = panda->Decrypt( pndid, fname, (unsigned char*)m_buf.buf, prop.sourcesize, &getsize );
	if( ret || (getsize != prop.sourcesize) ){
		_ASSERT( 0 );
		return 1;
	}

	m_buf.bufsize = prop.sourcesize;
	m_buf.pos = 0;

//////////
	
	m_writemode = 0;
	m_frombuf = 1;//!!!!!!!!


	ret = LoadQuaFile_aft( mcookieptr );
	if( ret ){
		DbgOut( "quafile : LoadQuaFileFromPnd : LoadQuaFile_aft error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CQuaFile::LoadQuaFileFromBuf( CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, char* bufptr, int bufsize, LONG* mcookieptr, float quamult )
{

	if( !srclpmh ){
		*mcookieptr = -1;
		return 1;
	}
	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;

	m_quamult = D3DXVECTOR3( quamult, quamult, quamult );


	m_buf.buf = (char*)malloc( sizeof( char ) * bufsize );
	if( !(m_buf.buf) ){
		DbgOut( "quafile : LoadQuaFileFromBuf : buf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	MoveMemory( m_buf.buf, bufptr, sizeof( char ) * bufsize );

	m_buf.bufsize = bufsize;
	m_buf.pos = 0;
	

//////////
	
	m_writemode = 0;
	m_frombuf = 1;//!!!!!!!!

	int ret;
	ret = LoadQuaFile_aft( mcookieptr );
	if( ret ){
		DbgOut( "quafile : LoadQuaFileFromBuf : LoadQuaFile_aft error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CQuaFile::LoadQuaFile_aft( LONG* mcookieptr )//！！！！！　変更するときは、ConvSymmXMotionも変更！！！！！
{

//！！！！！　変更するときは、ConvSymmXMotionも変更！！！！！

	int ret;
	QUAFILEHDR header;
	ret = ReadQuaHeader( &header );
	if( ret ){
		DbgOut( "QuaFile : LoadQuaFile : ReadQuaHeader error !!!\n" );
		_ASSERT( 0 );
		*mcookieptr = -1;
		return 1;
	}
	
	DbgOut( "QuaFile : LoadQuaFile : quaversion %d\r\n", header.magicno );


	if( (header.magicno != QUAFILEMAGICNO) && (header.magicno != QUAFILEMAGICNO2) && 
		(header.magicno != QUAFILEMAGICNO3) && (header.magicno != QUAFILEMAGICNO4) &&
		(header.magicno != QUAFILEMAGICNO5) && (header.magicno != QUAFILEMAGICNO6) &&
		(header.magicno != QUAFILEMAGICNO7) && (header.magicno != QUAFILEMAGICNO8) && 
		(header.magicno != QUAFILEMAGICNO9) && (header.magicno != QUAFILEMAGICNO10) && 
		(header.magicno != QUAFILEMAGICNO11) && (header.magicno != QUAFILEMAGICNO12) && (header.magicno != QUAFILEMAGICNO13) ){
		DbgOut( "QuaFile : LoadQuaFile : magicno error !!!\n" );
		_ASSERT( 0 );
		*mcookieptr = -1;
		return D3DAPPERR_MEDIANOTFOUND;
	}

	int interpolation;
	int motjump;
	int namequaflag = 0;
	int zatype = ZA_1;
	if( header.magicno >= QUAFILEMAGICNO2 ){
		QUAFILEHDR2 header2;

		ret = ReadQuaHeader2( &header2 );
		if( ret ){
			DbgOut( "QuaFile : LoadQuaFile : ReadQuaHeader2 error !!!\n" );
			_ASSERT( 0 );
			*mcookieptr = -1;
			return 1;
		}

		interpolation = header2.interpolation;

		if( (header2.flags & FLAGS_HASSCALE) != 0 ){
			m_hasscale = 1;
		}else{
			m_hasscale = 0;
		}

		motjump = header2.motjump;

		if( (header2.flags & FLAGS_NAMEFILE) != 0 ){
			namequaflag = 1;
		}else{
			namequaflag = 0;
		}

		zatype = header2.zatype;
	}else{
		interpolation = INTERPOLATION_SQUAD;

		m_hasscale = 0;

		motjump = 0;

		namequaflag = 0;
		zatype = ZA_1;
	}
DbgOut( "quafile : LoadQuaFile : framenum %d\n", header.framenum );

	int framemax;
	framemax = header.framenum - 1;


	//addmotion
//DbgOut( "QuaFile : LoadQuaFile : bef AddMotion\n" );

	m_motcookie = lpmh->AddMotion( zatype, header.motname, header.mottype, header.framenum, interpolation, motjump );
	if( m_motcookie < 0 ){
		DbgOut( "QuaFile : LoadQuaFile : mhandler->AddMotion error !!!\n" );
		_ASSERT( 0 );
		*mcookieptr = -1;
		return 1;
	}
	*mcookieptr = m_motcookie;


DbgOut( "quafile : checkzatype : motid %d : interpolation %d, zatype %d\r\n", m_motcookie, interpolation, zatype );

	if( namequaflag == 0 ){
		
	//DbgOut( "QuaFile : LoadQuaFile : AddMotion : %s, cookie %d, framenum %d\n",
	//	   header.motname, m_motcookie, header.framenum );

		int* boneno2serino;
		boneno2serino = (int*)malloc( sizeof( int ) * lpmh->s2mot_leng );
		if( !boneno2serino ){
			DbgOut( "QuaFile : LoadQuaFile : boneno2serino alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int elemno;
		for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
			*(boneno2serino + elemno) = -1;
		}

		for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
			CMotionCtrl* mc = (*lpmh)( elemno );
			int bno;// = mc->boneno;
			
			if( header.magicno >= QUAFILEMAGICNO4 ){
				bno = mc->boneno;
			}else{
				bno = mc->oldboneno;
			}

			if( bno >= 0 ){
				if( bno < lpmh->s2mot_leng ){
					*(boneno2serino + bno) = mc->serialno;
				}else{
					DbgOut( "QuaFile : LoadQuaFile : boneno2serino : bno out of range !!!\n" );
					_ASSERT( 0 );
				}
			}
		}

		//read motionpoint
		int bonenomax;
		bonenomax = lpmh->s2mot_leng - 1;


		if( header.magicno >= QUAFILEMAGICNO10 ){
			ret = LoadMorphIndices( QUATYPE_NUM );
			if( ret ){
				DbgOut( "quafile : LoadQuaFile : LoadMorphIndices error !!!\n" );
				_ASSERT( 0 );
				free( boneno2serino );
				return 1;
			}
		}


		int boneno = 0;
		int serialno;
		while( boneno >= 0 ){
			if( header.magicno <= QUAFILEMAGICNO2 ){

				QUADATA data;
			
				ret = ReadQuaData( &data );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : ReadQuaData error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}

				data.mp.m_mvx *= m_quamult.x;
				data.mp.m_mvy *= m_quamult.y;
				data.mp.m_mvz *= m_quamult.z;

				boneno = data.boneno;
				if( (boneno > bonenomax) || (boneno < 0) ){
					DbgOut( "QuaFile : LoadQuaFile : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}


				//serialno = *(boneno2serino + boneno);
				int tempseri;
				tempseri = *(boneno2serino + boneno);
				if( header.magicno >= QUAFILEMAGICNO4 ){
					serialno = tempseri;

					if( serialno >= 0 ){
						if( serialno >= lpmh->s2mot_leng ){
							DbgOut( "QuaFile : LoadQuaFile : serialno out of range error !!! %d, %d, %d\n",
								serialno, boneno, lpmh->s2mot_leng );
						}
						int frameno = (data.mp).m_frameno;
						if( (frameno >= 0) && (frameno <= framemax) ){
							CMotionCtrl* mcptr = (*lpmh)( serialno );
							CMotionPoint2* retmp;
							retmp = mcptr->AddMotionPoint( m_motcookie, &(data.mp), interpolation );
							if( !retmp ){
								DbgOut( "QuaFile : LoadQuaFile : AddMotionPoint error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
						}
					}


				}else{
					serialno = -1;//!!!!!!!!
					if( tempseri >= 0 ){
						CMotionCtrl* tempmc;
						tempmc = (*lpmh)( tempseri );
						if( tempmc ){
							CMotionCtrl* chilmc;
							chilmc = tempmc->child;
							if( chilmc ){
								serialno = chilmc->serialno;
							}else{
								serialno = tempseri;
							}
						}
					}

					//すべての子供に、モーションセット。
					while( serialno >= 0 ){
						if( serialno >= lpmh->s2mot_leng ){
							DbgOut( "QuaFile : LoadQuaFile : serialno out of range error !!! %d, %d, %d\n",
								serialno, boneno, lpmh->s2mot_leng );
						}
						int frameno = (data.mp).m_frameno;
						if( (frameno >= 0) && (frameno <= framemax) ){
							CMotionCtrl* mcptr = (*lpmh)( serialno );
							CMotionPoint2* retmp;
							if( mcptr->IsJoint() ){
								retmp = mcptr->AddMotionPoint( m_motcookie, &(data.mp), interpolation );
								if( !retmp ){
									DbgOut( "QuaFile : LoadQuaFile : AddMotionPoint error !!!\n" );
									_ASSERT( 0 );
									free( boneno2serino );
									return 1;
								}
							}
						}
						CMotionCtrl* tmpmc2;
						tmpmc2 = (*lpmh)( serialno );
						CMotionCtrl* bromc;
						bromc = tmpmc2->brother;
						if( bromc ){
							serialno = bromc->serialno;
						}else{
							serialno = -1;
						}
					}


				}

			}else{
			
				QUADATA2 data2;

				ret = ReadQuaData2( &data2 );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : ReadQuaData2 error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}

				data2.mp.m_mvx *= m_quamult.x;
				data2.mp.m_mvy *= m_quamult.y;
				data2.mp.m_mvz *= m_quamult.z;
				data2.mp.m_spp = 0;


				QUASPPARAM qspp;
				ZeroMemory( &qspp, sizeof( QUASPPARAM ) );
				if( header.magicno >= QUAFILEMAGICNO12 ){
					ret = ReadUCharaData( (unsigned char*)(&qspp), sizeof( QUASPPARAM ) );
					if( ret ){
						DbgOut( "QuaFile : LoadQuaFile : ReadUCharaData qspp error !!!\n" );
						_ASSERT( 0 );
						free( boneno2serino );
						return 1;
					}
				}

				boneno = data2.boneno;
				if( (boneno > bonenomax) || (boneno < 0) ){
					DbgOut( "QuaFile : LoadQuaFile : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}

				//serialno = *(boneno2serino + boneno);
				int tempseri;
				tempseri = *(boneno2serino + boneno);
				if( header.magicno >= QUAFILEMAGICNO4 ){
					serialno = tempseri;

					if( serialno >= 0 ){
						if( serialno >= lpmh->s2mot_leng ){
							DbgOut( "QuaFile : LoadQuaFile : serialno out of range error !!! %d, %d, %d\n",
								serialno, boneno, lpmh->s2mot_leng );
						}
						int frameno = (data2.mp).m_frameno;
						if( (frameno >= 0) && (frameno <= framemax) ){
							CMotionCtrl* mcptr = (*lpmh)( serialno );
							CMotionPoint2* retmp;

							if( m_hasscale == 0 ){
								data2.mp.m_scalex = 1.0f;
								data2.mp.m_scaley = 1.0f;
								data2.mp.m_scalez = 1.0f;
							}

							//！！！　 rokdebone2が対応するまで、mp->interpを使わず、interpolationを使う。

//if( m_motcookie == 7 ){
//	DbgOut( "quafile : checkinterp !!! : m_motcookie %d, mp.interp %d\r\n", m_motcookie, data2.mp.interp );
//}
							retmp = mcptr->AddMotionPoint( m_motcookie, &(data2.mp), data2.mp.interp );
							if( !retmp ){
								DbgOut( "QuaFile : LoadQuaFile : AddMotionPoint error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}


							if( header.magicno >= QUAFILEMAGICNO12 ){
								_ASSERT( retmp->m_spp );
								retmp->m_spp->m_rotparam = qspp.m_rotparam;
								retmp->m_spp->m_mvparam = qspp.m_mvparam;
								retmp->m_spp->m_scparam = qspp.m_scparam;

								//retmp->m_spp->ScaleMvDist( m_quamult );

								if( header.magicno < QUAFILEMAGICNO13 ){
									int ikind;
									for( ikind = INDX; ikind <= INDZ; ikind++ ){
										retmp->m_spp->m_rotparam.ctrlS[ikind].tmin = 0.0f;
										retmp->m_spp->m_rotparam.ctrlS[ikind].tmax = 1.0f;
										retmp->m_spp->m_rotparam.ctrlE[ikind].tmin = 0.0f;
										retmp->m_spp->m_rotparam.ctrlE[ikind].tmax = 1.0f;

										retmp->m_spp->m_mvparam.ctrlS[ikind].tmin = 0.0f;
										retmp->m_spp->m_mvparam.ctrlS[ikind].tmax = 1.0f;
										retmp->m_spp->m_mvparam.ctrlE[ikind].tmin = 0.0f;
										retmp->m_spp->m_mvparam.ctrlE[ikind].tmax = 1.0f;

										retmp->m_spp->m_scparam.ctrlS[ikind].tmin = 0.0f;
										retmp->m_spp->m_scparam.ctrlS[ikind].tmax = 1.0f;
										retmp->m_spp->m_scparam.ctrlE[ikind].tmin = 0.0f;
										retmp->m_spp->m_scparam.ctrlE[ikind].tmax = 1.0f;
									}
								}
							}

	//DbgOut( "check !!!, quafile, serialno %d, frameno %d, eul %f %f %f\r\n", serialno, frameno,
	//	   data2.mp.m_eul.x, data2.mp.m_eul.y, data2.mp.m_eul.z );


						}
					}

				}else{
					// magicno 3 ???

					serialno = -1;//!!!!!!!!
					if( tempseri >= 0 ){
						CMotionCtrl* tempmc;
						tempmc = (*lpmh)( tempseri );
						if( tempmc ){
							CMotionCtrl* chilmc;
							chilmc = tempmc->child;
							if( chilmc ){
								serialno = chilmc->serialno;
							}else{
								serialno = tempseri;
							}
						}
					}

					//すべての子供にモーションセット
					while( serialno >= 0 ){
						if( serialno >= lpmh->s2mot_leng ){
							DbgOut( "QuaFile : LoadQuaFile : serialno out of range error !!! %d, %d, %d\n",
								serialno, boneno, lpmh->s2mot_leng );
						}
						int frameno = (data2.mp).m_frameno;
						if( (frameno >= 0) && (frameno <= framemax) ){
							CMotionCtrl* mcptr = (*lpmh)( serialno );
							CMotionPoint2* retmp;
							if( mcptr->IsJoint() ){
								if( m_hasscale == 0 ){
									data2.mp.m_scalex = 1.0f;
									data2.mp.m_scaley = 1.0f;
									data2.mp.m_scalez = 1.0f;
								}

								//！！！　 rokdebone2が対応するまで、mp->interpを使わず、interpolationを使う。
//DbgOut( "quafile : magic 3 : checkinterp !!! : m_motcookie %d, mp.interp %d\r\n", m_motcookie, data2.mp.interp );

								retmp = mcptr->AddMotionPoint( m_motcookie, &(data2.mp), data2.mp.interp );
								if( !retmp ){
									DbgOut( "QuaFile : LoadQuaFile : AddMotionPoint error !!!\n" );
									_ASSERT( 0 );
									free( boneno2serino );
									return 1;
								}
							}
						}

						CMotionCtrl* tmpmc2;
						tmpmc2 = (*lpmh)( serialno );
						CMotionCtrl* bromc;
						bromc = tmpmc2->brother;
						if( bromc ){
							serialno = bromc->serialno;
						}else{
							serialno = -1;
						}

					}


				}
			}

			
			if( (serialno >= 0) && (serialno < lpmh->s2mot_leng) ){
				CMotionCtrl* mcptr3 = (*lpmh)( serialno );
				ret = mcptr3->ScaleSplineMv( m_motcookie, m_quamult );
				_ASSERT( !ret );
			}

		}

	//texanim
		if( header.magicno >= QUAFILEMAGICNO6 ){
			boneno = 0;
			while( boneno >= 0 ){
				QUATEXKEY qtk;

				ret = ReadQuaTexKey( &qtk );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : ReadQuaTexKey error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}
				
				boneno = qtk.boneno;
				//if( (boneno > bonenomax) || (boneno < 0) ){
				//	DbgOut( "QuaFile : LoadQuaFile  texanim : boneno out of range : skip data (bno %d)!!!\n", boneno );
				//	continue;
				//}
				if( boneno < 0 ){
					DbgOut( "QuaFile : LoadQuaFile  texanim : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}

				//serialno = *(boneno2serino + boneno);
				if( boneno <= bonenomax ){
					serialno = *(boneno2serino + boneno);
				}else{
					serialno = 0;
				}

				//if( serialno >= 0 ){
				//	if( serialno >= lpmh->s2mot_leng ){
				//		DbgOut( "QuaFile : LoadQuaFile texanim : serialno out of range error !!! %d, %d, %d\n",
				//			serialno, boneno, lpmh->s2mot_leng );
				//	}
					int frameno = qtk.frameno;
					if( (frameno >= 0) && (frameno <= framemax) ){
						CTexKey* tkptr;
						if( (serialno > 0) && (serialno < lpmh->s2mot_leng) ){
							ret = lpmh->CreateTexKey( serialno, m_motcookie, frameno, &tkptr );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile texanim : mh CreateTexKey error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
						}
						int tcno;
						for( tcno = 0; tcno < qtk.changenum; tcno++ ){
							QUATEXCHANGE qtc;
							ret = ReadQuaTexChange( &qtc );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile texanim : ReadQuaTexChange error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
							if( (serialno > 0) && (serialno < lpmh->s2mot_leng) ){	
								int matno;
								ret = lpsh->GetMaterialNoByName( qtc.matname, &matno );
								if( ret ){
									DbgOut( "QuaFile : LoadQuaFile texanim : GetMaterialNoByName error !!!\n" );
									_ASSERT( 0 );
									free( boneno2serino );
									return 1;
								}
									
								if( matno >= 0 ){
									CMQOMaterial* matptr;
									matptr = lpsh->GetMaterialFromNo( lpsh->m_mathead, matno );
									if( matptr ){
										ret = tkptr->SetTexChange( matptr, qtc.texname );
										if( ret ){
											DbgOut( "QuaFile : LoadQuaFile texanim : tk SetTexChange error !!!\n" );
											_ASSERT( 0 );
											free( boneno2serino );
											return 1;
										}
									}
								}
							}
						}
					}
				//}
			}
		}
	//alpanim
		if( header.magicno >= QUAFILEMAGICNO11 ){
			boneno = 0;
			while( boneno >= 0 ){
				QUAALPKEY alpk;

				ret = ReadQuaAlpKey( &alpk );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : ReadQuaAlpKey error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}
				
				boneno = alpk.boneno;
				//if( (boneno > bonenomax) || (boneno < 0) ){
				//	DbgOut( "QuaFile : LoadQuaFile  alpanim : boneno out of range : skip data (bno %d)!!!\n", boneno );
				//	continue;
				//}
				if( boneno < 0 ){
					DbgOut( "QuaFile : LoadQuaFile  alpanim : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}

				//serialno = *(boneno2serino + boneno);
				if( boneno <= bonenomax ){
					serialno = *(boneno2serino + boneno);
				}else{
					serialno = 0;
				}

				//if( serialno >= 0 ){
				//	if( serialno >= lpmh->s2mot_leng ){
				//		DbgOut( "QuaFile : LoadQuaFile alpanim : serialno out of range error !!! %d, %d, %d\n",
				//			serialno, boneno, lpmh->s2mot_leng );
				//	}
					int frameno = alpk.frameno;
					if( (frameno >= 0) && (frameno <= framemax) ){
						CAlpKey* alpkptr;
						if( (serialno > 0) && (serialno < lpmh->s2mot_leng) ){
							ret = lpmh->CreateAlpKey( serialno, m_motcookie, frameno, &alpkptr );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile alpanim : mh CreateAlpKey error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
						}
						int tcno;
						for( tcno = 0; tcno < alpk.changenum; tcno++ ){
							QUAALPCHANGE qalpc;
							ret = ReadQuaAlpChange( &qalpc );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile alpanim : ReadQuaAlpChange error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
							if( (serialno > 0) && (serialno < lpmh->s2mot_leng) ){	
								int matno;
								ret = lpsh->GetMaterialNoByName( qalpc.matname, &matno );
								if( ret ){
									DbgOut( "QuaFile : LoadQuaFile alpanim : GetMaterialNoByName error !!!\n" );
									_ASSERT( 0 );
									free( boneno2serino );
									return 1;
								}
									
								if( matno >= 0 ){
									CMQOMaterial* matptr;
									matptr = lpsh->GetMaterialFromNo( lpsh->m_mathead, matno );
									if( matptr ){
										ret = alpkptr->SetAlpChange( matptr, qalpc.alpha );
										if( ret ){
											DbgOut( "QuaFile : LoadQuaFile alpanim : tk SetAlpChange error !!!\n" );
											_ASSERT( 0 );
											free( boneno2serino );
											return 1;
										}
									}
								}
							}
						}
					}
				//}
			}
		}


	//dispswtich
		if( header.magicno >= QUAFILEMAGICNO7 ){
			boneno = 0;
			while( boneno >= 0 ){
				QUADSKEY qdsk;

				ret = ReadQuaDSKey( &qdsk );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : ReadQuaDSKey error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}
				
				boneno = qdsk.boneno;
				//if( (boneno > bonenomax) || (boneno < 0) ){
				if( boneno < 0 ){
					DbgOut( "QuaFile : LoadQuaFile  DS : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}

				//serialno = *(boneno2serino + boneno);
				if( boneno <= bonenomax ){
					serialno = *(boneno2serino + boneno);
				}else{
					serialno = 0;
				}

				//if( serialno >= 0 ){
					//if( serialno >= lpmh->s2mot_leng ){
					//	DbgOut( "QuaFile : LoadQuaFile DS : serialno out of range error !!! %d, %d, %d\n",
					//		serialno, boneno, lpmh->s2mot_leng );
					//}
					int frameno = qdsk.frameno;
					if( (frameno >= 0) && (frameno <= framemax) ){
						CDSKey* dskptr;
						if( (serialno > 0) && (serialno < lpmh->s2mot_leng) ){
							ret = lpmh->CreateDSKey( serialno, m_motcookie, frameno, &dskptr );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile DS : mh CreateDSKey error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
						}

						int dscno;
						for( dscno = 0; dscno < qdsk.changenum; dscno++ ){
							QUADSCHANGE qdsc;
							ret = ReadQuaDSChange( &qdsc );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile DS : ReadQuaDSChange error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
							
							if( (serialno > 0) && (serialno < lpmh->s2mot_leng) ){
								if( (qdsc.switchno >= 0) && (qdsc.switchno < DISPSWITCHNUM) ){
									CDispSwitch* dsptr;
									dsptr = lpsh->m_ds + qdsc.switchno;
									ret = dskptr->SetDSChange( dsptr, qdsc.change );
									if( ret ){
										DbgOut( "quafile : LoadQuaFile : DS : dsk SetDSChange error !!!\n" );
										_ASSERT( 0 );
										free( boneno2serino );
										return 1;
									}
								}
							}
						}
					}
				//}
			}
		}

		//morphanim
		if( header.magicno >= QUAFILEMAGICNO10 ){
			boneno = 0;
			while( boneno >= 0 ){
				int curboneno;

				ret = ReadUCharaData( (unsigned char*)&curboneno, sizeof( int ) );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : Read curboneno error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}
				
				boneno = curboneno;
				//if( (boneno > bonenomax) || (boneno < 0) ){
				if( boneno < 0 ){
					DbgOut( "QuaFile : LoadQuaFile  morph : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}

				if( boneno <= bonenomax ){
					serialno = *(boneno2serino + boneno);
				}else{
					serialno = 0;
				}

				CMotionCtrl* mcptr2 = 0;
				if( serialno > 0 ){
					if( serialno >= lpmh->s2mot_leng ){
						DbgOut( "QuaFile : LoadQuaFile morph : serialno out of range error !!! %d, %d, %d\n",
							serialno, boneno, lpmh->s2mot_leng );
						_ASSERT( 0 );
						return 1;
					}
					mcptr2 = (*lpmh)( serialno );
				}

				ret = LoadMorphVals( mcptr2, m_motcookie, 0 );
				if( ret ){
					DbgOut( "quafile : LoadQuaFile morph : LoadMorphVals error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

		free( boneno2serino );

	}else{
		int frameoffset = 0;
		ret = LoadNameQua( header.magicno, m_motcookie, framemax, interpolation, frameoffset );
		if( ret ){
			DbgOut( "quafile : LoadQuaFile_aft : LoadNameQua error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}




	//if( header.magicno < QUAFILEMAGICNO5 ){
	//	ret = lpmh->CalcMPEuler( lpsh, m_motcookie );
	//	if( ret ){
	//		DbgOut( "QuaFile : LoadQuaFile : lpmh CalcMPEuler error !!!\n" );
	//		_ASSERT( 0 );
	//		return 1;
	//	}
	//}


	return 0;
}

int CQuaFile::CheckNameFileMark()
{
	int ret;
	char cmpname[256];
	ZeroMemory( cmpname, sizeof( char ) * 256 );
	strcpy_s( cmpname, 256, "名前基準" );

	char lname[256];
	ZeroMemory( lname, sizeof( char ) * 256 );
	ret = ReadUCharaData( (unsigned char*)lname, 9 );
	if( ret ){
		DbgOut( "quafile : LoadQuaFile_aft : Read namebasefile header error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int cmp;
	cmp = strcmp( cmpname, lname );
	if( cmp != 0 ){
		DbgOut( "quafile : LoadQuaFile_aft : namebasefile header error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CQuaFile::LoadMorphIndices( int quatype )
{
	if( m_morphindex ){
		free( m_morphindex );
		m_morphindex = 0;
	}
	m_morphindexnum = 0;


	int minum = 0;
	int ret;

	ret = ReadUCharaData( (unsigned char*)&minum, sizeof( int ) );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	m_morphindexnum = minum;

	if( minum > 0 ){
		m_morphindex = (MORPHINDEX*)malloc( sizeof( MORPHINDEX ) * minum );
		if( !m_morphindex ){
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( m_morphindex, sizeof( MORPHINDEX ) * minum );

		ret = ReadMorphIndex( m_morphindex, minum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		if( quatype == QUATYPE_NAME ){
			// ！！！！！　serial のふり直し。
			int mino;
			for( mino = 0; mino < minum; mino++ ){
				MORPHINDEX* curmi;
				curmi = m_morphindex + mino;

				ret = lpth->GetPartNoByName( curmi->name, &(curmi->seri) );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}


	return 0;
}

int CQuaFile::LoadNameQua( int magicno, int motid, int framemax, int interpolation, int frameoffset )
{
	//quadata3 保存なし版

	int ret;

	ret = CheckNameFileMark();
	if( ret ){
		DbgOut( "QuaFile : LoadNameQua : CheckNameFileMark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( magicno >= QUAFILEMAGICNO10 ){
		ret = LoadMorphIndices( QUATYPE_NAME );
		if( ret ){
			DbgOut( "quafile : LoadNameQua : LoadMorphIndices error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int arrayleng = 0;
	ret = ReadUCharaData( (unsigned char*)&arrayleng, sizeof( int ) );
	if( ret ){
		DbgOut( "quafile : LoadNameQua : read arrayleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	QUANAMEHDR curhdr;
	ZeroMemory( &curhdr, sizeof( QUANAMEHDR ) );
	ret = ReadQuaNameHdr( &curhdr );
	if( ret ){
		DbgOut( "quafile : LoadNameQua : first header read error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	while( curhdr.seri >= 0 ){
	
		int newseri = -1;
		ret = lpth->GetBoneNoByName( curhdr.name, &newseri, lpsh, 0 );
		if( ret ){
			DbgOut( "quafile : LoadNameQua : GetBoneNoByName error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}	

		CMotionCtrl* mcptr = 0;
		if( newseri >= 0 ){
			mcptr = (*lpmh)( newseri );
		}

		if( curhdr.seri < arrayleng ){
			int motkeyno;
			for( motkeyno = 0; motkeyno < curhdr.framenum; motkeyno++ ){
				QUADATA3 curdata3;
				ZeroMemory( &curdata3, sizeof( QUADATA3 ) );
				ret = ReadQuaData3( &curdata3 );
				if( ret ){
					DbgOut( "quafile : LoadNameQua : ReadQuaData3 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				
				QUASPPARAM qspp;
				ZeroMemory( &qspp, sizeof( QUASPPARAM ) );
				if( magicno >= QUAFILEMAGICNO12 ){
					ret = ReadUCharaData( (unsigned char*)(&qspp), sizeof( QUASPPARAM ) );
					if( ret ){
						DbgOut( "quafile : LoadNameQua : ReadUC qspp error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}

				curdata3.m_mv.x *= m_quamult.x;
				curdata3.m_mv.y *= m_quamult.y;
				curdata3.m_mv.z *= m_quamult.z;

				if( newseri >= 0 ){
					if( newseri >= lpmh->s2mot_leng ){
						DbgOut( "QuaFile : LoadNameQua : serialno out of range error !!! %d, %d\n",
							newseri, lpmh->s2mot_leng );
					}

					int frameno = curdata3.m_frameno + frameoffset;
					if( (frameno >= 0) && (frameno <= framemax) ){
						CMotionPoint2* retmp;
						CMotionPoint2 setmp;

						setmp.ishead = curdata3.ishead;
						setmp.m_frameno = frameno;
						setmp.m_rotx = curdata3.m_rot.x;
						setmp.m_roty = curdata3.m_rot.y;
						setmp.m_rotz = curdata3.m_rot.z;
						setmp.m_mvx = curdata3.m_mv.x;
						setmp.m_mvy = curdata3.m_mv.y;
						setmp.m_mvz = curdata3.m_mv.z;
						setmp.m_q = curdata3.m_q;
						setmp.serialno = curdata3.serialno;
						setmp.interp = curdata3.interp;
						setmp.m_scalex = curdata3.m_scale.x;
						setmp.m_scaley = curdata3.m_scale.y;
						setmp.m_scalez = curdata3.m_scale.z;
						setmp.m_userint1 = curdata3.m_userint1;
						setmp.m_eul = curdata3.m_eul;

						//！！！　 rokdebone2が対応するまで、mp->interpを使わず、interpolationを使う。
//DbgOut( "quafile : checkinterp !!! : m_motcookie %d, setmp.interp %d\r\n", m_motcookie, setmp.interp );

						retmp = mcptr->AddMotionPoint( m_motcookie, &setmp, setmp.interp );
//						retmp = mcptr->AddMotionPoint( m_motcookie, &setmp, INTERPOLATION_SQUAD );
						if( !retmp ){
							DbgOut( "QuaFile : LoadNameQua : AddMotionPoint error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}

						if( magicno >= QUAFILEMAGICNO12 ){
							_ASSERT( retmp->m_spp );
							retmp->m_spp->m_rotparam = qspp.m_rotparam;
							retmp->m_spp->m_mvparam = qspp.m_mvparam;
							retmp->m_spp->m_scparam = qspp.m_scparam;

							//retmp->m_spp->ScaleMvDist( m_quamult );

							if( magicno < QUAFILEMAGICNO13 ){
								int ikind;
								for( ikind = INDX; ikind <= INDZ; ikind++ ){
									retmp->m_spp->m_rotparam.ctrlS[ikind].tmin = 0.0f;
									retmp->m_spp->m_rotparam.ctrlS[ikind].tmax = 1.0f;
									retmp->m_spp->m_rotparam.ctrlE[ikind].tmin = 0.0f;
									retmp->m_spp->m_rotparam.ctrlE[ikind].tmax = 1.0f;

									retmp->m_spp->m_mvparam.ctrlS[ikind].tmin = 0.0f;
									retmp->m_spp->m_mvparam.ctrlS[ikind].tmax = 1.0f;
									retmp->m_spp->m_mvparam.ctrlE[ikind].tmin = 0.0f;
									retmp->m_spp->m_mvparam.ctrlE[ikind].tmax = 1.0f;

									retmp->m_spp->m_scparam.ctrlS[ikind].tmin = 0.0f;
									retmp->m_spp->m_scparam.ctrlS[ikind].tmax = 1.0f;
									retmp->m_spp->m_scparam.ctrlE[ikind].tmin = 0.0f;
									retmp->m_spp->m_scparam.ctrlE[ikind].tmax = 1.0f;
								}
							}

						}

					}

				}

			}

			if( mcptr ){
				ret = mcptr->ScaleSplineMv( m_motcookie, m_quamult );
				_ASSERT( !ret );
			}

		}

		int texkeyno;
		for( texkeyno = 0; texkeyno < curhdr.texframenum; texkeyno++ ){
			QUATEXKEY qtk;

			ret = ReadQuaTexKey( &qtk );
			if( ret ){
				DbgOut( "QuaFile : LoadNameQua : ReadQuaTexKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			int frameno = qtk.frameno + frameoffset;
			if( (frameno >= 0) && (frameno <= framemax) ){
				CTexKey* tkptr = 0;
				if( newseri >= 0 ){
					ret = lpmh->CreateTexKey( newseri, m_motcookie, frameno, &tkptr );
					if( ret ){
						DbgOut( "QuaFile : LoadNameQua texanim : mh CreateTexKey error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}

				int tcno;
				for( tcno = 0; tcno < qtk.changenum; tcno++ ){
					QUATEXCHANGE qtc;
					ret = ReadQuaTexChange( &qtc );
					if( ret ){
						DbgOut( "QuaFile : LoadNameQua texanim : ReadQuaTexChange error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					if( newseri >= 0 ){
						int matno;
						ret = lpsh->GetMaterialNoByName( qtc.matname, &matno );
						if( ret ){
							DbgOut( "QuaFile : LoadNameQua texanim : GetMaterialNoByName error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
							
						if( matno >= 0 ){
							CMQOMaterial* matptr;
							matptr = lpsh->GetMaterialFromNo( lpsh->m_mathead, matno );
							if( matptr ){
								ret = tkptr->SetTexChange( matptr, qtc.texname );
								if( ret ){
									DbgOut( "QuaFile : LoadNameQua texanim : tk SetTexChange error !!!\n" );
									_ASSERT( 0 );
									return 1;
								}
							}
						}
					}
				}
			}
		}

		int alpkeyno;
		for( alpkeyno = 0; alpkeyno < curhdr.alpframenum; alpkeyno++ ){
			QUAALPKEY alpk;

			ret = ReadQuaAlpKey( &alpk );
			if( ret ){
				DbgOut( "QuaFile : LoadNameQua : ReadQuaAlpKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			int frameno = alpk.frameno + frameoffset;
			if( (frameno >= 0) && (frameno <= framemax) ){
				CAlpKey* alpkptr = 0;
				if( newseri >= 0 ){
					ret = lpmh->CreateAlpKey( newseri, m_motcookie, frameno, &alpkptr );
					if( ret ){
						DbgOut( "QuaFile : LoadNameQua alpanim : mh CreateAlpKey error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}

				int alpcno;
				for( alpcno = 0; alpcno < alpk.changenum; alpcno++ ){
					QUAALPCHANGE qalpc;
					ret = ReadQuaAlpChange( &qalpc );
					if( ret ){
						DbgOut( "QuaFile : LoadNameQua alpanim : ReadQuaAlpChange error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					if( newseri >= 0 ){
						int matno;
						ret = lpsh->GetMaterialNoByName( qalpc.matname, &matno );
						if( ret ){
							DbgOut( "QuaFile : LoadNameQua alpanim : GetMaterialNoByName error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
							
						if( matno >= 0 ){
							CMQOMaterial* matptr;
							matptr = lpsh->GetMaterialFromNo( lpsh->m_mathead, matno );
							if( matptr ){
								ret = alpkptr->SetAlpChange( matptr, qalpc.alpha );
								if( ret ){
									DbgOut( "QuaFile : LoadNameQua alpanim : tk SetAlpChange error !!!\n" );
									_ASSERT( 0 );
									return 1;
								}
							}
						}
					}
				}
			}
		}



		int dskeyno;
		for( dskeyno = 0; dskeyno < curhdr.dsframenum; dskeyno++ ){
			QUADSKEY qdsk;

			ret = ReadQuaDSKey( &qdsk );
			if( ret ){
				DbgOut( "QuaFile : LoadNameQua : ReadQuaDSKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
				
			int frameno = qdsk.frameno + frameoffset;
			if( (frameno >= 0) && (frameno <= framemax) ){
				CDSKey* dskptr = 0;
				if( newseri >= 0 ){
					ret = lpmh->CreateDSKey( newseri, m_motcookie, frameno, &dskptr );
					if( ret ){
						DbgOut( "QuaFile : LoadNameQua DS : mh CreateDSKey error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				int dscno;
				for( dscno = 0; dscno < qdsk.changenum; dscno++ ){
					QUADSCHANGE qdsc;
					ret = ReadQuaDSChange( &qdsc );
					if( ret ){
						DbgOut( "QuaFile : LoadNameQua DS : ReadQuaDSChange error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					
					if( newseri >= 0 ){
						if( (qdsc.switchno >= 0) && (qdsc.switchno < DISPSWITCHNUM) ){
							CDispSwitch* dsptr;
							dsptr = lpsh->m_ds + qdsc.switchno;
							ret = dskptr->SetDSChange( dsptr, qdsc.change );
							if( ret ){
								DbgOut( "quafile : LoadNameQua : DS : dsk SetDSChange error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}
					}
				}
			}

		}

		/// morph anim
		if( magicno >= QUAFILEMAGICNO10 ){
			ret = LoadMorphVals( mcptr, motid, frameoffset );
			if( ret ){
				DbgOut( "quafile : LoadNameQua : LoadMorphVals error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}


		ret = ReadQuaNameHdr( &curhdr );
		if( ret ){
			DbgOut( "quafile : LoadNameQua : namehdr read error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}



int CQuaFile::ImportQuaFile( CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, char* fname, int motid, int offset )
{
	int ret;

	if( !srclpmh ){
		return 1;
	}
	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;


	char pathname[2048];
	int ch = '\\';
	char* enptr = 0;
	enptr = strrchr( fname, ch );
	if( enptr ){
		strcpy_s( pathname, 2048, fname );
	}else{
		ZeroMemory( pathname, 2048 );
		int resdirleng;
		resdirleng = GetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPTSTR)pathname, 2048 );
		_ASSERT( resdirleng );
		strcat_s( pathname, 2048, fname );
	}

	hfile = CreateFile( (LPCTSTR)pathname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CQuaFile : LoadQuaFile : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return D3DAPPERR_MEDIANOTFOUND;
	}	


	m_writemode = 0;
	m_frombuf = 0;

	m_motcookie = motid;//!!!!!!!!!!!

/////////
	QUAFILEHDR header;
	ret = ReadQuaHeader( &header );
	if( ret ){
		DbgOut( "QuaFile : LoadQuaFile : ReadQuaHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( (header.magicno != QUAFILEMAGICNO) && (header.magicno != QUAFILEMAGICNO2) && 
		(header.magicno != QUAFILEMAGICNO3) && (header.magicno != QUAFILEMAGICNO4) &&
		(header.magicno != QUAFILEMAGICNO5) && (header.magicno != QUAFILEMAGICNO6) &&
		(header.magicno != QUAFILEMAGICNO7) && (header.magicno != QUAFILEMAGICNO8) && 
		(header.magicno != QUAFILEMAGICNO9) && (header.magicno != QUAFILEMAGICNO10) &&
		(header.magicno != QUAFILEMAGICNO11) && (header.magicno != QUAFILEMAGICNO12) && (header.magicno != QUAFILEMAGICNO13) ){
		_ASSERT( 0 );
		DbgOut( "QuaFile : LoadQuaFile : magicno error !!!\n" );
		return D3DAPPERR_MEDIANOTFOUND;
	}

	int interpolation;
	int motjump;
	int namequa;

	if( header.magicno >= QUAFILEMAGICNO2 ){
		QUAFILEHDR2 header2;

		ret = ReadQuaHeader2( &header2 );
		if( ret ){
			DbgOut( "QuaFile : LoadQuaFile : ReadQuaHeader2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//interpolation = header2.interpolation;

		if( (header2.flags & FLAGS_HASSCALE) != 0 ){
			m_hasscale = 1;
		}else{
			m_hasscale = 0;
		}

		if( (header2.flags & FLAGS_NAMEFILE) != 0 ){
			namequa = 1;
		}else{
			namequa = 0;
		}


		//motjump = header2.motjump;


	}else{
		//interpolation = INTERPOLATION_SQUAD;

		m_hasscale = 0;

		//motjump = 0;
		namequa = 0;
	}


	CMotionCtrl* firstmcptr = (*lpmh)( 1 );
	if( !firstmcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* firstmiptr = firstmcptr->motinfo;
	if( !firstmiptr ){
		_ASSERT( 0 );
		return 1;
	}

	//interpolation = *(firstmiptr->interpolation + m_motcookie);
	lpmh->GetDefInterp( m_motcookie, &interpolation );
	motjump = *(firstmiptr->motjump + m_motcookie);


	int newframenum;
	newframenum = offset + header.framenum;
	int framemax;
	framemax = newframenum - 1;


DbgOut( "quafile : ImportQuaFile : framenum %d\n", header.framenum );

	//addmotion
//DbgOut( "QuaFile : LoadQuaFile : bef AddMotion\n" );

	ret = lpmh->ChangeTotalFrame( m_motcookie, newframenum, 1 );
	if( ret ){
		DbgOut( "quafile : ImportQuaFile : ChangeTotalFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
//DbgOut( "QuaFile : LoadQuaFile : AddMotion : %s, cookie %d, framenum %d\n",
//	   header.motname, m_motcookie, header.framenum );

	if( namequa == 0 ){


		int* boneno2serino;
		boneno2serino = (int*)malloc( sizeof( int ) * lpmh->s2mot_leng );
		if( !boneno2serino ){
			DbgOut( "QuaFile : LoadQuaFile : boneno2serino alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int elemno;
		for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
			*(boneno2serino + elemno) = -1;
		}

		for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
			CMotionCtrl* mc = (*lpmh)( elemno );
			int bno;// = mc->boneno;
			if( header.magicno >= QUAFILEMAGICNO4 ){
				bno = mc->boneno;
			}else{
				bno = mc->oldboneno;
			}

			if( bno >= 0 ){
				if( bno < lpmh->s2mot_leng ){
					*(boneno2serino + bno) = mc->serialno;
				}else{
					DbgOut( "QuaFile : LoadQuaFile : boneno2serino : bno out of range !!!\n" );
					_ASSERT( 0 );
				}
			}
		}

		//read motionpoint
		int bonenomax;
		bonenomax = lpmh->s2mot_leng - 1;

		if( header.magicno >= QUAFILEMAGICNO10 ){
			ret = LoadMorphIndices( QUATYPE_NUM );
			if( ret ){
				DbgOut( "quafile : LoadQuaFile : LoadMorphIndices error !!!\n" );
				_ASSERT( 0 );
				free( boneno2serino );
				return 1;
			}
		}


		int boneno = 0;
		int serialno;
		while( boneno >= 0 ){
			if( header.magicno <= QUAFILEMAGICNO2 ){

				QUADATA data;
			
				ret = ReadQuaData( &data );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : ReadQuaData error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}

				boneno = data.boneno;
				if( (boneno > bonenomax) || (boneno < 0) ){
					DbgOut( "QuaFile : LoadQuaFile : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}
				//serialno = *(boneno2serino + boneno);
				int tempseri;
				tempseri = *(boneno2serino + boneno);
				if( header.magicno >= QUAFILEMAGICNO4 ){
					serialno = tempseri;
				}else{

					serialno = -1;//!!!!!!!!
					if( tempseri >= 0 ){
						CMotionCtrl* tempmc;
						tempmc = (*lpmh)( tempseri );
						if( tempmc ){
							CMotionCtrl* chilmc;
							chilmc = tempmc->child;
							if( chilmc ){
								serialno = chilmc->serialno;
							}else{
								serialno = tempseri;
							}
						}
					}
				}


				if( serialno >= 0 ){
					if( serialno >= lpmh->s2mot_leng ){
						DbgOut( "QuaFile : LoadQuaFile : serialno out of range error !!! %d, %d, %d\n",
							serialno, boneno, lpmh->s2mot_leng );
					}
					int frameno = (data.mp).m_frameno + offset;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					data.mp.m_frameno = frameno;//!!!!!!!!
					if( (frameno >= 0) && (frameno <= framemax) ){
						CMotionCtrl* mcptr = (*lpmh)( serialno );
						CMotionPoint2* retmp;
						retmp = mcptr->AddMotionPoint( m_motcookie, &(data.mp), interpolation );
						if( !retmp ){
							DbgOut( "QuaFile : LoadQuaFile : AddMotionPoint error !!!\n" );
							_ASSERT( 0 );
							free( boneno2serino );
							return 1;
						}
					}
				}
			}else{
			
				QUADATA2 data2;

				ret = ReadQuaData2( &data2 );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : ReadQuaData2 error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}
				data2.mp.m_spp = 0;

				QUASPPARAM qspp;
				ZeroMemory( &qspp, sizeof( QUASPPARAM ) );
				if( header.magicno >= QUAFILEMAGICNO12 ){
					ret = ReadUCharaData( (unsigned char*)(&qspp), sizeof( QUASPPARAM ) );
					if( ret ){
						DbgOut( "quafile : importQuaFile : ReadUC qspp error !!!\n" );
						_ASSERT( 0 );
						free( boneno2serino );
						return 1;
					}
				}



				boneno = data2.boneno;
				if( (boneno > bonenomax) || (boneno < 0) ){
					DbgOut( "QuaFile : LoadQuaFile : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}
				//serialno = *(boneno2serino + boneno);
				int tempseri;
				tempseri = *(boneno2serino + boneno);
				if( header.magicno >= QUAFILEMAGICNO4 ){
					serialno = tempseri;
				}else{

					serialno = -1;//!!!!!!!!
					if( tempseri >= 0 ){
						CMotionCtrl* tempmc;
						tempmc = (*lpmh)( tempseri );
						if( tempmc ){
							CMotionCtrl* chilmc;
							chilmc = tempmc->child;
							if( chilmc ){
								serialno = chilmc->serialno;
							}else{
								serialno = tempseri;
							}
						}
					}
				}

				if( serialno >= 0 ){
					if( serialno >= lpmh->s2mot_leng ){
						DbgOut( "QuaFile : LoadQuaFile : serialno out of range error !!! %d, %d, %d\n",
							serialno, boneno, lpmh->s2mot_leng );
					}
					int frameno = (data2.mp).m_frameno + offset;//!!!!!!!!!!!!!!!
					data2.mp.m_frameno = frameno;//!!!!!!!!!!!!!!!!!!!!!
					if( (frameno >= 0) && (frameno <= framemax) ){
						CMotionCtrl* mcptr = (*lpmh)( serialno );
						CMotionPoint2* retmp;

	//if( data2.mp.dispswitch != 0 ){
	//	DbgOut( "quafile : LoadQuaFile : switch %x\n", data2.mp.dispswitch );
	//}
						if( m_hasscale == 0 ){
							data2.mp.m_scalex = 1.0f;
							data2.mp.m_scaley = 1.0f;
							data2.mp.m_scalez = 1.0f;
						}

						//！！！　 rokdebone2が対応するまで、mp->interpを使わず、interpolationを使う。
						retmp = mcptr->AddMotionPoint( m_motcookie, &(data2.mp), data2.mp.interp );
						if( !retmp ){
							DbgOut( "QuaFile : LoadQuaFile : AddMotionPoint error !!!\n" );
							_ASSERT( 0 );
							free( boneno2serino );
							return 1;
						}
						if( header.magicno >= QUAFILEMAGICNO12 ){
							_ASSERT( retmp->m_spp );
							retmp->m_spp->m_rotparam = qspp.m_rotparam;
							retmp->m_spp->m_mvparam = qspp.m_mvparam;
							retmp->m_spp->m_scparam = qspp.m_scparam;

							//retmp->m_spp->ScaleMvDist( m_quamult );

							if( header.magicno < QUAFILEMAGICNO13 ){
								int ikind;
								for( ikind = INDX; ikind <= INDZ; ikind++ ){
									retmp->m_spp->m_rotparam.ctrlS[ikind].tmin = 0.0f;
									retmp->m_spp->m_rotparam.ctrlS[ikind].tmax = 1.0f;
									retmp->m_spp->m_rotparam.ctrlE[ikind].tmin = 0.0f;
									retmp->m_spp->m_rotparam.ctrlE[ikind].tmax = 1.0f;

									retmp->m_spp->m_mvparam.ctrlS[ikind].tmin = 0.0f;
									retmp->m_spp->m_mvparam.ctrlS[ikind].tmax = 1.0f;
									retmp->m_spp->m_mvparam.ctrlE[ikind].tmin = 0.0f;
									retmp->m_spp->m_mvparam.ctrlE[ikind].tmax = 1.0f;

									retmp->m_spp->m_scparam.ctrlS[ikind].tmin = 0.0f;
									retmp->m_spp->m_scparam.ctrlS[ikind].tmax = 1.0f;
									retmp->m_spp->m_scparam.ctrlE[ikind].tmin = 0.0f;
									retmp->m_spp->m_scparam.ctrlE[ikind].tmax = 1.0f;
								}
							}
						}
					}
				}

			}

			if( (serialno >= 0) && (serialno < lpmh->s2mot_leng) ){
				CMotionCtrl* mcptr3;
				mcptr3 = (*lpmh)( serialno );
				ret = mcptr3->ScaleSplineMv( m_motcookie, m_quamult );
				_ASSERT( !ret );
			}


		}


	//texanim
		if( header.magicno >= QUAFILEMAGICNO6 ){
			boneno = 0;
			while( boneno >= 0 ){
				QUATEXKEY qtk;

				ret = ReadQuaTexKey( &qtk );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : ReadQuaTexKey error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}
				
				boneno = qtk.boneno;
				if( (boneno > bonenomax) || (boneno < 0) ){
					DbgOut( "QuaFile : LoadQuaFile  texanim : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}

				//serialno = *(boneno2serino + boneno);
				serialno = *(boneno2serino + boneno);

				if( serialno >= 0 ){
					if( serialno >= lpmh->s2mot_leng ){
						DbgOut( "QuaFile : LoadQuaFile texanim : serialno out of range error !!! %d, %d, %d\n",
							serialno, boneno, lpmh->s2mot_leng );
					}
					int frameno = qtk.frameno + offset;
					if( (frameno >= 0) && (frameno <= framemax) ){
						CTexKey* tkptr;
						ret = lpmh->CreateTexKey( serialno, m_motcookie, frameno, &tkptr );
						if( ret ){
							DbgOut( "QuaFile : LoadQuaFile texanim : mh CreateTexKey error !!!\n" );
							_ASSERT( 0 );
							free( boneno2serino );
							return 1;
						}

						int tcno;
						for( tcno = 0; tcno < qtk.changenum; tcno++ ){
							QUATEXCHANGE qtc;
							ret = ReadQuaTexChange( &qtc );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile texanim : ReadQuaTexChange error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
							
							int matno;
							ret = lpsh->GetMaterialNoByName( qtc.matname, &matno );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile texanim : GetMaterialNoByName error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
								
							if( matno >= 0 ){
								CMQOMaterial* matptr;
								matptr = lpsh->GetMaterialFromNo( lpsh->m_mathead, matno );
								if( matptr ){
									ret = tkptr->SetTexChange( matptr, qtc.texname );
									if( ret ){
										DbgOut( "QuaFile : LoadQuaFile texanim : tk SetTexChange error !!!\n" );
										_ASSERT( 0 );
										free( boneno2serino );
										return 1;
									}
								}
							}
						}
					}
				}
			}
		}

	//alpanim
		if( header.magicno >= QUAFILEMAGICNO6 ){
			boneno = 0;
			while( boneno >= 0 ){
				QUAALPKEY qalpk;

				ret = ReadQuaAlpKey( &qalpk );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : ReadQuaAlpKey error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}
				
				boneno = qalpk.boneno;
				if( (boneno > bonenomax) || (boneno < 0) ){
					DbgOut( "QuaFile : LoadQuaFile  alpanim : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}

				//serialno = *(boneno2serino + boneno);
				serialno = *(boneno2serino + boneno);

				if( serialno >= 0 ){
					if( serialno >= lpmh->s2mot_leng ){
						DbgOut( "QuaFile : LoadQuaFile alpanim : serialno out of range error !!! %d, %d, %d\n",
							serialno, boneno, lpmh->s2mot_leng );
					}
					int frameno = qalpk.frameno + offset;
					if( (frameno >= 0) && (frameno <= framemax) ){
						CAlpKey* alpkptr;
						ret = lpmh->CreateAlpKey( serialno, m_motcookie, frameno, &alpkptr );
						if( ret ){
							DbgOut( "QuaFile : LoadQuaFile alpanim : mh CreateAlpKey error !!!\n" );
							_ASSERT( 0 );
							free( boneno2serino );
							return 1;
						}

						int alpcno;
						for( alpcno = 0; alpcno < qalpk.changenum; alpcno++ ){
							QUAALPCHANGE qalpc;
							ret = ReadQuaAlpChange( &qalpc );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile alpanim : ReadQuaAlpChange error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
							
							int matno;
							ret = lpsh->GetMaterialNoByName( qalpc.matname, &matno );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile alpanim : GetMaterialNoByName error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
								
							if( matno >= 0 ){
								CMQOMaterial* matptr;
								matptr = lpsh->GetMaterialFromNo( lpsh->m_mathead, matno );
								if( matptr ){
									ret = alpkptr->SetAlpChange( matptr, qalpc.alpha );
									if( ret ){
										DbgOut( "QuaFile : LoadQuaFile alpanim : tk SetAlpChange error !!!\n" );
										_ASSERT( 0 );
										free( boneno2serino );
										return 1;
									}
								}
							}
						}
					}
				}
			}
		}

	//dispswtich
		if( header.magicno >= QUAFILEMAGICNO7 ){
			boneno = 0;
			while( boneno >= 0 ){
				QUADSKEY qdsk;

				ret = ReadQuaDSKey( &qdsk );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : ReadQuaDSKey error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}
				
				boneno = qdsk.boneno;
				if( (boneno > bonenomax) || (boneno < 0) ){
					DbgOut( "QuaFile : LoadQuaFile  DS : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}

				//serialno = *(boneno2serino + boneno);
				serialno = *(boneno2serino + boneno);

				if( serialno >= 0 ){
					if( serialno >= lpmh->s2mot_leng ){
						DbgOut( "QuaFile : LoadQuaFile DS : serialno out of range error !!! %d, %d, %d\n",
							serialno, boneno, lpmh->s2mot_leng );
					}
					int frameno = qdsk.frameno + offset;
					if( (frameno >= 0) && (frameno <= framemax) ){
						CDSKey* dskptr;
						ret = lpmh->CreateDSKey( serialno, m_motcookie, frameno, &dskptr );
						if( ret ){
							DbgOut( "QuaFile : LoadQuaFile DS : mh CreateDSKey error !!!\n" );
							_ASSERT( 0 );
							free( boneno2serino );
							return 1;
						}

						int dscno;
						for( dscno = 0; dscno < qdsk.changenum; dscno++ ){
							QUADSCHANGE qdsc;
							ret = ReadQuaDSChange( &qdsc );
							if( ret ){
								DbgOut( "QuaFile : LoadQuaFile DS : ReadQuaDSChange error !!!\n" );
								_ASSERT( 0 );
								free( boneno2serino );
								return 1;
							}
							
							if( (qdsc.switchno >= 0) && (qdsc.switchno < DISPSWITCHNUM) ){
								CDispSwitch* dsptr;
								dsptr = lpsh->m_ds + qdsc.switchno;
								ret = dskptr->SetDSChange( dsptr, qdsc.change );
								if( ret ){
									DbgOut( "quafile : LoadQuaFile : DS : dsk SetDSChange error !!!\n" );
									_ASSERT( 0 );
									free( boneno2serino );
									return 1;
								}
							}
						}
					}
				}
			}
		}

		//morphanim
		if( header.magicno >= QUAFILEMAGICNO10 ){
			boneno = 0;
			while( boneno >= 0 ){
				int curboneno;

				ret = ReadUCharaData( (unsigned char*)&curboneno, sizeof( int ) );
				if( ret ){
					DbgOut( "QuaFile : LoadQuaFile : Read curboneno error !!!\n" );
					_ASSERT( 0 );
					free( boneno2serino );
					return 1;
				}
				
				boneno = curboneno;
				if( (boneno > bonenomax) || (boneno < 0) ){
					DbgOut( "QuaFile : LoadQuaFile  morph : boneno out of range : skip data (bno %d)!!!\n", boneno );
					continue;
				}

				serialno = *(boneno2serino + boneno);

				CMotionCtrl* mcptr2 = 0;
				if( serialno >= 0 ){
					if( serialno >= lpmh->s2mot_leng ){
						DbgOut( "QuaFile : LoadQuaFile morph : serialno out of range error !!! %d, %d, %d\n",
							serialno, boneno, lpmh->s2mot_leng );
						_ASSERT( 0 );
						return 1;
					}
					mcptr2 = (*lpmh)( serialno );
				}

				ret = LoadMorphVals( mcptr2, m_motcookie, offset );
				if( ret ){
					DbgOut( "quafile : LoadQuaFile morph : LoadMorphVals error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}



		free( boneno2serino );
	}else{
		ret = LoadNameQua( header.magicno, m_motcookie, framemax, interpolation, offset );
		if( ret ){
			DbgOut( "ImportQuaFile : LoadNameQua error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}


	return 0;
}



int CQuaFile::ReadQuaHeader( QUAFILEHDR* hdrptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUAFILEHDR );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)hdrptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaHeader : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)hdrptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}
int CQuaFile::ReadQuaHeader2( QUAFILEHDR2* hdrptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUAFILEHDR2 );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)hdrptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaHeader2 : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)hdrptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}

int CQuaFile::ReadQuaNameHdr( QUANAMEHDR* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUANAMEHDR );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaNameHdr : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;

}

int CQuaFile::ReadMorphIndex( MORPHINDEX* dataptr, int num )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( MORPHINDEX ) * num;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadMorphIndex : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}

int CQuaFile::ReadMorphVal( MORPHVAL* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( MORPHVAL );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadMorphVal : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;


}

int CQuaFile::ReadQuaData3( QUADATA3* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUADATA3 );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaData3 : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;

}




int CQuaFile::ReadUCharaData( unsigned char* dataptr, int insize )
{
	DWORD rleng, readleng;
	
	rleng = insize;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadUCharaData : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;

}



int CQuaFile::ReadQuaData( QUADATA* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUADATA );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaData : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}


int CQuaFile::ReadQuaData2( QUADATA2* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUADATA2 );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaData2 : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}

int CQuaFile::ReadQuaTexKey( QUATEXKEY* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUATEXKEY );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaTexKey : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}
int CQuaFile::ReadQuaTexChange( QUATEXCHANGE* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUATEXCHANGE );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaTexChange : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}
int CQuaFile::ReadQuaAlpKey( QUAALPKEY* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUAALPKEY );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaAlpKey : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}
int CQuaFile::ReadQuaAlpChange( QUAALPCHANGE* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUAALPCHANGE );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaAlpChange : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}

int CQuaFile::ReadQuaDSKey( QUADSKEY* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUADSKEY );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaDSKey : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}
int CQuaFile::ReadQuaDSChange( QUADSCHANGE* dataptr )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( QUADSCHANGE );

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dataptr, rleng, &readleng, NULL );
		if( rleng != readleng ){
			DbgOut( "CQuaFile::ReadQuaDSChange : ReadFile error %d %d\n", rleng, readleng );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dataptr, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}

int CQuaFile::ConvSymmXMotion( char* fname )
{

	return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!

	m_savemode = SAVEQUA_FILE;

	char pathname[2048];
	int ch = '\\';
	char* enptr = 0;
	enptr = strrchr( fname, ch );
	if( enptr ){
		strcpy_s( pathname, 2048, fname );
	}else{
		ZeroMemory( pathname, 2048 );
		int resdirleng;
		resdirleng = GetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPTSTR)pathname, 2048 );
		_ASSERT( resdirleng );
		strcat_s( pathname, 2048, fname );
	}
	hfile = CreateFile( (LPCTSTR)pathname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CQuaFile : ConvSymmXMotion : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	char outname[2060];
	ZeroMemory( outname, sizeof( char ) * 2060 );
	int ch2 = '.';
	char* conmaptr = 0;
	conmaptr = strrchr( pathname, ch2 );
	if( conmaptr ){
		int cpleng;
		cpleng = (int)( conmaptr - pathname );
		strncpy_s( outname, 2060, pathname, cpleng );
		strcat_s( outname, 2060, "_inv.qua" );
	}else{
		sprintf_s( outname, 2060, "%s_inv.qua", pathname );
	}
	HANDLE outfile;
	outfile = CreateFile( (LPCTSTR)outname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CQuaFile::ConvSymmXMotion : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	
	
//////////////////
//////////////////

	int ret = 0;
	QUAFILEHDR header;
	ret = ReadQuaHeader( &header );
	if( ret ){
		DbgOut( "QuaFile : ConvSymmXMotion : ReadQuaHeader error !!!\n" );
		_ASSERT( 0 );
		goto convexit;
	}
	
	if( (header.magicno != QUAFILEMAGICNO) && (header.magicno != QUAFILEMAGICNO2) && 
		(header.magicno != QUAFILEMAGICNO3) && (header.magicno != QUAFILEMAGICNO4) && 
		(header.magicno != QUAFILEMAGICNO5) && (header.magicno != QUAFILEMAGICNO6) &&
		(header.magicno != QUAFILEMAGICNO7) && (header.magicno != QUAFILEMAGICNO8) && 
		(header.magicno != QUAFILEMAGICNO9) && (header.magicno != QUAFILEMAGICNO10) && 
		(header.magicno != QUAFILEMAGICNO11) && (header.magicno != QUAFILEMAGICNO12) && (header.magicno != QUAFILEMAGICNO13) ){
		DbgOut( "QuaFile : ConvSymmXMotion : magicno error !!!\n" );
		_ASSERT( 0 );
		goto convexit;
	}

	ret = WriteUCharaData( outfile, (unsigned char*)(&header), sizeof( QUAFILEHDR ) );
	_ASSERT( !ret );

	int interpolation;
	int motjump;
	int namequa;
	if( header.magicno >= QUAFILEMAGICNO2 ){
		QUAFILEHDR2 header2;

		ret = ReadQuaHeader2( &header2 );
		if( ret ){
			DbgOut( "QuaFile : ConvSymmXMotion : ReadQuaHeader2 error !!!\n" );
			_ASSERT( 0 );
			goto convexit;
		}

		interpolation = header2.interpolation;

		if( (header2.flags & FLAGS_HASSCALE) != 0 ){
			m_hasscale = 1;
		}else{
			m_hasscale = 0;
		}

		motjump = header2.motjump;

		if( (header2.flags & FLAGS_NAMEFILE) != 0 ){
			namequa = 1;
		}else{
			namequa = 0;
		}

		ret = WriteUCharaData( outfile, (unsigned char*)(&header2), sizeof( QUAFILEHDR2 ) );
		_ASSERT(!ret );

	}else{
		interpolation = INTERPOLATION_SQUAD;

		m_hasscale = 0;

		motjump = 0;

		namequa = 0;
	}

DbgOut( "quafile : ConvSymmXMotion : framenum %d\n", header.framenum );


	//read motionpoint
	int framemax;
	framemax = header.framenum - 1;

	if( namequa == 0 ){
		_ASSERT( 0 );
		return 1;
	}else{

		ret = CheckNameFileMark();
		if( ret ){
			DbgOut( "QuaFile : ConvSymmXMotion : CheckNameFileMark error !!!\n" );
			_ASSERT( 0 );
			goto convexit;
		}
		ret = WriteNameFileMark( outfile );
		if( ret ){
			DbgOut( "quafile : ConvSymmXMotion : WriteNameFileMark error !!!\n" );
			_ASSERT( 0 );
			goto convexit;
		}

		if( header.magicno >= QUAFILEMAGICNO10 ){
			int minum;
			ReadUCharaData( (unsigned char*)&minum, sizeof( int ) );
			if( ret ){
				_ASSERT( 0 );
				goto convexit;
			}
			ret = WriteUCharaData( outfile, (unsigned char*)&minum, sizeof( int ) );
			if( ret ){
				_ASSERT( 0 );
				goto convexit;
			}
			if( minum > 0 ){
				int mino;
				for( mino = 0; mino < minum; mino++ ){
					MORPHINDEX curmi;
					ReadUCharaData( (unsigned char*)&curmi, sizeof( MORPHINDEX ) );
					if( ret ){
						_ASSERT( 0 );
						goto convexit;
					}
					ret = WriteUCharaData( outfile, (unsigned char*)&curmi, sizeof( MORPHINDEX ) );
					if( ret ){
						_ASSERT( 0 );
						goto convexit;
					}
				}
			}
		}

		int arrayleng = 0;
		ret = ReadUCharaData( (unsigned char*)&arrayleng, sizeof( int ) );
		if( ret ){
			DbgOut( "quafile : ConvSymmXMotion : read arrayleng error !!!\n" );
			_ASSERT( 0 );
			goto convexit;
		}
		ret = WriteUCharaData( outfile, (unsigned char*)&arrayleng, sizeof( int ) );
		if( ret ){
			DbgOut( "quafile : ConvSymmXMotion : write arrayleng error !!!\n" );
			_ASSERT( 0 );
			goto convexit;
		}












		QUANAMEHDR curhdr;
		ZeroMemory( &curhdr, sizeof( QUANAMEHDR ) );
		ret = ReadQuaNameHdr( &curhdr );
		if( ret ){
			DbgOut( "quafile : ConvSymmXMotion : first header read error !!!\n" );
			_ASSERT( 0 );
			goto convexit;
		}
		ret = WriteUCharaData( outfile, (unsigned char*)&curhdr, sizeof( QUANAMEHDR ) );
		if( ret ){
			DbgOut( "quafile : ConvSymmXMotion : first header write error !!!\n" );
			_ASSERT( 0 );
			goto convexit;
		}


		while( curhdr.seri >= 0 ){
			int motkeyno;
			for( motkeyno = 0; motkeyno < curhdr.framenum; motkeyno++ ){
				QUADATA3 curdata3;
				ZeroMemory( &curdata3, sizeof( QUADATA3 ) );
				ret = ReadQuaData3( &curdata3 );
				if( ret ){
					DbgOut( "quafile : ConvSymmXMotion : ReadQuaData3 error !!!\n" );
					_ASSERT( 0 );
					goto convexit;
				}

				QUASPPARAM qspp;
				ZeroMemory( &qspp, sizeof( QUASPPARAM ) );
				if( header.magicno >= QUAFILEMAGICNO12 ){
					ret = ReadUCharaData( (unsigned char*)(&qspp), sizeof( QUASPPARAM ) );
					if( ret ){
						DbgOut( "quafile : ConvSymmXMotion : ReadUC qspp error !!!\n" );
						_ASSERT( 0 );
						goto convexit;
					}
				}


				int frameno = curdata3.m_frameno;
				CMotionPoint2 setmp;
				setmp.ishead = curdata3.ishead;
				setmp.m_frameno = frameno;
				setmp.m_rotx = curdata3.m_rot.x;
				setmp.m_roty = curdata3.m_rot.y;
				setmp.m_rotz = curdata3.m_rot.z;
				setmp.m_mvx = curdata3.m_mv.x;
				setmp.m_mvy = curdata3.m_mv.y;
				setmp.m_mvz = curdata3.m_mv.z;
				setmp.m_q = curdata3.m_q;
				setmp.serialno = curdata3.serialno;
				setmp.interp = curdata3.interp;
				setmp.m_scalex = curdata3.m_scale.x;
				setmp.m_scaley = curdata3.m_scale.y;
				setmp.m_scalez = curdata3.m_scale.z;
				setmp.m_userint1 = curdata3.m_userint1;
				setmp.m_eul = curdata3.m_eul;
				setmp.m_spp = 0;

				CMotionPoint2 outmp;

				ret = SOSetSymMotionPoint( SYMMTYPE_X, &setmp, &outmp, 1 );
				_ASSERT( !ret );
				outmp.m_spp = 0;

				QUADATA3 outdata3;
				ZeroMemory( &outdata3, sizeof( QUADATA3 ) );
				outdata3 = curdata3;
				outdata3.m_q = outmp.m_q;
				outdata3.m_eul.x *= -1.0f;
				outdata3.m_mv.x = outmp.m_mvx;
				outdata3.m_mv.y = outmp.m_mvy;
				outdata3.m_mv.z = outmp.m_mvz;

				ret = WriteUCharaData( outfile, (unsigned char*)(&outdata3), sizeof( QUADATA3 ) );
				if( ret ){
					DbgOut( "quafile : ConvSymmXMotion : write outdata3 error !!!\n" );
					_ASSERT( 0 );
					goto convexit;
				}
				if( header.magicno >= QUAFILEMAGICNO12 ){
					ret = WriteUCharaData( outfile, (unsigned char*)(&qspp), sizeof( QUASPPARAM ) );
					if( ret ){
						DbgOut( "quafile : ConvSymmXMotion : write qspp error !!!\n" );
						_ASSERT( 0 );
						goto convexit;
					}
				}

			}

			int texkeyno;
			for( texkeyno = 0; texkeyno < curhdr.texframenum; texkeyno++ ){
				QUATEXKEY qtk;

				ret = ReadQuaTexKey( &qtk );
				if( ret ){
					DbgOut( "QuaFile : ConvSymmXMotion : ReadQuaTexKey error !!!\n" );
					_ASSERT( 0 );
					goto convexit;
				}
				ret = WriteUCharaData( outfile, (unsigned char*)&qtk, sizeof( QUATEXKEY ) );
				if( ret ){
					DbgOut( "QuaFile : ConvSymmXMotion : Write QuaTexKey error !!!\n" );
					_ASSERT( 0 );
					goto convexit;
				}

				
				int tcno;
				for( tcno = 0; tcno < qtk.changenum; tcno++ ){
					QUATEXCHANGE qtc;
					ret = ReadQuaTexChange( &qtc );
					if( ret ){
						DbgOut( "QuaFile : ConvSymmXMotion texanim : ReadQuaTexChange error !!!\n" );
						_ASSERT( 0 );
						goto convexit;
					}
					ret = WriteUCharaData( outfile, (unsigned char*)&qtc, sizeof( QUATEXCHANGE ) );
					if( ret ){
						DbgOut( "QuaFile : ConvSymmXMotion texanim : Write QuaTexChange error !!!\n" );
						_ASSERT( 0 );
						goto convexit;
					}
				}
			}

			int alpkeyno;
			for( alpkeyno = 0; alpkeyno < curhdr.alpframenum; alpkeyno++ ){
				QUAALPKEY qalpk;

				ret = ReadQuaAlpKey( &qalpk );
				if( ret ){
					DbgOut( "QuaFile : ConvSymmXMotion : ReadQuaAlpKey error !!!\n" );
					_ASSERT( 0 );
					goto convexit;
				}
				ret = WriteUCharaData( outfile, (unsigned char*)&qalpk, sizeof( QUAALPKEY ) );
				if( ret ){
					DbgOut( "QuaFile : ConvSymmXMotion : Write QuaAlpKey error !!!\n" );
					_ASSERT( 0 );
					goto convexit;
				}

				
				int alpcno;
				for( alpcno = 0; alpcno < qalpk.changenum; alpcno++ ){
					QUAALPCHANGE qalpc;
					ret = ReadQuaAlpChange( &qalpc );
					if( ret ){
						DbgOut( "QuaFile : ConvSymmXMotion alpanim : ReadQuaAlpChange error !!!\n" );
						_ASSERT( 0 );
						goto convexit;
					}
					ret = WriteUCharaData( outfile, (unsigned char*)&qalpc, sizeof( QUAALPCHANGE ) );
					if( ret ){
						DbgOut( "QuaFile : ConvSymmXMotion alpanim : Write QuaAlpChange error !!!\n" );
						_ASSERT( 0 );
						goto convexit;
					}
				}
			}

			int dskeyno;
			for( dskeyno = 0; dskeyno < curhdr.dsframenum; dskeyno++ ){
				QUADSKEY qdsk;

				ret = ReadQuaDSKey( &qdsk );
				if( ret ){
					DbgOut( "QuaFile : ConvSymmXMotion : ReadQuaDSKey error !!!\n" );
					_ASSERT( 0 );
					goto convexit;
				}
				ret = WriteUCharaData( outfile, (unsigned char*)&qdsk, sizeof( QUADSKEY ) );
				if( ret ){
					DbgOut( "QuaFile : ConvSymmXMotion : Write QuaDSKey error !!!\n" );
					_ASSERT( 0 );
					goto convexit;
				}

				int dscno;
				for( dscno = 0; dscno < qdsk.changenum; dscno++ ){
					QUADSCHANGE qdsc;
					ret = ReadQuaDSChange( &qdsc );
					if( ret ){
						DbgOut( "QuaFile : ConvSymmXMotion DS : ReadQuaDSChange error !!!\n" );
						_ASSERT( 0 );
						goto convexit;
					}
					ret = WriteUCharaData( outfile, (unsigned char*)&qdsc, sizeof( QUADSCHANGE ) );
					if( ret ){
						DbgOut( "QuaFile : ConvSymmXMotion DS : Write QuaDSChange error !!!\n" );
						_ASSERT( 0 );
						goto convexit;
					}
				}

			}

			//morph anim
			if( header.magicno >= QUAFILEMAGICNO10 ){
				MORPHVAL curmv;
				ret = ReadMorphVal( &curmv );
				if( ret ){
					_ASSERT( 0 );
					goto convexit;
				}
				ret = WriteUCharaData( outfile, (unsigned char*)&curmv, sizeof( MORPHVAL ) );
				if( ret ){
					_ASSERT( 0 );
					goto convexit;
				}
				while( curmv.frameno >= 0 ){
					ret = ReadMorphVal( &curmv );
					if( ret ){
						_ASSERT( 0 );
						goto convexit;
					}
					ret = WriteUCharaData( outfile, (unsigned char*)&curmv, sizeof( MORPHVAL ) );
					if( ret ){
						_ASSERT( 0 );
						goto convexit;
					}
				}
			}


			ret = ReadQuaNameHdr( &curhdr );
			if( ret ){
				DbgOut( "quafile : ConvSymmXMotion : namehdr read error !!!\n" );
				_ASSERT( 0 );
				goto convexit;
			}
			ret = WriteUCharaData( outfile, (unsigned char*)&curhdr, sizeof( QUANAMEHDR ) );
			if( ret ){
				DbgOut( "quafile : ConvSymmXMotion : NameQuaHeader write error !!!\n" );
				_ASSERT( 0 );
				goto convexit;
			}
		}
	}

	goto convexit;

convexit:

	FlushFileBuffers( outfile );
	SetEndOfFile( outfile );
	CloseHandle( outfile );

	return ret;
}

int CQuaFile::WriteMorphIndices( HANDLE srchfile )
{
	int ret;
	if( m_morphindex ){
		free( m_morphindex );
		m_morphindex = 0;
	}
	m_morphindexnum = 0;


	int morphnum = 0;
	ret = lpsh->GetMorphElem( 0, &morphnum, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int indexnum = 0;

	if( morphnum > 0 ){
		CShdElem** ppselem;
		ppselem = (CShdElem**)malloc( sizeof( CShdElem* ) * morphnum );
		if( !ppselem ){
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( ppselem, sizeof( CShdElem* ) * morphnum );

		int tmpnum = 0;
		ret = lpsh->GetMorphElem( ppselem, &tmpnum, morphnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		if( morphnum != tmpnum ){
			_ASSERT( 0 );
			return 1;
		}

		int mno;
		for( mno = 0; mno < morphnum; mno++ ){
			CMorph* morph;
			morph = (*(ppselem + mno))->morph;
			if( morph->m_baseelem ){
				indexnum += (morph->m_targetnum + 1);// +1はベースの分
//char strchk1[256];
//sprintf_s( strchk1, 256, "baseseri %d, targetnum %d, indexnum %d", morph->m_baseelem->serialno, morph->m_targetnum, indexnum );
//::MessageBox( NULL, strchk1, "チェック１", MB_OK );

			}
		}
		ret = WriteUCharaData( srchfile, (unsigned char*)&indexnum, sizeof( int ) );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

///////////
		m_morphindex = (MORPHINDEX*)malloc( sizeof( MORPHINDEX ) * indexnum );
		if( !m_morphindex ){
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( m_morphindex, sizeof( MORPHINDEX ) * indexnum );

		int setno = 0;
		for( mno = 0; mno < morphnum; mno++ ){
			CMorph* morph;
			morph = (*(ppselem + mno))->morph;
			if( morph->m_baseelem ){
				CTreeElem2* basete;
				basete = (*lpth)( morph->m_baseelem->serialno );

				(m_morphindex + setno)->seri = morph->m_baseelem->serialno;
				strcpy_s( (m_morphindex + setno)->name, 256, basete->name );
				(m_morphindex + setno)->index = setno;
				setno++;

				int tno;
				for( tno = 0; tno < morph->m_targetnum; tno++ ){
					CShdElem* curtarget;
					curtarget = ( morph->m_ptarget + tno )->selem;
					_ASSERT( curtarget );
					CTreeElem2* curtargette;
					curtargette = (*lpth)( curtarget->serialno );
					_ASSERT( curtargette );

					(m_morphindex + setno)->seri = curtarget->serialno;
					strcpy_s( (m_morphindex + setno)->name, 256, curtargette->name );
					(m_morphindex + setno)->index = setno;
					setno++;
				}
			}
		}
		if( setno != indexnum ){
			_ASSERT( 0 );
			return 1;
		}
		m_morphindexnum = indexnum;//!!!!!!!!!!!!

		ret = WriteMorphIndex( srchfile, m_morphindex, indexnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		free( ppselem );
	}else{
		ret = WriteUCharaData( srchfile, (unsigned char*)&indexnum, sizeof( int ) );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CQuaFile::Seri2MorphIndex( int srcseri )
{
	if( !m_morphindex || (m_morphindexnum <= 0 ) ){
		return -1;
	}

	int retindex = -1;

	int i;
	for( i = 0; i < m_morphindexnum; i++ ){
		MORPHINDEX* curmi;
		curmi = m_morphindex + i;

		if( curmi->seri == srcseri ){
			retindex = curmi->index;
			break;
		}
	}

	return retindex;
}

int CQuaFile::MorphIndex2Seri( int srcindex )
{
	if( !m_morphindex || (m_morphindexnum <= 0 ) ){
		return -1;
	}

	int retseri = -1;

	int i;
	for( i = 0; i < m_morphindexnum; i++ ){
		MORPHINDEX* curmi;
		curmi = m_morphindex + i;

		if( curmi->index == srcindex ){
			retseri = curmi->seri;
			break;
		}
	}

	return retseri;
}


int CQuaFile::LoadMorphVals( CMotionCtrl* mcptr, int motid, int frameoffset )
{
	int ret;
	CMotionInfo* motinfo = 0;
	if( mcptr ){
		motinfo = mcptr->motinfo;
	}


	MORPHVAL curmv;
	ret = ReadMorphVal( &curmv );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	while( curmv.frameno >= 0 ){
		int baseseri;
		baseseri = MorphIndex2Seri( curmv.baseindex );
		int targetseri;
		targetseri = MorphIndex2Seri( curmv.targetindex );

//		char strchk[256];
//		sprintf_s( strchk, 256, "base %d, target %d", baseseri, targetseri );
//		::MessageBox( NULL, strchk, "チェック", MB_OK );

		if( (baseseri > 0) && (targetseri > 0) && motinfo ){
			CShdElem* baseelem;
			baseelem = (*lpsh)( baseseri );
			CShdElem* targetelem;
			targetelem = (*lpsh)( targetseri );
			
			ret = motinfo->SetMMotValue( motid, curmv.frameno + frameoffset, baseelem, targetelem, curmv.value );
			if( ret ){
				DbgOut( "quafile : LoadMorphVals : mi SetMMotValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = ReadMorphVal( &curmv );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
