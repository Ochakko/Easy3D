#ifndef POSEFILEH
#define POSEFILEH

#include <coef.h>
#include <basedat.h>
#include <matrix2.h>
#include "motionpoint2.h"


class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CMotionPoint2;


typedef struct tag_poseata
{
	int serialno;
	char name[256];
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_mv;
	CQuaternion m_q;
		
	int interp;
	D3DXVECTOR3 m_scale;
	int m_userint1;
	D3DXVECTOR3 m_eul;

	int reserved[16];

} POSEDATA;


class CPoseFile
{
public:
	CPoseFile( int srcmode );
	~CPoseFile();

	int SavePoseFile( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, char* filename, 
		int srcmotid, int srcframeno, int* cplist, int createkeyflag );

	int LoadPoseFile( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, char* filename, 
		int srcmotid, int srcframeno );


private:
	int InitParams();
	int DestroyObjs();


	int WritePoseHeader();
	int WritePoseData( POSEDATA* srcpose );
	int WriteMotionPoint( int seri, CMotionPoint2* mpptr );

	int CheckFileVersion();
	int ReadUCharaData( unsigned char* dataptr, int insize );

	int SetPoseData( int srcmotid, int srcframeno, POSEDATA* poseptr );

private:
	CTreeHandler2* m_thandler;
	CShdHandler* m_shandler;
	CMotHandler* m_mhandler;

	HANDLE m_hfile;
	int m_mode;//save -> 0, load -> 1



};




#endif