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
	int pointnum; // <--- create時にセットする
	int* npno;// leng : pointnum, navilineのpointidを順番に格納。
	int roundnum;
	char* clearflag;// leng : pointnum * roundnum, 
	
	int lastround;//最後に、E3DSetNaviPointClearFlagしたときの、周回数
	int lastpointarno;//最後に、E3DSetNaviPointClearFlagしたときの、クリアーポイントの配列番号
	float lastdist;
};



#endif
