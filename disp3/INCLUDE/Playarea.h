#define		PLAYAREAH	1

#include	<d3drmwin.h>
#ifndef		COEFH
	#include	"Coef.h"
#endif
#ifndef		CHARPOSH
	#include	"CharPos.h"
#endif
#ifndef		MYMOVEFLAGH
	#include	"MyMoveFlag.h"
#endif


#ifndef		CALCH
	#include	"Calc.h"
#endif

class CPlayArea
{
public:
	CPlayArea();
	~CPlayArea();

	void	*operator new( size_t size );
	void	operator delete( void *p );

	//heap
	static HANDLE	s_hHeap;
	static unsigned int	s_uNumAllocsInHeap;

public:
	BOOL		ChkWalk( INDEX_L& index, int chardir ); 


	BOOL		isWalk[10][10]; // [walkflag][chardirindex]


				// movekey が oldkey と同じ motion group のものでないとき movekey
				// 同じ group のもののときは oldmove を 返す
	int			MyMoveChgIfNSame[MYMOVEMAX][MYMOVEMAX]; // [oldmove][movekey]
				
					// キーが 無いときの move の set に使う
				// oldmove が キーがなくても続行するイベントのときは oldmove
				// oldmove が キー有りのときのみ有効なイベントのときは idle  
	int			SetMyIdleIfNEvent[MYMOVEMAX]; // [oldmove]
	
				// pos の 移動を ともなうイベントかどうか を 判定
	BOOL		isMyMoving[MYMOVEMAX];

//EDITPOINT!!!IsMyMoveNameFlag
	BOOL		isMyIDLE[MYMOVEMAX + 1];
	BOOL		isMyWALK[MYMOVEMAX + 1];
	BOOL		isMyJUMP[MYMOVEMAX + 1];
	BOOL		isMyKICK[MYMOVEMAX + 1];
	BOOL		isMyRIDE_C[MYMOVEMAX + 1];
	BOOL		isMyPUSHOBJ[MYMOVEMAX + 1];
	BOOL		isMyPUSHED[MYMOVEMAX + 1];
	BOOL		isMyINTERSECT[MYMOVEMAX + 1];
	BOOL		isMyRIDEONIDLE[MYMOVEMAX + 1];
	BOOL		isMyRIDEON[MYMOVEMAX + 1];
	BOOL		isMyKICKED[MYMOVEMAX + 1];
	BOOL		isMyMPUSHED[MYMOVEMAX + 1];
	BOOL		isMyZROTL[MYMOVEMAX + 1];
	BOOL		isMyZROTR[MYMOVEMAX + 1];
	BOOL		isMyKANIL[MYMOVEMAX + 1];
	BOOL		isMyKANIR[MYMOVEMAX + 1];
	BOOL		isMyDROPHOLE[MYMOVEMAX + 1];
	BOOL		isMyUPFLOOR[MYMOVEMAX + 1];
	BOOL		isMyDOWNFLOOR[MYMOVEMAX + 1];
	BOOL		isMyUMAIDLE[MYMOVEMAX + 1];
	BOOL		isMyUMA[MYMOVEMAX + 1];
	BOOL		isMyPIS[MYMOVEMAX + 1];
	BOOL		isMyFIRE[MYMOVEMAX + 1];
	BOOL		isMyDETH[MYMOVEMAX + 1];
	BOOL		isMyBINDMOVE[MYMOVEMAX + 1];
//ENDEDIT

	BOOL		isKameIdle[KAMEFLAGMAX + 1];
	BOOL		isKameWalk[KAMEFLAGMAX + 1];
	BOOL		isKameWalkIdle[KAMEFLAGMAX + 1];
	BOOL		isKameKick[KAMEFLAGMAX + 1];
	BOOL		isKamePushObj[KAMEFLAGMAX + 1];
	BOOL		isKamePushed[KAMEFLAGMAX + 1];
	BOOL		isKameIntersect[KAMEFLAGMAX + 1];
	BOOL		isKameKicked[KAMEFLAGMAX + 1];
	BOOL		isKameMPushed[KAMEFLAGMAX + 1];
	BOOL		isKameZrotL[KAMEFLAGMAX + 1];
	BOOL		isKameZrotR[KAMEFLAGMAX + 1];
	BOOL		isKameZrot[KAMEFLAGMAX + 1];
	BOOL		isKameKaniL[KAMEFLAGMAX + 1];
	BOOL		isKameKaniR[KAMEFLAGMAX + 1];
	BOOL		isKameKani[KAMEFLAGMAX + 1];
	BOOL		isKameFire[KAMEFLAGMAX + 1];
	BOOL		isKameDeth[KAMEFLAGMAX + 1];

public:
	void		Index2Loc( LOCATION* loc, INDEX_L* index )
	{
		loc->x = (float)(index->x * INDEX2LOC + HALFSTEP);
		loc->y = (float)(index->y * INDEX2LOC + HALFSTEP);
		loc->z = (float)(index->z * INDEX2LOC + HALFSTEP);
	};
	void		Index2Loc( D3DVECTOR* loc, INDEX_L* index )
	{
		loc->x = (float)(index->x * INDEX2LOC + HALFSTEP);
		loc->y = (float)(index->y * INDEX2LOC + HALFSTEP);
		loc->z = (float)(index->z * INDEX2LOC + HALFSTEP);
	};
	void		Index2Loc_Z( float*	locz, int indexz )
	{
		*locz = (float)(indexz * INDEX2LOC + HALFSTEP);
	};

	void		Loc2Index( INDEX_L* index, LOCATION* loc )
	{
		index->x = (int)(loc->x * LOC2INDEX);
		index->y = (int)(loc->y * LOC2INDEX);
		index->z = (int)(loc->z * LOC2INDEX);
	};
	void		Loc2Index( INDEX_L* index, D3DVECTOR* loc )
	{
		index->x = (int)(loc->x * LOC2INDEX);
		index->y = (int)(loc->y * LOC2INDEX);
		index->z = (int)(loc->z * LOC2INDEX);
	};
	void		Loc2Index_X( int* indexx, float locx )
	{
		*indexx = (int)(locx * LOC2INDEX);
	}

	/////////// backg.cpp /// INDEX_L と grandvertの添え字の gindex の変換 ////
	void	GIndex2Index( INDEX_L*	index, INDEX_L&	gindex )
	{
		index->x = gi2i_x[gindex.x + GMAXINDEXX];
		index->z = gi2i_z[gindex.z + GMAXINDEXZ];
		index->y = gi2i_y[gindex.y + GMAXINDEXY];

	};

	void	GIndex2Index( INDEX_L*	index, int	gindexx, int gindexy, int gindexz )
	{
		index->x = gi2i_x[gindexx + GMAXINDEXX];
		index->z = gi2i_z[gindexz + GMAXINDEXZ];
		index->y = gi2i_y[gindexy + GMAXINDEXY];
	};
	void	GIndex2Index_Z( int* index_z, int gindexz )
	{
		*index_z = gi2i_z[gindexz + GMAXINDEXZ];
	}

	void	Index2GIndex( INDEX_L*	gindex, INDEX_L&	index )
	{
		gindex->x = i2gi_x[index.x + MAXINDEXX];
		gindex->z = i2gi_z[index.z + MAXINDEXZ];
		gindex->y = i2gi_y[index.y + MAXINDEXY];

	};
	void	Index2GIndex( INDEX_L*	gindex, int indexx, int indexy, int indexz )
	{
		gindex->x = i2gi_x[indexx + MAXINDEXX];
		gindex->z = i2gi_z[indexz + MAXINDEXZ];
		gindex->y = i2gi_y[indexy + MAXINDEXY];

	};
	void	Index2GIndex_X( int *gindex_x, int index_x )
	{
		*gindex_x = i2gi_x[index_x + MAXINDEXX];
	};
	void	Index2GIndex_Z(	int	*gindex_z, int index_z )
	{
		*gindex_z = i2gi_z[index_z + MAXINDEXZ];
		//return rest_i2gi_z[index_z];
	};
private:
	/***
	int		i2gi_x[MAXINDEXX + 1];
	int		i2gi_y[MAXINDEXY + 1];
	int		i2gi_z[MAXINDEXZ + 1];
	int		gi2i_x[GMAXINDEXX + 1];
	int		gi2i_y[GMAXINDEXY + 1];
	int		gi2i_z[GMAXINDEXZ + 1];
	***/
	int		i2gi_x[2 * MAXINDEXX + 1];
	int		i2gi_y[2 * MAXINDEXY + 1];
	int		i2gi_z[2 * MAXINDEXZ + 1];
	int		gi2i_x[2 * GMAXINDEXX + 1];
	int		gi2i_y[2 * GMAXINDEXY + 1];
	int		gi2i_z[2 * GMAXINDEXZ + 1];

	//int		rest_i2gi_z[MAXINDEXZ + 1];

private:
	void		InitIsMoveFlag();
	void		InitMyMoveChg();
	void		InitSetMyIdle();
	void		InitIsMyMoving();
	void		InitIsWalk();

	void		InitIndex2GIndex();
};