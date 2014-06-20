#ifndef NPCLEARDATAH
#define NPCLEARDATAH

class CNaviLine;

class CNPClearData
{
public:
	CNPClearData();
	~CNPClearData();

	int CreateData( int ownerid, CNaviLine* srcnl, int srcroundnum );
	int DestroyData();
	int InitFlag();
	int SetNaviPointClearFlag( D3DXVECTOR3 srcpos, float maxdist, int* pidptr, int* roundptr, float* distptr );

private:
	void InitParams();
	void DestroyObjs();

public:
	int ownerhsid;
	CNaviLine* nlptr;
	int pointnum; // <--- create���ɃZ�b�g����
	int* npno;// leng : pointnum, naviline��pointid�����ԂɊi�[�B
	int roundnum;
	char* clearflag;// leng : pointnum * roundnum, 
	
	int lastround;//�Ō�ɁAE3DSetNaviPointClearFlag�����Ƃ��́A����
	int lastpointarno;//�Ō�ɁAE3DSetNaviPointClearFlag�����Ƃ��́A�N���A�[�|�C���g�̔z��ԍ�
	float lastdist;
};



#endif
