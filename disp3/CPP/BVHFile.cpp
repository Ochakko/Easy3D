#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <windows.h>

#include <BVHFile.h>
#include <crtdbg.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>
#include <motionpoint2.h>
#include <quaternion.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <shdelem.h>

#include <bvhelem.h>

#include <mqofile.h>
#include <bonefile.h>

#include <BoneProp.h>

#include <d3dx9.h>


#define DBGH
#include <dbg.h>

#ifndef INEASY3D
#include "c:\pgfile9\RokDeBone2DX\MotParamDlg.h"
#endif

enum {
	STATE_ROOT,
	STATE_OFFSET,
	STATE_CHANELS,
	STATE_JOINT,
	STATE_ENDSITE,
	STATE_BRACE1,
	STATE_BRACE2,
	STATE_MOTION,
	STATE_FRAMES,
	STATE_FRAMETIME,
	STATE_MOTPARAMS,
	STATE_NONE,
	STATE_FINISH,
	STATE_MAX
};

char bvhpat[STATE_MAX + 1][20] = {
	"ROOT",
	"OFFSET",
	"CHANNELS",
	"JOINT",
	"End",
	"{",
	"}",
	"MOTION",
	"Frames:",
	"Frame Time:",
	"",
	"",
	""
};


CBVHFile::CBVHFile()
{
	InitParams();
}
CBVHFile::~CBVHFile()
{
	DestroyObjs();
}

int CBVHFile::InitParams()
{
	m_state = STATE_NONE;

	m_hfile = INVALID_HANDLE_VALUE;
	m_buf = 0;
	m_pos = 0;
	m_bufleng = 0;

	lpth = 0;
	lpsh = 0;
	lpmh = 0;

	m_hwfile = INVALID_HANDLE_VALUE;
	m_writenum = 0;

	m_motcookie = -1;

	m_behead = 0;
	m_curbe = 0;
	m_parbe = 0;

	ZeroMemory( m_linechar, BVHLINELENG );
	
	m_frames = 0;
	m_frametime = 0.00833333f;

	m_benum = 0;
	m_bearray = 0;

	m_appwnd = 0;

	m_jointnum = 0;
	m_bonenum = 0;

	m_motframenum = 0;

	return 0;
}
int CBVHFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}

	if( m_buf ){
		free( m_buf );
		m_buf = 0;
	}

	m_pos = 0;
	m_bufleng = 0;


	if( m_hwfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hwfile );
		SetEndOfFile( m_hwfile );
		CloseHandle( m_hwfile );
		m_hwfile = INVALID_HANDLE_VALUE;
	}


////////

	if( m_bearray ){

		int beno;
		CBVHElem* delbe;
		for( beno = 0; beno < m_benum; beno++ ){
			delbe = *( m_bearray + beno );
			delete delbe;
		}
		
		free( m_bearray );
		m_bearray = 0;
		m_benum = 0;
	}

	m_behead = 0;

	return 0;
}

int CBVHFile::LoadBVHFile( void* mpdlg, HWND srcapphwnd, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, 
	char* srcname, char* srcmqoname, char* srcbonname, float srcmult, int srcmotonly, LONG* cookieptr )
{
	int ret = 0;

	DestroyObjs();
	InitParams();

	lpth = srclpth;//<----- ここでは、まだ、ＮＵＬＬの可能性があるので、注意。
	lpsh = srclpsh;
	lpmh = srclpmh;
	m_appwnd = srcapphwnd;

	ret = SetBuffer( srcname );
	if( ret ){
		DbgOut( "bvhfile : LoadBVHFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto ldbvhexit;
	}

	ret = CheckFileHeader();
	if( ret ){
		DbgOut( "bvhfile : LoadBVHFile : CheckFileHeader error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto ldbvhexit;
	}

	m_state = STATE_NONE;
	while( m_pos < m_bufleng ){

		m_state = GetStateAndLine();

		switch( m_state ){
		case STATE_ROOT:
		case STATE_JOINT:
		case STATE_ENDSITE:
//DbgOut( "bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = CreateNewElem();
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : CreateNewElem error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			break;
		case STATE_OFFSET:
//DbgOut( "bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = LoadOffset();
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : LoadOffset error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			break;
		case STATE_CHANELS:
//DbgOut( "bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = LoadChanels();
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : LoadChanels error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			break;
		case STATE_BRACE1:
//DbgOut( "bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = BeginBrace();
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : BeginBrace error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			break;
		case STATE_BRACE2:
//DbgOut( "bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = EndBrace();
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : EndBrace error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			break;
		case STATE_MOTION:

//DbgOut( "bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			break;
		case STATE_FRAMES:
//DbgOut( "bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = LoadFrames();
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : LoadFrames error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
//_ASSERT( 0 );
			break;
		case STATE_FRAMETIME:
//DbgOut( "bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = LoadFrameTime();
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : LoadFrameTime error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
//_ASSERT( 0 );
			ret = LoadMotionParams();
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : LoadMotionParams error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
//_ASSERT( 0 );
			break;
		case STATE_MOTPARAMS:
			break;
		case STATE_NONE:
			m_pos += 1;
			break;
		case STATE_FINISH:
			m_pos = m_bufleng;
			break;
		default:
			_ASSERT( 0 );
			break;
		}

		if( (m_state < 0) || (m_state == STATE_FINISH) || (m_state == STATE_NONE) || (m_pos >= (int)m_bufleng) ){
			break;
		}
	}

	if( m_behead ){
		SetBVHElemPositionReq( m_behead );
		
		int beno;
		for( beno = 0; beno < m_benum; beno++ ){
			ret = (*( m_bearray + beno ))->ConvertRotate2Q();
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : be ConvertRotate2Q error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}

			/////// for debug
			//char* findname = 0;
			//(*( m_bearray + beno ))->CheckNotAlNumName( &findname );
			//if( findname ){
			//	DbgOut( "bvhfile : LoadBVHFile : be CheckNotAlNumName : find : %s\r\n", findname );
			//}
		}

		CalcBVHTreeQReq( m_behead );

	}

/// debug
//	if( m_behead ){
//		DbgOutBVHElemReq( m_behead, 0 );
//	}
/////////

	if( m_behead ){

		MultBVHElemReq( m_behead, srcmult );

		
	////////
		m_jointnum = 0;
		m_bonenum = 0;
		CountJointNumReq( m_behead, &m_jointnum, &m_bonenum );


		if( srcmotonly == 0 ){
		
		//////// mqofile出力		

//if( m_behead ){
//	DbgOut( "\r\n\r\n!!! 1 !!!\r\n" );
//	DbgOutBVHElemReq( m_behead, 0 );
//}


			CMQOFile mqofile( m_appwnd );
			ret = mqofile.SaveMQOFileFromBVH( m_jointnum, m_bonenum, m_behead, srcmqoname );
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : mqofile SaveMQOFileFromBVH error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}

//if( m_behead ){
//	DbgOut( "\r\n\r\n!!! 2 !!!\r\n" );
//	DbgOutBVHElemReq( m_behead, 0 );
//}

		//////// 既に読み込み済の、ジョイントの数を取得
			int findcnt = 0;

			if( lpsh ){
				CShdElem* selem;
				int seri;
				for( seri = 1; seri < lpsh->s2shd_leng; seri++ ){
					selem = (*lpsh)( seri );
					if( selem->IsJoint() ){
						findcnt++;
					}
				}
			}else{
				findcnt = 0;
			}
		///////　bonファイル出力

//if( m_behead ){
//	DbgOut( "\r\n\r\n!!! 3 !!!\r\n" );
//	DbgOutBVHElemReq( m_behead, 0 );
//}


			CBoneFile bonefile;
			ret = bonefile.WriteBoneFileFromBVH( srcbonname, findcnt, m_behead );
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : bonefile WriteBoneFileFromBVH error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}

		/////// mqo読み込み
			LOADFILEINFO mqoinfo;
			mqoinfo.filename = srcmqoname;
			//mqoinfo.mult = srcmult;
			mqoinfo.mult = 1.0f;
			mqoinfo.needsetresdir = 0;
			mqoinfo.lpth = 0;
			mqoinfo.lpsh = 0;
			mqoinfo.lpmh = 0;
			ret = (int)::SendMessage( m_appwnd, WM_USER_IMPORTMQO, 0, (LPARAM)&mqoinfo );
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : SendMessage WM_USER_IMPORTMQO error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			
			lpth = mqoinfo.lpth;//!!!!!!!!!!!!!!!!!!!!!!!!!!
			lpsh = mqoinfo.lpsh;
			lpmh = mqoinfo.lpmh;

			if( !lpth || !lpsh || !lpmh ){
				DbgOut( "bvhfile : LoadBVHFile : aft ImportMQO : handler NULL error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}


		///////　bon読み込み
			LOADFILEINFO boninfo;
			boninfo.filename = srcbonname;
			boninfo.mult = 1.0f;
			boninfo.needsetresdir = 0;	
			boninfo.lpth = 0;
			boninfo.lpsh = 0;
			boninfo.lpmh = 0;
			ret = (int)::SendMessage( m_appwnd, WM_USER_LOADBON, 0, (LPARAM)&boninfo );
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : SendMessage WM_USER_LOADBON error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}

		//////　モーション作成
#ifndef INEASY3D
	CMotParamDlg* dlg = (CMotParamDlg*)mpdlg;
	dlg->DestroyFUMotion();
#endif

			char motname[1024];
			ZeroMemory( motname, 1024 );
			SYSTEMTIME systime;
			GetLocalTime( &systime );
			sprintf_s( motname, 1024, "motion_%d_%d_%d_%d_%d_%d_%d",
				systime.wYear,
				systime.wMonth,
				systime.wDay,
				systime.wHour,
				systime.wMinute,
				systime.wSecond,
				systime.wMilliseconds
			);

			m_motframenum = m_frames * 2;//!!!!!!!!!!!!!!!!!!!!!

			m_motcookie = lpmh->AddMotion( ZA_1, motname, MOTION_CLAMP, m_motframenum, INTERPOLATION_SLERP, 0 );
			if( m_motcookie < 0 ){
				DbgOut( "bvhfile : LoadBVHFile : mhandler->AddMotion error !!!\n" );
				_ASSERT( 0 );
				*cookieptr = -1;
				goto ldbvhexit;
			}
			*cookieptr = m_motcookie;
DbgOut( "bvhfile : LoadBVHFile : mh AddMotion %d\r\n", m_motcookie );
		/////////

			int fno;
			int setframeno;
			
			int beno;
			CBVHElem* beptr;
			for( beno = 0; beno < m_benum; beno++ ){
				beptr = *( m_bearray + beno );
				_ASSERT( beptr );

				if( beptr->child ){
					int serialno;
					ret = lpth->GetBoneNoByName( beptr->name, &serialno, lpsh, 1 );
					if( ret || (serialno < 0) ){
						DbgOut( "bvhfile : LoadBVHFile : th GetPartNoByName error !!!\n" );
						_ASSERT( 0 );
						goto ldbvhexit;
					}


					CMotionCtrl* chilmc;
					chilmc = (*lpmh)( serialno )->child;
					while( chilmc ){

						CMotionPoint2* retmp;
						
						for( fno = 0; fno < m_frames; fno++ ){
							setframeno = fno * 2;//!!!!!!!!!!!!!!!!!!
		

							retmp = chilmc->AddMotionPoint( m_motcookie, setframeno,
								*( beptr->qptr + fno ),
								0.0f, 0.0f, 0.0f,
								//(beptr->trans + fno)->x, (beptr->trans + fno)->y, (beptr->trans + fno)->z,
								0xFFFFFFFF,
								INTERPOLATION_SLERP,
								1.0f, 1.0f, 1.0f,
								0 );
							
							if( !retmp ){
								DbgOut( "bvhfile : LoadBVHFile : mc AddMotionPoint error !!!\n" );
								_ASSERT( 0 );
								goto ldbvhexit;
							}

						}

						chilmc = chilmc->brother;
					}
				}	
			}

			if( m_behead ){
				int serialno;
				ret = lpth->GetBoneNoByName( m_behead->name, &serialno, lpsh, 1 );
				if( ret || (serialno < 0) ){
					DbgOut( "bvhfile : LoadBVHFile : th GetPartNoByName error !!!\n" );
					_ASSERT( 0 );
					goto ldbvhexit;
				}


				CMotionCtrl* mc;
				mc = (*lpmh)( serialno );
				if( mc ){

					CMotionPoint2* retmp;
					CQuaternion iniq;
					iniq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

					for( fno = 0; fno < m_frames; fno++ ){
						setframeno = fno * 2;//!!!!!!!!!!!!!!!!!!
		
						retmp = mc->AddMotionPoint( m_motcookie, setframeno,
							iniq,
							(m_behead->trans + fno)->x, (m_behead->trans + fno)->y, (m_behead->trans + fno)->z,
							0xFFFFFFFF,
							INTERPOLATION_SLERP,
							1.0f, 1.0f, 1.0f,
							0 );
							
						if( !retmp ){
							DbgOut( "bvhfile : LoadBVHFile : mc AddMotionPoint error !!!\n" );
							_ASSERT( 0 );
							goto ldbvhexit;
						}

					}
				}
			}

		}else{

		//////　モーション作成
			char motname[1024];
			ZeroMemory( motname, 1024 );
			SYSTEMTIME systime;
			GetLocalTime( &systime );
			sprintf_s( motname, 1024, "motion_%d_%d_%d_%d_%d_%d_%d",
				systime.wYear,
				systime.wMonth,
				systime.wDay,
				systime.wHour,
				systime.wMinute,
				systime.wSecond,
				systime.wMilliseconds
			);

			m_motframenum = m_frames * 2;//!!!!!!!!!!!!!!!!!!!!!

			m_motcookie = lpmh->AddMotion( ZA_1, motname, MOTION_CLAMP, m_motframenum, INTERPOLATION_SLERP, 0 );
			if( m_motcookie < 0 ){
				DbgOut( "bvhfile : LoadBVHFile : mhandler->AddMotion error !!!\n" );
				_ASSERT( 0 );
				*cookieptr = -1;
				goto ldbvhexit;
			}
			*cookieptr = m_motcookie;
DbgOut( "bvhfile : LoadBVHFile : mh AddMotion %d\r\n", m_motcookie );

		////////
			ret = SetBVHSameNameBoneSeri();
			if( ret ){
				DbgOut( "bvhfile : LoadBVHFile : SetBVHSameNameBoneFlag error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}

		///////
			int fno;
			int setframeno;
			int beno;
			CBVHElem* beptr;
			for( beno = 0; beno < m_benum; beno++ ){
				beptr = *( m_bearray + beno );
				_ASSERT( beptr );

				CMotionCtrl* chilmc;
				if( beptr->samenameboneseri > 0 ){
					chilmc = (*lpmh)( beptr->samenameboneseri )->child;

					while( chilmc ){

						CMotionPoint2* retmp;
							
						for( fno = 0; fno < m_frames; fno++ ){
							setframeno = fno * 2;//!!!!!!!!!!!!!!!!!!
								
							CQuaternion setq;
							ret = CalcMPQ( beptr, fno, &setq );
							if( ret ){
								DbgOut( "bvhfile : LoadBVHFile : CalcMPQ error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
					
							retmp = chilmc->AddMotionPoint( m_motcookie, setframeno,
								setq,
								//(beptr->trans + fno)->x, (beptr->trans + fno)->y, (beptr->trans + fno)->z,
								0.0f, 0.0f, 0.0f,
								0xFFFFFFFF,
								INTERPOLATION_SLERP,
								1.0f, 1.0f, 1.0f,
								0 );
								
							if( !retmp ){
								DbgOut( "bvhfile : LoadBVHFile : mc AddMotionPoint error !!!\n" );
								_ASSERT( 0 );
								goto ldbvhexit;
							}

						}

						chilmc = chilmc->brother;
					}
				}

			}

			if( m_behead ){
				int serialno;
				ret = lpth->GetBoneNoByName( m_behead->name, &serialno, lpsh, 1 );
				if( ret || (serialno < 0) ){
					DbgOut( "bvhfile : LoadBVHFile : th GetPartNoByName error !!!\n" );
					_ASSERT( 0 );
					goto ldbvhexit;
				}


				CMotionCtrl* mc;
				mc = (*lpmh)( serialno );
				if( mc ){

					CMotionPoint2* retmp;
					CQuaternion iniq;
					iniq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

					for( fno = 0; fno < m_frames; fno++ ){
						setframeno = fno * 2;//!!!!!!!!!!!!!!!!!!
		
						retmp = mc->AddMotionPoint( m_motcookie, setframeno,
							iniq,
							(m_behead->trans + fno)->x, (m_behead->trans + fno)->y, (m_behead->trans + fno)->z,
							0xFFFFFFFF,
							INTERPOLATION_SLERP,
							1.0f, 1.0f, 1.0f,
							0 );
							
						if( !retmp ){
							DbgOut( "bvhfile : LoadBVHFile : mc AddMotionPoint error !!!\n" );
							_ASSERT( 0 );
							goto ldbvhexit;
						}

					}
				}
			}


		}
	}


	if( m_behead && (m_motcookie >= 0 ) ){
		ret = lpmh->FillUpMotion( lpsh, m_motcookie, -1, 0, -1, 0 );
		if( ret ){
			DbgOut( "bvhfile : LoadBVHFile : fillupmotion error !!!\n" );
			_ASSERT( 0 );
			goto ldbvhexit;
		}
	}

	goto ldbvhexit;
ldbvhexit:
	
	DestroyObjs();


	return ret;
}

int CBVHFile::SetBuffer( char* filename )
{
	DestroyObjs();

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	

	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "bvhfile : SetBuffer :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "bvhfile : SetBuffer :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );//bufleng + 1
	if( !newbuf ){
		DbgOut( "bvhfile : SetBuffer :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "bvhfile : SetBuffer :  ReadFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_buf = newbuf;
	*(m_buf + bufleng) = 0;//!!!!!!!!
	m_bufleng = bufleng;
	m_pos = 0;	

	return 0;
}
int CBVHFile::CheckFileHeader()
{
	char headerstr[20] = "HIERARCHY\r\n";
	char headerstr2[20] = "HIERARCHY\n";
	char headerstr3[20] = "HIERARCHY\r";

	int headerleng;

	char* findptr;
	findptr = strstr( m_buf, headerstr );
	if( findptr )
		headerleng = (int)strlen( headerstr );

	if( !findptr ){
		findptr = strstr( m_buf, headerstr2 );
		if( findptr )
			headerleng = (int)strlen( headerstr2 );
	}
	if( !findptr ){
		findptr = strstr( m_buf, headerstr3 );
		if( findptr )
			headerleng = (int)strlen( headerstr3 );
	}

	if( findptr ){
		int leng;
		leng = (int)( findptr - m_buf + headerleng );
		m_pos += leng;//!!!!!!!
		return 0;

	}else{
		DbgOut( "bvhfile : CheckFileHeader : this file is not BVH format error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
}

int CBVHFile::GetStateAndLine()
{
	int retstate = STATE_NONE;

	char chkchar;
	int startpos = m_pos;
	while( *(m_buf + startpos) ){
		chkchar = *(m_buf + startpos);
		if( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}') || (chkchar == ':')
		){
			break;
		}else{
			if( startpos < (int)m_bufleng ){
				startpos++;
			}else{
				break;
			}
		}
	}

	int endpos = startpos;
	while( *(m_buf + endpos) ){
		chkchar = *(m_buf + endpos);
		if( (endpos < (int)m_bufleng) &&
			( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}') || (chkchar == ':') )
		){
			endpos++;
		}else{
			break;
		}
	}

	int patno;
	int matchflag;
	int findpat = -1;
	int patleng;
	for( patno = STATE_ROOT; patno <= STATE_FRAMETIME; patno++ ){
		patleng = (int)strlen( bvhpat[patno] );
		matchflag = strncmp( m_buf + startpos, bvhpat[patno], patleng );
		if( matchflag == 0 ){
			findpat = patno;
			break;
		}
	}
	if( findpat >= 0 ){
		retstate = patno;
		m_pos = startpos + patleng;//!!!!!!!!!!!!!!!!!!
	}else{
		//_ASSERT( 0 );
		m_pos = endpos;
	}

	if( (retstate == STATE_MOTION) || (retstate == STATE_BRACE1) || (retstate == STATE_BRACE2) ){
		return retstate;//!!!!!!!!!!!!!!!!!!!
	}

////////////////
	int startpos2 = m_pos;
	while( *(m_buf + startpos2) ){
		chkchar = *(m_buf + startpos2);
		if( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}') || (chkchar == ':')
		){
			break;
		}else{
			if( startpos2 < (int)m_bufleng ){
				startpos2++;
			}else{
				break;
			}
		}
	}

	int endpos2 = startpos2;
	while( *(m_buf + endpos2) ){
		chkchar = *(m_buf + endpos2);
		if( (chkchar == '\r') || (chkchar == '\n') ){
			break;
		}else{
			if( endpos2 < (int)m_bufleng ){
				endpos2++;
			}else{
				break;
			}
		}
	}

	int lineleng = endpos2 - startpos2;
	if( lineleng >= BVHLINELENG ){
		_ASSERT( 0 );
		ZeroMemory( m_linechar, sizeof( char ) * BVHLINELENG );
		retstate = STATE_NONE;
	}else{
		strncpy_s( m_linechar, BVHLINELENG, m_buf + startpos2, lineleng );
		m_linechar[ lineleng ] = 0;
	}

	m_pos = endpos2;

	return retstate;
}


int CBVHFile::GetState()
{
	int retstate = STATE_NONE;

	char chkchar;
	int startpos = m_pos;
	while( *(m_buf + startpos) ){
		chkchar = *(m_buf + startpos);
		if( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}')
		){
			break;
		}else{
			if( startpos < (int)m_bufleng ){
				startpos++;
			}else{
				break;
			}
		}
	}
//	m_pos = startpos;


	int patno;
	int matchflag;
	int findpat = -1;
	int patleng;
	for( patno = STATE_ROOT; patno <= STATE_FRAMETIME; patno++ ){
		patleng = (int)strlen( bvhpat[patno] );
//		matchflag = strncmp( m_buf + m_pos, bvhpat[patno], patleng );
		matchflag = strncmp( m_buf + startpos, bvhpat[patno], patleng );
		if( matchflag == 0 ){
			findpat = patno;
			break;
		}
	}

	if( findpat >= 0 ){
		retstate = patno;
//		m_pos += patleng;//!!!!!!!!!!!!!!!!!!
	}

	return retstate;
}

int CBVHFile::CreateNewElem()
{

	int ret;
//	ret = GetLine();
//	if( ret ){
//		DbgOut( "bvhfile : CreateNewElem : GetLine error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}


	CBVHElem* newbe;
	newbe = new CBVHElem();
	if( !newbe ){
		DbgOut( "bvhfile : CreateNewElem : newbe alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = newbe->SetName( m_linechar );
	if( ret ){
		DbgOut( "bvhfile : CreatenewElem : newbe SetName error !!!\n" );
		_ASSERT( 0 );
		delete newbe;
		return 1;
	}

	if( m_state == STATE_ROOT ){
		ret = newbe->SetIsRoot( 1 );
	}else{
		ret = newbe->SetIsRoot( 0 );
	}
	_ASSERT( !ret );

///////////

	newbe->parent = m_parbe;
	if( m_parbe ){
		if( !m_parbe->child ){
			//子供にセット
			m_parbe->child = newbe;
		}else{
			//最後のbrotherにセット
			CBVHElem* lastbro;
			CBVHElem* brobe;
			brobe = m_parbe->child;
			lastbro = brobe;
			while( brobe ){
				lastbro = brobe;
				brobe = brobe->brother;
			}

			_ASSERT( lastbro );
			lastbro->brother = newbe;
		}
	}

////////////

	if( !m_behead ){
		m_behead = newbe;
	}
////////////

	m_benum++;
	m_bearray = (CBVHElem**)realloc( m_bearray, sizeof( CBVHElem* ) * m_benum );
	if( !m_bearray ){
		DbgOut( "bvhfile : CreateNewElem : bearray alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( m_bearray + m_benum - 1 ) = newbe;

/////////////

	m_curbe = newbe;


//	if( newbe->serialno == 4 ){
//		_ASSERT( 0 );
//	}


	return 0;
}

int CBVHFile::GetLine()
{
	char chkchar;
	int startpos2 = m_pos;
	while( *(m_buf + startpos2) ){
		chkchar = *(m_buf + startpos2);
		if( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}') || (chkchar == ':')
		){
			break;
		}else{
			if( startpos2 < (int)m_bufleng ){
				startpos2++;
			}else{
				break;
			}
		}
	}

	int endpos2 = startpos2;
	while( *(m_buf + endpos2) ){
		chkchar = *(m_buf + endpos2);
		if( (chkchar == '\r') || (chkchar == '\n') ){
			break;
		}else{
			if( endpos2 < (int)m_bufleng ){
				endpos2++;
			}else{
				break;
			}
		}
	}

	int lineleng = endpos2 - startpos2;
	if( lineleng >= BVHLINELENG ){
		_ASSERT( 0 );
		ZeroMemory( m_linechar, sizeof( char ) * BVHLINELENG );
	}else{
		strncpy_s( m_linechar, BVHLINELENG, m_buf + startpos2, lineleng );
		m_linechar[ lineleng ] = 0;
	}

	m_pos = endpos2;

	return 0;
}


/***
int CBVHFile::GetLine()
{

	char chkchar;
	int startpos = m_pos;
	while( *(m_buf + startpos) ){
		chkchar = *(m_buf + startpos);
		if( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}') || (chkchar == ':')
		){
			break;
		}else{
			if( startpos < (int)m_bufleng ){
				startpos++;
			}else{
				break;
			}
		}
	}
	m_pos = startpos;



	char retmark[5] = "\r\n";
	char retmark2[5] = "\n";
	char retmark3[5] = "\r";
	int retkind = 0;
	char* retptr;
	int leng;

	retptr = strstr( m_buf + m_pos, retmark );
	if( retptr ){
		leng = (int)( retptr - (m_buf + m_pos) );
		retkind = 1;
	}else{
		retptr = strstr( m_buf + m_pos, retmark2 );
		if( retptr ){
			leng = (int)( retptr - (m_buf + m_pos) );
			retkind = 2;
		}else{
			retptr = strstr( m_buf + m_pos, retmark3 );
			if( retptr ){
				leng = (int)( retptr - (m_buf + m_pos) );
				retkind = 3;
			}else{
				leng = m_bufleng - m_pos;
				retkind = 0;
			}
		}
	}


	if( leng >= BVHLINELENG ){
		DbgOut( "bvhfile : GetLine : line leng too short error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strncpy_s( m_linechar, BVHLINELENG, m_buf + m_pos, leng );
	m_linechar[leng] = 0;

	switch( retkind ){
	case 1:
		m_pos += (leng + 2);
		break;
	case 2:
	case 3:
		m_pos += (leng + 1);
		break;
	default:
		m_pos += leng;
		break;
	}

	return 0;
}
***/
int CBVHFile::LoadOffset()
{
	int ret;
//	ret = GetLine();
//	if( ret ){
//		DbgOut( "bvhfile : LoadOffset : GetLine error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	if( !m_curbe ){
		DbgOut( "bvhfile : LoadOffset : curbe NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_curbe->SetOffset( m_linechar );
	if( ret ){
		DbgOut( "bvhfile : LoadOffset : curbe SetOffset error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBVHFile::LoadChanels()
{
	int ret;
//	ret = GetLine();
//	if( ret ){
//		DbgOut( "bvhfile : LoadChanels : GetLine error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	if( !m_curbe ){
		DbgOut( "bvhfile : LoadChanels : curbe NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_curbe->SetChanels( m_linechar );
	if( ret ){
		DbgOut( "bvhfile : LoadChanels : curbe SetChanels error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBVHFile::BeginBrace()
{
	m_parbe = m_curbe;

	m_pos += 1;//!!!!!!!!!!!!!!!!!!!!

	return 0;
}
int CBVHFile::EndBrace()
{
	if( !m_curbe ){
		DbgOut( "bvhfile : EndBrace : curbe NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_parbe = m_curbe->parent;
	m_curbe = m_parbe;//!!!!!!!!!!!!!!

	m_pos += 1;//!!!!!!!!!!!!!!!!!!!!

	return 0;
}

int CBVHFile::LoadFrames()
{
	int ret;
//	ret = GetLine();
//	if( ret ){
//		DbgOut( "bvhfile : LoadFrames : GetLine error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	int setflag;
	char* valuehead = m_linechar;

	valuehead = GetDigit( valuehead, &m_frames, &setflag );
	if( !setflag ){
		DbgOut( "bvhfile : LoadFrames : GetDigit frames error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
/////////
	int beno;
	CBVHElem* beptr;
	for( beno = 0; beno < m_benum; beno++ ){
		beptr = *( m_bearray + beno );
		_ASSERT( beptr );

		ret = beptr->CreateMotionObjs( m_frames );
		if( ret ){
			DbgOut( "bvhfile : LoadFrames : be CreateMotionObjs error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}
int CBVHFile::LoadFrameTime()
{

//	ret = GetLine();
//	if( ret ){
//		DbgOut( "bvhfile : LoadFrameTime : GetLine error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	int setflag;
	char* valuehead = m_linechar;

	float dummytime;
	//valuehead = GetFloat( valuehead, &m_frametime, &setflag );
	valuehead = GetFloat( valuehead, &dummytime, &setflag );
	if( !setflag ){
		DbgOut( "bvhfile : LoadFrameTime : GetFloat frametime error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBVHFile::LoadMotionParams()
{
	int ret;

	int frameno;
	for( frameno = 0; frameno < m_frames; frameno++ ){
		ret = GetLine();
		if( ret ){
			DbgOut( "bvhfile : LoadMotionParams : GetLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int setflag;
		char* valuehead = m_linechar;


		int beno;
		CBVHElem* beptr;
		for( beno = 0; beno < m_benum; beno++ ){
			beptr = *( m_bearray + beno );
			_ASSERT( beptr );

			int chanelnum;
			chanelnum = beptr->chanelnum;

			int paramno;
			for( paramno = 0; paramno < chanelnum; paramno++ ){
				if( !valuehead ){
					DbgOut( "bvhfile : LoadMotionParams : no more params error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				valuehead = GetFloat( valuehead, &(m_tempparam[paramno]), &setflag );
				if( !setflag ){
					DbgOut( "bvhfile : LoadMotionParams : GetFloat error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}


			ret = beptr->SetMotionParams( frameno, m_tempparam );
			if( ret ){
				DbgOut( "bvhfile : LoadMotionParams : be SetMotionParams error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

	}

	return 0;
}



char* CBVHFile::GetFloat( char* srcstr, float* dstfloat, int* dstsetflag )
{
	char* valuehead = srcstr;

	int curpos;
	int strleng;

	curpos = 0;
	strleng = (int)strlen( srcstr );


	//先頭の非数字をスキップ
	while( (curpos < strleng) && ( (isdigit( *valuehead ) == 0) && (*valuehead != '-'))  ){
		if( isalpha( *valuehead ) != 0 ){
			//illeagal letter
			DbgOut( "bvhfile : GetDigit : isalpha error !!!\n" );
			*dstsetflag = 0;
			return 0;//!!!!
		}
		valuehead++;
		curpos++;
	}

	char* valueend = valuehead;
	//valueの終わりをサーチ
	int valueleng = 0;
	while( (curpos < strleng) && ( isdigit( *valueend ) || (*valueend == '-') ) || (*valueend == '.') || (*valueend == 'e') ){
		valueend++;
		curpos++;
		valueleng++;
	}


	//char ---> float
	if( valueleng >= 255 ){
		_ASSERT( 0 );
		*dstsetflag = 0;//
		return 0;
	}
	char tmpchar[256];
	strncpy_s( tmpchar, 256, valuehead, valueleng );
	tmpchar[valueleng] = 0;

	float tempfloat;
	tempfloat = (float)atof( tmpchar );

	*dstfloat = tempfloat;
	*dstsetflag = 1;//

	return valueend;

}



char* CBVHFile::GetDigit( char* srcstr, int* dstint, int* dstsetflag )
{

	char* valuehead = srcstr;

	int curpos;
	int strleng;

	curpos = 0;
	strleng = (int)strlen( srcstr );

	//先頭の非数字をスキップ
	while( (curpos < strleng) && ( (isdigit( *valuehead ) == 0) && (*valuehead != '-'))  ){
		if( isalpha( *valuehead ) != 0 ){
			//illeagal letter
			DbgOut( "bvhfile : GetDigit : isalpha error !!!\n" );
			*dstsetflag = 0;
			return 0;//!!!!
		}
		valuehead++;
		curpos++;
	}

	char* valueend = valuehead;
	//valueの終わりをサーチ
	int valueleng = 0;
	while( (curpos < strleng) && ( isdigit( *valueend ) || (*valueend == '-') ) ){
		valueend++;
		curpos++;
		valueleng++;
	}


	//char ---> int
	if( valueleng >= 255 ){
		_ASSERT( 0 );
		*dstsetflag = 0;//
		return 0;
	}
	char tmpchar[256];
	strncpy_s( tmpchar, 256, valuehead, valueleng );
	tmpchar[valueleng] = 0;

	int tempint;
	tempint = atoi( tmpchar );

	*dstint = tempint;
	*dstsetflag = 1;//

	return valueend;
}

void CBVHFile::DbgOutBVHElemReq( CBVHElem* outbe, int srcdepth )
{
	int ret;

	ret = outbe->DbgOutBVHElem( srcdepth, 1 );
	if( ret ){
		DbgOut( "bvhfile : DbgOutBVHElemReq : be DbgOutBVHElem error !!!\n" );
		_ASSERT( 0 );
		return;
	}

	if( outbe->child ){
		DbgOutBVHElemReq( outbe->child, srcdepth + 1 );
	}

	if( outbe->brother ){
		DbgOutBVHElemReq( outbe->brother, srcdepth );
	}

}

void CBVHFile::SetBVHElemPositionReq( CBVHElem* srcbe )
{
	int ret;

	ret = srcbe->SetPosition();
	if( ret ){
		DbgOut( "bvhfile : SetBVHElemPositionReq : be SetPosition error !!!\n" );
		_ASSERT( 0 );
		return;
	}

	if( srcbe->child ){
		SetBVHElemPositionReq( srcbe->child );
	}

	if( srcbe->brother ){
		SetBVHElemPositionReq( srcbe->brother );
	}

}

void CBVHFile::CountJointNumReq( CBVHElem* srcbe, int* jointnumptr, int* bonenumptr )
{
	(*jointnumptr)++;

///////////
	if( srcbe->child ){
		(*bonenumptr)++;//!!!!!!!!!!!
		CountJointNumReq( srcbe->child, jointnumptr, bonenumptr );
	}
	if( srcbe->brother ){
		(*bonenumptr)++;//!!!!!!!!!!!
		CountJointNumReq( srcbe->brother, jointnumptr, bonenumptr );
	}
}

void CBVHFile::MultBVHElemReq( CBVHElem* srcbe, float srcmult )
{
	int ret;

	ret = srcbe->Mult( srcmult );
	if( ret ){
		DbgOut( "bvhfile : MultBVHElemReq : be Mult error !!!\n" );
		_ASSERT( 0 );
		return;
	}

////////
	if( srcbe->child ){
		MultBVHElemReq( srcbe->child, srcmult );
	}
	if( srcbe->brother ){
		MultBVHElemReq( srcbe->brother, srcmult );
	}
}


void CBVHFile::CalcBVHTreeQReq( CBVHElem* srcbe )
{
	int fno;

	if( srcbe->parent ){
		for( fno = 0; fno < m_frames; fno++ ){
			*(srcbe->treeq + fno) = *(srcbe->parent->treeq + fno) * *(srcbe->qptr + fno);
			//*(srcbe->treeq + fno) = *(srcbe->parent->treeq + fno) * *(srcbe->transpose + fno);
		}
	}else{
		for( fno = 0; fno < m_frames; fno++ ){
			*(srcbe->treeq + fno) = *(srcbe->qptr + fno);
			//*(srcbe->treeq + fno) = *(srcbe->transpose + fno);
		}
	}
//////////
	if( srcbe->child ){
		CalcBVHTreeQReq( srcbe->child );
	}
	if( srcbe->brother ){
		CalcBVHTreeQReq( srcbe->brother );
	}

}

int CBVHFile::SetBVHSameNameBoneSeri()
{
	int ret;

	if( !lpth || !lpsh ){
		DbgOut( "bvhfile : SetBVHSameNameBoneSeri : handler NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CBVHElem* curbe;
	int beno;
	for( beno = 0; beno < m_benum; beno++ ){
		curbe = *( m_bearray + beno );

		int serialno;
		int findserialno = -1;//!!!!!!!!
		ret = lpth->GetPartNoByName( curbe->name, &serialno );
		if( ret ){
			DbgOut( "bvhfile : SetBVHSameNameBoneSeri : th GetPartNoByName error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( serialno >= 0 ){

			CShdElem* selem;
			selem = (*lpsh)( serialno );
			_ASSERT( selem );

			if( selem->IsJoint() && (selem->type != SHDMORPH) ){
				CPart* partptr;
				partptr = selem->part;
				if( !partptr ){
					DbgOut( "bvhfile : SetBVHSameNameBoneSeri : partptr NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( partptr->bonenum > 0 ){
					findserialno = serialno;//!!!!!!!!!!
				}

			}
		}

		curbe->samenameboneseri = findserialno;

	}

	return 0;
}



int CBVHFile::CalcMPQ( CBVHElem* beptr, int fno, CQuaternion* dstq )
{
	//int ret;

	CBVHElem* curbe = beptr->parent;//!!!!!!!
	CBVHElem* findparbe = 0;
	while( curbe ){
		if( curbe->samenameboneseri > 0 ){
			findparbe = curbe;
			break;
		}
		curbe = curbe->parent;
	}

	if( findparbe ){
		CQuaternion curq;
		CQuaternion parq;
		CQuaternion invparq;

		curq = *(beptr->treeq + fno);
		parq = *(findparbe->treeq + fno);
		parq.inv( &invparq );

		*dstq = invparq * curq;
		//*dstq = curq * invparq;
	
	}else{
		*dstq = *(beptr->treeq + fno);
	}

	return 0;
}

int CBVHFile::SaveBVHFile( CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int motid,
		char* filename, float srcmult )
{
	int ret = 0;

	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;

	int* wno2seri = 0;


	m_hwfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hwfile == INVALID_HANDLE_VALUE ){
		DbgOut( "bvhfile : SaveBVHFile : CreateFile error !!! %s\n", filename );
		_ASSERT( 0 );
		ret = 1;
		goto savebvhexit;
	}	
	SetEndOfFile( m_hwfile );//!!!!!!!!!!
	
	CShdElem* firstse = (*lpsh)( 0 );
	CShdElem* firstjoint = 0;
	FindFirstJointReq( firstse, &firstjoint ); 
	if( !firstjoint ){
		DbgOut( "bvhfile : SaveBVHFile : firstjoint not found skip !!!\n" );
		_ASSERT( 0 );
		ret = 0;//!!!!!!!!
		goto savebvhexit;
	}
	
	int writeno = 0;
	int errorflag = 0;
	int depth = 0;
	int broflag = 0;
	//一回目はwritenumの調査呼び出し
	WriteBVHTreeReq( firstjoint, &writeno, depth, broflag, &errorflag, 0 );
	if( errorflag != 0 ){
		DbgOut( "bvhfile : SaveBVHFile : WriteBVHTreeReq 0 error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savebvhexit;
	}

	m_writenum = writeno;

	wno2seri = (int*)malloc( sizeof( int ) * m_writenum );
	if( !wno2seri ){
		DbgOut( "bvhfile : SaveBVHFile : wno2seri alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savebvhexit;
	}
	ZeroMemory( wno2seri, sizeof( int ) * m_writenum );


	Write2File( "HIERARCHY\r\n" );

	ret = lpth->ReplaceToEnglishName();
	if( ret ){
		DbgOut( "bvhfile : SaveBVHFile : th ReplaceToEnglishName error !!!\n" );
		_ASSERT( 0 );
		goto savebvhexit;
	}

	writeno = 0;
	errorflag = 0;
	depth = 0;
	broflag = 0;
	//二回目で書き出しとセット
	WriteBVHTreeReq( firstjoint, &writeno, depth, broflag, &errorflag, wno2seri );
	if( errorflag != 0 ){
		DbgOut( "bvhfile : SaveBVHFile : WriteBVHTreeReq wno2seri error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savebvhexit;
	}

	
	ret = WriteBVHMotion( wno2seri, motid, srcmult );
	if( ret ){
		DbgOut( "bvhfile : SaveBVHFile : WriteBVHMotion error !!!\n" );
		_ASSERT( 0 );
		goto savebvhexit;
	}



	goto savebvhexit;
savebvhexit:
	if( m_hwfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hwfile );
		SetEndOfFile( m_hwfile );
		CloseHandle( m_hwfile );
		m_hwfile = INVALID_HANDLE_VALUE;
	}
	if( wno2seri ){
		free( wno2seri );
	}

	return ret;
}

int CBVHFile::Write2File( char* lpFormat, ... )
{
	if( !m_hwfile ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[2048];
			
	ZeroMemory( outchar, 2048 );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, 2048, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 ){
		_ASSERT( 0 );
		return 1;
	}

	wleng = (unsigned long)strlen( outchar );
	WriteFile( m_hwfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );	
		return 1;
	}

	return 0;	
}

int CBVHFile::WriteTab( int tabnum )
{
	if( !m_hwfile ){
		_ASSERT( 0 );
		return 0;
	}

	if( tabnum >= 2048 ){
		_ASSERT( 0 );
		return 1;
	}

	char outchar[2048];
	ZeroMemory( outchar, sizeof( char ) * 2048 );
	int tno;
	for( tno = 0; tno < tabnum; tno++ ){
		outchar[tno] = '\t';
	}

	unsigned long wleng, writeleng;
	wleng = (unsigned long)strlen( outchar );
	WriteFile( m_hwfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );	
		return 1;
	}

	return 0;
}

void CBVHFile::FindFirstJointReq( CShdElem* selem, CShdElem** ppfirstjoint )
{

	if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		CShdElem* parelem;
		parelem = lpsh->FindUpperJoint( selem );
		if( !parelem && !*ppfirstjoint ){
			*ppfirstjoint = selem;
			return;
		}
	}

	if( selem->brother ){
		FindFirstJointReq( selem->brother, ppfirstjoint );
	}

	if( selem->child ){
		FindFirstJointReq( selem->child, ppfirstjoint );
	}
}

void CBVHFile::WriteBVHTreeReq( CShdElem* selem, int* pwriteno, int depth, int broflag, int* perrorflag, int* wno2seri )
{
	int topofflag = 0;
	int parentofflag = 0;
	int jointflag = 0;
	//int ret;

	if( selem->IsJoint() && (selem->type != SHDMORPH) ){

		CTreeElem2* te;
		te = (*lpth)( selem->serialno );
		_ASSERT( te );

		_ASSERT( selem->part );

		if( *pwriteno == 0 ){
			if( wno2seri ){
				Write2File( "ROOT TopOf%s\r\n", te->engname );
				Write2File( "{\r\n" );

				WriteTab( 1 );
				Write2File( "OFFSET\t0\t0\t0\r\n" );
				WriteTab( 1 );
				Write2File( "CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\r\n" );

				_ASSERT( *pwriteno < m_writenum );
				*( wno2seri + *pwriteno ) = selem->serialno;
			}
			depth++;
			(*pwriteno)++;
			topofflag = 1;

			if( wno2seri ){
				WriteTab( depth );
				Write2File( "JOINT %s\r\n", te->engname );
				WriteTab( depth );
				Write2File( "{\r\n" );

				_ASSERT( *pwriteno < m_writenum );
				if( selem->part->bonenum == 1 ){
					_ASSERT( selem->part->ppBI );
					CBoneInfo* pBI;
					pBI = *( selem->part->ppBI );
					_ASSERT( pBI );

					WriteTab( depth + 1 );
					Write2File( "OFFSET\t%f\t%f\t%f\r\n", selem->part->jointloc.x, selem->part->jointloc.y, selem->part->jointloc.z );
					WriteTab( depth + 1 );
					Write2File( "CHANNELS 3 Zrotation Xrotation Yrotation\r\n" );

					*( wno2seri + *pwriteno ) = pBI->childno;
				}else if( selem->part->bonenum == 0 ){

					WriteTab( depth + 1 );
					Write2File( "OFFSET\t%f\t%f\t%f\r\n", selem->part->jointloc.x, selem->part->jointloc.y, selem->part->jointloc.z );					

					*( wno2seri + *pwriteno ) = 0;
				}else{
					WriteTab( depth + 1 );
					Write2File( "OFFSET\t%f\t%f\t%f\r\n", selem->part->jointloc.x, selem->part->jointloc.y, selem->part->jointloc.z );					
					WriteTab( depth + 1 );
					Write2File( "CHANNELS 3 Zrotation Xrotation Yrotation\r\n" );

					*( wno2seri + *pwriteno ) = -1;
				}
			}
			(*pwriteno)++;
			jointflag = 1;

		}else{

			CShdElem* parse;
			parse = lpsh->FindUpperJoint( selem );
			if( parse ){
				if( parse->part->bonenum > 1 ){
					if( wno2seri ){
						WriteTab( depth );
						Write2File( "JOINT ParentOf%s\r\n", te->engname );
						WriteTab( depth );
						Write2File( "{\r\n" );

						WriteTab( depth + 1 );
						Write2File( "OFFSET\t0\t0\t0\r\n" );
						WriteTab( depth + 1 );
						Write2File( "CHANNELS 3 Zrotation Xrotation Yrotation\r\n" );
						
						_ASSERT( *pwriteno < m_writenum );
						*( wno2seri + *pwriteno ) = selem->serialno;

					}
					depth++;
					(*pwriteno)++;
					
					parentofflag = 1;
				}
			}

			if( wno2seri ){
				WriteTab( depth );
				Write2File( "JOINT %s\r\n", te->engname );
				WriteTab( depth );
				Write2File( "{\r\n" );

				if( parse ){
					CVec3f diffloc;
					diffloc.x = selem->part->jointloc.x - parse->part->jointloc.x;
					diffloc.y = selem->part->jointloc.y - parse->part->jointloc.y;
					diffloc.z = selem->part->jointloc.z - parse->part->jointloc.z;
					WriteTab( depth + 1 );
					Write2File( "OFFSET\t%f\t%f\t%f\r\n", diffloc.x, diffloc.y, diffloc.z );					
				}else{
					WriteTab( depth + 1 );
					Write2File( "OFFSET\t%f\t%f\t%f\r\n", selem->part->jointloc.x, selem->part->jointloc.y, selem->part->jointloc.z );
				}

				_ASSERT( *pwriteno < m_writenum );
				if( selem->part->bonenum == 1 ){

					WriteTab( depth + 1 );
					Write2File( "CHANNELS 3 Zrotation Xrotation Yrotation\r\n" );

					_ASSERT( selem->part->ppBI );
					*( wno2seri + *pwriteno ) = (*(selem->part->ppBI))->childno;
				}else if( selem->part->bonenum == 0 ){
					*( wno2seri + *pwriteno ) = 0;
				}else{
					WriteTab( depth + 1 );
					Write2File( "CHANNELS 3 Zrotation Xrotation Yrotation\r\n" );

					*( wno2seri + *pwriteno ) = -1;
				}

			}
			(*pwriteno)++;

			jointflag = 1;
		}


	}

	if( selem->child ){
		int nextchildepth;
		if( jointflag != 0 ){
			nextchildepth = depth + 1;
		}else{
			nextchildepth = depth;
		}
		WriteBVHTreeReq( selem->child, pwriteno, nextchildepth, 1, perrorflag, wno2seri );
	}

	int nextbrodepth;
	if( jointflag ){
		if( wno2seri ){
			WriteTab( depth );
			Write2File( "}\r\n" );
		}
	}
	if( topofflag || parentofflag ){
		if( wno2seri ){
			WriteTab( depth - 1 );
			Write2File( "}\r\n" );
		}
		nextbrodepth = depth - 1;
	}else{
		nextbrodepth = depth;
	}

	if( selem->brother && (broflag != 0) ){
		WriteBVHTreeReq( selem->brother, pwriteno, nextbrodepth, 1, perrorflag, wno2seri );
	}
}

int CBVHFile::WriteBVHMotion( int* wno2seri, int motid, float srcmult )
{
	int ret;
	int frameleng = 0;
	ret = lpmh->GetMotionFrameLength( motid, &frameleng );
	if( ret ){
		DbgOut( "bvhfile : WriteBVHMotion : mh GetMotionFrameLeng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3* befeul;
	befeul = new D3DXVECTOR3[ m_writenum ];
	if( !befeul ){
		DbgOut( "bvhfile : WriteBVHMotion : befuel alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( befeul, sizeof( D3DXVECTOR3 ) * m_writenum );



	Write2File( "MOTION\r\n" );
	Write2File( "Frames: %d\r\n", frameleng );
	float frametime = 1.0f / 60.0f;
	Write2File( "Frame Time: %f\r\n", frametime );

	int frameno;
	int wno;
	for( frameno = 0; frameno < frameleng; frameno++ ){
		for( wno = 0; wno < m_writenum; wno++ ){
			int seri;
			seri = *( wno2seri + wno );

			if( seri > 0 ){
				CMotionCtrl* mcptr;
				mcptr = (*lpmh)( seri );
				_ASSERT( mcptr );
				CMotionInfo* mi;
				mi = mcptr->motinfo;
				_ASSERT( mi );

				CShdElem* selem;
				selem = (*lpsh)( seri );
				_ASSERT( selem );

				ret = lpmh->SetBoneAxisQ( lpsh, selem->serialno, lpmh->GetZaType( motid ), motid, frameno );
				_ASSERT( !ret );
				CQuaternion axisq;
				ret = selem->GetBoneAxisQ( &axisq );
				_ASSERT( !ret );



				CMotionPoint2 mp;
				int hasmpflag;
				ret = mi->CalcMotionPointOnFrame( &axisq, selem, &mp, motid, frameno, &hasmpflag );
				if( ret ){
					DbgOut( "bvhfile : WriteBVHMotion : mi CalcMotionPointOnFrame error !!!\n" );
					_ASSERT( 0 );
					delete [] befeul;
					return 1;
				}
								
				D3DXVECTOR3 neweul;
				ret = qToEulerYXZ( selem, &mp.m_q, &neweul );
				_ASSERT( !ret );
				ret = modifyEuler( &neweul, &(befeul[wno]) );
				_ASSERT( !ret );
				
				befeul[wno] = neweul;

				if( wno == 0 ){
					Write2File( "%f\t%f\t%f\t", mp.m_mvx, mp.m_mvy, mp.m_mvz );
					Write2File( "%f\t%f\t%f\t", neweul.z, neweul.x, neweul.y - 180.0f );
				}else{
					Write2File( "%f\t%f\t%f\t", neweul.z, neweul.x, neweul.y );
				}



			}else if( seri == 0 ){
				//Write2File( "%d-EmptyData\r\n", wno );
			}else{
				//Write2File( "%d-IniData\r\n", wno );
				Write2File( "0\t0\t0\t" );
			}

		}
		Write2File( "\r\n" );
	}


	delete [] befeul;


	return 0;
}
