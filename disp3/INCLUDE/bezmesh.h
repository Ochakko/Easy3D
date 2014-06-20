#ifndef BEZMESHH
#define BEZMESHH

#include <vecmesh.h>

class CBezMesh : public CBaseDat
{
public:
	CBezMesh();
	~CBezMesh();


	CVecLine* operator() (int lineno, int kindno ) { 
		CBezLine* curbline = 0;

		#ifdef _DEBUG
			int ret;
			ret = CheckNo( lineno );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		#endif
		//curbline = blineptr + lineno;
		//return (*curbline)( 1, kindno );
		curbline = *( hbline + lineno );
		_ASSERT( curbline );
		//return (*curbline)( 1, kindno );
		return (*curbline)( 0, kindno );
	};

	CBezLine* operator() (int lineno ){
		#ifdef _DEBUG
			int ret;
			ret = CheckNo( lineno );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		#endif
		//return (blineptr + lineno);
		return *( hbline + lineno );
	}


	int CheckNo( int datano )
	{
		if( (datano >= 0) && (datano < meshinfo->n) )
			return 0;
		else
			return 1;
	};


	virtual void	InitParams();
	void	ResetParams();
	int	CreateObjs();
	void	DestroyObjs();
		
	int InitBezMesh( CBezMesh* srcbmesh );
	int InitBezMesh( CMeshInfo* srcmeshinfo, int setleng ); //

	int	CreateBezMesh( CMeshInfo* srcmeshinfo, int setleng );//
	int DestroyBezMesh();

	int SetMem( int* srcint, __int64 setflag );
	int SetMem( int* srcint, int lineno, int kindno, __int64 setflag );
	int SetMem( CBezMesh* srcbmesh, __int64 setflag );
	int SetMem( CBezData* srcbez, int srclineno, int srcdatno, __int64 setflag );
	
	int SetMem( CBezLine* srcbline, int lineno, int blseri, __int64 setflag ); //

	int GetSkipMax();

	int	CopyData( CBezMesh* srcdata );
		
	//meshinfo->m = 1; meshinfo->n = ”CˆÓ;
	virtual int CheckMeshInfo( CMeshInfo* srcmeshinfo ); 
	//eachline->meshinfo->m = ˆê’è; eachline->meshinfo->n = 1;
	virtual int CheckLineInfo( CMeshInfo* lineinfo, int infonum ); 


	int	DumpMem( HANDLE hfile, int tabnum, int dumpflag );

	int SetEvenData( CBezMesh* srcbm, int srcbdivU, int srcbdivV );

	int CalcBezDiv_U( int lineno, int newno, int befno, int aftno );
	int CalcNewpLat( int lineno, int beflno, int aftlno, int pno );
	int CalcBezDiv_V( int pno, int newlno, int beflno, int aftlno );
public:
	CMeshInfo* meshinfo;
	//‚±‚±‚ÌCBezLine‚ÍAŽÀ‘Ô‚Ì‚È‚¢‚½‚¾‚Ìpointer•\ ‚É•ÏX
	//CBezLine*	blineptr; // meshinfo->n ŒÂ‚ÌCBezLine‚Ìpointer
	CBezLine** hbline;
	int* hblseri;

};

#endif