#ifndef PluginKinectH
#define PluginKinectH

#include <d3dx9.h>

class CTxtSetting;
class CRpsFile;
class CHandlerSet;
//class CConvSetDlg;
class CShdHandler;
class CQuaternion;

class CPluginKinect
{
public:
	CPluginKinect( int fullscflag );
	~CPluginKinect();

	int SetFilePath( char* pluginpath );
	int LoadPlugin();

	int SetChara( CHandlerSet* hsptr, char* kstname, int motid, int capmode );

	int StartCapture();
	int EndCapture();

	int GetSkelPos( D3DXVECTOR3* posptr, int* getflag );
	int SetSkelPos( int kinectflag, int posmode, D3DXVECTOR3* srcpos, int* framenoptr );

	int IsTracking( int* flagptr, int* cntptr );
private:
	int UnloadPlugin();
	void DestroyRps();
	void DestroyTxtSetting();

	int CreateMotionPoints( int cookie, int startframe, int endframe, int forceflag );

//	int AvrgMotion( int motid, int frameleng, int type, int filter_size );

	//ガウシアンフィルタ用
//	int combi( int N, int rp );

	int QtoEul( CShdHandler* lpsh, CQuaternion srcq, D3DXVECTOR3 befeul, int boneno, D3DXVECTOR3* eulptr, CQuaternion* axisqptr );

public:
	CHandlerSet* m_hs;
	LPDIRECT3DDEVICE9 m_pdev;
	IDirect3DTexture9* m_ptex;
	HWND m_hwnd;
	int m_texwidth;
	int m_texheight;


	HMODULE m_hModule;
	char m_filepath[_MAX_PATH];
	char m_filename[_MAX_PATH];
	float m_pluginversion;

	int m_validflag;
	bool (*OpenNIInit)(int fullscflag,HWND hWnd,bool EngFlag,LPDIRECT3DDEVICE9 lpDevice,CHAR* f_path,CHAR* onifilename);
	void (*OpenNIClean)();
	void (*OpenNIDrawDepthMap)(bool waitflag);
	void (*OpenNIDepthTexture)(IDirect3DTexture9** ppTex);
	void (*OpenNIGetSkeltonJointPosition)(int num,D3DXVECTOR3* vec);
	void (*OpenNIIsTracking)(bool* lpb);
	void (*OpenNIGetVersion)(float* ver);

	int m_rendercnt;

	CTxtSetting* m_ts;
	CRpsFile* m_rps;

	int m_capmode;
	int m_capmotid;
	int m_capstartframe;
	int m_capendframe;
	int m_capframe;

	int m_timermax;
	int m_curtimer;

	int m_fullscflag;
//	CConvSetDlg* m_dlg;
};

#endif

