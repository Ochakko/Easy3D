#define	DISPMODEH	1

#ifndef WNDMAX
	#define WNDMAX	400
#endif

//******* WINDOW MESSAGE ***********

#define	WM_MOVEDLG	WM_USER + 5
#define	WM_DISPDLG	WM_USER + 6
	//WM_INITDIALOG �� �\���
#define WM_INITDLG	WM_USER + 7
#define	WM_LOADDLG	WM_USER + 8

#define WM_MPDLGDATAS	WM_USER + 9
#define WM_MPDLGDATAE	WM_USER + 10

#define WM_CALCDLG	WM_USER + 11
#define	WM_RENDER3D	WM_USER + 12
#define	WM_SWITCHGDI	WM_USER + 14

#define	WM_CHARDIR	WM_USER + 15

#define	WM_ENDAPP	WM_USER + 16

//*******  ALPHA MODE *************
	// gblendmode �Ɂ@�Z�b�g
enum	{
	BLEND_NONE, // BLEND �Ȃ�
	BLEND_ADD,	// src 1, dest 1 �́@add mode �ł́@blend 
				// (riva128,dx5�ł́@����ȊO��blend�́@����) alpha�́@�������@�����Ȃ�
	BLEND_MAX	// �z��m�ۗp�́@�萔�@
};
	

//*******  PROJECTION MODE **********
	// MODE �̃Z�b�g�� mainfrm.cpp, matrix���Z�̎����� matrix.cpp
enum	{
	PROJ_NORMAL,
	PROJ_DOOR,
	PROJ_MAX
};

//*******  DLG MODE for Set Pause ******
//EDITPOINT!!!DISPMODE
enum {
	MODE_EXITDLG, ////!!!!!!!!!!
	MODE_NORMAL,
	MODE_SELMODE,
	MODE_SELCTRL,
	MODE_CTLALL,
	MODE_HEAD0,
	MODE_HEAD00,
	MODE_HEAD000,
	MODE_HEAD010,
	MODE_ARML0,
	MODE_ARMR0,
	MODE_LEGL0,
	MODE_LEGR0,
	MODE_PIS,
	MODE_MOTP,
	MODE_MPCTLALL,
	MODE_MPHEAD0,
	MODE_MPHEAD00,
	MODE_MPHEAD000,
	MODE_MPHEAD010,
	MODE_MPARML0,
	MODE_MPARMR0,
	MODE_MPLEGL0,
	MODE_MPLEGR0,
	MODE_MPPIS,
	MODE_MOTF,
	MODE_MAX
	};
//ENDEDIT


enum { // dlgstate 
	DLG_DESTROYED, DLG_ONCREATE, DLG_CREATED, DLG_STATEMAX
	};